/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <autoconf.h>
#include <netinet/ether.h>


#include <sel4platsupport/arch/io.h>
#include <sel4utils/vspace.h>
#include <sel4utils/iommu_dma.h>
#include <simple/simple_helpers.h>
#include <vka/capops.h>
#include <utils/util.h>

#include <camkes.h>
#include <camkes/dataport.h>
#include <camkes/virtqueue.h>

#include <ethdrivers/virtio/virtio_pci.h>
#include <ethdrivers/virtio/virtio_net.h>
#include <ethdrivers/virtio/virtio_ring.h>
#include <vswitch.h>

#include "vmm/vmm.h"
#include "vmm/driver/pci_helper.h"
#include "vmm/driver/virtio_emul.h"
#include "vmm/platform/ioports.h"
#include "vmm/platform/guest_vspace.h"

#include "vm.h"
#include "i8259.h"
#include "virtio_net.h"

vswitch_t g_vswitch;

int __attribute__((weak)) eth_rx_ready_reg_callback_vswitch(void (*proc)(void*),void *blah) {
    ZF_LOGE("should not be here");
    return 0;
}


static virtio_net_t *virtio_net = NULL;


static void virtio_net_notify_vswitch_send(vswitch_node_t *node) {
    volatile void *used_buff = NULL;
    size_t used_buff_sz = 0;
    int dequeue_res = virtqueue_driver_dequeue(node->virtqueues.send_queue,
                                                &used_buff,
                                                &used_buff_sz);
    if(dequeue_res) {
        ZF_LOGE("Unable to dequeue used buff");
        return;
    }
    camkes_virtqueue_buffer_free(node->virtqueues.send_queue, used_buff);
}

/*
 * We transmit packets to other VM's through using our virtqueue
 * implementation. The current virtqueue implementation is limited to
 * sending a single packet at a time and hence may drop a large number of
 * packets when sending a multi-packet payloads. Our TX implementation
 * is limited by the following:
 * - Can't send payloads larger than 4K (maximum size of buffer)
 * - Can send only one packet at once
 *   - We yield between packet sends to give the other end a chance to
 *   consume the virtqueue buffer. However if this does not happen we
 *   will ultimately drop the packet.
 * - If we can't send a packet, we will drop the packet and end the
 *   transmission. We currently don't have a mechanism to retry the
 *   transmission.
 */
static int emul_raw_tx(struct eth_driver *driver,
                       unsigned int num, uintptr_t *phys, unsigned int *len,
                       void *cookie)
{
    size_t tot_len = 0;

    (void)tot_len;

    /* Copy to the virtqueue */
    for (int i = 0; i < num; i++) {
        struct ether_addr *destaddr;
        int err, destnode_start_idx, destnode_n_idxs;

        /* Initialize a convenience pointer to the dest macaddr.
         * The dest MAC addr is the first member of an ethernet frame.
         */
        destaddr = (struct ether_addr *)phys[i];

        /* Set up the bounds of the loop below that copies the frames into the
         * destination Guest's virtqueue.
         */
        if (mac802_addr_eq_bcast(destaddr)) {
            /* Send to all nodes on the VLAN if destaddr is bcast addr. */
            destnode_n_idxs = g_vswitch.n_connected;
            destnode_start_idx = 0;
        }
        else {
            /* Send only to the target node */
            destnode_n_idxs = 1;
            destnode_start_idx = vswitch_get_destnode_index_by_macaddr(
                                                            &g_vswitch,
                                                            destaddr);
            if (destnode_start_idx < 0) {
                ZF_LOGE("Unreachable dest macaddr " PR_MAC802_ADDR ". Dropping "
                        "frame.",
                        PR_MAC802_ADDR_ARGS(destaddr));

                /* This function seems to be pretending to send multiple frames
                 * at once, but in reality, it is only ever invoked with the
                 * "num" argment being "1".
                 *
                 * So return error instead of "continue"-ing here.
                 */
                return ETHIF_TX_COMPLETE;
            }
        }

        /* Copy the frame into the virtqueue of each of the targets we decided
         * upon.
         */
        for (int j=destnode_start_idx;
             j<destnode_start_idx + destnode_n_idxs; j++) {
            vswitch_node_t *destnode;
            virtqueue_t *destbuff;

            destnode = vswitch_get_destnode_by_index(&g_vswitch, j);
            if (destnode == NULL) {
                /* This could happen in the broadcast case if there are holes in
                 * the array, though that would still be odd.
                 */
                ZF_LOGW("Found holes in node array while sending to dest MAC "
                        PR_MAC802_ADDR".",
                        PR_MAC802_ADDR_ARGS(destaddr));
                continue;
            }
            volatile void *alloc_buffer = NULL;
            int err = camkes_virtqueue_buffer_alloc(destnode->virtqueues.send_queue, &alloc_buffer, len[i]);
            if (err) {
                ZF_LOGW("Dropping eth frame to dest " PR_MAC802_ADDR ": no buff "
                        "available.",
                        PR_MAC802_ADDR_ARGS(destaddr));

                return ETHIF_TX_COMPLETE;
            };

            memcpy((void *)alloc_buffer, (void *)phys[i], len[i]);

            err = virtqueue_driver_enqueue(destnode->virtqueues.send_queue,
                    alloc_buffer, len[i]);
            if(err != 0) {
                ZF_LOGE("Unknown error while enqueuing available buffer for dest "
                        PR_MAC802_ADDR ".",
                        PR_MAC802_ADDR_ARGS(destaddr));
                camkes_virtqueue_buffer_free(destnode->virtqueues.send_queue, alloc_buffer);
                return ETHIF_TX_COMPLETE;
            }

            err = virtqueue_driver_signal(destnode->virtqueues.send_queue);
            if (err != 0) {
                ZF_LOGE("Unknown error while signaling dest "
                        PR_MAC802_ADDR ".",
                        PR_MAC802_ADDR_ARGS(destaddr));

                return ETHIF_TX_COMPLETE;
            }
            /* This is a minor optimisation. We yield here to give the other end a chance to consume our
             * transmited packet. When we resume at this point we then have the chance to send further
             * packets, avoiding the need to drop them.
             */
            seL4_Yield();
            if(virtqueue_driver_poll(destnode->virtqueues.send_queue) == 1) {
                virtio_net_notify_vswitch_send(destnode);
            }
            tot_len += len[i];
        }
    }

    return ETHIF_TX_COMPLETE;
}


static void emul_low_level_init(struct eth_driver *driver, uint8_t *mac, int *mtu) {
    struct ether_addr res;
    struct ether_addr *resp;
    resp = ether_aton_r (vswitch_mac_address, &res);
    if (resp == NULL) {
        ZF_LOGF("Failed to get MAC address");
    }
    memcpy(mac, res.ether_addr_octet, ETH_ALEN);
    *mtu = 1500;
}


static void get_self_mac_addr(struct ether_addr *self_addr) {
    struct ether_addr * res = ether_aton_r (vswitch_mac_address, self_addr);
    if (res == NULL) {
        ZF_LOGF("Failed to get MAC address");
    }

}

/*
 * We recieve packets from other VM's through using our virtqueue
 * implementation. The current virtqueue implementation is limited to
 * recieving a single packet at a time. Our RX implementation is
 * limited by the following:
 * - Can't recieve payloads larger than 4K (maximum size of buffer)
 * - Can recive only one packet at once
 *   - We yield between packet consumes to give the other end a
 *   chance to send further virtqueue buffers.
 */
static void virtio_net_notify_vswitch_recv(vswitch_node_t *node) {
    int err;
    struct ether_addr myaddr;
    virtqueue_t *rxdata_buff;
    ssize_t rxdata_buff_sz;

     /* First we need to know who we are. Ask the template because it knows. */
    get_self_mac_addr(&myaddr);

    if (node == NULL) {
        ZF_LOGE("Please connect this Guest (" PR_MAC802_ADDR ") to the VSwitch "
                "before trying to use the library.",
                PR_MAC802_ADDR_ARGS(&myaddr));

        return;
    }

    /* The eth frame is already in the virtqueue. It was placed there by
     * the sender's libvswitch instance. Check the virtqueue and pull it out.
     */
    volatile void *available_buff = NULL;
    size_t available_buff_sz = 0;
    int dequeue_res = virtqueue_device_dequeue(node->virtqueues.recv_queue,
                                                &available_buff,
                                                &available_buff_sz);

    while (dequeue_res >= 0) {
        void *cookie, *emul_buf;
        size_t len = available_buff_sz;
        int enqueue_res = 0;
        /* Allocate ring space to put the eth frame into. */
        emul_buf = (void*)virtio_net->emul_driver->i_cb.allocate_rx_buf(
                                            virtio_net->emul_driver->cb_cookie,
                                            available_buff_sz, &cookie);
        if (emul_buf == NULL) {
            ZF_LOGW("Dropping frame for " PR_MAC802_ADDR ": No ring mem avail.",
                    PR_MAC802_ADDR_ARGS(&myaddr));
            enqueue_res = virtqueue_device_enqueue(node->virtqueues.recv_queue, available_buff, available_buff_sz);
            if(enqueue_res) {
                ZF_LOGE("Unable to enqueue frame at " PR_MAC802_ADDR "",
                        PR_MAC802_ADDR_ARGS(&myaddr));
            }
            goto vswitch_recv_signal;
        }

        memcpy(emul_buf, (void*)available_buff, len);

        virtio_net->emul_driver->i_cb.rx_complete(
                                    virtio_net->emul_driver->cb_cookie,
                                    1, &cookie, (unsigned int*)&len);

        enqueue_res = virtqueue_device_enqueue(node->virtqueues.recv_queue, available_buff, available_buff_sz);
        if(enqueue_res) {
            ZF_LOGE("Unable to enqueue frame at " PR_MAC802_ADDR "",
                    PR_MAC802_ADDR_ARGS(&myaddr));
        }
        /* We yield here to give the other end a chance to send more virtqueues for us to consume. This is a minor
         * optimisation to avoid having to re-enter the event loop on our global notification object. */
        seL4_Yield();
        dequeue_res = virtqueue_device_dequeue(node->virtqueues.recv_queue,
                                                &available_buff,
                                                &available_buff_sz);
    }
vswitch_recv_signal:
    err = virtqueue_device_signal(node->virtqueues.recv_queue);
    if(err) {
        ZF_LOGW("Failed to signal on virtqueue meant for Guest "
                PR_MAC802_ADDR ".",
                PR_MAC802_ADDR_ARGS(&myaddr));
    }
}

void virtio_net_notify_vswitch(vmm_t *vmm) {
    int err;
    for(int i=0; i < VSWITCH_NUM_NODES; i++) {
        if(g_vswitch.nodes[i].virtqueues.send_queue != NULL) {
            if(virtqueue_driver_poll(g_vswitch.nodes[i].virtqueues.send_queue) == 1) {
                virtio_net_notify_vswitch_send(&g_vswitch.nodes[i]);
            }
        }
        if(g_vswitch.nodes[i].virtqueues.recv_queue != NULL) {
            if(virtqueue_device_poll(g_vswitch.nodes[i].virtqueues.recv_queue) == 1) {
                virtio_net_notify_vswitch_recv(&g_vswitch.nodes[i]);
            }
        }
    }
}

static int make_vswitch_net(void) {
    int err;
    err = vswitch_init(&g_vswitch);
    if(err) {
        ZF_LOGE("Unable to initialise vswitch library");
        return -1;
    }
    int num_vswitch_entries = sizeof(vswitch_layout)/sizeof(struct vswitch_mapping);
    for(int i = 0; i < num_vswitch_entries; i++) {
        struct vswitch_mapping mac_mapping = vswitch_layout[i];
        virtqueue_driver_t *send_virtqueue;
        virtqueue_device_t *recv_virtqueue;
        err = camkes_virtqueue_driver_init(&send_virtqueue, mac_mapping.send_id);
        if(err) {
            ZF_LOGE("Unable to initialise send virtqueue for %s", mac_mapping.mac_addr);
            continue;
        }
        err = camkes_virtqueue_device_init(&recv_virtqueue, mac_mapping.recv_id);
        if(err) {
            ZF_LOGE("Unable to initialise recv virtqueue for %s", mac_mapping.mac_addr);
            camkes_virtqueue_driver_free(send_virtqueue);
            continue;
        }
        struct ether_addr guest_macaddr;
        struct ether_addr *res = ether_aton_r (mac_mapping.mac_addr, &guest_macaddr);
        if (res == NULL) {
            ZF_LOGF("Failed to get MAC address");
        }

        err = vswitch_connect(&g_vswitch, &guest_macaddr, send_virtqueue, recv_virtqueue);
        if(err) {
            ZF_LOGE("Unable to initialise vswitch for MAC address: %s", mac_mapping.mac_addr);
            camkes_virtqueue_driver_free(send_virtqueue);
            camkes_virtqueue_device_free(recv_virtqueue);
        }
    }
}

void make_virtio_net_vswitch(vmm_t *vmm) {
    struct raw_iface_funcs backend = virtio_net_default_backend();
    backend.raw_tx = emul_raw_tx;
    backend.low_level_init = emul_low_level_init;

    make_vswitch_net();
    virtio_net = common_make_virtio_net(vmm, 0x9000, backend);
    assert(virtio_net);
}

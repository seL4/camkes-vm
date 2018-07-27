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

#include <sel4platsupport/arch/io.h>
#include <sel4utils/vspace.h>
#include <sel4utils/iommu_dma.h>
#include <simple/simple_helpers.h>
#include <vka/capops.h>
#include <utils/util.h>

#include <camkes.h>
#include <camkes/dataport.h>
#include <camkes/buffqueue.h>

#include <ethdrivers/virtio/virtio_pci.h>
#include <ethdrivers/virtio/virtio_net.h>
#include <ethdrivers/virtio/virtio_ring.h>
#include <ethdrivers/sel4vswitch.h>

#include "vmm/vmm.h"
#include "vmm/driver/pci_helper.h"
#include "vmm/driver/virtio_emul.h"
#include "vmm/platform/ioports.h"
#include "vmm/platform/guest_vspace.h"

#include "vm.h"
#include "i8259.h"

#define VIRTIO_VID 0x1af4
#define VIRTIO_DID_START 0x1000

#define QUEUE_SIZE 128

volatile Buf*__attribute__((weak)) ethdriver_buf;

void __attribute__((weak)) ethdriver_mac_vswitch(uint8_t *b1, uint8_t *b2, uint8_t *b3, uint8_t *b4, uint8_t *b5, uint8_t *b6) {
    *b1 = (uint8_t)mac_address[0]; *b2 = (uint8_t)mac_address[1]; *b3 = (uint8_t)mac_address[2];
    *b4 = (uint8_t)mac_address[3]; *b5 = (uint8_t)mac_address[4]; *b6 = (uint8_t)mac_address[5];
}

int __attribute__((weak)) eth_rx_ready_reg_callback_vswitch(void (*proc)(void*),void *blah) {
    ZF_LOGE("should not be here");
    return 0;
}

typedef struct virtio_net {
    unsigned int iobase;
    ethif_virtio_emul_t *emul;
    struct eth_driver *emul_driver;
    ps_io_ops_t ioops;
} virtio_net_t;

static virtio_net_t *virtio_net = NULL;

static int virtio_net_io_in(void *cookie, unsigned int port_no, unsigned int size, unsigned int *result) {
    virtio_net_t *net = (virtio_net_t*)cookie;
    unsigned int offset = port_no - net->iobase;
    return net->emul->io_in(net->emul, offset, size, result);
}

static int virtio_net_io_out(void *cookie, unsigned int port_no, unsigned int size, unsigned int value) {
    int ret;
    virtio_net_t *net = (virtio_net_t*)cookie;
    unsigned int offset = port_no - net->iobase;
    ret = net->emul->io_out(net->emul, offset, size, value);
    return ret;
}

static int emul_raw_tx(struct eth_driver *driver,
                       unsigned int num, uintptr_t *phys, unsigned int *len,
                       void *cookie)
{
    sel4vswitch_t *g_vswitch;
    size_t tot_len = 0;

    (void)tot_len;

    g_vswitch = vmm_sel4vswitch_get_global_inst();
    assert(g_vswitch != NULL);

    /* Copy to the buffqueue */
    for (int i = 0; i < num; i++) {
        sel4vswitch_mac802_addr_t *destaddr;
        int err, destnode_start_idx, destnode_n_idxs;

        /* Initialize a convenience pointer to the dest macaddr.
         * The dest MAC addr is the first member of an ethernet frame.
         */
        destaddr = (sel4vswitch_mac802_addr_t *)phys[i];

        /* Set up the bounds of the loop below that copies the frames into the
         * destination Guest's buffqueue.
         */
        if (mac802_addr_eq_bcast(destaddr)) {
            /* Send to all nodes on the VLAN if destaddr is bcast addr. */
            destnode_n_idxs = g_vswitch->n_connected;
            destnode_start_idx = 0;
        }
        else {
            /* Send only to the target node */
            destnode_n_idxs = 1;
            destnode_start_idx = sel4vswitch_get_destnode_index_by_macaddr(
                                                            g_vswitch,
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

        /* Copy the frame into the buffqueue of each of the targets we decided
         * upon.
         */
        for (int j=destnode_start_idx;
             j<destnode_start_idx + destnode_n_idxs; j++) {
            sel4vswitch_node_t *destnode;
            buffqueue_t *destbuff;

            destnode = sel4vswitch_get_destnode_by_index(g_vswitch, j);
            if (destnode == NULL) {
                /* This could happen in the broadcast case if there are holes in
                 * the array, though that would still be odd.
                 */
                ZF_LOGW("Found holes in node array while sending to dest MAC "
                        PR_MAC802_ADDR".",
                        PR_MAC802_ADDR_ARGS(destaddr));
                continue;
            }
            void *alloc_buffer = NULL;
            int err = alloc_camkes_buffqueue_buffer(destnode->buffqueues.send_queue, &alloc_buffer, len[i]);
            if (err) {
                ZF_LOGW("Dropping eth frame to dest " PR_MAC802_ADDR ": no buff "
                        "available.",
                        PR_MAC802_ADDR_ARGS(destaddr));

                return ETHIF_TX_COMPLETE;
            };

            memcpy(alloc_buffer, (void *)phys[i], len[i]);

            err = buffqueue_enqueue_available_buff(destnode->buffqueues.send_queue,
                    alloc_buffer, len[i]);
            if(err != 0) {
                ZF_LOGE("Unknown error while enqueuing available buffer for dest "
                        PR_MAC802_ADDR ".",
                        PR_MAC802_ADDR_ARGS(destaddr));
                return ETHIF_TX_COMPLETE;
            }

            err = buffqueue_signal(destnode->buffqueues.send_queue);
            if (err != 0) {
                ZF_LOGE("Unknown error while signaling dest "
                        PR_MAC802_ADDR ".",
                        PR_MAC802_ADDR_ARGS(destaddr));

                return ETHIF_TX_COMPLETE;
            }

            tot_len += len[i];
        }
    }

    return ETHIF_TX_COMPLETE;
}

static void emul_raw_handle_irq(struct eth_driver *driver, int irq) {
    i8259_gen_irq(6);
}

static void emul_raw_poll(struct eth_driver *driver) {
    ZF_LOGE("not implemented");
}

static void emul_low_level_init(struct eth_driver *driver, uint8_t *mac, int *mtu) {
    ethdriver_mac_vswitch(&mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
    *mtu = 1500;
}

static void emul_print_state(struct eth_driver* driver) {
    ZF_LOGF("not implemented");
}

static struct raw_iface_funcs emul_driver_funcs = {
    .raw_tx = emul_raw_tx,
    .raw_handleIRQ = emul_raw_handle_irq,
    .raw_poll = emul_raw_poll,
    .print_state = emul_print_state,
    .low_level_init = emul_low_level_init
};

static int emul_driver_init(struct eth_driver *driver, ps_io_ops_t io_ops, void *config) {
    virtio_net_t *net = (virtio_net_t*)config;
    driver->eth_data = config;
    driver->dma_alignment = sizeof(uintptr_t);
    driver->i_fn = emul_driver_funcs;
    net->emul_driver = driver;
    return 0;
}

static void get_self_mac_addr(sel4vswitch_mac802_addr_t *self_addr) {
    for(int i = 0; i < 6; i++) {
            self_addr->bytes[i] = (uint8_t)mac_address[i];
    }
}


static void virtio_net_notify_vswitch_send(sel4vswitch_node_t *node) {
    void *used_buff = NULL;
    size_t used_buff_sz = 0;
    int dequeue_res = buffqueue_dequeue_used_buff(node->buffqueues.send_queue,
                                                &used_buff,
                                                &used_buff_sz);
    if(dequeue_res) {
        ZF_LOGE("Unable to dequeue used buff");
        return;
    }
    free_camkes_buffqueue_buffer(node->buffqueues.send_queue, used_buff);
}

static void virtio_net_notify_vswitch_recv(sel4vswitch_node_t *node) {
    int err;
    sel4vswitch_t *g_vswitch;
    sel4vswitch_mac802_addr_t myaddr;
    buffqueue_t *rxdata_buff;
    ssize_t rxdata_buff_sz;

     /* First we need to know who we are. Ask the template because it knows. */
    get_self_mac_addr(&myaddr);

    if (node == NULL) {
        ZF_LOGE("Please connect this Guest (" PR_MAC802_ADDR ") to the VSwitch "
                "before trying to use the library.",
                PR_MAC802_ADDR_ARGS(&myaddr));

        return;
    }

    /* The eth frame is already in the buffqueue. It was placed there by
     * the sender's libvswitch instance. Check the buffqueue and pull it out.
     */
    void *available_buff = NULL;
    size_t available_buff_sz = 0;
    int enqueue_res = 0;
    int dequeue_res = buffqueue_dequeue_available_buff(node->buffqueues.recv_queue,
                                                &available_buff,
                                                &available_buff_sz);

    while (dequeue_res >= 0 && enqueue_res >= 0) {
        void *cookie, *emul_buf;
        size_t len = available_buff_sz;

        /* Allocate ring space to put the eth frame into. */
        emul_buf = (void*)virtio_net->emul_driver->i_cb.allocate_rx_buf(
                                            virtio_net->emul_driver->cb_cookie,
                                            available_buff_sz, &cookie);
        if (emul_buf == NULL) {
            ZF_LOGW("Dropping frame for " PR_MAC802_ADDR ": No ring mem avail.",
                    PR_MAC802_ADDR_ARGS(&myaddr));
            return;
        }

        memcpy(emul_buf, (void*)available_buff, len);

        virtio_net->emul_driver->i_cb.rx_complete(
                                    virtio_net->emul_driver->cb_cookie,
                                    1, &cookie, (unsigned int*)&len);

        enqueue_res = buffqueue_enqueue_used_buff(node->buffqueues.recv_queue, available_buff, available_buff_sz);

        dequeue_res = buffqueue_dequeue_available_buff(node->buffqueues.recv_queue,
                                                &available_buff,
                                                &available_buff_sz);
    }
    err = buffqueue_signal(node->buffqueues.recv_queue);
    if(err) {
        ZF_LOGW("Failed to signal on buffqueue meant for Guest "
                PR_MAC802_ADDR ".",
                PR_MAC802_ADDR_ARGS(&myaddr));
    }
}

void virtio_net_notify_vswitch(vmm_t *vmm) {
    int err;
    sel4vswitch_t *g_vswitch;
    g_vswitch = vmm_sel4vswitch_get_global_inst();
    assert(g_vswitch != NULL);
    for(int i=0; i < CONFIG_SEL4VSWITCH_NUM_NODES; i++) {
        if(g_vswitch->nodes[i].buffqueues.send_queue != NULL) {
            if(buffqueue_poll(g_vswitch->nodes[i].buffqueues.send_queue) == 1) {
                virtio_net_notify_vswitch_send(&g_vswitch->nodes[i]);
            }
        }
        if(g_vswitch->nodes[i].buffqueues.recv_queue != NULL) {
            if(buffqueue_poll(g_vswitch->nodes[i].buffqueues.recv_queue) == 1) {
                virtio_net_notify_vswitch_recv(&g_vswitch->nodes[i]);
            }
        }
    }
}

static void* malloc_dma_alloc(void *cookie, size_t size, int align, int cached, ps_mem_flags_t flags) {
    assert(cached);
    int error;
    void *ret;
    error = posix_memalign(&ret, align, size);
    if (error) {
        return NULL;
    }
    return ret;
}

static void malloc_dma_free(void *cookie, void *addr, size_t size){
    free(addr);
}

static uintptr_t malloc_dma_pin(void *cookie, void *addr, size_t size) {
    return (uintptr_t)addr;
}

static void malloc_dma_unpin(void *cookie, void *addr, size_t size) {
}

static void malloc_dma_cache_op(void *cookie, void *addr, size_t size, dma_cache_op_t op) {
}

static int make_vswitch_net(void) {
    int err;

    sel4vswitch_t *vswitch_lib = vmm_sel4vswitch_get_global_inst();
    int num_vswitch_entries = sizeof(vswitch_layout)/sizeof(struct mapping);
    for(int i = 0; i < num_vswitch_entries; i++) {
        struct mapping mac_mapping = vswitch_layout[i];
        buffqueue_t *send_buffqueue;
        buffqueue_t *recv_buffqueue;
        err = init_camkes_buffqueue(&send_buffqueue, mac_mapping.send_id);
        if(err) {
            ZF_LOGE("Unable to initialise send buffqueue for %x:%x:%x:%x:%x:%x",
                    mac_mapping.mac_addr[0],mac_mapping.mac_addr[1],mac_mapping.mac_addr[2],
                    mac_mapping.mac_addr[3],mac_mapping.mac_addr[4],mac_mapping.mac_addr[5]);
            continue;
        }
        err = init_camkes_buffqueue(&recv_buffqueue, mac_mapping.recv_id);
        if(err) {
            ZF_LOGE("Unable to initialise recv buffqueue for %x:%x:%x:%x:%x:%x",
                    mac_mapping.mac_addr[0],mac_mapping.mac_addr[1],mac_mapping.mac_addr[2],
                    mac_mapping.mac_addr[3],mac_mapping.mac_addr[4],mac_mapping.mac_addr[5]);
            buffqueue_free(send_buffqueue);
            continue;
        }
        sel4vswitch_mac802_addr_t guest_macaddr;
        for(int i = 0; i < 6; i++) {
            guest_macaddr.bytes[i] = (uint8_t)mac_mapping.mac_addr[i];
        }
        err = seL4vswitch_connect(vswitch_lib, &guest_macaddr, send_buffqueue, recv_buffqueue);
        if(err) {
            ZF_LOGE("Unable to initialise sel4vswitch for MAC address: %x:%x:%x:%x:%x:%x",
                    mac_mapping.mac_addr[0],mac_mapping.mac_addr[1],mac_mapping.mac_addr[2],
                    mac_mapping.mac_addr[3],mac_mapping.mac_addr[4],mac_mapping.mac_addr[5]);
            buffqueue_free(send_buffqueue);
            buffqueue_free(recv_buffqueue);
        }
    }
}

void make_virtio_net_vswitch(vmm_t *vmm) {
    vmm_pci_device_def_t *pci_config = malloc(sizeof(*pci_config));
    assert(pci_config);
    memset(pci_config, 0, sizeof(*pci_config));
    *pci_config = (vmm_pci_device_def_t) {
        .vendor_id = VIRTIO_VID,
        .device_id = VIRTIO_DID_START,
        .cache_line_size = 64,
        .latency_timer = 64,
        .subsystem_id = 1,
        .interrupt_pin = 6,
        .interrupt_line = 6
    };
    vmm_pci_entry_t entry = (vmm_pci_entry_t) {
        .cookie = pci_config,
        .ioread = vmm_pci_mem_device_read,
        .iowrite = vmm_pci_entry_ignore_write
    };
    vmm_pci_bar_t bars[1] = {{
        .ismem = 0,
        .address = 0x9000,
        .size_bits = 6
    }};
    entry = vmm_pci_create_bar_emulation(entry, 1, bars);
    vmm_pci_add_entry(&vmm->pci, entry, NULL);
    virtio_net_t *net = malloc(sizeof(*net));
    virtio_net = net;
    assert(net);
    memset(net, 0, sizeof(*net));
    net->iobase = 0x9000;
    vmm_io_port_add_handler(&vmm->io_port, 0x9000, 0x9000 + MASK(6), net, virtio_net_io_in, virtio_net_io_out, "VIRTIO PCI NET");
    ps_io_ops_t ioops;
    ioops.dma_manager = (ps_dma_man_t) {
        .cookie = NULL,
        .dma_alloc_fn = malloc_dma_alloc,
        .dma_free_fn = malloc_dma_free,
        .dma_pin_fn = malloc_dma_pin,
        .dma_unpin_fn = malloc_dma_unpin,
        .dma_cache_op_fn = malloc_dma_cache_op
    };
    make_vswitch_net();
    net->emul = ethif_virtio_emul_init(ioops, QUEUE_SIZE, &vmm->guest_mem.vspace, emul_driver_init, net);
    assert(net->emul);
}

/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
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

#include <vswitch.h>
#include <virtqueue.h>

#include <sel4vm/guest_vm.h>
#include <sel4vm/guest_memory.h>
#include <sel4vm/guest_irq_controller.h>
#include <sel4vm/boot.h>

#include <sel4vmmplatsupport/drivers/pci.h>
#include <sel4vmmplatsupport/drivers/virtio_pci_emul.h>
#include <sel4vmmplatsupport/drivers/virtio_net.h>
#include <sel4vmmplatsupport/arch/ioport_defs.h>

#include "vm.h"
#include "virtio_net.h"
#include "virtio_irq.h"

static vswitch_t g_vswitch;
static virtio_net_t *virtio_net = NULL;

static vm_t *emul_vm;

static void virtio_net_notify_vswitch_send(vswitch_node_t *node)
{
    void *used_buff = NULL;
    unsigned used_buff_sz = 0, len;
    virtqueue_ring_object_t handle;
    vq_flags_t flags;

    while (virtqueue_get_used_buf(node->virtqueues.send_queue, &handle, &len)) {
        while (camkes_virtqueue_driver_gather_buffer(node->virtqueues.send_queue,
                                                     &handle, &used_buff, &used_buff_sz, &flags) == 0) {
            camkes_virtqueue_buffer_free(node->virtqueues.send_queue, used_buff);
        }
    }
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
        if (mac802_addr_eq_bcast(destaddr) || mac802_addr_eq_ipv6_mcast(destaddr)) {
            /* Send to all nodes on the VLAN if destaddr is bcast addr. */
            destnode_n_idxs = g_vswitch.n_connected;
            destnode_start_idx = 0;
        } else {
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
        for (int j = destnode_start_idx; j < destnode_start_idx + destnode_n_idxs; j++) {
            vswitch_node_t *destnode;

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

            if (camkes_virtqueue_driver_scatter_send_buffer(destnode->virtqueues.send_queue, (void *)phys[i], len[i]) < 0) {
                ZF_LOGE("Unknown error while enqueuing available buffer for dest "
                        PR_MAC802_ADDR ".",
                        PR_MAC802_ADDR_ARGS(destaddr));
                return ETHIF_TX_COMPLETE;
            }

            destnode->virtqueues.send_queue->notify();
        }
    }

    return ETHIF_TX_COMPLETE;
}

static void emul_low_level_init(struct eth_driver *driver, uint8_t *mac, int *mtu)
{
    struct ether_addr res;
    struct ether_addr *resp;
    resp = ether_aton_r(vswitch_mac_address, &res);
    if (resp == NULL) {
        ZF_LOGF("Failed to get MAC address");
    }
    memcpy(mac, res.ether_addr_octet, ETH_ALEN);
    *mtu = 1500;
}


static void get_self_mac_addr(struct ether_addr *self_addr)
{
    struct ether_addr *res = ether_aton_r(vswitch_mac_address, self_addr);
    if (res == NULL) {
        ZF_LOGF("Failed to get MAC address");
    }

}

/*
 * We recieve packets from other VM's through using our virtqueue
 * implementation.
 */
static void virtio_net_notify_vswitch_recv(vswitch_node_t *node)
{
    int err;
    struct ether_addr myaddr;
    ssize_t rxdata_buff_sz;
    vq_flags_t flags;

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
    virtqueue_ring_object_t handle;

    while (virtqueue_get_available_buf(node->virtqueues.recv_queue, &handle)) {
        void *cookie, *emul_buf;
        size_t len = virtqueue_scattered_available_size(node->virtqueues.recv_queue, &handle);
        int enqueue_res = 0;

        /* Allocate ring space to put the eth frame into. */
        emul_buf = (void *)virtio_net->emul_driver->i_cb.allocate_rx_buf(virtio_net->emul_driver->cb_cookie,
                                                                         len, &cookie);
        if (emul_buf == NULL) {
            ZF_LOGW("Dropping frame for " PR_MAC802_ADDR ": No ring mem avail.",
                    PR_MAC802_ADDR_ARGS(&myaddr));
            break;
        }
        if (camkes_virtqueue_device_gather_copy_buffer(node->virtqueues.recv_queue, &handle, emul_buf, len) < 0) {
            ZF_LOGW("Dropping frame for " PR_MAC802_ADDR ": Can't gather vq buffer.",
                    PR_MAC802_ADDR_ARGS(&myaddr));
            break;
        }

        virtio_net->emul_driver->i_cb.rx_complete(virtio_net->emul_driver->cb_cookie, 1,
                                                  &cookie, (unsigned int *)&len);
    }

    node->virtqueues.recv_queue->notify();
}

void virtio_net_notify_vswitch(vm_t *vm)
{
    for (int i = 0; i < VSWITCH_NUM_NODES; i++) {
        if (g_vswitch.nodes[i].virtqueues.send_queue && VQ_DRV_POLL(g_vswitch.nodes[i].virtqueues.send_queue)) {
            virtio_net_notify_vswitch_send(&g_vswitch.nodes[i]);
        }
        if (g_vswitch.nodes[i].virtqueues.recv_queue && VQ_DEV_POLL(g_vswitch.nodes[i].virtqueues.recv_queue)) {
            virtio_net_notify_vswitch_recv(&g_vswitch.nodes[i]);
        }
    }
}

static int make_vswitch_net(void)
{
    int err;
    err = vswitch_init(&g_vswitch);
    if (err) {
        ZF_LOGE("Unable to initialise vswitch library");
        return -1;
    }
    int num_vswitch_entries = ARRAY_SIZE(vswitch_layout);
    for (int i = 0; i < num_vswitch_entries; i++) {
        struct vswitch_mapping mac_mapping = vswitch_layout[i];
        struct ether_addr guest_macaddr;
        struct ether_addr *res = ether_aton_r(mac_mapping.mac_addr, &guest_macaddr);
        virtqueue_driver_t *vq_send;
        virtqueue_device_t *vq_recv;

        if (!((vq_send = malloc(sizeof(*vq_send))) && (vq_recv = malloc(sizeof(*vq_recv))))) {
            ZF_LOGE("Unable to malloc memory for virtqueues");
            return -1;
        }
        if (camkes_virtqueue_driver_init(vq_send, mac_mapping.send_id) < 0 ||
            camkes_virtqueue_device_init(vq_recv, mac_mapping.recv_id)) {
            ZF_LOGE("Unable to init virtqueues");
            return -1;
        }

        err = vswitch_connect(&g_vswitch, &guest_macaddr, vq_send, vq_recv);
        if (err) {
            ZF_LOGE("Unable to initialise vswitch for MAC address: %s", mac_mapping.mac_addr);
        }
    }
}

static void emul_raw_handle_irq(struct eth_driver *driver, int irq)
{
    vm_inject_irq(emul_vm->vcpus[BOOT_VCPU], VIRTIO_NET_IRQ);
}

void make_virtio_net_vswitch(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports)
{
    struct raw_iface_funcs backend = virtio_net_default_backend();
    backend.raw_tx = emul_raw_tx;
    backend.low_level_init = emul_low_level_init;
    backend.raw_handleIRQ = emul_raw_handle_irq;

    emul_vm = vm;

    make_vswitch_net();

    ioport_range_t virtio_port_range = {0, 0, VIRTIO_IOPORT_SIZE};
    virtio_net = common_make_virtio_net(vm, pci, io_ports, virtio_port_range, IOPORT_FREE, VIRTIO_NET_IRQ, VIRTIO_NET_IRQ,
                                        backend);
    assert(virtio_net);
}

/* Dummy function used to register irq callback handlers. */
void make_virtio_net_vswitch_driver_dummy(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports)
{

}

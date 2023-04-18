/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <autoconf.h>
#include <arm_vm/gen_config.h>
#include <vmlinux.h>
#include <netinet/ether.h>

#include <camkes.h>

#include <virtqueue.h>
#include <vswitch.h>
#include <camkes/virtqueue.h>
#include <virtioarm/virtio_net.h>

static virtio_net_t *virtio_net = NULL;
static vswitch_t virtio_vswitch;

extern vmm_pci_space_t *pci;
extern vmm_io_port_list_t *io_ports;

void self_mac(uint8_t *mac)
{
    struct ether_addr res;
    struct ether_addr *resp;
    resp = ether_aton_r(vswitch_mac_address, &res);
    if (resp == NULL) {
        ZF_LOGF("Failed to get MAC address");
    }
    memcpy(mac, res.ether_addr_octet, ETH_ALEN);
}

static int tx_virtqueue_forward(char *eth_buffer, size_t length, virtio_net_t *virtio_net)
{
    struct ether_addr *destaddr;
    int err, destnode_start_idx, destnode_n_idxs;

    /* The dest MAC addr is the first member of an ethernet frame. */
    destaddr = (struct ether_addr *)eth_buffer;

    destnode_n_idxs = virtio_vswitch.n_connected;
    destnode_start_idx = 0;
    for (int i = destnode_start_idx; i < destnode_start_idx + destnode_n_idxs; i++) {
        vswitch_node_t *destnode;
        destnode = vswitch_get_destnode_by_index(&virtio_vswitch, i);
        if (destnode == NULL) {
            /* This could happen in the broadcast case if there are holes in
             * the array, though that would still be odd.
             */
            continue;
        }

        if (camkes_virtqueue_driver_scatter_send_buffer(destnode->virtqueues.send_queue, (void *)eth_buffer, length) < 0) {
            ZF_LOGE("Unknown error while enqueuing available buffer for dest "
                    PR_MAC802_ADDR ".",
                    PR_MAC802_ADDR_ARGS(destaddr));
            continue;
        }
        destnode->virtqueues.send_queue->notify();
    }
    return 0;
}

static void virtio_net_notify_free_send(vswitch_node_t *node)
{
    void *buf = NULL;
    unsigned int buf_size = 0;
    uint32_t wr_len = 0;
    vq_flags_t flag;
    virtqueue_ring_object_t handle;
    while (virtqueue_get_used_buf(node->virtqueues.send_queue, &handle, &wr_len)) {
        while (camkes_virtqueue_driver_gather_buffer(node->virtqueues.send_queue, &handle, &buf, &buf_size, &flag) >= 0) {
            /* Clean up and free the buffer we allocated */
            camkes_virtqueue_buffer_free(node->virtqueues.send_queue, buf);
        }
    }
}

static int virtio_net_notify_recv(vswitch_node_t *node)
{
    int err;
    struct ether_addr myaddr;
    void *buf = NULL;
    size_t buf_size = 0;
    vq_flags_t flag;
    virtqueue_ring_object_t handle;

    while (virtqueue_get_available_buf(node->virtqueues.recv_queue, &handle)) {
        char emul_buf[MAX_MTU] = {0};
        size_t len = virtqueue_scattered_available_size(node->virtqueues.recv_queue, &handle);
        int enqueue_res = 0;
        if (camkes_virtqueue_device_gather_copy_buffer(node->virtqueues.recv_queue, &handle, (void *)emul_buf, len) < 0) {
            ZF_LOGW("Dropping frame for " PR_MAC802_ADDR ": Can't gather vq buffer.",
                    PR_MAC802_ADDR_ARGS(&myaddr));
            continue;
        }

        int err = virtio_net_rx(emul_buf, len, virtio_net);
        if (err) {
            ZF_LOGE("Unable to forward recieved buffer to the guest");
        }
        node->virtqueues.recv_queue->notify();
    }
}

static int virtio_net_notify(vm_t *vmm, void *cookie)
{
    for (int i = 0; i < VSWITCH_NUM_NODES; i++) {
        if (virtio_vswitch.nodes[i].virtqueues.recv_queue && VQ_DEV_POLL(virtio_vswitch.nodes[i].virtqueues.recv_queue)) {
            virtio_net_notify_recv(&virtio_vswitch.nodes[i]);
        }
        if (virtio_vswitch.nodes[i].virtqueues.send_queue && VQ_DRV_POLL(virtio_vswitch.nodes[i].virtqueues.send_queue)) {
            virtio_net_notify_free_send(&virtio_vswitch.nodes[i]);
        }
    }
    return 0;
}

void make_virtqueue_virtio_net(vm_t *vm, void *cookie)
{
    int err;
    virtio_net_callbacks_t callbacks;
    callbacks.tx_callback = tx_virtqueue_forward;
    callbacks.irq_callback = NULL;
    callbacks.get_mac_addr_callback = self_mac;
    virtio_net = virtio_net_init(vm, &callbacks, pci, io_ports);

    err = vswitch_init(&virtio_vswitch);
    if (err) {
        ZF_LOGF("Unable to initialise vswitch library");
    }

    int num_vswitch_entries = ARRAY_SIZE(vswitch_layout);
    for (int i = 0; i < num_vswitch_entries; i++) {
        struct vswitch_mapping mac_mapping = vswitch_layout[i];
        struct ether_addr guest_macaddr;
        struct ether_addr *res = ether_aton_r(mac_mapping.mac_addr, &guest_macaddr);
        virtqueue_driver_t *vq_send;
        virtqueue_device_t *vq_recv;
        seL4_CPtr recv_notif;
        seL4_CPtr recv_badge;
        vq_recv = malloc(sizeof(*vq_recv));
        if (!vq_recv) {
            ZF_LOGF("Unable to initialise recv virtqueue for MAC address: %s", mac_mapping.mac_addr);
        }
        vq_send = malloc(sizeof(*vq_send));
        if (!vq_send) {
            ZF_LOGF("Unable to initialise send virtqueue for MAC address: %s", mac_mapping.mac_addr);
        }

        /* Initialise recv virtqueue */
        err = camkes_virtqueue_device_init_with_recv(vq_recv, mac_mapping.recv_id, &recv_notif, &recv_badge);
        ZF_LOGF_IF(err, "Unable to initialise recv virtqueue");
        err = register_async_event_handler(recv_badge, virtio_net_notify, NULL);
        ZF_LOGF_IF(err, "Failed to register_async_event_handler for recv channel on MAC address: %s", mac_mapping.mac_addr);

        /* Initialise send virtqueue */
        err = camkes_virtqueue_driver_init_with_recv(vq_send, mac_mapping.send_id, &recv_notif, &recv_badge);
        ZF_LOGF_IF(err, "Unable to initialise send virtqueue");
        err = register_async_event_handler(recv_badge, virtio_net_notify, NULL);
        ZF_LOGF_IF(err, "Failed to register_async_event_handler for send channel on MAC address: %s", mac_mapping.mac_addr);

        err = vswitch_connect(&virtio_vswitch, &guest_macaddr, vq_send, vq_recv);
        if (err) {
            ZF_LOGF("Unable to initialise vswitch for MAC address: %s", mac_mapping.mac_addr);
        }
    }
}

DEFINE_MODULE(virtio_net, NULL, make_virtqueue_virtio_net)

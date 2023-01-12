/*
 * Copyright 2022, UNSW (ABN 57 195 873 179)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/**
 * @brief virtio vsock backend layer for x86
 *
 * The virtio vsock backend has two layers, the emul layer and the backend layer.
 * This file is part of the backend layer, it's responsible for:
 *
 * - TX: receiving data from the emul layer and forwarding them to the destination
 *   VM using camkes virtqueue (which is different from the virtqueue that's
 *   used to communicate between the guest and the emul layer)
 * - RX: receiving data from the source port via camkes virtqueue and invoking the
 *   emul layer to handle the data
 * - virtio vsock backend initialization
 * - sending virtual IRQ to the guest
 *
 * Data flow:
 *
 *          vq                      camkes vq                      vq
 *  src VM <--> (emul <-> backend) <---------> (backend <-> emul) <--> dest VM
 *                  src VMM                          dest VMM
 *
 * This file relies on the connection topology and camkes virtqueue connection
 * being configured correctly in camkes.
 */

#include <stdint.h>
#include <string.h>
#include <autoconf.h>

#include <utils/util.h>

#include <camkes.h>
#include <camkes/dataport.h>

#include <sel4vm/guest_irq_controller.h>
#include <sel4vm/boot.h>

#include <sel4vmmplatsupport/drivers/virtio_vsock.h>

#include <virtqueue.h>
#include <camkes/virtqueue.h>

#include <virtio/virtio_vsock.h>

#include "vm.h"
#include "virtio_vsock.h"
#include "virtio_irq.h"

static virtio_vsock_t *virtio_vsock;

/* Used as the parameter for callback functions (currently only `vsock_inject_irq`) */
typedef struct virtio_vsock_cookie {
    vm_t *vm;
} virtio_vsock_cookie_t;

/* Configured in camkes */
#define NUM_PORTS ARRAY_SIZE(socket_layout)

/*
 * Represents the virtqueues used in a given link between
 * two VMs.
 */
typedef struct conn {
    int cid;
    virtqueue_driver_t *send_queue;
    virtqueue_device_t *recv_queue;
} conn_t;
static conn_t connections[NUM_PORTS];

/* Temporary buffer for copying data from camkes virtqueue scatter-gather buffer */
static char temp_buf[VIRTIO_VSOCK_CAMKES_MTU];

/**
 * Cleans up and frees camkes buffers we allocated.
 *
 * @param queue Send queue
 */
static void virtio_vsock_notify_free_send(virtqueue_driver_t *queue)
{
    void *buf = NULL;
    uint32_t buf_size = 0, wr_len = 0;
    vq_flags_t flag;
    virtqueue_ring_object_t handle;

    while (virtqueue_get_used_buf(queue, &handle, &wr_len)) {
        while (camkes_virtqueue_driver_gather_buffer(queue, &handle, &buf, &buf_size, &flag) >= 0) {
            camkes_virtqueue_buffer_free(queue, buf);
        }
    }
}

/**
 * Receives data from the source VMM using camkes virtqueue, and invokes the emul layer
 * to transmit the data to the guest VM.
 *
 * Data transmission from the backend layer to the emul layer is not 0-copy due to
 * the scatter-gather buffer that camkes virtqueue is using.
 *
 * @param queue Receive queue
 */
static void virtio_vsock_notify_recv(virtqueue_device_t *queue)
{
    virtqueue_ring_object_t handle;

    while (virtqueue_get_available_buf(queue, &handle)) {
        size_t len = virtqueue_scattered_available_size(queue, &handle);
        if (camkes_virtqueue_device_gather_copy_buffer(queue, &handle, (void *)temp_buf, len) < 0) {
            ZF_LOGW("Dropping data: Can't gather vq buffer.");
            continue;
        }

        virtio_vsock->emul_driver->emul_cb.rx_complete(virtio_vsock->emul, temp_buf, len);
        queue->notify();
    }
}

/**
 * Callback function for camkes virtqueue notification. Polls send queues and receive queues
 * to see if there are data pending.
 *
 * @see main.c (struct device_notify) for parameters
 */
void handle_vsock_irq(vm_t *vmm)
{
    for (int i = 0; i < NUM_PORTS; i++) {
        if (connections[i].recv_queue && VQ_DEV_POLL(connections[i].recv_queue)) {
            virtio_vsock_notify_recv(connections[i].recv_queue);
        }
        if (connections[i].send_queue && VQ_DRV_POLL(connections[i].send_queue)) {
            virtio_vsock_notify_free_send(connections[i].send_queue);
        }
    }
}

/**
 * Callback function for the emul layer. Forwards data to the destination VM using camkes virtqueue.
 *
 * @see virtio_pci_vsock.h for function format
 *
 * @param cid Dest context ID
 * @param buffer Data to forward
 * @param len Length of the data
 */
static void tx_forward(int cid, void *buffer, unsigned int len)
{
    /* Find the CID */
    for (int i = 0; i < NUM_PORTS; i++) {
        if (connections[i].cid == cid) {
            int err = camkes_virtqueue_driver_scatter_send_buffer(connections[i].send_queue, buffer, len);
            connections[i].send_queue->notify();
            return;
        }
    }

    ZF_LOGE("Invalid CID: %d", cid);
}

/**
 * Sets up virtio vsock connections.
 */
static void make_virtio_vsock_virtqueues(void)
{
    int err;
    for (int i = 0; i < NUM_PORTS; i++) {
        virtqueue_driver_t *vq_send;
        virtqueue_device_t *vq_recv;

        vq_recv = calloc(1, sizeof(*vq_recv));
        if (!vq_recv) {
            ZF_LOGE("Unable to alloc recv camkes-virtqueue for port: %d", i);
            return;
        }

        vq_send = calloc(1, sizeof(*vq_send));
        if (!vq_send) {
            ZF_LOGE("Unable to alloc send camkes-virtqueue for port: %d", i);
            return;
        }

        /* Initialise send virtqueue */
        err = camkes_virtqueue_driver_init(vq_send, socket_layout[i].send_id);
        if (err) {
            ZF_LOGE("Unable to initialise send camkes-virtqueue for port: %d", i);
            return;
        }

        /* Initialise recv virtqueue */
        err = camkes_virtqueue_device_init(vq_recv, socket_layout[i].recv_id);
        if (err) {
            ZF_LOGE("Unable to initialise recv camkes-virtqueue for port: %d", i);
            return;
        }

        connections[i].cid = socket_layout[i].cid;
        connections[i].recv_queue = vq_recv;
        connections[i].send_queue = vq_send;
    }
}

/**
 * Callback function for the emul layer. Notifies the guest that there is
 * something in its used ring
 *
 * @see virtio_pci_vsock.h for function format
 *
 * @param cookie Assumes struct virtio_vsock_cookie
 */
static void vsock_inject_irq(void *cookie)
{
    virtio_vsock_cookie_t *virtio_cookie = (virtio_vsock_cookie_t *)cookie;
    if (!virtio_cookie || !virtio_cookie->vm) {
        ZF_LOGE("NULL virtio cookie given to raw irq handler");
        return;
    }

    int err = vm_inject_irq(virtio_cookie->vm->vcpus[BOOT_VCPU], VIRTIO_SCK_IRQ);
    if (err) {
        ZF_LOGE("Failed to inject irq");
        return;
    }
}

void make_virtio_vsock(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports)
{
    int err;

    struct virtio_vsock_passthrough backend;

    backend.guest_cid = guest_cid;
    backend.injectIRQ = vsock_inject_irq;
    backend.forward = tx_forward;

    virtio_vsock_cookie_t *vsock_cookie = (virtio_vsock_cookie_t *)calloc(1, sizeof(virtio_vsock_cookie_t));
    ZF_LOGF_IF(vsock_cookie == NULL, "Failed to allocated virtio vsock cookie");

    vsock_cookie->vm = vm;
    backend.vsock_data = (void *) vsock_cookie;

    ioport_range_t virtio_port_range = {0, 0, VIRTIO_IOPORT_SIZE};
    virtio_vsock = common_make_virtio_vsock(vm, pci, io_ports, virtio_port_range, IOPORT_FREE,
                                            VIRTIO_SCK_IRQ, VIRTIO_SCK_IRQ, backend);
    assert(virtio_vsock);

    make_virtio_vsock_virtqueues();
}

/**
 * Dummy function used for registering IRQ callback handlers.
 */
void make_virtio_vsock_driver_dummy(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports) {}
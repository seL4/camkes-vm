/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <autoconf.h>

#include <utils/util.h>

#include <camkes.h>
#include <camkes/dataport.h>

#include <sel4vmmplatsupport/drivers/virtio_con.h>
#include <sel4vmmplatsupport/device.h>

#include <sel4vm/guest_irq_controller.h>
#include <sel4vm/boot.h>

#include <platsupport/serial.h>

#include <virtqueue.h>
#include <camkes/virtqueue.h>

#include "virtio_con.h"

#define NUM_PORTS ARRAY_SIZE(serial_layout)

/*
 * Represents the virtqueues used in a given link between
 * two VMs.
 */
typedef struct serial_conn {
    virtqueue_driver_t *send_queue;
    virtqueue_device_t *recv_queue;
} serial_conn_t;

static virtio_con_t *virtio_con;
static serial_conn_t connections[NUM_PORTS];

typedef struct virtio_con_cookie {
    virtio_con_t *virtio_con;
    vm_t *vm;
} virtio_con_cookie_t;

/* This matches the size of the buffer in serial server */
#define BUFSIZE 4088

struct staging_buf {
    uint32_t end;
    char buf[BUFSIZE];
} ;

static struct staging_buf staging_area[NUM_PORTS];

static void virtio_con_notify_free_send(virtqueue_driver_t *queue)
{
    void *buf = NULL;
    uint32_t buf_size = 0, wr_len = 0;
    vq_flags_t flag;
    virtqueue_ring_object_t handle;
    while (virtqueue_get_used_buf(queue, &handle, &wr_len)) {
        while (camkes_virtqueue_driver_gather_buffer(queue, &handle, &buf, &buf_size, &flag) >= 0) {
            /* Clean up and free the buffer we allocated */
            camkes_virtqueue_buffer_free(queue, buf);
        }
    }
}

void virtio_console_putchar(int port, virtio_emul_t *con, char *buf, int len) WEAK;
static int virtio_con_notify_recv(int port, virtqueue_device_t *queue)
{
    int err;
    void *buf = NULL;
    size_t buf_size = 0;
    vq_flags_t flag;
    virtqueue_ring_object_t handle;

    while (virtqueue_get_available_buf(queue, &handle)) {
        char temp_buf[BUFSIZE];
        size_t len = virtqueue_scattered_available_size(queue, &handle);
        if (camkes_virtqueue_device_gather_copy_buffer(queue, &handle, (void *)temp_buf, len) < 0) {
            ZF_LOGW("Dropping data for port %d: Can't gather vq buffer.", port);
            continue;
        }

        virtio_console_putchar(port, virtio_con->emul, temp_buf, len);
        queue->notify();
    }
}

void handle_serial_console(vm_t *vmm)
{
    for (int i = 0; i < NUM_PORTS; i++) {
        if (connections[i].recv_queue && VQ_DEV_POLL(connections[i].recv_queue)) {
            virtio_con_notify_recv(i, connections[i].recv_queue);
        }
        if (connections[i].send_queue && VQ_DRV_POLL(connections[i].send_queue)) {
            virtio_con_notify_free_send(connections[i].send_queue);
        }
    }
}

static void tx_virtcon_forward(int port, char c)
{
#if CONFIG_NUM_DOMAINS > 1 && CONFIG_PRINTING
    seL4_DebugPutChar(c);
#else
    if (port >= NUM_PORTS) {
        return;
    }

    struct staging_buf *batch_buf = &staging_area[port];
    batch_buf->buf[batch_buf->end] = c;
    batch_buf->end = (batch_buf->end + 1);

    /* If newline or staging area full, it's time to send */
    if (c == '\n' || batch_buf->end == BUFSIZE) {
        int err = camkes_virtqueue_driver_scatter_send_buffer(connections[port].send_queue, (void *) batch_buf->buf, batch_buf->end);
        ZF_LOGE_IF(err < 0, "Unknown error while enqueuing available buffer for port %d", port);
        batch_buf->end = 0;
    }

    connections[port].send_queue->notify();
#endif
}

static void make_virtio_con_virtqueues(void)
{
    int err;
    for (int i = 0; i < NUM_PORTS; i++) {
        virtqueue_driver_t *vq_send;
        virtqueue_device_t *vq_recv;

        vq_recv = malloc(sizeof(*vq_recv));
        ZF_LOGF_IF(!vq_recv, "Unable to alloc recv camkes-virtqueue for port: %d", i);

        vq_send = malloc(sizeof(*vq_send));
        ZF_LOGF_IF(!vq_send, "Unable to alloc send camkes-virtqueue for port: %d", i);

        /* Initialise send virtqueue */
        err = camkes_virtqueue_driver_init(vq_send, serial_layout[i].send_id);
        ZF_LOGF_IF(err, "Unable to initialise send camkes-virtqueue for port: %d", i);

        /* Initialise recv virtqueue */
        err = camkes_virtqueue_device_init(vq_recv, serial_layout[i].recv_id);
        ZF_LOGF_IF(err, "Unable to initialise recv camkes-virtqueue for port: %d", i);


        connections[i].recv_queue = vq_recv;
        connections[i].send_queue = vq_send;
    }
}

static void console_handle_irq(void *cookie)
{
    virtio_con_cookie_t *virtio_cookie = (virtio_con_cookie_t *)cookie;
    if (!virtio_cookie) {
        ZF_LOGE("NULL virtio cookie given to raw irq handler");
        return;
    }

    int err = vm_inject_irq(virtio_cookie->vm->vcpus[BOOT_VCPU], 9);
    if (err) {
        ZF_LOGE("Failed to inject irq");
        return;
    }
}

void make_virtio_con_driver(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports)
{
    ZF_LOGF_IF((NUM_PORTS > VIRTIO_CON_MAX_PORTS), "Too many ports configured (up the constant)");

    int err;
    struct console_passthrough backend;
    virtio_con_cookie_t *console_cookie;

    backend.handleIRQ = console_handle_irq;
    backend.putchar = tx_virtcon_forward;

    console_cookie = (virtio_con_cookie_t *)calloc(1, sizeof(struct virtio_con_cookie));
    if (console_cookie == NULL) {
        ZF_LOGE("Failed to allocated virtio console cookie");
        return;
    }

    backend.console_data = (void *)console_cookie;
    ioport_range_t virtio_port_range = {0, 0, VIRTIO_IOPORT_SIZE};
    virtio_con = common_make_virtio_con(vm, pci, io_ports, virtio_port_range, IOPORT_FREE,
                                        9, 9, backend);
    console_cookie->virtio_con = virtio_con;
    console_cookie->vm = vm;

    make_virtio_con_virtqueues();
    assert(virtio_con);
}

void make_virtio_con_driver_dummy(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports) {}

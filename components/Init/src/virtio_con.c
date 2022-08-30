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

static virtio_con_t *virtio_con;

typedef struct virtio_con_cookie {
    virtio_con_t *virtio_con;
    vm_t *vm;
} virtio_con_cookie_t;

/* Configured in camkes */
#define NUM_PORTS ARRAY_SIZE(serial_layout)

/* 4088 because the serial_shmem_t struct has to be 0x1000 bytes big */
#define BUFSIZE 4088

/* This struct occupies exactly 1 page and represents the data in the shmem region */
typedef struct serial_shmem {
    uint32_t head;
    uint32_t tail;
    char buf[BUFSIZE];
} serial_shmem_t;
compile_time_assert(serial_shmem_1k_size, sizeof(serial_shmem_t) == 0x1000);

/*
 * Represents the virtqueues used in a given link between
 * two VMs.
 */
typedef struct serial_conn {
    void (*notify)(void);
    volatile serial_shmem_t *recv_buf;
    volatile serial_shmem_t *send_buf;
} serial_conn_t;
static serial_conn_t connections[NUM_PORTS];

/* This is a temporary buffer used to stage data before completing the virtio RX */
char temp_rx_buf[BUFSIZE];

void virtio_console_putchar(int port, virtio_emul_t *con, char *buf, int len) WEAK;
static int virtio_con_notify_recv(int port)
{
    volatile serial_shmem_t *buffer = connections[port].recv_buf;
    int count = 0;
    while (buffer->head != buffer->tail) {
        temp_rx_buf[count] = buffer->buf[buffer->head];
        buffer->head = (buffer->head + 1) % BUFSIZE;
        count++;
    }

    assert(count <= BUFSIZE);
    virtio_console_putchar(port, virtio_con->emul, temp_rx_buf, count);
}

void handle_serial_console(vm_t *vmm)
{
    /* Poll each ring buffer to see if data was added */
    for (int i = 0; i < NUM_PORTS; i++) {
        if (connections[i].recv_buf->head != connections[i].recv_buf->tail) {
            virtio_con_notify_recv(i);
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

    volatile serial_shmem_t *batch_buf = connections[port].send_buf;
    batch_buf->buf[batch_buf->tail] = c;
    batch_buf->tail = (batch_buf->tail + 1) % BUFSIZE;

    /* If newline or staging area full, it's time to send */
    if (c == '\n' || (batch_buf->tail + 1) % BUFSIZE == batch_buf->head) {
        connections[port].notify();
    }
#endif
}
static void make_virtio_con_virtqueues(void)
{
    int err;
    for (int i = 0; i < NUM_PORTS; i++) {
        camkes_virtqueue_channel_t *recv_channel = get_virtqueue_channel(VIRTQUEUE_DEVICE, serial_layout[i].recv_id);
        camkes_virtqueue_channel_t *send_channel = get_virtqueue_channel(VIRTQUEUE_DRIVER, serial_layout[i].send_id);

        if (!recv_channel || !send_channel) {
            ZF_LOGE("Failed to get channel");
            return;
        }

        connections[i].notify = send_channel->notify;
        connections[i].recv_buf = (serial_shmem_t *) recv_channel->channel_buffer;
        connections[i].send_buf = (serial_shmem_t *) send_channel->channel_buffer;
        connections[i].recv_buf->head = 0;
        connections[i].recv_buf->tail = 0;
        connections[i].send_buf->head = 0;
        connections[i].send_buf->tail = 0;
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

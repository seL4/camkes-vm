/*
 * Copyright 2022, UNSW (ABN 57 195 873 179)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/**
 * @brief virtio console backend layer for x86
 *
 * The virtio console backend has two layers, the emul layer and the backend layer.
 * This file is part of the backend layer, it's responsible for:
 *
 * - TX: receiving data from the emul layer and forwarding them to the destination
 *   port using camkes virtqueue (which is different from the virtqueue that's
 *   used to communicate between the guest and the emul layer).
 * - RX: receiving data from the source port via camkes virtqueue and invoking the
 *   emul layer to handle the data.
 * - virtio console backend initialization
 *
 * This file relies on the connection topology and camkes virtqueue connection
 * being configured correctly in camkes.
 *
 * @todo Handling the RX and TX is similar between the x86/ARM VMMs, hence this file
 * and `components/VM_Arm/src/modules/virtio_con.c` share a lot of common code.
 * A refactor for the backend layer is needed.
 */

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
#include "virtio_irq.h"

static virtio_con_t *virtio_con;

/* Used as the parameter for callback functions (currently only `console_handle_irq`) */
typedef struct virtio_con_cookie {
    virtio_con_t *virtio_con;
    vm_t *vm;
} virtio_con_cookie_t;

/* Configured in camkes */
#define NUM_PORTS ARRAY_SIZE(serial_layout)

/* 4088 because the serial_shmem_t struct has to be 0x1000 bytes big */
#define BUFSIZE (0x1000 - 2 * sizeof(uint32_t))

/**
 * This struct is a ring buffer that occupies exactly one page, it should represent
 * the data in the shmem region (that is shared between two nodes of a virtio console
 * connection.
 *
 * Example:
 *
 *    |----------*************------------|
 *    0        head         tail       BUFSIZE
 *
 *    (- available, * used)

 * Invariants of the ring buffer:
 * 1. 0 <= head < BUFSIZE, 0 <= tail < BUFSIZE
 * 2. used = (tail >= head) ? (tail - head) : (tail + BUFSIZE - head)
 * 3. empty = head - tail == 0
 * 4. full = (tail + 1) % BUFSIZE == head
 *
 * For this ring buffer, always add data to the tail and take from the head. The access
 * can be lock-free since an instance of this struct can only be either the receive buffer
 * or the sent buffer of a virtio console node, which guarantees that only one end of the
 * ring buffer struct can be modified in a node.
 */
typedef struct serial_shmem {
    uint32_t head;
    uint32_t tail;
    char buf[BUFSIZE];
} serial_shmem_t;
compile_time_assert(serial_shmem_4k_size, sizeof(serial_shmem_t) == 0x1000);

/**
 * Represents the virtqueues used in a given link between
 * two VMs.
 */
typedef struct serial_conn {
    void (*notify)(void);
    serial_shmem_t *recv_buf;
    serial_shmem_t *send_buf;
} serial_conn_t;
static serial_conn_t connections[NUM_PORTS];

/**
 * Writes data to the guest console attached to a port, and sets the head of the receive
 * ring buffer to the right place.
 *
 * @param port Port number
 */
static void virtio_con_notify_recv(int port)
{
    serial_shmem_t *buffer = connections[port].recv_buf;
    uint32_t last_read_tail = buffer->tail;

    virtio_con->emul_driver->emul_cb.emul_putchar(port, virtio_con->emul, buffer->buf, buffer->head, last_read_tail);
    buffer->head = last_read_tail;
}

/**
 * Callback function for camkes virtqueue notification. Polls each receive ring buffer
 * to see if there are data pending. For some legacy reasons, this callback passes a
 * VMM handler as a parameter, it's unused here.
 *
 * @see main.c (struct device_notify) for usages
 */
void handle_serial_console(vm_t *vmm)
{
    for (int i = 0; i < NUM_PORTS; i++) {
        if (connections[i].recv_buf->head != connections[i].recv_buf->tail) {
            virtio_con_notify_recv(i);
        }
    }
}

/**
 * Forwards data to the destination port using camkes virtqueue, and notifies the dest VM.
 *
 * @param port Port number
 * @param c Data to forward
 */
static void tx_virtcon_forward(int port, char c)
{
    if (port >= NUM_PORTS) {
        return;
    }

    serial_shmem_t *batch_buf = connections[port].send_buf;
    batch_buf->buf[batch_buf->tail] = c;
    __atomic_thread_fence(__ATOMIC_ACQ_REL);
    batch_buf->tail = (batch_buf->tail + 1) % BUFSIZE;

    /* If newline or staging area full, it's time to send */
    if (c == '\n' || (batch_buf->tail + 1) % BUFSIZE == batch_buf->head) {
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
        connections[port].notify();
    }
}

/**
 * Sets up virtio console connections.
 */
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

/**
 * Callback function for the emul layer. Notifies the guest that there is
 * something in its used ring
 *
 * @see virtio_pci_console.h for function format
 *
 * @param cookie Pointer to a virtio_con_cookie_t
 */
static void console_handle_irq(void *cookie)
{
    virtio_con_cookie_t *virtio_cookie = (virtio_con_cookie_t *)cookie;
    if (!virtio_cookie || !virtio_cookie->vm) {
        ZF_LOGE("NULL virtio cookie given to raw irq handler");
        return;
    }

    int err = vm_inject_irq(virtio_cookie->vm->vcpus[BOOT_VCPU], VIRTIO_CON_IRQ);
    if (err) {
        ZF_LOGE("Failed to inject irq");
        return;
    }
}

void make_virtio_con(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports)
{
    ZF_LOGF_IF((NUM_PORTS > VIRTIO_CON_MAX_PORTS), "Too many ports configured (up the constant)");

    int err;
    struct virtio_console_passthrough backend;

    backend.handleIRQ = console_handle_irq;
    backend.backend_putchar = tx_virtcon_forward;

    virtio_con_cookie_t *console_cookie = (virtio_con_cookie_t *)calloc(1, sizeof(virtio_con_cookie_t));
    ZF_LOGF_IF(console_cookie == NULL, "Failed to allocated virtio console cookie");

    backend.console_data = (void *)console_cookie;
    ioport_range_t virtio_port_range = {0, 0, VIRTIO_IOPORT_SIZE};
    virtio_con = common_make_virtio_con(vm, pci, io_ports, virtio_port_range, IOPORT_FREE,
                                        VIRTIO_CON_IRQ, VIRTIO_CON_IRQ, backend);
    console_cookie->virtio_con = virtio_con;
    console_cookie->vm = vm;

    make_virtio_con_virtqueues();
    assert(virtio_con);
}

/**
 * Dummy function used to register irq callback handlers.
 */
void make_virtio_con_driver_dummy(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports) {}

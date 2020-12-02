/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <autoconf.h>
#include <vmlinux.h>

#include <utils/util.h>

#include <camkes.h>
#include <camkes/dataport.h>

#include <sel4vmmplatsupport/drivers/virtio_con.h>
#include <sel4vmmplatsupport/device.h>
#include <sel4vmmplatsupport/arch/vpci.h>

#include <platsupport/serial.h>
#include <virtio/virtio_console.h>

static virtio_con_t *virtio_con = NULL;
extern void *serial_getchar_buf;

//this matches the size of the buffer in serial server
#define BUFSIZE 4088

char txbuf[BUFSIZE];
extern vmm_pci_space_t *pci;
extern vmm_io_port_list_t *io_ports;

struct {
    uint32_t head;
    uint32_t tail;
    char buf[BUFSIZE];
} extern volatile *batch_buf;

static int handle_serial_console(vm_t *vmm, void *cookie UNUSED)
{
    struct {
        uint32_t head;
        uint32_t tail;
        char buf[BUFSIZE];
    } volatile *buffer = serial_getchar_buf;

    int count = 0;
    while (buffer->head != buffer->tail) {
        txbuf[count % BUFSIZE] = buffer->buf[buffer->head];
        buffer->head = (buffer->head + 1) % BUFSIZE;
        count += 1 % BUFSIZE;
    }
    virtio_console_putchar(virtio_con->emul, txbuf, count);
    return 0;
}

static void emulate_console_putchar(char c)
{
#if CONFIG_NUM_DOMAINS > 1 && CONFIG_PRINTING
    seL4_DebugPutChar(c);
#else
    batch_buf->buf[batch_buf->tail] = c;
    batch_buf->tail = (batch_buf->tail + 1) % BUFSIZE;
    if (c == '\n' || batch_buf->tail + 1 == batch_buf->head) {
        batch_batch();
    }
#endif
}

extern seL4_Word serial_getchar_notification_badge(void);
void make_virtio_con(vm_t *vm, void *cookie)
{
    int err = register_async_event_handler(serial_getchar_notification_badge(), handle_serial_console, NULL);
    ZF_LOGF_IF(err, "Failed to register_async_event_handler for make_virtio_con.");
    virtio_con = virtio_console_init(vm, emulate_console_putchar, pci, io_ports);
    if (!virtio_con) {
        ZF_LOGF("Failed to initialise virtio console");
    }
}

DEFINE_MODULE(virtio_con, NULL, make_virtio_con)

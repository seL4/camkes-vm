/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <autoconf.h>

#include <sel4vm/guest_irq_controller.h>
#include <sel4vm/boot.h>

#include <sel4vmmplatsupport/drivers/virtio_con.h>
#include <sel4vmmplatsupport/device.h>
#include <sel4vmmplatsupport/arch/vpci.h>

#include <virtioarm/virtio.h>
#include <virtioarm/virtio_plat.h>
#include <virtioarm/virtio_console.h>

typedef struct virtio_con_cookie {
    virtio_con_t *virtio_con;
    vm_t *vm;
} virtio_con_cookie_t;

static void virtio_console_ack(vm_vcpu_t *vcpu, int irq, void *token) {}

static void console_handle_irq(void *cookie)
{
    virtio_con_cookie_t *virtio_cookie = (virtio_con_cookie_t *)cookie;
    if (!virtio_cookie || !virtio_cookie->vm) {
        ZF_LOGE("NULL virtio cookie given to raw irq handler");
        return;
    }
    int err = vm_inject_irq(virtio_cookie->vm->vcpus[BOOT_VCPU], VIRTIO_CON_PLAT_INTERRUPT_LINE);
    if (err) {
        ZF_LOGE("Failed to inject irq");
        return;
    }
}

virtio_con_t *virtio_console_init(vm_t *vm, console_putchar_fn_t putchar,
                                  vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports)
{

    int err;
    struct virtio_console_passthrough backend;
    virtio_con_cookie_t *console_cookie;
    virtio_con_t *virtio_con;

    backend.handleIRQ = console_handle_irq;
    backend.backend_putchar = putchar;

    console_cookie = (virtio_con_cookie_t *)calloc(1, sizeof(struct virtio_con_cookie));
    if (console_cookie == NULL) {
        ZF_LOGE("Failed to allocated virtio console cookie");
        return NULL;
    }

    backend.console_data = (void *)console_cookie;
    ioport_range_t virtio_port_range = {0, 0, VIRTIO_IOPORT_SIZE};
    virtio_con = common_make_virtio_con(vm, pci, io_ports, virtio_port_range, IOPORT_FREE,
                                        VIRTIO_INTERRUPT_PIN, VIRTIO_CON_PLAT_INTERRUPT_LINE, backend);
    console_cookie->virtio_con = virtio_con;
    console_cookie->vm = vm;
    err =  vm_register_irq(vm->vcpus[BOOT_VCPU], VIRTIO_CON_PLAT_INTERRUPT_LINE, &virtio_console_ack, NULL);
    if (err) {
        ZF_LOGE("Failed to register console irq");
        return NULL;
    }
    return virtio_con;
}

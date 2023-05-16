/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <sel4vmmplatsupport/drivers/virtio_con.h>
#include <virtqueue.h>

virtio_con_t *virtio_console_init(vm_t *vm, console_putchar_fn_t putchar,
                                  vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports);

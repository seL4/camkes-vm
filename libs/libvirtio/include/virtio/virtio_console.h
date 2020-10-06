/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#pragma once

#include <sel4vmmplatsupport/drivers/virtio_con.h>

virtio_con_t *virtio_console_init(vm_t *vm, console_putchar_fn_t putchar,
                                  vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports);

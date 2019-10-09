/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#pragma once

#include <sel4vmmplatsupport/ioports.h>
#include <sel4vmmplatsupport/drivers/pci.h>

void make_virtio_net_vswitch_driver(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports);

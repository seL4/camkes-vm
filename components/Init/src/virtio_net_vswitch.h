/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <sel4vmmplatsupport/ioports.h>
#include <sel4vmmplatsupport/drivers/pci.h>

void make_virtio_net_vswitch(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports);

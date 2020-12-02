/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <sel4vmmplatsupport/ioports.h>
#include <sel4vmmplatsupport/drivers/pci.h>

#define FREE_IOPORT_START   0x9000
#define VIRTIO_IOPORT_SIZE  0x400

void make_virtio_net(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports);
void virtio_net_notify(vm_t *vm);

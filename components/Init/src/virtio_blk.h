/*
 * Copyright 2019, Dornerworks
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#pragma once

#include <sel4vmmplatsupport/ioports.h>
#include <sel4vmmplatsupport/drivers/pci.h>

#define VIRTIO_IOPORT_SIZE 0x40

void make_virtio_blk(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports);
void virtio_blk_notify(vm_t *vm);

/*
 * Copyright 2022, UNSW (ABN 57 195 873 179)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <sel4vmmplatsupport/ioports.h>
#include <sel4vmmplatsupport/drivers/pci.h>

/* ioport size, used for ioport emulation of virtio console */
#define VIRTIO_IOPORT_SIZE  0x400

/**
 * Initialize a virtio console backend. This includes:
 * - adding an IO port handler
 * - adding a PCI entry
 * - initializing the emul layer
 * - setting up connections
 *
 * @param vm The vm handler of the caller
 * @param pci Virtual PCI space of the caller
 * @param io_ports list of registered ioports of the caller
 */
void make_virtio_con(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports);

/*
 * Copyright 2022, UNSW (ABN 57 195 873 179)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <sel4vmmplatsupport/ioports.h>
#include <sel4vmmplatsupport/drivers/pci.h>

/* ioport size, used for ioport emulation of virtio vsock */
#define VIRTIO_IOPORT_SIZE  0x400

/**
 * Initialize a virtio vsock backend. This includes:
 * - adding an IO port handler
 * - adding a PCI entry
 * - initializing the emul layer
 * - setting up connections
 *
 * @param vm
 * @param pci
 * @param io_ports
 */
void make_virtio_vsock(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports);

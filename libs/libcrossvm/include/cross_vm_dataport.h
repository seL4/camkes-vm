/*
 * Copyright 2017, Data61
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

#include <dataport_caps.h>
#include <sel4vm/guest_vm.h>
#include <sel4vmmplatsupport/drivers/pci_helper.h>
#include <pci/helper.h>

int cross_vm_dataports_init_common(vm_t *vm, dataport_caps_handle_t **d, int n, vmm_pci_space_t *pci);

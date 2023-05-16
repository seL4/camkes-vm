/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <sel4vm/guest_vm.h>
#include <sel4vm/guest_memory.h>
#include <sel4vm/guest_memory_helpers.h>
#include <sel4vm/guest_ram.h>
#include <sel4vmmplatsupport/guest_memory_util.h>
#include <vmlinux.h>

void WEAK init_ram_module(vm_t *vm, void *cookie)
{
    int err = vm_ram_register_at(vm,
                                 vm_config.ram.base,
                                 vm_config.ram.size,
                                 vm->mem.map_one_to_one);
    assert(!err);
}

DEFINE_MODULE(init_ram, NULL, init_ram_module)

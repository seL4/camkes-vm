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

/* TODO: move stuff from fdt_manipulation.c to library */
int fdt_generate_memory_node(void *fdt, unsigned long base, size_t size);

static int generate_fdt(void *fdt, vm_t *vm, void *cookie)
{
    /* generate a memory node (ram_base and ram_size) */
    int err = fdt_generate_memory_node(fdt, ram_base, ram_size);
    if (err) {
        ZF_LOGE("Couldn't generate memory node (%d)", err);
        return err;
    }

    return 0;
}

void WEAK init_ram_module(vm_t *vm, void *cookie)
{
    int err = vm_ram_register_at(vm, ram_base, ram_size, vm->mem.map_one_to_one);
    assert(!err);
}

DEFINE_MODULE(init_ram, NULL, init_ram_module)
DEFINE_MODULE_FDT(init_ram, generate_fdt)

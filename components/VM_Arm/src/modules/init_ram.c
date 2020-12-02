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

extern unsigned long linux_ram_base;
extern unsigned long linux_ram_size;

void WEAK init_ram_module(vm_t *vm, void *cookie)
{
    int err;

    if (config_set(CONFIG_PLAT_EXYNOS5) || config_set(CONFIG_PLAT_QEMU_ARM_VIRT) || config_set(CONFIG_PLAT_TX2)) {
        err = vm_ram_register_at(vm, linux_ram_base, linux_ram_size, true);
    } else {
        err = vm_ram_register_at(vm, linux_ram_base, linux_ram_size, false);
    }
    assert(!err);
}

DEFINE_MODULE(init_ram, NULL, init_ram_module)

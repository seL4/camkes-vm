/*
 * Copyright 2018, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <sel4vm/guest_vm.h>
#include <sel4vm/guest_memory.h>
#include <sel4vm/guest_memory_helpers.h>
#include <sel4vmmplatsupport/guest_memory_util.h>
#include <vmlinux.h>
extern int start_extra_frame_caps;

static vm_frame_t map_frame_hack_iterator(uintptr_t addr, void *cookie)
{
    int error;
    cspacepath_t return_frame;
    vm_frame_t frame_result = { seL4_CapNull, seL4_NoRights, 0, 0 };

    int cap_idx = (extra_frame_map_address - addr) / BIT(PAGE_BITS_4K);
    frame_result.cptr = start_extra_frame_caps + cap_idx;
    frame_result.rights = seL4_AllRights;
    frame_result.vaddr = addr;
    frame_result.size_bits = PAGE_BITS_4K;

    return frame_result;
}

static void map_frame_hack_init_module(vm_t *vm, void *cookie)
{

    /* hack to give access to other components
       see https://github.com/smaccm/vm_hack/blob/master/details.md for details */
    if (num_extra_frame_caps == 0) {
        return;
    }

    vm_memory_reservation_t *frame_hack_reservation = vm_reserve_memory_at(vm, extra_frame_map_address,
                                                                           num_extra_frame_caps * BIT(PAGE_BITS_4K), default_error_fault_callback, NULL);
    if (!frame_hack_reservation) {
        ZF_LOGE("Failed to reserve frame hack memory");
        return;
    }
    int err = vm_map_reservation(vm, frame_hack_reservation, map_frame_hack_iterator, NULL);
    if (err) {
        ZF_LOGE("Failed to map frame hack memory");
    }
}

DEFINE_MODULE(map_frame_hack, NULL, map_frame_hack_init_module)

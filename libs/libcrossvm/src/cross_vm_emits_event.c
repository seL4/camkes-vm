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

#include "camkes_emits_event.h"
#include "cross_vm_shared/cross_vm_shared_guest_to_vmm_event.h"
#include <sel4vm/guest_vm.h>
#include <sel4vm/arch/guest_x86_context.h>
#include <sel4vm/arch/vmcall.h>
#include <utils/zf_log.h>

static camkes_emit_fn *events;
static int num_events;

static int emit_event(int id)
{
    if (id == 0) {
        ZF_LOGE("Invalid event id 0");
        return -1;
    }

    if (id >= num_events) {
        ZF_LOGE("Invalid event id %d. Id must be in range 1..%d", id, num_events - 1);
        return -1;
    }

    camkes_emit_fn emit = events[id];

    if (emit == NULL) {
        ZF_LOGE("No emit function found for id %d", id);
        return -1;
    }

    emit();

    return 0;
}

static int event_vmcall_handler(vm_vcpu_t *vcpu)
{

    int cmd;
    int err = vm_get_thread_context_reg(vcpu, VCPU_CONTEXT_EBX, &cmd);
    if (err) {
        ZF_LOGE("Failed to get thread context register for command");
        return -1;
    }

    switch (cmd) {
    case EVENT_CMD_EMIT: {
        int id;
        err = vm_get_thread_context_reg(vcpu, VCPU_CONTEXT_ECX, &id);
        if (err) {
            ZF_LOGE("Failed to get thread context register for event id");
            return -1;
        }
        return emit_event(id);
    }
    default: {
        ZF_LOGE("Unknown command: %d", cmd);
        return -1;
    }
    }

    return 0;
}

int cross_vm_emits_events_init_common(vm_t *vm,  camkes_emit_fn *e, int n)
{
    events = e;
    num_events = n;
    return reg_new_handler(vm, event_vmcall_handler, EVENT_VMCALL_GUEST_TO_VMM_HANDLER_TOKEN);
}

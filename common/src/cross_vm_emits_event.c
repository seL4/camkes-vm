/*
 * Copyright 2016, Data 61
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(D61_GPL)
 */

#include <camkes.h>
#include <camkes_emits_event.h>
#include <cross_vm_shared_guest_to_vmm_event.h>
#include <vmm/vmm.h>
#include <utils/zf_log.h>

static camkes_emit_fn *events;
static int num_events;

static int emit_event(int id) {
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

static int event_vmcall_handler(vmm_vcpu_t *vcpu) {

    int cmd = vmm_read_user_context(&vcpu->guest_state, USER_CONTEXT_EBX);

    switch (cmd) {
    case EVENT_CMD_EMIT: {
        int id = vmm_read_user_context(&vcpu->guest_state, USER_CONTEXT_ECX);
        return emit_event(id);
    }
    default: {
        ZF_LOGE("Unknown command: %d", cmd);
        return -1;
    }
    }

    return 0;
}

int cross_vm_emits_events_init_common(vmm_t *vmm,  camkes_emit_fn *e, int n) {
    events = e;
    num_events = n;
    return reg_new_handler(vmm, event_vmcall_handler, EVENT_VMCALL_GUEST_TO_VMM_HANDLER_TOKEN);
}

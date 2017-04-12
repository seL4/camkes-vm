/*
 * Copyright 2016, Data 61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(D61_GPL)
 */

#include <sel4/sel4.h>
#include <camkes.h>
#include <camkes_consumes_event.h>
#include <camkes_emits_event.h>
#include <cross_vm_consumes_event.h>
#include <cross_vm_emits_event.h>
#include <camkes_mutex.h>
#include <vmm/vmm.h>
#include <vspace/vspace.h>

// events to forward to guest
static camkes_consumes_event_t consumed_events[] = {
    { .id = 1, .reg_callback = done_reg_callback },
};

// events to forward from guest
static camkes_emit_fn emitted_events[] = {
    NULL,
    ready_emit,
};

// mutex to protect shared event context
static camkes_mutex_t cross_vm_event_mutex = {
    .lock = cross_vm_event_mutex_lock,
    .unlock = cross_vm_event_mutex_unlock,
};

int cross_vm_consumes_events_init(vmm_t *vmm, vspace_t *vspace, seL4_Word irq_badge) {
    return cross_vm_consumes_events_init_common(vmm, vspace, &cross_vm_event_mutex,
            consumed_events, sizeof(consumed_events)/sizeof(consumed_events[0]), irq_badge);
}

int cross_vm_emits_events_init(vmm_t *vmm) {
    return cross_vm_emits_events_init_common(vmm, emitted_events,
            sizeof(emitted_events)/sizeof(emitted_events[0]));
}

/*
 * Copyright 2016, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(D61_BSD)
 */

#include <camkes.h>
#include <camkes_event.h>
#include <camkes_mutex.h>
#include <cross_vm_event.h>
#include <vmm/vmm.h>
#include <vspace/vspace.h>

// events to forward to guest
static camkes_event_t events[] = {
    { .id = 1, .reg_callback = ev1_reg_callback },
    { .id = 2, .reg_callback = ev2_reg_callback },
};

// mutex to protect shared event context
static camkes_mutex_t cross_vm_event_mutex = (camkes_mutex_t) {
    .lock = cross_vm_event_mutex_lock,
    .unlock = cross_vm_event_mutex_unlock,
};

int cross_vm_events_init(vmm_t *vmm, vspace_t *vspace) {
    return cross_vm_events_init_common(vmm, vspace, &cross_vm_event_mutex,
                                       events, sizeof(events)/sizeof(events[0]));
}

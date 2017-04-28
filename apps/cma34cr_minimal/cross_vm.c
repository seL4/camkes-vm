#include <sel4/sel4.h>
#include <camkes.h>
#include <camkes_mutex.h>
#include <camkes_consumes_event.h>
#include <camkes_emits_event.h>
#include <dataport_caps.h>
#include <cross_vm_consumes_event.h>
#include <cross_vm_emits_event.h>
#include <cross_vm_dataport.h>
#include <vmm/vmm.h>
#include <vspace/vspace.h>

// this is defined in the dataport's glue code
extern dataport_caps_handle_t data_handle;

static dataport_caps_handle_t *dataports[] = {
    NULL, // entry 0 is NULL so ids correspond with indices
    &data_handle,
};

// events to forward to guest
static camkes_consumes_event_t consumed_events[] = {
    { .id = 1, .reg_callback = done_printing_reg_callback },
};

// events to forward from guest
static camkes_emit_fn emitted_events[] = {
    NULL,   // entry 0 is NULL so ids correspond with indices
    do_print_emit,
};

// mutex to protect shared event context
static camkes_mutex_t cross_vm_event_mutex = (camkes_mutex_t) {
    .lock = cross_vm_event_mutex_lock,
    .unlock = cross_vm_event_mutex_unlock,
};

int cross_vm_dataports_init(vmm_t *vmm) {
    return cross_vm_dataports_init_common(vmm, dataports, sizeof(dataports)/sizeof(dataports[0]));
}

int cross_vm_emits_events_init(vmm_t *vmm) {
    return cross_vm_emits_events_init_common(vmm, emitted_events,
            sizeof(emitted_events)/sizeof(emitted_events[0]));
}

int cross_vm_consumes_events_init(vmm_t *vmm, vspace_t *vspace, seL4_Word irq_badge) {
    return cross_vm_consumes_events_init_common(vmm, vspace, &cross_vm_event_mutex,
            consumed_events, sizeof(consumed_events)/sizeof(consumed_events[0]), irq_badge);
}

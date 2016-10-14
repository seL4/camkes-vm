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

#include <cross_vm_dataport.h>
#include <vmm/vmm.h>

// these are defined in the dataport's glue code
extern cross_vm_dataport_handle_t dp1_handle;
extern cross_vm_dataport_handle_t dp2_handle;

static cross_vm_dataport_handle_t *dataports[] = {
    NULL, // entry 0 is NULL so ids correspond with indices
    &dp1_handle,
    &dp2_handle,
};

void cross_vm_dataports_init(vmm_t *vmm) {
    cross_vm_dataports_init_common(vmm, dataports, sizeof(dataports)/sizeof(dataports[0]));
}

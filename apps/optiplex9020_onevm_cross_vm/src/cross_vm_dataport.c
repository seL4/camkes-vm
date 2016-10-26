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
#include <dataport_caps.h>
#include <cross_vm_dataport.h>
#include <vmm/vmm.h>

// these are defined in the dataport's glue code
extern dataport_caps_handle_t dp1_handle;
extern dataport_caps_handle_t dp2_handle;

static dataport_caps_handle_t *dataports[] = {
    NULL, // entry 0 is NULL so ids correspond with indices
    &dp1_handle,
    &dp2_handle,
};

int cross_vm_dataports_init(vmm_t *vmm) {
    return cross_vm_dataports_init_common(vmm, dataports, sizeof(dataports)/sizeof(dataports[0]));
}

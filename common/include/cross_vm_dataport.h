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

#pragma once

#include <stdlib.h>
#include <sel4/sel4.h>
#include <vmm/vmm.h>

/* An interface for accessing information about a cross vm dataport.
 * This is implemented by the "to" side of the CrossVMSharedMemory
 * connector, and can be used by the component on that side (typically
 * a VMM).
 */

typedef seL4_CPtr (*cross_vm_get_nth_frame_cap_fn)(unsigned int n);
typedef unsigned int (*cross_vm_get_id_fn)(void);
typedef unsigned int (*cross_vm_get_num_frame_caps_fn)(void);
typedef seL4_CPtr* (*cross_vm_get_frame_caps_fn)(void);
typedef size_t (*cross_vm_get_size_fn)(void);

typedef struct {
    cross_vm_get_nth_frame_cap_fn get_nth_frame_cap;
    cross_vm_get_id_fn get_id;
    cross_vm_get_num_frame_caps_fn get_num_frame_caps;
    cross_vm_get_frame_caps_fn get_frame_caps;
    cross_vm_get_size_fn get_size;
} cross_vm_dataport_handle_t;

static inline seL4_CPtr
cross_vm_dataport_get_nth_frame_cap(cross_vm_dataport_handle_t *d, unsigned int n)
{
    return d->get_nth_frame_cap(n);
}

static inline unsigned int
cross_vm_dataport_get_id(cross_vm_dataport_handle_t *d)
{
    return d->get_id();
}

static inline unsigned int
cross_vm_dataport_get_num_frame_caps(cross_vm_dataport_handle_t *d)
{
    return d->get_num_frame_caps();
}

static inline seL4_CPtr*
cross_vm_dataport_get_frame_caps(cross_vm_dataport_handle_t *d)
{
    return d->get_frame_caps();
}

static inline size_t
cross_vm_dataport_get_size(cross_vm_dataport_handle_t *d)
{
    return d->get_size();
}

int cross_vm_dataports_init_common(vmm_t *vmm, cross_vm_dataport_handle_t **d, int n);

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

#include <camkes_event.h>
#include <camkes_mutex.h>
#include <vmm/vmm.h>
#include <vspace/vspace.h>

int cross_vm_events_init_common(vmm_t *vmm, vspace_t *vspace, camkes_mutex_t *mutex,
                                camkes_event_t *events, int n);

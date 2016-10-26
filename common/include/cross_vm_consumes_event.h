/*
 * Copyright 2016, Data 61
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(D61_GPL)
 */

#pragma once

#include <camkes_consumes_event.h>
#include <camkes_mutex.h>
#include <vmm/vmm.h>
#include <vspace/vspace.h>

int cross_vm_consumes_events_init_common(vmm_t *vmm, vspace_t *vspace,
                                         camkes_mutex_t *mutex,
                                         camkes_consumes_event_t *events, int n);

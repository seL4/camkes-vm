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

#include <camkes_emits_event.h>
#include <vmm/vmm.h>

int cross_vm_emits_events_init_common(vmm_t *vmm, camkes_emit_fn *events, int n);

/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#pragma once

#include <camkes_emits_event.h>
#include <sel4vm/guest_vm.h>
#include <sel4vm/vmm.h>

int cross_vm_emits_events_init_common(vm_t *vm, camkes_emit_fn *events, int n);

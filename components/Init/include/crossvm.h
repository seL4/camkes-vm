/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#pragma once

#include <sel4vm/guest_vm.h>
#include <sel4vmmplatsupport/drivers/cross_vm_connection.h>
#include <sel4vmmplatsupport/drivers/pci_helper.h>

#include <camkes/dataport_caps.h>

struct camkes_consumes_event {
    vm_t *vm;
    unsigned int id;
    int (*reg_callback)(event_callback_fn, void *arg);
    seL4_CPtr irq_notification;
};

struct camkes_crossvm_connection {
    dataport_caps_handle_t *handle;
    emit_fn emit_fn;
    struct camkes_consumes_event consume_event;
};

/**
 * Initialise and register a series of camkes crossvm connections with a given vm
 * @param[in] vm                    A handle to the VM
 * @param[in] connection_base_addr  The base guest physical address to interface the crossvm connection devices through
 * @param[in] connections           An array of camkes crossvm connections
 * @param[in] num_connection        The number of elements in the 'connections' array (parameter)
 * @param[in] pci                   A handle to the virtual pci driver
 * @param[in] irq_notification      Notification object to signal crossvm irq
 * @return -1 for error, otherwise 0 for success
 */
int cross_vm_connections_init(vm_t *vm, uintptr_t connection_base_addr, struct camkes_crossvm_connection *connections,
                              int num_connections, vmm_pci_space_t *pci, seL4_CPtr irq_notification);

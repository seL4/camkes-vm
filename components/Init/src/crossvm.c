/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <crossvm.h>
#include <sel4vm/guest_vm.h>
#include <sel4vmmplatsupport/drivers/cross_vm_connection.h>
#include <sel4vmmplatsupport/drivers/pci_helper.h>

extern int get_crossvm_irq_num(void);

static void event_camkes_callback(void *arg)
{
    struct camkes_crossvm_connection *conn = arg;
    consume_connection_event(conn->consume_event.vm, conn->consume_event.id, false);
    seL4_Signal(conn->consume_event.irq_notification);
    int err = conn->consume_event.reg_callback(event_camkes_callback, conn);
    assert(!err);
}

int cross_vm_connections_init(vm_t *vm, uintptr_t connection_base_addr, struct camkes_crossvm_connection *connections,
                              int num_connections, vmm_pci_space_t *pci, seL4_CPtr irq_notification)
{
    crossvm_handle_t *crossvm_connections = calloc(num_connections, sizeof(crossvm_handle_t));
    if (!crossvm_connections) {
        return -1;
    }
    for (int i = 0; i < num_connections; i++) {
        /* Initialise crossvm dataport handle */
        crossvm_dataport_handle_t *dp_handle = calloc(1, sizeof(crossvm_dataport_handle_t));
        if (!dp_handle) {
            ZF_LOGE("Failed to initialse cross vm connection dataport %d", i);
            return -1;
        }
        dataport_caps_handle_t *handle = connections[i].handle;
        dp_handle->size = handle->get_size();
        dp_handle->num_frames = handle->get_num_frame_caps();
        dp_handle->frames = handle->get_frame_caps();

        /* Initialise consume event connection callbacks */
        if (connections[i].consume_event.reg_callback) {
            connections[i].consume_event.irq_notification = irq_notification;
            connections[i].consume_event.reg_callback(event_camkes_callback, &connections[i]);
        }

        /* Initialise crossvm connection */
        crossvm_connections[i].dataport = dp_handle;
        crossvm_connections[i].emit_fn = connections[i].emit_fn;
        crossvm_connections[i].consume_id = (seL4_Word)connections[i].consume_event.id;
    }

    int ret = cross_vm_connections_init_common(vm, connection_base_addr, crossvm_connections, num_connections,
                                               pci, get_crossvm_irq_num);
    free(crossvm_connections);
    return ret;
}

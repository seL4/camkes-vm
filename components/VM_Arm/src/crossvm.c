/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <vmlinux.h>
#include <sel4vm/guest_vm.h>
#include <sel4vmmplatsupport/drivers/cross_vm_connection.h>
#include <sel4vmmplatsupport/drivers/pci_helper.h>

extern vmm_pci_space_t *pci;

/* Force the _vmm_cross_connector_definition  section to be created even if no modules are defined. */
static USED SECTION("_vmm_cross_connector_definition") struct {} dummy_module;
extern camkes_crossvm_connection_t *__start__vmm_cross_connector_definition[];
extern camkes_crossvm_connection_t *__stop__vmm_cross_connector_definition[];


static int setup_connection(crossvm_handle_t *crossvm_connections, size_t index,
                            camkes_crossvm_connection_t *connection)
{
    crossvm_dataport_handle_t *dp_handle = calloc(1, sizeof(crossvm_dataport_handle_t));
    if (!dp_handle) {
        ZF_LOGE("Failed to initialize cross vm connection dataport %zd", index);
        return -1;
    }

    dataport_caps_handle_t *handle = connection->handle;
    dp_handle->size = handle->get_size();
    dp_handle->num_frames = handle->get_num_frame_caps();
    dp_handle->frames = handle->get_frame_caps();

    crossvm_connections[index].dataport = dp_handle;
    crossvm_connections[index].emit_fn = connection->emit_fn;
    crossvm_connections[index].consume_id = connection->consume_badge;
    crossvm_connections[index].connection_name = connection->connection_name;
    return 0;
}

int cross_vm_connections_init(vm_t *vm, uintptr_t connection_base_addr, camkes_crossvm_connection_t *connections,
                              int num_connections)
{

    int sectioned_connections = 0;
    // Connections can also be declared in linker sections.
    for (camkes_crossvm_connection_t **i = __start__vmm_cross_connector_definition;
         i < __stop__vmm_cross_connector_definition; i++) {
        if (*i != NULL) {
            sectioned_connections++;
        }
    }


    crossvm_handle_t *crossvm_connections = calloc(num_connections + sectioned_connections, sizeof(crossvm_handle_t));
    if (!crossvm_connections) {
        return -1;
    }
    for (int i = 0; i < num_connections; i++) {
        if (setup_connection(crossvm_connections, i, &connections[i])) {
            return -1;
        }
    }
    int i = num_connections;
    for (camkes_crossvm_connection_t **connection = __start__vmm_cross_connector_definition;
         connection < __stop__vmm_cross_connector_definition; connection++, i++) {
        if (*connection != NULL) {
            if (setup_connection(crossvm_connections, i, *connection)) {
                return -1;
            }
        }
    }

    int ret = cross_vm_connections_init_common(vm, connection_base_addr, crossvm_connections,
                                               num_connections + sectioned_connections,
                                               pci, get_crossvm_irq_num);
    free(crossvm_connections);
    return ret;
}

/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <assert.h>
#include <camkes/error.h>
#include <stdio.h>
#include <stdint.h>
#include <sel4/sel4.h>
#include <sel4vm/guest_vm.h>
#include <sel4vmmplatsupport/drivers/pci_helper.h>
#include <sel4vmmplatsupport/ioports.h>
#include <camkes.h>

/*- set cons = configuration[me.name].get("init_cons") -*/
/*- set cons = lambda('x: [] if x is None else x')(cons) -*/

/*- for con in cons -*/
    void /*? con['init'].strip('"') ?*/(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports);
    /*- if 'irq' in con -*/
        void /*? con['irq'].strip('"') ?*/(vm_t *vm);
    /*- endif -*/
/*- endfor -*/

int init_cons_num_connections() {
    return /*? len(cons) ?*/;
}

uintptr_t init_cons_init_function(int con) {
    /*- if len(cons) == 0 -*/
        return -1;
    /*- else -*/
        switch(con) {
        /*- for con in cons -*/
            case /*? loop.index0 ?*/:
                return (uintptr_t)/*? con['init'].strip('"') ?*/;
        /*- endfor -*/
            default:
                return -1;
        }
    /*- endif -*/
}

int init_cons_has_interrupt(int con, uintptr_t *badge, uintptr_t *fun) {
    /*- if len(cons) == 0 -*/
        return -1;
    /*- else -*/
        switch (con) {
            /*- for con in cons -*/
            case /*? loop.index0 ?*/:
                /*- if 'irq' not in con -*/
                    return 0;
                /*- else -*/
                    *badge = /*? con['badge'] ?*/;
                    *fun = (uintptr_t)/*? con['irq'].strip('"') ?*/;
                    return 1;
                /*- endif -*/
            /*- endfor -*/
            default:
                return -1;
        }
    /*- endif -*/
}

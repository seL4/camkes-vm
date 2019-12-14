/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#include <assert.h>
#include <camkes/error.h>
#include <stdio.h>
#include <stdint.h>
#include <sel4/sel4.h>
#include <sel4vm/guest_vm.h>
#include <sel4vmmplatsupport/drivers/pci_helper.h>
#include <sel4vmmplatsupport/ioports.h>

/*? macros.show_includes(me.instance.type.includes) ?*/

/*- set cons = configuration[me.parent.to_instance.name].get(me.parent.to_interface.name) -*/
/*- set cons = lambda('x: [] if x is None else x')(cons) -*/

/*- for con in cons -*/
    void /*? con['init'].strip('"') ?*/(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports);
    /*- if con['irq'] is not none -*/
        void /*? con['irq'].strip('"') ?*/(vm_t *vm);
    /*- endif -*/
/*- endfor -*/

int /*? me.interface.name ?*/_num_connections() {
    return /*? len(cons) ?*/;
}

uintptr_t /*? me.interface.name ?*/_init_function(int con) {
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

int /*? me.interface.name ?*/_has_interrupt(int con, uintptr_t *badge, uintptr_t *fun) {
    /*- if len(cons) == 0 -*/
        return -1;
    /*- else -*/
        switch (con) {
            /*- for con in cons -*/
            case /*? loop.index0 ?*/:
                /*- if con['irq'] is none -*/
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

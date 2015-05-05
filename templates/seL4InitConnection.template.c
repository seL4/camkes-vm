/*#
 *# Copyright 2014, NICTA
 *#
 *# This software may be distributed and modified according to the terms of
 *# the BSD 2-Clause license. Note that NO WARRANTY is provided.
 *# See "LICENSE_BSD2.txt" for details.
 *#
 *# @TAG(NICTA_BSD)
 #*/

#include <assert.h>
#include <camkes/error.h>
#include <stdio.h>
#include <stdint.h>
#include <sel4/sel4.h>
#include <vmm/vmm.h>

/*? macros.show_includes(me.from_instance.type.includes) ?*/

/*- set cons = configuration[me.to_instance.name].get(me.to_interface.name) -*/
/*- set cons = lambda('x: [] if x is None else x')(cons) -*/

/*- for con in cons -*/
    void /*? con['init'] ?*/(vmm_t *vmm);
    /*- if con['irq'] is not none -*/
        void /*? con['irq'] ?*/(vmm_t *vmm);
    /*- endif -*/
/*- endfor -*/

int /*? me.from_interface.name ?*/_num_connections() {
    return /*? len(cons) ?*/;
}

unsigned int /*? me.from_interface.name ?*/_init_function(int con) {
    /*- if len(cons) == 0 -*/
        return -1;
    /*- else -*/
        switch(con) {
        /*- for con in cons -*/
            case /*? loop.index0 ?*/:
                return (unsigned int)/*? con['init'] ?*/;
        /*- endfor -*/
            default:
                return -1;
        }
    /*- endif -*/
}

int /*? me.from_interface.name ?*/_has_interrupt(int con, unsigned int *badge, unsigned int *fun) {
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
                    *fun = (unsigned int)/*? con['irq'] ?*/;
                    return 1;
                /*- endif -*/
            /*- endfor -*/
            default:
                return -1;
        }
    /*- endif -*/
}

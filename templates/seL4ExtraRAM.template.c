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

/*? macros.show_includes(me.instance.type.includes) ?*/

/*- set config_uts = configuration[me.parent.to_instance.name].get(me.parent.to_interface.name) -*/
/*- set uts = [] -*/
/*- if config_uts is not none -*/
    /*- for paddr, size_bits in config_uts -*/
        /*- set cap = alloc('extra_ram_cap_%d' % paddr, seL4_UntypedObject, read=True, write=True, paddr = paddr, size_bits = size_bits) -*/
        /*- do uts.append( (paddr, size_bits, cap) ) -*/
    /*- endfor -*/
/*- endif -*/

int /*? me.interface.name ?*/_num_untypeds() {
    return /*? len(uts) ?*/;
}

int /*? me.interface.name ?*/_get_untyped(int ut, uintptr_t *paddr, int *size_bits, seL4_CPtr *cap) {
    /*- if len(uts) == 0 -*/
        return -1;
    /*- else -*/
        switch (ut) {
        /*- for paddr, size_bits, cap in uts -*/
        case /*? loop.index0 ?*/:
            *paddr = /*? paddr ?*/;
            *size_bits = /*? size_bits ?*/;
            *cap = /*? cap ?*/;
            break;
        /*- endfor -*/
        default:
            return -1;
        }
        return 0;
    /*- endif -*/
}

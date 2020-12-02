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

/*- set config_uts = configuration[me.name].get("ram") -*/
/*- set uts = [] -*/
/*- if config_uts is not none -*/
    /*- for paddr, size_bits in config_uts -*/
        /*- set cap = alloc('extra_ram_cap_%d' % paddr, seL4_UntypedObject, read=True, write=True, paddr = paddr, size_bits = size_bits) -*/
        /*- do uts.append( (paddr, size_bits, cap) ) -*/
    /*- endfor -*/
/*- endif -*/

int ram_num_untypeds() {
    return /*? len(uts) ?*/;
}

int ram_get_untyped(int ut, uintptr_t *paddr, int *size_bits, seL4_CPtr *cap) {
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

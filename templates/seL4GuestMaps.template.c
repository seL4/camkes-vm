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

/*? macros.show_includes(me.from_instance.type.includes) ?*/

/*- set bits_to_frame_type = { 12:seL4_FrameObject, 20:seL4_ARM_SectionObject, 21:seL4_ARM_SectionObject } -*/


/*- set config_guestmaps = configuration[me.to_instance.name].get(me.to_interface.name) -*/
/*- set gmaps = [] -*/
/*- if config_guestmaps is not none -*/
    /*- for gmap in config_guestmaps -*/
        /*- for frame_offset in range (0, gmap['size'], 2 ** gmap['page_bits']) -*/
            /*- set frame = gmap['paddr'] + frame_offset -*/
            /*- set object = alloc_obj('gmap_frame_%d' % frame, bits_to_frame_type[gmap['page_bits']], paddr=frame) -*/
            /*- set cap = alloc_cap('gmap_frame_%d' % frame, object, read=true, write=true) -*/

            /*- do gmaps.append( (gmap['paddr'],  2 ** gmap['page_bits'], gmap['page_bits'], frame, cap) ) -*/
        /*- endfor -*/
    /*- endfor -*/
/*- endif -*/

int /*? me.from_interface.name ?*/_num_guestmaps() {
    return /*? len(gmaps) ?*/;
}

int /*? me.from_interface.name ?*/_get_guest_map(int num, uint64_t *frame, uint64_t *size) {
    /*- if len(gmaps) == 0 -*/
        return 0;
    /*- else -*/
        switch (num) {
        /*- for paddr, size, pgbits, frame, cap in gmaps -*/
            case /*? loop.index0 ?*/:
                *frame = /*? frame ?*/;
                *size = /*? size ?*/;
                return 1;
        /*- endfor -*/
            default:
                return 0;
        }
    /*- endif -*/
}
seL4_CPtr /*? me.from_interface.name ?*/_get_mapping_mem_frame(uintptr_t paddr) {
    /*- if len(gmaps) == 0 -*/
        return 0;
    /*- else -*/
        switch(paddr) {
            /*- for pd, sz, pb, paddr, cap in gmaps -*/
                case /*? paddr ?*/:
                    return /*? cap ?*/;
            /*- endfor -*/
        default:
            return 0;
        }
    /*- endif -*/
}

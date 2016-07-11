/*#
 *# Copyright 2014, NICTA
 *#
 *# This software may be distributed and modified according to the terms of
 *# the BSD 2-Clause license. Note that NO WARRANTY is provided.
 *# See "LICENSE_BSD2.txt" for details.
 *#
 *# @TAG(NICTA_BSD)
 #*/

#include <sel4/sel4.h>

/*? macros.show_includes(me.from_instance.type.includes) ?*/

/*- set is_reader = False -*/
/*- set instance = me.from_instance.name -*/
/*- set interface = me.from_interface.name -*/
/*- include 'global-endpoint.template.c' -*/

/*- set aep = pop('notification') -*/

int /*? me.from_interface.name ?*/__run(void) {
    /* Nothing required. */
    return 0;
}

void /*? me.from_interface.name ?*/_emit_underlying(void) {
    seL4_Signal(/*? aep ?*/);
}

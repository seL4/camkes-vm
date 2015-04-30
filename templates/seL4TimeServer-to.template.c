/*#
 *# Copyright 2014, NICTA
 *#
 *# This software may be distributed and modified according to the terms of
 *# the BSD 2-Clause license. Note that NO WARRANTY is provided.
 *# See "LICENSE_BSD2.txt" for details.
 *#
 *# @TAG(NICTA_BSD)
 #*/

/*- include 'seL4RPCCall-to.template.c' -*/

/*# Look through the composition and find all '-to' connectors that would be
 *# duplicates of this one
 #*/
/*- set badges = [] -*/
/*- for id, c in enumerate(composition.connections) -*/
    /*- if c.to_instance.name == me.to_instance.name and c.to_interface.name == me.to_interface.name -*/
        /*- if c.type.name == me.type.name -*/
            /*- set is_reader = False -*/
            /*- set instance = c.from_instance.name -*/
            /*- set interface = c.from_interface.name -*/
            /*- include 'global-endpoint.template.c' -*/
            /*- set aep = pop('aep') -*/
            /*- set badge = configuration[c.from_instance.name].get("%s_attributes" % c.from_interface.name).strip('"') -*/
            void /*? me.to_interface.name ?*/_emit_/*? badge ?*/(void) {
                seL4_Notify(/*? aep ?*/, 0);
            }
            /*- do badges.append(badge) -*/
        /*- endif -*/
    /*- endif -*/
/*- endfor -*/

/*- do badges.sort() -*/

void /*? me.to_interface.name ?*/_emit(unsigned int badge) {
    /*# create a lookup table under the assumption that the
        badges are sensibly made as low as possible #*/
    static void (*lookup[])(void) = {
        /*- for badge in badges -*/
            [/*? badge ?*/] = /*? me.to_interface.name ?*/_emit_/*? badge ?*/,
        /*- endfor -*/
    };
    assert(badge < ARRAY_SIZE(lookup));
    assert(lookup[badge]);
    lookup[badge]();
}

int /*? me.to_interface.name ?*/_largest_badge(void) {
    return /*? badges[len(badges) - 1] ?*/;
}

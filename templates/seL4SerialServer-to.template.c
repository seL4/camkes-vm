/*#
 *# Copyright 2014, NICTA
 *#
 *# This software may be distributed and modified according to the terms of
 *# the BSD 2-Clause license. Note that NO WARRANTY is provided.
 *# See "LICENSE_BSD2.txt" for details.
 *#
 *# @TAG(NICTA_BSD)
 #*/

/*- include 'seL4RPCDataport-to.template.c' -*/

/*- set badges = [] -*/
/*- for c in me.parent.from_ends -*/
    /*- set is_reader = False -*/
    /*- set instance = c.instance.name -*/
    /*- set interface = c.interface.name -*/
    /*- include 'global-endpoint.template.c' -*/
    /*- set notification = pop('notification') -*/
    /*- set badge = configuration[c.instance.name].get("%s_attributes" % c.interface.name).strip('"') -*/
    void /*? me.interface.name ?*/_emit_/*? badge ?*/(void) {
        seL4_Signal(/*? notification ?*/);
    }
    /*- do badges.append(badge) -*/
/*- endfor -*/

/*- do badges.sort() -*/

void /*? me.interface.name ?*/_emit(unsigned int badge) {
    /*# create a lookup table under the assumption that the
        badges are sensibly made as low as possible #*/
    static void (*lookup[])(void) = {
        /*- for badge in badges -*/
            [/*? badge ?*/] = /*? me.interface.name ?*/_emit_/*? badge ?*/,
        /*- endfor -*/
    };
    assert(badge < ARRAY_SIZE(lookup));
    assert(lookup[badge]);
    lookup[badge]();
}

int /*? me.interface.name ?*/_largest_badge(void) {
    return /*? badges[len(badges) - 1] ?*/;
}

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

/*# Look through the composition and find all '-to' connectors that would be
 *# duplicates of this one
 #*/
/*- set badges = [] -*/
/*- set macs = [] -*/

/*- for c in me.parent.from_ends -*/
    /*- set is_reader = False -*/
    /*- set instance = c.instance.name -*/
    /*- set interface = c.interface.name -*/
    /*- include 'global-endpoint.template.c' -*/
    /*- set notification = pop('notification') -*/
    /*- set badge = configuration[c.instance.name].get("%s_attributes" % c.interface.name).strip('"') -*/
    /*- set mac = configuration[c.instance.name].get("%s_mac" % c.interface.name) -*/
    void /*? me.interface.name ?*/_emit_/*? badge ?*/(void) {
        seL4_Signal(/*? notification ?*/);
    }
    /*- do badges.append(badge) -*/
    /*- do macs.append( (badge, mac) ) -*/
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

void /*? me.interface.name ?*/_get_mac(unsigned int badge, uint8_t *mac) {
    /*- if len(macs) > 0 -*/
        switch (badge) {
            /*- for badge,mac in macs -*/
            case /*? badge ?*/: {
                uint8_t temp[] = {
                    /*- for num in mac -*/
                        /*? num ?*/,
                    /*- endfor -*/
                };
                memcpy(mac, temp, sizeof(temp));
                break;
            }
            /*- endfor -*/
        }
    /*- endif -*/
}

/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#include <sel4/sel4.h>
#include <string.h>
#include <lwip/ip_addr.h>

/*? macros.show_includes(me.instance.type.includes) ?*/

/*- set ep = alloc('ep', seL4_EndpointObject, write=True, grant=True) -*/
/*- set badge = configuration[me.instance.name].get('%s_attributes' % me.interface.name) -*/
/*- if badge is not none -*/
    /*- set badge = badge.strip('"') -*/
    /*- do cap_space.cnode[ep].set_badge(int(badge, 0)) -*/
/*- endif -*/

/* assume a dataport symbols exists */
extern void * /*? me.interface.name?*/_buf;

int /*? me.interface.name ?*/_send(void *p, unsigned int len, ip_addr_t addr) {
    seL4_SetMR(0, len);
    seL4_SetMR(1, addr.addr);
    if (len > 4096) {
        len = 4096;
    }
    memcpy(/*? me.interface.name?*/_buf, p, len);
    seL4_Call(/*? ep ?*/, seL4_MessageInfo_new(0, 0, 0, 2));
    return len;
}

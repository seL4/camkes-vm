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
#include <camkes/dataport.h>
#include <lwip/udp.h>
#include <utils/util.h>

/*- set ep = alloc('ep', seL4_EndpointObject, write=True, grant=True) -*/

/*- set badge = configuration[me.from_instance.name].get('%s_attributes' % me.from_interface.name) -*/
/*- if badge is not none -*/
    /*- set badge = badge.strip('"') -*/
    /*- do cap_space.cnode[ep].set_badge(int(badge, 0)) -*/
/*- endif -*/

int /*? me.from_interface.name ?*/_poll(unsigned int *len, uint16_t *port, ip_addr_t *addr) {
    int status;
    seL4_MessageInfo_t UNUSED info;
    info = seL4_Call(/*? ep ?*/, seL4_MessageInfo_new(0, 0, 0, 0));
    assert(seL4_MessageInfo_get_length(info) > 0);
    status = seL4_GetMR(0);
    if (status != -1) {
        *len = seL4_GetMR(1);
        *port = seL4_GetMR(2);
        *addr =  (ip_addr_t){.addr = seL4_GetMR(3)};
        assert(*len < 4096);
    }
    return status;
}


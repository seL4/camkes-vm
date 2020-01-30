/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

/*- set suffix = "_buf" -*/
/*- include 'seL4MultiSharedData-from.template.c' -*/

#include <sel4/sel4.h>
#include <camkes/dataport.h>
#include <lwip/udp.h>
#include <utils/util.h>

/*- set ep = alloc('ep', seL4_EndpointObject, write=True, grantreply=True) -*/

/*- set badge = configuration[me.instance.name].get('%s_attributes' % me.interface.name) -*/
/*- if badge is not none -*/
    /*- set badge = badge.strip('"') -*/
    /*- do cap_space.cnode[ep].set_badge(int(badge, 0)) -*/
/*- endif -*/

int /*? me.interface.name ?*/_poll(unsigned int *len, uint16_t *port, ip_addr_t *addr) {
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

/*- set is_reader = True -*/
/*- set instance = me.instance.name -*/
/*- set interface = me.interface.name -*/
/*- include 'global-endpoint.template.c' -*/
/*- set notification = pop('notification') -*/

seL4_CPtr /*? me.interface.name ?*/_notification(void) {
    return /*? notification ?*/;
}

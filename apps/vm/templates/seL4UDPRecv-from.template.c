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

/*- set ep = alloc('ep', seL4_EndpointObject, write=True, grant=True) -*/
/*- set aep = alloc('aep', seL4_AsyncEndpointObject, read=True) -*/

/* Actual dataport is emitted in the per-component template. */
/*- set p = Perspective(dataport=me.from_interface.name) -*/
extern char /*? p['dataport_symbol'] ?*/[ROUND_UP_UNSAFE(sizeof(/*? show(me.from_interface.type) ?*/), PAGE_SIZE_4K)];
extern volatile /*? show(me.from_interface.type) ?*/ * /*? me.from_interface.name ?*/;

/* Prototype for user supplied function for processing received packets */
void /*? me.from_interface.name ?*/_recv(void *p, unsigned int len, uint16_t port, ip_addr_t addr);

int /*? me.from_interface.name ?*/__run(void) {
    while(1) {
        int status;
        /* wait on the async endpoint for packet notifications */
        seL4_Wait(/*? aep ?*/, NULL);
        /* receive packets until we have them all */
        status = 0;
        while (status == 0) {
            seL4_MessageInfo_t info;
            info = seL4_Call(/*? ep ?*/, seL4_MessageInfo_new(0, 0, 0, 0));
            assert(seL4_MessageInfo_get_length(info) > 0);
            status = seL4_GetMR(0);
            if (status != -1) {
                /*? me.from_interface.name ?*/_recv(/*? p['dataport_symbol'] ?*/, seL4_GetMR(1), seL4_GetMR(2), (ip_addr_t){.addr = seL4_GetMR(3)});
            }
        }
    }
    /* Nothing required. */
    return 0;
}

int /*? me.from_interface.name ?*/_wrap_ptr(dataport_ptr_t *p, void *ptr) {
    /* should not be used */
    return 0;
}

void * /*? me.from_interface.name ?*/_unwrap_ptr(dataport_ptr_t *p) {
    /* should not be used */
    return NULL;
}


/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#include <camkes/dataport.h>
#include <sel4/sel4.h>
#include <lwip/udp.h>
#include <string.h>

/*? macros.show_includes(me.from_instance.type.includes) ?*/

/*- set ep = alloc('ep', seL4_EndpointObject, write=True, grant=True) -*/

/* Actual dataport is emitted in the per-component template. */
/*- set p = Perspective(dataport=me.from_interface.name) -*/
char /*? p['dataport_symbol'] ?*/[ROUND_UP_UNSAFE(sizeof(/*? show(me.from_interface.type) ?*/), PAGE_SIZE_4K)]
    __attribute__((aligned(PAGE_SIZE_4K)))
    __attribute__((section("shared_/*? me.from_interface.name ?*/")))
    __attribute__((externally_visible));
volatile /*? show(me.from_interface.type) ?*/ * /*? me.from_interface.name ?*/ = (volatile /*? show(me.from_interface.type) ?*/ *) /*? p['dataport_symbol'] ?*/;

void /*? me.from_interface.name ?*/_send(void *p, unsigned int len, ip_addr_t addr, uint16_t port) {
    seL4_SetMR(0, len);
    seL4_SetMR(1, addr.addr);
    seL4_SetMR(2, port);
    memcpy(/*? p['dataport_symbol'] ?*/, p, len);
    seL4_Call(/*? ep ?*/, seL4_MessageInfo_new(0, 0, 0, 3));
}

int /*? me.from_interface.name ?*/_wrap_ptr(dataport_ptr_t *p, void *ptr) {
    /* should not be used */
    return -1;
}

void * /*? me.from_interface.name ?*/_unwrap_ptr(dataport_ptr_t *p) {
    /* should not be used */
    return NULL;
}


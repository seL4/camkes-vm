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
#include <sync/sem-bare.h>
#include <string.h>

/*- set ep = alloc('ep', seL4_EndpointObject, read=True, write=True) -*/

/* Actual dataport is emitted in the per-component template. */
/*- set p = Perspective(dataport=me.to_interface.name) -*/
char /*? p['dataport_symbol'] ?*/[ROUND_UP_UNSAFE(sizeof(/*? show(me.to_interface.type) ?*/), PAGE_SIZE_4K)]
    __attribute__((aligned(PAGE_SIZE_4K)))
    __attribute__((section("shared_/*? me.to_interface.name ?*/")))
    __attribute__((externally_visible));
volatile /*? show(me.to_interface.type) ?*/ * /*? me.to_interface.name ?*/ = (volatile /*? show(me.to_interface.type) ?*/ *) /*? p['dataport_symbol'] ?*/;

/*- set attributes = [] -*/
/*- for s in configuration.settings -*/
    /*- if s.instance == me.to_instance.name -*/
        /*- if s.attribute == "%s_attributes" % (me.to_interface.name) -*/
            /*- set port = s.value.strip('"') -*/
            /*- set port = int(port, 0) -*/
            /*- do attributes.append(port) -*/
        /*- endif -*/
    /*- endif -*/
/*- endfor -*/

void lwip_lock();
void lwip_unlock();

static struct udp_pcb *upcb;

void /*? me.to_interface.name ?*/__run(void) {
    while(1) {
        /*- set cnode = alloc_cap('cnode', my_cnode, write=True) -*/
        /*- set reply_cap_slot = alloc_cap('reply_cap_slot', None) -*/
        int result UNUSED;
        unsigned int len;
        ip_addr_t addr;
        uint16_t port;
        struct pbuf *p;
        seL4_Wait(/*? ep ?*/, NULL);
        result = seL4_CNode_SaveCaller(/*? cnode ?*/, /*? reply_cap_slot ?*/, 32);
        assert(result == seL4_NoError);

        len = seL4_GetMR(0);
        addr.addr = seL4_GetMR(1);
        port = seL4_GetMR(2);
        lwip_lock();
        p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
        memcpy(p->payload, /*? p['dataport_symbol'] ?*/, len);
        udp_sendto(upcb, p, &addr, port);
        pbuf_free(p);
        lwip_unlock();
        seL4_Send(/*? reply_cap_slot ?*/, seL4_MessageInfo_new(0, 0, 0, 0));
    }
}

void /*? me.to_interface.name ?*/__init(void) {
    lwip_lock();
    upcb = udp_new();
    assert(upcb);
    /* we cheat here and set a local port without using the actual lwip bind function.
     * this is because we want to persuade lwip to send packets with this as the from
     * port, but we don't actually want to receive packets here */
    upcb->local_port = /*? attributes[0] ?*/;
    lwip_unlock();
}

int /*? me.to_interface.name ?*/_wrap_ptr(dataport_ptr_t *p, void *ptr) {
    return 0;
}

void * /*? me.to_interface.name ?*/_unwrap_ptr(dataport_ptr_t *p) {
    return NULL;
}


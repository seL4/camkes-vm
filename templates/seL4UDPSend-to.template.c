/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/*- include 'seL4MultiSharedData-to.template.c' -*/

#include <sel4/sel4.h>
#include <camkes/dataport.h>
#include <lwip/udp.h>
#include <sync/sem-bare.h>
#include <string.h>

/*- from 'rpc-connector.c' import allocate_badges with context -*/

/*- set client_ids = namespace() -*/
/*- do allocate_badges(client_ids) -*/

/*- set badges = client_ids.badges -*/

/*- set ep = alloc('ep', seL4_EndpointObject, read=True, write=True) -*/

/*- set clients = [] -*/

/*- for c in me.parent.from_ends -*/

    /*- set ports = configuration[c.instance.name].get('%s_ports' % c.interface.name) -*/
    /*- set client = badges[loop.index0] -*/
    /*- do clients.append( (client, ports['source'], ports['dest']) ) -*/

/*- endfor -*/

void lwip_lock();
void lwip_unlock();

static struct udp_pcb *upcb[/*? len(clients) ?*/];

void /*? me.interface.name ?*/__run(void) {
    while(1) {
        /*- set cnode = alloc_cap('cnode', my_cnode, write=True) -*/
        /*- set reply_cap_slot = alloc_cap('reply_cap_slot', None) -*/
        int result UNUSED;
        unsigned int len;
        ip_addr_t addr;
        struct pbuf *p;
        seL4_Word badge;
        seL4_Wait(/*? ep ?*/, &badge);
        result = seL4_CNode_SaveCaller(/*? cnode ?*/, /*? reply_cap_slot ?*/, CONFIG_WORD_SIZE);
        assert(result == seL4_NoError);

        len = seL4_GetMR(0);
        if (len < 4096) {
            addr.addr = seL4_GetMR(1);
            lwip_lock();
            p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
            if (p) {
                memcpy(p->payload, /*? me.interface.name?*/_buf(badge), len);
                switch (badge) {
                /*- for client, source, dest in clients -*/
                case /*? client ?*/:
                    udp_sendto(upcb[/*? loop.index0 ?*/], p, &addr, /*? dest ?*/);
                    break;
                /*- endfor -*/
                }
                pbuf_free(p);
            }
            lwip_unlock();
        }
        seL4_Send(/*? reply_cap_slot ?*/, seL4_MessageInfo_new(0, 0, 0, 0));
    }
}

void /*? me.interface.name ?*/__init(void) {
    lwip_lock();
    /*- for client, source, dest in clients -*/
        upcb[/*? loop.index0 ?*/] = udp_new();
        assert(upcb[/*? loop.index0 ?*/]);
        /* we cheat here and set a local port without using the actual lwip bind function.
         * this is because we want to persuade lwip to send packets with this as the from
         * port, but we don't actually want to receive packets here */
        upcb[/*? loop.index0 ?*/]->local_port = /*? source ?*/;
    /*- endfor -*/
    lwip_unlock();
}

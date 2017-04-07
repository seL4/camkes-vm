/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#include <sel4/sel4.h>
#include <camkes/dataport.h>
#include <sync/sem-bare.h>
#include <string.h>
#include <pico_socket.h>
#include <pico_addressing.h>
#include <pico_stack.h>
#include <sel4utils/sel4_zf_logif.h>

#define MAX_UDP_PACKET 4096

/*- set ep = alloc('ep', seL4_EndpointObject, read=True, write=True) -*/

/* assume a function exists to get a dataport */
void * /*? me.interface.name?*/_buf_buf(unsigned int client_id);

/*- set clients = [] -*/

/*- for c in me.parent.from_ends -*/

    /*- set ports = configuration[c.instance.name].get('%s_ports' % c.interface.name) -*/
    /*- set client = configuration[c.instance.name].get('%s_attributes' % c.interface.name) -*/
    /*- set client = client.strip('"') -*/
    /*- do clients.append( (client, ports['source'], ports['dest']) ) -*/

/*- endfor -*/

void picotcp_lock();
void picotcp_unlock();

static struct pico_socket *socks[/*? len(clients) ?*/];

static void udpsend_cb(uint16_t ev, struct pico_socket *s){
    /* We only need this to create the socket. For now it is just a dummy cb provided and unused. */
}

void /*? me.interface.name ?*/__run(void) {
    while(1) {
        /*- set cnode = alloc_cap('cnode', my_cnode, write=True) -*/
        /*- set reply_cap_slot = alloc_cap('reply_cap_slot', None) -*/
        int result UNUSED;
        unsigned int len;
        struct pico_ip4 addr;
        seL4_Word badge;
        seL4_Wait(/*? ep ?*/, &badge);
        int ret;
        result = seL4_CNode_SaveCaller(/*? cnode ?*/, /*? reply_cap_slot ?*/, 32);
        ZF_LOGF_IFERR(result != seL4_NoError, "seL4 failed to save caller.");

        len = seL4_GetMR(0);
        if (len < MAX_UDP_PACKET) {
            addr.addr = seL4_GetMR(1);
            picotcp_lock();
          
            switch (badge) {
            /*- for client, source, dest in clients -*/
            case /*? client ?*/:

                if (socks[/*? loop.index0 ?*/] == NULL){
                    // retry open
                    socks[/*? loop.index0 ?*/] = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_UDP, &udpsend_cb);
                    ZF_LOGE_IF(socks[/*? loop.index0 ?*/] == NULL, "Warning: Attempt to reopen socket failed");
                    break;
                }
  
                // Set the local port so the packet of the socket to what is set in the camkes component config. 
                socks[/*? loop.index0 ?*/]->local_port = short_be(/*? source ?*/);
                ret = pico_socket_sendto(socks[/*? loop.index0 ?*/], /*? me.interface.name ?*/_buf_buf(badge), len, &addr, short_be(/*? dest ?*/));
                pico_stack_tick();
                break;
            /*- endfor -*/
            }
         
            picotcp_unlock();
        }
        seL4_Send(/*? reply_cap_slot ?*/, seL4_MessageInfo_new(0, 0, 0, 0));
    }
}

void /*? me.interface.name ?*/__init(void) {
    picotcp_lock();
    /*- for client, source, dest in clients -*/
        socks[/*? loop.index0 ?*/] = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_UDP, &udpsend_cb);
        ZF_LOGE_IF(socks[/*? loop.index0 ?*/] == NULL, "Failed to open socket %d", /*? loop.index0 ?*/);
        /* we cheat here and set a local port without using the actual bind function.
         * this is because we want to persuade picotcp to send packets with this as the from
         * port, but we don't actually want to receive packets here */
        socks[/*? loop.index0 ?*/]->local_port = short_be(/*? source ?*/);
    /*- endfor -*/
    picotcp_unlock();
}

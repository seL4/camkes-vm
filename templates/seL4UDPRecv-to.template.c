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
/*- set aep = alloc('aep', seL4_AsyncEndpointObject, write=True) -*/

/* Actual dataport is emitted in the per-component template. */
/*- set p = Perspective(dataport=me.to_interface.name) -*/
char /*? p['dataport_symbol'] ?*/[ROUND_UP_UNSAFE(sizeof(/*? show(me.to_interface.type) ?*/), PAGE_SIZE_4K)]
    __attribute__((aligned(PAGE_SIZE_4K)))
    __attribute__((section("shared_/*? me.to_interface.name ?*/")))
    __attribute__((externally_visible));
volatile /*? show(me.to_interface.type) ?*/ * /*? me.to_interface.name ?*/ = (volatile /*? show(me.to_interface.type) ?*/ *) /*? p['dataport_symbol'] ?*/;

void lwip_lock();
void lwip_unlock();

/*- set attributes = [] -*/
/*- for s in configuration.settings -*/
    /*- if s.instance == me.to_instance.name -*/
        /*- if s.attribute == "%s_attributes" % (me.to_interface.name) -*/
            /*- set bufs,port = s.value.strip('"').split(',') -*/
            /*- set bufs = int(bufs, 0) -*/
            /*- set port = int(port, 0) -*/
            /*- do attributes.append(bufs) -*/
            /*- do attributes.append(port) -*/
        /*- endif -*/
    /*- endif -*/
/*- endfor -*/

typedef struct udp_message {
    struct pbuf *pbuf;
    ip_addr_t addr;
    uint16_t port;
    struct udp_message *next;
}udp_message_t;

static struct udp_pcb *upcb = NULL;
static udp_message_t message_memory[/*? attributes[0] ?*/] = {
    /*- for i in range(attributes[0]) -*/
        /*- if i == 0 -*/
            {.pbuf = NULL, .port = 0, .next = NULL},
        /*- else -*/
            {.pbuf = NULL, .port = 0, .next = &message_memory[/*? i - 1 ?*/]},
        /*- endif -*/
    /*- endfor -*/
    };
static udp_message_t *free_head = &message_memory[/*? attributes[0] - 1 ?*/];
static udp_message_t *used_head = NULL;

static int need_signal = 1;

static void udprecv(void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port) {
    lwip_lock();
    if (!free_head) {
        pbuf_free(p);
        lwip_unlock();
        return;
    }
    udp_message_t *m = free_head;
    free_head = free_head->next;

    m->pbuf = p;
    m->addr = *addr;
    m->port = port;

    if (need_signal) {
        seL4_Notify(/*? aep ?*/, 0);
        need_signal = 0;
    }

    m->next = used_head;
    used_head = m;
    lwip_unlock();
}

void /*? me.to_interface.name ?*/__run(void) {
    while (1) {
        /*- set cnode = alloc_cap('cnode', my_cnode, write=True) -*/
        /*- set reply_cap_slot = alloc_cap('reply_cap_slot', None) -*/
        int len;
        int result UNUSED;
        seL4_Wait(/*? ep ?*/, NULL);
        result = seL4_CNode_SaveCaller(/*? cnode ?*/, /*? reply_cap_slot ?*/, 32);
        assert(result == seL4_NoError);
        lwip_lock();
        len = 0;
        if (!used_head) {
            seL4_SetMR(0, -1);
            len = 1;
            need_signal = 1;
        } else {
            unsigned int packet_len = 0;
            void *p = /*? p['dataport_symbol'] ?*/;
            udp_message_t *m = used_head;
            used_head = used_head->next;
            if (!used_head) {
                need_signal = 1;
            }

            for (struct pbuf *q = m->pbuf; q; q = q->next) {
                memcpy(p + packet_len, q->payload, q->len);
                packet_len += q->len;
            }
            pbuf_free(m->pbuf);
            seL4_SetMR(0, used_head ? 0 : 1);
            seL4_SetMR(1, packet_len);
            seL4_SetMR(2, m->port);
            seL4_SetMR(3, m->addr.addr);
            len = 4;

            m->next = free_head;
            free_head = m;
        }
        seL4_Send(/*? reply_cap_slot ?*/, seL4_MessageInfo_new(0, 0, 0, len));
        lwip_unlock();
    }
}

void /*? me.to_interface.name ?*/__init(void) {
    int err;
    lwip_lock();
    upcb = udp_new();
    assert(upcb);
    udp_recv(upcb, udprecv, NULL);
    err = udp_bind(upcb, NULL, 7);
    assert(!err);
    lwip_unlock();
}

int /*? me.to_interface.name ?*/_wrap_ptr(dataport_ptr_t *p, void *ptr) {
    /* should not be used */
    return -1;
}

void * /*? me.to_interface.name ?*/_unwrap_ptr(dataport_ptr_t *p) {
    /* should not be used */
    return NULL;
}


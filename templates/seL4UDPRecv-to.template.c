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
/*- from 'global-endpoint.template.c' import allocate_cap with context -*/

/*- set client_ids = namespace() -*/
/*- do allocate_badges(client_ids) -*/

/*- set badges = client_ids.badges -*/

/*- set ep = alloc('ep', seL4_EndpointObject, read=True, write=True) -*/

void lwip_lock();
void lwip_unlock();

/*- set bufs = configuration[me.instance.name].get('num_client_recv_bufs') -*/
/*- set clients = [] -*/

/*- for c in me.parent.from_ends -*/

    /*- set port = configuration[c.instance.name].get('%s_port' % c.interface.name) -*/
    /*- set client = badges[loop.index0] -*/
    /*- do allocate_cap(c, is_reader=False) -*/
    /*- set notification = pop('notification') -*/

    /*- do clients.append( (client, port, notification) ) -*/
/*- endfor -*/

typedef struct udp_message {
    struct pbuf *pbuf;
    ip_addr_t addr;
    uint16_t port;
    struct udp_message *next;
}udp_message_t;

typedef struct udp_client {
    struct udp_pcb *upcb;
    int client_id;
    uint16_t port;
    int need_signal;
    seL4_CPtr notification;
    udp_message_t *free_head;
    udp_message_t *used_head;
    udp_message_t *used_tail;
    udp_message_t message_memory[ /*? bufs ?*/];
} udp_client_t;

static udp_client_t udp_clients[/*? len(clients) ?*/] = {
/*- for client,port,notification in clients -*/
    {.upcb = NULL, .client_id = /*? client ?*/, .port = /*? port ?*/, .need_signal = 1, .notification = /*? notification ?*/, .used_head = NULL},
/*- endfor -*/
};

static void udprecv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    udp_client_t *client = (udp_client_t*)arg;
    if (!client->free_head) {
        pbuf_free(p);
        return;
    }
    udp_message_t *m = client->free_head;
    client->free_head = client->free_head->next;

    m->pbuf = p;
    m->addr = *addr;
    m->port = port;
    m->next = NULL;

    if (client->need_signal) {
        seL4_Signal(client->notification);
        client->need_signal = 0;
    }

    if (!client->used_head) {
        client->used_head = client->used_tail = m;
    } else {
        client->used_tail->next = m;
        client->used_tail = m;
    }
}

void /*? me.interface.name ?*/__run(void) {
    while (1) {
        /*- set cnode = alloc_cap('cnode', my_cnode, write=True) -*/
        /*- set reply_cap_slot = alloc_cap('reply_cap_slot', None) -*/
        int len;
        int result UNUSED;
        seL4_Word badge;
        seL4_Wait(/*? ep ?*/, &badge);
        udp_client_t *client = NULL;
        for (int i = 0; i < /*? len(clients) ?*/ && !client; i++) {
            if (udp_clients[i].client_id == badge) {
                client = &udp_clients[i];
            }
        }
        assert(client);
        result = seL4_CNode_SaveCaller(/*? cnode ?*/, /*? reply_cap_slot ?*/, CONFIG_WORD_SIZE);
        assert(result == seL4_NoError);
        lwip_lock();
        len = 0;
        if (!client->used_head) {
            seL4_SetMR(0, -1);
            len = 1;
            client->need_signal = 1;
        } else {
            unsigned int packet_len = 0;
            void *p = /*? me.interface.name ?*/_buf(badge);
            udp_message_t *m = client->used_head;
            client->used_head = client->used_head->next;
            if (!client->used_head) {
                client->need_signal = 1;
            }

            for (struct pbuf *q = m->pbuf; q; q = q->next) {
                memcpy(p + packet_len, q->payload, q->len);
                packet_len += q->len;
            }
            pbuf_free(m->pbuf);
            seL4_SetMR(0, client->used_head ? 0 : 1);
            seL4_SetMR(1, packet_len);
            seL4_SetMR(2, m->port);
            seL4_SetMR(3, m->addr.addr);
            len = 4;

            m->next = client->free_head;
            client->free_head = m;
        }
        seL4_Send(/*? reply_cap_slot ?*/, seL4_MessageInfo_new(0, 0, 0, len));
        lwip_unlock();
    }
}

void /*? me.interface.name ?*/__init(void) {
    int UNUSED err;
    int i, j;
    lwip_lock();
    for (i = 0; i < /*? len(clients) ?*/; i++) {
        for (j = 0; j < /*? bufs ?*/; j++) {
            if (j == 0) {
                udp_clients[i].message_memory[j] =
                    (udp_message_t){.pbuf = NULL, .port = 0, .next = NULL};
            } else {
                udp_clients[i].message_memory[j] =
                    (udp_message_t){.pbuf = NULL, .port = 0, .next = &udp_clients[i].message_memory[j - 1]};
            }
        }
        udp_clients[i].free_head = &udp_clients[i].message_memory[/*? bufs ?*/ - 1];
        udp_clients[i].upcb = udp_new();
        assert(udp_clients[i].upcb);
        udp_recv(udp_clients[i].upcb, udprecv, &udp_clients[i]);
        err = udp_bind(udp_clients[i].upcb, NULL, udp_clients[i].port);
        assert(!err);
    }
    lwip_unlock();
}

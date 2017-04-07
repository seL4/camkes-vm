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
#include <sel4utils/sel4_zf_logif.h>
#include <camkes/dataport.h>
#include <pico_socket.h>
#include <pico_addressing.h>
#include <pico_ipv4.h>
#include <sync/sem-bare.h>
#include <string.h>
#include <assert.h>

#define MAX_UDP_PACKET 4096

/*- set ep = alloc('ep', seL4_EndpointObject, read=True, write=True) -*/

/* Assume a function exists to get a dataport */
void * /*? me.interface.name?*/_buf_buf(unsigned int id);

void picotcp_lock();
void picotcp_unlock();

/*- set bufs = configuration[me.instance.name].get('num_client_recv_bufs') -*/
/*- set clients = [] -*/

/*- for c in me.parent.from_ends -*/

    /*- set port = configuration[c.instance.name].get('%s_port' % c.interface.name) -*/
    /*- set client = configuration[c.instance.name].get('%s_attributes' % c.interface.name) -*/
    /*- set client = client.strip('"') -*/

    /*- set is_reader = False -*/
    /*- set instance = c.instance.name -*/
    /*- set interface = c.interface.name -*/
    /*- include 'global-endpoint.template.c' -*/
    /*- set notification = pop('notification') -*/

    /*- do clients.append( (client, port, notification) ) -*/
/*- endfor -*/

typedef struct udp_message {
    char message_buf[MAX_UDP_PACKET];
    struct pico_ip4 addr;
    uint16_t port;
    int len;
    struct udp_message *next;
}udp_message_t;

typedef struct udp_client {
    struct pico_socket *sock;
    int client_id;
    uint16_t port;
    int need_signal;
    int socket_open;
    seL4_CPtr notification;
    udp_message_t *free_head;
    udp_message_t *used_head;
    udp_message_t *used_tail;
    udp_message_t message_memory[ /*? bufs ?*/];
} udp_client_t;

static udp_client_t udp_clients[/*? len(clients) ?*/] = {
/*- for client,port,notification in clients -*/
    {.sock = NULL, .client_id = /*? client ?*/, .port = /*? port ?*/, .need_signal = 1, .socket_open = 1, .notification = /*? notification ?*/, .used_head = NULL},
/*- endfor -*/
};

static void udprecv_cb(uint16_t ev, struct pico_socket *s){

    if (ev == PICO_SOCK_EV_RD){
        /* Need to find the client in the array */
        udp_client_t *client = NULL; 
        for (int i=0; i</*? len(clients) ?*/; i++){
            if (udp_clients[i].sock == s){
                client = &udp_clients[i];
            }
        }
        
        // If you get to this stage, the UDP callback has been triggered from a socket that is not ours. 
        // All of our sockets were initialised below in _init. 
        ZF_LOGF_IF(client == NULL, "No such client. Callback received from an unregistered socket.");
            
        int r=0;
        do {
            udp_message_t *m = client->free_head; 
            client->free_head = client->free_head->next; 
            m->next = NULL;

            r = pico_socket_recvfrom(s, m->message_buf, MAX_UDP_PACKET, &m->addr.addr, &m->port);
            if (r <= 0){
                /* Return the buffer to the pool */
                m->next = client->free_head;
                client->free_head = m;
                break; 
            }
            m->len = r;

            if (client->need_signal){
                seL4_Signal(client->notification);
                client->need_signal = 0;
            }

            if (!client->used_head) {
                client->used_head = client->used_tail = m;
            } else {
                client->used_tail->next = m;
                client->used_tail = m;
            }

        } while (r > 0);
   }


}

void /*? me.interface.name ?*/__run(void) {
    while (1) {
        /*- set cnode = alloc_cap('cnode', my_cnode, write=True) -*/
        /*- set reply_cap_slot = alloc_cap('reply_cap_slot', None) -*/
        int len;
        int result UNUSED;
        int err;
        seL4_Word badge;
        seL4_Wait(/*? ep ?*/, &badge);
        udp_client_t *client = NULL;
        for (int i = 0; i < /*? len(clients) ?*/ && !client; i++) {
            if (udp_clients[i].client_id == badge) {
                client = &udp_clients[i];
            }
        }
        ZF_LOGF_IF(client == NULL, "No such client");
        result = seL4_CNode_SaveCaller(/*? cnode ?*/, /*? reply_cap_slot ?*/, 32);
        ZF_LOGF_IFERR(result != seL4_NoError, "seL4 failed to save caller.");
        
        // Check if the socket is not open. If it is not, retry opening.
        if (client->sock == NULL){
            client->sock = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_UDP, &udprecv_cb);
            if(client->sock == NULL){
                ZF_LOGE("Warning: socket failed to open");
                seL4_SetMR(0, -1);
                len = 1;
                seL4_Send(/*? reply_cap_slot ?*/, seL4_MessageInfo_new(0, 0, 0, len));
                return;
            }
        }
        
        // Check if the socket is bound correctly. If not, retry here. 
        if (client->socket_open == 0){
            // This is if the socket failed to bind
            struct pico_ip4 ip; 
            ip.addr = PICO_IPV4_INADDR_ANY; 
            uint16_t pico_port = short_be(client->port);
            err = pico_socket_bind(client->sock, &ip, &pico_port);
            if (err == -1){
                ZF_LOGE("Warning: socket failed to bind");
                seL4_SetMR(0, -1);
                len = 1;
                seL4_Send(/*? reply_cap_slot ?*/, seL4_MessageInfo_new(0, 0, 0, len));
                return;
            } else {
                client->socket_open = 1;
            }
        }

        picotcp_lock();
        len = 0;
        if (!client->used_head) {
            seL4_SetMR(0, -1);
            len = 1;
            client->need_signal = 1;
        } else {
            void *p = /*? me.interface.name ?*/_buf_buf(badge);
            udp_message_t *m = client->used_head;
            client->used_head = client->used_head->next;
            if (!client->used_head) {
                client->need_signal = 1;
            }

            /* Copy a message across */
            memcpy(p, m->message_buf, m->len);
            seL4_SetMR(0, client->used_head ? 0 : 1);
            seL4_SetMR(1, m->len);
            seL4_SetMR(2, m->port);
            seL4_SetMR(3, m->addr.addr);
            len = 4;

            m->next = client->free_head;
            client->free_head = m;
        }
        seL4_Send(/*? reply_cap_slot ?*/, seL4_MessageInfo_new(0, 0, 0, len));
        picotcp_unlock();
    }
}

void /*? me.interface.name ?*/__init(void) {
    int err;
    int i, j;
    picotcp_lock();
    for (i = 0; i < /*? len(clients) ?*/; i++) {
        for (j = 0; j < /*? bufs ?*/; j++) {
            if (j == 0) {
                udp_clients[i].message_memory[j] =
                    (udp_message_t){.message_buf = {0}, .port = 0, .next = NULL};
            } else {
                udp_clients[i].message_memory[j] =
                    (udp_message_t){.message_buf = {0}, .port = 0, .next = &udp_clients[i].message_memory[j - 1]};
            }
        }
        udp_clients[i].free_head = &udp_clients[i].message_memory[/*? bufs ?*/ - 1];
        udp_clients[i].sock = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_UDP, &udprecv_cb);
        if (udp_clients[i].sock == NULL){
            // Mark the socket as unopened.
            ZF_LOGE("Warning: socket no %d failed to open", i);
            udp_clients[i].socket_open = 0;
        } else {
            struct pico_ip4 ip; 
            ip.addr = PICO_IPV4_INADDR_ANY; 
            uint16_t pico_port = short_be(udp_clients[i].port);
            err = pico_socket_bind(udp_clients[i].sock, &ip, &pico_port);
            if (err == -1){
                // Mark the socket as unopened.
                ZF_LOGE("Warning: socket no %d failed to bind", i);
                udp_clients[i].socket_open = 0;
            }
        }
    }
    picotcp_unlock();
}

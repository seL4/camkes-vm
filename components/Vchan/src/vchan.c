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

#include <stdio.h>
#include <stdint.h>
#include <autoconf.h>

#include <sel4/sel4.h>
#include <sel4utils/util.h>

#include <camkes.h>

#include "sel4vm/vchan_copy.h"
#include "sel4vm/vchan_sharemem.h"
#include "vm.h"
#include "sel4vm/debug.h"

#include "sel4vm/vmm_manager.h"

/*
    State needed to represent a vchan instance in the vmm
*/
typedef struct vchan_connection {
    uint32_t domx, domy, port;
    uint32_t client_connected, server_connected;

    vchan_shared_mem_t *buffers;
    struct vchan_connection *next;
} vchan_instance_t;


static void init_buffer(void);
static void clear_buf(vchan_buf_t *b);
static void print_vchan_instances();

static int new_vchan_instance(vchan_connect_t *con);
static void rem_vchan_instance(vchan_instance_t *inst);
static int vchan_status(uint32_t domx, uint32_t domy, uint32_t port);

static vchan_shared_mem_t *alloc_buffer(void);

static vchan_buf_t *get_dom_buf(uint32_t buf, vchan_shared_mem_t *b);
static vchan_shared_mem_t *get_buffer(uint32_t id);
static vchan_instance_t *get_vchan_instance(uint32_t domx, uint32_t domy, uint32_t port);

static vchan_instance_t *first_inst = NULL;
static vchan_headers_t *headers = NULL; /* Shared dataport struct for intervm communication  */

void pre_init(void) {
    headers = (vchan_headers_t *) share_mem;
    headers->token = VCHAN_DATA_TOKEN;
    init_buffer();
}

static void clear_buf(vchan_buf_t *b) {
    assert(b != NULL);
    b->owner = -1;
    b->filled = 0;
    b->read_pos = 0;
    b->write_pos = 0;
}

/*
    Sets up initial values for data copying
*/
static void init_buffer(void) {
    int x, y;
    vchan_shared_mem_t *m;
    for(x = 0; x < NUM_BUFFERS; x++) {
        m = &headers->shared_buffers[x];
        m->alloced = 0;
        for(y = 0; y < 2; y++) {
            clear_buf(&m->bufs[y]);
        }
    }
}

int vchan_com_new_connection(vchan_connect_t con) {
    int res;
    res = new_vchan_instance(&con);
    return res;
}

/*
    Disconnect or destroy a given vchan connection
*/
int vchan_com_rem_connection(vchan_connect_t con) {
    /* See if this connection already exists */
    vchan_instance_t *inst = get_vchan_instance(con.v.domain, con.v.dest, con.v.port);
    if(inst != NULL) {
        //DPRINTF(4, "vchan: closing down on %d side\n", con.server);
        if(con.server)
            inst->server_connected = -1;
        else
            inst->client_connected = -1;

        if(inst->client_connected == -1 && inst->server_connected == -1) {
            //DPRINTF(4, "vchan: SHUTTING DOWN CONNECTIOn\n");
            rem_vchan_instance(inst);
        } else {
            vchan_com_ping();
        }
    }

    return 0;
}


intptr_t vchan_com_get_buf(vchan_ctrl_t args, int cmd) {
    vchan_buf_t *b;

    vchan_instance_t *i = get_vchan_instance(args.domain, args.dest, args.port);
    if(i == NULL)
        return -2;

    if(cmd == VCHAN_RECV) {
        b = get_dom_buf(args.domain, i->buffers);
    } else {
        b = get_dom_buf(args.dest, i->buffers);
    }

    if(b == NULL) {
        return -1;
    }

    return (intptr_t) ( (void *) b - (void *) share_mem);
}

void vchan_com_ping() {
    vevent_cl_emit();
    vevent_sv_emit();
}

int vchan_com_status(vchan_ctrl_t args) {
    int val = -1;
    vchan_instance_t *inst = get_vchan_instance(args.domain, args.dest, args.port);
    if(inst == NULL) {
        return 0;
    }

    /* closed connection - no client or server connected */
    if(inst->client_connected == -1 || inst->server_connected == -1) {
        val = 0;
    /* full connection - client and server connected */
    } else if (inst->client_connected == 1 && inst->server_connected == 1) {
        val = 1;
    /* server initialised, but no client connected */
    } else if (inst->client_connected == 0 && inst->server_connected == 1) {
        val = 2;
    }

    return val;
}


int vchan_com_alert_status(vchan_ctrl_t args) {
    int alert = VCHAN_EMPTY_BUF;
    vchan_instance_t *i = get_vchan_instance(args.domain, args.dest, args.port);
    vchan_ctrl_t cargs = {
        .domain = args.domain,
        .dest = args.dest,
        .port =  args.port,
    };
    int closed = vchan_com_status(cargs);

    vchan_buf_t *b = get_dom_buf(args.domain, i->buffers);
    assert(b != NULL);

    b->filled = abs(b->read_pos - b->write_pos);

    if(closed == 0) {
        if(b->filled) {
            alert = VCHAN_CLOSED_DATA;
        } else {
            alert = VCHAN_CLOSED;
        }
    } else {
        if(b->filled) {
            if(b->filled == VCHAN_BUF_SIZE) {
                alert = VCHAN_BUF_FULL;
            } else {
                alert = VCHAN_BUF_DATA;
            }
        }
    }

    return alert;
}

/*
    Creates a new vchan connection between two guests
*/
static int new_vchan_instance(vchan_connect_t *con) {
    vchan_shared_mem_t *buffers;
    uint32_t domx, domy, port, caller;

    domx = caller = con->v.domain;
    domy = con->v.dest;
    port = con->v.port;

    /* See if this connection already exists */
    vchan_instance_t *new = get_vchan_instance(domx, domy, port);
    if(new == NULL) {
        if(! con->server) {
            return -1;
        }

        new = malloc(sizeof(vchan_instance_t));
        if(new == NULL) {
            return -1;
        }

        buffers = alloc_buffer();
        if(buffers == NULL) {
            printf("vchan: bad buffer allocation!\n");
            return -1;
        }

        new->domx = MIN(domx, domy);
        new->domy = MAX(domx, domy);

        new->server_connected = 0;
        new->client_connected = 0;

        new->buffers = buffers;

        for(int x = 0; x < 2; x++) {
            clear_buf(&new->buffers->bufs[x]);
        }

        new->port = port;
        new->next = first_inst;
        first_inst = new;
    }

    if(con->server) {
        //DPRINTF(4, "vchan: server connection %d|%d|%d established\n", domx, domy, port);
        new->buffers->bufs[0].owner = MIN(domx, domy);
        new->buffers->bufs[1].owner = MAX(domx, domy);
        new->server_connected = 1;
    } else {
        if(new->server_connected == -1) {
            printf("vchan: trying to connect to closed server\n");
            return -1;
        }
        //DPRINTF(4, "vchan: client connection %d|%d|%d established\n", domx, domy, port);
        new->buffers->bufs[0].owner = MIN(domx, domy);
        new->buffers->bufs[1].owner = MAX(domx, domy);
        new->client_connected = 1;
    }

    vchan_com_ping();
    return 0;
}

/*
    Returns the address of a buffer belonging to a vm guest domain
*/
static vchan_buf_t *get_dom_buf(uint32_t buf, vchan_shared_mem_t *b) {

    if(b->bufs[0].owner == buf) {
        //DPRINTF(4, "Tagged buf 0 - returning\n");
        return &b->bufs[0];
    } else if(b->bufs[1].owner == buf) {
        //DPRINTF(4, "Tagged buf 1 - returning\n");
        return &b->bufs[1];
    }

    return NULL;
}

/*
    Free the memory of a given vchan instance
*/
static void rem_vchan_instance(vchan_instance_t *inst) {
    vchan_instance_t *prev = NULL;
    vchan_instance_t *find = first_inst;

    while(find != NULL) {
        if(inst == find) {
            if(prev != NULL) {
                prev->next = find->next;
            } else {
                first_inst = find->next;
            }
            printf("vchan: removing %d | %d | %d\n", inst->domx, inst->domy, inst->port);
            inst->buffers->alloced = 0;
            free(find);
            return;
        }
        prev = find;
        find = find->next;
    }
}

/*
    Buffer manipulation functions
*/
static vchan_shared_mem_t *alloc_buffer(void) {
    int x;
    vchan_shared_mem_t *cur;
    for(x = 0; x < NUM_BUFFERS; x++) {
        cur = get_buffer(x);
        assert(cur != NULL);
        if(cur->alloced == 0) {
            cur->alloced = 1;
            return cur;
        }
    }
    return NULL;
}

/*
    Return the address of a given buffer
*/
static vchan_shared_mem_t *get_buffer(uint32_t id) {
    if(id >= NUM_BUFFERS) {
        printf("get_buffer: bad id\n");
        return NULL;
    }
    return &(headers->shared_buffers[id]);
}

/*
    Find and return a given vchan instance
*/
static vchan_instance_t *get_vchan_instance(uint32_t domx, uint32_t domy, uint32_t port) {
    vchan_instance_t *inst = first_inst;
    while(inst != NULL) {
        if(inst->domx == MIN(domx, domy)) {
            if(inst->domy == MAX(domx, domy) && inst->port == port) {
                return inst;
            }
        }
        inst = inst->next;
    }
    return NULL;
}

static void print_vchan_instances() {
    vchan_instance_t *inst = first_inst;
    while(inst != NULL) {
        printf("(inst|%d|%d|%d)\n", inst->domx, inst->domy, inst->port);
        inst = inst->next;
    }
}

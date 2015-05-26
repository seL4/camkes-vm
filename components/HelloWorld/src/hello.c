/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#include <stdio.h>
#include <stdint.h>
#include <autoconf.h>

#include <sel4/sel4.h>
#include <sel4utils/util.h>

#include <HelloWorld.h>

#include "vm.h"
#include "vmm/vmm_manager.h"
#include "vmm/vchan_component.h"
#include "vmm/vchan_copy.h"
#include "vmm/vchan_sharemem.h"
#include "vmm/debug.h"

#include <camkes/dataport.h>

static char char_buf[256];

static void rec_packet(libvchan_t * con);
static void puffout_strings(libvchan_t * con);

static camkes_vchan_con_t con = {
    .connect = &vchan_con_new_connection,
    .disconnect = &vchan_con_rem_connection,
    .get_buf = &vchan_con_get_buf,
    .status = &vchan_con_status,

    .alert = &vchan_con_ping,
    .wait = &vevent_wait,
    .poll = &vevent_poll,

    .component_dom_num = 50,
};

/*
    Check if data in a test packet is correct
*/
static int verify_packet(vchan_packet_t *pak) {
    for(int i = 0; i < 4; i++) {
        if(pak->datah[i] != i + pak->pnum) {
            /* Malformed data */
            return 0;
        }
    }
    return 1;
}

static void rec_packet(libvchan_t * con) {
    size_t sz;
    char done = 1;
    int x, pnum;
    char comp[6];
    vchan_packet_t pak;

    libvchan_wait(con);
    sz = libvchan_read(con, &pnum, sizeof(int));
    assert(sz == sizeof(int));

    DPRINTF(2, "hello: number of packets to recieve = %d\n", pnum);
    for(x = 0; x < pnum; x++) {
        libvchan_wait(con);
        sz = libvchan_read(con, &pak, sizeof(pak));
        /* See if the given packet is correct */
        assert(sz == sizeof(pak));
        assert(pak.pnum == x);
        assert(verify_packet(&pak) == 1);
        assert(pak.guard == TEST_VCHAN_PAK_GUARD);

        DPRINTF(4, "hello.packet %d|%d\n", x, sizeof(pak));
    }

    DPRINTF(2, "hello: sending ack\n");

    sz = libvchan_write(con, &done, sizeof(char));
    assert(sz == sizeof(char));
}

void pre_init(void) {
}

int run(void) {
    libvchan_t *connection;

    DPRINTF(2, "Hello.Component Init\n");
    init_camkes_vchan(&con);
    con.data_buf = (void *)share_mem;

    connection = libvchan_server_init(0, 25, 0, 0);
    assert(connection != NULL);

    DPRINTF(2, "Connection Active\n");

    while(1) {
        DPRINTF(2, "hello.packet: begin\n");
        rec_packet(connection);
        DPRINTF(2, "hello.packet: end\n");
    }
}

/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

/*
 * Hello World Component.
 *
 * Tests the vchan connection between the comonent (me) and a server that should
 * be running on linux component.
 */

#include <stdio.h>
#include <stdint.h>
#include <autoconf.h>

#include <sel4/sel4.h>
#include <sel4utils/util.h>

#include <camkes.h>

#include "vm.h"
#include "vmm/vmm_manager.h"
#include "vmm/vchan_component.h"
#include "vmm/vchan_copy.h"
#include "vmm/vchan_sharemem.h"
#include "vmm/debug.h"

#include <camkes/dataport.h>

static void rec_packet(libvchan_t * con);

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
 * Connects to sel4
 */
static int ftp_esque(libvchan_t *con) {
    int portnumber;
    int ack = 1;
    size_t size;
    // con, the base communication channel
    libvchan_wait(con);
    // tell the component we want a new connection on this port
    libvchan_read(con, &portnumber, sizeof(int));
    libvchan_write(con, &ack, sizeof(int));

    /* Do tests */
    libvchan_t *sample = libvchan_server_init(0, portnumber, 0, 0);
    assert(sample != NULL);
    rec_packet(sample);
    libvchan_close(sample);
    printf("hello: ftp_esque: done\n");

    return 0;
}


/*
 *   Check if data in a test packet is correct
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


/*
 * Fills the buffer.
 */
static void funnel(libvchan_t *con) {
    int c = 0;
    size_t sz;

    assert(libvchan_buffer_space(con) == FILE_DATAPORT_MAX_SIZE);

    while(libvchan_buffer_space(con) > 0) {
        sz = libvchan_write(con, &c, sizeof(int));
        assert(sz == sizeof(int));
        //printf("hello funnel: remaining space: %d\n", libvchan_buffer_space(con));
        c++;
        assert(libvchan_data_ready(con) == 0);
    }
    printf("hello funnel: Funnel done\n");

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

    printf("hello: number of packets to recieve = %d\n", pnum);

    for(x = 0; x < pnum; x++) {
        libvchan_wait(con);
        /* Buffer sanity checking */
        assert(libvchan_data_ready(con) != 0);
        assert(libvchan_buffer_space(con) == FILE_DATAPORT_MAX_SIZE);
        /* Perform read operation */
        sz = libvchan_read(con, &pak, sizeof(pak));
        /* See if the given packet is correct */
        assert(sz == sizeof(pak));
        assert(pak.pnum == x);
        assert(verify_packet(&pak) == 1);
        assert(pak.guard == TEST_VCHAN_PAK_GUARD);
        //DPRINTF(4, "hello.packet %d|%d\n", x, sizeof(pak));
    }

    printf("hello: sending ack\n");

    sz = libvchan_write(con, &done, sizeof(char));
    assert(sz == sizeof(char));
}

void pre_init(void) {
}

int run(void) {
    libvchan_t *connection;

    printf("Hello component Init\n");
    init_camkes_vchan(&con);
    con.data_buf = (void *)share_mem;

    // Hardcoded value that should match up with the clients.
    connection = libvchan_server_init(0, 25, 0, 0);
    assert(connection != NULL);

    printf("Hello componenet Connection active\n");
    funnel(connection);
    ftp_esque(connection);
    while(1) {
        printf("Hello packet begin\n");
        rec_packet(connection);
        printf("Hello packet end\n");
    }
}

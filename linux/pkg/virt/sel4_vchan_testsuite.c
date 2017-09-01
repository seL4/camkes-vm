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

/*
 * seL4 Vchan Test Suite
 *
 * Tests the Vcahn connectiob between the Hello World component and the linux
 * Vm that this is connected to.
 *
 */

#include <stdint.h>

#include <vmm_manager.h>
#include <vchan_copy.h>
#include <libvchan.h>
#include <vmm_utils.h>

#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

static int funnel(libvchan_t *con);
static int send_packet(libvchan_t *con, int num_packets);
static int read_arg(char *str);
static int ftp_esque(libvchan_t *con);

/* 
 * Attempt to negotiate and send data using a different port over the same
 * vchan connection. 
 */
static int ftp_esque(libvchan_t *con) {
	int portnumber = 890;
	int ack = 0;
	// con, the base communication channel
	printf("testsuite: ftp_esque: start\n");

	// tell the component we want a new connection on this port
	libvchan_write(con, &portnumber, sizeof(int));
	// wait for an ok
	libvchan_wait(con);
	libvchan_read(con, &ack, sizeof(int));
	if(!ack) { // error
		printf("testsuite: ftp_esque: error on returned ack %d|%d\n", ack, 1);
	}

	/* Do tests */
	libvchan_t *sample = libvchan_client_init(50, portnumber);
	if(sample == NULL) {
		printf("bad client connection on %d\n", portnumber);
	}

	if(send_packet(sample, 30000) != 0) {
		printf("testsuite: ftp_esque: error on data channel\n");
		return -1;
	}

        libvchan_close(sample);
	printf("testsuite: ftp_esque: done\n");

	return 0;
}


/*
 * Drains the buffer that should be filled in the Hello world component's Funnel
 * function.
 */
static int funnel(libvchan_t *con) {
    int c = 0;
    int val;
    size_t sz; 

    printf("testsuite funnel: waiting\n");
    while(libvchan_data_ready(con) != FILE_DATAPORT_MAX_SIZE) {
    	continue;
    }

    printf("testsuite funnel: checking data\n");
    while(libvchan_data_ready(con) > 0) {
        sz = libvchan_read(con, &val, sizeof(int));
        if(sz != sizeof(int)) {
    		printf("funnel: bad size\n");
    		return -1;
        }
    	if(val != c) {
    		printf("funnel: bad val for %d|%d\n", c, val);
    		return -1;
    	}
    	if(libvchan_buffer_space(con) != FILE_DATAPORT_MAX_SIZE) {
    		printf("funnel: bad dport size %d\n", libvchan_buffer_space(con));
    		return -1;
    	}
    	c++;
    }

    printf("testsuite funnel: done\n");
    return 0;
}

static int send_packet(libvchan_t *con, int num_packets) {

	size_t sz;
	vchan_packet_t pak;
	int x, i;
	char fnack;

	printf("testsuite: packet start\n");

	/* Check that buffer data is correct */
	sz = libvchan_data_ready(con);
	if(sz != 0) {
		printf("error: incorrect start packet buffer size (data ready) %d\n", sz);
		return -1;
	}

	sz = libvchan_buffer_space(con);
	if(sz != FILE_DATAPORT_MAX_SIZE) {
		printf("error: incorrect start packet buffer size (bspace) %d\n", sz);
		return -1;
	}

	/* Start */

	sz = libvchan_send(con, &num_packets, sizeof(int));
	if(sz < sizeof(int)) {
		printf("--BAD PACKET NUM -- SEND\n");
		return -1;
	}

	printf("testsuite: send packets\n");

	for(x = 0; x < num_packets; x++) {
		pak.pnum = x;
		for(i = 0; i < 4; i++) {
			pak.datah[i] = i + x;
		}
		pak.guard = TEST_VCHAN_PAK_GUARD;

		while(libvchan_buffer_space(con) < sizeof(pak));
		sz = libvchan_send(con, &pak, sizeof(pak));
		if(sz < sizeof(pak)) {
			printf("--BAD PACKET -- SEND\n");
			return -1;
		}
	}

	printf("testsuite: waiting for ack..\n");

	libvchan_wait(con);
	sz = libvchan_read(con, &fnack, sizeof(char));
	if(sz < sizeof(char) || ! fnack) {
		return -1;
	}

	printf("testsuite: pack end\n");
	return 0;
}

/* Read arguments */
static int read_arg(char *str) {
	char *pend = NULL;
	long int ret = strtol(str, &pend, 10);
	if(pend[0] != '\0') {
		return -1;
	}
	if(ret < 1)
		return -1;

	return (int) ret;
}

static void usage(void) {
	printf("sel4_vchan_testsuite -n [1..n]\n-n: number of packets to send\n");
}

int main(int argc, char **argv) {

	int ecount = 0;
	if(argc != 3) {
		usage();
		exit(1);
	}

	int pnum = read_arg(argv[2]);
	if(pnum == -1) {
		usage();
		exit(1);
	}

	// Hardcoded to the connection defined in the server (HelloWorld component).
	printf("testsuite: Creating connection in image\n");
	libvchan_t *ctrl = libvchan_client_init(50, 25);

	assert(ctrl != NULL);
	printf("testsuite: Connection Established!\n");

	ecount += funnel(ctrl);
	printf("testsuite: Ecount is %d\n", ecount);
	if(ecount >= 0)
		ecount += ftp_esque(ctrl);
	if(ecount >= 0)
		ecount += send_packet(ctrl, pnum);

	printf("testsuite: %d errors\n", ecount);

	return 0;
}

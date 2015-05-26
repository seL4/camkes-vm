/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

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


static int send_packet(libvchan_t *con, int num_packets) {

	size_t sz;
	vchan_packet_t pak;
	int x, i;
	char fnack;

	printf("testsuite: packet start\n");

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

	printf("testsuite: Creating connection in image\n");
	libvchan_t *ctrl = libvchan_client_init(50, 25);
	assert(ctrl != NULL);
	printf("testsuite: Connection Established!\n");

	char test[] = "Hello World!, this is a test string.";
	// ecount += bad_values(ctrl);

	if(ecount == 0) {
		ecount += send_packet(ctrl, pnum);
	}

	printf("testsuite: %d errors\n", ecount);

	return 0;
}

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


int send_packet(libvchan_t *con) {

	size_t sz;
	vchan_packet_t pak;
	int x;

	for(x = 0; x < 800; x++) {
		sprintf(pak.pnum, "I%d", x);
		while(libvchan_buffer_space(con) < sizeof(pak));
		sz = libvchan_send(con, &pak, sizeof(pak));
		if(sz < sizeof(pak)) {
			printf("--BAD PACKET -- SEND\n");
			return -1;
		}
	}

	printf("testsuite: pack end\n");

	return 0;
}

int send_string(const char *string, libvchan_t *con) {
	size_t sz;
	vchan_header_t msg = {
		.msg_type = MSG_HELLO,
		.len = strlen(string),
	};

	printf("testsuite: hello\n");
	/* Send hello */
	sz = libvchan_write(con, &msg, sizeof(msg));
	assert(sz == sizeof(msg));

	printf("testsuite: ack wait\n");
	/* Wait for ack */
	libvchan_wait(con);
	sz = libvchan_read(con, &msg, sizeof(msg));
	assert(sz == sizeof(msg));
	assert(msg.msg_type == MSG_ACK);

	/* Send string */
	printf("testsuite: string\n");
	sz = libvchan_write(con, string, strlen(string));
	assert(sz = strlen(string));

	// /* Wait for conclude */
	// printf("testsuite: conc wait\n");
	// libvchan_wait(con);
	// sz = libvchan_read(con, &msg, sizeof(msg));
	// if(sz < sizeof(msg) || msg.msg_type != MSG_CONC) {
	// 	return -1;
	// }

	printf("testsuite: finished string\n");


	return 0;

}

int main(int argc, char **argv) {

	int ecount = 0;

	printf("testsuite: Creating connection in image\n");
	libvchan_t *ctrl = libvchan_client_init(50, 25);
	assert(ctrl != NULL);
	printf("testsuite: Connection Established!\n");

	char test[] = "Hello World!, this is a test string.";
	ecount += send_string(test, ctrl);
	if(ecount == 0) {
		ecount += send_packet(ctrl);
	}


	printf("testsuite: %d errors\n", ecount);

	return 0;
}

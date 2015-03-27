// #include <vmm_manager.h>
// #include <libvchan.h>
// #include <vmm_utils.h>

// #include <sys/ioctl.h>
// #include <string.h>
// #include <fcntl.h>
// #include <stdio.h>
// #include <sys/mman.h>
// #include <errno.h>
// #include <sys/eventfd.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <assert.h>
// #include <time.h>

// #define MAX_PACKETS 12
// #define SERVER 1
// #define CLIENT 0
// #define PACKET_TOK 0xdeedbaaf
// #define NUM_PORT 4

// typedef struct test_packet {
// 	int seq;
// 	int token;
// } test_packet_t;

// static int instbuf[] = { 1, 2, 5, 10, 8, 6, 4, 1};

// int server;
// libvchan_t *ctrl = NULL;
// static uint64_t selectfd = 0;

// int init();
// int simple_write(libvchan_t *cn);
// int packets(libvchan_t *cn);
// int close_open();

// int multiport();

// int multiport() {
// 	int x;
// 	int port_num = 100;
// 	libvchan_t *port[NUM_PORT];
// 	printf("mulitport: starting\n");


// 	if(server) {
// 		for(x = 0; x < NUM_PORT; x++) {
// 			port[x] = libvchan_server_init(1, port_num + x, 0, 0);
// 			if(port[x] == NULL) {
// 				printf("multiport: failed server init %x", x);
// 				return -1;
// 			}
// 		}
// 		wakeup_vm(1);
// 		sleep_vm();
// 	} else {
// 		sleep_vm();
// 		for(x = 0; x < NUM_PORT; x++) {
// 			port[x] = libvchan_client_init(0, port_num + x);
// 			if(port[x] == NULL) {
// 				printf("multiport: failed client init %x", x);
// 				return -1;
// 			}
// 		}
// 		wakeup_vm(0);
// 	}

// 	for(x = 0; x < NUM_PORT; x++) {
// 		printf("mul: con %d\n", x);
// 		if(simple_write(port[x]) < 0) {
// 			return -1;
// 		}
// 	}

// 	printf("mutliport: closing connections\n");
// 	for(x = 0; x < NUM_PORT; x++) {
// 		libvchan_close(port[x]);
// 	}

// 	return 0;
// }



// int close_open() {
// 	int res;
// 	int fail_num = 0;

// 	printf("closeopen: closing and reopening\n");

// 	libvchan_close(ctrl);

// 	res = init();
// 	if(res) {
// 		return -1;
// 	}

// 	fail_num += simple_write(ctrl);
// 	fail_num += packets(ctrl);

// 	return fail_num;
// }

// int packets(libvchan_t *cn) {
// 	int x, res;
// 	int siz;
// 	test_packet_t packet;

// 	if(server) {
// 		sleep_vm();
// 		if(libvchan_buffer_space(cn) <= 0) {
// 			printf("server_packet: incorrect buf val\n");
// 			return -1;
// 		}
// 		for(x = 0; x < MAX_PACKETS; x++) {
// 			packet.seq = x;
// 			packet.token = PACKET_TOK;
// 			res = libvchan_write(cn, &packet, sizeof(test_packet_t));
// 			if(res <= 0) {
// 				printf("server_packet: failed write\n");
// 				return -1;
// 			}
// 		}
// 	} else {

// 		wakeup_vm(0);

// 		for(x = 0; x < MAX_PACKETS; x++) {
// 			res = libvchan_wait(cn);
// 			if(res < 0) {
// 				printf("client_packet: Failed to wait %d\n", res);
// 				return -1;
// 			}


// 			printf("client: reading\n");
// 			siz = libvchan_data_ready(cn);
// 			if(siz != sizeof(test_packet_t)) {
// 				printf("client: warning, data_ready has incorrect value |%d|%d|\n", siz, sizeof(test_packet_t));
// 			}

// 			res = libvchan_read(cn, &packet, sizeof(test_packet_t));
// 			if(res <= 0) {
// 				printf("client_packet: Failed to read\n");
// 				return -1;
// 			}
// 			if(packet.seq != x) {
// 				printf("client_packet: incorrect seq x:%d p:%d\n", x, packet.seq);
// 			}
// 			if(packet.token != PACKET_TOK) {
// 				printf("client_packet: incorrect token\n");
// 			}
// 		}
// 	}


// 	return 0;
// }



// int simple_write(libvchan_t *cn) {

// 	int buf[128];
// 	int res;
// 	int siz;

// 	if(server) {
// 		printf("writing\n");

// 		// if(libvchan_buffer_space(cn) <= 0) {
// 		// 	printf("server nowait: no space when there should be\n");
// 		// 	return -1;
// 		// }

// 		res = libvchan_write(cn, instbuf, sizeof(instbuf));
// 		if(res <= 0) {
// 			printf("Failed to perform simple write\n");
// 			return -1;
// 		}

// 	} else {

// 		res = eventfd_read(libvchan_fd_for_select(cn), &selectfd);
// 		if(res < 0) {
// 			printf("client: failed on eventfd reading\n");
// 			return -1;
// 		}

// 		printf("client: performing full wait\n");
// 		/* This should not block */
// 		res = libvchan_wait(cn);
// 		if(res < 0) {
// 			printf("client: Failed to wait %d\n", res);
// 			return -1;
// 		}

// 		printf("client: reading\n");
// 		siz = libvchan_data_ready(cn);
// 		if(siz != sizeof(instbuf)) {
// 			printf("client: warning, data_ready has incorrect value |%d|%d|\n", siz, sizeof(instbuf));
// 		}

// 		res = libvchan_read(cn, buf, sizeof(instbuf));
// 		if(res <= 0) {
// 			printf("client: Failed to read\n");
// 			return -1;
// 		}

// 		if(memcmp(buf, instbuf, sizeof(instbuf)) != 0) {
// 			printf("client: Failed to compare data\n");
// 			return -1;
// 		} else {
// 			printf("client_test1: SUCCESS\n");
// 		}
// 	}

// 	return 0;
// }



// int init() {

// 	int vm = vm_number();
// 	if(vm > 1) {
// 		printf("vm %d not running in this test\n", vm);
// 		exit(0);
// 	}

// 	if(vm == 0) {
// 		ctrl = libvchan_server_init(1, 20, 0, 0);
// 		wakeup_vm(1);
// 		server = 1;
// 		sleep_vm();
// 	} else {
// 		server = 0;
// 		sleep_vm();
// 		ctrl = libvchan_client_init(0, 20);
// 		wakeup_vm(0);
// 	}
// 	if(ctrl == NULL) {
// 		printf("Failed to establish ctrl\n");
// 		return -1;
// 	}

// 	return 0;

// }


int main() {
// 	int res;
// 	int fail_num = 0;

// 	res = init();
// 	if(res) {
// 		return -1;
// 	}

// 	fail_num += simple_write(ctrl);
// 	fail_num += packets(ctrl);
// 	fail_num += multiport();
// 	fail_num += close_open();

// 	if(fail_num == 0) {
// 		printf("++All Vchan Tests passed++\n");
// 	} else {
// 		printf("--%d Vchan Tests failed--\n", abs(fail_num));

// 	}

// 	libvchan_close(ctrl);


// 	printf("++VMMTESTCONCLUDE++\n");

	return 0;
}

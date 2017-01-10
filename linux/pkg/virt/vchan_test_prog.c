/**
 * @file
 * @section AUTHORS
 *
 * Copyright (C) 2010  Rafal Wojtczuk  <rafal@xxxxxxxxxxxxxxxxxxxxxx>
 *
 *  Authors:
 *       Rafal Wojtczuk  <rafal@xxxxxxxxxxxxxxxxxxxxxx>
 *       Daniel De Graaf <dgdegra@xxxxxxxxxxxxx>
 *
 * @section LICENSE
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; under version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * @section DESCRIPTION
 *
 * This is a test program for libvchan.  Communications are bidirectional,
 * with either server (grant offeror) or client able to read and write.
 */

// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <errno.h>

// #include "../mod_create/includes/libvchan.h"

// void usage(char** argv)
// {
//        fprintf(stderr, "usage:\n"
//                "\t%s [client|server] domainid nodeid [rbufsiz wbufsiz]\n",
//                argv[0]);
//        exit(1);
// }

// #define BUFSIZE 5000
// char inbuf[BUFSIZE];
// char outbuf[BUFSIZE];
// int insiz = 0;
// int outsiz = 0;
// struct libvchan *ctrl = 0;

// void vchan_wr() {
//        if (!insiz)
//                return;
//        int ret = libvchan_write(ctrl, inbuf, insiz);
//        if (ret < 0) {
//                fprintf(stderr, "vchan write failed\n");
//                exit(1);
//        }
//        if (ret > 0) {
//                insiz -= ret;
//                memmove(inbuf, inbuf + ret, insiz);
//        }
// }

// void stdout_wr() {
//        if (!outsiz)
//                return;
//        int ret = write(1, outbuf, outsiz);
//        if (ret < 0 && errno != EAGAIN)
//                exit(1);
//        if (ret > 0) {
//                outsiz -= ret;
//                memmove(outbuf, outbuf + ret, outsiz);
//        }
// }

// /**
//     Simple libvchan application, both client and server.
//        Both sides may write and read, both from the libvchan and from
//        stdin/stdout (just like netcat).
// */

// int main(int argc, char **argv)
// {
//        int ret;
//        int libvchan_fd;
//        if (argc < 4)
//                usage(argv);
//        if (!strcmp(argv[1], "server")) {
//                int rsiz = argc > 4 ? atoi(argv[4]) : 0;
//                int wsiz = argc > 5 ? atoi(argv[5]) : 0;
//                ctrl = libvchan_server_init(atoi(argv[2]), atoi(argv[3]), rsiz,
// wsiz);
//        } else if (!strcmp(argv[1], "client"))
//                ctrl = libvchan_client_init(atoi(argv[2]), atoi(argv[3]));
//        else
//                usage(argv);
//        if (!ctrl) {
//                perror("libvchan_*_init");
//                exit(1);
//        }

//        fcntl(0, F_SETFL, O_NONBLOCK);
//        fcntl(1, F_SETFL, O_NONBLOCK);

//        libvchan_fd = libvchan_fd_for_select(ctrl);
//        for (;;) {
//                fd_set rfds;
//                fd_set wfds;
//                FD_ZERO(&rfds);
//                FD_ZERO(&wfds);
//                if (insiz != BUFSIZE)
//                        FD_SET(0, &rfds);
//                if (outsiz)
//                        FD_SET(1, &wfds);
//                FD_SET(libvchan_fd, &rfds);
//                ret = select(libvchan_fd + 1, &rfds, &wfds, NULL, NULL);
//                if (ret < 0) {
//                        perror("select");
//                        exit(1);
//                }
//                if (FD_ISSET(0, &rfds)) {
//                        ret = read(0, inbuf + insiz, BUFSIZE - insiz);
//                        if (ret < 0 && errno != EAGAIN)
//                                exit(1);
//                        if (ret == 0) {
//                                while (insiz) {
//                                        vchan_wr();
//                                        libvchan_wait(ctrl);
//                                }
//                                return 0;
//                        }
//                        if (ret)
//                                insiz += ret;
//                        vchan_wr();
//                }
//                if (FD_ISSET(libvchan_fd, &rfds)) {
//                        libvchan_wait(ctrl);
//                        vchan_wr();
//                }
//                if (FD_ISSET(1, &wfds))
//                        stdout_wr();
//                while (libvchan_data_ready(ctrl) && outsiz < BUFSIZE) {
//                        ret = libvchan_read(ctrl, outbuf + outsiz, BUFSIZE -
// outsiz);
//                        if (ret < 0)
//                                exit(1);
//                        outsiz += ret;
//                        stdout_wr();
//                }
//                if (!libvchan_is_open(ctrl)) {
//                        fcntl(1, F_SETFL, 0);
//                        while (outsiz)
//                                stdout_wr();
//                        return 0;
//                }
//        }
// }


/**
 * @file
 * @section AUTHORS
 *
 * Copyright (C) 2010  Rafal Wojtczuk  <rafal@xxxxxxxxxxxxxxxxxxxxxx>
 *
 *  Authors:
 *       Rafal Wojtczuk  <rafal@xxxxxxxxxxxxxxxxxxxxxx>
 *       Daniel De Graaf <dgdegra@xxxxxxxxxxxxx>
 *
 * @section LICENSE
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; under version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * @section DESCRIPTION
 *
 * This is a test program for libvchan.  Communications are in one direction,
 * either server (grant offeror) to client or vice versa.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <vmm_manager.h>
#include <vchan_copy.h>
#include <libvchan.h>
#include <vmm_utils.h>

int libvchan_write_all(struct libvchan *ctrl, char *buf, int size)
{
       int written = 0;
       int ret;
       while (written < size) {
          printf("Writing data\n");
               ret = libvchan_write(ctrl, buf + written, size - written);
               if (ret <= 0) {
                       perror("write");
                       exit(1);
               }
               written += ret;
       }
       return size;
}

int write_all(int fd, char *buf, int size)
{
       int written = 0;
       int ret;
       while (written < size) {
               ret = write(fd, buf + written, size - written);
               if (ret <= 0) {
                       perror("write");
                       exit(1);
               }
               written += ret;
       }
       return size;
}

void usage(char** argv)
{
       fprintf(stderr, "usage:\n"
               "%s [client|server] [read|write] domid nodeid\n", argv[0]);
       exit(1);
}

#define BUFSIZE 5000
char buf[BUFSIZE];
void reader(struct libvchan *ctrl)
{
    printf("starting read\n");
       int size;
       for (;;) {
               size = rand() % (BUFSIZE - 1) + 1;
               size = libvchan_read(ctrl, buf, size);
               printf("#");
               if (size < 0) {
                       perror("read vchan");
                       libvchan_close(ctrl);
                       exit(1);
               }
               if (size == 0) {
                   break;
               }

               size = write_all(1, buf, size);
               if (size < 0) {
                       perror("stdout write");
                       exit(1);
               }
               if (size == 0) {
                       perror("write size=0?\n");
                       exit(1);
               }
       }
}

void writer(struct libvchan *ctrl)
{
    printf("Begin Writer\n");
       int size;
       for (;;) {
               size = rand() % (BUFSIZE - 1) + 1;
               size = read(0, buf, size);
               if (size < 0) {
                       perror("read stdin");
                       libvchan_close(ctrl);
                       exit(1);
               }
               if (size == 0)
                      break;
               size = libvchan_write_all(ctrl, buf, size);
               fprintf(stderr, "#");
               if (size < 0) {
                       perror("vchan write");
                       exit(1);
               }
               if (size == 0) {
                       perror("write size=0?\n");
                       exit(1);
               }
       }
}


/**
       Simple libvchan application, both client and server.
       One side does writing, the other side does reading; both from
       standard input/output fds.
*/
int main(int argc, char **argv)
{
       int seed = time(0);
       struct libvchan *ctrl = 0;
       int wr;
       if (argc < 4)
               usage(argv);
       if (!strcmp(argv[2], "read"))
               wr = 0;
       else if (!strcmp(argv[2], "write"))
               wr = 1;
       else
               usage(argv);
       if (!strcmp(argv[1], "server"))
               ctrl = libvchan_server_init(atoi(argv[3]), atoi(argv[4]), 0, 0);
       else if (!strcmp(argv[1], "client"))
               ctrl = libvchan_client_init(atoi(argv[3]), atoi(argv[4]));
       else
               usage(argv);
       if (!ctrl) {
               perror("libvchan_*_init");
               exit(1);
       }

       srand(seed);
       printf("seed=%d\n", seed);
       if (wr) {
           writer(ctrl);
       } else {
       		/* Child reader */
			// if(fork() == 0) {
       		reader(ctrl);
			// } else {
				/* Parent reader */
				// return 0;
			// }
       }
        printf("Closing connection\n");
       libvchan_close(ctrl);
       return 0;
}

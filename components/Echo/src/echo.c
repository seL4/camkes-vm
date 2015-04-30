/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#include <autoconf.h>

#include <Echo.h>
#include <string.h>
#include <lwip/udp.h>

void echo_recv_ready_callback() {
    int status = 0;
    while (status == 0) {
        unsigned int len;
        uint16_t port;
        ip_addr_t addr;
        status = echo_recv_poll(&len, &port, &addr);
        if (status != -1) {
            echo_send_send((uintptr_t)echo_recv_buf, len, addr);
        }
    }
}

void echo2_recv_ready_callback() {
    int status = 0;
    while (status == 0) {
        unsigned int len;
        uint16_t port;
        ip_addr_t addr;
        status = echo2_recv_poll(&len, &port, &addr);
        if (status != -1) {
            echo2_send_send((uintptr_t)echo2_recv_buf, len, addr);
        }
    }
}

void post_init() {
    /* timeout once a second */
    int ret;
    ret = timer_periodic(0, 1000000000);
}

void timer_complete_callback() {
    /* do nothing with the timer tick */
}

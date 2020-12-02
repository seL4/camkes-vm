/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <autoconf.h>

#include <camkes.h>
/* get rid of camkes ERR_IF macro that collides with the lwip one */
#undef ERR_IF
#include <string.h>

extern volatile void *echo_recv_buf;
extern volatile void *echo2_recv_buf;

void echo_recv_ready_callback()
{
    int status = 0;
    while (status == 0) {
        unsigned int len;
        uint16_t port;
        uint32_t ip4addr;
        status = echo_recv_poll(&len, &port, &ip4addr);
        if (status != -1) {
            echo_send_send((uintptr_t)echo_recv_buf, len, ip4addr);
        }
    }
}

void echo2_recv_ready_callback()
{
    int status = 0;
    while (status == 0) {
        unsigned int len;
        uint16_t port;
        uint32_t ip4addr;
        status = echo2_recv_poll(&len, &port, &ip4addr);
        if (status != -1) {
            echo2_send_send((uintptr_t)echo2_recv_buf, len, ip4addr);
        }
    }
}


int run(void)
{

    while (1) {
        seL4_Word badge;
        seL4_Wait(echo2_recv_notification(), &badge);
        if (badge & echo_recv_notification_badge()) {
            echo_recv_ready_callback();
        }
        if (badge & echo2_recv_notification_badge()) {
            echo2_recv_ready_callback();
        }
    }
}

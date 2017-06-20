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

#include <autoconf.h>

#include <camkes.h>
/* get rid of camkes ERR_IF macro that collides with the lwip one */
#undef ERR_IF
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

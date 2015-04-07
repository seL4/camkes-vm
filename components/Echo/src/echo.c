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

void echo_send_send(void *p, unsigned int len, ip_addr_t addr, uint16_t port);

void echo_recv_recv(void *p, unsigned int len, uint16_t port, ip_addr_t addr) {
    echo_send_send(p, len, addr, port);
}

void post_init() {
    set_putchar(putchar_putchar);
}

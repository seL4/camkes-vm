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

void post_init() {
    /* Check it's the right year */
    rtc_time_date_t time_date = rtc_time_date();
    assert(time_date.year == 2017);
    /* timeout once a second */
    int UNUSED ret;
    ret = timer_periodic(0, 1000000000);
}

void timer_complete_callback() {
    /* do nothing with the timer tick */
}

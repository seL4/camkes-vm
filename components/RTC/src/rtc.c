/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#include <camkes.h>
#include <platsupport/plat/rtc.h>
#include <utils/util.h>

#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

static int
cmos_io_port_in(void *cookie, uint32_t port, int io_size, uint32_t *result) {
    if (io_size != 1) {
        return -1;
    }
    switch (port) {
    case CMOS_ADDRESS:
        *result = cmos_address_in8(port);
        return 0;
    case CMOS_DATA:
        *result = cmos_data_in8(port);
        return 0;
    default:
        return -1;
    }
}


static int
cmos_io_port_out(void *cookie, uint32_t port, int io_size, uint32_t val) {
    if (io_size != 1) {
        return -1;
    }
    switch(port) {
    case CMOS_ADDRESS:
        cmos_address_out8(port, val);
        return 0;
    case CMOS_DATA:
        cmos_data_out8(port, val);
        return 0;
    default:
        return -1;
    }
}

rtc_time_date_t rtc_time_date(void) {
    rtc_time_date_t time_date;
    int error UNUSED;
    ps_io_port_ops_t ops = (ps_io_port_ops_t){.io_port_in_fn = cmos_io_port_in, .io_port_out_fn = cmos_io_port_out};
    error = rtc_get_time_date_reg(&ops, 0, &time_date);
    assert(!error);
    return time_date;
}

void pre_init(void) {
    set_putchar(putchar_putchar);
}

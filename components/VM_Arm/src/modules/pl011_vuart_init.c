/*
 * Copyright 2024, DornerWorks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <vmlinux.h>
#include <camkes.h>
#include <camkes/io.h>
#include <camkes/irq.h>
#include <platsupport/chardev.h>
#include <platsupport/irq.h>

#include <sel4vmmplatsupport/plat/devices.h>
#include <sel4vmmplatsupport/arch/pl011_vuart.h>

struct ps_chardevice serial_device;
struct ps_chardevice *serial = NULL;
static ps_io_ops_t io_ops;

static int handle_pl011_vuart_console(void *data, ps_irq_acknowledge_fn_t acknowledge_fn, void *ack_data)
{
    if (serial) {
        int c = 0;
        ps_cdev_handle_irq(serial, 0);
        c = ps_cdev_getchar(serial);
        pl011_vuart_handle_irq(c);
    }

    int error = acknowledge_fn(ack_data);
    ZF_LOGF_IF(error, "Failed to acknowledge pl011 vuart IRQ");
    return 0;
}

/*
  Wrapper function around printf that formats the char and
  flushes the buffer for immediate  printing
*/
static void vuart_callback(int c) {
    ps_cdev_putchar(serial, c);
}

static int init_vuart_serial()
{
    camkes_io_ops(&io_ops);
    assert(&io_ops);
    serial = ps_cdev_init(PS_SERIAL_DEFAULT, &io_ops, &serial_device);

    if (serial == NULL) {
        ZF_LOGE("Failed to initialise character device for pl011 vuart");
        return -1;
    }

    ps_irq_t irq_info = { .type = PS_INTERRUPT, .irq = { .number = DEFAULT_SERIAL_INTERRUPT }};
    irq_id_t serial_irq_id = ps_irq_register(&io_ops.irq_ops, irq_info, (irq_callback_fn_t)handle_pl011_vuart_console, NULL);
    if(serial_irq_id < 0) {
        ZF_LOGE("Failed to register irq for pl011 vuart");
        return -1;
    }

    return 0;
}

/* Install vuart */
static void pl011_vconsole_init_module(vm_t *vm, void *cookie)
{
    int err = init_vuart_serial();
    assert(err == 0);
    vm_install_pl011_vconsole(vm, (print_func_t) vuart_callback);
}

DEFINE_MODULE(pl011_vconsole, NULL, pl011_vconsole_init_module)
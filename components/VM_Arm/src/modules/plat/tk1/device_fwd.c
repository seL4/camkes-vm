/*
 * Copyright 2018, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <sel4vm/plat/devices.h>

#include <vmlinux.h>

#include <sel4vm/guest_vm.h>
#include <sel4vm/devices/generic_forward.h>

struct generic_forward_cfg camkes_uart_d = {
    .read_fn = uartfwd_read,
    .write_fn = uartfwd_write
};

struct generic_forward_cfg camkes_clk_car =  {
    .read_fn = clkcarfwd_read,
    .write_fn = clkcarfwd_write
};


static void device_fwd_init_module(vm_t *vm, void *cookie)
{

    /* Configure UART forward device */
    int err = vm_install_generic_forward_device(vm, &dev_vconsole, camkes_uart_d);
    assert(!err);

    /* Configure Clock and Reset forward device */
    err = vm_install_generic_forward_device(vm, &dev_clkcar, camkes_clk_car);
    assert(!err);
}

DEFINE_MODULE(device_fwd, NULL, device_fwd_init_module)

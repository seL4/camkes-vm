/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <autoconf.h>
#include <arm_vm/gen_config.h>

#include <vmlinux.h>

#include <string.h>

#include <vka/capops.h>
#include <camkes.h>

#include <sel4vm/guest_vm.h>
#include <sel4utils/irq_server.h>
#include <cpio/cpio.h>

#include <sel4vmmplatsupport/device.h>
#include <sel4vmmplatsupport/device_utils.h>
#include <sel4vmmplatsupport/plat/usb.h>
#include <sel4vmmplatsupport/plat/devices.h>
#include <sel4vmmplatsupport/arch/guest_reboot.h>

static const struct device *linux_pt_devices[] = {
    &dev_usb1,
    &dev_usb3,
    &dev_sdmmc,
};

static const struct device *linux_ram_devices[] = {
#ifndef CONFIG_TK1_INSECURE
    &dev_rtc_kbc_pmc,
    &dev_exception_vectors,
    &dev_system_registers,
    &dev_ictlr,
    &dev_apb_misc,
    &dev_fuse,
    &dev_gpios,
#endif /* CONFIG_TK1_INSECURE */
    &dev_data_memory,
};

extern reboot_hooks_list_t reboot_hooks_list;

static void plat_init_module(vm_t *vm, void *cookie)
{
    int err;
    int i;

    err = vm_install_tk1_usb_passthrough_device(vm, &reboot_hooks_list);
    assert(!err);

    /* Install pass through devices */
    /* In insecure mode TK1 passes through all devices at the moment by using on-demand device mapping */
    for (i = 0; i < sizeof(linux_pt_devices) / sizeof(*linux_pt_devices); i++) {
        err = vm_install_passthrough_device(vm, linux_pt_devices[i]);
        assert(!err);
    }

    /* Install ram backed devices */
    /* Devices that are just anonymous memory mappings */
    for (i = 0; i < sizeof(linux_ram_devices) / sizeof(*linux_ram_devices); i++) {
        err = vm_install_ram_only_device(vm, linux_ram_devices[i]);
        assert(!err);
    }
}

DEFINE_MODULE(plat, NULL, plat_init_module)

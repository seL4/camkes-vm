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

#include <sel4vm/guest_vm.h>
#include <sel4vm/guest_vcpu_fault.h>
#include <sel4vmmplatsupport/device_utils.h>
#include <sel4vmmplatsupport/plat/devices.h>
#include <sel4vmmplatsupport/arch/vusb.h>
#include <sel4vmmplatsupport/plat/vuart.h>
#include <sel4vmmplatsupport/plat/vsdhc.h>
#include <sel4vmmplatsupport/plat/vclock.h>
#include <sel4vmmplatsupport/plat/vmct.h>
#include <sel4vmmplatsupport/plat/vpower.h>

#include <sel4vmmplatsupport/plat/irq_combiner.h>


#include <sel4utils/irq_server.h>
#include <cpio/cpio.h>
#include <utils/util.h>

#include <camkes.h>


#define MACH_TYPE_EXYNOS5410 4151

#ifdef CONFIG_VM_EMMC2_NODMA
#define FEATURE_MMC_NODMA
#endif

#ifdef CONFIG_VM_VUSB
#define FEATURE_VUSB
#endif

extern irq_server_t _irq_server;
extern seL4_CPtr _fault_endpoint;

static const struct device *linux_pt_devices[] = {
    &dev_ps_chip_id,
    &dev_msh0,
#ifndef FEATURE_MMC_NODMA
    &dev_msh2,
#endif
};

static int vm_shutdown_cb(vm_t *vm, void *token)
{
    printf("Received shutdown from linux\n");
    return -1;
}

void restart_component(void);

static int vm_reboot_cb(vm_t *vm, void *token)
{
    restart_component();

    return 0;

}

static memory_fault_result_t pwmsig_device_fault_handler(vm_t *vm, vm_vcpu_t *vcpu, uintptr_t fault_addr,
                                                         size_t fault_length, void *cookie)
{
    uint32_t data = get_vcpu_fault_data(vcpu);
    advance_vcpu_fault(vcpu);

//    printf("IN VM, GOT PWM SIGNAL 0x%x\n", data);
//    fflush(stdout);
    pwm_vmsig(data);
    return FAULT_HANDLED;
}

struct device pwmsig_dev = {
    .name = "NICTAcopter signal",
    .pstart = 0x30000000,
    .size = 0x1000,
    .priv = NULL,
};




#if defined FEATURE_VUSB

static vusb_device_t *_vusb;
static usb_host_t _hcd;

static void usb_irq_handler(void *data, ps_irq_acknowledge_fn_t acknowledge_fn, void *ack_data)
{
    assert(data);
    usb_host_t *hcd = (usb_host_t *)data;
    usb_hcd_handle_irq(hcd);
    assert(!acknowledge_fn(ack_data));
}

static int install_vusb(vm_t *vm)
{
    irq_server_t *irq_server;
    ps_io_ops_t *io_ops;
    vusb_device_t *vusb;
    usb_host_t *hcd;
    irq_id_t irq_id;
    seL4_CPtr vmm_ep;
    int err;
    irq_server = &_irq_server;
    io_ops = vm->io_ops;
    hcd = &_hcd;
    vmm_ep = _fault_endpoint;

    /* Initialise the physical host controller */
    err = usb_host_init(USB_HOST_DEFAULT, io_ops, hcd);
    assert(!err);
    if (err) {
        return -1;
    }

    /* Route physical IRQs */
    irq_id = irq_server_register_irq(irq_server, 103, usb_irq_handler, hcd);
    if (irq_id < 0) {
        return -1;
    }
    /* Install the virtual device */
    vusb = vm_install_vusb(vm, hcd, VUSB_ADDRESS, VUSB_IRQ, vmm_ep, VUSB_NINDEX,
                           VUSB_NBADGE);
    assert(vusb != NULL);
    if (vusb == NULL) {
        return -1;
    }
    _vusb = vusb;

    return 0;
}

void vusb_notify(void)
{
    vm_vusb_notify(_vusb);
}

#else /* FEATURE_VUSB */

#include <platsupport/gpio.h>
#include <platsupport/plat/gpio.h>
#include <platsupport/mach/pmic.h>
#include <usb/drivers/usb3503_hub.h>

#define NRESET_GPIO              XEINT12
#define HUBCONNECT_GPIO          XEINT6
#define NINT_GPIO                XEINT7

static int install_vusb(vm_t *vm)
{
    /* Passthrough USB for linux, however, we must first initialise
     * dependent systems which linux is not granted access to.
     * Primarily, we must turn on the ethernet and on board hub */
    ps_io_ops_t *io_ops = vm->io_ops;
    gpio_sys_t gpio_sys;
    struct i2c_bb i2c_bb;
    i2c_bus_t i2c_bus;
    pmic_t pmic;
    usb_host_t hcd;
    usb3503_t usb3503_hub;
    int err;

    /* Initialise the USB host controller. We hand it over to linux later */
    err = usb_host_init(USB_HOST_DEFAULT, vm->io_ops, NULL, &hcd);
    assert(!err);

    /* Initialise I2C and GPIO and PMIC for USB power control */
    err = gpio_sys_init(io_ops, &gpio_sys);
    assert(!err);
    err = i2c_bb_init(&gpio_sys, GPIOID(GPA2, 1), GPIOID(GPA2, 0), &i2c_bb, &i2c_bus);
    assert(!err);
    err = pmic_init(&i2c_bus, PMIC_BUSADDR, &pmic);
    assert(!err);

    /* Power on the USB hub */
    err = usb3503_init(&i2c_bus, &gpio_sys, NRESET_GPIO, HUBCONNECT_GPIO,
                       NINT_GPIO, &usb3503_hub);
    assert(!err);
    usb3503_connect(&usb3503_hub);

    /* Power on the ethernet chip */
    pmic_ldo_cfg(&pmic, LDO_ETH, LDO_ON, 3300);

    return 0;
}

void vusb_notify(void)
{
}

#endif /* FEATURE_VUSB */


void configure_clocks(vm_t *vm)
{
    struct clock_device *clock_dev;
    clock_dev = vm_install_ac_clock(vm, VACDEV_DEFAULT_DENY, VACDEV_REPORT_AND_MASK);
    assert(clock_dev);
    vm_clock_provide(clock_dev, CLK_MMC0);
    vm_clock_provide(clock_dev, CLK_MMC2);
    vm_clock_provide(clock_dev, CLK_SCLKVPLL);
    vm_clock_provide(clock_dev, CLK_SCLKGPLL);
    vm_clock_provide(clock_dev, CLK_SCLKCPLL);
}

static void plat_init_module(vm_t *vm, void *cookie)
{
    int err;
    int i;

    err = vm_install_vmct(vm);
    assert(!err);

    /* Add hooks for specific power management hooks */
    err = vm_install_vpower(vm, &vm_shutdown_cb, NULL, &vm_reboot_cb, NULL);
    assert(!err);
    /* Install virtual USB */
    err = install_vusb(vm);
    assert(!err);
#if defined FEATURE_MMC_NODMA
    /* Install SDHC controller with DMA restricted */
    err = vm_install_nodma_sdhc2(vm);
    assert(!err);
#endif
    configure_clocks(vm);

    err = vm_install_ac_uart(vm, &dev_vconsole);
    assert(!err);

    /* Device for signalling to the VM */
    vm_memory_reservation_t *reservation = vm_reserve_memory_at(vm, pwmsig_dev.pstart, pwmsig_dev.size,
                                                                pwmsig_device_fault_handler, &pwmsig_dev);
    assert(reservation);

    /* Install pass through devices */
    for (i = 0; i < ARRAY_SIZE(linux_pt_devices); i++) {
        err = vm_install_passthrough_device(vm, linux_pt_devices[i]);
    }

}


static void vcombiner_irq_handler(void *data, ps_irq_acknowledge_fn_t acknowledge_fn, void *ack_data)
{
    assert(data);
    irq_token_t irq_data;
    vm_t *vm;
    irq_data = (irq_token_t)data;

    irq_data->acknowledge_fn = acknowledge_fn;
    irq_data->ack_data = ack_data;

    vm = irq_data->vm;
    ps_irq_t irq = irq_data->irq;
    vm_combiner_irq_handler(vm, irq.irq.number);
    acknowledge_fn(ack_data);
}


irq_callback_fn_t get_custom_irq_handler(ps_irq_t irq)
{
    assert(irq.type == PS_INTERRUPT);

    if (irq.irq.number >= 32 && irq.irq.number <= 63) {
        /* IRQ combiner IRQs must be handled by the combiner directly */
        return vcombiner_irq_handler;
    }
    return NULL;
}

DEFINE_MODULE(plat, NULL, plat_init_module)

/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#define VM_PASSTHROUGH_IRQ_0() ( \
        (18, 1, 1, 10), \
        (10, 1, 1, 11) \
    ) \
    /**/

#define VM_PASSTHROUGH_IRQ_1() ( \
        (3, 0, 0, 3) \
    ) \
    /**/

#define PLAT_CONNECT_DEF() \
    /* Connect vm0 to vm1 with virtual ethernet */ \
    connection seL4VMNet vm0_to_vm1_net(from vm0.vm1net, to vm1.vm0net); \
    connection seL4GlobalAsynch vm0_net_ready(from vm0.vm1net_emit, to vm1.intready); \
    connection seL4GlobalAsynch vm1_net_ready(from vm1.vm0net_emit, to vm0.intready); \
    /**/

#define VM_CONFIGURATION_IOSPACES_0() ( \
    /* USB */ \
    0xf:0x2:0x00:0, \
    /* Ethernet */ \
    0xf:0x2:0x01:0 \
    /**/

#define VM_CONFIGURATION_IOSPACES_1() \
    /**/

#define VM_CONFIGURATION_MMIO_0() \
    /**/

#define VM_CONFIGURATION_MMIO_1() \
    /**/

#define VM_CONFIGURATION_IOPORT_0() ( \
        (0x2080, 0x209F, 0, "USB"), \
        (0x2000, 0x207F, 0, "Ethernet") \
    ) \
    /**/

#define VM_CONFIGURATION_IOPORT_1() ( \
        (0x2f8, 0x2FF, 1, "COM2"), \
        (0x2e8, 0x2ef, 1, "COM4") \
    ) \
    /**/

#define VM_GUEST_IMAGE "bzimage"
#define VM_GUEST_ROOTFS "rootfs.cpio"
#define VM_GUEST_CMDLINE  "console=ttyS0,115200 console=tty0 root=/dev/mem i8042.nokbd=y i8042.nomux=y i8042.noaux=y io_delay=udelay noisapnp"

#define PLAT_CONFIG_DEF() \
    vm0.simple_untyped24_pool = 7; \
    vm1.simple_untyped24_pool = 7; \
    vm0.vm1net_emit_attributes = "134479872"; /* BIT(18) + BIT(27) */ \
    vm1.vm0net_emit_attributes = "134479872"; /* BIT(18) + BIT(27) */ \
    vm0.vm1net_attributes ="06,00,00,20,12,13:13:0x9000"; \
    vm1.vm0net_attributes ="06,00,00,20,12,14:13:0x9000"; \
    vm0.kernel_cmdline = VM_GUEST_CMDLINE; \
    vm0.kernel_image = VM_GUEST_IMAGE; \
    vm0.kernel_relocs = VM_GUEST_IMAGE; \
    vm0.initrd_image = VM_GUEST_ROOTFS; \
    vm0.iospace_domain = 0x0f; \
    vm1.kernel_cmdline = VM_GUEST_CMDLINE; \
    vm1.kernel_image = VM_GUEST_IMAGE; \
    vm1.kernel_relocs = VM_GUEST_IMAGE; \
    vm1.initrd_image = VM_GUEST_ROOTFS; \
    vm1.iospace_domain = 0x10; \
    /**/

#define VM_GUEST_PASSTHROUGH_DEVICES_0() \
    {.ven = 0x15ad, .dev = 0x0774, .fun = -1, .irq = 10}, /* usb */ \
    {.ven = 0x1022, .dev = 0x2000, .fun = -1, .irq = 11}, /* eth */ \
    /**/

#define VM_GUEST_PASSTHROUGH_DEVICES_1() \
    /**/

/* We use a compressed image with the relocs attached
 * to the end */
#define VM_GUEST_RELOCS_0() VM_GUEST_IMAGE_0()
#define VM_GUEST_RELOCS_1() VM_GUEST_IMAGE_0()

#define VM_INIT_COMPONENT() \
    component Init0 { \
        include "ringbuf.h"; \
        dataport Ringbuf_t vm1net; \
        emits VMNet vm1net_emit; \
        VM_INIT_DEF() \
    } \
    component Init1 { \
        include "ringbuf.h"; \
        dataport Ringbuf_t vm0net; \
        emits VMNet vm0net_emit; \
        VM_INIT_DEF() \
    } \
    /**/


#define VM_ASYNC_DEVICE_BADGES_0() ( \
        (VM_FIRST_BADGE_BIT + 1, vm1net_notify) \
    ) \
    /**/

#define VM_ASYNC_DEVICE_BADGES_1() ( \
        (VM_FIRST_BADGE_BIT + 1, vm0net_notify) \
    ) \
    /**/

#define VM_INIT_SOURCE_DEFS() \
    void __attribute__((weak)) vm0net_init(vmm_t *vmm) { assert(!"should not be called");} \
    void __attribute__((weak)) vm1net_init(vmm_t *vmm) { assert(!"should not be called");} \
    void __attribute__((weak)) vm0net_notify() { assert(!"should not be called"); } \
    void __attribute__((weak)) vm1net_notify() { assert(!"should not be called"); } \
    /**/



#define VM_DEVICE_INIT_FN_0() ( \
        vm1net_init \
    ) \
    /**/

#define VM_DEVICE_INIT_FN_1() ( \
        vm0net_init \
    ) \
    /**/

#define PLAT_COMPONENT_DEF() \
    /**/

#define VM_ETHDRIVER_NUM_CLIENTS 0


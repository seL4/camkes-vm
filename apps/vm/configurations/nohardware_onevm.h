/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

/* We expect 1 VM */

#define PLAT_CONNECT_DEF() \
    /**/

#define VM_CONFIGURATION_IOSPACES_0() \
    /**/

#define VM_CONFIGURATION_MMIO_0() \
    /**/

#define VM_CONFIGURATION_IOPORT_0() \
    /**/

#define PLAT_CONFIG_DEF() \
    vm0.simple_untyped24_pool = 16; \
    /**/

#define VM_GUEST_PASSTHROUGH_DEVICES_0() \
    /**/

#define VCHAN_COMPONENT_DEF() \
    /**/

#define KERNEL_IMAGE "bzimage"
#define ROOTFS "rootfs.cpio"
#define VM_GUEST_CMDLINE "console=ttyS0,115200 console=tty0 root=/dev/mem i8042.nokbd=y i8042.nomux=y i8042.noaux=y io_delay=udelay noisapnp pci=nomsi"

#define VM_GUEST_IMAGE_0() KERNEL_IMAGE
#define VM_GUEST_ROOTFS_0() ROOTFS


/* We use a compressed image with the relocs attached
 * to the end */
#define VM_GUEST_RELOCS_0() VM_GUEST_IMAGE_0()

#define VM_GUEST_CMDLINE_0() VM_GUEST_CMDLINE

#define VM_GUEST_IOSPACE_DOMAIN_0() 0x0f


#define VM_INIT_COMPONENT() \
    component Init0 { \
        VM_INIT_DEF() \
    } \
    /**/

#define VM_ASYNC_DEVICE_BADGES_0() \
    /**/

#define VM_INIT_SOURCE_DEFS() \
    /**/

#define VM_DEVICE_INIT_FN_0() \
    /**/

#define PLAT_COMPONENT_DEF() \
    /**/

#define VM_ETHDRIVER_NUM_CLIENTS 0

#define HPET_IRQ() 20

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

#define VM_NUM_GUESTS 1

#define PLAT_CONNECT_DEF() \
    /**/

#define VM_CONFIGURATION_IOSPACES_0() \
    /**/

#define VM_CONFIGURATION_MMIO_0() \
    /**/

#define VM_CONFIGURATION_IOPORT_0() \
    /**/

#define KERNEL_IMAGE "bzimage"
#define ROOTFS "rootfs.cpio"
#define VM_GUEST_CMDLINE "console=ttyS0,115200 console=tty0 root=/dev/mem i8042.nokbd=y i8042.nomux=y i8042.noaux=y io_delay=udelay noisapnp pci=nomsi"

#define PLAT_CONFIG_DEF() \
    vm0.simple_untyped24_pool = 16; \
    vm0.guest_ram_mb = 80; \
    vm0.kernel_cmdline = VM_GUEST_CMDLINE; \
    vm0.kernel_image = KERNEL_IMAGE; \
    vm0.kernel_relocs = KERNEL_IMAGE; \
    vm0.initrd_image = ROOTFS; \
    vm0.iospace_domain = 0x0f; \
    /**/

#define VM_GUEST_PASSTHROUGH_DEVICES_0() \
    /**/

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


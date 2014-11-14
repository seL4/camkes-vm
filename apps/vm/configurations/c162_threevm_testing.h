/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

/* Configuration for three virtual machines running on the c162 */
#if CAMKES_VM_CONFIG == c162_threevm_testing

/* We want three guests */
#define VM_NUM_GUESTS 3

#define VM_CONFIGURATION_EXTRA_RAM
#define VM_CONFIGURATION_EXTRA_RAM_0() (0x21000000,0x5000000)
#define VM_CONFIGURATION_EXTRA_RAM_1() (0x27000000,0x5000000)
#define VM_CONFIGURATION_EXTRA_RAM_2() (0x2D000000,0x5000000)

#define PLAT_CONNECT_DEF() \
    /* Connect hardware interrupts for vm0 */ \
    connection seL4IOAPICHardwareInterrupt irq11(from ioapic.irq11, to IntMan0.irq10); \
    connection seL4IOAPICHardwareInterrupt irq23(from ioapic.irq23, to IntMan0.irq14); \
    /* Connect hardware interrupts for vm1 */ \
    connection seL4IOAPICHardwareInterrupt irq16(from ioapic.irq16, to IntMan1.irq10); \
    connection seL4IOAPICHardwareInterrupt irq18(from ioapic.irq18, to IntMan1.irq12); \
    /* Connect hardware interrupts for vm2 */ \
    connection seL4IOAPICHardwareInterrupt irq17(from ioapic.irq17, to IntMan2.irq11); \
    connection seL4IOAPICHardwareInterrupt irq3(from ioapic.irq3, to IntMan2. irq3); \
    /**/

#define VM_CONFIGURATION_IOSPACES_0() ( \
    /* Sata controller */ \
    "0xf:0x0:0x1f:2", \
    /* USB */ \
    "0xf:0x0:0x1d:0", \
    /* VME */ \
    "0xf:0x5:0x8:0" \
    ) \
    /**/

#define VM_CONFIGURATION_IOSPACES_1() ( \
    /* I2C */ \
    "0x10:0x0:0x1f:3", \
    /* Eth0 */ \
    "0x10:0x1:0x0:0" \
    ) \
    /**/

#define VM_CONFIGURATION_IOSPACES_2() ( \
    /* Eth1 */ \
    "0x11:0x1:0x0:1" \
    ) \
    /**/

#define VM_CONFIGURATION_MMIO_0() ( \
    /* VME */ \
    "0xe0000000:0x1000:12", \
    /* USB */ \
    "0xf2c07000:0x400:12" \
    ) \
    /**/

#define VM_CONFIGURATION_MMIO_1() ( \
    /* I2C */ \
    "0xf2c05000:0x100:12", \
    /* Eth 0 */ \
    "0xf1b80000:0x80000:12", \
    "0xf1c0c000:0x4000:12" \
    ) \
    /**/

#define VM_CONFIGURATION_MMIO_2() ( \
    /* Eth1 */ \
    "0xf1a80000:0x80000:12", \
    "0xf1c08000:0x4000:12" \
    ) \
    /**/

#define VM_CONFIGURATION_IOPORT_0() \
    /**/

#define VM_CONFIGURATION_IOPORT_1() ( \
    (0x162e, 0x162f, 0, "PLD Registers"), \
    (0x378, 0x37f, 0, "PLD Discrete I/O"), \
    (0x3f0, 0x3f7, 0, "PLD Extended Discrete I/O"), \
    (0x160E, 0x160F, 0, "CANbus 1 Two address"), \
    (0x1680, 0x16A0, 0, "CANbus 1 Multi address"), \
    (0x161E, 0x161F, 0, "CANbus 2 Two address"), \
    (0x16C0, 0x16E0, 0, "CANbus 2 Multi address"), \
    /* devices */ \
    (0xd060, 0xd07f, 1,), \
    (0xe000, 0xe01f, 1,) \
    ) \
    /**/

#define VM_CONFIGURATION_IOPORT_2() ( \
    (0x2f8, 0x2ff, 0, "COM2 Serial Port"), \
    (0x2e8, 0x2ef, 0, "COM4 Serial Port"), \
    /* device */ \
    (0xd040, 0xd05f, 1,) \
    ) \
    /**/

#define PLAT_CONFIG_DEF() \
    /* IOAPIC interrupt definitions. These are all PCI interrupts \
     * so are level triggered active low */ \
    ioapic.irq3_attributes = "3,0,0"; \
    ioapic.irq11_attributes = "11,1,1"; \
    ioapic.irq16_attributes = "16,1,1"; \
    ioapic.irq17_attributes = "17,1,1"; \
    ioapic.irq18_attributes = "18,1,1"; \
    ioapic.irq23_attributes = "23,1,1"; \
    vm0.untyped24_pool = 2; \
    vm1.untyped24_pool = 2; \
    vm2.untyped24_pool = 2; \
    /**/

#define VM_GUEST_PASSTHROUGH_DEVICES_0() \
    {.ven = 0x8086, .dev = 0x3b34, .fun = -1, .irq = 14}, /* USB */ \
    {.ven = 0x10e3, .dev = 0x0148, .fun = -1, .irq = 10}, /* VMEBus */ \
    /**/

#define VM_GUEST_PASSTHROUGH_DEVICES_1() \
    {.ven = 0x8086, .dev = 0x150e, .fun = 0, .irq = 10}, /* Network */ \
    {.ven = 0x8086, .dev = 0x3b30, .fun = -1, .irq = 12}, /* SMBus (I2C) */ \
    /**/

#define VM_GUEST_PASSTHROUGH_DEVICES_2() \
    {.ven = 0x8086, .dev = 0x150e, .fun = 1, .irq = 11}, /* Network */ \
    /**/

/* All our guests use the same kernel image, rootfs and cmdline */
#define C162_KERNEL_IMAGE "bzimage"
#define C162_ROOTFS "rootfs.cpio"
#define VM_GUEST_CMDLINE "console=ttyS0,115200 console=tty0 root=/dev/mem i8042.nokbd=y i8042.nomux=y i8042.noaux=y io_delay=udelay noisapnp"

#define VM_GUEST_IMAGE_0() C162_KERNEL_IMAGE
#define VM_GUEST_IMAGE_1() C162_KERNEL_IMAGE
#define VM_GUEST_IMAGE_2() C162_KERNEL_IMAGE

#define VM_GUEST_ROOTFS_0() C162_ROOTFS
#define VM_GUEST_ROOTFS_1() C162_ROOTFS
#define VM_GUEST_ROOTFS_2() C162_ROOTFS

/* We use a compressed image with the relocs attached
 * to the end */
#define VM_GUEST_RELOCS_0() VM_GUEST_IMAGE_0()
#define VM_GUEST_RELOCS_1() VM_GUEST_IMAGE_1()
#define VM_GUEST_RELOCS_2() VM_GUEST_IMAGE_2()

#define VM_GUEST_CMDLINE_0() VM_GUEST_CMDLINE
#define VM_GUEST_CMDLINE_1() VM_GUEST_CMDLINE
#define VM_GUEST_CMDLINE_2() VM_GUEST_CMDLINE

#define VM_GUEST_IOSPACE_DOMAIN_0() 0x0f
#define VM_GUEST_IOSPACE_DOMAIN_1() 0x10
#define VM_GUEST_IOSPACE_DOMAIN_2() 0x11

#define HPET_IRQ() 20

#endif

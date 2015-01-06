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

#define VM_PASSTHROUGH_IRQ_0() ( \
        (11, 1, 1, 10), \
        (23, 1, 1, 14) \
    ) \
    /**/

#define VM_PASSTHROUGH_IRQ_1() ( \
        (18, 1, 1, 12) \
    ) \
    /**/

#define VM_PASSTHROUGH_IRQ_2() ( \
        (17, 1, 1, 11), \
        (3, 0, 0, 3) \
    ) \
    /**/

#define PLAT_CONNECT_DEF() \
    /* Give ethernet driver same output as its vm */ \
    connection seL4RPCCall eth_putchar(from ethdriver0.putchar, to serial.vm1); \
    /* Give echo output */ \
    connection seL4RPCCall echo_putchar(from echo.putchar, to serial.vm1); \
    /* Give udp server output */ \
    connection seL4RPCCall udpserver_putchar (from udpserver.putchar, to serial.vm1); \
    /* Connect ethernet driver to vm 1 */ \
    connection seL4SharedData eth_packet1(from ethdriver0.packet0, to vm1.packet); \
    connection seL4RPCCall eth_driver1(from vm1.ethdriver, to ethdriver0.client0); \
    connection seL4AsynchBind eth_rx_ready1(from ethdriver0.rx_ready0, to vm1.intready); \
    /* Connect ethernet driver to udpserver */ \
    connection seL4RPCCall udpserver_putchar(from udpserver.putchar, to serial.vm1); \
    connection seL4SharedData eth_packet2(from ethdriver0.packet1, to udpserver.packet); \
    connection seL4RPCCall eth_driver2(from udpserver.ethdriver, to ethdriver0.client1); \
    connection seL4Asynch eth_rx_ready2(from ethdriver0.rx_ready1, to udpserver.eth_rx_ready); \
    /* Define hardware resources for ethdriver0 */ \
    connection seL4HardwareMMIO ethdrivermmio1(from ethdriver0.EthDriver, to HWEthDriver.mmio); \
    connection seL4IOAPICHardwareInterrupt hwethirq(from HWEthDriver.irq, to ethdriver0.irq); \
    /* UDP connections for echo server */ \
    connection seL4UDPRecv udp_echo_recv(from echo.echo_recv, to udpserver.client_recv); \
    connection seL4UDPSend udp_echo_send(from echo.echo_send, to udpserver.client_send); \
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
    "0x10:0x0:0x1f:3" \
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
    "0xf2c05000:0x100:12" \
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
    (0xe000, 0xe01f, 0,) \
    ) \
    /**/

#define VM_CONFIGURATION_IOPORT_2() ( \
    (0x2f8, 0x2ff, 0, "COM2 Serial Port"), \
    (0x2e8, 0x2ef, 0, "COM4 Serial Port"), \
    /* device */ \
    (0xd040, 0xd05f, 0,) \
    ) \
    /**/

#define PLAT_CONFIG_DEF() \
    vm0.simple_untyped24_pool = 2; \
    vm1.simple_untyped24_pool = 2; \
    vm2.simple_untyped24_pool = 2; \
    HWEthDriver.mmio_attributes = "0xf1b80000:0x80000"; \
    HWEthDriver.irq_attributes = "16,1,1"; \
    ethdriver0.simple = true; \
    ethdriver0.cnode_size_bits = 12; \
    ethdriver0.iospace = "0x12:0x1:0x0:0"; \
    ethdriver0.simple_untyped20_pool = 2; \
    ethdriver0.rx_ready0_attributes = "134348800"; /* BIT(17) + BIT(27)*/ \
    udpserver.client_recv_attributes = "8,7"; \
    udpserver.client_send_attributes = "7"; \
    /**/

#define VM_GUEST_PASSTHROUGH_DEVICES_0() \
    {.ven = 0x8086, .dev = 0x3b34, .fun = -1, .irq = 14}, /* USB */ \
    {.ven = 0x10e3, .dev = 0x0148, .fun = -1, .irq = 10}, /* VMEBus */ \
    /**/

#define VM_GUEST_PASSTHROUGH_DEVICES_1() \
    {.ven = 0x8086, .dev = 0x3b30, .fun = -1, .irq = 12}, /* SMBus (I2C) */ \
    /**/

#define VM_GUEST_PASSTHROUGH_DEVICES_2() \
    {.ven = 0x8086, .dev = 0x150e, .fun = 1, .irq = 11}, /* Network */ \
    /**/

/* All our guests use the same kernel image, rootfs and cmdline */
#define C162_KERNEL_IMAGE "bzimage"
#define C162_ROOTFS "rootfs.cpio"
#define VM_GUEST_CMDLINE "console=ttyS0,115200 console=tty0 root=/dev/mem i8042.nokbd=y i8042.nomux=y i8042.noaux=y io_delay=udelay noisapnp pci=nomsi"

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

#define VM_INIT_COMPONENT() \
    maybe dataport Buf packet; \
    maybe uses Ethdriver ethdriver; \
    /**/

#define VM_ASYNC_DEVICE_BADGES_0() \
    /**/

#define VM_ASYNC_DEVICE_BADGES_1() ( \
        (VM_FIRST_BADGE_BIT, virtio_net_notify) \
    ) \
    /**/

#define VM_ASYNC_DEVICE_BADGES_2() \
    /**/

#define VM_DEVICE_INIT_FN_0() \
    /**/

#define VM_DEVICE_INIT_FN_1() ( \
        make_virtio_net \
    ) \
    /**/

#define VM_DEVICE_INIT_FN_2() \
    /**/

#define PLAT_COMPONENT_DEF() \
    component Ethdriver ethdriver0; \
    component HWEthDriver HWEthDriver; \
    component Echo echo; \
    component UDPServer udpserver; \
    /**/

#define VM_NUM_ETHDRIVERS 1

#define VM_ETHDRIVER_IOSPACE_0() 0x12
#define VM_ETHDRIVER_PCI_BDF_0() ( (1 << 8) | (0 << 3) | 0)

#define VM_ETHDRIVER_NUM_CLIENTS 2
#define VM_ETHDRIVER_CLIENTS_0() ( \
    (06, 00, 00, 11, 12, 13), \
    (06, 00, 00, 12, 13, 14) \
    ) \
    /**/

#define HPET_IRQ() 20

#endif

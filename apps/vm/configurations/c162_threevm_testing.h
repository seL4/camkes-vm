/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

/* This entire configuration file is a bit of a mess. There is various duplication
 * of information, not to mention the fact that this is all done in macros.
 * Ideally most of the logic for the VMs configuration could be moved to
 * a template and camkes configurations could be used along with some slightly
 * nicer code generation.
 *
 * When modifying this it is important to remember that some macros just expand to
 * their contents, and some macros are expected to be used in boost list expansions
 * and care must be taken to get the syntax correct else difficult to debug build
 * errors will occur. For a boost list an empty list would be defined here as
   #define EMPTY_LIST()
 * Where as a list with one element is
   #define ONE_ELEMENT() \
        element \
   )
 * And multiple elements becomes
   #define MULTI_ELEMENT() \
        element1, \
        element2, \
        element3
   )
 * Importantly note that you cannot have a trailing comma on the last element
 * of a list
 *
 * Many options have _N varients. The N here refers to which VM number the option
 * applies to. This is the mechanism by which different VMs can be given different
 * resources */


/* If VM_CONFIGURATION_EXTRA_RAM is defined then the corresponding _N definitions
 * will be used to find ranges for extra ram. These are boost lists and contain
 * physical address and 2^n size pairs. Each range must fall within a single
 * untyped that is given to the rootserver (capdl-loader) by the kernel. The capdl-loader
 * has a configuration option that can be turned on that will print out what
 * device untypeds it has received */
#define VM_CONFIGURATION_EXTRA_RAM
#define VM_CONFIGURATION_EXTRA_RAM_0() ( \
        (0x21000000,24), \
        (0x22000000,25), \
        (0x24000000,24) \
    ) \
    /**/
#define VM_CONFIGURATION_EXTRA_RAM_1() ( \
        (0x27000000,24), \
        (0x28000000,25), \
        (0x2A000000,24) \
    ) \
    /**/
#define VM_CONFIGURATION_EXTRA_RAM_2() ( \
        (0x2D000000,25), \
        (0x2F000000,24), \
        (0x30000000,24) \
    ) \
    /**/

/* Passthrough IRQs are used to give a guest a direct hardware interrupt. The format here
 * is (source_irq, level_triggered, active_low, dest_irq).
 * source_irq - The interrupt index on the I/O APIC for this device. Note that seL4
 *              does not interrupt remapping of any kind and will just use the default
 * level_triggered - 1 if level triggred 0 if edge triggered. Generally PCI devices are
 *                   level triggered and old ISA devices (serial ports etc) are edge
 *                   triggered
 * active_low - 1 if triggered when low, 0 if triggered when high. Generally
 *              PCI interrupts are active low, and ISA interrupts are active high
 * dest_irq - The IRQ number on the PIC we emulate to Linux to deliver this interrupt
 *            to. Frequently the source and dest irqs will need to be different,
 *            in this case the VMM supports a way to indicate in the PCI config space
 *            virtualization where the interrupt is. See the comments on
 *            VM_GUEST_PASSTHROUGH_DEVICES
 */
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

/* Camkes definitions for defining any connections that are specific
 * to this VM configuration. In this case we are defining connections
 * for UDPServer as well as inter-vm communication
 */
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
    connection seL4GlobalAsynch eth_rx_ready1(from ethdriver0.rx_ready0, to vm1.intready); \
    /* Connect ethernet driver to udpserver */ \
    connection seL4SharedData eth_packet2(from ethdriver0.packet1, to udpserver.packet); \
    connection seL4RPCCall eth_driver2(from udpserver.ethdriver, to ethdriver0.client1); \
    connection seL4Asynch eth_rx_ready2(from ethdriver0.rx_ready1, to udpserver.eth_rx_ready); \
    /* Define hardware resources for ethdriver0 */ \
    connection seL4HardwareMMIO ethdrivermmio1(from ethdriver0.EthDriver, to HWEthDriver.mmio); \
    connection seL4IOAPICHardwareInterrupt hwethirq(from HWEthDriver.irq, to ethdriver0.irq); \
    /* UDP connections for echo server */ \
    connection seL4UDPRecv udp_echo_recv(from echo.echo_recv, to udpserver.client_recv); \
    connection seL4UDPSend udp_echo_send(from echo.echo_send, to udpserver.client_send); \
    /* Connect vm0 to vm2 with virtual ethernet */ \
    connection seL4VMNet vm0_to_vm2_net(from vm0.vm2net, to vm2.vm0net); \
    connection seL4GlobalAsynch vm0_net_ready(from vm0.vm2net_emit, to vm2.intready); \
    connection seL4GlobalAsynch vm2_net_ready(from vm2.vm0net_emit, to vm0.intready); \
    /**/

/* Define any IOSpaces that need be created and populated with mappings
 * in the IOMMU. Each entry here is the format
   "iospace_domain:pci_bus:pci_device:pci_fun"
 * The iospace domain needs to match the definition in
 * VM_GUEST_IOSPACE_DOMAIN_N There needs to be a definition here for
 * every PCI device given to the guest in VM_GUEST_PASSTHROUGH_DEVICES
 */
#define VM_CONFIGURATION_IOSPACES_0() ( \
    /* Sata controller */ \
    0xf:0x0:0x1f:2, \
    /* USB */ \
    0xf:0x0:0x1d:0, \
    /* VME */ \
    0xf:0x5:0x8:0 \
    ) \
    /**/

#define VM_CONFIGURATION_IOSPACES_1() ( \
    /* I2C */ \
    0x10:0x0:0x1f:3 \
    ) \
    /**/

#define VM_CONFIGURATION_IOSPACES_2() ( \
    /* Eth1 */ \
    0x11:0x1:0x0:1 \
    ) \
    /**/

/* This is a list of any memory mapped IO regions that will be needed when
 * giving the guest PCI devices. When the VMM gives the guest access to
 * devices in VM_GUEST_PASSTHROUGH_DEVICES, the memory regions for the
 * bars need to be in this list. Essentially this list requests the
 * capdl-loader give the VMM these resources, which we then may or may
 * not actually give to Linux, depending on whether a device that Linux
 * uses actually needs it. Format is
   "physical_address:size:page_bits"
 * Size can be less than a page, although in practice the actual region
 * requested will be round up to a multiple of the page size.
 * page_bits is size of the frame that backs this region. This is to account
 * for seL4 potentially giving large frames for device regions that can
 * support it. In practice this doesn't happen at the moment and this
 * value should always be 12
 */
#define VM_CONFIGURATION_MMIO_0() ( \
    /* VME */ \
    0xe0000000:0x1000:12, \
    /* USB */ \
    0xf2c07000:0x400:12 \
    ) \
    /**/

#define VM_CONFIGURATION_MMIO_1() ( \
    /* I2C */ \
    0xf2c05000:0x100:12 \
    ) \
    /**/

#define VM_CONFIGURATION_MMIO_2() ( \
    /* Eth1 */ \
    0xf1a80000:0x80000:12, \
    0xf1c08000:0x4000:12 \
    ) \
    /**/

/* Definitions of legacy I/O ports that should be given to each guest.
 * The format of each entry is
   (first_port, last_port, not_PCI, description)
 * the first and last ports are inclusive
 * not_PCI is an option that exists for legacy reasons and indicates
 * whether the I/O port will be found and given to the guest from
 * scanning the bars when giving passthrough devices, or whether it
 * should just be given to the guest. This means that if 1 is given
 * and port range is also in a device, then an error will occur as
 * the VMM will attempt to give the port twice to the guest.
 * Similarly if 0 is given and the port does not appear in a PCI
 * device then the range will *not* be given to Linux
 */
#define VM_CONFIGURATION_IOPORT_0() \
    /**/

#define VM_CONFIGURATION_IOPORT_1() ( \
    (0x162e, 0x162f, 1, "PLD Registers"), \
    (0x378, 0x37f, 1, "PLD Discrete I/O"), \
    (0x3f0, 0x3f7, 1, "PLD Extended Discrete I/O"), \
    (0x160E, 0x160F, 1, "CANbus 1 Two address"), \
    (0x1680, 0x16A0, 1, "CANbus 1 Multi address"), \
    (0x161E, 0x161F, 1, "CANbus 2 Two address"), \
    (0x16C0, 0x16E0, 1, "CANbus 2 Multi address"), \
    /* devices */ \
    (0xe000, 0xe01f, 0, "Some device") \
    ) \
    /**/

#define VM_CONFIGURATION_IOPORT_2() ( \
    (0x2f8, 0x2ff, 1, "COM2 Serial Port"), \
    (0x2e8, 0x2ef, 1, "COM4 Serial Port"), \
    /* device */ \
    (0xd040, 0xd05f, 0, "Some device") \
    ) \
    /**/

/* All our guests use the same kernel image, rootfs and cmdline */
#define C162_KERNEL_IMAGE "bzimage"
#define C162_ROOTFS "rootfs.cpio"
#define VM_GUEST_CMDLINE "console=ttyS0,115200 console=tty0 root=/dev/mem i8042.nokbd=y i8042.nomux=y i8042.noaux=y io_delay=udelay noisapnp pci=nomsi"

/* camkes definitions that will get placed in the configuration section
 * of the camkes assembly. Most of the definitions here are to do with
 * the udp server and vm to vm communication that is specific to this
 * configuration. The most important option being set for each VMM is the
   simple_untypedX_pool = Y
 * This gives Y untypeds each of size 2^X to th VMM. Multiple definitions
 * with different X can be given for each VMM. This memory will be used
 * for almost all allocations (including Linux guest memory) by the VMM.
 * The exception is that physical frames will be preferentially allocated
 * from the EXTRA_GUEST_RAM regions instead. Because this configuration
 * has an EXTRA_GUEST_RAM_REGION we make do with relatively small ammounts
 * of untyped memory
 */
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
    ethdriver0.rx_ready0_global_endpoint = "vm0"; \
    ethdriver0.rx_ready0_badge = "134742016"; /* BIT(19) + BIT(27)*/ \
    udpserver.client_recv_attributes = "8,7"; \
    udpserver.client_send_attributes = "7"; \
    vm0.vm2net_emit_global_endpoint = "vm2"; \
    vm2.vm0net_emit_global_endpoint = "vm0"; \
    vm0.vm2net_emit_badge = "134479872"; /* BIT(18) + BIT(27) */ \
    vm2.vm0net_emit_badge = "134479872"; /* BIT(18) + BIT(27) */ \
    vm0.vm2net_attributes ="06,00,00,20,12,13:13:0x9000"; \
    vm2.vm0net_attributes ="06,00,00,20,12,14:13:0x9000"; \
    vm0.kernel_cmdline = VM_GUEST_CMDLINE; \
    vm0.kernel_image = C162_KERNEL_IMAGE; \
    vm0.kernel_relocs = C162_KERNEL_IMAGE; \
    vm0.initrd_image = c162_ROOTFS; \
    vm0.iospace_domain = 0x0f; \
    vm1.kernel_cmdline = VM_GUEST_CMDLINE; \
    vm1.kernel_image = C162_KERNEL_IMAGE; \
    vm1.kernel_relocs = C162_KERNEL_IMAGE; \
    vm1.initrd_image = c162_ROOTFS; \
    vm0.iospace_domain = 0x10; \
    vm2.kernel_cmdline = VM_GUEST_CMDLINE; \
    vm2.kernel_image = C162_KERNEL_IMAGE; \
    vm2.kernel_relocs = C162_KERNEL_IMAGE; \
    vm2.initrd_image = c162_ROOTFS; \
    vm0.iospace_domain = 0x11; \
    /**/

/* List of pci devices that should be given as passthrough to the guest
 * Format is
   {.ven = pci_vendor_id, .dev = pci_device_id, .fun = pci_function, .irq = irq_remap}
 * The pci_function is an option argument, and is used as a rudimentary
 * way of picking when there are multiple of the same device. This assumes
 * that duplicate devices will be on the same pci bus/device and only
 * differ by function. A value of -1 means the device can appear at
 * any function
 * irq_remap is where we will tell the guest the IRQ is. This should probably
 * match at least one dest_irq in VM_PASSTHROUGH_IRQ. Can be -1 to use the
 * default *PIC* IRQ
 */
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

#define VM_INIT_COMPONENT() \
    component Init0 { \
        include "ringbuf.h"; \
        dataport Ringbuf_t vm2net; \
        emits VMNet vm2net_emit; \
        VM_INIT_DEF() \
    } \
    component Init1 { \
        VM_INIT_DEF() \
        dataport Buf packet; \
        uses Ethdriver ethdriver; \
    } \
    component Init2 { \
        include "ringbuf.h"; \
        dataport Ringbuf_t vm0net; \
        emits VMNet vm0net_emit; \
        VM_INIT_DEF() \
    } \
    /**/

#define VM_ASYNC_DEVICE_BADGES_0() ( \
        (VM_FIRST_BADGE_BIT, vm2net_notify) \
    ) \
    /**/

#define VM_ASYNC_DEVICE_BADGES_1() ( \
        (VM_FIRST_BADGE_BIT + 1, virtio_net_notify) \
    ) \
    /**/

#define VM_ASYNC_DEVICE_BADGES_2() ( \
        (VM_FIRST_BADGE_BIT, vm0net_notify) \
    ) \
    /**/

#define VM_INIT_SOURCE_DEFS() \
    void __attribute__((weak)) vm0net_init(vmm_t *vmm) { assert(!"should not be called");} \
    void __attribute__((weak)) vm2net_init(vmm_t *vmm) { assert(!"should not be called");} \
    void __attribute__((weak)) vm0net_notify() { assert(!"should not be called"); } \
    void __attribute__((weak)) vm2net_notify() { assert(!"should not be called"); } \
    /**/

#define VM_DEVICE_INIT_FN_0() ( \
        vm2net_init \
    ) \
    /**/

#define VM_DEVICE_INIT_FN_1() ( \
        make_virtio_net \
    ) \
    /**/

#define VM_DEVICE_INIT_FN_2() ( \
        vm0net_init \
    ) \
    /**/

#define VCHAN_COMPONENT_DEF() \
    static camkes_vchan_con_t vchan_camkes_component = { \
    }; \
    /**/
#define VCHAN_COMPONENT_INIT_MEM() \
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



/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#define VM_NUM_GUESTS 1

/* 
 * Configuration for one virtual machine running on a DQ67EP 
 * motherboard and an Intel i7-2600S 
 * Uses passthrough for the root file system on ATA1, no initrd.
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


/* 
 * If VM_CONFIGURATION_EXTRA_RAM is defined then the corresponding _N
 * definitions will be used to find ranges for extra ram. These are
 * boost lists and contain physical address and 2^n size pairs. Each
 * range must fall within a single untyped that is given to the
 * rootserver (capdl-loader) by the kernel. The capdl-loader has a
 * configuration option that can be turned on that will print out what
 * device untypeds it has received
 */

#define VM_CONFIGURATION_EXTRA_RAM
#define VM_CONFIGURATION_EXTRA_RAM_0() (	\
	(0x40000000, 29), /* 512M */		\
	(0x60000000, 30), /* 1G */		\
	(0xA0000000, 30)  /* 1G */		\
    )						\
/**/

/* 
 * Passthrough IRQs are used to give a guest a direct hardware
 * interrupt. The format here is (source_irq, level_triggered,
 * active_low, dest_irq).  source_irq - The interrupt index on the I/O
 * APIC for this device. Note that seL4 does not interrupt remapping
 * of any kind and will just use the default
 *
 * level_triggered - 1 if level triggred 0 if edge triggered. Generally 
 *		     PCI devices are level triggered and old ISA devices 
 *		     (serial ports etc) are edge triggered
 * active_low - 1 if triggered when low, 0 if triggered when high. Generally
 *              PCI interrupts are active low, and ISA interrupts are
 *	        active high
 * dest_irq - The IRQ number on the PIC we emulate to Linux to deliver
 * 	      this interrupt to. Frequently the source and dest irqs
 *	      will need to be different, in this case the VMM supports
 *	      a way to indicate in the PCI config space
 *	      virtualization where the interrupt is. See the
 *	      comments on VM_GUEST_PASSTHROUGH_DEVICES
 */

/*
 * VM0 -- SATA, ETH, VGA, Serial, USB
 */
#define VM_PASSTHROUGH_IRQ_0() (		\
        (22, 1, 1, 7), \
  /* e1000e, 00:19.0 0200: 8086:1502 */         \
        (20, 1, 1, 9),                           \
  /* xhcd 02:00.0 0c03 1033:0194 */             \
        (18, 1, 1, 10),                         \
  /* ehcd_hcd, usb3 00:1a.0 0c03: 8086:1c26 */  \
        (16, 1, 1, 11),				\
  /* ata_PIIX_3, 00:1f.2 0101: 8086:1c00 */     \
        (14, 0, 0, 14),                         \
  /* Legacy IDE */				\
        (15, 0, 0, 15)                         \
  /* SMBus 00:1f.3 0c05: 8086:1c22 */           \
	/*(18, 1, 1, 12)*/				\
   ) \
  /**/

#if 0
/* 
 * Camkes definitions for defining any connections that are specific
 * to this VM configuration. In this case we are defining connections
 * for UDPServer as well as inter-vm communication
 */
#define PLAT_CONNECT_DEF() \
    /* Give ethernet driver same output as its vm */ \
    connection seL4RPCCall eth_putchar(from ethdriver0.putchar, to serial.vm1); \
    /* Connect ethernet driver to vm 1 */ \
    connection seL4SharedData eth_packet1(from ethdriver0.packet0, to vm1.packet); \
    connection seL4RPCCall eth_driver1(from vm1.ethdriver, to ethdriver0.client0); \
    connection seL4Asynch eth_rx_ready1(from ethdriver0.rx_ready0, to vm1.eth_rx_ready); \
    /* Connect ethernet driver to echo */ \
    connection seL4RPCCall echo_putchar(from echo.putchar, to serial.vm1); \
    connection seL4SharedData eth_packet2(from ethdriver0.packet1, to echo.packet); \
    connection seL4RPCCall eth_driver2(from echo.ethdriver, to ethdriver0.client1); \
    connection seL4Asynch eth_rx_ready2(from ethdriver0.rx_ready1, to echo.eth_rx_ready); \
    /* Define hardware resources for ethdriver0 */ \
    connection seL4HardwareMMIO ethdrivermmio1(from ethdriver0.EthDriver, to HWEthDriver.mmio); \
    connection seL4IOAPICHardwareInterrupt hwethirq(from HWEthDriver.irq, to ethdriver0.irq); \
    /**/
#endif

/* Define any IOSpaces that need be created and populated with
 * mappings in the IOMMU. Each entry here is the format
 * "iospace_domain:pci_bus:pci_device:pci_fun" 
 *
 * The iospace domain needs to match the definition in
 * VM_GUEST_IOSPACE_DOMAIN_N. There needs to be a definition here for
 * every PCI device given to the guest in VM_GUEST_PASSTHROUGH_DEVICES
 */

/* "Domain:Bus:Dev:Function" */
#define VM_CONFIGURATION_IOSPACES_0() ( \
  /* AGP-GART */				\
  0x0f:0x0:0x0:0,				\
  /* Audio */					\
  0x0f:0x0:0x1b:0,				\
  /* i915 */					\
  0x0f:0x0:0x2:0,				\
  /* Generic ATA */				\
  0x0f:0x0:0x16:2,				\
  /* IDE controller */				\
  0x0f:0x0:0x1f:2,				\
  /* IDE controller */				\
  0x0f:0x0:0x1f:5,				\
  /* e1000e */					\
  0x0f:0x0:0x19:0,				\
  /* USB 0*/					\
  0x0f:0x0:0x1a:0,				\
  /* USB 1 */					\
  0x0f:0x0:0x1d:0,				\
  /* USB 2 */					\
  0x0f:0x2:0x0:0,				\
  /* SMB */					\
  0x0f:0x0:0x1f:3				\
  )	\
    /**/



/* 
 * This is a list of any memory mapped IO regions that will be needed
 * when giving the guest PCI devices. When the VMM gives the guest
 * access to devices in VM_GUEST_PASSTHROUGH_DEVICES, the memory
 * regions for the bars need to be in this list. Essentially this list
 * requests the capdl-loader give the VMM these resources, which we
 * then may or may not actually give to Linux, depending on whether a
 * device that Linux uses actually needs it. 
 *
 * Format is  "physical_address:size:page_bits" 
 *
 * Size can be less than a page, although in practice the actual
 * region requested will be rounded up to a multiple of the page size.
 *
 * page_bits is size of the frame that backs this region. This is to
 * account for seL4 potentially giving large frames for device regions
 * that can support it. In practice this doesn't happen at the moment
 * and this value should always be 12
 */

#define VM_CONFIGURATION_MMIO_0() ( \
    /* Audio */			\
    0xfe520000:0x4000:12,		\
     /* VGA i915 */			\
    0xfe000000:0x400000:12,		\
    0xd0000000:10000000:12,		\
    /* USB @1a.0*/                      \
    0xfe526000:0x400:12,              \
    /* USB @1d.0 */                     \
    0xfe525000:0x400:12,              \
     /* USB @2:0.0 */                   \
    0xfe400000:0x2000:12,             \
     /* e1000e */                       \
    0xfe500000:0x20000:12,            \
    0xfe527000:0x1000:12,		\
        /* SMBus */                     \
    0xfe524000:0x100:12               \
    )                                   \
    /**/

/*
 * Definitions of legacy I/O ports that should be given to each guest.
 *
 * The format of each entry is
 *  (first_port, last_port, not_PCI, description)
 *
 * the first and last ports are inclusive
 *
 * not_PCI is an option that exists for legacy reasons and indicates
 * whether the I/O port will be found and given to the guest from
 * scanning the BARS when giving passthrough devices, or whether it
 * should just be given to the guest. This means that if 1 is given
 * and port range is also in a device, then an error will occur as the
 * VMM will attempt to give the port twice to the guest.  Similarly if
 * 0 is given and the port does not appear in a PCI device then the
 * range will *not* be given to Linux
 */

/* first, last, alloc-to-vm, name */
#define VM_CONFIGURATION_IOPORT_0() (		\
	(0x0064, 0x0064, 1, "PS/2 controller (for reboot)"),	   \
	(0x0170, 0x017f, 1, "IDE1 Legacy ports"),	   \
	(0x01f0, 0x01f7, 1, "IDE0 Legacy ports"),	\
	(0x0376, 0x0376, 1, "IDE1 DCR"),	\
	(0x03f6, 0x03f6, 1, "IDE0 DCR"),	\
	(0xf040, 0xf05f, 0, "SMB"),		\
	(0xf060, 0xf07f, 0, "eth0"),		\
	(0xf000, 0xf03f, 0, "VGA"),		\
	(0xf0e0, 0xf0ef, 0, "IDE3 BAR 5"),	\
	(0xf0f0, 0xf0ff, 0, "IDE3 BAR 4"),	\
	(0xf100, 0xf103, 0, "IDE3 BAR 3"),	\
	(0xf110, 0xf117, 0, "IDE3 BAR 2"),	\
	(0xf120, 0xf123, 0, "IDE3 BAR 1"),	\
	(0xf130, 0xf137, 0, "IDE3 BAR 0"),	\
	(0xf150, 0xf15f, 0, "IDE2 BAR 4"),	\
	(0xf160, 0xf163, 0, "IDE2 BAR 3"),	\
	(0xf170, 0xf177, 0, "IDE2 BAR 2"),	\
	(0xf180, 0xf183, 0, "IDE2 BAR 1"),	\
	(0xf190, 0xf197, 0, "IDE2 BAR 0")	\
   ) \
    /**/

/* All our guests use the same kernel image, rootfs and cmdline */
#define DQ67EP_KERNEL_IMAGE "bzimage-DQ67EP"
#define DQ67EP_ROOTFS "DQ67EP.cpio"
#define VM_GUEST_CMDLINE "initrd=rootfs.cpio console=ttyS0,115200 i8042.nokbd=y i8042.nomux=y i8042.noaux=y io_delay=udelay noisapnp pci=nomsi noacpi"

/* 
 * camkes definitions that will get placed in the configuration section
 * of the camkes assembly. Most of the definitions here are to do with
 * the udp server and vm to vm communication that is specific to this
 * configuration. The most important option being set for each VMM is the
 *    simple_untypedX_pool = Y
 *
 * This gives Y untypeds each of size 2^X to th VMM. Multiple
 * definitions with different X can be given for each VMM. This memory
 * will be used for almost all allocations (including Linux guest
 * memory) by the VMM.  The exception is that physical frames will be
 * preferentially allocated from the EXTRA_GUEST_RAM regions
 * instead. Because this configuration has an EXTRA_GUEST_RAM_REGION
 * we make do with relatively small amounts of untyped memory
 */

#define PLAT_CONFIG_DEF() \
    vm0.simple_untyped24_pool = 16; \
    vm0.guest_ram_mb = 80; \
    vm0.kernel_cmdline = VM_GUEST_CMDLINE; \
    vm0.kernel_image = DQ67EP_KERNEL_IMAGE; \
    vm0.kernel_relocs = DQ67EP_KERNEL_IMAGE; \
    vm0.initrd_image = DQ67EP_ROOTFS; \
    vm0.iospace_domain = 0x0f; \
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
  {.ven = 0x8086, .dev = 0x0100, .fun = -1, .irq = -1},	/* AGP-GART */ \
  {.ven = 0x8086, .dev = 0x0102, .fun = -1, .irq = 11}, /* i915 */	\
  {.ven = 0x8086, .dev = 0x1c20, .fun = -1, .irq = 7},  /* Audio */	\
  {.ven = 0x1033, .dev = 0x0194, .fun = -1, .irq = 10}, /* USB3 */	\
  {.ven = 0x8086, .dev = 0x1502, .fun = -1, .irq = 9},  /* eth0 */      \
  {.ven = 0x8086, .dev = 0x1c00, .fun = -1, .irq = 14}, /* IDE */       \
  {.ven = 0x8086, .dev = 0x1c22, .fun = -1, .irq = 10}, /* SMB */	\
  /*{.ven = 0x8086, .dev = 0x1c26, .fun = -1, .irq = 11},*/ /* USB0 */      \
  /*{.ven = 0x8086, .dev = 0x1c2d, .fun = -1, .irq = 11},*/ /* USB1 */      \
  {.ven = 0x8086, .dev = 0x1c3c, .fun = -1, .irq = 13}  /* IDE */	\
    /**/

#define PLAT_CONNECT_DEF() \
    /**/

#define PLAT_COMPONENT_DEF() \
    /**/

#define VM_ASYNC_DEVICE_BADGES_0() \
    /**/

#define VM_INIT_SOURCE_DEFS() \
    /**/

#define VM_DEVICE_INIT_FN_0() \
    /**/

#define VM_INIT_COMPONENT() \
    component Init0 { \
        VM_INIT_DEF() \
    } \
    /**/

#define VM_ETHDRIVER_NUM_CLIENTS 0

/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#include <autoconf.h>

#define _VAR_STRINGIZE(...) #__VA_ARGS__
#define VAR_STRINGIZE(...) _VAR_STRINGIZE(__VA_ARGS__)

#define _CAT(x, y) x ## y
#define CAT(x, y) _CAT(x, y)

#define VM_CONFIGURATION_HEADER() VAR_STRINGIZE(CAMKES_VM_CONFIG.h)
#include VM_CONFIGURATION_HEADER()

/* For all the async sources on the intready endpoint the high bit
 * is set to indicate that an async event occured, and the low bits
 * indicate which async events */

/* The timer completions are also on the interrupt manager badge */
#define VM_INIT_TIMER_BADGE 134217729 /* BIT(27) | BIT(0) */

#define VM_PIC_BADGE_IRQ_0 134217730 /* BIT(27) | BIT(1) */
#define VM_PIC_BADGE_IRQ_1 134217732 /* BIT(27) | BIT(2) */
#define VM_PIC_BADGE_IRQ_2 134217736 /* BIT(27) | BIT(3) */
#define VM_PIC_BADGE_IRQ_3 134217744 /* BIT(27) | BIT(4) */
#define VM_PIC_BADGE_IRQ_4 134217760 /* BIT(27) | BIT(5) */
#define VM_PIC_BADGE_IRQ_5 134217792 /* BIT(27) | BIT(6) */
#define VM_PIC_BADGE_IRQ_6 134217856 /* BIT(27) | BIT(7) */
#define VM_PIC_BADGE_IRQ_7 134217984 /* BIT(27) | BIT(8) */
#define VM_PIC_BADGE_IRQ_8 134218240 /* BIT(27) | BIT(9) */
#define VM_PIC_BADGE_IRQ_9 134218752 /* BIT(27) | BIT(10) */
#define VM_PIC_BADGE_IRQ_10 134219776 /* BIT(27) | BIT(11) */
#define VM_PIC_BADGE_IRQ_11 134221824 /* BIT(27) | BIT(12) */
#define VM_PIC_BADGE_IRQ_12 134225920 /* BIT(27) | BIT(13) */
#define VM_PIC_BADGE_IRQ_13 134234112 /* BIT(27) | BIT(14) */
#define VM_PIC_BADGE_IRQ_14 134250496 /* BIT(27) | BIT(15) */
#define VM_PIC_BADGE_IRQ_15 134283264 /* BIT(27) | BIT(16) */

#define VM_PIC_BADGE_SERIAL_HAS_DATA 134348800 /* BIT(27) | BIT(17) */
#define VM_PIC_BADGE_VCHAN_HAS_DATA 134479872 /* BIT(27) | BIT(18) */

/* First available badge for user bits */
#define VM_FIRST_BADGE_BIT 18

/* Base definition of the Init component. This gets
 * extended in the per Vm configuration */
#define VM_INIT_DEF() \
    control; \
    uses PutChar putchar; \
    uses PutChar guest_putchar; \
    uses PCIConfig pci_config; \
    uses RTC system_rtc; \
    uses ExtraRAM ram; \
    uses VMIOPorts ioports; \
    uses VMIRQs irqs; \
    uses GuestMaps guest_mappings; \
    uses VMPCIDevices pci_devices; \
    uses InitConnection init_cons; \
    consumes HaveInterrupt intready; \
    emits HaveInterrupt intready_connector; \
    uses Timer init_timer; \
    /* File Server */ \
    uses FileServerInterface fs; \
    uses GetChar serial_getchar; \
    attribute string kernel_cmdline; \
    attribute string kernel_image; \
    attribute string kernel_relocs; \
    attribute string initrd_image; \
    attribute int iospace_domain; \
    attribute int guest_ram_mb; \
    /**/

/* VM and per VM componenents */
#define VM_PER_VM_COMPONENTS(num) \
    component Init##num vm##num; \
    component VMConfig CAT(vm##num, _config); \
    /**/


#define VM_PER_VM_CONNECTIONS(num) \
    /* Connect the intready to itself to generate a template for retrieving the AEP */ \
    connection seL4GlobalAsynch intreadycon##num(from vm##num.intready_connector, to vm##num.intready); \
    /* Connect all Init components to the fileserver */ \
    connection seL4RPCDataport fs##num(from vm##num.fs, to fserv.fs_ctrl); \
    /* Connect all the components to the serial server */ \
    connection seL4RPCCall serial_vm##num(from vm##num.putchar, to serial.vm_putchar); \
    connection seL4RPCCall serial_guest_vm##num(from vm##num.guest_putchar, to serial.guest_putchar); \
    /* Connect the emulated serial input to the serial server */ \
    connection seL4SerialServer serial_input##num(from vm##num.serial_getchar, to serial.getchar); \
    /* Temporarily connect the VM directly to the RTC */ \
    connection seL4RPCCall rtctest##num(from vm##num.system_rtc, to rtc.rtc); \
    /* Connect the VM to the timer server */ \
    connection seL4TimeServer CAT(pit##num,_timer)(from vm##num.init_timer, to time_server.the_timer); \
    /* Connect config space to main VM */ \
    connection seL4RPCCall pciconfig##num(from vm##num.pci_config, to pci_config.pci_config); \
    /* Connect the fake hardware devices */ \
    connection seL4ExtraRAM extra_ram##num(from vm##num.ram, to CAT(vm##num,_config).ram); \
    connection seL4VMIOPorts vm_ioports##num(from vm##num.ioports, to CAT(vm##num,_config).ioports); \
    connection seL4GuestMaps vm_guest_maps##num(from vm##num.guest_mappings, to CAT(vm##num,_config).guest_mappings); \
    connection seL4VMIRQs vm_irqs##num(from vm##num.irqs, to CAT(vm##num,_config).irqs); \
    connection seL4VMPCIDevices vm_pci_devices##num(from vm##num.pci_devices, to CAT(vm##num,_config).pci_devices); \
    connection seL4InitConnection vm_init_cons##num(from vm##num.init_cons, to CAT(vm##num,_config).init_cons); \
    /**/

#ifdef CONFIG_APP_CAMKES_VM_GUEST_DMA_ONE_TO_ONE
#define VM_MAYBE_ZONE_DMA(num) vm##num.mmio = "0x8000:0x97000:12";
#else
#define VM_MAYBE_ZONE_DMA(num)
#endif

#define VM_PER_VM_CONFIG_DEF(num, numplustwo) \
    vm##num.fs_attributes = VAR_STRINGIZE(num); \
    vm##num.fs_shmem_size = 0x1000; \
    vm##num.init_timer_global_endpoint = VAR_STRINGIZE(vm##num); \
    vm##num.init_timer_badge = VAR_STRINGIZE(VM_INIT_TIMER_BADGE); \
    vm##num.init_timer_attributes = numplustwo; \
    vm##num.intready_global_endpoint = VAR_STRINGIZE(vm##num); \
    vm##num.intready_connector_global_endpoint = VAR_STRINGIZE(vm##num); \
    vm##num.putchar_attributes = VAR_STRINGIZE(num); \
    vm##num.guest_putchar_attributes = VAR_STRINGIZE(num); \
    vm##num.serial_getchar_global_endpoint = VAR_STRINGIZE(vm##num); \
    vm##num.serial_getchar_badge = VAR_STRINGIZE(VM_PIC_BADGE_SERIAL_HAS_DATA); \
    vm##num.serial_getchar_attributes = VAR_STRINGIZE(num); \
    vm##num.serial_getchar_shmem_size = 0x1000; \
    vm##num.cnode_size_bits = 21; \
    vm##num.simple = true; \
    vm##num.asid_pool = true; \
    VM_MAYBE_ZONE_DMA(num) \
    /**/

#define VM_COMPOSITION_DEF() \
    component FileServer fserv; \
    /* Hardware multiplexing components */ \
    component SerialServer serial; \
    component PCIConfigIO pci_config; \
    component TimeServer time_server; \
    component RTC rtc; \
    /* Hardware components that are not actuall instantiated */ \
    component PIT pit; \
    component CMOS cmos; \
    component Serial hw_serial; \
    /* Hack to get hardware definitions sensibly in camkes for the cmoment */ \
    component PieceOfHardware poh; \
    /* These components don't do much output, but just in case they can pretend to \
     * be vm0 */ \
    connection seL4RPCCall serial_pci_config(from pci_config.putchar, to serial.vm_putchar); \
    connection seL4RPCCall serial_time_server(from time_server.putchar, to serial.vm_putchar); \
    connection seL4RPCCall serial_rtc(from rtc.putchar, to serial.vm_putchar); \
    /* Connect the hardware RTC to the RTC component */ \
    connection seL4HardwareIOPort rtc_cmos_address(from rtc.cmos_address, to cmos.cmos_address); \
    connection seL4HardwareIOPort rtc_cmos_data(from rtc.cmos_data, to cmos.cmos_data); \
    /* COnnect the serial server to the timer server */ \
    connection seL4TimeServer serialserver_timer(from serial.timeout, to time_server.the_timer); \
    /* Connect io ports to pci config space */ \
    connection seL4HardwareIOPort config_address_ports(from pci_config.config_address, to poh.pci_config_address); \
    connection seL4HardwareIOPort config_data_ports(from pci_config.config_data, to poh.pci_config_data); \
    /* Connect the hardware pit to the timer driver */ \
    connection seL4HardwareIOPort pit_command(from time_server.pit_command, to pit.command); \
    connection seL4HardwareIOPort pit_channel0(from time_server.pit_channel0, to pit.channel0); \
    connection seL4HardwareInterrupt pit_irq(from pit.irq, to time_server.irq); \
    /* Connect the hardware serial to the serial server */ \
    connection seL4HardwareIOPort serial_ioport(from serial.serial_port, to hw_serial.serial); \
    connection seL4HardwareInterrupt serial_irq(from hw_serial.serial_irq, to serial.serial_irq); \
    /**/

#define VM_PER_VM_COMP_DEF(num) \
    VM_PER_VM_COMPONENTS(num) \
    VM_PER_VM_CONNECTIONS(num) \
    /**/

#define VM_CONFIGURATION_DEF() \
    fserv.heap_size = 165536; \
    serial.timeout_attributes = 1; \
    serial.timeout_global_endpoint = "serial_server"; \
    serial.timeout_badge = "1"; \
    serial.heap_size = 4096; \
    time_server.putchar_attributes = "0"; \
    time_server.timers_per_client = 9; \
    time_server.heap_size = 8192; \
    pit.command_attributes = "0x43:0x43"; \
    pit.channel0_attributes = "0x40:0x40"; \
    pit.irq_irq_type = "isa"; \
    pit.irq_irq_ioapic = 0; \
    pit.irq_irq_ioapic_pin = 2; \
    pit.irq_irq_vector = 2; \
    pit.heap_size = 0; \
    /* Serial port definitions */ \
    hw_serial.serial_attributes="0x3f8:0x3ff"; \
    hw_serial.serial_irq_irq_type = "isa"; \
    hw_serial.serial_irq_irq_ioapic = 0; \
    hw_serial.serial_irq_irq_ioapic_pin = 4; \
    hw_serial.serial_irq_irq_vector = 4; \
    pci_config.putchar_attributes = "0"; \
    pci_config.heap_size = 0; \
    rtc.putchar_attributes = "0"; \
    rtc.heap_size = 0; \
    /* PCI config space definitions */ \
    poh.pci_config_address_attributes = "0xcf8:0xcfb"; \
    poh.pci_config_data_attributes = "0xcfc:0xcff"; \
    cmos.cmos_address_attributes = "0x70:0x70"; \
    cmos.cmos_data_attributes = "0x71:0x71"; \
    cmos.heap_size = 0; \
    /* Put the entire time server at the highest priority */ \
    time_server.priority = 255; \
    /* Put the serial interrupt at 200  \
     * but Leave the rest of the serial at default priority */ \
    serial.serial_irq_priority = 200; \
    /* Now the VMM, guest and everything else should be at \
     * the default priority of 100 */ \
    /**/

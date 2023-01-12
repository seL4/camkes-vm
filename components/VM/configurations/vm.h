/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#define _VAR_STRINGIZE(...) #__VA_ARGS__
#define VAR_STRINGIZE(...) _VAR_STRINGIZE(__VA_ARGS__)

#define _CAT(x, y) x ## y
#define CAT(x, y) _CAT(x, y)

/* For all the async sources on the intready endpoint the high bit
 * is set to indicate that an async event occured, and the low bits
 * indicate which async events */

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

/* Base definition of the Init component. This gets
 * extended in the per Vm configuration */
#define VM_INIT_DEF() \
    control; \
    uses PutChar putchar; \
    uses PutChar guest_putchar; \
    uses PCIConfig pci_config; \
    uses RTC system_rtc; \
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
    attribute int cnode_size_bits = 21; \
    attribute vswitch_mapping vswitch_layout[] = []; \
    attribute string vswitch_mac_address = ""; \
    attribute { \
        /* virtq ids */ \
        int send_id; \
        int recv_id; \
    } serial_layout[] = []; \
    attribute { \
        /* cid of the VM on the other side of this connection*/ \
        int cid; \
        /* virtq ids */ \
        int send_id; \
        int recv_id; \
    } socket_layout[] = []; \
    /* unique cid that identifies this vm's socket < 256 */ \
    attribute int guest_cid = 0; \
    /**/

/* VM and per VM componenents */
#define VM_PER_VM_COMPONENTS(num) \
    component Init##num vm##num; \
    /**/


#define VM_PER_VM_CONNECTIONS(num) \
    /* Connect the intready to itself to generate a template for retrieving the AEP */ \
    connection seL4GlobalAsynch intreadycon##num(from vm##num.intready_connector, to vm##num.intready); \
    /* Connect all Init components to the fileserver */ \
    connection seL4RPCDataport fs##num(from vm##num.fs, to fserv.fs_ctrl); \
    /* Connect all the components to the serial server */ \
    connection seL4RPCCall serial_vm##num(from vm##num.putchar, to serial.processed_putchar); \
    connection seL4RPCCall serial_guest_vm##num(from vm##num.guest_putchar, to serial.raw_putchar); \
    /* Connect the emulated serial input to the serial server */ \
    connection seL4SerialServer serial_input##num(from vm##num.serial_getchar, to serial.getchar); \
    /* Temporarily connect the VM directly to the RTC */ \
    connection seL4RPCCall rtctest##num(from vm##num.system_rtc, to rtc.rtc); \
    /* Connect the VM to the timer server */ \
    connection seL4TimeServer CAT(pit##num,_timer)(from vm##num.init_timer, to time_server.the_timer); \
    /* Connect config space to main VM */ \
    connection seL4RPCCall pciconfig##num(from vm##num.pci_config, to pci_config.pci_config); \
    /**/

#define VM_MAYBE_ZONE_DMA(num)

#define VM_PER_VM_CONFIG_DEF(num) \
    vm##num.fs_shmem_size = 0x1000; \
    vm##num.serial_getchar_attributes = VAR_STRINGIZE(num); \
    vm##num.serial_getchar_shmem_size = 0x1000; \
    vm##num.simple = true; \
    vm##num.asid_pool = true; \
    vm##num.global_endpoint_mask = 0x1fffffff & ~0x1fffe; \
    vm##num.global_endpoint_base = 1 << 27; \
    VM_MAYBE_ZONE_DMA(num) \
    /**/

#define VM_COMPOSITION_DEF() \
    component FileServer fserv; \
    /* Hardware multiplexing components */ \
    component SerialServer serial; \
    component PCIConfigIO pci_config; \
    component TimeServer time_server; \
    component RTC rtc; \
    /* These components don't do much output, but just in case they can pretend to \
     * be vm0 */ \
    connection seL4RPCCall serial_pci_config(from pci_config.putchar, to serial.processed_putchar); \
    connection seL4RPCCall serial_time_server(from time_server.putchar, to serial.processed_putchar); \
    connection seL4RPCCall serial_rtc(from rtc.putchar, to serial.processed_putchar); \
    /* COnnect the serial server to the timer server */ \
    connection seL4TimeServer serialserver_timer(from serial.timeout, to time_server.the_timer); \
    /**/

#define VM_PER_VM_COMP_DEF(num) \
    VM_PER_VM_COMPONENTS(num) \
    VM_PER_VM_CONNECTIONS(num) \
    /**/

#define VM_CONFIGURATION_DEF() \
    fserv.heap_size = 0x30000; \
    time_server.timers_per_client = 9; \
    /* Put the entire time server at the highest priority */ \
    time_server.priority = 255; \
    /* The timer server runs better if it can get the true tsc frequency from the kernel */ \
    time_server.simple_extra_bootinfo = ["SEL4_BOOTINFO_HEADER_X86_TSC_FREQ"]; \
    time_server.simple = true; \
    /* Put the serial interrupt at 200  \
     * but Leave the rest of the serial at default priority */ \
    serial.serial_irq_priority = 200; \
    /* Now the VMM, guest and everything else should be at \
     * the default priority of 100 */ \
    /**/

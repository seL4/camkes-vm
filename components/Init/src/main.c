/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

/*The init thread for the vmm system*/

#include <stdio.h>
#include <stdint.h>
#include <autoconf.h>
#include <utils/util.h>
#include <sel4utils/sel4_zf_logif.h>
#include <sel4platsupport/arch/io.h>
#include <sel4utils/vspace.h>
#include <sel4utils/stack.h>
#include <allocman/utspace/split.h>
#include <allocman/bootstrap.h>
#include <allocman/vka.h>
#include <simple/simple_helpers.h>
#include <utils/util.h>
#include <vka/capops.h>

#include <camkes.h>

#include "vmm/vmm.h"
#include "vmm/driver/pci_helper.h"
#include "vmm/platform/ioports.h"
#include "vmm/platform/boot.h"
#include "vmm/platform/boot_guest.h"
#include "vmm/platform/guest_vspace.h"
#include "vmm/vchan_component.h"

#include "vmm/vmm_manager.h"
#include "vm.h"
#include "i8259.h"
#include "timers.h"
#include "fsclient.h"
#include "vchan_init.h"

#define BRK_VIRTUAL_SIZE 400000000
#define ALLOCMAN_VIRTUAL_SIZE 400000000

reservation_t muslc_brk_reservation;
void *muslc_brk_reservation_start;
vspace_t  *muslc_this_vspace;
static sel4utils_res_t muslc_brk_reservation_memory;

seL4_CPtr intready_notification();
/* TODO: these exist for other components that have been collapsed into
 * the init componenet, but we have not yet removed their dependency
 * on having a async endpoint interface */
volatile seL4_CPtr hw_irq_handlers[16] = {0};

static seL4_CPtr get_async_event_notification() {
    return intready_notification();
}

void platsupport_serial_setup_simple(vspace_t *vspace, simple_t *simple, vka_t *vka) {
    ZF_LOGW("Ignoring call to %s\n", __FUNCTION__);
    ZF_LOGW("Assuming camkes sorted this out\n");
}

void camkes_make_simple(simple_t *simple);

static allocman_t *allocman;
static char allocator_mempool[888608];
static simple_t camkes_simple;
static vka_t vka;
static vspace_t vspace;
static sel4utils_alloc_data_t vspace_data;
static vmm_t vmm;

int cross_vm_dataports_init(vmm_t *vmm) WEAK;
int cross_vm_consumes_events_init(vmm_t *vmm, vspace_t *vspace, seL4_Word irq_badge) WEAK;
int cross_vm_consumes_event_irq_num(void) WEAK;
int cross_vm_emits_events_init(vmm_t *vmm) WEAK;

static seL4_CPtr simple_ioport_wrapper(void *data, uint16_t start_port, uint16_t end_port) {
    return ioports_get_ioport(start_port, end_port);
}

static seL4_Error simple_frame_cap_wrapper(void *data, void *paddr, int size_bits, cspacepath_t *path) {
    seL4_CPtr cap = pci_devices_get_device_mem_frame((uintptr_t)paddr);
    if (cap != 0) {
        vka_cspace_make_path(&vka, cap, path);
        return 0;
    }

    /* Check whether it is a guest mapped region. */
    cap = guest_mappings_get_mapping_mem_frame((uintptr_t)paddr);
    if (cap != 0) {
        ZF_LOGI("Guest map found at %p\n", paddr);
        vka_cspace_make_path(&vka, cap, path);
        return 0;
    }

    /* Else */
    return -1;
}

void pit_pre_init(void);
void rtc_pre_init(void);
void serial_pre_init(void);

void pre_init(void) {
    int error;

    set_putchar(putchar_putchar);

    /* Camkes adds nothing to our address space, so this array is empty */
    void *existing_frames[] = {
        NULL
    };
    camkes_make_simple(&camkes_simple);
    camkes_simple.arch_simple.IOPort_cap = simple_ioport_wrapper;
    camkes_simple.frame_cap = simple_frame_cap_wrapper;

    /* Initialize allocator */
    allocman = bootstrap_use_current_1level(
            simple_get_cnode(&camkes_simple),
            simple_get_cnode_size_bits(&camkes_simple),
            simple_last_valid_cap(&camkes_simple) + 1,
            BIT(simple_get_cnode_size_bits(&camkes_simple)),
            sizeof(allocator_mempool), allocator_mempool
    );
    ZF_LOGF_IF(allocman == NULL, "Failed to create allocman");

    error = allocman_add_simple_untypeds(allocman, &camkes_simple);
    ZF_LOGF_IF(error, "Failed to add untypeds to allocman");

    allocman_make_vka(&vka, allocman);

    /* Initialize the vspace */
    error = sel4utils_bootstrap_vspace(&vspace, &vspace_data,
            simple_get_init_cap(&camkes_simple, seL4_CapInitThreadPD), &vka, NULL, NULL, existing_frames);
    ZF_LOGF_IF(error, "Failed to bootstrap vspace");

    /* Create virtual pool */
    reservation_t pool_reservation;
    void *vaddr;
    pool_reservation.res = allocman_mspace_alloc(allocman, sizeof(sel4utils_res_t), &error);
    if (!pool_reservation.res) {
        ZF_LOGF("Failed to allocate reservation");
    }
    error = sel4utils_reserve_range_no_alloc(&vspace, pool_reservation.res,
                                             ALLOCMAN_VIRTUAL_SIZE, seL4_AllRights, 1, &vaddr);
    if (error) {
        ZF_LOGF("Failed to provide virtual memory allocator");
    }
    bootstrap_configure_virtual_pool(allocman, vaddr, ALLOCMAN_VIRTUAL_SIZE, simple_get_init_cap(&camkes_simple, seL4_CapInitThreadPD));

    /* Add additional untypeds that make up extra RAM */
    int num = ram_num_untypeds();

    for (int i = 0; i < num; i++) {
        cspacepath_t path;
        seL4_CPtr cap;
        uintptr_t paddr;
        int size_bits;
        size_t sz_size_bits;
        ram_get_untyped(i, &paddr, &size_bits, &cap);
        sz_size_bits = size_bits;
        vka_cspace_make_path(&vka, cap, &path);
        error = allocman_utspace_add_uts(allocman, 1, &path, &sz_size_bits, &paddr, ALLOCMAN_UT_DEV_MEM);
        ZF_LOGF_IF(error, "Failed to add device mem uts to allocman");
    }

    /* add untyped mmios */
    for (int i = 0; i < simple_get_untyped_count(&camkes_simple); i++) {
        size_t size;
        uintptr_t paddr;
        bool device;
        seL4_CPtr cap = simple_get_nth_untyped(&camkes_simple, i, &size, &paddr, &device);
        if (device) {
            cspacepath_t path;
            vka_cspace_make_path(&vka, cap, &path);
            error = allocman_utspace_add_uts(allocman, 1, &path, &size, &paddr, ALLOCMAN_UT_DEV);
            ZF_LOGF_IF(error, "Failed to add MMIO uts allocman");
        }
    }

    sel4utils_reserve_range_no_alloc(&vspace, &muslc_brk_reservation_memory, BRK_VIRTUAL_SIZE, seL4_AllRights, 1, &muslc_brk_reservation_start);
    muslc_this_vspace = &vspace;
    muslc_brk_reservation = (reservation_t){.res = &muslc_brk_reservation_memory};

}

typedef struct memory_range {
    uintptr_t base;
    size_t size;
} memory_range_t;

#if  defined(CONFIG_APP_CAMKES_VM_GUEST_DMA_ONE_TO_ONE) || defined(CONFIG_APP_CAMKES_VM_GUEST_DMA_ONE_TO_ONE_UNSAFE)
static memory_range_t guest_ram_regions[] = {
    /* Define one of the standard ram regions
     * Making sure it is page aligned so it will
     * work from device memory */
#ifdef CONFIG_APP_CAMKES_VM_GUEST_DMA_ONE_TO_ONE
    {0x8000, 0x9f000 - 0x8000},
#else
    /* Make the range as small as possible since
     * we will not be allocating this one to one
     * and we want to pray that linux cannot end
     * up using it for DMA */
    {0x8000, 0x81000 - 0x8000},
#endif
};
#else
static memory_range_t guest_ram_regions[] = {
    /* Allocate all the standard low memory areas */
    {0x500, 0x7c00 - 0x500},
    {0x7e00, 0x80000 - 0x7e00},
    {0x80000, 0x9fc00 - 0x80000},
};
#endif

static memory_range_t guest_fake_devices[] = {
    {0xf0000, 0x10000}, // DMI
    {0xe0000, 0x10000}, // PCI BIOS
    {0xc0000, 0xc8000 - 0xc0000}, // VIDEO BIOS
    {0xc8000, 0xe0000 - 0xc8000}, // Mapped hardware and MISC
#if defined(CONFIG_APP_CAMKES_VM_GUEST_DMA_ONE_TO_ONE) || defined(CONFIG_APP_CAMKES_VM_GUEST_DMA_ONE_TO_ONE_UNSAFE)
    /* Fake a BIOS page. Done in one_to_one case as we don't
     * allocate a low memory frame already */
    {0x0, 0x1000},
    /* Same for EBDA region */
    {0x9f000, 0x1000},
#endif
};

typedef struct device_notify {
    uint32_t badge;
    /* the function (as described by the user in the configuration) to call
     * when the message has been received. */
    void (*func)(vmm_t *vmm);
} device_notify_t;

/* Wrappers for passing PCI config space calls to camkes */
static uint8_t camkes_pci_read8(void *cookie, vmm_pci_address_t addr, unsigned int offset) {
    return pci_config_read8(addr.bus, addr.dev, addr.fun, offset);
}
static uint16_t camkes_pci_read16(void *cookie, vmm_pci_address_t addr, unsigned int offset) {
    return pci_config_read16(addr.bus, addr.dev, addr.fun, offset);
}
static uint32_t camkes_pci_read32(void *cookie, vmm_pci_address_t addr, unsigned int offset) {
    return pci_config_read32(addr.bus, addr.dev, addr.fun, offset);
}
static void camkes_pci_write8(void *cookie, vmm_pci_address_t addr, unsigned int offset, uint8_t val) {
    pci_config_write8(addr.bus, addr.dev, addr.fun, offset, val);
}
static void camkes_pci_write16(void *cookie, vmm_pci_address_t addr, unsigned int offset, uint16_t val) {
    pci_config_write16(addr.bus, addr.dev, addr.fun, offset, val);
}
static void camkes_pci_write32(void *cookie, vmm_pci_address_t addr, unsigned int offset, uint32_t val) {
    pci_config_write32(addr.bus, addr.dev, addr.fun, offset, val);
}

vmm_pci_config_t make_camkes_pci_config() {
    return (vmm_pci_config_t) {
        .cookie = NULL,
        .ioread8 = camkes_pci_read8,
        .ioread16 = camkes_pci_read16,
        .ioread32 = camkes_pci_read32,
        .iowrite8 = camkes_pci_write8,
        .iowrite16 = camkes_pci_write16,
        .iowrite32 = camkes_pci_write32
    };
}

typedef struct ioport_desc {
    uint16_t start_port;
    uint16_t end_port;
    ioport_in_fn port_in;
    ioport_out_fn port_out;
    const char *desc;
} ioport_desc_t;

int i8254_port_in(void *cookie, unsigned int port_no, unsigned int size, unsigned int *result);
int i8254_port_out(void *cookie, unsigned int port_no, unsigned int size, unsigned int value);

int cmos_port_in(void *cookie, unsigned int port_no, unsigned int size, unsigned int *result);
int cmos_port_out(void *cookie, unsigned int port_no, unsigned int size, unsigned int value);

int serial_port_in(void *cookie, unsigned int port_no, unsigned int size, unsigned int *result);
int serial_port_out(void *cookie, unsigned int port_no, unsigned int size, unsigned int value);

ioport_desc_t ioport_handlers[] = {
    {X86_IO_SERIAL_1_START,   X86_IO_SERIAL_1_END,   serial_port_in, serial_port_out, "COM1 Serial Port"},
//    {X86_IO_SERIAL_3_START,   X86_IO_SERIAL_3_END,   NULL, NULL, "COM3 Serial Port"},
    {X86_IO_PIC_1_START,      X86_IO_PIC_1_END,      i8259_port_in, i8259_port_out, "8259 Programmable Interrupt Controller (1st, Master)"},
    {X86_IO_PIC_2_START,      X86_IO_PIC_2_END,      i8259_port_in, i8259_port_out, "8259 Programmable Interrupt Controller (2nd, Slave)"},
    {X86_IO_ELCR_START,       X86_IO_ELCR_END,       i8259_port_in, i8259_port_out, "ELCR (edge/level control register) for IRQ line"},
    /* PCI config requires a cookie and is specced dynamically in code */
//    {X86_IO_PCI_CONFIG_START, X86_IO_PCI_CONFIG_END, vmm_pci_io_port_in, vmm_pci_io_port_out, "PCI Configuration"},
    {X86_IO_RTC_START,        X86_IO_RTC_END,        cmos_port_in, cmos_port_out, "CMOS Registers / RTC Real-Time Clock / NMI Interrupts"},
    {X86_IO_PIT_START,        X86_IO_PIT_END,        i8254_port_in, i8254_port_out, "8253/8254 Programmable Interval Timer"},
//    {X86_IO_PS2C_START,       X86_IO_PS2C_END,       NULL, NULL, "8042 PS/2 Controller"},
//    {X86_IO_POS_START,        X86_IO_POS_END,        NULL, NULL, "POS Programmable Option Select (PS/2)"},

#if 0
    {0xC000,                  0xF000,                NULL, NULL, "PCI Bus IOPort Mapping Space"},
    {0x1060,                  0x1070,                NULL, NULL, "IDE controller"},
    {0x01F0,                  0x01F8,                NULL, NULL, "Primary IDE controller"},
    {0x0170,                  0x0178,                NULL, NULL, "Secondary IDE controller"},
    {0x3f6,                   0x03f7,                NULL, NULL, "Additional ATA register"},
    {0x376,                   0x0377,                NULL, NULL, "Additional ATA register"},
    {0x3b0,                   0x3df,                 NULL, NULL, "IBM VGA"},

    {0x80,                    0x80,                  NULL, NULL, "DMA IOPort timer"},
#endif

#if 0
    {0x164e,                  0x164f,                NULL, NULL, "Serial Configuration Registers"},
#endif
};

int pci_config_io_in(void *cookie, uint32_t port, int io_size, uint32_t *result) {
    uint32_t *conf_port_addr = (uint32_t*)cookie;
    uint8_t offset;
    if (port >= PCI_CONF_PORT_ADDR && port < PCI_CONF_PORT_ADDR_END) {
        offset = port - PCI_CONF_PORT_ADDR;
        /* Emulate read addr */
        *result = 0;
        memcpy(result, ((char*)conf_port_addr) + offset, io_size);
        return 0;
    }
    if (port < PCI_CONF_PORT_DATA || port + io_size > PCI_CONF_PORT_DATA_END) {
        return -1;
    }
    offset = port - PCI_CONF_PORT_DATA;
    /* Make an address out of the config */
    uint8_t bus, dev, fun, reg;
    bus = ((*conf_port_addr) >> 16) & MASK(8);
    dev = ((*conf_port_addr) >> 11) & MASK(5);
    fun = ((*conf_port_addr) >> 8) & MASK(3);
    reg = (*conf_port_addr) & MASK(8);
    reg += offset;
    /* Read the real config */
    switch(io_size) {
    case 1:
        *result=pci_config_read8(bus, dev, fun, reg);
        break;
    case 2:
        *result=pci_config_read16(bus, dev, fun, reg);
        break;
    case 4:
        *result=pci_config_read32(bus, dev, fun, reg);
        break;
    default:
        ZF_LOGF("Invalid size");
        return -1;
    }
    return 0;
}

int pci_config_io_out(void *cookie, uint32_t port, int io_size, uint32_t val) {
    uint32_t *conf_port_addr = (uint32_t*)cookie;
    uint8_t offset;
    if (port >= PCI_CONF_PORT_ADDR && port < PCI_CONF_PORT_ADDR_END) {
        offset = port - PCI_CONF_PORT_ADDR;
        /* Emulate read addr */
        val &= ~MASK(2);
        memcpy(((char*)conf_port_addr) + offset, &val, io_size);
        return 0;
    }
    if (port < PCI_CONF_PORT_DATA || port + io_size > PCI_CONF_PORT_DATA_END) {
        return -1;
    }
    offset = port - PCI_CONF_PORT_DATA;
    /* Make an address out of the config */
    uint8_t bus, dev, fun, reg;
    bus = ((*conf_port_addr) >> 16) & MASK(8);
    dev = ((*conf_port_addr) >> 11) & MASK(5);
    fun = ((*conf_port_addr) >> 8) & MASK(3);
    reg = (*conf_port_addr) & MASK(8);
    reg += offset;
    /* Read the real config */
    switch(io_size) {
    case 1:
        pci_config_write8(bus, dev, fun, reg, val);
        break;
    case 2:
        pci_config_write16(bus, dev, fun, reg, val);
        break;
    case 4:
        pci_config_write32(bus, dev, fun, reg, val);
        break;
    default:
        ZF_LOGF("Invalid size");
        return -1;
    }
    return 0;
}

ps_io_port_ops_t make_pci_io_ops() {
    return (ps_io_port_ops_t) { .cookie = malloc(sizeof(uint32_t)),
                                 .io_port_in_fn = pci_config_io_in,
                                 .io_port_out_fn = pci_config_io_out
                               };
}

static int device_notify_list_len = 0;
static device_notify_t *device_notify_list = NULL;

void pit_timer_interrupt(void);
void rtc_timer_interrupt(uint32_t);
void serial_timer_interrupt(uint32_t);

static seL4_Word irq_badges[16] = {
    VM_PIC_BADGE_IRQ_0,
    VM_PIC_BADGE_IRQ_1,
    VM_PIC_BADGE_IRQ_2,
    VM_PIC_BADGE_IRQ_3,
    VM_PIC_BADGE_IRQ_4,
    VM_PIC_BADGE_IRQ_5,
    VM_PIC_BADGE_IRQ_6,
    VM_PIC_BADGE_IRQ_7,
    VM_PIC_BADGE_IRQ_8,
    VM_PIC_BADGE_IRQ_9,
    VM_PIC_BADGE_IRQ_10,
    VM_PIC_BADGE_IRQ_11,
    VM_PIC_BADGE_IRQ_12,
    VM_PIC_BADGE_IRQ_13,
    VM_PIC_BADGE_IRQ_14,
    VM_PIC_BADGE_IRQ_15
};

void serial_character_interrupt(void);

static int handle_async_event(seL4_Word badge) {
    if (badge & BIT(27)) {
        if ( (badge & VM_INIT_TIMER_BADGE) == VM_INIT_TIMER_BADGE) {
            uint32_t completed = init_timer_completed();
            if (completed & BIT(TIMER_PIT)) {
                pit_timer_interrupt();
            }
            if (completed & (BIT(TIMER_PERIODIC_TIMER) | BIT(TIMER_COALESCED_TIMER) | BIT(TIMER_SECOND_TIMER) | BIT(TIMER_SECOND_TIMER2))) {
                rtc_timer_interrupt(completed);
            }
            if (completed & (BIT(TIMER_FIFO_TIMEOUT) | BIT(TIMER_TRANSMIT_TIMER) | BIT(TIMER_MODEM_STATUS_TIMER) | BIT(TIMER_MORE_CHARS))) {
                serial_timer_interrupt(completed);
            }
        }
        if ( (badge & VM_PIC_BADGE_SERIAL_HAS_DATA) == VM_PIC_BADGE_SERIAL_HAS_DATA) {
            serial_character_interrupt();
        }
        for (int i = 0; i < 16; i++) {
            if ( (badge & irq_badges[i]) == irq_badges[i]) {
                i8259_gen_irq(i);
            }
        }
        for (int i = 0; i < device_notify_list_len; i++) {
            uint32_t device_badge = device_notify_list[i].badge;
            if ( (badge & device_badge) == device_badge) {
                ZF_LOGF_IF(device_notify_list[i].func == NULL, "Undefined notify func");
                device_notify_list[i].func(&vmm);
            }
        }
    }
    /* return 0 to indicate an interrupt occured */
    return i8259_has_interrupt() ? 0 : 1;
}

static void init_irqs() {
    int error UNUSED;

    int num_irqs = irqs_num_irqs();

    if (cross_vm_consumes_event_irq_num && num_irqs > cross_vm_consumes_event_irq_num()) {
        ZF_LOGE("Cross vm event irq number not available");
    }

    for (int i = 0; i < num_irqs; i++) {
        seL4_CPtr irq_handler;
        uint8_t source;
        int level_trig;
        int active_low;
        uint8_t dest;
        cspacepath_t badge_path;
        cspacepath_t async_path;
        irqs_get_irq(i, &irq_handler, &source, &level_trig, &active_low, &dest);
        vka_cspace_make_path(&vka, intready_notification(), &async_path);
        error = vka_cspace_alloc_path(&vka, &badge_path);
        ZF_LOGF_IF(error, "Failed to alloc cspace path");

        error = vka_cnode_mint(&badge_path, &async_path, seL4_AllRights, seL4_CapData_Badge_new(irq_badges[dest]));
        ZF_LOGF_IF(error, "Failed to mint cnode");
        error = seL4_IRQHandler_SetNotification(irq_handler, badge_path.capPtr);
        ZF_LOGF_IF(error, "Failed to set notification for irq handler");
        error = seL4_IRQHandler_Ack(irq_handler);
        ZF_LOGF_IF(error, "Failed to ack irq handler");
        hw_irq_handlers[dest] = irq_handler;
    }
}

int fake_vchan_handler(vmm_vcpu_t *vcpu) {
    return 0;
}

void init_con_irq_init(void) {
    int i;
    int irqs = 0;
    uintptr_t badge;
    uintptr_t fun;
    for (i = 0; i < init_cons_num_connections(); i++) {
        if (init_cons_has_interrupt(i, &badge, &fun)) {
            irqs++;
        }
    }
    device_notify_list_len = irqs;
    device_notify_list = malloc(sizeof(*device_notify_list) * irqs);
    ZF_LOGF_IF(device_notify_list == NULL, "Malloc failed");
    for (i = 0; i < irqs; i++) {
        init_cons_has_interrupt(i, &badge, &fun);
        device_notify_list[i].badge = badge;
        device_notify_list[i].func = (void (*)(vmm_t*))fun;
    }
}

void *main_continued(void *arg) {
    int error;
    int i;
    int have_initrd = 0;
    ps_io_port_ops_t ioops;

    rtc_time_date_t time_date = system_rtc_time_date();
    ZF_LOGI("Starting VM %s at: %04d:%02d:%02d %02d:%02d:%02d\n", get_instance_name(), time_date.year, time_date.month, time_date.day, time_date.hour, time_date.minute, time_date.second);

    ioops = make_pci_io_ops();

    ZF_LOGI("PCI scan");
    libpci_scan(ioops);

    /* install custom open/close/read implementations to redirect I/O from the VMM to
     * our file server */
    install_fileserver();

    /* Construct a new VM */
    platform_callbacks_t callbacks = (platform_callbacks_t) {
        .get_interrupt = i8259_get_interrupt,
        .has_interrupt = i8259_has_interrupt,
        .do_async = handle_async_event,
        .get_async_event_notification = get_async_event_notification,
    };

    ZF_LOGI("VMM init");
    error = vmm_init(&vmm, allocman, camkes_simple, vka, vspace, callbacks);
    ZF_LOGF_IF(error, "VMM init failed");

    /* First we initialize any host information */
    ZF_LOGI("VMM init host");
    error = vmm_init_host(&vmm);
    ZF_LOGF_IF(error, "VMM init host failed");

    ZF_LOGI("Init guest");
    /* Early init of guest. We populate everything later */
    error = vmm_init_guest(&vmm, CONFIG_CAMKES_DEFAULT_PRIORITY);
    ZF_LOGF_IF(error, "Guest init failed");

    /* Initialize the init device badges and notification functions */
    ZF_LOGI("Init device badges and notification functions");
    init_con_irq_init();

    have_initrd = !(strcmp(initrd_image, "") == 0);

    ZF_LOGI("Init irqs");
    init_irqs();

    ZF_LOGI("i8259 pre init");
    i8259_pre_init();

    ZF_LOGI("serial pre init");
    serial_pre_init();

    ZF_LOGI("Pit pre init");
    pit_pre_init();

    ZF_LOGI("RTC pre init");
    rtc_pre_init();

#ifdef CONFIG_APP_CAMKES_VM_GUEST_DMA_IOMMU
    /* Do early device discovery and find any relevant PCI busses that
     * need to get added */
    ZF_LOGI("PCI early device discovery");
    for (i = 0; i < pci_devices_num_devices(); i++) {
        uint8_t bus;
        uint8_t dev;
        uint8_t fun;
        seL4_CPtr iospace_cap;
        pci_devices_get_device(i, &bus, &dev, &fun, &iospace_cap);
        error = vmm_guest_vspace_add_iospace(&vmm.host_vspace, &vmm.guest_mem.vspace, iospace_cap);
        ZF_LOGF_IF(error, "failed to add iospace to vspace");
    }
#endif

    /* Do we need to do any early reservations of guest address space? */
#ifdef CONFIG_APP_CAMKES_VM_GUEST_DMA_ONE_TO_ONE
    ZF_LOGI("Setting up early guest reservations");
    for (i = 0; i < ARRAY_SIZE(guest_ram_regions); i++) {
        /* try and put a device here */
        error = vmm_map_guest_device_at(&vmm, guest_ram_regions[i].base, guest_ram_regions[i].base, guest_ram_regions[i].size);
    }
    /* We now run the normal loop to allocate ram regions. Because the addresses are
     * already in the vspace no additional frames will get mapped, but it will result
     * in ram regions being defined for the guest */
#endif
    for (i = 0; i < ARRAY_SIZE(guest_ram_regions); i++) {
        error = vmm_alloc_guest_ram_at(&vmm, guest_ram_regions[i].base, guest_ram_regions[i].size);
        ZF_LOGF_IF(error, "Failed to alloc guest ram at %p", (void*)guest_ram_regions[i].base);
    }

    for (i = 0; i < ARRAY_SIZE(guest_fake_devices); i++) {
        error = vmm_alloc_guest_device_at(&vmm, guest_fake_devices[i].base, guest_fake_devices[i].size);
        ZF_LOGF_IF(error, "Failed to alloc guest device at %p", (void*)guest_fake_devices[i].base);
    }

    /* Add in the device mappings specified by the guest. */
    for (i = 0; i < guest_mappings_num_guestmaps(); i++) {
        uint64_t frame_paddr;
        uint64_t size;
        error = guest_mappings_get_guest_map(i, &frame_paddr, &size);
        ZF_LOGF_IF(error, "Failed to get guest map at %d\n", i);
    }

    /* Allocate guest ram. This is the main memory that the guest will actually get
     * told exists. Other memory may get allocated and mapped into the guest */
    int paddr_is_vaddr;
#if defined(CONFIG_APP_CAMKES_VM_GUEST_DMA_ONE_TO_ONE) || defined(CONFIG_APP_CAMKES_VM_GUEST_DMA_ONE_TO_ONE_UNSAFE)
    paddr_is_vaddr = 1;
#else
    paddr_is_vaddr = 0;
#endif
    error = vmm_alloc_guest_ram(&vmm, guest_ram_mb * 1024 * 1024, paddr_is_vaddr);
    ZF_LOGF_IF(error, "Failed to allocate guest ram");

    /* Perform device discovery and give passthrough device information */
    ZF_LOGI("PCI device discovery");
    for (i = 0; i < pci_devices_num_devices(); i++) {
        uint8_t bus;
        uint8_t dev;
        uint8_t fun;
        const char *irq_name;
        int irq = -1;
        seL4_CPtr iospace_cap;
        pci_devices_get_device(i, &bus, &dev, &fun, &iospace_cap);
        irq_name = pci_devices_get_device_irq(i);
        /* search for the irq */
        for (int j = 0; j < irqs_num_irqs(); j++) {
            seL4_CPtr cap;
            uint8_t source;
            int level_trig;
            int active_low;
            uint8_t dest;
            const char *this_name;
            this_name = irqs_get_irq(j, &cap, &source, &level_trig, &active_low, &dest);
            if (strcmp(irq_name, this_name) == 0) {
                irq = dest;
                break;
            }
        }
        assert(irq != -1);
        libpci_device_t *device = libpci_find_device_bdf(bus, dev, fun);
        if (!device) {
            LOG_ERROR("Failed to find device %02x:%02x.%d\n", bus, dev, fun);
            return NULL;
        }
        /* Allocate resources */
        vmm_pci_bar_t bars[6];
        int num_bars = vmm_pci_helper_map_bars(&vmm, &device->cfg, bars);
        assert(num_bars >= 0);
        vmm_pci_entry_t entry = vmm_pci_create_passthrough((vmm_pci_address_t){bus, dev, fun}, make_camkes_pci_config());
        if (num_bars > 0) {
            entry = vmm_pci_create_bar_emulation(entry, num_bars, bars);
        }
        entry = vmm_pci_create_irq_emulation(entry, irq);
        entry = vmm_pci_no_msi_cap_emulation(entry);
        error = vmm_pci_add_entry(&vmm.pci, entry, NULL);
        assert(!error);
    }

    /* Initialize any extra init devices */
    ZF_LOGI("Init extra devices");
    for (i = 0; i < init_cons_num_connections(); i++) {
        void (*proc)(vmm_t*) = (void (*)(vmm_t*))init_cons_init_function(i);
        proc(&vmm);
    }

    /* Add any IO ports */
    ZF_LOGI("Adding IO ports");
    for (i = 0; i < ARRAY_SIZE(ioport_handlers); i++) {
        if (ioport_handlers[i].port_in) {
            error = vmm_io_port_add_handler(&vmm.io_port, ioport_handlers[i].start_port, ioport_handlers[i].end_port, NULL, ioport_handlers[i].port_in, ioport_handlers[i].port_out, ioport_handlers[i].desc);
            assert(!error);
        } else {
            error = vmm_io_port_add_passthrough(&vmm.io_port, ioport_handlers[i].start_port, ioport_handlers[i].end_port, ioport_handlers[i].desc);
            assert(!error);
        }
    }
    for (i = 0; i < ioports_num_nonpci_ioports(); i++) {
        uint16_t start;
        uint16_t end;
        const char *desc;
        seL4_CPtr cap;
        desc = ioports_get_nonpci_ioport(i, &cap, &start, &end);
        error = vmm_io_port_add_passthrough(&vmm.io_port, start, end, desc);
        assert(!error);
    }
    /* config start and end encomposes both addr and data ports */
    error = vmm_io_port_add_handler(&vmm.io_port, X86_IO_PCI_CONFIG_START, X86_IO_PCI_CONFIG_END, &vmm.pci, vmm_pci_io_port_in, vmm_pci_io_port_out, "PCI Configuration Space");
    assert(!error);

    /* Load in an elf file. Hard code alignment to 4M */
    /* TODO: use proper libc file handles with the CPIO file system */
    ZF_LOGI("Elf loading");
    error = vmm_load_guest_elf(&vmm, kernel_image, BIT(PAGE_BITS_4M));
    ZF_LOGF_IF(error, "Failed to load guest elf file");

    /* Relocate the elf */
    ZF_LOGI("Relocate elf");
    vmm_plat_guest_elf_relocate(&vmm, kernel_relocs);

    /* Add a boot module */
    if (have_initrd) {
        error = vmm_guest_load_boot_module(&vmm, initrd_image);
        ZF_LOGF_IF(error, "Failed to load boot module");
    }

    vmm_plat_init_guest_boot_structure(&vmm, kernel_cmdline);

    error = reg_new_handler(&vmm, &vchan_handler, VMM_MANAGER_TOKEN);
    ZF_LOGF_IF(error, "Failed register vchan_handler");

    if (cross_vm_dataports_init) {
        error = cross_vm_dataports_init(&vmm);
        ZF_LOGF("cross vm dataports init failed");
    }

    if (cross_vm_emits_events_init) {
        error = cross_vm_emits_events_init(&vmm);
        assert(!error);
    }

    if (cross_vm_consumes_events_init && cross_vm_consumes_event_irq_num) {
        error = cross_vm_consumes_events_init(&vmm, &vspace,
            irq_badges[cross_vm_consumes_event_irq_num()]);

        assert(!error);
    }

    /* Final VMM setup now that everything is defined and loaded */
    ZF_LOGI("Finalising VMM");
    error = vmm_finalize(&vmm);
    ZF_LOGF_IF(error, "Failed to finalise VMM");

//    vmm_exit_init();
    /* Now go run the event loop */
    vmm_run(&vmm);

    return NULL;
}

int run(void) {
    sel4utils_run_on_stack(&vspace, main_continued, NULL, NULL);
    assert(!"Should not get here");

    return 0;
}

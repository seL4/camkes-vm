/*
 * Copyright 2023, Hensoldt Cyber
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <vmlinux.h>
#include <camkes.h>
#include <sel4vm/guest_vcpu_fault.h>

#include <libfdt.h>

extern char gen_dtb_buf[];
extern void *fdt_ori;


#define PL011_DR            0x00 // data register

// rsrecr;     // 0x04: receive status/error clear register
// (unused)    // 0x08 - 0x14

#define PL011_FR            0x18 // flag register
#define PL011_FR_CTS        BIT(0)
#define PL011_FR_DSR        BIT(1)
#define PL011_FR_DCD        BIT(2)
#define PL011_FR_BUSY       BIT(3)
#define PL011_FR_RXFE       BIT(4) // RX FIFO empty
#define PL011_FR_TXFF       BIT(5) // TX FIFO full
#define PL011_FR_RXFF       BIT(5) // RX FIFO full
#define PL011_FR_TXFE       BIT(7) // TX FIFO empty
#define PL011_FR_RI         BIT(8)

// (unused1)   // 0x1c
// ilpr;       // 0x20: not in use
// ibrd;       // 0x24: integer baud rate divisor
// fbrd;       // 0x28: fractional baud rate divisor

#define PL011_LCR           0x2c // line control register
#define PL011_LCR_FEN       BIT(4) // Enable FIFO
// 0x60/0x70 -> x11x xxxx -> WLEN = 8 bit

#define PL011_CR            0x30 //  control register
#define PL011_CR_EN         BIT(0)
#define PL011_CR_TXE        BIT(8)
#define PL011_CR_RXE        BIT(9)
#define PL011_CR_DTR        BIT(10)
#define PL011_CR_RTS        BIT(11)

// ifls;       // 0x34: interrupt FIFO level select register

#define PL011_IMSCR         0x38 // interrupt mask set clear register
#define PL011_IMSCR_RXIM    BIT(4) // RX interrupt
#define PL011_IMSCR_TXIM    BIT(5) // TX interrupt
#define PL011_IMSCR_RTIM    BIT(6) // RX timeout interrupt

// ris;        // 0x3c: raw interrupt status register
// mis;        // 0x40: masked interrupt status register
// icr;        // 0x44: interrupt clear register
// dmacr;      // 0x48: DMA control register


#define PL011_ID_R1P5  { 0x11, 0x10, 0x34, 0x00, 0x0D, 0xF0, 0x05, 0xB1 }


typedef struct {
    uint16_t lcr;
    uint16_t cr;
    uint16_t im;
} vpl011_regs_t;

#define VPL011_REG_INIT    { \
        .lcr = 0,  \
        .cr  = PL011_CR_TXE | PL011_CR_RXE,  \
        .im  = 0,  \
    }


typedef struct {
    uintptr_t     base;
    size_t        size;
    unsigned int  intr;
    const char    *name;
    const uint8_t id[8];
    vpl011_regs_t *regs;
} vpl011_t;



static vpl011_regs_t vpl011_regs[] = {
    VPL011_REG_INIT,
    VPL011_REG_INIT,
};

static const vpl011_t all_vpl011[] = {
    {
        .base = 0xf9000000,
        .size = PAGE_SIZE,
        .intr = 1, /* ToDO: clarify what can be used, Linux says 14 here */
        .id = PL011_ID_R1P5,
        .name = "vpl011_0",
        .regs = &vpl011_regs[0],
    },
    {
        .base = 0xf9001000,
        .size = PAGE_SIZE,
        .intr = 2, /* ToDO: clarify what can be used, Linux says 15 here */
        .id = PL011_ID_R1P5,
        .name = "vpl011_1",
        .regs = &vpl011_regs[1],
    },
};


//------------------------------------------------------------------------------
static bool is_addr_in_range(uintptr_t addr, uintptr_t start, size_t size)
{
    return ((addr >= start) || ((addr - start) < size));
}


//------------------------------------------------------------------------------
static bool is_addr_in_vpl011m(vpl011_t const *vpl011, uintptr_t addr)
{
    return is_addr_in_range(addr, vpl011->base, vpl011->size);
}

//------------------------------------------------------------------------------
static memory_fault_result_t reg_read(vm_vcpu_t *vcpu, vpl011_t *vpl011,
                                      size_t offset)
{
    // vm_t *vm = vcpu->vm;
    seL4_Word mask = get_vcpu_fault_data_mask(vcpu);

    // if (4 != fault_length) {
    //     ZF_LOGI("invalid %zu-bit accesses at offset 0x%zx", fault_length*8, offset);
    //     if (is_read) {
    //         /* Out of range, treat as SBZ */
    //         set_vcpu_fault_data(vcpu, 0);
    //     }
    //     return FAULT_IGNORE;
    // }

    seL4_Word data = 0; // default to dummy

    switch (offset) {
        case PL011_FR: // flag register
            data = PL011_FR_TXFE | PL011_FR_RXFE;
            break;

        case PL011_LCR: // line control register
            data = vpl011->regs->lcr;
            break;

        case PL011_CR: // control register
            data = vpl011->regs->cr;
            break;

        case PL011_IMSCR: // interrupt mask set clear register
            data = vpl011->regs->im;
            break;

        default:
            if (offset >= 0xfe0) {
                data = vpl011->id[(offset >> 2) & 0x7];
            } else {
                ZF_LOGI("%s: unsupported read at [%p]", vpl011->name, (void *)offset);
            }
            break;
    }

    set_vcpu_fault_data(vcpu, data & mask);
    advance_vcpu_fault(vcpu);
    return FAULT_HANDLED;
}


//------------------------------------------------------------------------------
static memory_fault_result_t reg_write(vm_vcpu_t *vcpu, vpl011_t *vpl011,
                                       size_t offset)
{
    // vm_t *vm = vcpu->vm;
    seL4_Word val_raw = get_vcpu_fault_data(vcpu);
    seL4_Word mask = get_vcpu_fault_data_mask(vcpu);
    seL4_Word val = val_raw & mask;

    switch (offset) {
        case PL011_DR: // data register
            /* ToDo: buffer line and pritn on '\n' */
            if (val != '\r') { printf("%c", val); }
            break;

        case PL011_LCR: // line control register
            vpl011->regs->lcr = val;
            break;

        case PL011_CR: // control register
            //{
            //    seL4_Word en = val & PL011_CR_EN;
            //    if ((en ^ vpl011->regs->cr) & PL011_CR_EN) {
            //        ZF_LOGI("%s: %s", vpl011->name, en?"ON":"OFF");
            //    }
            //}
            vpl011->regs->cr = val;
            break;

        case PL011_IMSCR: // interrupt mask set clear register
            //ZF_LOGI("%s: IMSCR <- 0x%x", vpl011->name, val);
            vpl011->regs->im = val;
            break;

        case 0x24: // integer baud rate divisor
        case 0x28: // fractional baud rate divisor
        case 0x34: // interrupt FIFO level select register
        case 0x44: // interrupt clear register
        case 0x48: // DMA control register
            // ignore write
            break;

        default:
            ZF_LOGI("%s: unsupported write 0x%x -> [%p]",
                    vpl011->name, val, (void *)offset);
            break;
    }

    advance_vcpu_fault(vcpu);
    return FAULT_HANDLED;
}


//------------------------------------------------------------------------------
static memory_fault_result_t vpl011_handle_fault(vm_t *vm,
                                                 vm_vcpu_t *vcpu,
                                                 uintptr_t fault_addr,
                                                 size_t fault_length,
                                                 void *cookie)
{
    assert(vm == vcpu->vm);
    vpl011_t *vpl011 = (vpl011_t *)cookie;
    bool is_read = is_vcpu_read_fault(vcpu);

    if (!is_addr_in_vpl011m(vpl011, fault_addr)) {
        ZF_LOGI("%s: invalid addr %p", vpl011->name, (void *)fault_addr);
        if (is_read) {
            /* Out of range, treat as SBZ */
            set_vcpu_fault_data(vcpu, 0);
        }
        return FAULT_IGNORE;
    }

    size_t offset = fault_addr - vpl011->base;

    // ZF_LOGI("%s: %s %p", vpl011->name, is_read?"read":"write", (void *)offset);

    return is_read ? reg_read(vcpu, vpl011, offset)
                   : reg_write(vcpu, vpl011, offset);
}


//------------------------------------------------------------------------------
static int vpl011_install(vm_t *vm, const vpl011_t *vpl011)
{
    int err;

    void *cookie = (void *)vpl011; /* cast away the const */
    vm_memory_reservation_t *r = vm_reserve_memory_at(vm,
                                                      vpl011->base,
                                                      vpl011->size,
                                                      vpl011_handle_fault,
                                                      cookie);
    if (!r) {
        ZF_LOGE("failed to reserve memory for vpl011@%p", (void *)vpl011->base);
        return -1;
    }

    // ToDO: setup generic AC-Device...
    //   int mask_size = UART_SIZE;
    //   uint32_t *mask = (uint32_t *)calloc(1, mask_size);
    //   err = vm_install_generic_ac_device(vm, d, mask, mask_size, VACDEV_MASK_ONLY);

    if (!vm_config.generate_dtb) {
        return 0;
    }

    // Create the device node for our UART
    ZF_LOGF_IF(!fdt_ori, "fdt_ori not set");
    void *fdt = gen_dtb_buf;

    char name_buf[32] = {0};
    sprintf(name_buf, "pl011@%" PRIxPTR, vpl011->base);
    ZF_LOGI("add DTB node '%s' for pl011 at %p", name_buf, (void *)vpl011->base);
    int root_offset = fdt_path_offset(fdt, "/");
    int node = fdt_add_subnode(fdt, root_offset, name_buf);
    if (node < 0) {
        ZF_LOGE("failed to create node");
        return node;
    }

    // https://www.kernel.org/doc/Documentation/devicetree/bindings/serial/pl011.txt
    //
    // ARM AMBA Primecell PL011 serial UART
    //
    // Required properties:
    // - compatible: must be "arm,primecell", "arm,pl011", "zte,zx296702-uart"
    // - reg: exactly one register range with length 0x1000
    // - interrupts: exactly one interrupt specifier
    //
    // Optional properties:
    // - pinctrl:
    //     When present, must have one state named "default",
    //     and may contain a second name named "sleep". The former
    //     state sets up pins for ordinary operation whereas
    //     the latter state will put the associated pins to sleep
    //     when the UART is unused
    // - clocks:
    //     When present, the first clock listed must correspond to
    //     the clock named UARTCLK on the IP block, i.e. the clock
    //     to the external serial line, whereas the second clock
    //     must correspond to the PCLK clocking the internal logic
    //     of the block. Just listing one clock (the first one) is
    //     deprecated.
    // - clock-names:
    //     When present, the first clock listed must be named
    //     "uartclk" and the second clock listed must be named
    //     "apb_pclk"
    // - dmas:
    //     When present, may have one or two dma channels.
    //     The first one must be named "rx", the second one
    //     must be named "tx".
    // - auto-poll:
    //     Enables polling when using RX DMA.
    // - poll-rate-ms:
    //     Rate at which poll occurs when auto-poll is set,
    //     default 100ms.
    // - poll-timeout-ms:
    //     Poll timeout when auto-poll is set, default
    //     3000ms.
    //
    // See also bindings/arm/primecell.txt
    //
    // Example:
    //
    //   uart@80120000 {
    //       compatible = "arm,pl011", "arm,primecell";
    //       reg = <0x80120000 0x1000>;
    //       interrupts = <0 11 IRQ_TYPE_LEVEL_HIGH>;
    //       dmas = <&dma 13 0 0x2>, <&dma 13 0 0x0>;
    //       dma-names = "rx", "tx";
    //       clocks = <&foo_clk>, <&bar_clk>;
    //       clock-names = "uartclk", "apb_pclk";
    //   };
    //
    // qemu-arm-virt uses:
    //
    //   pl011@9000000 {
    //       compatible = "arm,pl011\0arm,primecell";
    //       reg = <0x00 0x9000000 0x00 0x1000>;
    //       interrupts = <0x00 0x01 0x04>;
    //       clocks = <0x8000 0x8000>;
    //       clock-names = "uartclk\0apb_pclk";
    //   };

    err = fdt_appendprop_string(fdt, node, "compatible", "arm,pl011");
    assert(!err);
    err = fdt_appendprop_string(fdt, node, "compatible", "arm,primecell");
    assert(!err);

    err = fdt_appendprop_u64(fdt, node, "reg", vpl011->base);
    assert(!err);
    err = fdt_appendprop_u64(fdt, node, "reg", vpl011->size);
    assert(!err);

    /* ToDO: clarify what to use here */
    err = fdt_appendprop_string(fdt, node, "clock-names", "uartclk");
    assert(!err);
    err = fdt_appendprop_string(fdt, node, "clock-names", "apb_pclk");
    assert(!err);
    err = fdt_appendprop_u32(fdt, node, "clocks", 0x8000);
    assert(!err);
    err = fdt_appendprop_u32(fdt, node, "clocks", 0x8000);
    assert(!err);

    /* Interrupt type: 0 = PPI, 1 = SPI */
    err = fdt_appendprop_u32(fdt, node, "interrupts", 0);
    assert(!err);
    /* Interrupt number, PPI/SPI may add an offset then */
    err = fdt_appendprop_u32(fdt, node, "interrupts", vpl011->intr);
    assert(!err);
    /* Interrupt trigger: 4 = IRQ_TYPE_LEVEL_HIGH */
    err = fdt_appendprop_u32(fdt, node, "interrupts", 4);
    assert(!err);

    return 0;
}


//------------------------------------------------------------------------------
static void vpl011_init_module(vm_t *vm, void *cookie)
{
    for(int i = 0; i < ARRAY_SIZE(all_vpl011); i++) {
        const vpl011_t *vpl011 = &all_vpl011[i];
        int err = vpl011_install(vm, &all_vpl011[i]);
        if (err) {
            ZF_LOGE("failed to install vpl011@%p (%d)", (void *)vpl011->base, err);
            return;
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DEFINE_MODULE(vpl011, NULL, vpl011_init_module)

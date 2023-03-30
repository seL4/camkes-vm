/*
 * Copyright 2023, Hensoldt Cyber
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <vmlinux.h>
#include <camkes.h>
#include <sel4vm/guest_vcpu_fault.h>

// This is a dummy driver modulke for the GiCv2's extension GICv2m
//
//
//  Random GIC notes related to qemu-arm-virt GICv2
//
//      intc@8000000 {
//          interrupt-controller;
//          compatible = "arm,cortex-a15-gic";
//          phandle = <0x8001>;
//          interrupts = <0x01 0x09 0x04>;
//          reg = <0x00 0x8000000 0x00 0x10000      // distributor
//                 0x00 0x8010000 0x00 0x10000      // cpu interface
//                 0x00 0x8030000 0x00 0x10000      // virtual interface
//                 0x00 0x8040000 0x00 0x10000>;    //  virtual cpu interface
//          ranges;
//          #size-cells = <0x02>;
//          #address-cells = <0x02>;
//          #interrupt-cells = <0x03>;
//
//          v2m@8020000 {
//              phandle = <0x8002>;
//              reg = <0x00 0x8020000 0x00 0x1000>;
//              msi-controller;
//              compatible = "arm,gic-v2m-frame";
//      };
//
//
//  Random GIC notes related to odroid-c2 GICv2
//
//  interrupt-controller@c4301000 {
//      interrupt-controller;
//      compatible = "arm,gic-400";
//      reg = < 0x00 0xc4301000 0x00 0x1000         // distributor
//              0x00 0xc4302000 0x00 0x2000         // cpu interface
//              0x00 0xc4304000 0x00 0x2000         // virtual interface
//              0x00 0xc4306000 0x00 0x2000 >;      // virtual cpu interface

//      interrupts = < 0x01 0x09 0xff04 >;
//      #interrupt-cells = < 0x03 >;
//      #address-cells = < 0x00 >;
//      phandle = < 0x01 >;
//  };


//
//------------------------------------------------------------------------------
//
// https://www.kernel.org/doc/Documentation/devicetree/bindings/interrupt-controller/arm%2Cgic.txt
//
// ARM Generic Interrupt Controller
//
// ARM SMP cores are often associated with a GIC, providing per processor
// interrupts (PPI), shared processor interrupts (SPI) and software
// generated interrupts (SGI).
//
// Primary GIC is attached directly to the CPU and typically has PPIs and SGIs.
// Secondary GICs are cascaded into the upward interrupt controller and do not
// have PPIs or SGIs.
//
// Main node required properties:
//
// - compatible : should be one of:
//     "arm,arm1176jzf-devchip-gic"
//     "arm,arm11mp-gic"
//     "arm,cortex-a15-gic"
//     "arm,cortex-a7-gic"
//     "arm,cortex-a9-gic"
//     "arm,eb11mp-gic"
//     "arm,gic-400"
//     "arm,pl390"
//     "arm,tc11mp-gic"
//     "brcm,brahma-b15-gic"
//     "nvidia,tegra210-agic"
//     "qcom,msm-8660-qgic"
//     "qcom,msm-qgic2"
// - interrupt-controller : Identifies the node as an interrupt controller
// - #interrupt-cells : Specifies the number of cells needed to encode an
//   interrupt source.  The type shall be a <u32> and the value shall be 3.
//
//   The 1st cell is the interrupt type; 0 for SPI interrupts, 1 for PPI
//   interrupts.
//
//   The 2nd cell contains the interrupt number for the interrupt type.
//   SPI interrupts are in the range [0-987].  PPI interrupts are in the
//   range [0-15].
//
//   The 3rd cell is the flags, encoded as follows:
//     bits[3:0] trigger type and level flags.
//         1 = low-to-high edge triggered
//         2 = high-to-low edge triggered (invalid for SPIs)
//         4 = active high level-sensitive
//         8 = active low level-sensitive (invalid for SPIs).
//     bits[15:8] PPI interrupt cpu mask.  Each bit corresponds to each of
//     the 8 possible cpus attached to the GIC.  A bit set to '1' indicated
//     the interrupt is wired to that CPU.  Only valid for PPI interrupts.
//     Also note that the configurability of PPI interrupts is IMPLEMENTATION
//     DEFINED and as such not guaranteed to be present (most SoC available
//     in 2014 seem to ignore the setting of this flag and use the hardware
//     default value).
//
// - reg : Specifies base physical address(s) and size of the GIC registers. The
//   first region is the GIC distributor register base and size. The 2nd region is
//   the GIC cpu interface register base and size.
//
// Optional
// - interrupts    : Interrupt source of the parent interrupt controller on
//   secondary GICs, or VGIC maintenance interrupt on primary GIC (see
//   below).
//
// - cpu-offset    : per-cpu offset within the distributor and cpu interface
//   regions, used when the GIC doesn't have banked registers. The offset is
//   cpu-offset * cpu-nr.
//
// - clocks        : List of phandle and clock-specific pairs, one for each entry
//   in clock-names.
// - clock-names   : List of names for the GIC clock input(s). Valid clock names
//   depend on the GIC variant:
//     "ic_clk" (for "arm,arm11mp-gic")
//     "PERIPHCLKEN" (for "arm,cortex-a15-gic")
//     "PERIPHCLK", "PERIPHCLKEN" (for "arm,cortex-a9-gic")
//     "clk" (for "arm,gic-400" and "nvidia,tegra210")
//     "gclk" (for "arm,pl390")
//
// - power-domains : A phandle and PM domain specifier as defined by bindings of
//           the power controller specified by phandle, used when the GIC
//           is part of a Power or Clock Domain.
//
//
// Example:
//
//     intc: interrupt-controller@fff11000 {
//         compatible = "arm,cortex-a9-gic";
//         #interrupt-cells = <3>;
//         #address-cells = <1>;
//         interrupt-controller;
//         reg = <0xfff11000 0x1000>,
//               <0xfff10100 0x100>;
//     };
//
//
// * GIC virtualization extensions (VGIC)
//
// For ARM cores that support the virtualization extensions, additional
// properties must be described (they only exist if the GIC is the
// primary interrupt controller).
//
// Required properties:
//
// - reg : Additional regions specifying the base physical address and
//   size of the VGIC registers. The first additional region is the GIC
//   virtual interface control register base and size. The 2nd additional
//   region is the GIC virtual cpu interface register base and size.
//
// - interrupts : VGIC maintenance interrupt.
//
// Example:
//
//     interrupt-controller@2c001000 {
//         compatible = "arm,cortex-a15-gic";
//         #interrupt-cells = <3>;
//         interrupt-controller;
//         reg = <0x2c001000 0x1000>,
//               <0x2c002000 0x2000>,
//               <0x2c004000 0x2000>,
//               <0x2c006000 0x2000>;
//         interrupts = <1 9 0xf04>;
//     };
//
//
// * GICv2m extension for MSI/MSI-x support (Optional)
//
// Certain revisions of GIC-400 supports MSI/MSI-x via V2M register frame(s).
// This is enabled by specifying v2m sub-node(s).
//
// Required properties:
//
// - compatible        : The value here should contain "arm,gic-v2m-frame".
//
// - msi-controller    : Identifies the node as an MSI controller.
//
// - reg            : GICv2m MSI interface register base and size
//
// Optional properties:
//
// - arm,msi-base-spi  : When the MSI_TYPER register contains an incorrect
//                 value, this property should contain the SPI base of
//               the MSI frame, overriding the HW value.
//
// - arm,msi-num-spis  : When the MSI_TYPER register contains an incorrect
//                 value, this property should contain the number of
//               SPIs assigned to the frame, overriding the HW value.
//
// Example:
//
//     interrupt-controller@e1101000 {
//         compatible = "arm,gic-400";
//         #interrupt-cells = <3>;
//         #address-cells = <2>;
//         #size-cells = <2>;
//         interrupt-controller;
//         interrupts = <1 8 0xf04>;
//         ranges = <0 0 0 0xe1100000 0 0x100000>;
//         reg = <0x0 0xe1110000 0 0x01000>,
//               <0x0 0xe112f000 0 0x02000>,
//               <0x0 0xe1140000 0 0x10000>,
//               <0x0 0xe1160000 0 0x10000>;
//         v2m0: v2m@8000 {
//             compatible = "arm,gic-v2m-frame";
//             msi-controller;
//             reg = <0x0 0x80000 0 0x1000>;
//         };
//
//         ....
//
//         v2mN: v2m@9000 {
//             compatible = "arm,gic-v2m-frame";
//             msi-controller;
//             reg = <0x0 0x90000 0 0x1000>;
//         };
//     };




#define PADDR_GIC2_M    0x08020000

typedef struct {
    uintptr_t     base;
    size_t        size;
    const char    *name;
} vgicv2m_t;



static const vgicv2m_t vgicv2m_0x08020000 = {
    .base = PADDR_GIC2_M,
    .size = PAGE_SIZE,
    .name = "vgicv2m@0x08020000",
};


//------------------------------------------------------------------------------
static bool is_addr_in_range(uintptr_t addr, uintptr_t start, size_t size)
{
    return ((addr >= start) || ((addr - start) < size));
}


//------------------------------------------------------------------------------
static bool is_addr_in_vgicv2m(vgicv2m_t const *vgicv2m, uintptr_t addr)
{
    return is_addr_in_range(addr, vgicv2m->base, vgicv2m->size);
}


//------------------------------------------------------------------------------
static memory_fault_result_t vgicv2m_handle_fault(vm_t *vm,
                                                 vm_vcpu_t *vcpu,
                                                 uintptr_t fault_addr,
                                                 size_t fault_length,
                                                 void *cookie)
{
    assert(vm == vcpu->vm);
    vgicv2m_t *vgicv2m = (vgicv2m_t *)cookie;
    bool is_read = is_vcpu_read_fault(vcpu);

    if (!is_addr_in_vgicv2m(vgicv2m, fault_addr)) {
        ZF_LOGI("%s: invalid addr %p", vgicv2m->name, (void *)fault_addr);
        if (is_read) {
            /* Out of range, treat as SBZ */
            set_vcpu_fault_data(vcpu, 0);
        }
        return FAULT_IGNORE;
    }

    size_t offset = fault_addr - vgicv2m->base;

    /* No implementation, just log some details. Seems Lnux tries to read at
     * offset 0x8 and if we return 0 is gives up further accesses.
     */
    ZF_LOGI("%s: %s at %p", vgicv2m->name, is_read?"read":"write", (void *)offset);
    if (is_read) {
        set_vcpu_fault_data(vcpu, 0);
    }
    advance_vcpu_fault(vcpu);
    return FAULT_HANDLED;

}


//------------------------------------------------------------------------------
static int vgicv2m_install(vm_t *vm, vgicv2m_t const *vgicv2m)
{
    ZF_LOGI("Installing vgicv2m at %p", (void *)(vgicv2m->base));

    void *cookie = (void *)vgicv2m; /* cast away the const */
    vm_memory_reservation_t *r = vm_reserve_memory_at(vm,
                                                      vgicv2m->base,
                                                      vgicv2m->size,
                                                      vgicv2m_handle_fault,
                                                      cookie);
    if (!r) {
        ZF_LOGE("failed to reserve memory for %s", vgicv2m->name);
        return -1;
    }

    return 0;
}


//------------------------------------------------------------------------------
static void vgicv2m_init_module(vm_t *vm, void *cookie)
{
    vgicv2m_t const *vgicv2m = &vgicv2m_0x08020000;
    int err = vgicv2m_install(vm, vgicv2m);
    if (err) {
        ZF_LOGE("failed to install %s (%d)", vgicv2m->name, err);
        return;
    }

}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DEFINE_MODULE(vgicv2m, NULL, vgicv2m_init_module)

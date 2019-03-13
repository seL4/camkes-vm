/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */
#pragma once

#define HARDWARE_ETHERNET_COMPONENT                                     \
    component HWEthDriverIMX6 {                                         \
        hardware;                                                       \
        emits IRQ irq;                                                  \
        dataport Buf(16384) mmio;                                       \
        dataport Buf(16384) ocotp;                                      \
        dataport Buf(16384) iomux;                                      \
        dataport Buf(16384) ccm;                                        \
        dataport Buf(4096) analog;                                      \
        dataport Buf(16384) gpio3;                                      \
        dataport Buf(16384) gpio6;                                      \
    }

#define HARDWARE_ETHERNET_INTERFACES                                    \
    dataport Buf(16384) EthDriver;                                      \
    dataport Buf(16384) ocotp;                                          \
    dataport Buf(16384) iomux;                                          \
    dataport Buf(16384) ccm;                                            \
    dataport Buf(4096) analog;                                          \
    dataport Buf(16384) gpio3;                                          \
    dataport Buf(16384) gpio6;                                          \
    consumes IRQ irq;        

#define HARDWARE_ETHERNET_COMPOSITION                                   \
    component HWEthDriverIMX6 hwethdriver;                              \
    connection seL4HardwareMMIO ethdrivermmio(from EthDriver,           \
                                              to hwethdriver.mmio);     \
    connection seL4HardwareMMIO ocotpmmio(from ocotp,                   \
                                          to hwethdriver.ocotp);        \
    connection seL4HardwareMMIO iomuxmmio(from iomux,                   \
                                          to hwethdriver.iomux);        \
    connection seL4HardwareMMIO ccmmmio(from ccm,                       \
                                        to hwethdriver.ccm);            \
    connection seL4HardwareMMIO analogmmio(from analog,                 \
                                           to hwethdriver.analog);      \
    connection seL4HardwareMMIO gpio3mmio(from gpio3,                   \
                                          to hwethdriver.gpio3);        \
    connection seL4HardwareMMIO gpio6mmio(from gpio6,                   \
                                          to hwethdriver.gpio6);        \
    connection seL4HardwareInterrupt hwethirq(from hwethdriver.irq,     \
                                              to irq);                  

#define HARDWARE_ETHERNET_CONFIG                                        \
    hwethdriver.mmio_paddr = 0x02188000;                                \
    hwethdriver.mmio_size = 0x4000;                                     \
    hwethdriver.ocotp_paddr = 0x021bc000;                               \
    hwethdriver.ocotp_size = 0x4000;                                    \
    hwethdriver.iomux_paddr = 0x020e0000;                               \
    hwethdriver.iomux_size = 0x4000;                                    \
    hwethdriver.ccm_paddr = 0x020c4000;                                 \
    hwethdriver.ccm_size = 0x4000;                                      \
    hwethdriver.analog_paddr = 0x020c8000;                              \
    hwethdriver.analog_size = 0x1000;                                   \
    hwethdriver.gpio3_paddr = 0x020a4000;                               \
    hwethdriver.gpio3_size = 0x4000;                                    \
    hwethdriver.gpio6_paddr = 0x020b0000;                               \
    hwethdriver.gpio6_size = 0x4000;                                    \
    hwethdriver.irq_irq_number = 150;


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
    component HWEthDriverZYNQ7000 {                                     \
        hardware;                                                       \
        emits IRQ irq;                                                  \
        dataport Buf(4096) mmio;                                        \
        dataport Buf(4096) slcr;                                        \
    }

#define HARDWARE_ETHERNET_INTERFACES                                    \
    dataport Buf(4096) EthDriver;                                       \
    dataport Buf(4096) slcr;                                            \
    consumes IRQ irq;

#define HARDWARE_ETHERNET_COMPOSITION                                   \
    component HWEthDriverZYNQ7000 hwethdriver;                          \
    connection seL4HardwareMMIO ethdrivermmio(from EthDriver,           \
                                              to hwethdriver.mmio);     \
    connection seL4HardwareMMIO slcrmmio(from slcr,                     \
                                         to hwethdriver.slcr);          \
    connection seL4HardwareInterrupt hwethirq(from hwethdriver.irq,     \
                                              to irq);                  

#define HARDWARE_ETHERNET_CONFIG                                        \
    hwethdriver.mmio_paddr = 0xe000b000;                                \
    hwethdriver.mmio_size = 0x1000;                                     \
    hwethdriver.slcr_paddr = 0xf8000000;                                \
    hwethdriver.slcr_size = 0x1000;                                     \
    hwethdriver.irq_irq_number = 54;                                    

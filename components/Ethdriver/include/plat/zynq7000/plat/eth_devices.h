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

#define HARDWARE_ETHERNET_COMPONENT

#define HARDWARE_ETHERNET_INTERFACES                                    \
    consumes Dummy EthDriver;                                           \
    consumes Dummy slcr;                                                \
    emits Dummy dummy_source;

#define HARDWARE_ETHERNET_COMPOSITION                                   \
    connection seL4DTBHardware ethdriver_conn(from dummy_source,        \
                                             to EthDriver);             \
    connection seL4DTBHardware slcr_conn(from dummy_source,             \
                                         to slcr);

#define HARDWARE_ETHERNET_CONFIG                                        \
    EthDriver.dtb = dtb({ "path" : "/amba/ethernet@e000b000" });        \
    EthDriver.generate_interrupts = 1;                                  \
    slcr.dtb = dtb({ "path" : "/amba/slcr@f8000000" });


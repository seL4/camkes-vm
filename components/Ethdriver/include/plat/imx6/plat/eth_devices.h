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

#define HARDWARE_ETHERNET_INTERFACES                                                \
    consumes Dummy EthDriver;                                                       \
    consumes Dummy ocotp;                                                           \
    consumes Dummy iomux;                                                           \
    consumes Dummy ccm;                                                             \
    consumes Dummy analog;                                                          \
    consumes Dummy gpio3;                                                           \
    consumes Dummy gpio6;                                                           \
    emits Dummy dummy_source;

#define HARDWARE_ETHERNET_COMPOSITION                                               \
    connection seL4DTBHardware ethernet_conn(from dummy_source, to EthDriver);      \
    connection seL4DTBHardware ocotp_conn(from dummy_source, to ocotp);             \
    connection seL4DTBHardware iomux_conn(from dummy_source, to iomux);             \
    connection seL4DTBHardware ccm_conn(from dummy_source, to ccm);                 \
    connection seL4DTBHardware analog_conn(from dummy_source, to analog);           \
    connection seL4DTBHardware gpio3_conn(from dummy_source, to gpio3);             \
    connection seL4DTBHardware gpio6_conn(from dummy_source, to gpio6);

#define HARDWARE_ETHERNET_CONFIG                                                    \
    EthDriver.dtb = dtb({ "path" : "/soc/aips-bus@2100000/ethernet@2188000" });     \
    EthDriver.generate_interrupts = 1;                                              \
    ocotp.dtb = dtb({ "path" : "/soc/aips-bus@2100000/ocotp@21bc000" });            \
    iomux.dtb = dtb({ "path" : "/soc/aips-bus@2000000/iomuxc@20e0000" });           \
    ccm.dtb = dtb({ "path" : "/soc/aips-bus@2000000/ccm@20c4000" });                \
    analog.dtb = dtb({ "path" : "/soc/aips-bus@2000000/anatop@20c8000" });          \
    gpio3.dtb = dtb({ "path" : "/soc/aips-bus@2000000/gpio@20a4000" });             \
    gpio6.dtb = dtb({ "path" : "/soc/aips-bus@2000000/gpio@20b0000" });


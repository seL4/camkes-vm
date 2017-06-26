/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <camkes.h>

#define CONFIG_ADDR 0xcf8
#define CONFIG_DATA 0xcfc

static void config_select(uint8_t bus, uint8_t dev, uint8_t fun, unsigned int offset) {
    /* Convert to 32bit values for doing bit shifting on */
    uint32_t lbus = bus;
    uint32_t ldev = dev;
    uint32_t lfun = fun;
    uint32_t address = (lbus << 16) | (ldev << 11) | (lfun << 8) | (offset & 0xfc) | 0x80000000u;
    config_address_out32(CONFIG_ADDR, address);
}

uint8_t pci_config_read8(uint8_t bus, uint8_t dev, uint8_t fun, unsigned int offset) {
    config_select(bus, dev, fun, offset);
    return config_data_in8(CONFIG_DATA + (offset & 3));
}

uint16_t pci_config_read16(uint8_t bus, uint8_t dev, uint8_t fun, unsigned int offset) {
    config_select(bus, dev, fun, offset);
    return config_data_in16(CONFIG_DATA + (offset & 2));
}

uint32_t pci_config_read32(uint8_t bus, uint8_t dev, uint8_t fun, unsigned int offset) {
    config_select(bus, dev, fun, offset);
    return config_data_in32(CONFIG_DATA);
}

void pci_config_write8(uint8_t bus, uint8_t dev, uint8_t fun, unsigned int offset, uint8_t val) {
    config_select(bus, dev, fun, offset);
    config_data_out8(CONFIG_DATA + (offset & 3), val);
}

void pci_config_write16(uint8_t bus, uint8_t dev, uint8_t fun, unsigned int offset, uint16_t val) {
    config_select(bus, dev, fun, offset);
    config_data_out16(CONFIG_DATA + (offset & 2), val);
}

void pci_config_write32(uint8_t bus, uint8_t dev, uint8_t fun, unsigned int offset, uint32_t val) {
    config_select(bus, dev, fun, offset);
    config_data_out32(CONFIG_DATA, val);
}

void pre_init(void) {
    set_putchar(putchar_putchar);
}

/*
 * Copyright 2026, Shankaraditya N S
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#pragma once

#define IRQ_SPI_OFFSET 32
#define GIC_NODE_PATH  "/soc/interrupt-controller@40041000"

static const int linux_pt_irqs[] = {};

/* SPI supplied to cross-VM connectors via get_crossvm_irq_num().
 * Must not be claimed as a passthrough IRQ by any VM on this platform.
 * SPI 50 does not appear in the BCM2711 device tree; the highest SPI
 * declared there is 0x9d. Not exercised by vm_minimal, which has no
 * cross-VM connections. */
static const int free_plat_interrupts[] =  { 50 + IRQ_SPI_OFFSET };
static const char *plat_keep_devices[] = {
    "/timer",
    "/uart_fixed_clk",
};
static const char *plat_keep_device_and_disable[] = {};
static const char *plat_keep_device_and_subtree[] = {
    GIC_NODE_PATH,
    "/soc/gpio@7e200000",
    "/soc/serial@7e201000",
};
static const char *plat_keep_device_and_subtree_and_disable[] = {};

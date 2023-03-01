/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#pragma once

#define IRQ_SPI_OFFSET 32
#define GIC_NODE_PATH  "/soc/interrupt-controller@10481000"

static const int linux_pt_irqs[] = {
};

static const int free_plat_interrupts[] =  { 92 + IRQ_SPI_OFFSET,
                                             93 + IRQ_SPI_OFFSET,
                                             101 + IRQ_SPI_OFFSET,
                                             102 + IRQ_SPI_OFFSET
                                           };
static const char *plat_keep_devices[] = {
    "/fixed-rate-clocks/oscclk",
    "/timer",
    "/soc/chipid@10000000",
    GIC_NODE_PATH
};
static const char *plat_keep_device_and_disable[] = {};
static const char *plat_keep_device_and_subtree[] = {};
static const char *plat_keep_device_and_subtree_and_disable[] = {};

/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#pragma once

#define IRQ_SPI_OFFSET 32
#define GIC_NODE_PATH  "/intc@8000000"

static const int linux_pt_irqs[] = {};

static const int free_plat_interrupts[] =  { 50 + IRQ_SPI_OFFSET };
static const char *plat_keep_devices[] = {
    "/timer",
    "/apb-pclk",
    "/platform@c000000",
    "/pmu",
    "/flash@0",
    "/psci",
};
static const char *plat_keep_device_and_disable[] = {};
static const char *plat_keep_device_and_subtree[] = {
    GIC_NODE_PATH,
};
static const char *plat_keep_device_and_subtree_and_disable[] = {};

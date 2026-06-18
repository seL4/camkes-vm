/*
 * Copyright 2026, STMicroelectronics
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#pragma once

#define IRQ_SPI_OFFSET 32
#define GIC_NODE_PATH     "/interrupt-controller@4ac00000"

static const int linux_pt_irqs[] = {};

static const int free_plat_interrupts[] =  { 264 + IRQ_SPI_OFFSET};

static const char *plat_keep_devices[] = {
    "/timer",
};

static const char *plat_keep_device_and_disable[] = {};
static const char *plat_keep_device_and_subtree[] = {
    GIC_NODE_PATH,
};
static const char *plat_keep_device_and_subtree_and_disable[] = {};

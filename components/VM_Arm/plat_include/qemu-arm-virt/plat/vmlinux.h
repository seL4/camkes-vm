/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#pragma once

#define LINUX_RAM_BASE    0x60000000
#define LINUX_RAM_PADDR_BASE LINUX_RAM_BASE
#define LINUX_RAM_SIZE    0x40000000
#define PLAT_RAM_END      0xc0000000
#define LINUX_RAM_OFFSET  0
#define DTB_ADDR          (LINUX_RAM_BASE + 0x0F000000)
#define INITRD_MAX_SIZE   0x1900000 //25 MB
#define INITRD_ADDR       (DTB_ADDR - INITRD_MAX_SIZE) //0x4D700000

#define IRQ_SPI_OFFSET 32
#define GIC_IRQ_PHANDLE 0x8001

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
    "/intc@8000000",
};
static const char *plat_keep_device_and_subtree_and_disable[] = {};

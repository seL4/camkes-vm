/*
 * Copyright 2023, Hensoldt Cyber
 * Copyright 2019, DornerWorks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 *
 * The "zynqmp" platform target means the zcu102 board.
 *
 */

#pragma once

#ifdef CONFIG_ZYNQMP_PETALINUX_2018_3
#define GIC_NODE_PATH "/amba_apu@0/interrupt-controller@f9010000"
#else
#define GIC_NODE_PATH "/axi/interrupt-controller@f9010000"
#endif

static const int linux_pt_irqs[] = {};

static const int free_plat_interrupts[] =  { -1 };

static const char *plat_keep_devices[] = {
    "/timer",
};
static const char *plat_keep_device_and_disable[] = {};
static const char *plat_keep_device_and_subtree[] = {
    GIC_NODE_PATH,
};
static const char *plat_keep_device_and_subtree_and_disable[] = {};
static const char *plat_linux_bootcmdline = "";
static const char *plat_linux_stdout = "";

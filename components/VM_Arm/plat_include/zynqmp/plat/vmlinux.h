/*
 * Copyright 2019, DornerWorks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define GIC_NODE_PATH "/amba_apu@0/interrupt-controller@f9010000"

static const int linux_pt_irqs[] = {};

static const int free_plat_interrupts[] =  { -1 };

static const char *plat_keep_devices[] = {};
static const char *plat_keep_device_and_disable[] = {};
static const char *plat_keep_device_and_subtree[] = {};
static const char *plat_keep_device_and_subtree_and_disable[] = {};
static const char *plat_linux_bootcmdline = "";
static const char *plat_linux_stdout = "";

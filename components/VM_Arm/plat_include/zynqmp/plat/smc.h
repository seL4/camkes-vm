/*
 * Copyright 2022, DornerWorks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define PM_GET_API_VERSION              0xC2000001
#define PM_SET_CONFIGURATION            0xC2000002
#define PM_GET_NODE_STATUS              0xC2000003
#define PM_GET_OPERATING_CHARACTERISTIC 0xC2000004
#define PM_REGISTER_NOTIFIER            0xC2000005
#define PM_REQUEST_SUSPEND              0xC2000006
#define PM_SELF_SUSPEND                 0xC2000007
#define PM_FORCE_POWERDOWN              0xC2000008
#define PM_ABORT_SUSPEND                0xC2000009
#define PM_REQUEST_WAKEUP               0xC200000A
#define PM_SET_WAKEUP_SOURCE            0xC200000B
#define PM_SYSTEM_SHUTDOWN              0xC200000C
#define PM_REQUEST_NODE                 0xC200000D
#define PM_RELEASE_NODE                 0xC200000E
#define PM_SET_REQUIREMENT              0xC200000F
#define PM_SET_MAX_LATENCY              0xC2000010
#define PM_RESET_ASSERT                 0xC2000011
#define PM_RESET_GET_STATUS             0xC2000012
#define PM_MMIO_WRITE                   0xC2000013
#define PM_MMIO_READ                    0xC2000014
#define PM_INIT_FINALIZE                0xC2000015
#define PM_FPGA_LOAD                    0xC2000016
#define PM_FPGA_GET_STATUS              0xC2000017
#define PM_GET_CHIPID                   0xC2000018
#define PM_SECURE_SHA                   0xC200001A
#define PM_SECURE_RSA                   0xC200001B
#define PM_PINCTRL_REQUEST              0xC200001C
#define PM_PINCTRL_RELEASE              0xC200001D
#define PM_PINCTRL_GET_FUNCTION         0xC200001E
#define PM_PINCTRL_SET_FUNCTION         0xC200001F
#define PM_PINCTRL_CONFIG_PARAM_GET     0xC2000020
#define PM_PINCTRL_CONFIG_PARAM_SET     0xC2000021
#define PM_IOCTL                        0xC2000022
#define PM_QUERY_DATA                   0xC2000023
#define PM_CLOCK_ENABLE                 0xC2000024
#define PM_CLOCK_DISABLE                0xC2000025
#define PM_CLOCK_GETSTATE               0xC2000026
#define PM_CLOCK_SETDIVIDER             0xC2000027
#define PM_CLOCK_GETDIVIDER             0xC2000028
#define PM_CLOCK_SETRATE                0xC2000029
#define PM_CLOCK_GETRATE                0xC200002A
#define PM_CLOCK_SETPARENT              0xC200002B
#define PM_CLOCK_GETPARENT              0xC200002C
#define PM_SECURE_IMAGE                 0xC200002D
#define PM_FPGA_READ                    0xC200002E
#define PM_SECURE_AES                   0xC200002F
#define PM_CLOCK_PLL_GETPARAM           0xC2000030
#define PM_REGISTER_ACCESS              0xC2000034
#define PM_EFUSE_ACCESS                 0xC2000035
#define PM_FEATURE_CHECK                0xC200003F
#define PM_API_MAX                      0xC2000040

#define IPI_MAILBOX_OPEN                0x82001000
#define IPI_MAILBOX_RELEASE             0x82001001
#define IPI_MAILBOX_STATUS_ENQUIRY      0x82001002
#define IPI_MAILBOX_NOTIFY              0x82001003
#define IPI_MAILBOX_ACK                 0x82001004
#define IPI_MAILBOX_ENABLE_IRQ          0x82001005
#define IPI_MAILBOX_DISABLE_IRQ         0x82001006

#define PM_GET_TRUSTZONE_VERSION        0xC2000A03

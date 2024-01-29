/*
 * Copyright 2022, DornerWorks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define SMC_PM_GET_API_VERSION              0xC2000001
#define SMC_PM_SET_CONFIGURATION            0xC2000002
#define SMC_PM_GET_NODE_STATUS              0xC2000003
#define SMC_PM_GET_OPERATING_CHARACTERISTIC 0xC2000004
#define SMC_PM_REGISTER_NOTIFIER            0xC2000005
#define SMC_PM_REQUEST_SUSPEND              0xC2000006
#define SMC_PM_SELF_SUSPEND                 0xC2000007
#define SMC_PM_FORCE_POWERDOWN              0xC2000008
#define SMC_PM_ABORT_SUSPEND                0xC2000009
#define SMC_PM_REQUEST_WAKEUP               0xC200000A
#define SMC_PM_SET_WAKEUP_SOURCE            0xC200000B
#define SMC_PM_SYSTEM_SHUTDOWN              0xC200000C
#define SMC_PM_REQUEST_NODE                 0xC200000D
#define SMC_PM_RELEASE_NODE                 0xC200000E
#define SMC_PM_SET_REQUIREMENT              0xC200000F
#define SMC_PM_SET_MAX_LATENCY              0xC2000010
#define SMC_PM_RESET_ASSERT                 0xC2000011
#define SMC_PM_RESET_GET_STATUS             0xC2000012
#define SMC_PM_MMIO_WRITE                   0xC2000013
#define SMC_PM_MMIO_READ                    0xC2000014
#define SMC_PM_INIT_FINALIZE                0xC2000015
#define SMC_PM_FPGA_LOAD                    0xC2000016
#define SMC_PM_FPGA_GET_STATUS              0xC2000017
#define SMC_PM_GET_CHIPID                   0xC2000018
#define SMC_PM_SECURE_SHA                   0xC200001A
#define SMC_PM_SECURE_RSA                   0xC200001B
#define SMC_PM_PINCTRL_REQUEST              0xC200001C
#define SMC_PM_PINCTRL_RELEASE              0xC200001D
#define SMC_PM_PINCTRL_GET_FUNCTION         0xC200001E
#define SMC_PM_PINCTRL_SET_FUNCTION         0xC200001F
#define SMC_PM_PINCTRL_CONFIG_PARAM_GET     0xC2000020
#define SMC_PM_PINCTRL_CONFIG_PARAM_SET     0xC2000021
#define SMC_PM_IOCTL                        0xC2000022
#define SMC_PM_QUERY_DATA                   0xC2000023
#define SMC_PM_CLOCK_ENABLE                 0xC2000024
#define SMC_PM_CLOCK_DISABLE                0xC2000025
#define SMC_PM_CLOCK_GETSTATE               0xC2000026
#define SMC_PM_CLOCK_SETDIVIDER             0xC2000027
#define SMC_PM_CLOCK_GETDIVIDER             0xC2000028
#define SMC_PM_CLOCK_SETRATE                0xC2000029
#define SMC_PM_CLOCK_GETRATE                0xC200002A
#define SMC_PM_CLOCK_SETPARENT              0xC200002B
#define SMC_PM_CLOCK_GETPARENT              0xC200002C
#define SMC_PM_SECURE_IMAGE                 0xC200002D
#define SMC_PM_FPGA_READ                    0xC200002E
#define SMC_PM_SECURE_AES                   0xC200002F
#define SMC_PM_CLOCK_PLL_GETPARAM           0xC2000030
#define SMC_PM_REGISTER_ACCESS              0xC2000034
#define SMC_PM_EFUSE_ACCESS                 0xC2000035
#define SMC_PM_ADD_SUBSYSTEM                0xC2000036
#define SMC_PM_FEATURE_CHECK                0xC200003F
#define SMC_PM_API_MAX                      0xC2000040

#define SMC_IPI_MAILBOX_OPEN                0x82001000
#define SMC_IPI_MAILBOX_RELEASE             0x82001001
#define SMC_IPI_MAILBOX_STATUS_ENQUIRY      0x82001002
#define SMC_IPI_MAILBOX_NOTIFY              0x82001003
#define SMC_IPI_MAILBOX_ACK                 0x82001004
#define SMC_IPI_MAILBOX_ENABLE_IRQ          0x82001005
#define SMC_IPI_MAILBOX_DISABLE_IRQ         0x82001006

#define SMC_PM_GET_TRUSTZONE_VERSION        0xC2000A03

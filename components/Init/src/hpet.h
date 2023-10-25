/*
 * Copyright 2019, DornerWorks
 * SPDX-License-Identifier: GPL-2.0-only
 */

#define HPET_BASE               0xfed00000
#define HPET_LEN                0x500
#define HPET_CLK_PERIOD         10 /* 10 ns*/

#define FS_PER_NS               1000000       /* 1000000 femtoseconds == 1 ns */
#define HPET_MIN_TIMERS         3
#define HPET_MAX_TIMERS         3

#define HPET_NUM_IRQ_ROUTES     32

#define HPET_LEGACY_PIT_INT     0
#define HPET_LEGACY_RTC_INT     1

#define HPET_CFG_ENABLE 0x001
#define HPET_CFG_LEGACY 0x002

#define HPET_ID         0x000
#define HPET_PERIOD     0x004
#define HPET_CFG        0x010
#define HPET_STATUS     0x020
#define HPET_COUNTER    0x0f0
#define HPET_TN_CFG     0x000
#define HPET_TN_CMP     0x008
#define HPET_TN_ROUTE   0x010
#define HPET_CFG_WRITE_MASK  0x3

#define HPET_ID_NUM_TIM_SHIFT   8
#define HPET_ID_NUM_TIM_MASK    0x1f00

#define HPET_TN_TYPE_LEVEL       0x002
#define HPET_TN_ENABLE           0x004
#define HPET_TN_PERIODIC         0x008
#define HPET_TN_PERIODIC_CAP     0x010
#define HPET_TN_SIZE_CAP         0x020
#define HPET_TN_SETVAL           0x040
#define HPET_TN_32BIT            0x100
#define HPET_TN_INT_ROUTE_MASK  0x3e00
#define HPET_TN_FSB_ENABLE      0x4000
#define HPET_TN_FSB_CAP         0x8000
#define HPET_TN_CFG_WRITE_MASK  0x7f4e
#define HPET_TN_INT_ROUTE_SHIFT      9
#define HPET_TN_INT_ROUTE_CAP_SHIFT 32
#define HPET_TN_CFG_BITS_READONLY_OR_RESERVED 0xffff80b1U

#define RTC_ISA_IRQ 8

typedef int (*timer_oneshot_callback_fn)(int p_tid, uint64_t p_ns);
typedef int (*timer_stop_callback_fn)(int p_tid);

void hpet_pre_init(uint64_t initial_tsc_frequency,
                   seL4_Word hpet_id_base,
                   timer_oneshot_callback_fn timer_oneshot_callback,
                   timer_stop_callback_fn timer_stop_callback);
int vm_create_hpet(vm_t *vm);

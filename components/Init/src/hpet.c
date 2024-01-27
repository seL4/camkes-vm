/*
 *  High Precision Event Timer emulation
 *
 *  Copyright (c) 2007 Alexander Graf
 *  Copyright (c) 2008 IBM Corporation
 *
 *  Authors: Beth Kon <bkon@us.ibm.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 * *****************************************************************
 *
 * This driver attempts to emulate an HPET device in software.
 */

/*
 * Copyright 2019, DornerWorks
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <autoconf.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <sel4/sel4.h>
#include <stdio.h>

#include <sel4vm/guest_vm.h>
#include <sel4vm/boot.h>
#include <sel4vm/guest_irq_controller.h>
#include <sel4vm/guest_vcpu_fault.h>

#include <platsupport/arch/tsc.h>

#include "hpet.h"

#define HPET_MSI_SUPPORT        0

#define HPET(obj) OBJECT_CHECK(HPETState, (obj), TYPE_HPET)

struct HPETState;
typedef struct HPETTimer {  /* timers */
    uint8_t tn;             /*timer number*/
    struct HPETState *state;
    /* Memory-mapped, software visible timer registers */
    uint64_t config;        /* configuration/cap */
    uint64_t cmp;           /* comparator */
    uint64_t fsb;           /* FSB route */
    /* Hidden register state */
    uint64_t period;        /* Last value written to comparator */
    uint8_t wrap_flag;      /* timer pop will indicate wrap for one-shot 32-bit
                             * mode. Next pop will be actual timer expiration.
                             */
    int tid;
} HPETTimer;

typedef struct HPETState {
    uint64_t hpet_offset;
    bool hpet_offset_saved;
    uint32_t flags;
    uint8_t rtc_irq_level;
    uint8_t num_timers;
    uint32_t intcap;

    timer_oneshot_callback_fn timer_oneshot_callback;
    timer_stop_callback_fn timer_stop_callback;

    HPETTimer timer[HPET_MAX_TIMERS];

    /* Memory-mapped, software visible registers */
    uint64_t capability;        /* capabilities */
    uint64_t config;            /* configuration */
    uint64_t isr;               /* interrupt status reg */
    uint64_t hpet_counter;      /* main counter */
    uint8_t  hpet_id;           /* instance id */
} HPETState;

static HPETState hpet_state;

extern vm_t vm;

static uint64_t tsc_frequency = 0;

static uint64_t current_time_ns()
{
    return muldivu64(rdtsc_pure(), NS_IN_S, tsc_frequency);
}

static uint32_t hpet_in_legacy_mode(HPETState *s)
{
    return s->config & HPET_CFG_LEGACY;
}

static uint32_t timer_int_route(struct HPETTimer *timer)
{
    return (timer->config & HPET_TN_INT_ROUTE_MASK) >> HPET_TN_INT_ROUTE_SHIFT;
}

static uint32_t timer_fsb_route(HPETTimer *t)
{
    return t->config & HPET_TN_FSB_ENABLE;
}

static uint32_t hpet_enabled(HPETState *s)
{
    return s->config & HPET_CFG_ENABLE;
}

static uint32_t timer_is_periodic(HPETTimer *t)
{
    return t->config & HPET_TN_PERIODIC;
}

static uint32_t timer_enabled(HPETTimer *t)
{
    return t->config & HPET_TN_ENABLE;
}

static uint32_t hpet_time_after(uint64_t a, uint64_t b)
{
    return ((int32_t)(b - a) < 0);
}

static uint32_t hpet_time_after64(uint64_t a, uint64_t b)
{
    return ((int64_t)(b - a) < 0);
}

static uint64_t ticks_to_ns(uint64_t value)
{
    return value * HPET_CLK_PERIOD;
}

static uint64_t ns_to_ticks(uint64_t value)
{
    return value / HPET_CLK_PERIOD;
}

static uint64_t hpet_fixup_reg(uint64_t new, uint64_t old, uint64_t mask)
{
    new &= mask;
    new |= old & ~mask;
    return new;
}

static int activating_bit(uint64_t old, uint64_t new, uint64_t mask)
{
    return (!(old & mask) && (new & mask));
}

static int deactivating_bit(uint64_t old, uint64_t new, uint64_t mask)
{
    return ((old & mask) && !(new & mask));
}

static uint64_t hpet_get_ticks(HPETState *s)
{
    return ns_to_ticks(current_time_ns() + s->hpet_offset);
}

/*
 * calculate diff between comparator value and current ticks
 */
static inline uint64_t hpet_calculate_diff(HPETTimer *t, uint64_t current)
{

    if (t->config & HPET_TN_32BIT) {
        uint32_t diff, cmp;

        cmp = (uint32_t)t->cmp;
        diff = cmp - (uint32_t)current;
        diff = (int32_t)diff > 0 ? diff : (uint32_t)1;
        return (uint64_t)diff;
    } else {
        uint64_t diff, cmp;

        cmp = t->cmp;
        diff = cmp - current;
        diff = (int64_t)diff > 0 ? diff : (uint64_t)1;
        return diff;
    }
}

static void update_irq(struct HPETTimer *timer, int set)
{
    uint64_t mask;
    HPETState *s;
    int route;

    if (timer->tn <= 1 && hpet_in_legacy_mode(timer->state)) {
        /* if LegacyReplacementRoute bit is set, HPET specification requires
         * timer0 be routed to IRQ0 in NON-APIC or IRQ2 in the I/O APIC,
         * timer1 be routed to IRQ8 in NON-APIC or IRQ8 in the I/O APIC.
         */
        route = (timer->tn == 0) ? 0 : RTC_ISA_IRQ;
    } else {
        route = timer_int_route(timer);
    }
    s = timer->state;
    mask = 1 << timer->tn;
    if (!set || !timer_enabled(timer) || !hpet_enabled(timer->state)) {
        s->isr &= ~mask;
        if (!timer_fsb_route(timer)) {

            vm_set_irq_level(vm.vcpus[BOOT_VCPU], route, 0);
        }
    } else if (timer_fsb_route(timer)) {

    } else if (timer->config & HPET_TN_TYPE_LEVEL) {
        s->isr |= mask;
        vm_set_irq_level(vm.vcpus[BOOT_VCPU], route, 1);
    } else {
        s->isr &= ~mask;
        vm_set_irq_level(vm.vcpus[BOOT_VCPU], route, 1);
        vm_set_irq_level(vm.vcpus[BOOT_VCPU], route, 0);
    }
}

static int hpet_pre_save(void *opaque)
{
    HPETState *s = opaque;

    /* save current counter value */
    if (hpet_enabled(s)) {
        s->hpet_counter = hpet_get_ticks(s);
    }

    return 0;
}

static int hpet_pre_load(void *opaque)
{
    HPETState *s = opaque;

    /* version 1 only supports 3, later versions will load the actual value */
    s->num_timers = HPET_MIN_TIMERS;
    return 0;
}

static bool hpet_validate_num_timers(void *opaque, int version_id)
{
    HPETState *s = opaque;

    if (s->num_timers < HPET_MIN_TIMERS) {
        return false;
    } else if (s->num_timers > HPET_MAX_TIMERS) {
        return false;
    }
    return true;
}

static int hpet_post_load(void *opaque, int version_id)
{
    HPETState *s = opaque;

    /* Recalculate the offset between the main counter and guest time */
    if (!s->hpet_offset_saved) {
        uint64_t ticks = ticks_to_ns(s->hpet_counter);
        uint64_t cur = current_time_ns();
        if (cur > ticks) {
            ZF_LOGD("Underflow");
            s->hpet_offset = 0;
        } else {
            s->hpet_offset = ticks - cur;
        }
    }

    /* Push number of timers into capability returned via HPET_ID */
    s->capability &= ~HPET_ID_NUM_TIM_MASK;
    s->capability |= (s->num_timers - 1) << HPET_ID_NUM_TIM_SHIFT;

    /* Derive HPET_MSI_SUPPORT from the capability of the first timer. */
    s->flags &= ~(1 << HPET_MSI_SUPPORT);
    if (s->timer[0].config & HPET_TN_FSB_CAP) {
        s->flags |= 1 << HPET_MSI_SUPPORT;
    }
    return 0;
}

static bool hpet_offset_needed(void *opaque)
{
    HPETState *s = opaque;

    return hpet_enabled(s) && s->hpet_offset_saved;
}

static bool hpet_rtc_irq_level_needed(void *opaque)
{
    HPETState *s = opaque;

    return s->rtc_irq_level != 0;
}

/*
 * timer expiration callback
 */
static void hpet_timer(void *opaque)
{
    HPETTimer *t = opaque;
    uint64_t diff;

    uint64_t period = t->period;
    uint64_t cur_tick = hpet_get_ticks(t->state);

    if (timer_is_periodic(t) && period != 0) {
        if (t->config & HPET_TN_32BIT) {
            while (hpet_time_after(cur_tick, t->cmp)) {
                t->cmp = (uint32_t)(t->cmp + t->period);
            }
        } else {
            while (hpet_time_after64(cur_tick, t->cmp)) {
                t->cmp += period;
            }
        }
        diff = hpet_calculate_diff(t, cur_tick);
        t->state->timer_oneshot_callback(t->tid, (int64_t)ticks_to_ns(diff));
    } else if (t->config & HPET_TN_32BIT && !timer_is_periodic(t)) {
        if (t->wrap_flag) {
            diff = hpet_calculate_diff(t, cur_tick);
            t->state->timer_oneshot_callback(t->tid, (int64_t)ticks_to_ns(diff));
            t->wrap_flag = 0;
        }
    }
    update_irq(t, 1);
}

static void hpet_set_timer(HPETTimer *t)
{
    uint64_t diff;
    uint32_t wrap_diff;  /* how many ticks until we wrap? */
    uint64_t cur_tick = hpet_get_ticks(t->state);

    /* whenever new timer is being set up, make sure wrap_flag is 0 */
    t->wrap_flag = 0;
    diff = hpet_calculate_diff(t, cur_tick);

    /* hpet spec says in one-shot 32-bit mode, generate an interrupt when
     * counter wraps in addition to an interrupt with comparator match.
     */
    if (t->config & HPET_TN_32BIT && !timer_is_periodic(t)) {
        wrap_diff = 0xffffffff - (uint32_t)cur_tick;
        if (wrap_diff < (uint32_t)diff) {
            diff = wrap_diff;
            t->wrap_flag = 1;
        }
    }
    t->state->timer_oneshot_callback(t->tid, (int64_t)ticks_to_ns(diff));
}

static void hpet_del_timer(HPETTimer *t)
{
    t->state->timer_stop_callback(t->tid);
    update_irq(t, 0);
}

static void vm_hpet_mmio_read(vm_vcpu_t *vcpu, void *opaque, uint32_t offset,
                              int size, seL4_Word *result)
{
    HPETState *s = opaque;
    uint64_t cur_tick, index;

    uintptr_t addr = HPET_BASE + offset;

    ZF_LOGI("Enter vm_hpet_mmio_readl at %" PRIx64 "", addr);

    index = offset;
    /*address range of all TN regs*/
    if (index >= 0x100 && index <= 0x3ff) {
        uint8_t timer_id = (addr - 0x100) / 0x20;
        HPETTimer *timer = &s->timer[timer_id];

        if (timer_id > s->num_timers) {
            ZF_LOGE("timer id out of range - %d", timer_id);
            *result = 0;
            return;
        }

        switch ((addr - 0x100) % 0x20) {
        case HPET_TN_CFG:
            *result = (seL4_Word)timer->config;
            break;
        case HPET_TN_CFG + 4: // Interrupt capabilities
            *result = (seL4_Word)(timer->config >> 32);
            break;
        case HPET_TN_CMP: // comparator register
            *result = (seL4_Word)timer->cmp;
            break;
        case HPET_TN_CMP + 4:
            *result = (seL4_Word)(timer->cmp >> 32);
            break;
        case HPET_TN_ROUTE:
            *result = (seL4_Word)timer->fsb;
            break;
        case HPET_TN_ROUTE + 4:
            *result = (seL4_Word)(timer->fsb >> 32);
            break;
        default:
            *result = 0;
            printf("invalid %s - %lx\n", __func__, (seL4_Word)index);
            break;
        }
    } else {
        switch (index) {
        case HPET_ID:
            *result = (seL4_Word)s->capability;
            break;
        case HPET_PERIOD:
            *result = (seL4_Word)(s->capability >> 32);
            break;
        case HPET_CFG:
            *result = (seL4_Word)s->config;
            break;
        case HPET_CFG + 4:
            ZF_LOGE("invalid HPET_CFG + 4");
            *result = 0;
            break;
        case HPET_COUNTER:
            if (hpet_enabled(s)) {
                cur_tick = hpet_get_ticks(s);
            } else {
                cur_tick = s->hpet_counter;
            }
            *result = (seL4_Word)cur_tick;
            break;
        case HPET_COUNTER + 4:
            if (hpet_enabled(s)) {
                cur_tick = hpet_get_ticks(s);
            } else {
                cur_tick = s->hpet_counter;
            }
            *result = (seL4_Word)(cur_tick >> 32);
            break;
        case HPET_STATUS:
            *result = (seL4_Word)s->isr;
            break;
        default:
            *result = 0;
            printf("invalid %s - %lx\n", __func__, (seL4_Word)index);
            break;
        }
    }
}

static void vm_hpet_mmio_write(vm_vcpu_t *vcpu, void *opaque, uint32_t offset,
                               int size, seL4_Word value)
{
    int i;
    HPETState *s = opaque;
    uint64_t new_val, val, index;
    seL4_Word old_val;

    uintptr_t addr = HPET_BASE + offset;

    ZF_LOGD("Enter at %lx = %lx", addr, value);
    index = offset;
    uint64_t res;
    vm_hpet_mmio_read(vcpu, opaque, offset, 4, &old_val);
    new_val = (uint64_t)value;

    /*address range of all TN regs*/
    if (index >= 0x100 && index <= 0x3ff) {
        uint8_t timer_id = (addr - 0x100) / 0x20;
        if (timer_id > s->num_timers) {
            ZF_LOGE("timer id %d out of range", timer_id);
            return;
        }
        HPETTimer *timer = &s->timer[timer_id];

        switch ((addr - 0x100) % 0x20) {
        case HPET_TN_CFG:
            if (activating_bit(old_val, new_val, HPET_TN_FSB_ENABLE)) {
                update_irq(timer, 0);
            }
            val = hpet_fixup_reg(new_val, old_val, HPET_TN_CFG_WRITE_MASK);
            timer->config = (timer->config & 0xffffffff00000000ULL) | val;
            if (new_val & HPET_TN_32BIT) {
                timer->cmp = (uint32_t)timer->cmp;
                timer->period = (uint32_t)timer->period;
            }
            if (activating_bit(old_val, new_val, HPET_TN_ENABLE) &&
                hpet_enabled(s)) {
                hpet_set_timer(timer);
            } else if (deactivating_bit(old_val, new_val, HPET_TN_ENABLE)) {
                hpet_del_timer(timer);
            }
            break;
        case HPET_TN_CFG + 4: // Interrupt capabilities
            ZF_LOGE("invalid HPET_TN_CFG+4 write");
            break;
        case HPET_TN_CMP: // comparator register
            if (timer->config & HPET_TN_32BIT) {
                new_val = (uint32_t)new_val;
            }
            if (!timer_is_periodic(timer)
                || (timer->config & HPET_TN_SETVAL)) {
                timer->cmp = (timer->cmp & 0xffffffff00000000ULL) | new_val;
            }
            if (timer_is_periodic(timer)) {
                /*
                 * FIXME: Clamp period to reasonable min value?
                 * Clamp period to reasonable max value
                 */
                new_val &= (timer->config & HPET_TN_32BIT ? ~0u : ~0ull) >> 1;
                timer->period =
                    (timer->period & 0xffffffff00000000ULL) | new_val;
            }
            timer->config &= ~HPET_TN_SETVAL;
            if (hpet_enabled(s)) {
                hpet_set_timer(timer);
            }
            break;
        case HPET_TN_CMP + 4: // comparator register high order
            if (!timer_is_periodic(timer)
                || (timer->config & HPET_TN_SETVAL)) {
                timer->cmp = (timer->cmp & 0xffffffffULL) | new_val << 32;
            } else {
                /*
                 * FIXME: Clamp period to reasonable min value?
                 * Clamp period to reasonable max value
                 */
                new_val &= (timer->config & HPET_TN_32BIT ? ~0u : ~0ull) >> 1;
                timer->period =
                    (timer->period & 0xffffffffULL) | new_val << 32;
            }
            timer->config &= ~HPET_TN_SETVAL;
            if (hpet_enabled(s)) {
                hpet_set_timer(timer);
            }
            break;
        case HPET_TN_ROUTE:
            timer->fsb = (timer->fsb & 0xffffffff00000000ULL) | new_val;
            break;
        case HPET_TN_ROUTE + 4:
            timer->fsb = (new_val << 32) | (timer->fsb & 0xffffffff);
            break;
        default:
            ZF_LOGE("invalid offset: %lx <- %lx", addr, value);
            break;
        }
        return;
    } else {
        switch (index) {
        case HPET_ID:
            return;
        case HPET_CFG:
            val = hpet_fixup_reg(new_val, old_val, HPET_CFG_WRITE_MASK);
            s->config = (s->config & 0xffffffff00000000ULL) | val;
            if (activating_bit(old_val, new_val, HPET_CFG_ENABLE)) {
                /* Enable main counter and interrupt generation. */
                uint64_t ticks = ticks_to_ns(s->hpet_counter);
                uint64_t cur = current_time_ns();
                if (cur > ticks) {
                    ZF_LOGD("Underflow");
                    s->hpet_offset = 0;
                } else {
                    s->hpet_offset = ticks - cur;
                }
                for (i = 0; i < s->num_timers; i++) {
                    if ((&s->timer[i])->cmp != ~0ULL) {
                        hpet_set_timer(&s->timer[i]);
                    }
                }
            } else if (deactivating_bit(old_val, new_val, HPET_CFG_ENABLE)) {
                /* Halt main counter and disable interrupt generation. */
                s->hpet_counter = hpet_get_ticks(s);
                for (i = 0; i < s->num_timers; i++) {
                    hpet_del_timer(&s->timer[i]);
                }
            }
            /* i8254 and RTC output pins are disabled
             * when HPET is in legacy mode */
            if (activating_bit(old_val, new_val, HPET_CFG_LEGACY)) {
                vm_set_irq_level(vm.vcpus[BOOT_VCPU], 0, 0);
                vm_set_irq_level(vm.vcpus[BOOT_VCPU], RTC_ISA_IRQ, 0);
            } else if (deactivating_bit(old_val, new_val, HPET_CFG_LEGACY)) {
                vm_set_irq_level(vm.vcpus[BOOT_VCPU], 0, 0);
                vm_set_irq_level(vm.vcpus[BOOT_VCPU], RTC_ISA_IRQ, s->rtc_irq_level);
            }
            break;
        case HPET_CFG + 4:
            ZF_LOGE("invalid HPET_CFG+4 write");
            break;
        case HPET_STATUS:
            val = new_val & s->isr;
            for (i = 0; i < s->num_timers; i++) {
                if (val & (1 << i)) {
                    update_irq(&s->timer[i], 0);
                }
            }
            break;
        case HPET_COUNTER:
            s->hpet_counter =
                (s->hpet_counter & 0xffffffff00000000ULL) | value;
            break;
        case HPET_COUNTER + 4:
            s->hpet_counter =
                (s->hpet_counter & 0xffffffffULL) | (((uint64_t)value) << 32);
            break;
        default:
            ZF_LOGE("invalid %lx <- %lx", addr, value);
            break;
        }
    }
}

memory_fault_result_t hpet_fault_callback(vm_t *vm, vm_vcpu_t *vcpu,
                                          uintptr_t fault_addr, size_t fault_length,
                                          void *cookie)
{
    seL4_Word data;
    if (is_vcpu_read_fault(vcpu)) {
        vm_hpet_mmio_read(vcpu, cookie, fault_addr - HPET_BASE, fault_length, &data);
        set_vcpu_fault_data(vcpu, data);
    } else {
        data = get_vcpu_fault_data(vcpu);
        vm_hpet_mmio_write(vcpu, cookie, fault_addr - HPET_BASE, fault_length, data);
    }
    advance_vcpu_fault(vcpu);
    return FAULT_HANDLED;
}

static void hpet_reset(HPETState *s)
{
    int i;

    for (i = 0; i < s->num_timers; i++) {
        HPETTimer *timer = &s->timer[i];
        hpet_del_timer(timer);
        timer->cmp = ~0ULL;
        timer->config = HPET_TN_PERIODIC_CAP | HPET_TN_SIZE_CAP;
        if (s->flags & (1 << HPET_MSI_SUPPORT)) {
            timer->config |= HPET_TN_FSB_CAP;
        }
        /* advertise availability of ioapic int */
        timer->config |= (uint64_t)s->intcap << 32;
        timer->period = 0ULL;
        timer->wrap_flag = 0;
    }

    s->hpet_counter = 0ULL;
    s->hpet_offset = 0ULL;
    s->config = 0ULL;

    /* to document that the RTC lowers its output on reset as well */
    s->rtc_irq_level = 0;
}

static void hpet_handle_legacy_irq(void *opaque, int n, int level)
{
    HPETState *s = opaque;

    if (n == HPET_LEGACY_PIT_INT) {
        if (!hpet_in_legacy_mode(s)) {
            vm_set_irq_level(vm.vcpus[BOOT_VCPU], 0, level);
        }
    } else {
        s->rtc_irq_level = level;
        if (!hpet_in_legacy_mode(s)) {
            vm_set_irq_level(vm.vcpus[BOOT_VCPU], RTC_ISA_IRQ, level);
        }
    }
}

static void hpet_realize(HPETState *s, seL4_Word HPET_ID_BASE,
                         timer_oneshot_callback_fn timer_oneshot_callback,
                         timer_stop_callback_fn timer_stop_callback)
{
    HPETTimer *timer;

    s->hpet_id = 0;
    s->timer_oneshot_callback = timer_oneshot_callback;
    s->timer_stop_callback = timer_stop_callback;

    for (int i = 0; i < HPET_MAX_TIMERS; i++) {
        timer = &s->timer[i];
        timer->tid = HPET_ID_BASE + i;
        timer->tn = i;
        timer->state = s;
    }

    s->num_timers = HPET_MIN_TIMERS;

    /* 64-bit main counter; LegacyReplacementRoute. */
    s->capability = 0x8086a001ULL;
    s->capability |= (s->num_timers - 1) << HPET_ID_NUM_TIM_SHIFT;
    s->capability |= ((uint64_t)(HPET_CLK_PERIOD * FS_PER_NS) << 32);
}

void hpet_timer_interrupt(int completed)
{
    for (int i = 0; i < HPET_MAX_TIMERS; i++) {
        HPETTimer *timer = &hpet_state.timer[i];
        if (completed & BIT(timer->tid)) {
            hpet_timer((void *)timer);
        }
    }
}

int vm_create_hpet(vm_t *vm)
{
    vm_vcpu_t *vcpu = vm->vcpus[BOOT_VCPU];
    assert(NULL != vcpu);

    vm_memory_reservation_t *hpet_reservation;
    hpet_reservation = vm_reserve_memory_at(vm, HPET_BASE, HPET_LEN,
                                            hpet_fault_callback,
                                            &hpet_state);
    if (!hpet_reservation) {
        ZF_LOGE("Failed to reserve hpet memory");
        return -1;
    }

    return 0;
}

void hpet_pre_init(uint64_t initial_tsc_frequency,
                   seL4_Word hpet_id_base,
                   timer_oneshot_callback_fn timer_oneshot_callback,
                   timer_stop_callback_fn timer_stop_callback)
{
    tsc_frequency = initial_tsc_frequency;
    hpet_realize(&hpet_state,
                 hpet_id_base,
                 timer_oneshot_callback,
                 timer_stop_callback);
    hpet_reset(&hpet_state);
}

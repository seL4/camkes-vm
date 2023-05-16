// SPDX-License-Identifier: MIT

/*
 * QEMU 8253/8254 interval timer emulation
 *
 * Copyright (c) 2003-2004 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//#include "qemu/osdep.h"
//#include "hw/hw.h"
//#include "hw/isa/isa.h"
//#include "qemu/timer.h"
//#include "hw/timer/i8254.h"
//#include "hw/timer/i8254_internal.h"

#include <autoconf.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <sel4/sel4.h>
#include <stdio.h>
#include <camkes.h>
#include <sel4vm/arch/ioports.h>
#include <sel4vm/guest_irq_controller.h>
#include <sel4vm/boot.h>
#include <platsupport/arch/tsc.h>

#include "timers.h"
#include "virtio_irq.h"

//#define DEBUG_PIT

#define PIT_FREQ 1193182

#define RW_STATE_LSB 1
#define RW_STATE_MSB 2
#define RW_STATE_WORD0 3
#define RW_STATE_WORD1 4

#define PIT_CLASS(class) OBJECT_CLASS_CHECK(PITClass, (class), TYPE_I8254)
#define PIT_GET_CLASS(obj) OBJECT_GET_CLASS(PITClass, (obj), TYPE_I8254)

typedef struct PITChannelInfo {
    int gate;
    int mode;
    int initial_count;
    int out;
} PITChannelInfo;

typedef struct PITChannelState {
    int count; /* can be 65536 */
    uint16_t latched_count;
    uint8_t count_latched;
    uint8_t status_latched;
    uint8_t status;
    uint8_t read_state;
    uint8_t write_state;
    uint8_t write_latch;
    uint8_t rw_mode;
    uint8_t mode;
    uint8_t bcd; /* not supported */
    uint8_t gate; /* timer start */
    int64_t count_load_time;
    /* irq handling */
    int64_t next_transition_time;
//    QEMUTimer *irq_timer;
    int irq_timer;
//    qemu_irq irq;
    uint32_t irq_disabled;
} PITChannelState;

typedef struct PITCommonState {
//    ISADevice dev;
//    MemoryRegion ioports;
//    uint32_t iobase;
    PITChannelState channels[3];
} PITCommonState;

static PITCommonState pit_state;

extern vm_t vm;

static void pit_irq_timer_update(PITChannelState *s, int64_t current_time);

static uint64_t tsc_frequency = 0;

static uint64_t current_time_ns()
{
    return muldivu64(rdtsc_pure(), NS_IN_S, tsc_frequency);
}


static int pit_get_count(PITChannelState *s)
{
    uint64_t d;
    int counter;

    d = muldivu64(current_time_ns()/*qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL)*/ - s->count_load_time, PIT_FREQ,
                  NS_IN_S);
    switch (s->mode) {
    case 0:
    case 1:
    case 4:
    case 5:
        counter = (s->count - d) & 0xffff;
        break;
    case 3:
        /* XXX: may be incorrect for odd counts */
        counter = s->count - ((2 * d) % s->count);
        break;
    default:
        counter = s->count - (d % s->count);
        break;
    }
    return counter;
}

/* val must be 0 or 1 */
#if 0
static void pit_set_channel_gate(PITCommonState *s, PITChannelState *sc,
                                 int val)
{
    switch (sc->mode) {
    default:
    case 0:
    case 4:
        /* XXX: just disable/enable counting */
        break;
    case 1:
    case 5:
        if (sc->gate < val) {
            /* restart counting on rising edge */
            sc->count_load_time = current_time_ns()/*qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL)*/;
            pit_irq_timer_update(sc, sc->count_load_time);
        }
        break;
    case 2:
    case 3:
        if (sc->gate < val) {
            /* restart counting on rising edge */
            sc->count_load_time = current_time_ns()/*qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL)*/;
            pit_irq_timer_update(sc, sc->count_load_time);
        }
        /* XXX: disable/enable counting */
        break;
    }
    sc->gate = val;
}
#endif

/* val must be 0 or 1 */
#if 0
void pit_set_gate(ISADevice *dev, int channel, int val)
{
    PITCommonState *pit = PIT_COMMON(dev);
    PITChannelState *s = &pit->channels[channel];
    PITCommonClass *c = PIT_COMMON_GET_CLASS(pit);

    c->set_channel_gate(pit, s, val);
}
#endif

/* get pit output bit */
int pit_get_out(PITChannelState *s, int64_t current_time)
{
    uint64_t d;
    int out;

    d = muldivu64(current_time - s->count_load_time, PIT_FREQ,
                  NS_IN_S);
    switch (s->mode) {
    default:
    case 0:
        out = (d >= s->count);
        break;
    case 1:
        out = (d < s->count);
        break;
    case 2:
        if ((d % s->count) == 0 && d != 0) {
            out = 1;
        } else {
            out = 0;
        }
        break;
    case 3:
        out = (d % s->count) < ((s->count + 1) >> 1);
        break;
    case 4:
    case 5:
        out = (d == s->count);
        break;
    }
    return out;
}

/* return -1 if no transition will occur.  */
int64_t pit_get_next_transition_time(PITChannelState *s, int64_t current_time)
{
    uint64_t d, next_time, base;
    int period2;

    d = muldivu64(current_time - s->count_load_time, PIT_FREQ,
                  NS_IN_S);
    switch (s->mode) {
    default:
    case 0:
    case 1:
        if (d < s->count) {
            next_time = s->count;
        } else {
            return -1;
        }
        break;
    case 2:
        base = ROUND_DOWN(d, s->count);
        if ((d - base) == 0 && d != 0) {
            next_time = base + s->count;
        } else {
            next_time = base + s->count + 1;
        }
        break;
    case 3:
        base = ROUND_DOWN(d, s->count);
        period2 = ((s->count + 1) >> 1);
        if ((d - base) < period2) {
            next_time = base + period2;
        } else {
            next_time = base + s->count;
        }
        break;
    case 4:
    case 5:
        if (d < s->count) {
            next_time = s->count;
        } else if (d == s->count) {
            next_time = s->count + 1;
        } else {
            return -1;
        }
        break;
    }
    /* convert to timer units */
    next_time = s->count_load_time + muldivu64(next_time, NS_IN_S,
                                               PIT_FREQ);
    /* fix potential rounding problems */
    /* XXX: better solution: use a clock at PIT_FREQ Hz */
    if (next_time <= current_time) {
        next_time = current_time + 1;
    }
    return next_time;
}

void pit_get_channel_info_common(PITCommonState *s, PITChannelState *sc,
                                 PITChannelInfo *info)
{
    info->gate = sc->gate;
    info->mode = sc->mode;
    info->initial_count = sc->count;
    info->out = pit_get_out(sc, current_time_ns()/*qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL)*/);
}

#if 0
void pit_get_channel_info(ISADevice *dev, int channel, PITChannelInfo *info)
{
    PITCommonState *pit = PIT_COMMON(dev);
    PITChannelState *s = &pit->channels[channel];
    PITCommonClass *c = PIT_COMMON_GET_CLASS(pit);

    c->get_channel_info(pit, s, info);
}
#endif

void pit_reset_common(PITCommonState *pit)
{
    PITChannelState *s;
    int i;

    for (i = 0; i < 3; i++) {
        s = &pit->channels[i];
        s->mode = 3;
        s->gate = (i != 2);
        s->count_load_time = current_time_ns()/*qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL)*/;
        s->count = 0x10000;
        if (i == 0 && !s->irq_disabled) {
            s->next_transition_time =
                pit_get_next_transition_time(s, s->count_load_time);
        }
    }
}
static inline void pit_load_count(PITChannelState *s, int val)
{
    if (val == 0) {
        val = 0x10000;
    }
    s->count_load_time = current_time_ns()/*qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL)*/;
    s->count = val;
    pit_irq_timer_update(s, s->count_load_time);
}

/* if already latched, do not latch again */
static void pit_latch_count(PITChannelState *s)
{
    if (!s->count_latched) {
        s->latched_count = pit_get_count(s);
        s->count_latched = s->rw_mode;
    }
}

static void pit_ioport_write(void *opaque, uintptr_t addr,
                             uint64_t val, unsigned size)
{
    PITCommonState *pit = opaque;
    int channel, access;
    PITChannelState *s;

    addr &= 3;
    if (addr == 3) {
        channel = val >> 6;
        if (channel == 3) {
            /* read back command */
            for (channel = 0; channel < 3; channel++) {
                s = &pit->channels[channel];
                if (val & (2 << channel)) {
                    if (!(val & 0x20)) {
                        pit_latch_count(s);
                    }
                    if (!(val & 0x10) && !s->status_latched) {
                        /* status latch */
                        /* XXX: add BCD and null count */
                        s->status =
                            (pit_get_out(s,
                                         current_time_ns()/*qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL)*/) << 7) |
                            (s->rw_mode << 4) |
                            (s->mode << 1) |
                            s->bcd;
                        s->status_latched = 1;
                    }
                }
            }
        } else {
            s = &pit->channels[channel];
            access = (val >> 4) & 3;
            if (access == 0) {
                pit_latch_count(s);
            } else {
                s->rw_mode = access;
                s->read_state = access;
                s->write_state = access;

                s->mode = (val >> 1) & 7;
                s->bcd = val & 1;
                /* XXX: update irq timer ? */
            }
        }
    } else {
        s = &pit->channels[addr];
        switch (s->write_state) {
        default:
        case RW_STATE_LSB:
            pit_load_count(s, val);
            break;
        case RW_STATE_MSB:
            pit_load_count(s, val << 8);
            break;
        case RW_STATE_WORD0:
            s->write_latch = val;
            s->write_state = RW_STATE_WORD1;
            break;
        case RW_STATE_WORD1:
            pit_load_count(s, s->write_latch | (val << 8));
            s->write_state = RW_STATE_WORD0;
            break;
        }
    }
}

static uint64_t pit_ioport_read(void *opaque, uintptr_t addr,
                                unsigned size)
{
    PITCommonState *pit = opaque;
    int ret, count;
    PITChannelState *s;

    addr &= 3;

    if (addr == 3) {
        /* Mode/Command register is write only, read is ignored */
        return 0;
    }

    s = &pit->channels[addr];
    if (s->status_latched) {
        s->status_latched = 0;
        ret = s->status;
    } else if (s->count_latched) {
        switch (s->count_latched) {
        default:
        case RW_STATE_LSB:
            ret = s->latched_count & 0xff;
            s->count_latched = 0;
            break;
        case RW_STATE_MSB:
            ret = s->latched_count >> 8;
            s->count_latched = 0;
            break;
        case RW_STATE_WORD0:
            ret = s->latched_count & 0xff;
            s->count_latched = RW_STATE_MSB;
            break;
        }
    } else {
        switch (s->read_state) {
        default:
        case RW_STATE_LSB:
            count = pit_get_count(s);
            ret = count & 0xff;
            break;
        case RW_STATE_MSB:
            count = pit_get_count(s);
            ret = (count >> 8) & 0xff;
            break;
        case RW_STATE_WORD0:
            count = pit_get_count(s);
            ret = count & 0xff;
            s->read_state = RW_STATE_WORD1;
            break;
        case RW_STATE_WORD1:
            count = pit_get_count(s);
            ret = (count >> 8) & 0xff;
            s->read_state = RW_STATE_WORD0;
            break;
        }
    }
    return ret;
}

static void pit_irq_timer_update(PITChannelState *s, int64_t current_time)
{
    int64_t expire_time;
    int irq_level;

    if (!s->irq_timer || s->irq_disabled) {
        return;
    }
    expire_time = pit_get_next_transition_time(s, current_time);
    irq_level = pit_get_out(s, current_time);
    //qemu_set_irq(s->irq, irq_level);
    vm_set_irq_level(vm.vcpus[BOOT_VCPU], TIMER_IRQ, irq_level);
#ifdef DEBUG_PIT
    printf("irq_level=%d next_delay=%f\n",
           irq_level,
           (double)(expire_time - current_time) / NS_IN_S);
#endif
    s->next_transition_time = expire_time;
    if (expire_time != -1) {
        init_timer_oneshot_absolute(TIMER_PIT, expire_time);
    }
    //timer_mod(s->irq_timer, expire_time);
    else {
        init_timer_stop(TIMER_PIT);
    }
    //timer_del(s->irq_timer);
}

static void pit_irq_timer(void *opaque)
{
    PITChannelState *s = opaque;

    pit_irq_timer_update(s, s->next_transition_time);
}

static void pit_reset(/*DeviceState *dev*/PITCommonState *pit)
{
//    PITCommonState *pit = PIT_COMMON(dev);
    PITChannelState *s;

    pit_reset_common(pit);

    s = &pit->channels[0];
    if (!s->irq_disabled) {
        //timer_mod(s->irq_timer, s->next_transition_time);
        init_timer_oneshot_absolute(TIMER_PIT, s->next_transition_time);
    }
}

/* When HPET is operating in legacy mode, suppress the ignored timer IRQ,
 * reenable it when legacy mode is left again. */
static void pit_irq_control(void *opaque, int n, int enable)
{
    PITCommonState *pit = opaque;
    PITChannelState *s = &pit->channels[0];

    if (enable) {
        s->irq_disabled = 0;
        pit_irq_timer_update(s, current_time_ns()/*qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL)*/);
    } else {
        s->irq_disabled = 1;
        //timer_del(s->irq_timer);
        init_timer_stop(TIMER_PIT);
    }
}

#if 0
static const MemoryRegionOps pit_ioport_ops = {
    .read = pit_ioport_read,
    .write = pit_ioport_write,
    .impl = {
        .min_access_size = 1,
        .max_access_size = 1,
    },
    .endianness = DEVICE_LITTLE_ENDIAN,
};
#endif

static void pit_post_load(PITCommonState *s)
{
    PITChannelState *sc = &s->channels[0];

    if (sc->next_transition_time != -1) {
        //timer_mod(sc->irq_timer, sc->next_transition_time);
        init_timer_oneshot_absolute(TIMER_PIT, sc->next_transition_time);
    } else {
        //timer_del(sc->irq_timer);
        init_timer_stop(TIMER_PIT);
    }
}

#if 0
static void pit_realizefn(DeviceState *dev, Error **errp)
{
    PITCommonState *pit = PIT_COMMON(dev);
    PITClass *pc = PIT_GET_CLASS(dev);
    PITChannelState *s;

    s = &pit->channels[0];
    /* the timer 0 is connected to an IRQ */
    s->irq_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, pit_irq_timer, s);
    qdev_init_gpio_out(dev, &s->irq, 1);

    memory_region_init_io(&pit->ioports, OBJECT(pit), &pit_ioport_ops,
                          pit, "pit", 4);

    qdev_init_gpio_in(dev, pit_irq_control, 1);

    pc->parent_realize(dev, errp);
}

static Property pit_properties[] = {
    DEFINE_PROP_UINT32("iobase", PITCommonState, iobase,  -1),
    DEFINE_PROP_END_OF_LIST(),
};

static void pit_class_initfn(ObjectClass *klass, void *data)
{
    PITClass *pc = PIT_CLASS(klass);
    PITCommonClass *k = PIT_COMMON_CLASS(klass);
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_parent_realize(dc, pit_realizefn, &pc->parent_realize);
    k->set_channel_gate = pit_set_channel_gate;
    k->get_channel_info = pit_get_channel_info_common;
    k->post_load = pit_post_load;
    dc->reset = pit_reset;
    dc->props = pit_properties;
}

static const TypeInfo pit_info = {
    .name          = TYPE_I8254,
    .parent        = TYPE_PIT_COMMON,
    .instance_size = sizeof(PITCommonState),
    .class_init    = pit_class_initfn,
    .class_size    = sizeof(PITClass),
};

static void pit_register_types(void)
{
    type_register_static(&pit_info);
}

type_init(pit_register_types)
#endif

void pit_timer_interrupt(void)
{
    PITCommonState *pit = &pit_state;
    PITChannelState *s;
    s = &pit->channels[0];
    pit_irq_timer(s);
}

void pit_pre_init(void)
{
    tsc_frequency = init_timer_tsc_frequency();
    pit_state.channels[0].irq_timer = 1;
    pit_irq_control(&pit_state, 0, 1);
    pit_reset(&pit_state);
}

ioport_fault_result_t i8254_port_in(vm_vcpu_t *vcpu, void *cookie, unsigned int port_no, unsigned int size,
                                    unsigned int *result)
{
    if (size != 1) {
        LOG_ERROR("i8254 only supports reads of size 1");
        return IO_FAULT_ERROR;
    }
    *result = pit_ioport_read(&pit_state, port_no, 1);
    return IO_FAULT_HANDLED;
}

ioport_fault_result_t i8254_port_out(vm_vcpu_t *vcpu, void *cookie, unsigned int port_no, unsigned int size,
                                     unsigned int value)
{
    if (size != 1) {
        LOG_ERROR("i8254 only supports writes of size 1");
        return IO_FAULT_ERROR;
    }
    pit_ioport_write(&pit_state, port_no, value, 1);
    return IO_FAULT_HANDLED;
}


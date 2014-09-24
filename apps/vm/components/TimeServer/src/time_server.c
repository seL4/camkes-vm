/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#include <autoconf.h>

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <sel4/sel4.h>
#include <sel4/arch/constants.h>
#include <TimeServer.h>
#include <platsupport/plat/hpet.h>
#include "vm.h"
#include <boost/preprocessor/repeat.hpp>

#ifdef CONFIG_APP_CAMKES_VM_HPET_MSI
#define TIMER_IRQ (MSI_MIN + IRQ_OFFSET) //24 + IRQ_OFFSET
#else
#define TIMER_IRQ HPET_IRQ()
#endif

static pstimer_t *timer = NULL;

#define TIMER_TYPE_OFF 0
#define TIMER_TYPE_PERIODIC 1
#define TIMER_TYPE_ABSOLUTE 2
#define TIMER_TYPE_RELATIVE 3

typedef struct client_state {
    int id;
    int timer_type;
    uint64_t periodic_ns;
    uint64_t timeout_time;
} client_state_t;

static client_state_t client_state[VM_NUM_TIMERS];
static client_state_t *sorted_clients[VM_NUM_TIMERS];

static uint64_t current_timeout = 0;

//static uint64_t tsc_frequency = 0;

#define TIMER_COMPLETE_EMIT_OUTPUT(a, vm, b) BOOST_PP_CAT(timer##vm,_complete_emit),
static void (*timer_complete_emit[])(void) = {
    BOOST_PP_REPEAT(VM_NUM_TIMERS, TIMER_COMPLETE_EMIT_OUTPUT, _)
};

static void signal_client(int id) {
    timer_complete_emit[id]();
    switch(client_state[id].timer_type) {
    case TIMER_TYPE_OFF:
        assert(!"not possible");
        break;
    case TIMER_TYPE_PERIODIC:
        client_state[id].timeout_time += client_state[id].periodic_ns;
        assert(client_state[id].periodic_ns > 0);
        break;
    case TIMER_TYPE_ABSOLUTE:
    case TIMER_TYPE_RELATIVE:
        client_state[id].timer_type = TIMER_TYPE_OFF;
        break;
    }
}

static void signal_clients(uint64_t current_time) {
    for (int i = 0; i < VM_NUM_TIMERS; i++) {
        if (client_state[i].timer_type != TIMER_TYPE_OFF &&
            client_state[i].timeout_time <= current_time) {
            signal_client(i);
        }
    }
}

static int client_cmp(const void *a, const void *b) {
    const client_state_t *aa = *(const client_state_t**)a;
    const client_state_t *bb = *(const client_state_t**)b;
    if (aa->timer_type == TIMER_TYPE_OFF && bb->timer_type == TIMER_TYPE_OFF) {
        return 0;
    } else if (aa->timer_type == TIMER_TYPE_OFF && bb->timer_type != TIMER_TYPE_OFF) {
        return 1;
    } else if (aa->timer_type != TIMER_TYPE_OFF && bb->timer_type == TIMER_TYPE_OFF) {
        return -1;
    } else if (aa->timeout_time == bb->timeout_time) {
        return 0;
    } else if (aa->timeout_time < bb->timeout_time) {
        return -1;
    } else if (aa->timeout_time > bb->timeout_time) {
        return 1;
    }
    assert(!"unreachable");
    __builtin_unreachable();
}

static void sort_clients() {
    qsort(sorted_clients, VM_NUM_TIMERS, sizeof(client_state_t*), client_cmp);
}

void reprogram_timer() {
    sort_clients();
    if (sorted_clients[0]->timer_type == TIMER_TYPE_OFF) {
        timer_stop(timer);
        current_timeout = 0;
        return;
    }
    uint64_t timeout = sorted_clients[0]->timeout_time;
    if (timeout == current_timeout) {
        return;
    }
    if (current_timeout == 0) {
        timer_start(timer);
    }
    current_timeout = timeout;
    if (!timer_oneshot_absolute(timer, timeout)) {
        return;
    }
    /* deadline already passed. try again */
    signal_client(sorted_clients[0]->id);
    reprogram_timer();
}

static void timer_interrupt(void *cookie) {
    time_server_lock();
    signal_clients(timer_get_time(timer));
    timer_handle_irq(timer, TIMER_IRQ);
    reprogram_timer();
    irq_reg_callback(timer_interrupt, cookie);
    time_server_unlock();
}

static int _oneshot_relative(int id, uint64_t ns) {
    time_server_lock();
    client_state[id].timer_type = TIMER_TYPE_RELATIVE;
    client_state[id].timeout_time = timer_get_time(timer) + ns;
    reprogram_timer();
    time_server_unlock();
    return 0;
}

static int _oneshot_absolute(int id, uint64_t ns) {
    time_server_lock();
    client_state[id].timer_type = TIMER_TYPE_ABSOLUTE;
    client_state[id].timeout_time = ns;
    reprogram_timer();
    time_server_unlock();
    return 0;
}

static int _periodic(int id, uint64_t ns) {
    time_server_lock();
    client_state[id].timer_type = TIMER_TYPE_PERIODIC;
    client_state[id].periodic_ns = ns;
    client_state[id].timeout_time = timer_get_time(timer) + ns;
    reprogram_timer();
    time_server_unlock();
    return 0;
}

static int _stop(int id) {
    time_server_lock();
    client_state[id].timer_type = TIMER_TYPE_OFF;
    reprogram_timer();
    time_server_unlock();
    return 0;
}

static uint64_t _time(int id) {
    uint64_t ret;
    time_server_lock();
    ret = timer_get_time(timer);
    time_server_unlock();
    return ret;
}

/* Generate stub interfaces for each camkes interface */
#define INTERFACE_OUTPUT(unused1, n, unused2) \
    int BOOST_PP_CAT(timer##n, _oneshot_relative)(uint64_t ns) { \
        return _oneshot_relative(n, ns); \
    } \
    int BOOST_PP_CAT(timer##n, _oneshot_absolute)(uint64_t ns) { \
        return _oneshot_absolute(n, ns); \
    } \
    int BOOST_PP_CAT(timer##n, _periodic)(uint64_t ns) { \
        return _periodic(n, ns); \
    } \
    int BOOST_PP_CAT(timer##n, _stop)() { \
        return _stop(n); \
    } \
    uint64_t BOOST_PP_CAT(timer##n, _time)() { \
        return _time(n); \
    } \
    /**/

BOOST_PP_REPEAT(VM_NUM_TIMERS, INTERFACE_OUTPUT, _);

void pre_init() {
    time_server_lock();
    set_putchar(putchar_putchar);
    for (int i = 0; i < VM_NUM_TIMERS; i++) {
        client_state[i].id = i;
        client_state[i].timer_type = TIMER_TYPE_OFF;
        sorted_clients[i] = &client_state[i];
    }
    int ioapic;
#ifdef CONFIG_APP_CAMKES_VM_HPET_MSI
    ioapic = 0;
#else
    ioapic = 1;
#endif
    hpet_config_t config = (hpet_config_t){.vaddr = (void*)hpet, .irq = TIMER_IRQ, .ioapic_delivery = ioapic};
    timer = hpet_get_timer(&config);
    assert(timer);
//    tsc_frequency = tsc_calculate_frequency(timer);
//    assert(tsc_frequency);
    irq_reg_callback(timer_interrupt, NULL);
    time_server_unlock();
}

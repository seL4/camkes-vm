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

#define TIMER_IRQ (MSI_MIN + IRQ_OFFSET) //16 + IRQ_OFFSET

#define NUM_CLIENTS 9

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

static client_state_t client_state[NUM_CLIENTS];
static client_state_t *sorted_clients[NUM_CLIENTS];

static uint64_t current_timeout = 0;

//static uint64_t tsc_frequency = 0;

static void signal_client(int id) {
    switch(id) {
    case 0:
        timer0_complete_emit();
        break;
    case 1:
        timer1_complete_emit();
        break;
    case 2:
        timer2_complete_emit();
        break;
    case 3:
        timer3_complete_emit();
        break;
    case 4:
        timer4_complete_emit();
        break;
    case 5:
        timer5_complete_emit();
        break;
    case 6:
        timer6_complete_emit();
        break;
    case 7:
        timer7_complete_emit();
        break;
    case 8:
        timer8_complete_emit();
        break;
    default:
        LOG_ERROR("Unknown client %d\n", id);
        assert(!"unknown client");
    }
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
    for (int i = 0; i < NUM_CLIENTS; i++) {
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
    qsort(sorted_clients, NUM_CLIENTS, sizeof(client_state_t*), client_cmp);
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

int timer0_oneshot_relative(uint64_t ns) {
    return _oneshot_relative(0, ns);
}

int timer0_oneshot_absolute(uint64_t ns) {
    return _oneshot_absolute(0, ns);
}

int timer0_periodic(uint64_t ns) {
    return _periodic(0, ns);
}

int timer0_stop() {
    return _stop(0);
}

uint64_t timer0_time() {
    return _time(0);
}

int timer1_oneshot_relative(uint64_t ns) {
    return _oneshot_relative(1, ns);
}

int timer1_oneshot_absolute(uint64_t ns) {
    return _oneshot_absolute(1, ns);
}

int timer1_periodic(uint64_t ns) {
    return _periodic(1, ns);
}

int timer1_stop() {
    return _stop(1);
}

uint64_t timer1_time() {
    return _time(1);
}

int timer2_oneshot_relative(uint64_t ns) {
    return _oneshot_relative(2, ns);
}

int timer2_oneshot_absolute(uint64_t ns) {
    return _oneshot_absolute(2, ns);
}

int timer2_periodic(uint64_t ns) {
    return _periodic(2, ns);
}

int timer2_stop() {
    return _stop(2);
}

uint64_t timer2_time() {
    return _time(2);
}

int timer3_oneshot_relative(uint64_t ns) {
    return _oneshot_relative(3, ns);
}

int timer3_oneshot_absolute(uint64_t ns) {
    return _oneshot_absolute(3, ns);
}

int timer3_periodic(uint64_t ns) {
    return _periodic(3, ns);
}

int timer3_stop() {
    return _stop(3);
}

uint64_t timer3_time() {
    return _time(3);
}

int timer4_oneshot_relative(uint64_t ns) {
    return _oneshot_relative(4, ns);
}

int timer4_oneshot_absolute(uint64_t ns) {
    return _oneshot_absolute(4, ns);
}

int timer4_periodic(uint64_t ns) {
    return _periodic(4, ns);
}

int timer4_stop() {
    return _stop(4);
}

uint64_t timer4_time() {
    return _time(4);
}

int timer5_oneshot_relative(uint64_t ns) {
    return _oneshot_relative(5, ns);
}

int timer5_oneshot_absolute(uint64_t ns) {
    return _oneshot_absolute(5, ns);
}

int timer5_periodic(uint64_t ns) {
    return _periodic(5, ns);
}

int timer5_stop() {
    return _stop(5);
}

uint64_t timer5_time() {
    return _time(5);
}

int timer6_oneshot_relative(uint64_t ns) {
    return _oneshot_relative(6, ns);
}

int timer6_oneshot_absolute(uint64_t ns) {
    return _oneshot_absolute(6, ns);
}

int timer6_periodic(uint64_t ns) {
    return _periodic(6, ns);
}

int timer6_stop() {
    return _stop(6);
}

uint64_t timer6_time() {
    return _time(6);
}

int timer7_oneshot_relative(uint64_t ns) {
    return _oneshot_relative(7, ns);
}

int timer7_oneshot_absolute(uint64_t ns) {
    return _oneshot_absolute(7, ns);
}

int timer7_periodic(uint64_t ns) {
    return _periodic(7, ns);
}

int timer7_stop() {
    return _stop(7);
}

uint64_t timer7_time() {
    return _time(7);
}

int timer8_oneshot_relative(uint64_t ns) {
    return _oneshot_relative(8, ns);
}

int timer8_oneshot_absolute(uint64_t ns) {
    return _oneshot_absolute(8, ns);
}

int timer8_periodic(uint64_t ns) {
    return _periodic(8, ns);
}

int timer8_stop() {
    return _stop(8);
}

uint64_t timer8_time() {
    return _time(8);
}

void pre_init() {
    time_server_lock();
    set_putchar(putchar_putchar);
    for (int i = 0; i < NUM_CLIENTS; i++) {
        client_state[i].id = i;
        client_state[i].timer_type = TIMER_TYPE_OFF;
        sorted_clients[i] = &client_state[i];
    }
    hpet_config_t config = (hpet_config_t){.vaddr = (void*)hpet, .irq = TIMER_IRQ};
    timer = hpet_get_timer(&config);
    assert(timer);
//    tsc_frequency = tsc_calculate_frequency(timer);
//    assert(tsc_frequency);
    irq_reg_callback(timer_interrupt, NULL);
    time_server_unlock();
}

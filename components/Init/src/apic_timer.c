/*
 * Copyright 2022, UNSW (ABN 57 195 873 179)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/**
 * This file abstracts the time_server implementation away from the
 * x86 APIC timer. It also provides all the functions necessary for the APIC
 * timer to be emulated. See: sel4vmmplatsupport/arch/drivers/timer_emul.h
*/
#include <camkes.h>

#include "timers.h"

uint64_t apic_tsc_freq(void)
{
    return init_timer_tsc_frequency();
}

int apic_oneshot_absolute(uint64_t ns)
{
    return init_timer_oneshot_absolute(TIMER_APIC, ns);
}

int apic_oneshot_relative(uint64_t ns)
{
    return init_timer_oneshot_relative(TIMER_APIC, ns);
}

int apic_timer_stop(void)
{
    return init_timer_stop(TIMER_APIC);
}

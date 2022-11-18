/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

/* define the different timer ids for the init component. */
/* TODO: Allocate timers from within the devices so that we are not polluting
 * all of the source with nitty gritty details */

/* pit timers */
#define TIMER_PIT 0

/* rtc timers*/
#define TIMER_PERIODIC_TIMER 1
#define TIMER_COALESCED_TIMER 2
#define TIMER_SECOND_TIMER 3
#define TIMER_SECOND_TIMER2 4

/* serial timers */
#define TIMER_FIFO_TIMEOUT 5
#define TIMER_TRANSMIT_TIMER 6
#define TIMER_MODEM_STATUS_TIMER 7
#define TIMER_MORE_CHARS 8

#define TIMER_APIC 9

/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
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

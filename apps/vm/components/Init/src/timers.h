/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#ifndef VM_INIT_TIMERS_H
#define VM_INIT_TIMERS_H

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

#endif

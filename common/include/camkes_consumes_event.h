/*
 * Copyright 2016, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(D61_BSD)
 */

#pragma once

typedef void (*camkes_event_callback_fn)(void *arg);

typedef struct camkes_event {
    unsigned int id;
    int (*reg_callback)(camkes_event_callback_fn, void *arg);
} camkes_event_t;

/* Registers a provided callback function with the event,
 * passing the callback a pointer to the camkes_event_t.
 */
static inline int
camkes_event_reg_callback_self(camkes_event_t *event, camkes_event_callback_fn cb)
{
    return event->reg_callback(cb, (void*)event);
}

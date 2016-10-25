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

#include <camkes.h>

#define BUSY_DELAY 2000000000llu

int run(void) {

    while (1) {
        // busy wait for a while
        for (volatile long long int i = 0; i < BUSY_DELAY; i++);

        // send event
        periodic_emit();
    }

    return 0;
}

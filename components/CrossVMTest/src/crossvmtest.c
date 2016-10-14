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

#include <autoconf.h>
#include <camkes.h>
#include <stdio.h>

#define DP1_SIZE 4096
#define DP2_SIZE (4096 * 16)

int run(void) {
    set_putchar(putchar_putchar);

    volatile char *dp1_data = (volatile char*)dp1;
    volatile char *dp2_data = (volatile char*)dp2;

    while(1) {
        /* Poll the start of the second page of the large dataport,
         * then print the contents of both dataports. This is to let
         * the guest populate at least a page of data before we start
         * printing. Note that it's possible that not all the data
         * will be transferred by the time we start printing it.
         */
        if (dp2_data[4096]) {

            /* Null terminate the ends of the buffers */
            dp1_data[DP1_SIZE - 1] = '\0';
            dp2_data[DP2_SIZE - 1] = '\0';

            printf("################ dp1 ###############\n"
                   "%s\n"
                   "################ dp2 ###############\n"
                   "%s\n\n", dp1_data, dp2_data);
            break;
        }
    }

    return 0;
}

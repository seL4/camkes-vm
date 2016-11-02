/*
 * Copyright 2016, Data 61
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(D61_GPL)
 */

#include <camkes.h>
#include <string_reverse.h>

static void reverse_dataport_string(volatile void *src, volatile void *dest, size_t n) {

    volatile char *src_str = (volatile char*)src;
    volatile char *dest_str = (volatile char*)dest;

    int len = 0;
    while(len < n && src_str[len] != '\0') {
        len++;
    }

    for (int i = 0; i < len; i++) {
        dest_str[i] = src_str[len - i - 1];
    }

    if (len < n) {
        dest_str[len] = '\0';
    } else {
        dest_str[n - 1] = '\0';
    }
}

int run(void) {

    set_putchar(putchar_putchar);

    while (1) {
        ready_wait();

        reverse_dataport_string(src_dp, dest_dp, STRING_REVERSE_BUFSIZE);

        done_emit();
    }

    return 0;
}

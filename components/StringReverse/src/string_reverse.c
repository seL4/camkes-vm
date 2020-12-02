/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <string.h>

#include <camkes.h>
#include <string_reverse.h>

static void reverse_dataport_string(volatile char *src, volatile char *dest, size_t n)
{

    int len = strnlen((char *)src, n - 1);

    for (int i = 0; i < len; i++) {
        dest[i] = src[len - i - 1];
    }

    dest[len] = '\0';
}

int run(void)
{

    set_putchar(putchar_putchar);

    while (1) {
        ready_wait();

        reverse_dataport_string(src_dp, dest_dp, STRING_REVERSE_BUFSIZE);

        done_emit();
    }

    return 0;
}

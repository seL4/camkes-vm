/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#include <sel4cxx/fixup.hpp>
#include <algorithm>
#include <string>

extern "C" {
#include <Echo.h>
}

static char string_buf[100];

static char *echo_string(const char *s) {
    std::string str(s);
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    std::copy(str.begin(), str.end(), string_buf);
    return string_buf;
}

extern "C" {
char * s_echo_string(const char *s) {
    return echo_string(s);
}
}

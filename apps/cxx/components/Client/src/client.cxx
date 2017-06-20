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

extern "C" {
#include <stdio.h>
#include <Client.h>
}

/* Do some pointless template things to prove we can*/
template<typename T>
T echo_fun(T thing, T (*echo)(T)) {
    T ret;
    ret = echo(thing);
    return ret;
}

int echo_int(int i) { return i; }

static int cxx_run() {
    printf("Echo gave us: %s\n", echo_fun<const char *>((const char*)"hello world", (const char*(*)(const char*))s_echo_string));
    return 0;
}

extern "C" {

int run() {
    return cxx_run();
}

}

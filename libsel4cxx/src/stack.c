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

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* If we're linking against a prebuilt libc++ then it may have been built with stack protection, but the libc
 * we use does not have stack protection functions. So implement some dummy wrappers here */

void * __memcpy_chk(void * dest, const void * src, size_t len, size_t destlen) {
    return memcpy(dest, src, len);
}

int __snprintf_chk(char * str, size_t maxlen, int flag, size_t strlen, const char * format,...) {
    va_list ap;
    va_start(ap, format);
    int ret = vsnprintf(str, maxlen, format, ap);
    va_end(ap);
    return ret;
}

int __sprintf_chk(char * str, int flag, size_t strlen, const char * format,...) {
    va_list ap;
    va_start(ap, format);
    int ret = vsprintf(str, format, ap);
    va_end(ap);
    return ret;
}

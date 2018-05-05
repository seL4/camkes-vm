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

#ifndef SEL4_CXX_FIXUP_HPP
#define SEL4_CXX_FIXUP_HPP

#ifndef __GLIBC_PREREQ
#define __GLIBC_PREREQ(...) 0
#endif

#define __DEFINED_max_align_t

#ifndef NDEBUG
#define _GLIBCXX_DEBUG
#endif

extern "C" {
#define __NEED_locale_t
#include <bits/alltypes.h>
typedef locale_t __locale_t;
}

#include <sel4cxx/ctypefixup.hpp>

#endif

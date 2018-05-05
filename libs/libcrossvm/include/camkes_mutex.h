/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#pragma once

#include <utils/attribute.h>

typedef int (*camkes_mutex_lock_fn)(void);
typedef int (*camkes_mutex_unlock_fn)(void);

typedef struct camkes_mutex {
    camkes_mutex_lock_fn lock;
    camkes_mutex_lock_fn unlock;
} camkes_mutex_t;

WARN_UNUSED_RESULT
static inline int
camkes_mutex_lock(camkes_mutex_t *mutex)
{
    return mutex->lock();
}

WARN_UNUSED_RESULT
static inline int
camkes_mutex_unlock(camkes_mutex_t *mutex)
{
    return mutex->unlock();
}

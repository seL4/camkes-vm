/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */
#ifndef VMM_CONFIG_RINGBUF_H
#define VMM_CONFIG_RINGBUF_H

typedef struct Ringbuf {
    /* 128 KiB ringbuf */
    char buf[0x20000];
} Ringbuf_t;

#endif

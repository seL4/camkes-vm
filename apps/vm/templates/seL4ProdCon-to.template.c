/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#include <sel4/sel4.h>
#include <camkes/dataport.h>
#include <lwip/udp.h>
#include <sync/sem-bare.h>
#include <string.h>

/* Actual dataport is emitted in the per-component template. */
/*- set p = Perspective(dataport=me.to_interface.name) -*/
char /*? p['dataport_symbol'] ?*/[ROUND_UP_UNSAFE(sizeof(/*? show(me.to_interface.type) ?*/), PAGE_SIZE_4K)]
    __attribute__((aligned(PAGE_SIZE_4K)))
    __attribute__((section("shared_/*? me.to_interface.name ?*/")))
    __attribute__((externally_visible));
volatile /*? show(me.to_interface.type) ?*/ * /*? me.to_interface.name ?*/ = (volatile /*? show(me.to_interface.type) ?*/ *) /*? p['dataport_symbol'] ?*/;

#define DATAPORT_BASE ( (uintptr_t)/*? p['dataport_symbol'] ?*/)
#define WORD_INDEX(x) ( (uint32_t*) (DATAPORT_BASE + (x) * sizeof(uint32_t)) )
#define DATA_INDEX(x) ( (void*) (DATAPORT_BASE + 2 * sizeof(uint32_t) + (x)) )

static unsigned int get_end() {
    return *WORD_INDEX(0);
}

static unsigned int get_start() {
    return *WORD_INDEX(1);
}

static void set_start(uint32_t start) {
    *WORD_INDEX(1) = start;
}

static unsigned int get_size() {
    return sizeof(/*? show(me.from_interface.type) ?*/) - 8;
}

static unsigned int buffer_space_used() {
    unsigned int end = get_end();
    unsigned int start = get_start();
    if (start <= end) {
        return end - start;
    } else {
        return get_size() - start + end;
    }
}

int /*? me.to_interface.name ?*/_dequeue(void *p, unsigned int len) {
    unsigned int used = buffer_space_used();
    if (used < len) {
        return 0;
    }
    memcpy(p, DATA_INDEX(get_start()), len);
    set_start(get_start() + len);
    return len;
}

int /*? me.to_interface.name ?*/_wrap_ptr(dataport_ptr_t *p, void *ptr) {
    return -1;
}

void * /*? me.to_interface.name ?*/_unwrap_ptr(dataport_ptr_t *p) {
    return NULL;
}


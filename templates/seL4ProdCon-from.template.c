/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#include <camkes/dataport.h>
#include <sel4/sel4.h>
#include <lwip/udp.h>
#include <string.h>

/*? macros.show_includes(me.from_instance.type.includes) ?*/

/* Actual dataport is emitted in the per-component template. */
/*- set p = Perspective(dataport=me.from_interface.name) -*/
char /*? p['dataport_symbol'] ?*/[ROUND_UP_UNSAFE(sizeof(/*? show(me.from_interface.type) ?*/), PAGE_SIZE_4K)]
    __attribute__((aligned(PAGE_SIZE_4K)))
    __attribute__((section("shared_/*? me.from_interface.name ?*/")))
    __attribute__((externally_visible));
volatile /*? show(me.from_interface.type) ?*/ * /*? me.from_interface.name ?*/ = (volatile /*? show(me.from_interface.type) ?*/ *) /*? p['dataport_symbol'] ?*/;

static int last_read_index = -1;

static void (*notify_function)() = NULL;

#define DATAPORT_BASE ( (uintptr_t)/*? p['dataport_symbol'] ?*/)
#define WORD_INDEX(x) ( (uint32_t*) (DATAPORT_BASE + (x) * sizeof(uint32_t)) )
#define DATA_INDEX(x) ( (void*) (DATAPORT_BASE + 2 * sizeof(uint32_t) + (x)) )

static unsigned int get_end() {
    return *WORD_INDEX(0);
}

static void set_end(uint32_t end) {
    *WORD_INDEX(0) = end;
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

static unsigned int buffer_space_remain() {
    unsigned int end = get_end();
    unsigned int start = get_start();
    if (start < end) {
        return get_size() - end + start - 1;
    } else {
        return start - end - 1;
    }
}

void /*? me.from_interface.name ?*/__init() {
    set_end(0);
    set_start(0);
}

void /*? me.from_interface.name ?*/_set_notify(void (*func)()) {
    notify_function = func;
}

int /*? me.from_interface.name ?*/_enqueue(void *p, unsigned int len) {
    unsigned int remain = buffer_space_remain();
    if (remain < len) {
        return -1;
    }
    unsigned int read_index = get_start();
    memcpy(DATA_INDEX(get_end()), p, len);
    set_end(get_end() + len);
    /* ensure modifications are seen */
    __sync_synchronize();
    /* see if we need to notify user */
    if (read_index != last_read_index && notify_function) {
        last_read_index = read_index;
        notify_function();
    }
    return 0;
}

int /*? me.from_interface.name ?*/_wrap_ptr(dataport_ptr_t *p, void *ptr) {
    /* should not be used */
    return -1;
}

void * /*? me.from_interface.name ?*/_unwrap_ptr(dataport_ptr_t *p) {
    /* should not be used */
    return NULL;
}


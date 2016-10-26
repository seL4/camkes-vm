/*
 * Copyright 2016, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(D61_BSD)
 */

/* A library for interacting with the cross vm dataport module */

#include <stdio.h>
#include <stdlib.h>
#include <stropts.h>

#include <sys/mman.h>
#include <sys/ioctl.h>

#include <emits_event/ioctl_commands.h>

int emits_event_emit(int fd) {
    return ioctl(fd, EMITS_EVENT_EMIT);
}

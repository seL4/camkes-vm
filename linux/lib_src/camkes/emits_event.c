/*
 * Copyright 2016, Data 61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(D61_GPL)
 */

/* A library for interacting with the cross vm dataport module */

#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/ioctl.h>

#include <emits_event/ioctl_commands.h>

int emits_event_emit(int fd) {
    return ioctl(fd, EMITS_EVENT_EMIT);
}

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#include "device.h"

#define DEVICE_NAME "emits_event"

int make_node(char *name, unsigned int major, unsigned int minor) {

    dev_t device = makedev(major, minor);

    printf("Creating emitting event node %s\n", name);

    return mknod(name, S_IFCHR, device);
}

int main(int argc, char *argv[]) {

    unsigned major = device_get_major(DEVICE_NAME);

    if (major < 0 || major == NO_MAJOR) {
        fprintf(stderr, "Failed to locate device %s.\n", DEVICE_NAME);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (make_node(argv[i], major, i) != 0) {
            fprintf(stderr, "Failed to create device node for %s.\n", DEVICE_NAME);
            return 1;
        }
    }

    return 0;
}

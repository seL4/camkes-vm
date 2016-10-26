/*
 * Copyright 2016, Data 61
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(D61_GPL)
 */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <sys/stat.h>

#include "emits_event.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s eventfile\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    int fd = open(filename, O_RDWR);

    int error = emits_event_emit(fd);

    if (error) {
        printf("Error: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

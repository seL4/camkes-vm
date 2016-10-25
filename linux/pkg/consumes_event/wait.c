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

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <poll.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define TIMEOUT_MS 20000

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s eventfile\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    int fd = open(filename, O_RDWR);

    struct pollfd fds[] = {{ .fd = fd, .events = POLLIN }};

    int result = poll(fds, 1, TIMEOUT_MS);

    if (result < 0) {
        printf("Error: %s\n", strerror(errno));
    } else if (result == 0) {
        printf("Timed out after %dms\n", TIMEOUT_MS);
    } else {
        printf("Back from waiting\n");
    }

    close(fd);

    return 0;
}

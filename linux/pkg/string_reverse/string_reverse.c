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
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <sys/stat.h>
#include <sys/mman.h>

#include "dataport.h"
#include "consumes_event.h"
#include "emits_event.h"

#define READY "/dev/camkes_reverse_ready"
#define DONE "/dev/camkes_reverse_done"
#define SRC "/dev/camkes_reverse_src"
#define DEST "/dev/camkes_reverse_dest"

#define BUFSIZE 8192
char buf[BUFSIZE];

int main(int argc, char *argv[]) {

    int ready = open(READY, O_RDWR);
    int done = open(DONE, O_RDWR);
    int src = open(SRC, O_RDWR);
    int dest = open(DEST, O_RDWR);

    volatile char *src_data = (volatile char*)dataport_mmap(src);
    assert(src_data != MAP_FAILED);

    volatile char *dest_data = (volatile char*)dataport_mmap(dest);
    assert(dest_data != MAP_FAILED);

    while (fgets(buf, BUFSIZE, stdin)) {
        int last_idx = strnlen(buf, BUFSIZE - 1) - 1;
        if (buf[last_idx] == '\n') {
            buf[last_idx] = '\0';
        }

        // copy buf to src_data
        int i;
        for (i = 0; buf[i]; i++) {
            src_data[i] = buf[i];
        }
        src_data[i] = '\0';

        // signal that we're ready for camkes component to reverse string
        int error = emits_event_emit(ready);
        assert(error == 0);

        // wait for camkes to signal that the string is reversed
        error = consumes_event_wait(done);
        assert(error > 0);

        // read the result out of dest_data
        strncpy(buf, (char*)dest_data, BUFSIZE);

        printf("%s\n", buf);
    }

    close(ready);
    close(done);
    close(src);
    close(dest);

    return 0;
}

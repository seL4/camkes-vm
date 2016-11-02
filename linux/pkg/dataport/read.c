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
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "dataport.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s file\n\n"
               "Reads the c string contents of a specified dataport file to stdout",
               argv[0]);
        return 1;
    }

    char *dataport_name = argv[1];

    int fd = open(dataport_name, O_RDWR);
    assert(fd >= 0);

    char *dataport = dataport_mmap(fd);

    printf("%s\n", dataport);

    close(fd);

    return 0;
}

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
#include "dataport.h"
#include "dataport_allocate.h"

#define DEVICE_NAME "dataport"

#define MAX_NUM_DATAPORTS 256

typedef struct dataport {
    char *file;
    size_t size;
    unsigned int minor;
    dev_t device;
    long long unsigned int paddr;
} dataport_t;

#define BUF_SIZE 256

/* Parses an array of strings {name0, size0, name1, size1, ...}
 * into an array of dataport_t */
int process_dataport_args(int argc, char *argv[],
                          dataport_t *dataports,
                          int max_num_dataports)
{

    int num_dataports = argc / 2;
    int count = 0;

    while (count < num_dataports && count < max_num_dataports) {
        char *filename = argv[count * 2];
        char *size_str = argv[count * 2 + 1];
        unsigned int size = strtoul(size_str, NULL, 0);

        dataports[count].file = filename;
        dataports[count].size = size;

        count++;
    }

    return count;
}

int make_node(dataport_t *dataport, unsigned int major, unsigned int minor) {
    dev_t device = makedev(major, minor);
    dataport->device = device;

    dataport->minor = minor;

    printf("Creating dataport node %s\n", dataport->file);

    return mknod(dataport->file, S_IFCHR, dataport->device);
}

void make_nodes(dataport_t *dataports, int num_dataports, unsigned int major) {
    unsigned int minor = 1;
    for (int i = 0; i < num_dataports; i++) {

        int error = make_node(&dataports[i], major, minor);
        assert(error == 0);

        minor++;
    }
}

void init_node(dataport_t *dataport) {

    FILE *f = fopen(dataport->file, "r+");
    assert(f);

    int fd = fileno(f);
    assert(fd >= 0);

    printf("Allocating %d bytes for %s\n", dataport->size, dataport->file);

    int error = dataport_allocate(fd, dataport->size);
    assert(error == 0);

    dataport->paddr = dataport_get_paddr(fd);
    assert(dataport->paddr >= 0);

    fclose(f);
}

void init_nodes(dataport_t *dataports, int num_dataports) {
    for (int i = 0; i < num_dataports; i++) {
        init_node(&dataports[i]);
    }
}

int main(int argc, char *argv[]) {

    if (argc == 1) {
        printf("Usage: %s name1 size1 ... nameN sizeN\n\n"
               "Creates device files for dataports in /dev with given names\n"
               "and sizes (in bytes).\n"
               "Dataport ids will be allocated in increasing order from left\n"
               "to right, starting from 1.\n"
               "Sizes must match those given in the corresponding camkes spec.\n"
               "This should only be run once per boot.\n", argv[0]);
        return 1;
    }

    dataport_t dataports[MAX_NUM_DATAPORTS];
    int num_dataports = process_dataport_args(argc - 1, argv + 1,
                                              dataports,
                                              MAX_NUM_DATAPORTS);

    unsigned major = device_get_major(DEVICE_NAME);
    assert(major != NO_MAJOR);

    printf("Device \"%s\" found with major number %d\n\n", DEVICE_NAME, major);

    make_nodes(dataports, num_dataports, major);
    init_nodes(dataports, num_dataports);

    return 0;
}

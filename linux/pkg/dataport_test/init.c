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
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>
#include <unistd.h>
#include <stropts.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#include "dataport.h"

#define DEVICE_NAME "dataport"

#define MAX_NUM_DATAPORTS 256

typedef struct dataport {
    char *name;
    char *file;
    size_t size;
    unsigned int minor;
    dev_t device;
    long long unsigned int paddr;
} dataport_t;

/* No device can have 0 as its major number */
#define NO_MAJOR 0
#define BUF_SIZE 256

unsigned int device_get_major(char *device) {

    char buf[BUF_SIZE];
    FILE *devices = fopen("/proc/devices", "r");

    while (fgets(buf, BUF_SIZE, devices)) {

        buf[strlen(buf) - 1] = '\0';

        int number = atoi(buf);
        if (number == NO_MAJOR) {
            // this line did not contain a major number
            continue;
        }

        strtok(buf, " ");
        char *current_device = strtok(NULL, " ");

        if (strcmp(current_device, device) == 0) {
            fclose(devices);
            return number;
        }
    }

    fclose(devices);

    return NO_MAJOR;
}

/* Parses an array of strings {name0, size0, name1, size1, ...}
 * into an array of dataport_t */
int process_dataport_args(int argc, char *argv[],
                          dataport_t *dataports,
                          int max_num_dataports)
{

    int num_dataports = argc / 2;
    int count = 0;

    while (count < num_dataports && count < max_num_dataports) {
        char *name = argv[count * 2];
        char *size_str = argv[count * 2 + 1];
        unsigned int size = strtoul(size_str, NULL, 0);

        dataports[count].name = name;
        dataports[count].size = size;

        count++;
    }

    return count;
}

int make_node(dataport_t *dataport, unsigned int major, unsigned int minor) {
    char buf[BUF_SIZE];
    snprintf(buf, BUF_SIZE, "/dev/%s", dataport->name);
    dataport->file = strdup(buf);

    dev_t device = makedev(major, minor);
    dataport->device = device;

    dataport->minor = minor;

    printf(" - %s\n\tfile: %s\n\tdevice: { maj: %d, min: %d }\n",
           dataport->name, dataport->file, major, minor);

    return mknod(dataport->file, S_IFCHR, dataport->device);
}

void make_nodes(dataport_t *dataports, int num_dataports, unsigned int major) {
    unsigned int minor = 1;
    printf("Making nodes:\n");
    for (int i = 0; i < num_dataports; i++) {

        int error = make_node(&dataports[i], major, minor);
        assert(error == 0);

        minor++;
    }
    printf("\n");
}

void init_node(dataport_t *dataport) {

    FILE *f = fopen(dataport->file, "r+");
    assert(f);

    int fd = fileno(f);
    assert(fd >= 0);

    int error = dataport_allocate(fd, dataport->size);
    assert(error == 0);

    dataport->paddr = dataport_get_paddr(fd);
    assert(dataport->paddr >= 0);

    fclose(f);

    printf(" - %s\n\tsize: 0x%x\n\tpaddr: 0x%llx\n",
           dataport->name, dataport->size, dataport->paddr);
}

void init_nodes(dataport_t *dataports, int num_dataports) {
    printf("Initialising nodes:\n");
    for (int i = 0; i < num_dataports; i++) {
        init_node(&dataports[i]);
    }
    printf("\n");
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

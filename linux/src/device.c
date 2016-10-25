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
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "device.h"

#define BUF_SIZE 256

#define DEVICE_LIST_FILE "/proc/devices"

unsigned int device_get_major(char *device) {

    char buf[BUF_SIZE];
    FILE *devices = fopen(DEVICE_LIST_FILE, "r");

    if (devices == NULL) {
        fprintf(stderr, "Couldn't open %s: %s\n", DEVICE_LIST_FILE, strerror(errno));
        return -1;
    }

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



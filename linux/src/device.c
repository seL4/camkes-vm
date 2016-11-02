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

        int last_idx = strnlen(buf, BUF_SIZE - 1) - 1;
        if (buf[last_idx] == '\n') {
            buf[last_idx] = '\0';
        }

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

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

#include <dataport/ioctl_commands.h>
#include "dataport.h"

#define BUF_SIZE 256
static char buf[BUF_SIZE];

int dataport_allocate(int fd, size_t size) {
    snprintf(buf, BUF_SIZE, "%u", size);
    return ioctl(fd, DATAPORT_ALLOCATE, buf);
}

long long unsigned int dataport_get_paddr(int fd) {
    int error = ioctl(fd, DATAPORT_GET_PADDR, buf);
    if (error < 0) {
        return error;
    }

    return strtoull(buf, NULL, 0);
}

ssize_t dataport_get_size(int fd) {
    int error = ioctl(fd, DATAPORT_GET_SIZE, buf);
    if (error < 0) {
        return error;
    }

    return strtoull(buf, NULL, 0);
}

void *dataport_mmap(int fd) {
    ssize_t size = dataport_get_size(fd);
    if (size < 0) {
        return MAP_FAILED;
    }

    return mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
}

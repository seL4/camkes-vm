/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#pragma once


typedef struct {
    void *ext_buf;
    int (*ext_open)(const char *name, int flags);
    ssize_t (*ext_read)(int fd, size_t size);
    int64_t (*ext_seek)(int fd, int64_t offset, int whence);
    int (*ext_close)(int fd);
} file_server_interface_t;


/*
 * This is a macro for defining an interface based on the camkes component
 * interface name.
 */
#define FILE_SERVER_INTERFACE(fs) \
    (file_server_interface_t) { \
        .ext_buf = fs##_buf, \
        .ext_open = fs##_open, \
        .ext_read = fs##_read, \
        .ext_seek = fs##_seek, \
        .ext_close = fs##_close, \
    }


void install_fileserver(file_server_interface_t fs_interface);

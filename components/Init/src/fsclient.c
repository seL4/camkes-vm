/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#include "vm.h"
#include <sel4/sel4.h>
#include <camkes.h>
#include "vmm/debug.h"

#include "fsclient.h"

#define BUF_SIZE PAGE_SIZE_4K
#define FS_ERR_NOFILE -1
#define FS_ERR_BLOCKED_PORT -2

static void *client_get_file_dataport(void);

/* Open up a file on the fileserver */
int fsclient_open(const char *name) {
    int fd = fs_lookup(name);
    if(fd < 0) {
        return -1;
    }
    return fd;
}

/* Read some data from a file on the fileserver */
int fsclient_read(void *dest, int fd, off_t offset, size_t size) {
    int ret_len;
    size_t copy_len = size;
    void *port = client_get_file_dataport();
    while(copy_len > 0) {
        do {
            ret_len = fs_read(fd, offset, copy_len);
        } while(ret_len == FS_ERR_BLOCKED_PORT);

        if(ret_len < 0) {
            DPRINTF(2, "Unexpected end of read with res %d", ret_len);
            return -1;
        }

        DPRINTF(4, "plat: got %d|%d from fs\n", ret_len, copy_len);
        memcpy(dest, port, ret_len);
        DPRINTF(4, "plat: memcpy done\n");
        offset += ret_len;
        dest += ret_len;
        copy_len -= ret_len;
    }

    fs_read_complete();
    return 0;
}

size_t fsclient_filelength(int fd) {
    return fs_filesize(fd);
}

void fsclient_close(int fd) {
    return;
}

static void *client_get_file_dataport(void) {
    return (void *)fs_mem;
}

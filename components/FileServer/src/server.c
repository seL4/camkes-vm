/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <cpio/cpio.h>

#include <muslcsys/io.h>
#include <sel4/sel4.h>

#include <camkes.h>

seL4_Word fs_ctrl_get_sender_id(void);
void *fs_ctrl_buf(seL4_Word);
size_t fs_ctrl_buf_size(seL4_Word);

typedef struct cpio_file_data_wrap {
    cpio_file_data_t data;
    seL4_Word client;
} cpio_file_data_wrap_t;

extern char _cpio_archive[];

void pre_init() {
    /* install the _cpio_archive */
    muslcsys_install_cpio_interface(_cpio_archive, cpio_get_file);
}

bool validate_client_fd(int fd, seL4_Word client) {
    if (!valid_fd(fd)) {
        ZF_LOGE("Client %zu attempted to use invalid fd %d", client, fd);
        return false;
    }
    muslcsys_fd_t *fd_struct = get_fd_struct(fd);
    if (fd_struct->filetype != FILE_TYPE_CPIO) {
        ZF_LOGE("Client %zu attempted to use fd %d of a non-open file", client, fd);
        return false;
    }
    cpio_file_data_wrap_t *data = (cpio_file_data_wrap_t*)fd_struct->data;
    if (data->client != client) {
        ZF_LOGE("Client %zu attempted to use fd %d that is for client %zu", client, fd, data->client);
        return false;
    }
    return true;
}

int fs_ctrl_open(const char *name, int flags) {
    /* try the open and return early if we get an error */
    int fd = open(name, flags);
    if (fd < 0) {
        return fd;
    }
    /* we make an assumption that we're still backed by the libsel4muslcsys
     * implementation and we can extend its book keeping slightly to track
     * the current client */
    muslcsys_fd_t *fd_struct = get_fd_struct(fd);
    assert(fd_struct);
    cpio_file_data_wrap_t *newdata = realloc(fd_struct->data, sizeof(cpio_file_data_wrap_t));
    if (!newdata) {
        ZF_LOGE("Failed to allocate space for additional file metadata");
        close(fd);
        return -ENOMEM;
    }
    newdata->client = fs_ctrl_get_sender_id();
    fd_struct->data = newdata;
    return fd;
}

int64_t fs_ctrl_seek(int fd, int64_t offset, int whence) {
    seL4_Word client = fs_ctrl_get_sender_id();
    if (!validate_client_fd(fd, client)) {
        return -1;
    }
    return lseek(fd, offset, whence);
}

ssize_t fs_ctrl_read(int fd, size_t size) {
    seL4_Word client = fs_ctrl_get_sender_id();
    if (!validate_client_fd(fd, client)) {
        return -1;
    }
    void *dataport = fs_ctrl_buf(client);
    assert(dataport);

    size_t max = fs_ctrl_buf_size(client);

    size = MIN(size, max);
    return read(fd, dataport, size);
}

int fs_ctrl_close(int fd) {
    seL4_Word client = fs_ctrl_get_sender_id();
    if (!validate_client_fd(fd, client)) {
        return -EBADF;
    }
    return close(fd);
}

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

#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/uio.h>
#include <utils/util.h>
#include <muslcsys/vsyscall.h>
#include "fsclient.h"

static void *ext_buf;
static int (*ext_open)(const char *name, int flags);
static ssize_t (*ext_read)(int fd, size_t size);
static int64_t (*ext_seek)(int fd, int64_t offset, int whence);
static int (*ext_close)(int fd);

static long fileserver_open(va_list ap) {
    const char *pathname = va_arg(ap, const char *);
    int flags = va_arg(ap, int);
    return ext_open(pathname, flags);
}

static long fileserver_openat(va_list ap) {
    int dirfd = va_arg(ap, int);
    const char *pathname = va_arg(ap, const char *);
    int flags = va_arg(ap, int);

    if (dirfd != AT_FDCWD) {
        ZF_LOGE("Openat only supports relative path to the current working directory\n");
        return -EINVAL;
    }
    return ext_open(pathname, flags);
}

static long fileserver_close(va_list ap) {
    int fd = va_arg(ap, int);
    return ext_close(fd);
}

static long fileserver_read(va_list ap) {
    int fd = va_arg(ap, int);
    void *buf = va_arg(ap, void*);
    size_t count = va_arg(ap, size_t);
    ssize_t total = 0;
    size_t remain = count;
    while (total < count) {
        ssize_t result = ext_read(fd, remain);
        if (result <= 0) {
            return total;
        }
        memcpy(buf + total, ext_buf, result);
        total += result;
        remain -= result;
    }
    return total;
}

static long fileserver_readv(va_list ap) {
    int fd = va_arg(ap, int);
    struct iovec *iov = va_arg(ap, struct iovec*);
    int iovcnt = va_arg(ap, int);
    ssize_t total = 0;
    int i;
    for (i = 0; i < iovcnt; i++) {
        long iov_offset = 0;
        while (iov_offset < iov[i].iov_len) {
            long read = ext_read(fd, iov[i].iov_len - iov_offset);
            if (read <= 0) {
                return total;
            }
            memcpy(iov[i].iov_base + iov_offset, ext_buf, read);
            iov_offset += read;
            total += read;
        }
    }
    return total;
}

static long fileserver_lseek(va_list ap) {
    int fd = va_arg(ap, int);
    off_t offset = va_arg(ap, off_t);
    int whence = va_arg(ap, int);

    return ext_seek(fd, offset, whence);
}

static long fileserver_llseek(va_list ap) {
    int fd = va_arg(ap, int);
    uint32_t offset_high = va_arg(ap, uint32_t);
    uint32_t offset_low = va_arg(ap, uint32_t);
    off_t *result = va_arg(ap, off_t*);
    int whence = va_arg(ap, int);

    *result = ext_seek(fd, (((uint64_t)offset_high) << 32) | offset_low, whence);
    return 0;
}

void install_fileserver(file_server_interface_t fs_interface) {
    ext_buf = fs_interface.ext_buf;
    ext_open = fs_interface.ext_open;
    ext_read = fs_interface.ext_read;
    ext_seek = fs_interface.ext_seek;
    ext_close = fs_interface.ext_close;
#ifdef __NR_open
    muslcsys_install_syscall(__NR_open, fileserver_open);
#endif
#ifdef __NR_openat
    muslcsys_install_syscall(__NR_openat, fileserver_openat);
#endif
    muslcsys_install_syscall(__NR_close, fileserver_close);
    muslcsys_install_syscall(__NR_read, fileserver_read);
    muslcsys_install_syscall(__NR_readv, fileserver_readv);
    muslcsys_install_syscall(__NR_lseek, fileserver_lseek);
#ifdef __NR__llseek
    muslcsys_install_syscall(__NR__llseek, fileserver_llseek);
#endif
}

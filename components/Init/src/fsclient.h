/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#ifndef VM_INIT_FSCLIENT_H
#define VM_INIT_FSCLIENT_H

#include <stdint.h>

int fsclient_open(const char *name);
int fsclient_read(void *dest, int fd, off_t offset, size_t size);
size_t fsclient_filelength(int fd);
void fsclient_close(int fd);

#endif

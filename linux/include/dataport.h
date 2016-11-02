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

#ifndef __DATAPORT_H_
#define __DATAPORT_H_

/* Interface to cross vm dataport library */

#include <stdlib.h>

long long unsigned int dataport_get_paddr(int fd);
ssize_t dataport_get_size(int fd);
void *dataport_mmap(int fd);

#endif

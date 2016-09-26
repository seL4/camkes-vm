/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#ifndef _VMM_UTILS
#define _VMM_UTILS

#include <stdint.h>

int num_of_vms(void);
int vm_number(void);
// int sleep_vm(void);
// int wakeup_vm(uint32_t vm);
int ioctl_intf(int cmd, int fd, void *data, unsigned size);

#endif

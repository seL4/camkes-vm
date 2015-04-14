/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#include <vmm_manager.h>
#include <libvchan.h>
#include <vmm_utils.h>


#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

int main() {
	printf("%d", vm_number());
	return 0;
}

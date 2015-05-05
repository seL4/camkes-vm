/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#ifndef VM_INIT_VCHAN_INIT_H
#define VM_INIT_VCHAN_INIT_H

#include <vmm/vmm.h>

void vchan_init(void);
void vchan_interrupt(vmm_t *vmm);

#endif

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
#include <linux/slab.h>
#include <linux/time.h>

#ifndef _ADT_LINUX_H
#define _ADT_LINUX_H

/* Linux headers have to be included only if __KERNEL__
 * is defined because cogent's C parser does not support
 * all gcc extensions in Linux headers.
 */
#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#endif  /* __KERNEL__ */

#endif /* _ADT_LINUX_H */

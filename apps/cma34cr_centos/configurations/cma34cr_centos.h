/*
 * Copyright 2017, Data 61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(D61_GPL)
 */

#define VM_NUM_GUESTS 1

/* All our guests use the same kernel image, rootfs and cmdline */
#define C162_KERNEL_IMAGE "bzimage"
#define C162_ROOTFS "rootfs.cpio"

#define PASSTHROUGH_SATA
//#define PASSTHROUGH_ETHERNET

#define VM_GUEST_CMDLINE "earlyprintk=ttyS0,115200 console=ttyS0,115200 i8042.nokbd=y i8042.nomux=y i8042.noaux=y io_delay=udelay noisapnp pci=nomsi debug root=/dev/mapper/centos-root ro rd.lvm.lv=centos/root rd.lvm.lv=centos/swap rdinit=/init 5"

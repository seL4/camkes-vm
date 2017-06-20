<!--
  Copyright 2017, Data61
  Commonwealth Scientific and Industrial Research Organisation (CSIRO)
  ABN 41 687 119 230.

  This software may be distributed and modified according to the terms of
  the GNU General Public License version 2. Note that NO WARRANTY is provided.
  See "LICENSE_GPLv2.txt" for details.

  @TAG(DATA61_GPL)
-->

CentOS Guest
============

This boots the guest kernel from the "bzimage" file in this directory using the
initrd from the "rootfs.cpio" file in this directory. Both files were generated
by building a linux kernel using CentOS's package build system.

To reproduce these files, follow the instructions here:
https://wiki.centos.org/HowTos/Custom_Kernel

The build config is in the centos-kernel-config file in this directory. When
building linux the CentOS way, the "bzimage" file here will be at
/boot/vmlinuz-3.10.0-514.el7.centos.YOUR_BUILDID.i686, and the rootfs.cpio
file will be at:
/boot/initramfs-3.10.0-514.el7.centos.YOUR_BUILDID.i686.img

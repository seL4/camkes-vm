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

The bzimage and roofs.cpio in this directory are from altarch CentOS-7. They are the
/boot/initramfs-3.10.0-693.5.2.el7.centos.plus.i686.img
/boot/vmlinuz-3.10.0-693.5.2.el7.centos.plus.i686
From an i386 CentOS-7 installation. You should be able to replace these with the
equivalent files from any i386 CentOS-7 installation
You may need to update the kernel command line in ../configurations/cma34cr_centos.h

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

Installation
------------

CentOS should be installed on the 32GiB flash drive of the cma34cr using the entire drive,
i.e. no dual booting etc. It must be an i386 CentOS 7 installation, images of which can be
found at http://mirror.centos.org/altarch/7/isos/i386/

Kernel Image
------------

The bzimage and roofs.cpio in this directory are from altarch CentOS-7. They are the
/boot/initramfs-3.10.0-693.5.2.el7.centos.plus.i686.img
/boot/vmlinuz-3.10.0-693.5.2.el7.centos.plus.i686
From an i386 CentOS-7 installation. You should be able to replace these with the
equivalent files from any i386 CentOS-7 installation, regardless of the precise version,
although you should not need to do this as this kernel/initramfs should be compatible with
any installation.

Kernel Command Line
-------------------

Assuming a default installation of CentOS the command line passed to the Linux kernel,
as described in ../configurations/cma34cr_centos.h should be correct. In the case where
you provided a custom name for your root partitions, or if something changes in later
CentOS versions, then this needs to be updated from the command line in the grub
configuration of your actual installation

Booting the VMM
--------------

After building you will have two files in the images/ directory, a kernel- and a capdl-.
These need to be booted with a multiboot compatible loader, with the capdl- image passed
as a boot module. This can be done with with a PXE based network loader, or by adding to
the grub menu of the installed CentOS.

Hardware Configuration
----------------------

The component specification assumes that the cma34cr is close to its default configuration,
with the exception that the SATA controller has been placed into legacy IDE mode instead of
its default AHCI mode.

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

CAmkES VM Linux
===============

This directory contains a linux image and root filesystem suitable for running
on top of the CAmkES VM (x86). The root filesystem can be modified to contain
additional kernel modules by adding them to the "modules" directory (as with the
vmm_maanger module), and running the build-rootfs script. This script takes care
of building all kernel modules and adding them to the root filesystem image.

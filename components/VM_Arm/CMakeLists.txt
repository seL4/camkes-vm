#
# Copyright 2018, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.8.2)

project(VM C)

set(configure_string "")

# ARM VM Configuration Options
config_option(
    VmOnDemandDeviceInstall
    VM_ONDEMAND_DEVICE_INSTALL
    "Allow the VMM to install arbitrary devices into the VM as they are accessed."
    DEFAULT
    ON
)

config_option(VmEmmc2NoDMA VM_EMMC2_NODMA "SD access control to prevent DMA \
    Installs SD card into the VM with read only access. Write access is \
    trapped and emulated to ensure that DMA is not used." DEFAULT OFF)

config_option(VmVUSB VM_VUSB "Para-virtual USB driver \
    Selects the device tree which enables the para virtual USB driver and \
    installs the device into the VM." DEPENDS "KernelPlatformExynos5410" DEFAULT OFF)

config_option(
    VmPCISupport
    VM_PCI_SUPPORT
    "Enable virtual pci device support"
    DEPENDS
    "KernelPlatformExynos5410 OR KernelPlatformExynos5422 OR KernelPlatformTx2 OR KernelPlatformQEMUArmVirt OR KernelPlatformOdroidc2 OR KernelPlatformZynqmp"
    DEFAULT
    OFF
)

config_option(
    VmVirtioNetArping
    VM_VIRTIO_NET_ARPING
    "Enable virtio net arping module"
    DEPENDS
    "KernelPlatformExynos5410 OR KernelPlatformExynos5422 OR KernelPlatformTx2 OR KernelPlatformQEMUArmVirt;VmPCISupport"
    DEFAULT
    OFF
)

config_option(
    VmVirtioNetVirtqueue
    VM_VIRTIO_NET_VIRTQUEUE
    "Enable virtio net virtqueue forwarding module"
    DEPENDS
    "KernelPlatformExynos5410 OR KernelPlatformExynos5422 OR KernelPlatformTx2 OR KernelPlatformQEMUArmVirt OR KernelPlatformZynqmp;VmPCISupport"
    DEFAULT
    OFF
)

config_option(
    VmVirtioConsole
    VM_VIRTIO_CON
    "Enable virtio console module"
    DEPENDS
    "KernelPlatformExynos5410 OR KernelPlatformExynos5422 OR KernelPlatformQEMUArmVirt;VmPCISupport"
    DEFAULT
    OFF
)

config_choice(
    VmRootfs
    VM_ROOTFS
    "Root file system selection \
    Selects the root device and partition which should be used for the root filesystem. \
    mmcblk0p2 -> Selects partition 2 of the on board eMMC as the root filesystem. \
    mmcblk1p2 -> Selects partition 2 of the on removable SD card as the root filesystem."
    "mmcblk0p2;VmRootfsMmcblk0p2;VM_ROOTFS_MMCBLK0P2"
    "mmcblk1p2;VmRootfsMmcblk1p2;VM_ROOTFS_MMCBLK1P2"
)

config_choice(
    VmTk1Rootfs
    VM_TK1_ROOTFS
    "TK1 boot mode selection. Selects whether to load root file system off emmc or ramdisk \
    initrd -> Selects initrd as the root filesystem. \
    mmcblk0p2 -> Selects partition 2 of the emmc card as the root filesystem."
    "initrd;VmTk1InitrdRootfs;VM_TK1_INITRD_ROOTFS"
    "mmcblk0p2;VmTk1EmmcRootfs;VM_TK1_EMMC_ROOTFS"
)

config_option(
    Tk1DeviceFwd
    TK1_DEVICE_FWD
    "Forward VM access to UART and Clock and reset controller. \
    When using the VM with other components that need access to the UART and clock and reset controller \
    devices, this option will cause the vm to forward the faults over a RPC connection"
    DEPENDS
    "KernelPlatformTK1"
    DEFAULT
    OFF
)

config_option(
    Tk1Insecure
    TK1_INSECURE
    "Insecure: Give all devices to VM component \
    The option when disabled only passes through UARTD, USB, EMMC, VGIC and a Clock and reset controller"
    DEPENDS
    "KernelPlatformTK1"
    DEFAULT
    OFF
)

config_option(
    VmInitRdFile
    VM_INITRD_FILE
    "Enables the option for the VM to open and load a seperate initrd file"
    DEFAULT
    OFF
)

config_option(
    VmDtbFile
    VM_DTB_FILE
    "Enables the option for the VM to open and load a seperate dtb file"
    DEFAULT
    OFF
)

config_option(
    VmVirtUart
    VM_VIRT_UART
    "Option for virtualizing console"
    DEFAULT
    OFF
    DEPENDS
    "KernelPlatformExynos5410 OR KernelPlatformExynos5422 OR KernelPlatformZynqmp"
)

add_config_library(arm_vm "${configure_string}")

DeclareCAmkESARMVM(VM)

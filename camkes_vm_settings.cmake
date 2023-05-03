#
# Copyright 2018, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.8.2)

# Kernel settings
set(KernelArch x86 CACHE STRING "" FORCE)
set(KernelPlatform pc99 CACHE STRING "" FORCE)
set(KernelVTX ON CACHE BOOL "" FORCE)
set(KernelRootCNodeSizeBits 20 CACHE STRING "" FORCE)
set(KernelIRQController IOAPIC CACHE STRING "" FORCE)
if(CAmkESVMGuestDMAIommu)
    set(KernelIOMMU ON CACHE BOOL "" FORCE)
endif()
# Release settings
set(RELEASE OFF CACHE BOOL "Performance optimized build")
# capDL settings
set(CapDLLoaderMaxObjects 900000 CACHE STRING "" FORCE)
# Our components will all define their own heaps if needed
# Otherwise we provide enough of a heap to initialise libc
set(CAmkESDefaultHeapSize 4096 CACHE STRING "" FORCE)
# We need to pre-process our specs
set(CAmkESCPP ON CACHE BOOL "" FORCE)

# Set release/verification configuration
ApplyCommonReleaseVerificationSettings(${RELEASE} FALSE)

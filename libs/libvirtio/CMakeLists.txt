#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.8.2)

project(libvirtio C)

add_compile_options(-std=gnu99)

set(sources src/virtio_net.c src/virtio_console.c)

add_library(virtioarm STATIC EXCLUDE_FROM_ALL ${sources})

target_include_directories(virtioarm PUBLIC include plat_include/${KernelPlatform})

target_link_libraries(virtioarm muslc sel4 sel4camkes sel4vm sel4vmmplatsupport sel4_autoconf)

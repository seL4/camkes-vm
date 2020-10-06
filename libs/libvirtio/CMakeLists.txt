#
# Copyright 2019, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(DATA61_BSD)
#

cmake_minimum_required(VERSION 3.8.2)

project(libvirtio C)

add_compile_options(-std=gnu99)

set(sources src/virtio_net.c src/virtio_console.c)

add_library(virtio STATIC EXCLUDE_FROM_ALL ${sources})

target_include_directories(virtio PUBLIC include plat_include/${KernelPlatform})

target_link_libraries(virtio muslc sel4 sel4camkes sel4vm sel4vmmplatsupport sel4_autoconf)

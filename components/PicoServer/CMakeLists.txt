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

project(PicoServer C)

DeclareCAmkESComponent(PicoServer SOURCES src/picoserver.c src/eth_interface.c src/picoserver_client.c
                                  INCLUDES include
                                  LIBS sel4utils picotcp sel4vspace ethdrivers PicoServer-client)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libPicoServer-client" ${CMAKE_CURRENT_BINARY_DIR}/libPicoServer-client)

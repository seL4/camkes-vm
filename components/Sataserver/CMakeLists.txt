#
# Copyright 2021, Data61, CSIRO (ABN 41 687 119 230)
# Copyright 2019, DornerWorks
#
# SPDX-License-Identifier: BSD-2-Clause
#
cmake_minimum_required(VERSION 3.8.2)

project(Sataserver C)

set(configure_string "")

config_option(
    SataserverUseAHCI
    SATASERVER_USE_AHCI
    "Enables the use of SATA AHCI interface for the Sataserver."
    DEFAULT
    OFF
)

add_config_library(sataserver "${configure_string}")

if(SataserverUseAHCI STREQUAL "ON")
    if(NOT "${CAmkESVMDestHardware}" STREQUAL "")
        CAmkESAddCPPInclude(
            "${CMAKE_CURRENT_LIST_DIR}/include/plat/${KernelPlatform}/${CAmkESVMDestHardware}"
        )
    else()
        message(
            FATAL_ERROR
                "Sataserver is using AHCI but CAmkESVMDestHardware is not set, cannot find hardware details"
        )
    endif()
else()
    # Add the empty sata definitions file
    CAmkESAddCPPInclude("${CMAKE_CURRENT_LIST_DIR}/include/plat/empty")
endif()

# Find header with hardware details, can find the empty
DeclareCAmkESComponent(
    Sataserver
    SOURCES
    src/sataserver.c
    INCLUDES
    include
    LIBS
    sel4allocman
    sel4platsupport
    satadrivers
    sataserver_Config
)

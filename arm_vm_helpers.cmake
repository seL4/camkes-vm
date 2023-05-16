#
# Copyright 2018, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.8.2)
set(ARM_VM_PROJECT_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "")

# Function appends a given list of CMake config variables as CAmkES CPP flags
# 'configure_string': The variable to append the CPP flags onto
# 'CONFIG_VARS': followed by the CMake variables to turn into CPP flags.
# Converts the CMake variables to upper case i.e. VmVchan -> VMVCHAN.
# If the CMake variable is a boolean it passes the flag with a 0 or 1 value
# e.g VMVCHAN=0. Intended to be used with CMake variables that contain boolean
# values.
function(AddCamkesCPPFlag configure_string)
    cmake_parse_arguments(PARSE_ARGV 1 ADD_CPP "" "" "CONFIG_VARS")
    foreach(configure_var IN LISTS ADD_CPP_CONFIG_VARS)
        # Convert the configuration variable name into uppercase
        string(TOUPPER ${configure_var} config_var_name)
        if(${${configure_var}})
            # If ON value, set the flag to "=1"
            list(APPEND ${configure_string} "-D${config_var_name}=1")
        else()
            # If OFF value, set the flag to "=0"
            list(APPEND ${configure_string} "-D${config_var_name}=0")
        endif()
    endforeach()
    # Update the configure_string value
    set(${configure_string} "${${configure_string}}" PARENT_SCOPE)
endfunction(AddCamkesCPPFlag)

function(DeclareCAmkESARMVM init_component)
    cmake_parse_arguments(
        PARSE_ARGV
        1
        VM_COMP
        ""
        ""
        "EXTRA_SOURCES;EXTRA_INCLUDES;EXTRA_LIBS;EXTRA_C_FLAGS;EXTRA_LD_FLAGS"
    )

    set(
        vm_src
        ${ARM_VM_PROJECT_DIR}/components/VM_Arm/src/main.c
        ${ARM_VM_PROJECT_DIR}/components/VM_Arm/src/fdt_manipulation.c
        ${ARM_VM_PROJECT_DIR}/components/VM_Arm/src/crossvm.c
        ${ARM_VM_PROJECT_DIR}/components/VM_Arm/src/modules/map_frame_hack.c
        ${ARM_VM_PROJECT_DIR}/components/VM_Arm/src/modules/init_ram.c
    )

    if(VmVirtUart)
        list(APPEND vm_src ${ARM_VM_PROJECT_DIR}/components/VM_Arm/src/modules/vuart_init.c)
    endif()

    if(Tk1DeviceFwd)
        list(
            APPEND vm_src ${ARM_VM_PROJECT_DIR}/components/VM_Arm/src/modules/plat/tk1/device_fwd.c
        )
    endif()

    # A module that is expected to exist for each platform but not required.
    # It should provide basic device intialisation required for every vm configuratoin
    set(
        platform_module
        ${ARM_VM_PROJECT_DIR}/components/VM_Arm/src/modules/plat/${KernelPlatform}/init.c
    )
    if(EXISTS ${platform_module})
        list(APPEND vm_src ${platform_module})
    endif()

    # Append virtio net sources if the virtio net config is enabled
    if(VmVirtioNetArping)
        list(APPEND vm_src ${ARM_VM_PROJECT_DIR}/components/VM_Arm/src/modules/virtio_net_arping.c)
    endif()

    if(VmVirtioNetVirtqueue)
        list(
            APPEND vm_src ${ARM_VM_PROJECT_DIR}/components/VM_Arm/src/modules/virtio_net_virtqueue.c
        )
    endif()

    if(VmVirtioConsole)
        list(APPEND vm_src ${ARM_VM_PROJECT_DIR}/components/VM_Arm/src/modules/virtio_con.c)
    endif()

    if(KernelPlatformExynos5410)
        list(
            APPEND vm_src ${ARM_VM_PROJECT_DIR}/components/VM_Arm/src/modules/plat/exynos5410/init.c
        )
        set(vm_plat_include "${ARM_VM_PROJECT_DIR}/components/VM_Arm/plat_include/exynos5410")
    elseif(KernelPlatformExynos5422)
        set(vm_plat_include "${ARM_VM_PROJECT_DIR}/components/VM_Arm/plat_include/exynos5422")
    elseif(KernelPlatformZynqmpUltra96v2)
        set(vm_plat_include "${ARM_VM_PROJECT_DIR}/components/VM_Arm/plat_include/ultra96v2")
    else()
        set(
            vm_plat_include
            "${ARM_VM_PROJECT_DIR}/components/VM_Arm/plat_include/${KernelPlatform}"
        )
    endif()
    # Declare the CAmkES VM component
    DeclareCAmkESComponent(
        ${init_component}
        SOURCES
        ${vm_src}
        ${VM_COMP_EXTRA_SOURCES}
        INCLUDES
        ${ARM_VM_PROJECT_DIR}/components/VM_Arm/include
        ${vm_plat_include}
        ${VM_COMP_EXTRA_INCLUDES}
        LIBS
        sel4allocman
        elf
        sel4simple
        sel4simple-default
        cpio
        sel4vm
        sel4dma
        FileServer-client
        sel4vmmplatsupport
        arm_vm_Config
        sel4_autoconf
        sel4muslcsys_Config
        fdt
        fdtgen
        ${VM_COMP_EXTRA_LIBS}
        LD_FLAGS
        ${VM_COMP_EXTRA_LD_FLAGS}
        C_FLAGS
        ${VM_COMP_EXTRA_C_FLAGS}
        TEMPLATE_SOURCES
        seL4AllocatorMempool.template.c
        seL4VMParameters.template.c
        TEMPLATE_HEADERS
        seL4AllocatorMempool.template.h
        seL4VMParameters.template.h
    )

    if(VmVirtioNetArping OR VmVirtioNetVirtqueue OR VmVirtioConsole)
        DeclareCAmkESComponent(${init_component} LIBS virtioarm vswitch)
    endif()

    # Append the USB driver library if building for exynos
    if("${KernelARMPlatform}" STREQUAL "exynos5410")
        DeclareCAmkESComponent(${init_component} LIBS usbdrivers)
    endif()

    if("${KernelArmSMMU}")
        DeclareCAmkESComponent(${init_component} TEMPLATE_SOURCES seL4SMMUV2.template.c)
    endif()

endfunction()

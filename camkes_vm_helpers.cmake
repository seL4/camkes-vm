#
# Copyright 2018, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.8.2)

set(VM_PROJECT_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "")
if(NOT TARGET vm_fserver_config)
    add_custom_target(vm_fserver_config)
endif()

# Function for declaring a CAmkESVM. This is called for each Init component in the applications
# camkes config.
# init_component: is the name of CamkESVM Init component described in the .camkes config
# In addition the user can pass in extra compilation sources, includes, libs and flags through
# the SOURCES, INCLUDES, LIBS, LD_FLAGS and C_FLAGS arguments.
function(DeclareCAmkESVM init_component)
    cmake_parse_arguments(
        PARSE_ARGV
        1
        VM_COMP
        ""
        ""
        "EXTRA_SOURCES;EXTRA_INCLUDES;EXTRA_LIBS;EXTRA_C_FLAGS;EXTRA_LD_FLAGS"
    )
    # Retrieve sources for Init component
    file(GLOB base_sources ${VM_PROJECT_DIR}/components/Init/src/*.c)
    # Declare an Init component for CAmkESVM
    DeclareCAmkESComponent(
        ${init_component}
        SOURCES
        ${base_sources}
        ${VM_COMP_EXTRA_SOURCES}
        INCLUDES
        ${VM_PROJECT_DIR}/components/Init/src
        ${VM_PROJECT_DIR}/components/Init/include
        ${VM_PROJECT_DIR}/components/VM/configurations
        ${VM_COMP_EXTRA_INCLUDES}
        LIBS
        sel4allocman
        sel4vm
        sel4vmmplatsupport
        sel4_autoconf
        camkes_vmm_Config
        virtqueue
        vswitch
        FileServer-client
        ${VM_COMP_EXTRA_LIBS}
        LD_FLAGS
        ${VM_COMP_EXTRA_LD_FLAGS}
        C_FLAGS
        ${VM_COMP_EXTRA_C_FLAGS}
        TEMPLATE_SOURCES
        seL4ExtraRAM.template.c
        seL4ExcludeGuestPAddr.template.c
        seL4InitConnection.template.c
        seL4VMIOPorts.template.c
        seL4GuestMaps.template.c
        seL4VMIRQs.template.c
        seL4VMPCIDevices.template.c
    )
endfunction(DeclareCAmkESVM)

# Function defines a CAmkESVMFileServer using the declared fileserver images
# and fileserver dependencies. These images are placed into a CPIO archive.
function(DefineCAmkESVMFileServer)
    # Retrieve defined kernel images, rootfs images and extraction dependencies
    get_target_property(fileserver_images vm_fserver_config FILES)
    get_target_property(fileserver_deps vm_fserver_config DEPS)
    # Build CPIO archive given the defined kernel and rootfs images
    include(cpio)
    MakeCPIO(file_server_archive.o "${fileserver_images}" DEPENDS "${fileserver_deps}")
    add_library(fileserver_cpio STATIC EXCLUDE_FROM_ALL file_server_archive.o)
    set_property(TARGET fileserver_cpio PROPERTY LINKER_LANGUAGE C)
    ExtendCAmkESComponentInstance(FileServer fserv LIBS fileserver_cpio)
endfunction(DefineCAmkESVMFileServer)

# Function for declaring the CAmkESVM root server. Taking the camkes application
# config file we declare a CAmkES Root server and the VM File Server. It is
# expected the caller has declared the file server images before using this
# function.
# camkes_config: The applications .camkes file
# In addition the user can pass in extra CPP compilation includes and flags through
# the CPP_INCLUDES and CPP_FLAGS arguments.
function(DeclareCAmkESVMRootServer camkes_config)
    cmake_parse_arguments(PARSE_ARGV 1 CAMKES_ROOT_VM "" "" "CPP_INCLUDES;CPP_FLAGS")
    # Initialise the CAmKES VM fileserver
    DefineCAmkESVMFileServer()
    get_absolute_source_or_binary(config_file "${camkes_config}")
    # Declare CAmkES root server
    DeclareCAmkESRootserver(
        ${config_file}
        CPP_FLAGS
        ${CAMKES_ROOT_VM_CPP_FLAGS}
        CPP_INCLUDES
        "${VM_PROJECT_DIR}/components/VM"
        ${CAMKES_ROOT_VM_CPP_INCLUDES}
    )
endfunction(DeclareCAmkESVMRootServer)

# Function for adding a file/image to the vm file server.
# filename_pref: The name the caller wishes to use to reference the file in the CPIO archive. This
# corresponds with the name set in the 'kernel_image' camkes variable for a given instance vm.
# file_dest: The location of the file/image the caller is adding to the file server
# DEPENDS: Any additional dependencies for the file/image the caller is adding to the
# file server
function(AddToFileServer filename_pref file_dest)
    get_filename_component(basename ${file_dest} NAME)
    # Get any existing dependencies when adding the image into the file server archive
    cmake_parse_arguments(PARSE_ARGV 2 CAMKES_FILESERVER "" "" "DEPENDS")
    if(NOT "${CAMKES_FILESERVER_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "Unknown arguments to AddToFileServer")
    endif()
    # Create a copy of the file in the binary directory to the callers
    # preferred name
    add_custom_command(
        OUTPUT file_server/${filename_pref}
        COMMAND
            ${CMAKE_COMMAND} -E copy "${file_dest}"
            "${CMAKE_CURRENT_BINARY_DIR}/file_server/${filename_pref}"
        VERBATIM
        DEPENDS ${file_dest} ${CAMKES_FILESERVER_DEPENDS}
    )
    #Create custom target for copy command
    add_custom_target(
        copy_${filename_pref}
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/file_server/${filename_pref}"
    )
    # Store the rootfs file location. Used when building the CPIO at a later stage
    set_property(
        TARGET vm_fserver_config
        APPEND
        PROPERTY FILES "${CMAKE_CURRENT_BINARY_DIR}/file_server/${filename_pref}"
    )
    # Append soft link dependency
    set_property(TARGET vm_fserver_config APPEND PROPERTY DEPS "copy_${filename_pref}")
endfunction(AddToFileServer)

# Function for decompressing/extracting a vmlinux file from a given kernel image
# decompress_target: The target name the caller wishes to use to generate the decompressed kernel
# image
# decompressed_kernel_image: caller variable which is set with the decompressed kernel image location
# compressed_kernel_image: The location of the compressed kernel image
# DEPENDS: Any additional dependencies for the compressed kernel image
function(DecompressLinuxKernel decompress_target decompressed_kernel_image compressed_kernel_image)
    # Get any existing dependencies for decompressing linux kernel
    cmake_parse_arguments(PARSE_ARGV 3 DECOMPRESS_KERNEL "" "" "DEPENDS")
    if(NOT "${DECOMPRESS_KERNEL_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "Unknown arguments to DecompressLinuxKernel")
    endif()
    # Retrieve filename from kernel path
    get_filename_component(kernel_basename ${compressed_kernel_image} NAME)
    # Extract vmlinux from bzimage
    add_custom_command(
        OUTPUT decomp/${kernel_basename}
        COMMAND
            bash -c
            "${VM_PROJECT_DIR}/tools/elf/extract-vmlinux ${compressed_kernel_image} > decomp/${kernel_basename}"
        VERBATIM
        DEPENDS ${compressed_kernel_image} ${DECOMPRESS_KERNEL_DEPENDS}
    )
    # Create custom target for extraction
    add_custom_target(
        ${decompress_target}
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/decomp/${kernel_basename}"
    )
    # Set parameter to tell the caller location of decompressed kernel image
    set(
        ${decompressed_kernel_image} ${CMAKE_CURRENT_BINARY_DIR}/decomp/${kernel_basename}
        PARENT_SCOPE
    )
endfunction(DecompressLinuxKernel)

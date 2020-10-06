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

set(CAMKES_ARM_VM_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE STRING "")
set(CAMKES_ARM_VM_HELPERS_PATH "${CMAKE_CURRENT_LIST_DIR}/arm_vm_helpers.cmake" CACHE STRING "")
mark_as_advanced(CAMKES_ARM_VM_DIR CAMKES_ARM_VM_HELPERS_PATH)

macro(camkes_arm_vm_import_project)
    include(${CAMKES_VM_HELPERS_PATH})
    include(${CAMKES_ARM_VM_HELPERS_PATH})
    # Common build definitions
    CAmkESAddImportPath(${CAMKES_ARM_VM_DIR}/components camkes-arm-vm/components)
    CAmkESAddImportPath(${CAMKES_ARM_VM_DIR}/interfaces camkes-arm-vm/interfaces)
    CAmkESAddTemplatesPath(${CAMKES_ARM_VM_DIR}/templates camkes-arm-vm/templates)
    DeclareCAmkESConnector(
        seL4VMDTBPassthrough
        FROM
        seL4VMDTBPassthrough-from.template.c
        TO
        seL4VMDTBPassthrough-to.template.c
    )

    # Add libraries
    add_subdirectory(${CAMKES_ARM_VM_DIR}/libs/libvirtio camkes-arm-vm/libs/libvirtio)

    # VM components
    add_subdirectory(${CAMKES_ARM_VM_DIR}/components/VM_Arm camkes-arm-vm/components/VM)

endmacro()

macro(camkes_arm_vm_setup_arm_vm_environment)

    find_package(camkes-tool REQUIRED)
    find_package(global-components REQUIRED)
    find_package(camkes-vm REQUIRED)
    find_package(camkes-vm-images REQUIRED)
    find_package(sel4_projects_libs REQUIRED)
    camkes_tool_setup_camkes_build_environment()
    sel4_projects_libs_import_libraries()
    global_components_import_project()

    camkes_arm_vm_import_project()
endmacro()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    camkes-arm-vm
    DEFAULT_MSG
    CAMKES_ARM_VM_DIR
    CAMKES_ARM_VM_HELPERS_PATH
)

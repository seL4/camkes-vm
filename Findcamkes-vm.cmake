#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

set(CAMKES_VM_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE STRING "")
set(CAMKES_VM_HELPERS_PATH "${CMAKE_CURRENT_LIST_DIR}/camkes_vm_helpers.cmake" CACHE STRING "")
set(CAMKES_VM_SETTINGS_PATH "${CMAKE_CURRENT_LIST_DIR}/camkes_vm_settings.cmake" CACHE STRING "")
mark_as_advanced(CAMKES_VM_DIR CAMKES_VM_HELPERS_PATH CAMKES_VM_SETTINGS_PATH)

macro(camkes_x86_vm_setup_x86_vm_environment)

    find_package(camkes-tool REQUIRED)
    find_package(global-components REQUIRED)
    find_package(camkes-vm REQUIRED)
    find_package(sel4_projects_libs REQUIRED)
    camkes_tool_setup_camkes_build_environment()
    global_components_import_project()
    sel4_projects_libs_import_libraries()
    # Add project
    add_subdirectory(${CAMKES_VM_DIR} camkes-vm)

endmacro()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    camkes-vm
    DEFAULT_MSG
    CAMKES_VM_DIR
    CAMKES_VM_HELPERS_PATH
    CAMKES_VM_SETTINGS_PATH
)

#
# Copyright 2017, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(DATA61_GPL)
#

TARGETS := $(notdir ${SOURCE_DIR}).cdl
# Subdirectory containing extra templates.
TEMPLATES += templates global-templates

# Construct configuration name
VM_CONFIG := $(patsubst "%",%, ${CONFIG_APP_CAMKES_VM_CONFIG})

-include ${SOURCE_DIR}/configurations/${VM_CONFIG}.config.mk

# Declare this if the configuration didn't
VM_NUM_VM ?= $(patsubst "%",%, ${CONFIG_APP_CAMKES_VM_NUM_VM})
ADL ?= vm.camkes

include TimeServer/TimeServer.mk
include SerialServer/SerialServer.mk
include RTC/RTC.mk
include Ethdriver/Ethdriver.mk
include UDPServer/UDPServer.mk
include HelloWorld/HelloWorld.mk
include Echo/Echo.mk
include Vchan/Vchan.mk
include PCIConfigIO/PCIConfigIO.mk
include FileServer/FileServer.mk
include Init/Init.mk
include StringReverse/StringReverse.mk
include Firewall/Firewall.mk

# We want to run C99
CFLAGS += -std=gnu99 "-DCAMKES_VM_CONFIG=${VM_CONFIG}" -I${SOURCE_DIR}/configurations -I${SOURCE_DIR}/../../components/VM/configurations

CAMKES_FLAGS += --cpp-flag=-I${SOURCE_DIR}/configurations --cpp-flag=-I${SOURCE_DIR}/../../components/VM --cpp-flag="-DCAMKES_VM_CONFIG=${VM_CONFIG}" --cache-dir=${PWD}/build/camkes_cache

# Include the per configuration Makefile. This should include the common
# camkes.mk where appropriate
include ${SOURCE_DIR}/configurations/${VM_CONFIG}.mk

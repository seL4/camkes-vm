#
# Copyright 2014, NICTA
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(NICTA_GPL)
#

TARGETS := $(notdir ${SOURCE_DIR}).cdl
ADL := vm.camkes

# Subdirectory containing extra templates.
TEMPLATES := templates

# Construct configuration name
VM_CONFIG := $(patsubst "%",%, ${CONFIG_APP_CAMKES_VM_CONFIG})
VM_NUM_VM := $(patsubst "%",%, ${CONFIG_APP_CAMKES_VM_NUM_VM})

include TimeServer/TimeServer.mk
include SerialServer/SerialServer.mk
include RTC/RTC.mk
include Ethdriver/Ethdriver.mk
include UDPServer/UDPServer.mk
include HelloWorld/HelloWorld.mk
include Echo/Echo.mk
include Vchan/Vchan.mk
include PCIConfigIO/PCIConfigIO.mk
include Init/Init.mk

# We want to run C99
CFLAGS += -std=gnu99 "-DCAMKES_VM_CONFIG=${VM_CONFIG}" "-DBOOST_PP_VARIADICS" -I${SOURCE_DIR}/configurations -I${SOURCE_DIR}/../../components/VM/configurations

CAMKES_FLAGS += --cpp-flag="-DBOOST_PP_VARIADICS" --cpp-flag=-I${SOURCE_DIR} --cpp-flag=-I${SOURCE_DIR}/configurations --cpp-flag=-I${SOURCE_DIR}/../../components/VM --cpp-flag="-DCAMKES_VM_CONFIG=${VM_CONFIG}" --cache-dir=${PWD}/build/camkes_cache

include ${PWD}/tools/camkes/camkes.mk

# Include the per configuration Makefile
include ${SOURCE_DIR}/configurations/${VM_CONFIG}.makefile

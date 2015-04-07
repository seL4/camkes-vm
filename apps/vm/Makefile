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

#Source files required to build the target
Init_CFILES   := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/Init/src/*.c))

# Libraries required to build the target

#LIBS-y = muslc sel4 sel4platsupport sel4muslcsys sel4vka sel4allocman elf \
#sel4vspace sel4utils utils pci cpio sel4vmm sel4simple sel4simple-stable
#LIBS = $(LIBS-y)

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

# We want to run C99
CFLAGS += -std=gnu99 "-DCAMKES_VM_CONFIG=${VM_CONFIG}" "-DBOOST_PP_VARIADICS" -I${SOURCE_DIR}/configurations

Init_OFILES := archive.o

Init_HFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/configurations/*.h))
Init_HFILES += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/Init/src/*.h))
Init_LIBS := sel4camkes sel4vmm sel4utils cpio elf sel4vka sel4allocman sel4vspace pci sel4simple sel4simple-stable sel4platsupport ethdrivers platsupport

VM_LIST := $(shell seq 0 `expr ${VM_NUM_VM} - 1`)
$(foreach var, $(VM_LIST), \
    $(eval Init$(var)_HFILES := $(value Init_HFILES)); \
    $(eval Init$(var)_CFILES := $(value Init_CFILES)); \
    $(eval Init$(var)_OFILES := $(value Init_OFILES)); \
    $(eval Init$(var)_LIBS := $(value Init_LIBS)); \
)

SerialEmulator_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/SerialEmulator/src/*.c))


CAMKES_FLAGS += --cpp-flag="-DBOOST_PP_VARIADICS" --cpp-flag=-I${SOURCE_DIR} --cpp-flag="-DCAMKES_VM_CONFIG=${VM_CONFIG}" --cache-dir=${PWD}/build/camkes_cache

include ${PWD}/tools/camkes/camkes.mk

# Include the per configuration Makefile
include ${SOURCE_DIR}/configurations/${VM_CONFIG}.makefile

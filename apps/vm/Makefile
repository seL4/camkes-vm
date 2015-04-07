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

# We want to run C99
CFLAGS += -std=gnu99 "-DCAMKES_VM_CONFIG=${VM_CONFIG}" "-DBOOST_PP_VARIADICS" -I${SOURCE_DIR}/configurations

Init_OFILES := archive.o

Init_HFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/configurations/*.h))
Init_HFILES += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/Init/src/*.h))
Init_LIBS := sel4camkes sel4vmm sel4utils cpio elf sel4vka sel4allocman sel4vspace pci sel4simple sel4simple-stable sel4platsupport ethdrivers platsupport
Init_HFILES += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/Vchan/include/*.h))

VM_LIST := $(shell seq 0 `expr ${VM_NUM_VM} - 1`)
$(foreach var, $(VM_LIST), \
    $(eval Init$(var)_HFILES := $(value Init_HFILES)); \
    $(eval Init$(var)_CFILES := $(value Init_CFILES)); \
    $(eval Init$(var)_OFILES := $(value Init_OFILES)); \
    $(eval Init$(var)_LIBS := $(value Init_LIBS)); \
)

Ethdriver_HFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/configurations/*.h))
Ethdriver_HFILES += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/configurations/c162/*.h))
Ethdriver_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/Ethdriver/src/*.c))
Ethdriver_LIBS := sel4camkes sel4utils sel4vka sel4allocman sel4vspace sel4simple sel4platsupport ethdrivers

Echo_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/Echo/src/*.c))

UDPServer_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/UDPServer/src/*.c))
UDPServer_LIBS := sel4camkes ethdrivers lwip

SerialEmulator_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/SerialEmulator/src/*.c))

PCIConfigIO_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/PCIConfigIO/src/*.c))

RTC_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/RTC/src/*.c))
RTC_LIBS := platsupport


Vchan_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/Vchan/src/*.c))
Vchan_HFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/configurations/*.h))
Vchan_HFILES += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/Vchan/include/*.h))
Vchan_LIBS := sel4camkes sel4vmm sel4utils sel4vka sel4allocman sel4vspace sel4simple sel4simple-stable sel4platsupport

HelloWorld_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/HelloWorld/src/*.c))
HelloWorld_HFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/configurations/*.h))
HelloWorld_HFILES += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/HelloWorld/include/*.h))
HelloWorld_LIBS := sel4camkes sel4vmm sel4utils sel4vka sel4allocman sel4vspace sel4simple sel4simple-stable
CAMKES_FLAGS += --cpp-flag="-DBOOST_PP_VARIADICS" --cpp-flag=-I${SOURCE_DIR} --cpp-flag="-DCAMKES_VM_CONFIG=${VM_CONFIG}" --cache-dir=${PWD}/build/camkes_cache

include ${PWD}/tools/camkes/camkes.mk

# Include the per configuration Makefile
include ${SOURCE_DIR}/configurations/${VM_CONFIG}.makefile

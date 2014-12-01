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

#Source files required to build the target 
Init_CFILES   := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/Init/src/*.c))

Init_OFILES := archive.o

# Libraries required to build the target

#LIBS-y = muslc sel4 sel4platsupport sel4muslcsys sel4vka sel4allocman elf \
#sel4vspace sel4utils utils pci cpio sel4vmm sel4simple sel4simple-stable
#LIBS = $(LIBS-y)

# Construct configuration name
VM_CONFIG := $(patsubst "%",%, ${CONFIG_APP_CAMKES_VM_CONFIG})

# We want to run C99
CFLAGS += -std=gnu99 "-DCAMKES_VM_CONFIG=${VM_CONFIG}" "-DBOOST_PP_VARIADICS"

Init_HFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/configurations/*.h))
Init_HFILES += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/Init/src/*.h))
Init_LIBS := sel4camkes sel4vmm sel4utils cpio elf sel4vka sel4allocman sel4vspace pci cpio sel4simple sel4simple-stable sel4platsupport ethdrivers

Ethdriver_HFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/configurations/*.h))
Ethdriver_HFILES += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/configurations/c162/*.h))
Ethdriver_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/Ethdriver/src/*.c))
Ethdriver_LIBS := sel4camkes sel4utils sel4vka sel4allocman sel4vspace sel4simple sel4platsupport ethdrivers

Echo_HFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/configurations/*.h))
Echo_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/Echo/src/*.c))

PICEmulator_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/PICEmulator/src/*.c))
PITEmulator_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/PITEmulator/src/*.c))
RTCEmulator_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/RTCEmulator/src/*.c))
SerialEmulator_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/SerialEmulator/src/*.c))

SerialServer_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/SerialServer/src/*.c))
SerialServer_HFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/configurations/*.h))
PCIConfigIO_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/PCIConfigIO/src/*.c))

RTC_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/RTC/src/*.c))
RTC_LIBS := platsupport

TimeServer_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/TimeServer/src/*.c))
TimeServer_HFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/configurations/*.h))
TimeServer_LIBS := platsupport

CAMKES_FLAGS += --cpp-flag="-DBOOST_PP_VARIADICS" --cpp-flag=-I${SOURCE_DIR} --cpp-flag="-DCAMKES_VM_CONFIG=${VM_CONFIG}" --cache-dir=${PWD}/build/camkes_cache

include ${PWD}/tools/camkes/camkes.mk

# Include the per configuration Makefile
include ${SOURCE_DIR}/configurations/${VM_CONFIG}.makefile

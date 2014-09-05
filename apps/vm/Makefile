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

# We want to run C99
CFLAGS += -std=gnu99

Init_LIBS := sel4camkes sel4vmm sel4utils cpio elf sel4vka sel4allocman sel4vspace pci cpio sel4simple sel4simple-stable sel4platsupport

PICEmulator_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/PICEmulator/src/*.c))
PITEmulator_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/PITEmulator/src/*.c))
RTCEmulator_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/RTCEmulator/src/*.c))
SerialEmulator_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/SerialEmulator/src/*.c))

SerialServer_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/SerialServer/src/*.c))
PCIConfigIO_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/PCIConfigIO/src/*.c))

RTC_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/RTC/src/*.c))
RTC_LIBS := platsupport

TimeServer_CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/components/TimeServer/src/*.c))
TimeServer_LIBS := platsupport

include ${PWD}/tools/camkes/camkes.mk

KERNEL1_FILENAME=$(patsubst "%",%,$(CONFIG_APP_CAMKES_VM_MULTIBOOT_KERNEL1))

ARCHIVE1_DEPS := ${STAGE_DIR}/${KERNEL1_FILENAME}

ifneq (${CONFIG_APP_CAMKES_VM_COMPRESSED_KERNEL1},y)
${STAGE_DIR}/${KERNEL1_FILENAME}: $(SOURCE_DIR)/linux/${KERNEL1_FILENAME}
	@echo "[CP] $@"
	$(Q)mkdir -p $(@D)
	@cp $< $@

${STAGE_DIR}/${KERNEL1_FILENAME}.relocs: ${STAGE_DIR}/${KERNEL1_FILENAME}
	@echo "[RELOCS] $@"
	@${PWD}/tools/elf/relocs $< > \
		${STAGE_DIR}/${KERNEL1_FILENAME}.relocs
ARCHIVE1_DEPS += ${STAGE_DIR}/${KERNEL1_FILENAME}.relocs
else
${STAGE_DIR}/${KERNEL1_FILENAME}: $(SOURCE_DIR)/linux/${KERNEL1_FILENAME}
	@echo "[EXTRACT-VMLINUX] $@"
	$(Q)mkdir -p $(@D)
	${PWD}/tools/elf/extract-vmlinux $< > $@
endif # CONFIG_APP_CAMKES_VM_COMPRESSED_KERNEL

ifeq (${CONFIG_APP_CAMKES_VM_INITRD1_SUPPORT},y)
INITRD1_FILENAME=$(patsubst "%",%,$(CONFIG_APP_CAMKES_VM_INITRD1_IMG))
${STAGE_DIR}/${INITRD1_FILENAME}: ${SOURCE_DIR}/linux/${INITRD1_FILENAME}
	@echo "[CP] $@"
	@cp $< $@
ARCHIVE1_DEPS += ${STAGE_DIR}/${INITRD1_FILENAME}
endif

${BUILD_DIR}/src/vm0/static/archive.o: ${ARCHIVE1_DEPS}
	$(Q)mkdir -p $(dir $@)
	@echo "[CPIO] $@"
	$(Q)${COMMON_PATH}/files_to_obj.sh $@ _cpio_archive $^
	@echo "[CPIO] done."	


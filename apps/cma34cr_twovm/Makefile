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
TEMPLATES := templates global-templates

VM_CONFIG := cma34cr_twovm
VM_NUM_VM := 2

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

# We want to run C99
CFLAGS += -std=gnu99 "-DCAMKES_VM_CONFIG=${VM_CONFIG}" -I${SOURCE_DIR}/configurations -I${SOURCE_DIR}/../../components/VM/configurations

CAMKES_FLAGS += --cpp-flag=-I${SOURCE_DIR}/configurations --cpp-flag=-I${SOURCE_DIR}/../../components/VM --cpp-flag="-DCAMKES_VM_CONFIG=${VM_CONFIG}" --cache-dir=${PWD}/build/camkes_cache

include ${PWD}/tools/camkes/camkes.mk

KERNEL_FILENAME := bzimage
ROOTFS_FILENAME := rootfs.cpio

ARCHIVE_DEPS := ${STAGE_DIR}/${KERNEL_FILENAME} ${STAGE_DIR}/${ROOTFS_FILENAME}

${STAGE_DIR}/${KERNEL_FILENAME}: $(SOURCE_DIR)/linux/${KERNEL_FILENAME}
	@echo "[EXTRACT-VMLINUX] $@"
	$(Q)mkdir -p $(@D)
	${PWD}/tools/elf/extract-vmlinux $< > $@

${STAGE_DIR}/${ROOTFS_FILENAME}: ${SOURCE_DIR}/linux/${ROOTFS_FILENAME}
	@echo "[CP] $@"
	@cp $< $@

${BUILD_DIR}/src/vm.fserv/static/archive.o: ${ARCHIVE_DEPS}
	$(Q)mkdir -p $(dir $@)
	@echo "[CPIO] $@"
	$(Q)${COMMON_PATH}/files_to_obj.sh $@ _cpio_archive $^
	@echo "[CPIO] done."


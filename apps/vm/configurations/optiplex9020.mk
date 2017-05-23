#
# Copyright 2017, Data61, CSIRO
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(D61_GPL)
#

Init0_CFILES += $(wildcard $(SOURCE_DIR)/src/optiplex9020/*.c) \
				$(wildcard $(SOURCE_DIR)/common/src/*.c)

Init0_HFILES += $(wildcard $(SOURCE_DIR)/common/include/*.h) \
				$(wildcard $(SOURCE_DIR)/common/shared_include/cross_vm_shared/*.h)

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

CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

Init_CFILES := $(wildcard $(CURRENT_DIR)/src/*.c) \
			   $(wildcard $(SOURCE_DIR)/src/*.c) \
			   $(wildcard $(SOURCE_DIR)/common/src/*.c)
Init_HFILES := $(wildcard $(CURRENT_DIR)/../../apps/vm/configurations/*.h)
Init_HFILES += $(wildcard $(CURRENT_DIR)/src/*.h) \
			   $(wildcard $(SOURCE_DIR)/common/include/*.h)
Init_LIBS := sel4camkes sel4vmm sel4utils cpio elf sel4vka sel4allocman sel4vspace pci sel4simple sel4simple-stable sel4platsupport ethdrivers platsupport

VM_LIST := $(shell seq 0 `expr ${VM_NUM_VM} - 1`)
$(foreach var, $(VM_LIST), \
    $(eval Init$(var)_HFILES := $(value Init_HFILES)); \
    $(eval Init$(var)_CFILES := $(value Init_CFILES)); \
    $(eval Init$(var)_OFILES := $(value Init_OFILES)); \
    $(eval Init$(var)_LIBS := $(value Init_LIBS)); \
)


CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

Ethdriver_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)
Ethdriver_HFILES := $(wildcard $(CURRENT_DIR)/../../apps/vm/configurations/*.h)
Ethdriver_HFILES += $(wildcard $(CURRENT_DIR)/../../apps/vm/configurations/c162/*.h)
Ethdriver_LIBS := sel4camkes sel4utils sel4vka sel4allocman sel4vspace sel4simple sel4platsupport ethdrivers


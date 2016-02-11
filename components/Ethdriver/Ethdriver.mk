CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

Ethdriver_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)
Ethdriver_HFILES := $(wildcard $(CURRENT_DIR)/include/*.h)
Ethdriver_HFILES += $(wildcard $(CURRENT_DIR)/../../apps/configurations/$(VM_CONFIG)/*.h)

Ethdriver_LIBS := sel4camkes sel4utils sel4vka sel4allocman sel4vspace sel4simple sel4platsupport ethdrivers

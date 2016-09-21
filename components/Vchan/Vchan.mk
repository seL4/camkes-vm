CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

Vchan_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)
Vchan_HFILES := $(wildcard $(CURRENT_DIR)/../../apps/vm/configurations/*.h)
Vchan_LIBS := sel4camkes sel4vmm sel4utils sel4vka sel4allocman sel4vspace sel4simple sel4simple-default sel4platsupport


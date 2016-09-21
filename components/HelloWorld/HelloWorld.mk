CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

HelloWorld_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)
HelloWorld_HFILES := $(wildcard $(CURRENT_DIR)/../../apps/vm/configurations/*.h)
HelloWorld_HFILES += $(wildcard $(CURRENT_DIR)/include/*.h)
HelloWorld_LIBS := sel4camkes sel4vmm sel4utils sel4vka sel4allocman sel4vspace sel4simple sel4simple-default


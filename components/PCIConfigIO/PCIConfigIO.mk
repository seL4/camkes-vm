CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

PCIConfigIO_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)


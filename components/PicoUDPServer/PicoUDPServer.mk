CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

PicoUDPServer_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)
PicoUDPServer_LIBS := sel4camkes ethdrivers picotcp sel4vspace


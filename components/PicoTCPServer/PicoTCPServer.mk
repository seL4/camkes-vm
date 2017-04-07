CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

PicoTCPServer_CFILES := $(wildcard ${CURRENT_DIR}/src/*)
PicoTCPServer_LIBS := sel4camkes ethdrivers picotcp sel4vspace


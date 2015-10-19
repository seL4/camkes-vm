CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

UDPServer_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)
UDPServer_LIBS := sel4camkes ethdrivers lwip sel4vspace


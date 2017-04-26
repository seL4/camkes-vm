CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

Firewall_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)

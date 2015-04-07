CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

TimeServer_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)

TimeServer_LIBS := platsupport


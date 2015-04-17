CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

FileServer_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)
FileServer_OFILES := archive.o
FileServer_LIBS := cpio sel4utils


CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

Echo_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)
Echo_LIBS := sel4vspace

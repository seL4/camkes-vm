CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

RTC_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)

RTC_LIBS := platsupport sel4vspace


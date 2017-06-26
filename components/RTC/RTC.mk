#
# Copyright 2017, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(DATA61_BSD)
#
CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

RTC_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)

RTC_LIBS := platsupport sel4vspace


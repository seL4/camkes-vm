#
# Copyright 2016, Data 61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(D61_GPL)
#

CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

StringReverse_CFILES := $(wildcard $(CURRENT_DIR)/src/*.c)
StringReverse_HFILES := $(wildcard $(CURRENT_DIR)/include/*.h)

CAMKES_FLAGS := ${CAMKES_FLAGS} --cpp-flag=-I$(CURRENT_DIR)/include

StringReverse_LIBS := sel4vspace

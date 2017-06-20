#
# Copyright 2017, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(DATA61_GPL)
#
CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

HelloWorld_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)
HelloWorld_HFILES := $(wildcard $(CURRENT_DIR)/../../apps/vm/configurations/*.h)
HelloWorld_HFILES += $(wildcard $(CURRENT_DIR)/include/*.h)
HelloWorld_LIBS := sel4camkes sel4vmm sel4utils sel4vka sel4allocman sel4vspace sel4simple sel4simple-default


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

Ethdriver_CFILES := $(wildcard ${CURRENT_DIR}/src/ethdriver.c)
Ethdriver_HFILES := $(wildcard $(CURRENT_DIR)/include/*.h)

Ethdriver_LIBS := sel4camkes sel4utils sel4vka sel4allocman sel4vspace sel4simple sel4platsupport ethdrivers

Ethdriver82574_CFILES := $(Ethdriver_CFILES) ${CURRENT_DIR}/src/82574.c
Ethdriver82574_HFILES := $(Ethdriver_HFILES)
Ethdriver82574_LIBS := $(Ethdriver_LIBS)

Ethdriver82580_CFLAGS += -DETH_82580 ${CURRENT_DIR}/src/82580.c
Ethdriver82580_CFILES := $(Ethdriver_CFILES)
Ethdriver82580_HFILES := $(Ethdriver_HFILES)
Ethdriver82580_LIBS := $(Ethdriver_LIBS)

#
# Copyright 2016, Data61, CSIRO
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(NICTA_GPL)
#

CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

PeriodicEvent_CFILES := $(wildcard $(CURRENT_DIR)/src/*.c)

PeriodicEvent_LIBS := sel4vspace

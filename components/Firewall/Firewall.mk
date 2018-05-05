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

# Firewall_CFILES := $(wildcard ${CURRENT_DIR}/src/*.c)
# Firewall_OFILES := ${CURRENT_DIR}/libsample.a
Firewall_RUST = ${CURRENT_DIR}/rustwall

# Replaces build.rs scritp
# Repalce with your own build configuration
Firewall_CFILES := ${CURRENT_DIR}/rustwall/src/external_firewall.c

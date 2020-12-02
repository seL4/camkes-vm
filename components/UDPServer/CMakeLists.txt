#
# Copyright 2018, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.8.2)

project(UDPServer C)

DeclareCAmkESComponent(UDPServer SOURCES src/udpserver.c LIBS sel4camkes ethdrivers lwip sel4vspace)

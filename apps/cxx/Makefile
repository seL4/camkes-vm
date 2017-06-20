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

TARGETS := $(notdir ${SOURCE_DIR}).cdl
ADL := cxx.camkes

Client_CXXFILES := \
    $(patsubst ${SOURCE_DIR}/%,%,$(wildcard ${SOURCE_DIR}/components/Client/src/*.cxx))

Echo_CXXFILES := \
    $(patsubst ${SOURCE_DIR}/%,%,$(wildcard ${SOURCE_DIR}/components/Echo/src/*.cxx))

Echo_LIBS := supc++ sel4cxx stdc++ ssp_nonshared

# Do not want any 'advanced' (read: unsupported) C++ features
# No position independent code, exceptions or rtti
CXXFLAGS += -fno-PIC -fno-pic -fno-exceptions -fno-rtti

# We want to add the c++ include paths, and the instrinsics headers, but not any other include paths.
# We build what we hope are the correct ones based on the compiler
GCCVERSION := $(shell ${TOOLPREFIX}gcc -dumpversion)
GCCMACHINE := $(shell ${TOOLPREFIX}gcc -dumpmachine)
CXXPATHS := -I/usr/include/c++/${GCCVERSION} -I/usr/include/${GCCMACHINE}/c++/${GCCVERSION}/32/ -I/usr/lib/gcc/${GCCMACHINE}/${GCCVERSION}/include
CXXFLAGS += ${CXXPATHS}

include ${PWD}/tools/camkes/camkes.mk

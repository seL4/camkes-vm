#
# Copyright 2016, Data 61
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(D61_GPL)
#

CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))
TOP_LEVEL := $(CURRENT_DIR)

CC = gcc
CFLAGS += -g -O2 -Wall -m32 -I$(CURRENT_DIR)include
LDFLAGS += -static -static-libgcc

all: $(TARGET)

.PHONY: clean
clean:
	rm -rf *.o $(TARGET)

%.o : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

#
# Copyright 2016, Data 61
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(D61_GPL)
#

CC = gcc
CFLAGS = -g -O2 -Wall -m32 -I../../include
LDFLAGS = -static -static-libgcc

COMMON_SRC = ../../src

.PHONY: all clean
all: dataport_init dataport_read dataport_write

clean:
	rm -rf *.o dataport_init dataport_read dataport_write

%.o: $(COMMON_SRC)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

dataport_init: init.o dataport.o device.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

dataport_read: read.o dataport.o device.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

dataport_write: write.o dataport.o device.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

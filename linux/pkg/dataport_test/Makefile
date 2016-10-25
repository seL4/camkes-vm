#
# Copyright 2016, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(D61_BSD)
#

CC = gcc
CFLAGS = -g -O2 -Wall -m32 -I../../include
LDFLAGS = -static -static-libgcc

.PHONY: all clean
all: dataport_init dataport_read dataport_write

clean:
	rm -rf *.o dataport_init dataport_read dataport_write

%.o : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

dataport_init: init.o dataport.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

dataport_read: read.o dataport.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

dataport_write: write.o dataport.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

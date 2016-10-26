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

COMMON_SRC = ../../src

.PHONY: all clean
all: string_reverse

clean:
	rm -rf *.o string_reverse

%.o: $(COMMON_SRC)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

string_reverse: string_reverse.o dataport.o emits_event.o consumes_event.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

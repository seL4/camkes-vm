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
all: emits_event_init emits_event_emit

clean:
	rm -rf *.o emits_event_init emits_event_emit

%.o: $(COMMON_SRC)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

emits_event_init: init.o device.o emits_event.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

emits_event_emit: emit.o device.o emits_event.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

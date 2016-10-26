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
all: string_reverse

clean:
	rm -rf *.o string_reverse

%.o: $(COMMON_SRC)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

string_reverse: string_reverse.o dataport.o emits_event.o consumes_event.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

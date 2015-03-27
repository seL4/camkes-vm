#
# Copyright 2014, NICTA
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(NICTA_GPL)
#

CC=gcc
CFLAGS=-O3 -Wall -Werror -m32 -I includes
DEPS=includes/%.h

all: sel4lib

OBJS := $(patsubst %.c,%.o,$(wildcard *.c))

# %.o: %.c $(DEPS)
# 	$(CC) $(CFLAGS) -c -o $@ $<

sel4lib: $(OBJS)
	ar -cvq libsel4vchan.a $(OBJS)
	ranlib libsel4vchan.a





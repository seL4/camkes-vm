#
# Copyright 2016, Data 61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(D61_GPL)
#

TARGET = string_reverse

include ../../common.mk
include ../../common_app.mk

string_reverse: string_reverse.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -lcamkes -o $@

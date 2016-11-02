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

TARGET = consumes_event_init consumes_event_wait

include ../../common.mk
include ../../common_app.mk

consumes_event_init: init.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -ldevice -o $@

consumes_event_wait: wait.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -lcamkes -o $@

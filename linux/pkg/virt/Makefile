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

TARGET = vmm_manager_tests \
         vm_guest_manip \
         sel4_vchan_testsuite \
         vchan_test_prog

include ../../common.mk
include ../../common_app.mk

CFLAGS += -I$(TOP_LEVEL)include/vchan

vmm_manager_tests: vmm_manager_tests.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -lsel4vchan -o $@

vm_guest_manip: vm_guest_manip.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -lsel4vchan -o $@

sel4_vchan_testsuite: sel4_vchan_testsuite.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -lsel4vchan -o $@

vchan_test_prog: vchan_test_prog.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -lsel4vchan -o $@

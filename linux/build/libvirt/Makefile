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
CFLAGS=-O3 -Wall -m32 -I ../libs/includes

FILES = vmm_manager_tests vm_guest_manip sel4_vchan_testsuite

all: $(FILES)
	mv vmm_manager_tests ../out/bin/
	mv vm_guest_manip ../out/bin/
	mv sel4_vchan_testsuite ../out/bin/

vmm_manager_tests: vmm_manager_tests.o
	$(CC) $(CFLAGS) -o $@ vmm_manager_tests.o -L../libs -lsel4vchan

vm_guest_manip: vm_guest_manip.o
	$(CC) $(CFLAGS) -o $@ vm_guest_manip.o -L../libs -lsel4vchan

conclude_vchan_init: conclude_vchan_init.o
	$(CC) $(CFLAGS) -o $@ conclude_vchan_init.o -L../libs -lsel4vchan

sel4_vchan_testsuite: sel4_vchan_testsuite.o
	$(CC) $(CFLAGS) -o $@ sel4_vchan_testsuite.o -L../libs -lsel4vchan

clean:
	rm $(FILES)

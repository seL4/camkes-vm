#
# Copyright 2014, NICTA
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(NICTA_GPL)
#

all:
	./buildout

	cd libs && $(MAKE)

	cd libvirt && $(MAKE)
	cd driver_module && $(MAKE)
	#
	cp driver_module/vmm_manager.ko out/lib/modules/3.15.0/kernel/drivers/vmm/vmm_manager.ko

#	fakeroot ../editfs -i
#	fakeroot ../editfs -p
#	rm libs/libsel4vchan.a


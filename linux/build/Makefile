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

	fakeroot ../editfs -i
	fakeroot ../editfs -p
	rm libs/libsel4vchan.a


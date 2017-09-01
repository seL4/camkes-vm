#
# Copyright 2017, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(DATA61_GPL)
#
obj-m += cg-fat.o
obj-m += cg-vfat.o

cg-fat-y := cache.o dir.o fatent.o file.o inode.o misc.o nfs.o
cg-vfat-y := module.o

EXTRA_CFLAGS=-I$(PWD)

all: cogent_vfat_module

ifndef KHEAD
KHEAD:= /lib/modules/$(shell uname -r)/build
endif

cogent_vfat_module:
	echo "DIR   $(PWD)"
	make -C $(KHEAD) M=$(PWD) modules

clean:
	make -C $(KHEAD) M=$(PWD) clean

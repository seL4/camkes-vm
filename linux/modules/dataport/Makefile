#
# Copyright 2016, Data 61
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(D61_GPL)
#

obj-m += dataport.o
CFLAGS_dataport.o = -I../../include -I../../../common/shared_include

all:
	make -C $(KHEAD) M=$(PWD) modules

clean:
	make -C $(KHEAD) M=$(PWD) clean

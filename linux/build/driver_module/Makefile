#
# Copyright 2014, NICTA
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(NICTA_GPL)
#

obj-m += driver.o
driver-objs += sel4_vmm_manager_driver.o event_thread.o
KHEAD=linux-3.15

all: vmm_module
	mv driver.ko ../out/lib/modules/3.15.0/kernel/drivers/vmm/vmm_manager.ko

vmm_module:
	echo "DIR   $(PWD)"
	make -C $(KHEAD) M=$(PWD) modules

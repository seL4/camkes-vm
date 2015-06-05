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
KHEAD=linux-3.15.1

all: vmm_module
	mv driver.ko vmm_manager.ko

vmm_module:
	echo "DIR   $(PWD)"
	make -C $(KHEAD) M=$(PWD) modules

# get .config file < prop setup
# set KHEAD to linux driver name,
# make oldconfig
# make prepare
# make modules_prepare

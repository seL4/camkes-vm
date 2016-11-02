/*
 * Copyright 2016, Data 61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(D61_GPL)
 */

// Linux kernel module for cross vm events

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>

#include <asm/uaccess.h>
#include <asm/kvm_para.h>
#include <asm/irq.h>
#include <asm/io.h>

#include <cross_vm_shared/cross_vm_shared_guest_to_vmm_event.h>
#include <emits_event/ioctl_commands.h>

#define DEVICE_NAME "emits_event"

static int major_number;

static void event_vmcall_emit(unsigned int id) {
    kvm_hypercall2(EVENT_VMCALL_GUEST_TO_VMM_HANDLER_TOKEN,
                   EVENT_CMD_EMIT, id);
}

static long emits_event_ioctl(struct file *filp,
                              unsigned int ioctl_num,
                              unsigned long ioctl_param)
{
    if (ioctl_num == EMITS_EVENT_EMIT) {
        int minor = iminor(filp->f_path.dentry->d_inode);
        event_vmcall_emit(minor);
    } else {
        printk(KERN_ERR "%s unknown ioctl command: %d\n", DEVICE_NAME, ioctl_num);
        return -1;
    }

    return 0;
}

struct file_operations fops = {
    .unlocked_ioctl = emits_event_ioctl,
};

static int __init emits_event_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    printk(KERN_INFO "%s initialized with major number %d\n", DEVICE_NAME, major_number);

    return 0;
}

static void __exit emits_event_exit(void) {
    unregister_chrdev(major_number, DEVICE_NAME);

    printk(KERN_INFO "%s exit\n", DEVICE_NAME);
}

module_init(emits_event_init);
module_exit(emits_event_exit);

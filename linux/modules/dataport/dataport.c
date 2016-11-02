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

/* Linux kernel module for cross vm dataports.
 * This allows the creation of device files which represent cross vm dataports
 * (ie. shared memory regions between linux processes and camkes components).
 * This module implements mmap for dataport files so linux processes can mmap
 * dataport files to establish shared memory. Additionally, it takes care of
 * making the relevant hypercalls to invoke the vmm to set up shared memory
 * between the guest (linux) and camkes.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mm.h>

#include <asm/uaccess.h>
#include <asm/kvm_para.h>

#include <dataport/ioctl_commands.h>
#include <cross_vm_shared/cross_vm_shared_dataport.h>

#define DEVICE_NAME "dataport"
#define MAX_NUM_DATAPORTS 256
#define BUF_SIZE 64
#define IOCTL_CMD_SIZE 64

typedef struct dataport {
    unsigned long id;
    size_t size;
    void* data;
    phys_addr_t paddr;
} dataport_t;

static dataport_t dataports[MAX_NUM_DATAPORTS];
static int major_number;

static void dataport_vmcall_share(dataport_t *dataport)
{
    /* Tell the vmm the id, guest paddr and size of the dataport.
     * The vmm will replace the mappings at the specified guest paddr
     * with mappings to frames backing the dataport specified by id.
     */
    kvm_hypercall4(DATAPORT_VMCALL_HANDLER_TOKEN,
                   DATAPORT_CMD_SHARE,
                   dataport->id,
                   dataport->paddr,
                   dataport->size);
}

static int dataport_is_allocated(dataport_t *dataport)
{
    return dataport->data != NULL;
}

static void dataport_free(dataport_t *dataport)
{
    // TODO vmcall to map the original frames back in
    kfree(dataport->data);
    dataport->data = NULL;
}

static dataport_t* dataport_get(unsigned long dataport_id)
{
    if (dataport_id >= MAX_NUM_DATAPORTS) {
        return NULL;
    }

    return &dataports[dataport_id];
}

static dataport_t* dataport_allocate(unsigned long dataport_id,
                                     unsigned long size)
{
    dataport_t *dataport = dataport_get(dataport_id);
    if (dataport == NULL) {
        return NULL;
    }

    if (dataport_is_allocated(dataport)) {
        dataport_free(dataport);
    }

    dataport->id = dataport_id;
    dataport->size = size;

    // dataports are guest-physically contiguous
    dataport->data = kmalloc(size, 0);
    dataport->paddr = virt_to_phys(dataport->data);

    return dataport;
}

static long dataport_ioctl(struct file *filp,
                             unsigned int ioctl_num,
                             unsigned long ioctl_param)
{
    int error;
    int minor;
    dataport_t *dataport;
    __user char* user_buf;

    minor = iminor(filp->f_path.dentry->d_inode);

    user_buf = (__user char*)ioctl_param;

    if (ioctl_num == DATAPORT_ALLOCATE) {
        unsigned long size;

        error = kstrtoul_from_user(user_buf, IOCTL_CMD_SIZE, 0, &size);

        if (error) {
            printk(KERN_ERR "invalid dataport size");
            return -1;
        }

        dataport = dataport_allocate(minor, size);

        if (dataport == NULL) {
            printk(KERN_ERR "failed to allocate dataport");
            return -1;
        }

        dataport_vmcall_share(dataport);
    } else {
        char buf[BUF_SIZE];

        dataport = dataport_get(minor);

        if (dataport == NULL) {
            printk(KERN_ERR "failed to retrieve dataport with id %d", minor);
            return -1;
        } else if (!dataport_is_allocated(dataport)) {
            printk(KERN_ERR "dataport %d is not allocated", minor);
            return -1;
        }

        switch (ioctl_num) {
        case DATAPORT_GET_PADDR:
            snprintf(buf, BUF_SIZE, "%u", dataport->paddr);
            error = copy_to_user(user_buf, buf, strlen(buf) + 1);

            if (error) {
                return -1;
            }

            break;
        case DATAPORT_GET_SIZE:
            snprintf(buf, BUF_SIZE, "%u", dataport->size);
            error = copy_to_user(user_buf, buf, strlen(buf) + 1);

            if (error) {
                return -1;
            }

            break;
        }
    }

    return 0;
}

static int dataport_mmap(struct file *filp,
                           struct vm_area_struct *vma)
{
    int minor;
    int error;
    dataport_t *dataport;

    minor = iminor(filp->f_path.dentry->d_inode);

    printk(KERN_INFO "%s received mmap for minor %d\n",
           DEVICE_NAME, minor);

    dataport = dataport_get(minor);

    if (dataport == NULL) {
        printk(KERN_ERR "failed to retrieve dataport with id %d", minor);
        return -ENODEV;
    }

    error = remap_pfn_range(vma,
                            vma->vm_start,
                            dataport->paddr >> PAGE_SHIFT,
                            dataport->size,
                            vma->vm_page_prot);

    if (error < 0) {
        printk(KERN_ERR "%s mmap of dataport %d failed\n",
               DEVICE_NAME, minor);
        return -EAGAIN;
    }

    return 0;
}

struct file_operations fops = {
    .mmap = dataport_mmap,
    .unlocked_ioctl = dataport_ioctl,
};

static int __init dataport_init(void) {
    int i;
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    printk(KERN_INFO "%s initialized with major number %d\n", DEVICE_NAME, major_number);

    for (i = 0; i < MAX_NUM_DATAPORTS; i++) {
        dataports[i].data = NULL;
    }

    return 0;
}

static void __exit dataport_exit(void) {
    int i;

    unregister_chrdev(major_number, DEVICE_NAME);

    for (i = 0; i < MAX_NUM_DATAPORTS; i++) {
        if (dataports[i].data != NULL) {
            dataport_free(&dataports[i]);
        }
    }

    printk(KERN_INFO "%s exit\n", DEVICE_NAME);
}

module_init(dataport_init);
module_exit(dataport_exit);

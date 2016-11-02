/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

/* Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/ioctl.h>
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/uaccess.h> /* copy_from/to_user */
#include <asm-generic/ioctl.h>
#include <linux/wait.h>
#include <linux/sched.h> /*Helps fix TASK_UNINTERRUPTIBLE */
#include <linux/pid.h>
#include <linux/fdtable.h>
#include <linux/rcupdate.h>
#include <linux/eventfd.h>
#include <linux/kthread.h>  // for threads
#include <linux/time.h>   // for using jiffies
#include <linux/timer.h>
#include <linux/io.h>

#include "../../lib_src/vchan/includes/vmm_manager.h"
#include "../../lib_src/vchan/includes/sel4libvchan.h"
#include "../../lib_src/vchan/includes/vmm_driver.h"
#include "../../lib_src/vchan/includes/vchan_copy.h"

#define DRIVER_NAME "vmm_manager"
#define DRIVER_AUTH "<nicta.com.au>"
#define DRIVER_DESC "Guest Level - Linux Kernel to sel4 vm manager"
#define _ASM_VMCALL ".byte 0x0f,0x01,0xc1" /* ia32 instruction to perform a vmcall hypervisor exception */

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTH);
MODULE_DESCRIPTION(DRIVER_DESC);

/* Driver management functions */
int vmm_manager_open(struct inode *inode, struct file *filp);
int vmm_manager_release(struct inode *inode, struct file *filp);
static long vmm_manager_ioctl(struct file *f, unsigned int cmd, unsigned long arg);
static void vmm_manager_exit(void);
static int vmm_manager_init(void);

/* Actions that correspond to sel4 vmm manager actions */
int vmm_num_of_dom(void *cont, ioctl_arg_t *args, int cmd);
int vmm_def_action(void *cont, ioctl_arg_t *args, int cmd);
int vmm_read_write(void *cont, ioctl_arg_t *args, int cmd);
int vmm_guest_num(void *cont, ioctl_arg_t *args, int cmd);
int driver_sleep_vm(void *cont, ioctl_arg_t *args, int cmd);
int driver_wakeup_vm(void *cont, ioctl_arg_t *args, int cmd);

int sel4_driver_vchan_connect(void *cont, ioctl_arg_t *args, int cmd);
int sel4_driver_vchan_close(void *cont, ioctl_arg_t *args, int cmd);
int sel4_driver_vchan_wait(void *cont, ioctl_arg_t *args, int cmd);
int sel4_driver_vchan_state(void *cont, ioctl_arg_t *args, int cmd);

struct file_operations vmm_fops = {
    open: vmm_manager_open,
    release: vmm_manager_release,
    unlocked_ioctl: vmm_manager_ioctl
};

/* Function lookup table for driver actions */
static struct vmm_op_lookup_table {
    int (*op_func[NUM_VMM_OPS])(void *, ioctl_arg_t *, int);
} vmm_op_lookup_table = {

    .op_func[VMM_NUM_OF_DOM] = &vmm_def_action,

    .op_func[VCHAN_SEND] = &vmm_read_write,
    .op_func[VCHAN_RECV] = &vmm_read_write,

    .op_func[VMM_GUEST_NUM] = &vmm_guest_num,

    .op_func[SEL4_VCHAN_CONNECT] = &sel4_driver_vchan_connect,
    .op_func[SEL4_VCHAN_CLOSE] = &sel4_driver_vchan_close,
    .op_func[SEL4_VCHAN_WAIT] = &sel4_driver_vchan_wait,
    .op_func[SEL4_VCHAN_STATE] = &sel4_driver_vchan_state,

    // .op_func[VCHAN_SLEEP] = &driver_sleep_vm,
    // .op_func[VCHAN_WAKEUP] = &driver_wakeup_vm,
};

int vmm_major = 1;
int vmm_run_num;

/* Memory buffer for storing arguments passed to vmm */
void *vmm_mem_buf = NULL;
struct semaphore hyp_sem; /* Mutex for modifying the vchan instance list */
struct vmcall_args *vargs = NULL;
vmm_args_t *uvargs = NULL;


/*
    Initialise the vmm_manager
*/
static int __init vmm_manager_init(void) {
    int err;
    int *data;

    sema_init(&hyp_sem, 1);

    if(init_event_thread() < 0) {
        printk ("k_vmm_manager: Failed to initialise event manager\n");
        return -EINVAL;
    }

    /* Create a memory buffer in the kernel for copying in/out non vchan arguments */
    vmm_mem_buf = kmalloc(sizeof(struct vmcall_args), 0);
    if (vmm_mem_buf == NULL) {
        printk ("k_vmm_manager: Failed to allocate buffer memory\n");
        free_event_thread();
        return -ENOMEM;
    }

    vargs = (struct vmcall_args *)vmm_mem_buf;
    vargs->data = kmalloc(sizeof(struct vmm_args), 0);
    if (vargs->data == NULL) {
        printk ("k_vmm_manager: Failed to allocate buffer memory\n");
        kfree(vmm_mem_buf);
        free_event_thread();
        return -ENOMEM;
    }

    /* Store physical addresses of memory buffers, as the vmm uses physical addresses */
    uvargs = (vmm_args_t *)vargs->data;

    /* Check with the hypervisor for ok connection */
    err = call_into_hypervisor(VMM_CONNECT, uvargs, DRIVER_ARGS_MAX_SIZE, vargs);
    if (err) {
        printk ("k_vmm_manager: failed on sel4 hypervisor connection |%d|\n", err);
        kfree(vargs->data);
        kfree(vmm_mem_buf);
        free_event_thread();
        return -EINVAL;
    }

    /* Set the running number for this vm */
    data = (int *)uvargs->ret_data;
    vmm_run_num = *data;
    printk(KERN_INFO "k_vmm_manager: vmm driver running on guest %d|%d\n", vmm_run_num,  *data);

    /* Register this device with the linux kernel */
    vmm_major = register_chrdev(0, DRIVER_NAME, &vmm_fops);
    if (vmm_major < 0) {
        printk ("k_vmm_manager: failed to register character device %d\n", vmm_major);
        kfree(vargs->data);
        kfree(vmm_mem_buf);
        free_event_thread();
        return -EINVAL;
    }

    // printk(KERN_INFO "k_vmm_manager: Starting test suite %d\n", vmm_major);
    // /* Use the test suite to verify the vmm manager is working */
    // err = call_into_hypervisor(VMM_TESTS, uvargs, DRIVER_ARGS_MAX_SIZE, vargs);
    // if(err) {
    //     printk(KERN_INFO "k_vmm_manager: Failed in hypervisor test suite\n");
    //     kfree(vargs->data);
    //     kfree(vmm_mem_buf);
    //     unregister_chrdev(vmm_major, DRIVER_NAME);
    //     return -EINVAL;
    // }

    printk(KERN_INFO "k_vmm_manager: Salut, Mundi, vmm_manager: assigned major: %d\n", vmm_major);
    printk(KERN_INFO "k_vmm_manager: create node with mknod /dev/%s c %d 0\n", DRIVER_NAME, vmm_major);

    return 0;
}

/* Removal of the vmm_driver */
static void __exit vmm_manager_exit(void) {
    unregister_chrdev(vmm_major, DRIVER_NAME);

    kfree(vargs->data);
    kfree(vmm_mem_buf);

    printk(KERN_INFO "k_vmm_manager: Removed vmm manager module\n");
    return;
}

// Open - Does nothing
int vmm_manager_open(struct inode *inode, struct file *filp) {
    return 0;
}

// Open - Does nothing
int vmm_manager_release(struct inode *inode, struct file *filp) {
    return 0;
}

/*
    Vmm Iocotl, main interaction handler for the vmm_manager

    Recieves cmd, and pointer to user space memory for arguments
        - Performs an operation based on the command passed in,
*/
static long vmm_manager_ioctl(struct file *f, unsigned int cmd, unsigned long arg) {

    ioctl_arg_t ioctl_args;
    void *in_arg_addr, *user_arg;
    int err;

    size_t size = _IOC_SIZE(cmd);
    cmd = _IOC_NR(cmd);
    user_arg = (void __user *)arg;

    if (cmd > NUM_VMM_OPS) {
        return -EINVAL;
    } else {
        /* Check for a valid command and a non-null pointer */
        if(vmm_op_lookup_table.op_func[cmd] == NULL ) {
            printk("drvr: bad cmd %d\n", cmd);
            return -ENOTTY;
        }

        in_arg_addr = kmalloc(size, GFP_KERNEL);
        if(in_arg_addr == NULL) {
            printk("drvr: bad malloc\n");
            return -ENOMEM;
        }

        err = copy_from_user(in_arg_addr, user_arg, size);
        if(err) {
            printk("drvr: bad cpy %d\n", err);
            kfree(in_arg_addr);
            return -EINVAL;
        }

        ioctl_args.size = size;
        ioctl_args.ptr = user_arg;

        err = (*vmm_op_lookup_table.op_func[cmd])(in_arg_addr, &ioctl_args, cmd);
        kfree(in_arg_addr);

        return err;
    }
}

/*
    Function to call into the sel4 vmm
    - Given command/request is copied into the eax register,
    - the ebx register stores the PHYSICAL address of the vmm_managers memory buffer

    The underlying vm is expected to operate on the memory buffer and store results in it
    If an error occured, eax will store -1 on return from sel4 vmm, else 0
*/
int call_into_hypervisor(int cmd, void *data, size_t sz, vmcall_args_t *vmcall) {

    int res = 0;
    int err;
    unsigned phys_ptr = virt_to_phys(vmcall);

    vmcall->data = data;
    vmcall->cmd = cmd;
    vmcall->phys_data = virt_to_phys(data);
    vmcall->size = sz;

    down(&hyp_sem);

    /*
        Set up arguments for the vmcall
    */
    __asm volatile ( "movl %0, %%eax; movl %1, %%ebx" : : "r" (VMM_MANAGER_TOKEN), "r" (phys_ptr): "%eax", "%ebx");
    /* Perform the vmcall */
    __asm volatile (_ASM_VMCALL);
    /* Check if we did not get -1 */
    __asm ("movl %%eax, %0" : "=r" (res) : : );

    err = vargs->err;

    up(&hyp_sem);

    return err;
}

/*
    Perform a vmm manager action that can be fit into the standard vmm_manager argument
*/
int vmm_def_action(void *cont, ioctl_arg_t *args, int cmd) {

    int err;
    /* Call into the sel4 vmm, return early if error encountered */
    err = call_into_hypervisor(cmd, cont, args->size, vargs);
    if (err) {
        return err;
    }

    /* Finished, copy out arguments */
    err = copy_to_user(args->ptr, cont, args->size);
    if(err) {
        return -EINVAL;
    }

    return args->size;
}

/*
    Perform a read/write to a given operation
*/
int vmm_read_write(void *cont, ioctl_arg_t *args, int cmd) {

    int err, res;
    size_t send_size;
    void *user_ptr;

    vchan_args_t *vchan_args = (vchan_args_t *)cont;
    user_ptr = vchan_args->mmap_ptr;

    /* Simple sanity checking of size and ptr */
    if (user_ptr == NULL) {
        printk("k_vmm_manager_vchan: bad ptr\n");
        return -EINVAL;
    } else if (vchan_args->size <= 0) {
        printk("k_vmm_manager_vchan: bad size\n");
        return 0;
    }

    /* Make an argument that can be passed into the kernel */
    vchan_args->mmap_ptr = kmalloc(vchan_args->size, GFP_KERNEL);
    if(vchan_args->mmap_ptr == NULL) {
        printk("k_vmm_manager_vchan: bad malloc\n");
        return -ENOMEM;
    }

    /* Copy data from user level if valid */
    if(cmd == VCHAN_SEND) {
        err = copy_from_user(vchan_args->mmap_ptr, user_ptr, vchan_args->size);
        if(err) {
            // printk("k_vmm_manager_vchan: BAD 2nd COPY\n");
            kfree(vchan_args->mmap_ptr);
            return -EINVAL;
        }
    }

    vchan_args->mmap_phys_ptr = virt_to_phys(vchan_args->mmap_ptr);
    if(vchan_args->mmap_phys_ptr == 0) {
        printk("k_vmm_manager_vchan_readwrite: bad phys pointer\n");
        return -EINVAL;
    }

    /* Wait until buffer is free */
    res = wait_for_event(vchan_args->v.dest, vchan_args->v.port, cmd);
    /* Connection closed and/or there is no data */
    if(res == -1) {
        printk("k_vmm_manager_vchan_readwrite: error in wait!\n");
        kfree(vchan_args->mmap_ptr);
        return 0;
    }

    // printk("readwrite: %d|%d size: %d\n", cmd, vchan_args->v.port, vchan_args->size);

    err = call_into_hypervisor(cmd, vchan_args, args->size, vargs);
    if (err) {
        printk("k_vmm_manager_vchan_readwrite: bad hypervisor call %d\n", err);
        kfree(vchan_args->mmap_ptr);
        return err;
    }

    send_size = vchan_args->size;

    if(cmd == VCHAN_RECV && send_size >= 0) {
        err = copy_to_user(user_ptr, vchan_args->mmap_ptr, vchan_args->size);
        if(err) {
            printk("k_vmm_manager_vchan_readwrite: copying out %d failed\n", vchan_args->size);
            kfree(vchan_args->mmap_ptr);
            return -EINVAL;
        }
    }

    // printk("readwrite done: %d|%d size: %d\n", cmd, vchan_args->v.port, vchan_args->size);

    kfree(vchan_args->mmap_ptr);
    return send_size;
}

/*
    Return the number vm guest this driver is running on
*/
int vmm_guest_num(void *cont, ioctl_arg_t *args, int cmd) {

    int err;

    int *ret = (int *) uvargs->ret_data;
    *ret = vmm_run_num;
    uvargs->datatype = DATATYPE_INT;

    /* Finished, copy out arguments */
    err = copy_to_user(args->ptr, uvargs, sizeof(vmm_args_t));
    if(err) {
        return -EINVAL;
    }

    return sizeof(vmm_args_t);
}

/*
    Creates a new vchan instance,
        by notifying the vmm and creating kernel level state
*/
int sel4_driver_vchan_connect(void *cont, ioctl_arg_t *args, int cmd) {
    int err;

    vchan_alert_t *event_mon;
    vchan_connect_t *pass = (vchan_connect_t *)cont;

    // printk("cn: %d %d %d %d\n", pass->v.dest, pass->v.port, pass->event_mon, pass->eventfd);

    /* Set up event monitor */
    event_mon = kmalloc(sizeof(vchan_alert_t), GFP_KERNEL);
    if(event_mon == NULL) {
        return -ENOMEM;
    }

    pass->event_mon = virt_to_phys(event_mon);
    event_mon->alert = VCHAN_EMPTY_BUF;
    event_mon->dest = pass->v.dest;
    event_mon->port = pass->v.port;

    if(new_event_instance(pass->v.dest, pass->v.port, pass->eventfd, event_mon, vmm_run_num) < 0) {
        printk("k_vmm_manager_vchan_connect: bad event creation\n");
        return -EINVAL;
    }

    err = call_into_hypervisor(cmd, cont, args->size, vargs);
    if (err) {
        printk("k_vmm_manager_vchan_connect: bad hypervisor call\n");
        rem_event_instance(pass->v.dest, pass->v.port);
        return -EINVAL;
    }

    err = copy_to_user(args->ptr, cont, sizeof(vchan_connect_t));
    if(err) {
        printk("k_vmm_manager_vchan_state: bad copy\n");
        return -EINVAL;
    }

    return 0;
}

/*
    Closes a vchan instance,
        by notifying the vmm and closing kernel level state
*/
int sel4_driver_vchan_close(void *cont, ioctl_arg_t *args, int cmd) {

    int err;
    vchan_connect_t *pass;

    pass = (vchan_connect_t *)cont;

    // printk("cn close: %d %d %d %d\n", pass->v.dest, pass->v.port, pass->event_mon, pass->eventfd);
    rem_event_instance(pass->v.dest, pass->v.port);

    err = call_into_hypervisor(cmd, cont, args->size, vargs);
    if (err) {
        printk("k_vmm_manager_vchan_close: bad hypervisor call\n");
        return err;
    }

    return sizeof(vchan_connect_t);
}

/*
    nowait = 0: Wait until a read/write action is possible, blocking
    nowait > 0: check the state of the shared vchan buffer
*/
int sel4_driver_vchan_wait(void *cont, ioctl_arg_t *args, int cmd) {

    int err;
    vchan_check_args_t *in_wait = (vchan_check_args_t *)cont;

    if(in_wait->nowait) {
        err = call_into_hypervisor(SEL4_VCHAN_BUF, cont, args->size, vargs);
        if(err) {
            return -1;
        }
    } else {
        // printk("kwait: perfoming wait\n");
        /* Wait for data, or closed connection */
        in_wait->state = wait_for_event(in_wait->v.dest, in_wait->v.port, VCHAN_RECV);
    }

    err = copy_to_user(args->ptr, cont, sizeof(vchan_check_args_t));
    if(err) {
        printk("k_vmm_manager_vchan_wait: failed copyout\n");
        return -1;
    }

    // printk("%d;;retval\n", in_wait->state);

    return sizeof(vchan_check_args_t);

}

/*
    Check the status of a vchan connection
*/
int sel4_driver_vchan_state(void *cont, ioctl_arg_t *args, int cmd) {

    int err;
    // printk("k_vmm_manager_vchan_state: checking state\n");

    err = call_into_hypervisor(cmd, cont, args->size, vargs);
    if (err) {
        printk("k_vmm_manager_vchan_state: bad hypervisor call %d err\n", err);
        return -EINVAL;
    }

    err = copy_to_user(args->ptr, cont, sizeof(vchan_check_args_t));
    if(err) {
        printk("k_vmm_manager_vchan_state: bad copy\n");
        return -EINVAL;
    }

    return sizeof(vchan_check_args_t);
}

/*
    Sleep this vm until it is woken up by an event
*/
int driver_sleep_vm(void *cont, ioctl_arg_t *args, int cmd) {
    int err;
    err = call_into_hypervisor(cmd, cont, args->size, vargs);
    if (err) {
        printk("k_vmm_manager_vchan_state: bad sleep call\n");
        return err;
    }

    return 0;
}

/*
    Wakeup a given vm by sending it an event
*/
int driver_wakeup_vm(void *cont, ioctl_arg_t *args, int cmd) {
    int err;

    err = call_into_hypervisor(cmd, cont, args->size, vargs);
    if (err) {
        printk("k_vmm_manager_vchan_state: bad wakeup call\n");
        return err;
    }

    return  0;
}


module_init(vmm_manager_init);
module_exit(vmm_manager_exit);

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
#include <linux/sched.h>

#include <asm/uaccess.h>
#include <asm/kvm_para.h>
#include <asm/irq.h>
#include <asm/io.h>

#include <cross_vm_shared/cross_vm_shared_vmm_to_guest_event.h>

#define DEVICE_NAME "consumes_event"

typedef struct event_state {
    unsigned int id; // id of the event
    unsigned int status; // non-zero if event has been sent to us but not
                         // received by linux userlevel
    struct mutex status_lock;
    wait_queue_head_t queue;
} event_state_t;

static int major_number;
static volatile event_context_t *event_context;
static event_state_t event_states[MAX_NUM_EVENTS];

/* Establishes a page of shared memory between the
 * vmm and this kernel module.
 * TODO return the number of event interfaces present
 */
static void event_vmcall_init(void) {
    phys_addr_t context_paddr = virt_to_phys(event_context);
    kvm_hypercall2(EVENT_VMCALL_VMM_TO_GUEST_HANDLER_TOKEN,
                   EVENT_CMD_INIT,
                   context_paddr);
}

static void event_vmcall_ack(void) {
    kvm_hypercall1(EVENT_VMCALL_VMM_TO_GUEST_HANDLER_TOKEN,
                   EVENT_CMD_ACK);
}

static event_state_t *get_event_state(int id) {
    if (id < MAX_NUM_EVENTS) {
        return &event_states[id];
    }
    return NULL;
}

static unsigned int consumes_event_poll(struct file *file, poll_table *wait) {
    int minor;
    event_state_t *event_state;

    minor = iminor(file->f_path.dentry->d_inode);

    event_state = get_event_state(minor);
    if (event_state == NULL) {
        printk(KERN_ERR "%s poll: invalid event id", DEVICE_NAME);
        return POLLERR;
    }

    poll_wait(file, &event_state->queue, wait);

    mutex_lock(&event_state->status_lock);

    if (event_state->status) {
        event_state->status = 0;

        mutex_unlock(&event_state->status_lock);
        return POLLIN;
    }

    mutex_unlock(&event_state->status_lock);

    return 0;
}

static struct file_operations fops = {
    .poll = consumes_event_poll,
};

static irqreturn_t handle_interrupt(int irq, void *dev_id) {

    event_state_t *event_state;
    unsigned int event_id = event_context->id;
    event_vmcall_ack();

    event_state = get_event_state(event_id);
    if (event_state == NULL) {
        printk(KERN_ERR "%s poll: invalid event id", DEVICE_NAME);
        return IRQ_RETVAL(IRQ_HANDLED);
    }

    mutex_lock(&event_state->status_lock);

    event_state->status = 1;

    mutex_unlock(&event_state->status_lock);

    if (waitqueue_active(&event_state->queue)) {
        wake_up_interruptible(&event_state->queue);
    }

    return IRQ_RETVAL(IRQ_HANDLED);
}

static int __init consumes_event_init(void) {
    int error, i;
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    error = request_irq(EVENT_IRQ_NUM, handle_interrupt, IRQF_NO_SUSPEND, DEVICE_NAME, NULL);

    if (error) {
        printk(KERN_ERR "couldn't request irq %d\n", error);
        return -1;
    }

    /* This is allocated in its own page so it can be
     * shared with the vmm. */
    event_context = kmalloc(PAGE_SIZE, 0);

    if (event_context == NULL) {
        printk(KERN_ERR "couldn't allocate context");
        free_irq(EVENT_IRQ_NUM, NULL);
        return -1;
    }

    event_vmcall_init();

    // sanity check for shared memory
    if (event_context->magic != EVENT_CONTEXT_MAGIC) {

        printk(KERN_ERR "incorrect event context magic number (expected %d, got %d)",
               EVENT_CONTEXT_MAGIC, event_context->magic);

        kfree((void*)event_context);
        free_irq(EVENT_IRQ_NUM, NULL);
        return -1;
    }

    for (i = 0; i < MAX_NUM_EVENTS; i++) {
        event_states[i].id = i;
        event_states[i].status = 0;
        init_waitqueue_head(&event_states[i].queue);
        mutex_init(&event_states[i].status_lock);
    }


    printk(KERN_INFO "%s initialized with major number %d\n", DEVICE_NAME, major_number);

    return 0;
}

static void __exit consumes_event_exit(void) {
    free_irq(EVENT_IRQ_NUM, NULL);
    unregister_chrdev(major_number, DEVICE_NAME);
    kfree((void*)event_context);

    // TODO tear down shared memory

    printk(KERN_INFO "%s exit\n", DEVICE_NAME);
}

module_init(consumes_event_init);
module_exit(consumes_event_exit);

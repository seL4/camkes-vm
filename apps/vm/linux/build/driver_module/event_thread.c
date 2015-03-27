/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/irq_vectors.h>
#include <linux/list.h>

#include "../libs/includes/vmm_manager.h"
#include "../libs/includes/sel4libvchan.h"
#include "../libs/includes/vmm_driver.h"

/*
    Represents an active vchan connection
        Handles relaying state of vchan buffer, and blocking threads
*/
typedef struct event_instance {
    int port, domain, self; /* vchan connection */
    int wait_type; /* What is this instance interested in */
    /*
        Signaled:
            0: Thread is waiting for a non-block, should be woken up on non-block
            1: No waiting thread, don't perform any wakeups
    */
    int signaled;
    /*
        Value linked to hypervisor,
            hypervisor changes this value whenever the status of the vchan buffer changes
    */
    vchan_alert_t *event_mon;

    struct completion wait; /* Used for sleeping threads */
    struct eventfd_ctx *efd_ctx; /* Used for pinging an user level eventfd */
    struct list_head node;
} einstance_t;

static struct vchan_event_control {
    int num_instances;
    struct semaphore inst_sem; /* Mutex for modifying the vchan instance list */
    struct list_head instances; /* All active vchan instances in the system */

    /*
        Used by event_thread_run, a top half interrupt handler
    */
    struct workqueue_struct *event_work;
    struct work_struct *event_work_struct;
} vchan_ctrl = {
    .num_instances = 0,
};

static uint64_t plus_one = 1;

static irqreturn_t event_irq_handler(int, void *);
void event_thread_run(struct work_struct *work);
einstance_t *get_event_instance(int domain, int port);
void print_inst(void);

/*
    Bottom half interrupt handler
        When the hypervisor changes the state of a vchan connection, it triggers an interrupt
        We register a function to run later (via a workqueue), to see what state has changed
*/
static irqreturn_t event_irq_handler(int irq, void *dev_id) {
    struct vchan_event_control *dv = (struct vchan_event_control *)dev_id;
    queue_work(dv->event_work, dv->event_work_struct);

    return IRQ_HANDLED;
}

/*
    Functions for setting up the hypervisor interrupt
*/
int reg_event_irq_handler() {
    return request_irq(VCHAN_EVENT_IRQ, &event_irq_handler, 0 | IRQF_SHARED, DRIVER_NAME, &vchan_ctrl);
}

void free_event_irq_handler() {
    free_irq(VCHAN_EVENT_IRQ, &vchan_ctrl);
}

/*
    Initialise the instance event monitor
*/
int init_event_thread(void) {

    /* Set up interrupt handler */
    if(reg_event_irq_handler() < 0) {
        return -1;
    }

    /* Set up workqueue */
    vchan_ctrl.event_work = create_workqueue("vmm_event");
    if(vchan_ctrl.event_work) {
        vchan_ctrl.event_work_struct = (struct work_struct *)kmalloc(sizeof(struct work_struct), GFP_ATOMIC);
        if(vchan_ctrl.event_work_struct) {
            INIT_WORK( vchan_ctrl.event_work_struct, event_thread_run );
        } else {
            return -1;
        }
    } else {
        return -1;
    }

    INIT_LIST_HEAD(&vchan_ctrl.instances);
    sema_init(&vchan_ctrl.inst_sem, 1);

    return 0;
}

/*
    Connect a vchan instance to the event monitor
*/
int new_event_instance(int domain, int port, int eventfd, vchan_alert_t *mon, int self) {

    int a;
    einstance_t *inst;
    pid_t pid = task_pid_nr(current);
    struct task_struct * userspace_task = NULL;
    struct file * efd_file = NULL;

    if(mon == NULL) {
        printk("ethread: bad mon!\n");
        return -1;
    }

    down(&vchan_ctrl.inst_sem);

    inst = get_event_instance(domain, port);
    if(inst != NULL) {
        printk("ethread: warning: event already exists and is not closed %d!\n", a);
        up(&vchan_ctrl.inst_sem);
        return -1;
    }

    inst = kmalloc(sizeof(einstance_t), GFP_ATOMIC);
    if(inst == NULL) {
        printk("ethread: bad inst!\n");
        up(&vchan_ctrl.inst_sem);
        return -1;
    }

    vchan_ctrl.num_instances++;
    inst->domain = domain;
    inst->port = port;
    inst->signaled = 1;
    inst->wait_type = VCHAN_RECV;
    inst->event_mon = mon;
    inst->self = self;

    /* Find the eventfd a user level vchan connection uses and link it in */
    userspace_task = pid_task(find_vpid(pid), PIDTYPE_PID);
    BUG_ON(userspace_task == NULL);

    rcu_read_lock();
    efd_file = fcheck_files(userspace_task->files, eventfd);
    rcu_read_unlock();

    BUG_ON(efd_file == NULL);

    inst->efd_ctx = eventfd_ctx_fileget(efd_file);
    BUG_ON(inst->efd_ctx == NULL);


    init_completion(&(inst->wait));

    /* Add to the instance list */
    INIT_LIST_HEAD(&inst->node);
    list_add(&inst->node, &vchan_ctrl.instances);

    up(&vchan_ctrl.inst_sem);

    return 0;
}

/*
    Disconnect a vchan instance from the event monitor
*/
void rem_event_instance(int domain, int port) {

    einstance_t *inst, *next;
    struct list_head *head = &vchan_ctrl.instances;

    down(&vchan_ctrl.inst_sem);

    list_for_each_entry_safe(inst, next, head, node) {
        if(inst->domain == domain && inst->port == port) {
            list_del(&inst->node);
            eventfd_ctx_put(inst->efd_ctx);
            /*
                FIXME this should be free'd
                     but causes a kernel panic for some reason right now
            */
            // kfree(inst->event_mon);
            kfree(inst);
            vchan_ctrl.num_instances--;
            up(&vchan_ctrl.inst_sem);
            return;
        }
    }

    up(&vchan_ctrl.inst_sem);
}

int have_data(int a) {
    if(a == VCHAN_BUF_DATA || a == VCHAN_BUF_FULL || a == VCHAN_CLOSED_DATA) {
        return 1;
    }

    return 0;
}

/*
    Check to see if the state of a given vchan is blocking or not
*/
int check_event(einstance_t *inst) {

    int a = inst->event_mon->alert;
    // printk("event_thread: checking stat... %d|%d|%d\n", inst->domain, a, inst->wait_type);

    if(inst->wait_type == VCHAN_RECV) {
        if(a != VCHAN_EMPTY_BUF) {
            if(have_data(a)) {
                return 1;
            }
            printk("ret -1 : %d\n", a);
            return -1;
        }
    } else {
        if(a != VCHAN_BUF_FULL) {
            if(a == VCHAN_CLOSED || a == VCHAN_CLOSED_DATA) {
                printk("ret -1 : %d\n", a);
                return -1;
            }
            return 1;
        }
    }

    return 0;
}

/*
    Top half interrupt handler for hypervisor state change interrupt
*/
void event_thread_run(struct work_struct *work) {

    int alert;
    einstance_t *inst, *next;
    struct list_head *head = &vchan_ctrl.instances;

    down(&vchan_ctrl.inst_sem);
        list_for_each_entry_safe(inst, next, head, node) {
            /* If a thread is waiting, see if its state is non-blocking */
            if(have_data(inst->event_mon->alert)) {
                // printk("setting eventfd_sig for %d\n", inst->self);
                eventfd_signal(inst->efd_ctx, plus_one);
            }

            alert = check_event(inst);
            if(alert != 0) {
                if(inst->signaled == 0) {
                    inst->signaled = 1;
                    complete(&(inst->wait));
                }
                if(alert == -1) {
                    list_del(&inst->node);
                    eventfd_ctx_put(inst->efd_ctx);
                    kfree(inst);
                }
            }
        }
    up(&vchan_ctrl.inst_sem);
}

/*
    Returns an event instance
*/
einstance_t *get_event_instance(int domain, int port) {

    einstance_t *inst, *next;
    struct list_head *head = &vchan_ctrl.instances;

    list_for_each_entry_safe(inst, next, head, node) {
        if(inst->port == port && inst->domain == domain) {
            up(&vchan_ctrl.inst_sem);
            return inst;
        }
    }

    return NULL;
}

/*
    Wait for a desired event to happen, blocking if it has not happened already
*/
int wait_for_event(int domain, int port, int type) {

    einstance_t *inst;
    int status = 0;

    down(&vchan_ctrl.inst_sem);

    inst = get_event_instance(domain, port);
    if(inst == NULL) {
        printk("event: failed to get inst\n");
        return -1;
    }

    inst->wait_type = type;
    status = check_event(inst);
    if(status < 0) {
        printk("event: bad status of %d\n", status);
    }


    /* Vchan is blocking, sleep until non-block */
    if(status == 0) {
        inst->signaled = 0;
        up(&vchan_ctrl.inst_sem);
        wait_for_completion(&(inst->wait));
        status = check_event(inst);
    } else {
        inst->signaled = 1;
        up(&vchan_ctrl.inst_sem);
    }

    return status;
}

/*
    Debug printing routine
*/
void print_inst(void) {
    einstance_t *inst, *next;
    struct list_head *head = &vchan_ctrl.instances;
    printk("--I %d--\n", vchan_ctrl.num_instances);
    list_for_each_entry_safe(inst, next, head, node) {
        printk("|INST|p:%d|d:%d|-", inst->port, inst->domain);

    }
    printk("\n-----\n");
}

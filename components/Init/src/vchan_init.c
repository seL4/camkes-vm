/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sel4/sel4.h>
#include <sel4utils/util.h>
#include <simple/simple.h>
#include <camkes/dataport.h>

#include <sel4vm/guest_vm.h>
#include <sel4vm/guest_memory.h>
#include <sel4vm/guest_ram.h>

#include "sel4vm/debug.h"
#include "sel4vm/vmm.h"
#include "sel4vm/vmm_manager.h"
#include "sel4vm/vchan_copy.h"
#include "sel4vm/vchan_component.h"
#include "sel4vm/vmcall.h"
#include "vm.h"

#include "i8259.h"

#include <camkes.h>

#define VCHAN_VM_MAX_ACTIVE_PORTS 25

static int vm_args(vm_t *vm, uintptr_t phys, void *vaddr, size_t size, size_t offset, void *cookie);
static int vchan_sync_copy(vm_t *vm, uintptr_t phys, void *vaddr, size_t size, size_t offset, void *cookie);

static void data_to_guest(vm_t *vm, uintptr_t phys_ptr, size_t size, void *buf);
static void *data_from_guest(vm_t *vm, uintptr_t phys_ptr, size_t size, void *buf);

static int driver_connect(vm_t *vm, void *data, uint64_t cmd);

static int vchan_connect(vm_t *vm, void *data, uint64_t cmd);
static int vchan_close(vm_t *vm, void *data, uint64_t cmd);
static int vchan_buf_state(vm_t *vm, void *data, uint64_t cmd);
static int vchan_readwrite(vm_t *vm, void *data, uint64_t cmd);
static int vchan_state(vm_t *vm, void *data, uint64_t cmd);

static int guest_vchan_init(int domain, int port, int server);

void vchan_vmcall_init();

/* Struct for managing copyin's/copyout's to a dataport via touch callback */
typedef struct vchan_copy_mem {
    void *buf;
    int copy_type;
    size_t copy_size;
} vchan_copy_mem_t;

/* Function lookup table for handling requests */
static struct vmm_manager_ops {
    int (*op_func[NUM_VMM_OPS])(vm_t *, void *, uint64_t);
} vmm_manager_ops_table = {
    .op_func[VMM_CONNECT]       =   &driver_connect,
    .op_func[SEL4_VCHAN_CONNECT]    =   &vchan_connect,
    .op_func[SEL4_VCHAN_CLOSE]      =   &vchan_close,
    .op_func[VCHAN_SEND]            =   &vchan_readwrite,
    .op_func[VCHAN_RECV]            =   &vchan_readwrite,
    .op_func[SEL4_VCHAN_BUF]        =   &vchan_buf_state,
    .op_func[SEL4_VCHAN_STATE]      =   &vchan_state,
};

static bool driver_connected = 0;
static char driver_arg[1024];
static vmcall_args_t driver_vmcall;

void *vchan_callback_addr_buffer[VCHAN_VM_MAX_ACTIVE_PORTS];

static camkes_vchan_con_t vchan_camkes_component;
static int have_vchan = 0;

void vchan_init_camkes(camkes_vchan_con_t vchan) {
    vchan_camkes_component = vchan;
    have_vchan = 1;
    init_camkes_vchan(&vchan_camkes_component);
    for(int i = 0; i < VCHAN_VM_MAX_ACTIVE_PORTS; i++) {
        vchan_callback_addr_buffer[i] = NULL;
    }
}

static int add_new_callback_addr(void *addr) {
    for(int i = 0; i < VCHAN_VM_MAX_ACTIVE_PORTS; i++) {
        if(vchan_callback_addr_buffer[i] == NULL) {
            vchan_callback_addr_buffer[i] = addr;
            return 0;
        }
    }
    return -1;
}

static int rem_callback_addr(void *addr) {
    for(int i = 0; i < VCHAN_VM_MAX_ACTIVE_PORTS; i++) {
        if(vchan_callback_addr_buffer[i] == addr) {
            vchan_callback_addr_buffer[i] = NULL;
            return 0;
        }
    }
    return -1;
}

void vchan_interrupt(vm_t *vm) {
    for(int i = 0; i < VCHAN_VM_MAX_ACTIVE_PORTS; i++) {
        if(vchan_callback_addr_buffer[i] != NULL) {
            vchan_alert_t in_alert;
            uintptr_t addr = (uintptr_t) vchan_callback_addr_buffer[i];
            data_from_guest(vm, addr, sizeof(vchan_alert_t), &in_alert);

            vchan_ctrl_t ct = {
                .domain = vchan_camkes_component.component_dom_num,
                .dest = in_alert.dest,
                .port = in_alert.port,
            };

            in_alert.alert = vchan_camkes_component.alert_status(ct);

            data_to_guest(vm, addr, sizeof(vchan_alert_t), &in_alert);
            i8259_gen_irq(VCHAN_EVENT_IRQ);
        }
    }
}


/*
    Return the given vm guest number of this component
*/
int get_vm_num() {
    int res;
    char *name = (char *) get_instance_name();
    int ret = sscanf(name, "vm.vm%d", &res);
    if(ret == 0) {
        printf("vchan_driver: failed to get run num\n");
        return -1;
    }

    return res;
}

/*
    Used to connect a client/server vchan running in a linux image
*/
static int guest_vchan_init(int domain, int port, int server) {
    vchan_connect_t t = {
        .v.domain = vchan_camkes_component.component_dom_num,
        .v.dest = domain,
        .v.port = port,
        .server = server,
    };

    vchan_camkes_component.connect(t);
    return 0;
}

/*
    Callback function for copying in data from guest memory, into vmm memory
        - The amount of memory expected is small

    This requires testing to make sure all arguments are copied correctly, with variable sizes
*/
static int vm_args(vm_t *vm, uintptr_t phys, void *vaddr, size_t size, size_t offset, void *cookie) {
    vchan_copy_mem_t *tok = (vchan_copy_mem_t *)cookie;
    if(tok->copy_type == 1) {
        memcpy(vaddr, (void *)tok->buf + offset, size);
    } else {
        memcpy((void *)tok->buf + offset, vaddr, size);
    }
    return 0;
}

/*
    Touch callback for copying data into and out of guest memory
*/
static int vchan_sync_copy(vm_t *vm, uintptr_t phys, void *vaddr, size_t size, size_t offset, void *cookie) {
    vchan_copy_mem_t *arg = (vchan_copy_mem_t *) cookie;
    if(arg->copy_size <= 0) {
        return 1;
    }

    size_t in_size = MIN(size, arg->copy_size);

    arg->copy_size -= size;

    if(arg->copy_type == VCHAN_SEND) {
        memcpy(arg->buf + offset, vaddr, in_size);
    } else {
        memcpy(vaddr, arg->buf + offset, in_size);
    }
    __sync_synchronize();

    return 0;
}

/*
    Return an address pointing to data inside the virtualised guest
*/
static void *data_from_guest(vm_t *vm, uintptr_t phys_ptr, size_t size, void *buf) {
    vchan_copy_mem_t tok;
    tok.buf = buf;
    tok.copy_type = 0;
    tok.copy_size = size;

    vm_ram_touch(vm, phys_ptr, size, &vm_args, &tok);

    return buf;
}

/*
    Return an address pointing to data inside the virtualised guest
*/
static void data_to_guest(vm_t *vm, uintptr_t phys_ptr, size_t size, void *buf) {
    vchan_copy_mem_t tok;
    tok.buf = buf;
    tok.copy_type = 1;
    tok.copy_size = size;

    vm_ram_touch(vm, phys_ptr, size, &vm_args, &tok);

}

/*
    Return the state of a given vchan connection
*/
static int vchan_state(vm_t *vm, void *data, uint64_t cmd) {
    vchan_check_args_t *args = (vchan_check_args_t *)data;

    args->v.domain = vchan_camkes_component.component_dom_num;
    args->state = vchan_camkes_component.status(args->v);

    return 0;
}


/*
    Function for sending/recieving data from other vm's
        Copies into a memory buffer, and then defers to VmmManager to finish the operation
        Defering is necessary for ensuring concurrency
*/
static int vchan_readwrite(vm_t *vm, void *data, uint64_t cmd) {

    vchan_copy_mem_t tok;
    vchan_args_t *args = (vchan_args_t *)data;
    vspace_t *vs = &vm->mem.vm_vspace;

    int *update;
    size_t size = args->size;
    uintptr_t phys = args->mmap_phys_ptr;

    vchan_ctrl_t bargs = {
        .domain = vchan_camkes_component.component_dom_num,
        .dest = args->v.dest,
        .port = args->v.port,
    };

    /* Perfom copy of data to appropriate destination */
    vchan_buf_t *b = get_vchan_buf(&bargs, &vchan_camkes_component, cmd);
    assert(b != NULL);

    size_t filled = abs(b->write_pos - b->read_pos);
    tok.copy_type = cmd;
    //printf("Write pos - %d, Read pos - %d, Filled - %d, Owner - %d\n", b->write_pos, b->read_pos, filled, b->owner);

    /*
        If streaming, send as much data as possible
         If not streaming, any operation that can't fit into the buffer fails
    */
    if(cmd == VCHAN_RECV) {
        if(args->stream) {
            size = MIN(filled, args->size);
        } else if(size > filled) {
            return -1;
        }
        update = &(b->read_pos);
    } else {
        if(args->stream) {
            size = MIN(VCHAN_BUF_SIZE - filled, args->size);
        } else if (size > (VCHAN_BUF_SIZE - filled)) {
            return -1;
        }
        update = &(b->write_pos);
    }


    off_t start = (*update % VCHAN_BUF_SIZE);
    off_t remain = 0;
    if(start + size > VCHAN_BUF_SIZE) {
        remain = (start + size) - VCHAN_BUF_SIZE;
        size -= remain;
    }

    tok.buf = (void *)b->sync_data + start;
    tok.copy_size = size;
    if(vm_ram_touch(vm, phys, size, &vchan_sync_copy, &tok) < 0) {
        printf("vmcall_readwrite: did not perform a good write!\n");
        return -1;
    }

    tok.buf = &b->sync_data;
    tok.copy_size = remain;
    if(vm_ram_touch(vm, phys + size, remain, &vchan_sync_copy, &tok) < 0) {
        printf("vmcall_readwrite: did not perform a good write!\n");
        return -1;
    }

    *update += (size + remain);
    filled = abs(b->write_pos - b->read_pos);
    vchan_camkes_component.alert();

    args->size = (size + remain);


    return 0;
}

/*
    See the state of a given vchan buffer
        i.e. how much data is in the buffer, how much can be written into the buffer
             - if NOWAIT_DATA_READY, return how much data is in the buffer to read
             - if NOWAIT_BUF_SPACE, return how much data can be read into buffer
*/
static int vchan_buf_state(vm_t *vm, void *data, uint64_t cmd) {
    vchan_check_args_t *args = (vchan_check_args_t *)data;

    vchan_ctrl_t bargs = {
        .domain = vchan_camkes_component.component_dom_num,
        .dest = args->v.dest,
        .port = args->v.port,
    };

    vchan_buf_t *b;
    if(args->checktype == NOWAIT_DATA_READY) {
        b = get_vchan_buf(&bargs, &vchan_camkes_component, VCHAN_RECV);
    } else { /* NOWAIT_BUF_SPACE */
        b = get_vchan_buf(&bargs, &vchan_camkes_component, VCHAN_SEND);
    }

    size_t filled = abs(b->write_pos - b->read_pos);
    if(args->checktype == NOWAIT_DATA_READY) {
        args->state = filled;
    } else {
        args->state = VCHAN_BUF_SIZE - filled;
    }

    return 0;
}

/*
    Connect a vchan to a another guest vm
*/
static int vchan_connect(vm_t *vm, void *data, uint64_t cmd) {
    vchan_connect_t *pass = (vchan_connect_t *)data;
    if(add_new_callback_addr((void *) pass->event_mon) != 0) {
        return -1;
    }

    //DPRINTF(4, "ADDING %x as ADDR\n", pass->event_mon);
    guest_vchan_init(pass->v.dest, pass->v.port, pass->server);

    return 0;
}

/*
    Close a vchan connection this guest vm is using
*/
static int vchan_close(vm_t *vm, void *data, uint64_t cmd) {
    vchan_connect_t *pass = (vchan_connect_t *)data;

    vchan_connect_t t = {
        .v.domain = vchan_camkes_component.component_dom_num,
        .v.dest = pass->v.dest,
        .v.port = pass->v.port,
        .server = pass->server,
        .event_mon = pass->event_mon,
    };

    vchan_camkes_component.disconnect(t);

    //DPRINTF(4, "REMOVING %x as ADDR\n", pass->event_mon);
    rem_callback_addr((void *) pass->event_mon);

    return 0;
}

/*
    Used for replying back to a driver successfully connecting
*/
static int driver_connect(vm_t *vm, void *data, uint64_t cmd) {
    /* Only allow one vchan driver instance to be connected */
    if(driver_connected)
        return -1;

    driver_connected = 1;

    struct vmm_args *vargs = (struct vmm_args *)data;
    vargs->datatype = DATATYPE_INT;
    int *res = (int *)vargs->ret_data;
    *res = get_vm_num();
    if(*res < 0) {
        return -1;
    }

    //DPRINTF(4, "vmcall: num is %d\n", *res);
    return 0;
}


/*
    VM Call VMM Handler
    - eax and ebx contain the reason for the vmcall,
        eax is the command, ebx is the guest physical address of arguments
    - Depending on the command and arguments, we perform the top level administration of the request
*/
int vchan_handler(vm_vcpu_t *vcpu) {
    void *data;
    int cmd;
    uintptr_t paddr = vmm_read_user_context(&vcpu->vcpu_arch.guest_state, USER_CONTEXT_EBX);

    /*
        Get the location of the arguments in virtual memory
            - vmcall_args contains a token that determines what component the request refers to (if any)
            - It also contains the physical address of the arguments for this request
            - Unrecognised tokens mean the request is rejected with an error
    */
    data_from_guest(vcpu->vm, paddr, sizeof(vmcall_args_t), &driver_vmcall);
    vmcall_args_t *args = (vmcall_args_t *) &driver_vmcall;
    cmd = args->cmd;
    if (!have_vchan) {
        args->err = -1;
        data_to_guest(vcpu->vm, paddr, sizeof(vmcall_args_t), &driver_vmcall);
        return 0;
    }

    /* Catch if the request is for an invalid command */
    if(cmd >= NUM_VMM_OPS || vmm_manager_ops_table.op_func[cmd] == NULL) {
        DPRINTF(2, "unsupported command %d\n", cmd);
        args->err = -1;
    } else {
        /* Perform given token:command action */
        data = data_from_guest(vcpu->vm, args->phys_data, args->size, (void *) driver_arg);
        args->err = (*vmm_manager_ops_table.op_func[cmd])(vcpu->vm, data, cmd );
        if(args->err != -1) {
            data_to_guest(vcpu->vm, args->phys_data, args->size, (void *) driver_arg);
        }
    }

    data_to_guest(vcpu->vm, paddr, sizeof(vmcall_args_t), &driver_vmcall);
    /* Return success */
    return 0;
}

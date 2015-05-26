/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sel4/sel4.h>
#include <sel4utils/util.h>
#include <simple/simple.h>
#include <camkes/dataport.h>

#include "vmm/debug.h"
#include "vmm/vmm.h"
#include "vmm/platform/guest_vspace.h"
#include "vmm/vmm_manager.h"
#include "vmm/vchan_copy.h"
#include "vmm/vchan_component.h"
#include "vmm/vmcall.h"
#include "vm.h"

#include "i8259.h"

#include <camkes.h>

static int vm_args(uintptr_t phys, void *vaddr, size_t size, size_t offset, void *cookie);
static int vchan_sync_copy(uintptr_t phys, void *vaddr, size_t size, size_t offset, void *cookie);

static void data_to_guest(vmm_t *vmm, uintptr_t phys_ptr, size_t size, void *buf);
static void *data_from_guest(vmm_t *vmm, uintptr_t phys_ptr, size_t size, void *buf);

static int driver_connect(vmm_t *vmm, void *data, uint64_t cmd);

static int vchan_connect(vmm_t *vmm, void *data, uint64_t cmd);
static int vchan_close(vmm_t *vmm, void *data, uint64_t cmd);
static int vchan_buf_state(vmm_t *vmm, void *data, uint64_t cmd);
static int vchan_readwrite(vmm_t *vmm, void *data, uint64_t cmd);
static int vchan_state(vmm_t *vmm, void *data, uint64_t cmd);

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
    int (*op_func[NUM_VMM_OPS])(vmm_t *, void *, uint64_t);
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
static void *vchan_callback_addr = NULL;
static camkes_vchan_con_t vchan_camkes_component;
static int have_vchan = 0;

void vchan_init_camkes(camkes_vchan_con_t vchan) {
    vchan_camkes_component = vchan;
    have_vchan = 1;
    init_camkes_vchan(&vchan_camkes_component);
}

void vchan_interrupt(vmm_t *vmm) {
    vchan_alert_t in_alert;
    void *addr = vchan_callback_addr;
    if (!addr) {
        return;
    }

    data_from_guest(vmm, (uintptr_t) addr, sizeof(vchan_alert_t), &in_alert);

    vchan_ctrl_t ct = {
        .domain = vchan_camkes_component.component_dom_num,
        .dest = in_alert.dest,
        .port = in_alert.port,
    };

    in_alert.alert = vchan_camkes_component.alert_status(ct);

    data_to_guest(vmm, (uintptr_t) addr, sizeof(vchan_alert_t), &in_alert);
    i8259_gen_irq(VCHAN_EVENT_IRQ);
}


/*
    Return the given vm guest number of this component
*/
int get_vm_num() {
    int res;
    char *name = (char *) get_instance_name();
    int ret = sscanf(name, "vm_vm%d", &res);
    if(ret == 0) {
        DPRINTF(2, "vchan_driver: failed to get run num\n");
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
static int vm_args(uintptr_t phys, void *vaddr, size_t size, size_t offset, void *cookie) {
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
static int vchan_sync_copy(uintptr_t phys, void *vaddr, size_t size, size_t offset, void *cookie) {
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
static void *data_from_guest(vmm_t *vmm, uintptr_t phys_ptr, size_t size, void *buf) {
    vchan_copy_mem_t tok;
    tok.buf = buf;
    tok.copy_type = 0;
    tok.copy_size = size;

    vspace_t *vs = &vmm->guest_mem.vspace;
    vmm_guest_vspace_touch(vs, phys_ptr, size, &vm_args, &tok);

    return buf;
}

/*
    Return an address pointing to data inside the virtualised guest
*/
static void data_to_guest(vmm_t *vmm, uintptr_t phys_ptr, size_t size, void *buf) {
    vchan_copy_mem_t tok;
    tok.buf = buf;
    tok.copy_type = 1;
    tok.copy_size = size;

    vspace_t *vs = &vmm->guest_mem.vspace;
    vmm_guest_vspace_touch(vs, phys_ptr, size, &vm_args, &tok);

}

/*
    Return the state of a given vchan connection
*/
static int vchan_state(vmm_t *vmm, void *data, uint64_t cmd) {
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
static int vchan_readwrite(vmm_t *vmm, void *data, uint64_t cmd) {

    vchan_copy_mem_t tok;
    vchan_args_t *args = (vchan_args_t *)data;
    vspace_t *vs = &vmm->guest_mem.vspace;

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

    size_t filled = abs(b->read_pos - b->write_pos);
    tok.copy_type = cmd;

    /*
        If streaming, send as much data as possible
         If not streaming, any operation that can't fit into the buffer fails
    */
    if(cmd == VCHAN_RECV) {
        if(args->stream) {
            args->size = MIN(filled, args->size);
        } else if(args->size > filled) {
            return -1;
        }
        update = &(b->read_pos);
    } else {
        if(args->stream) {
            args->size = MIN(VCHAN_BUF_SIZE - filled, args->size);
        } else if (args->size > (VCHAN_BUF_SIZE - filled)) {
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
    if(vmm_guest_vspace_touch(vs, phys, size, &vchan_sync_copy, &tok) < 0) {
        DPRINTF(2, "vmcall_readwrite: did not perform a good write!\n");
        return -1;
    }

    tok.buf = &b->sync_data;
    tok.copy_size = remain;
    if(vmm_guest_vspace_touch(vs, phys + size, remain, &vchan_sync_copy, &tok) < 0) {
        DPRINTF(2, "vmcall_readwrite: did not perform a good write!\n");
        return -1;
    }

    *update += (size + remain);
    filled = abs(b->read_pos - b->write_pos);
    vchan_camkes_component.alert();

    args->size = (size + remain);


    return 0;
}

/*
    See the state of a given vchan buffer
        i.e. how much data is in the buffer, how much can be written into the buffer
*/
static int vchan_buf_state(vmm_t *vmm, void *data, uint64_t cmd) {
    vchan_check_args_t *args = (vchan_check_args_t *)data;

    vchan_ctrl_t bargs = {
        .domain = vchan_camkes_component.component_dom_num,
        .dest = args->v.dest,
        .port = args->v.port,
    };

    /* Perfom copy of data to appropriate destination */
    vchan_buf_t *b = get_vchan_buf(&bargs, &vchan_camkes_component, cmd);


    size_t filled = abs(b->read_pos - b->write_pos);
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
static int vchan_connect(vmm_t *vmm, void *data, uint64_t cmd) {
    vmm_args_t *args = (vmm_args_t *)data;
    vchan_connect_t *pass = (vchan_connect_t *)args->ret_data;

    guest_vchan_init(pass->v.dest, pass->v.port, pass->server);
    vchan_callback_addr = (void*)pass->event_mon;

    return 0;
}

/*
    Close a vchan connection this guest vm is using
*/
static int vchan_close(vmm_t *vmm, void *data, uint64_t cmd) {
    panic("init-side vchan close not implemented!");
    // vmm_args_t *args = (vmm_args_t *)data;
    // vchan_connect_t *pass = (vchan_connect_t *)args->ret_data;
    // uint32_t domx = get_vm_num();
    // uint32_t domy = pass->v.domain;
    // ctrl_rem_vchan_connection(domx, domy, pass->server, pass->v.port);
    return 0;
}

/*
    Used for replying back to a driver successfully connecting
*/
static int driver_connect(vmm_t *vmm, void *data, uint64_t cmd) {
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

    DPRINTF(4, "vmcall: num is %d\n", *res);
    return 0;
}


/*
    VM Call VMM Handler
    - eax and ebx contain the reason for the vmcall,
        eax is the command, ebx is the guest physical address of arguments
    - Depending on the command and arguments, we perform the top level administration of the request
*/
int vchan_handler(vmm_vcpu_t *vcpu) {
    void *data;
    int cmd;
    uintptr_t paddr = vmm_read_user_context(&vcpu->guest_state, USER_CONTEXT_EBX);

    /*
        Get the location of the arguments in virtual memory
            - vmcall_args contains a token that determines what component the request refers to (if any)
            - It also contains the physical address of the arguments for this request
            - Unrecognised tokens mean the request is rejected with an error
    */
    data_from_guest(vcpu->vmm, paddr, sizeof(vmcall_args_t), &driver_vmcall);
    vmcall_args_t *args = (vmcall_args_t *) &driver_vmcall;
    cmd = args->cmd;
    if (!have_vchan) {
        args->err = -1;
        data_to_guest(vcpu->vmm, paddr, sizeof(vmcall_args_t), &driver_vmcall);
        return 0;
    }

    /* Catch if the request is for an invalid command */
    if(cmd >= NUM_VMM_OPS || vmm_manager_ops_table.op_func[cmd] == NULL) {
        DPRINTF(2, "unsupported command %d\n", cmd);
        args->err = -1;
    } else {
        /* Perform given token:command action */
        data = data_from_guest(vcpu->vmm, args->phys_data, args->size, (void *) driver_arg);
        args->err = (*vmm_manager_ops_table.op_func[cmd])(vcpu->vmm, data, cmd );
        if(args->err != -1) {
            data_to_guest(vcpu->vmm, args->phys_data, args->size, (void *) driver_arg);
        }
    }

    data_to_guest(vcpu->vmm, paddr, sizeof(vmcall_args_t), &driver_vmcall);
    /* Return success */
    return 0;
}

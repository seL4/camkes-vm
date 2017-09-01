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
#include <vmm_utils.h>
#include <vmm_manager.h>

#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>


int ioctl_intf(int cmd, int fd, void *data, unsigned size) {
    assert(size != 4);
    return ioctl(fd, VM_IOCTL_CMD(cmd, struct vmm_args), data);
}


int num_of_vms(void) {
    vmm_args_t args;
    int fd, res;
    int *store;

    if ((fd = open("/dev/vmm_manager", O_RDWR)) < 0) {
        return -1;
    }

    res = ioctl(fd, VM_IOCTL_CMD(VMM_NUM_OF_DOM, vmm_args_t), &args);
    if (res < 0)
        return -1;
    store = (int *)args.ret_data;
    return *store;

}

int vm_number(void) {
    struct vmm_args args;
    int fd, res;
    int *store;

    if ((fd = open("/dev/vmm_manager", O_RDWR)) < 0) {
        return -1;
    }

    res = ioctl(fd, VM_IOCTL_CMD(VMM_GUEST_NUM, struct vmm_args), &args);

    if (res < 0)
        return -1;
    store = (int *)args.ret_data;
    return *store;
}


// int sleep_vm(void) {

//     struct vmm_args args;
//     int fd, res;

//     if ((fd = open("/dev/vmm_manager", O_RDWR)) < 0) {
//         return -1;
//     }

//     res = ioctl(fd, VM_IOCTL_CMD(VCHAN_SLEEP, struct vmm_args), &args);

//     if (res < 0)
//         return -1;

//     return 0;
// }

// int wakeup_vm(uint32_t vm) {
//     int fd, res;

//     if ((fd = open("/dev/vmm_manager", O_RDWR)) < 0) {
//         return -1;
//     }

//     res = ioctl(fd, VM_IOCTL_CMD(VCHAN_WAKEUP, uint32_t), &vm);
//     if (res < 0)
//         return -1;

//     return 0;
// }

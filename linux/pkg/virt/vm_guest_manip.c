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
#include <vmm_manager.h>
#include <libvchan.h>
#include <vmm_utils.h>
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

#define VM_SLEEP 0
#define VM_WAKE 1
#define VM_NUM 2

typedef struct arg {
    char arg_name[32];
    int arg_num;
} vm_arg_t;

static vm_arg_t valid_args[] = {
    { .arg_name = "num", .arg_num = VM_NUM },
};

#define NUM_ARGS sizeof(valid_args) / sizeof(vm_arg_t)

char astr[] = "guest_vm_action (sleep|wakeup|num) -n=\n";

void usage() {
    printf(astr, sizeof(astr));
    exit(1);
}

void error() {
	printf("error encountered on operation\n");
	exit(1);
}

int search_args(char *arg) {
    int x;
    char *name;

    for(x = 0; x < NUM_ARGS; x++) {
        name = valid_args[x].arg_name;
        if(strcmp(arg, name) == 0) {
            return valid_args[x].arg_num;
        }
    }

    return -1;
}

int main(int argc, char *argv[]) {

    if(argc < 2) {
        printf("Invalid argument count");
        usage();
    } else {
        switch (search_args(argv[1])) {
            // case VM_SLEEP:
            	// if(sleep_vm() < 0)
            		// error();
                // break;
            case VM_NUM:
                printf("%d", vm_number());
                break;
            // case VM_WAKE:
                // if(argc < 3)
                    // usage();
            	// if(wakeup_vm((int) argv[2]) < 0)
            		// error();
                // break;
            default:
                usage();
                printf("Invalid args");
        }
    }

    return 0;
}

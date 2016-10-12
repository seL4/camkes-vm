/*
 * Copyright 2016, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(D61_BSD)
 */

#include <cross_vm_dataport.h>
#include <vmm/vmm.h>

// XXX these must match the token and commands in the dataport linux kernel module
#define DATAPORT_VMCALL_HANDLER_TOKEN 1
#define DATAPORT_CMD_SHARE      1

static cross_vm_dataport_handle_t **dataports;
static unsigned int num_dataports;

static int dataport_map_guest(cross_vm_dataport_handle_t *dataport, void *guest_paddr, size_t size,
                              guest_memory_t *guest_memory) {

    printf("aa\n");
    size_t host_size = cross_vm_dataport_get_size(dataport);
    if (size != host_size) {
        printf("Dataport guest size and host size are different (%x and %x)\n", size, host_size);
        return -1;
    }

    vspace_t *guest_vspace = &guest_memory->vspace;
    unsigned int num_frames = cross_vm_dataport_get_num_frame_caps(dataport);
    seL4_CPtr *frames = cross_vm_dataport_get_frame_caps(dataport);

    printf("Mapping %x bytes to guest paddr %p\n", size, guest_paddr);

    for (int i = 0; i < num_frames; i++) {
        printf("0x%x\n", frames[i]);
    }

    vspace_unmap_pages(guest_vspace, guest_paddr, num_frames, PAGE_BITS_4K, VSPACE_PRESERVE);

    reservation_t res = vspace_reserve_range_at(guest_vspace, guest_paddr, size, seL4_AllRights,
                                                0 /* cacheable */);

    int error = vspace_map_pages_at_vaddr(guest_vspace, frames, NULL /* cookies */, guest_paddr, num_frames,
                              PAGE_BITS_4K, res);
    if (error) {
        return error;
    }

    vspace_free_reservation(guest_vspace, res);

    return 0;
}

static int dataport_vmcall_handler(vmm_vcpu_t *vcpu) {
    int error;
    int cmd = vmm_read_user_context(&vcpu->guest_state, USER_CONTEXT_EBX);
    unsigned int dataport_id = vmm_read_user_context(&vcpu->guest_state, USER_CONTEXT_ECX);

    printf("received hypercall %d %d\n", cmd, dataport_id);

    if (dataport_id == 0) {
        printf("Illegal dataport id 0\n");
        return -1;
    }

    if (dataport_id >= num_dataports) {
        printf("Dataport %d not found\n", dataport_id);
        return -1;
    }

    cross_vm_dataport_handle_t *dataport = dataports[dataport_id];

    switch (cmd) {
    case DATAPORT_CMD_SHARE: {
        void *guest_paddr = (void*)vmm_read_user_context(&vcpu->guest_state, USER_CONTEXT_EDX);
        size_t size = (size_t)vmm_read_user_context(&vcpu->guest_state, USER_CONTEXT_ESI);
        printf("mapping into guest\n");
        error = dataport_map_guest(dataport, guest_paddr, size, &vcpu->vmm->guest_mem);
        if (error) {
            printf("Failed to map dataport into guest\n");
            return error;
        }
        break;
    }
    default: {
        ZF_LOGE("Unknown command: %d", cmd);
        return -1;
    }
    }

    printf("returning\n");

    return 0;
}

int cross_vm_dataports_init_common(vmm_t *vmm, cross_vm_dataport_handle_t **d, int n) {
    dataports = d;
    num_dataports = n;
    int error = reg_new_handler(vmm, &dataport_vmcall_handler, DATAPORT_VMCALL_HANDLER_TOKEN);
    assert(!error);
}

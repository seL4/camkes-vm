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

#include <cross_vm_shared_dataport.h>
#include <dataport_caps.h>
#include <vmm/vmm.h>

static dataport_caps_handle_t **dataports;
static unsigned int num_dataports;

static int dataport_map_guest(dataport_caps_handle_t *dataport, void *guest_paddr, size_t size,
                              guest_memory_t *guest_memory) {

    size_t host_size = dataport_get_size(dataport);
    if (size != host_size) {
        ZF_LOGE("Dataport guest size and host size are different (%x and %x)", size, host_size);
        return -1;
    }

    vspace_t *guest_vspace = &guest_memory->vspace;
    unsigned int num_frames = dataport_get_num_frame_caps(dataport);
    seL4_CPtr *frames = dataport_get_frame_caps(dataport);

    ZF_LOGI("Mapping %x bytes to guest paddr %p", size, guest_paddr);

    vspace_unmap_pages(guest_vspace, guest_paddr, num_frames, PAGE_BITS_4K, VSPACE_PRESERVE);

    reservation_t res = vspace_reserve_range_at(guest_vspace, guest_paddr, size, seL4_AllRights,
                                                1 /* cacheable */);

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

    if (dataport_id == 0) {
        ZF_LOGE("Illegal dataport id 0");
        return -1;
    }

    if (dataport_id >= num_dataports) {
        ZF_LOGE("Dataport %d not found", dataport_id);
        return -1;
    }

    dataport_caps_handle_t *dataport = dataports[dataport_id];

    switch (cmd) {
    case DATAPORT_CMD_SHARE: {
        void *guest_paddr = (void*)vmm_read_user_context(&vcpu->guest_state, USER_CONTEXT_EDX);
        size_t size = (size_t)vmm_read_user_context(&vcpu->guest_state, USER_CONTEXT_ESI);
        error = dataport_map_guest(dataport, guest_paddr, size, &vcpu->vmm->guest_mem);
        if (error) {
            ZF_LOGE("Failed to map dataport into guest");
            return error;
        }
        break;
    }
    default: {
        ZF_LOGE("Unknown command: %d", cmd);
        return -1;
    }
    }

    return 0;
}

int cross_vm_dataports_init_common(vmm_t *vmm, dataport_caps_handle_t **d, int n) {
    dataports = d;
    num_dataports = n;
    int error = reg_new_handler(vmm, &dataport_vmcall_handler, DATAPORT_VMCALL_HANDLER_TOKEN);
    assert(!error);
}

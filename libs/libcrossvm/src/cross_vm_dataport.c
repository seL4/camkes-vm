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

#include "cross_vm_shared/cross_vm_shared_dataport.h"
#include "dataport_caps.h"

#include <sel4vm/guest_vm.h>
#include <sel4vm/guest_memory.h>
#include <sel4vm/guest_memory_helpers.h>

#include <sel4vm/arch/guest_x86_context.h>
#include <sel4vm/arch/vmcall.h>

#include <sel4vmmplatsupport/guest_memory_util.h>
#include <sel4vmmplatsupport/drivers/virtio.h>
#include <sel4vmmplatsupport/drivers/pci_helper.h>
#include <pci/helper.h>

#define MAX_NUM_DATAPORTS 6

struct dataport_info {
    uintptr_t address;
    size_t size_bits;
};

static dataport_caps_handle_t **dataports;
static unsigned int num_dataports;

static int construct_dataport_virtio_bar(vm_t *vm, struct dataport_info *info, int num_dataports, vmm_pci_space_t *pci)
{
    vmm_pci_device_def_t *pci_config;
    pci_config = calloc(1, sizeof(*pci_config));
    ZF_LOGF_IF(pci_config == NULL, "Failed to allocate pci config");
    *pci_config = (vmm_pci_device_def_t) {
        .vendor_id = 0x1af4,
        .device_id = 0xa111,
        .revision_id = 0x1,
        .subsystem_vendor_id = 0x0,
        .subsystem_id = 0x0,
        .command = PCI_COMMAND_IO | PCI_COMMAND_MEMORY,
        .header_type = PCI_HEADER_TYPE_NORMAL,
        .subclass = (PCI_CLASS_MEMORY_RAM >> 8) & 0xff,
        .class_code = (PCI_CLASS_MEMORY_RAM >> 16) & 0xff,
    };

    uint32_t *pci_config_bars = (uint32_t *) & (pci_config->bar0);
    for (int i = 0; i < num_dataports; i++) {
        pci_config_bars[i] = info[i].address | PCI_BASE_ADDRESS_SPACE_MEMORY;
    }

    vmm_pci_entry_t entry = (vmm_pci_entry_t) {
        .cookie = pci_config,
        .ioread = vmm_pci_mem_device_read,
        .iowrite = vmm_pci_mem_device_write
    };

    vmm_pci_bar_t *bars = calloc(num_dataports, sizeof(vmm_pci_bar_t));
    if (!bars) {
        ZF_LOGE("Failed to allocate bar data");
        return -1;
    }
    for (int i = 0; i < num_dataports; i++) {
        bars[i].mem_type = PREFETCH_MEM;
        bars[i].address = info[i].address;
        bars[i].size_bits = info[i].size_bits;
    }
    vmm_pci_entry_t virtio_pci_bar;
    virtio_pci_bar = vmm_pci_create_bar_emulation(entry, num_dataports, bars);
    vmm_pci_add_entry(pci, virtio_pci_bar, NULL);
    free(bars);
    return 0;
}

struct dataport_iterator_cookie {
    seL4_CPtr *dataport_frames;
    uintptr_t dataport_start;
    size_t dataport_size;
    vm_t *vm;
};

static vm_frame_t dataport_memory_iterator(uintptr_t addr, void *cookie)
{
    int error;
    cspacepath_t return_frame;
    vm_frame_t frame_result = { seL4_CapNull, seL4_NoRights, 0, 0 };
    struct dataport_iterator_cookie *dataport_cookie = (struct dataport_iterator_cookie *)cookie;
    seL4_CPtr *dataport_frames = dataport_cookie->dataport_frames;
    vm_t *vm = dataport_cookie->vm;
    uintptr_t dataport_start = dataport_cookie->dataport_start;
    size_t dataport_size = dataport_cookie->dataport_size;
    int page_size = seL4_PageBits;

    uintptr_t frame_start = ROUND_DOWN(addr, BIT(page_size));
    if (frame_start <  dataport_start ||
        frame_start > dataport_start + dataport_size) {
        ZF_LOGE("Error: Not Dataport region");
        return frame_result;
    }
    int page_idx = (frame_start - dataport_start) / BIT(page_size);
    frame_result.cptr = dataport_frames[page_idx];
    frame_result.rights = seL4_AllRights;
    frame_result.vaddr = frame_start;
    frame_result.size_bits = page_size;
    return frame_result;
}

static int reserve_dataport_memory(vm_t *vm, dataport_caps_handle_t **d, int n, struct dataport_info *info)
{
    int err;
    for (int i = 0; i < n; i++) {
        size_t size = dataport_get_size(d[i]);
        unsigned int num_frames = dataport_get_num_frame_caps(d[i]);
        seL4_CPtr *frames = dataport_get_frame_caps(d[i]);
        uintptr_t dataport_addr;

        vm_memory_reservation_t *dataport_reservation = vm_reserve_anon_memory(vm, size, default_error_fault_callback, NULL,
                                                                               &dataport_addr);
        if (!dataport_reservation) {
            ZF_LOGE("Failed to reserve dataport (id:%d) memory", i);
            return -1;
        }

        struct dataport_iterator_cookie *dataport_cookie = malloc(sizeof(struct dataport_iterator_cookie));
        if (!dataport_cookie) {
            ZF_LOGE("Failed to allocate dataport (id:%d) iterator cookie", i);
            return -1;
        }
        dataport_cookie->vm = vm;
        dataport_cookie->dataport_frames = frames;
        dataport_cookie->dataport_start = dataport_addr;
        dataport_cookie->dataport_size = size;
        err = vm_map_reservation(vm, dataport_reservation, dataport_memory_iterator, (void *)dataport_cookie);
        if (err) {
            ZF_LOGE("Failed to map dataport (id:%d) memory", i);
            return -1;
        }
        info[i].address = dataport_addr;
        info[i].size_bits = BYTES_TO_SIZE_BITS(size);
    }
    return 0;
}

int cross_vm_dataports_init_common(vm_t *vm, dataport_caps_handle_t **d, int n, vmm_pci_space_t *pci)
{
    dataports = d;
    num_dataports = n;
    uintptr_t guest_paddr = 0;
    size_t guest_size = 0;
    if (n > 6) {
        ZF_LOGE("Unable to register more than 6 dataports");
        return -1;
    }
    struct dataport_info info[MAX_NUM_DATAPORTS];
    int err = reserve_dataport_memory(vm, d, n, info);
    if (err) {
        ZF_LOGE("Failed to reserve memory for dataports");
        return -1;
    }
    err = construct_dataport_virtio_bar(vm, info, n, pci);
    if (err) {
        ZF_LOGE("Failed to construct pci device for dataports");
        return -1;
    }
    return 0;
}

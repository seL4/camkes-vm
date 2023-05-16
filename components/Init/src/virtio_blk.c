/*
 * Copyright 2019, Dornerworks
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <autoconf.h>

#include <sel4platsupport/arch/io.h>
#include <sel4utils/vspace.h>
#include <sel4utils/iommu_dma.h>
#include <simple/simple_helpers.h>
#include <vka/capops.h>

#include <camkes.h>
#include <camkes/dataport.h>

#include <virtio/virtio_pci.h>
#include <virtio/virtio_net.h>
#include <virtio/virtio_ring.h>
#include <virtio/virtio_blk.h>

#include <sel4vm/guest_vm.h>
#include <sel4vm/guest_memory.h>
#include <sel4vm/guest_irq_controller.h>
#include <sel4vm/boot.h>

#include <sel4vmmplatsupport/drivers/pci.h>
#include <sel4vmmplatsupport/drivers/virtio_pci_emul.h>
#include <sel4vmmplatsupport/drivers/virtio_blk.h>

#include "vm.h"
#include "virtio_blk.h"
#include "virtio_irq.h"

#define VIRTIO_VENDOR_ID            0x1af4
#define VIRTIO_DEVICE_ID            0x1001

#define VIRTIO_BLK_IOBASE           0x8000
#define VIRTIO_QUEUE_SIZE           128
#define VIRTIO_BLK_DISK_BLK_SIZE    512
#define VIRTIO_BLK_SIZE_MAX         4096
#define VIRTIO_BLK_SEG_MAX          1

#define CACHE_LINE_SIZE             64
#define LATENCY_TIMER               64
#define SUBSYSTEM_ID                2

/* Global pointer for the virtio_blk interface structure
 * Doesn't actually get used, but memory space is filled with proper values */
static virtio_blk_t *virtio_blk = NULL;
static vm_t *emul_vm = NULL;

#define SATASERVER_STATUS_GOOD          0
#define SATASERVER_STATUS_NOT_DONE      1
#define SATASERVER_STATUS_INVALID_CONF  2

/*
 *    These functions have the WEAK declaration because of the CAmkES protocol
 *    requirements. Virtio_blk uses the Sataserver component with interface functions (declared as
 *    "sataserver") so the compiler needs these so the build doesn't fail; however, the "weak"
 *    attribute means they are overridden by the proper functions (tx, rx, get_capacity)
 */
volatile Buf *sataserver_iface_buf WEAK;

int WEAK sataserver_iface_tx(unsigned int sector, unsigned int len)
{
    assert(!"should not be here");
    return 0;
}

int WEAK sataserver_iface_rx(unsigned int sector, unsigned int len)
{
    assert(!"should not be here");
    return 0;
}

unsigned int WEAK sataserver_iface_get_capacity(void)
{
    assert(!"should not be here");
    return 0;
}

unsigned int WEAK sataserver_iface_get_status(void)
{
    assert(!"should not be here");
    return 0;
}

/*
 * Purpose: Determine the transfer direction (In/Out) and either read/write from the Dataport.
 *          If the transfer failed, set the return variable accordingly
 *
 * Inputs:
 *   - *driver: disk information
 *   - direction: read/write to the disk
 *   - sector: sector to manipulate (512 byte offset)
 *   - len: number of bytes
 *   - guest_buf_phys: physical buffer to share data with
 *
 * Returns: TX_COMPLETE or TX_FAILED based on the transfer status
 *
 */
static int virtio_blk_emul_transfer(struct disk_driver *driver, uint8_t direction, uint64_t sector,
                                    uint32_t len, uintptr_t guest_buf_phys)
{
    int status = 0;  /* Variable to ensure transfer succeeded */

    switch (direction) {
    case VIRTIO_BLK_T_IN:
        status = sataserver_iface_rx(sector, len);
        memcpy((void *)guest_buf_phys, (void *)sataserver_iface_buf, len);
        break;
    case VIRTIO_BLK_T_OUT:
        memcpy((void *)sataserver_iface_buf, (void *)guest_buf_phys, len);
        status = sataserver_iface_tx(sector, len);
        break;
    case VIRTIO_BLK_T_SCSI_CMD:
    case VIRTIO_BLK_T_FLUSH:
    case VIRTIO_BLK_T_GET_ID:
        /* Do nothing */
        break;
    default:
        ZF_LOGE("virtio_blk: Invalid command (%d)", direction);
        break;
    }

    return (status != 0 ? VIRTIO_BLK_XFER_COMPLETE : VIRTIO_BLK_XFER_FAILED);
}

/*
 * Purpose: Handle an Interrupt (May not be necessary...)
 *
 * Inputs:
 *   - *driver: disk information
 *   - irq: interrupt to trigger
 *
 * Returns: void
 *
 */
static void emul_raw_handle_irq(struct disk_driver *driver, int irq)
{
    vm_inject_irq(emul_vm->vcpus[BOOT_VCPU], VIRTIO_BLK_IRQ);
}

/*
 * Purpose: Configure the virtio blk structure's capacity, max segments, and size.
 *
 * Inputs:
 *   - *driver: disk information
 *   - *cfg: virtio blk configuration structure pointer
 *
 * Returns: void
 *
 */
static void emul_low_level_init(struct disk_driver *driver, struct virtio_blk_config *cfg)
{
    cfg->capacity = sataserver_iface_get_capacity();
    cfg->seg_max = VIRTIO_BLK_SEG_MAX;
    cfg->size_max = VIRTIO_BLK_SIZE_MAX;
    cfg->blk_size = VIRTIO_BLK_DISK_BLK_SIZE;
}

/* Interrupt Function Handler for Virtual Machine Monitor - Doesn't really do anything since we're polling */
UNUSED void virtio_blk_notify(vm_t *vm)
{
    // This is where handler code would go
}

/* Initialization Function for Virtio Blk */
void make_virtio_blk(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports)
{
    unsigned int status = sataserver_iface_get_status();

    while (SATASERVER_STATUS_NOT_DONE == status) {
        status = sataserver_iface_get_status();
    }

    if (SATASERVER_STATUS_INVALID_CONF == status) {
        ZF_LOGF("Invalid partition configuration");
        return;
    }

    raw_diskiface_funcs_t backend = virtio_blk_default_backend();
    backend.raw_xfer = virtio_blk_emul_transfer;
    backend.low_level_init = emul_low_level_init;
    backend.raw_handleIRQ = emul_raw_handle_irq,
    emul_vm = vm;

    ioport_range_t virtio_port_range = {VIRTIO_BLK_IOBASE, VIRTIO_BLK_IOBASE + VIRTIO_IOPORT_SIZE, VIRTIO_IOPORT_SIZE};
    virtio_blk = common_make_virtio_blk(vm, pci, io_ports, virtio_port_range, IOPORT_ADDR,
                                        VIRTIO_BLK_IRQ, VIRTIO_BLK_IRQ, backend);

    assert(virtio_blk);
}

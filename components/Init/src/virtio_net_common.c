/*
 * Copyright 2018, Data61
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
#include <stdint.h>
#include <string.h>
#include <autoconf.h>

#include <sel4platsupport/arch/io.h>
#include <sel4utils/vspace.h>
#include <sel4utils/iommu_dma.h>
#include <simple/simple_helpers.h>
#include <vka/capops.h>
#include <utils/util.h>

#include <camkes.h>
#include <camkes/dataport.h>

#include <ethdrivers/virtio/virtio_pci.h>
#include <ethdrivers/virtio/virtio_net.h>
#include <ethdrivers/virtio/virtio_ring.h>

#include "vmm/vmm.h"
#include "vmm/driver/pci_helper.h"
#include "vmm/driver/virtio_emul.h"
#include "vmm/platform/ioports.h"
#include "vmm/platform/guest_vspace.h"

#include "virtio_net.h"
#include "vm.h"
#include "i8259.h"

#define VIRTIO_VID 0x1af4
#define VIRTIO_DID_START 0x1000

#define QUEUE_SIZE 128


static virtio_net_t *virtio_net = NULL;

static int virtio_net_io_in(void *cookie, unsigned int port_no, unsigned int size, unsigned int *result) {
    virtio_net_t *net = (virtio_net_t*)cookie;
    unsigned int offset = port_no - net->iobase;
    return net->emul->io_in(net->emul, offset, size, result);
}

static int virtio_net_io_out(void *cookie, unsigned int port_no, unsigned int size, unsigned int value) {
    int ret;
    virtio_net_t *net = (virtio_net_t*)cookie;
    unsigned int offset = port_no - net->iobase;
    ret = net->emul->io_out(net->emul, offset, size, value);
    return ret;
}

static int emul_raw_tx(struct eth_driver *driver, unsigned int num, uintptr_t *phys, unsigned int *len, void *cookie) {
    ZF_LOGF("not implemented");
}

static void emul_raw_handle_irq(struct eth_driver *driver, int irq) {
    i8259_gen_irq(6);
}

static void emul_raw_poll(struct eth_driver *driver) {
    ZF_LOGF("not implemented");
}

static void emul_low_level_init(struct eth_driver *driver, uint8_t *mac, int *mtu) {
    ZF_LOGF("not implemented");
}

static void emul_print_state(struct eth_driver* driver) {
    ZF_LOGF("not implemented");
}

static struct raw_iface_funcs emul_driver_funcs = {
    .raw_tx = emul_raw_tx,
    .raw_handleIRQ = emul_raw_handle_irq,
    .raw_poll = emul_raw_poll,
    .print_state = emul_print_state,
    .low_level_init = emul_low_level_init
};

static int emul_driver_init(struct eth_driver *driver, ps_io_ops_t io_ops, void *config) {
    virtio_net_t *net = (virtio_net_t*)config;
    driver->eth_data = config;
    driver->dma_alignment = sizeof(uintptr_t);
    driver->i_fn = net->emul_driver_funcs;
    net->emul_driver = driver;
    return 0;
}

static void* malloc_dma_alloc(void *cookie, size_t size, int align, int cached, ps_mem_flags_t flags) {
    assert(cached);
    int error;
    void *ret;
    error = posix_memalign(&ret, align, size);
    if (error) {
        return NULL;
    }
    return ret;
}

static void malloc_dma_free(void *cookie, void *addr, size_t size){
    free(addr);
}

static uintptr_t malloc_dma_pin(void *cookie, void *addr, size_t size) {
    return (uintptr_t)addr;
}

static void malloc_dma_unpin(void *cookie, void *addr, size_t size) {
}

static void malloc_dma_cache_op(void *cookie, void *addr, size_t size, dma_cache_op_t op) {
}


struct raw_iface_funcs virtio_net_default_backend() {
    return emul_driver_funcs;
}

static vmm_pci_entry_t vmm_virtio_net_pci_bar(unsigned int iobase) {
    vmm_pci_device_def_t *pci_config = malloc(sizeof(*pci_config));
    assert(pci_config);
    memset(pci_config, 0, sizeof(*pci_config));
    *pci_config = (vmm_pci_device_def_t) {
        .vendor_id = VIRTIO_VID,
        .device_id = VIRTIO_DID_START,
        .cache_line_size = 64,
        .latency_timer = 64,
        .subsystem_id = 1,
        .interrupt_pin = 6,
        .interrupt_line = 6
    };
    vmm_pci_entry_t entry = (vmm_pci_entry_t) {
        .cookie = pci_config,
        .ioread = vmm_pci_mem_device_read,
        .iowrite = vmm_pci_entry_ignore_write
    };
    vmm_pci_bar_t bars[1] = {{
        .ismem = 0,
        .address = iobase,
        .size_bits = 6
    }};
    return vmm_pci_create_bar_emulation(entry, 1, bars);

}

virtio_net_t *common_make_virtio_net(vmm_t *vmm, unsigned int iobase, struct raw_iface_funcs backend) {
    vmm_pci_entry_t entry = vmm_virtio_net_pci_bar(iobase);
    vmm_pci_add_entry(&vmm->pci, entry, NULL);
    virtio_net_t *net = malloc(sizeof(*net));
    assert(net);
    memset(net, 0, sizeof(*net));
    net->iobase = iobase;
    vmm_io_port_add_handler(&vmm->io_port, iobase, iobase + MASK(6), net, virtio_net_io_in, virtio_net_io_out, "VIRTIO PCI NET");

    ps_io_ops_t ioops;
	ioops.dma_manager = (ps_dma_man_t) {
        .cookie = NULL,
        .dma_alloc_fn = malloc_dma_alloc,
        .dma_free_fn = malloc_dma_free,
        .dma_pin_fn = malloc_dma_pin,
        .dma_unpin_fn = malloc_dma_unpin,
        .dma_cache_op_fn = malloc_dma_cache_op
    };

    net->emul_driver_funcs = backend;
    net->emul = ethif_virtio_emul_init(ioops, QUEUE_SIZE, &vmm->guest_mem.vspace, emul_driver_init, net);
    assert(net->emul);
    return net;
}

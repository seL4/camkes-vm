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

#include "vm.h"
#include "i8259.h"

#define VIRTIO_VID 0x1af4
#define VIRTIO_DID_START 0x1000

#define QUEUE_SIZE 128

volatile Buf*__attribute__((weak)) ethdriver_buf;

int __attribute__((weak)) ethdriver_tx(int len) {
    ZF_LOGF("should not be here");
    return 0;
}

int __attribute__((weak)) ethdriver_rx(int *len) {
    ZF_LOGF("should not be here");
    return 0;
}

void __attribute__((weak)) ethdriver_mac(uint8_t *b1, uint8_t *b2, uint8_t *b3, uint8_t *b4, uint8_t *b5, uint8_t *b6) {
    ZF_LOGF("should not be here");
}

int __attribute__((weak)) eth_rx_ready_reg_callback(void (*proc)(void*),void *blah) {
    ZF_LOGF("should not be here");
    return 0;
}

typedef struct virtio_net {
    unsigned int iobase;
    ethif_virtio_emul_t *emul;
    struct eth_driver *emul_driver;
    ps_io_ops_t ioops;
} virtio_net_t;

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
    size_t tot_len = 0;
    char *p = (char*)ethdriver_buf;
    /* copy to the data port */
    for (int i = 0; i < num; i++) {
        memcpy(p + tot_len, (void*)phys[i], len[i]);
        tot_len += len[i];
    }
    ethdriver_tx(tot_len);
    return ETHIF_TX_COMPLETE;
}

static void emul_raw_handle_irq(struct eth_driver *driver, int irq) {
    i8259_gen_irq(6);
}

static void emul_raw_poll(struct eth_driver *driver) {
    ZF_LOGF("not implemented");
}

static void emul_low_level_init(struct eth_driver *driver, uint8_t *mac, int *mtu) {
    ethdriver_mac(&mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
    *mtu = 1500;
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
    driver->i_fn = emul_driver_funcs;
    net->emul_driver = driver;
    return 0;
}

void virtio_net_notify(vmm_t *vmm) {
    int len;
    int status;
    status = ethdriver_rx(&len);
    while (status != -1) {
        void *cookie;
        void *emul_buf = (void*)virtio_net->emul_driver->i_cb.allocate_rx_buf(virtio_net->emul_driver->cb_cookie, len, &cookie);
        if (emul_buf) {
            memcpy(emul_buf, (void*)ethdriver_buf, len);
            virtio_net->emul_driver->i_cb.rx_complete(virtio_net->emul_driver->cb_cookie, 1, &cookie, (unsigned int*)&len);
        }
        if (status == 1) {
            status = ethdriver_rx(&len);
        } else {
            /* if status is 0 we already saw the last packet */
            assert(status == 0);
            status = -1;
        }
    }
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

void make_virtio_net(vmm_t *vmm) {
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
        .address = 0x9000,
        .size_bits = 6
    }};
    entry = vmm_pci_create_bar_emulation(entry, 1, bars);
    vmm_pci_add_entry(&vmm->pci, entry, NULL);
    virtio_net_t *net = malloc(sizeof(*net));
    virtio_net = net;
    assert(net);
    memset(net, 0, sizeof(*net));
    net->iobase = 0x9000;
    vmm_io_port_add_handler(&vmm->io_port, 0x9000, 0x9000 + MASK(6), net, virtio_net_io_in, virtio_net_io_out, "VIRTIO PCI NET");
    ps_io_ops_t ioops;
    ioops.dma_manager = (ps_dma_man_t) {
        .cookie = NULL,
        .dma_alloc_fn = malloc_dma_alloc,
        .dma_free_fn = malloc_dma_free,
        .dma_pin_fn = malloc_dma_pin,
        .dma_unpin_fn = malloc_dma_unpin,
        .dma_cache_op_fn = malloc_dma_cache_op
    };
    net->emul = ethif_virtio_emul_init(ioops, QUEUE_SIZE, &vmm->guest_mem.vspace, emul_driver_init, net);
    assert(net->emul);
    /* drain any existing packets */
    int len;
    while (ethdriver_rx(&len) != -1);
}

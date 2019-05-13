/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#include <autoconf.h>
#include <camkes_vmm/gen_config.h>
#include <camkes/dataport.h>
#include <sel4/sel4.h>
#include <lwip/udp.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <camkes.h>

#include <sel4platsupport/arch/io.h>
#include <sel4utils/vspace.h>
#include <sel4utils/iommu_dma.h>
#include <simple/simple_helpers.h>
#include <vka/capops.h>

#include <ethdrivers/virtio/virtio_pci.h>
#include <ethdrivers/virtio/virtio_net.h>
#include <ethdrivers/virtio/virtio_ring.h>

#include <sel4vm/guest_vm.h>
#include "sel4vm/vmm.h"
#include "sel4vm/driver/pci_helper.h"
#include "sel4vm/driver/virtio_emul.h"
#include "sel4vm/platform/ioports.h"
#include "sel4vm/platform/guest_vspace.h"

#include "vm.h"
#include "i8259.h"

#define RINGBUF_SIZE sizeof(/*? show(interface.type) ?*/)
#define BUFSLOTS (RINGBUF_SIZE / 2 / 2048)

/* Actual dataport is emitted in the per-component template. */
/*- set p = Perspective(dataport=interface.name) -*/
char /*? p['dataport_symbol'] ?*/[ROUND_UP_UNSAFE(RINGBUF_SIZE, PAGE_SIZE_4K)]
    __attribute__((aligned(PAGE_SIZE_4K)))
    __attribute__((section("shared_/*? interface.name ?*/")))
    __attribute__((externally_visible));
volatile /*? show(interface.type) ?*/ * /*? interface.name ?*/ = (volatile /*? show(interface.type) ?*/ *) /*? p['dataport_symbol'] ?*/;

/*- set settings = [] -*/

/*- for s in configuration.settings -*/
    /*- if s.instance == instance.name -*/
        /*- if s.attribute == "%s_attributes" % (interface.name) -*/
            /*- set mac, irq, iobase = s.value.strip('"').split(':') -*/
            /*- do settings.append( ( mac, int(irq, 0), int(iobase, 0) ) ) -*/
        /*- endif -*/
    /*- endif -*/
/*- endfor -*/

/*- set mac, irq, iobase = settings.pop() -*/

int /*? interface.name ?*/_wrap_ptr(dataport_ptr_t *p, void *ptr) {
    /* should not be used */
    return -1;
}

void * /*? interface.name ?*/_unwrap_ptr(dataport_ptr_t *p) {
    /* should not be used */
    return NULL;
}

#define VIRTIO_VID 0x1af4
#define VIRTIO_DID_START 0x1000

#define QUEUE_SIZE 128

typedef struct buf_entry {
    int full;
    uint32_t len;
    char data[2048 - sizeof(int) * 2];
} __attribute__((packed)) buf_entry_t;

typedef struct vmnet {
    unsigned int iobase;
    ethif_virtio_emul_t *emul;
    struct eth_driver *emul_driver;
    ps_io_ops_t ioops;
    int rx_head;

    int tx_head;
    int tx_tail;
    int have_queued_tx[BUFSLOTS];
    void *tx_cookies[BUFSLOTS];
    volatile buf_entry_t *tx_ring;
    volatile buf_entry_t *rx_ring;
} vmnet_t;

static vmnet_t *vmnet = NULL;

static void notify_remote() {
    /*? interface.name ?*/_emit_emit();
}

static int vmnet_io_in(void *cookie, unsigned int port_no, unsigned int size, unsigned int *result) {
    vmnet_t *net = (vmnet_t*)cookie;
    unsigned int offset = port_no - net->iobase;
    return net->emul->io_in(net->emul, offset, size, result);
}

static int vmnet_io_out(void *cookie, unsigned int port_no, unsigned int size, unsigned int value) {
    int ret;
    vmnet_t *net = (vmnet_t*)cookie;
    unsigned int offset = port_no - net->iobase;
    ret = net->emul->io_out(net->emul, offset, size, value);
    return ret;
}

static int emul_raw_tx(struct eth_driver *driver, unsigned int num, uintptr_t *phys, unsigned int *len, void *cookie) {
    vmnet_t *net = (vmnet_t*)driver->eth_data;
    size_t tot_len = 0;
    volatile buf_entry_t *buf = &net->tx_ring[net->tx_head];
    /* see if there is a free buffer */
    int idx = net->tx_head;
    if ( (idx + 1) % BUFSLOTS == net->tx_tail) {
        return ETHIF_TX_FAILED;
    }
    assert(!buf->full);
    char *p = (char*)buf->data;
    /* copy to the data port */
    for (int i = 0; i < num; i++) {
        if (tot_len + len[i] > 1500) {
            return ETHIF_TX_FAILED;
        }
        memcpy(p + tot_len, (void*)phys[i], len[i]);
        tot_len += len[i];
    }
    net->tx_head = (net->tx_head + 1) % BUFSLOTS;
    net->have_queued_tx[idx] = 1;
    net->tx_cookies[idx] = cookie;
    buf->len = tot_len;
    buf->full = 1;
    notify_remote();
    return ETHIF_TX_ENQUEUED;
}

static void emul_raw_handle_irq(struct eth_driver *driver, int irq) {
    i8259_gen_irq(/*? irq ?*/);
}

static void emul_raw_poll(struct eth_driver *driver) {
    assert(!"not implemented");
}

static void emul_low_level_init(struct eth_driver *driver, uint8_t *mac, int *mtu) {
    static uint8_t eth_mac[6] = { /*? mac ?*/ };
    memcpy(mac, eth_mac, sizeof(eth_mac));
    *mtu = 1500;
}

static void emul_print_state(struct eth_driver* driver) {
    assert(!"not implemented");
}

static struct raw_iface_funcs emul_driver_funcs = {
    .raw_tx = emul_raw_tx,
    .raw_handleIRQ = emul_raw_handle_irq,
    .raw_poll = emul_raw_poll,
    .print_state = emul_print_state,
    .low_level_init = emul_low_level_init
};

static int emul_driver_init(struct eth_driver *driver, ps_io_ops_t io_ops, void *config) {
    vmnet_t *net = (vmnet_t*)config;
    driver->eth_data = config;
    driver->dma_alignment = sizeof(uintptr_t);
    driver->i_fn = emul_driver_funcs;
    net->emul_driver = driver;
    return 0;
}

void /*? interface.name ?*/_notify() {
    int len;
    int notify = 0;
    /* scan for completed rx and tx packets */
    int idx;
    for (idx = vmnet->tx_tail; vmnet->have_queued_tx[idx] && !vmnet->tx_ring[idx].full && idx != vmnet->tx_head; idx = (idx + 1) % BUFSLOTS) {
        vmnet->emul_driver->i_cb.tx_complete(vmnet->emul_driver->cb_cookie, vmnet->tx_cookies[idx]);
        vmnet->have_queued_tx[idx] = 0;
        vmnet->tx_cookies[idx] = NULL;
    }
    vmnet->tx_tail = idx;
    for (idx = vmnet->rx_head; vmnet->rx_ring[idx].full; idx = (idx + 1) % BUFSLOTS) {
        void *cookie;
        len = vmnet->rx_ring[idx].len;
        void *emul_buf = (void*)vmnet->emul_driver->i_cb.allocate_rx_buf(vmnet->emul_driver->cb_cookie, len, &cookie);
        if (emul_buf) {
            memcpy(emul_buf, (void*)vmnet->rx_ring[idx].data, len);
            vmnet->emul_driver->i_cb.rx_complete(vmnet->emul_driver->cb_cookie, 1, &cookie, (unsigned int*)&len);
        }
        vmnet->rx_ring[idx].full = 0;
        notify = 1;
    }
    if (notify) {
        notify_remote(vmnet);
    }
    vmnet->rx_head = idx;
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

void /*? interface.name ?*/_init(vm_t *vm) {
    vmm_pci_device_def_t *pci_config = malloc(sizeof(*pci_config));
    assert(pci_config);
    memset(pci_config, 0, sizeof(*pci_config));
    *pci_config = (vmm_pci_device_def_t) {
        .vendor_id = VIRTIO_VID,
        .device_id = VIRTIO_DID_START,
        .cache_line_size = 64,
        .latency_timer = 64,
        .subsystem_id = 1,
        .interrupt_pin = /*? irq ?*/,
        .interrupt_line = /*? irq ?*/
    };
    vmm_pci_entry_t entry = (vmm_pci_entry_t) {
        .cookie = pci_config,
        .ioread = vmm_pci_mem_device_read,
        .iowrite = vmm_pci_entry_ignore_write
    };
    vmm_pci_bar_t bars[1] = {{
        .ismem = 0,
        .address = /*? iobase ?*/,
        .size_bits = 6
    }};
    entry = vmm_pci_create_bar_emulation(entry, 1, bars);
    vmm_pci_add_entry(&vm->pci, entry, NULL);
    vmnet_t *net = malloc(sizeof(*net));
    assert(net);
    vmnet = net;
    memset(net, 0, sizeof(*net));
    char *p = /*? p['dataport_symbol'] ?*/;
    /*- if buf_order -*/
        net->tx_ring = (volatile buf_entry_t *)p;
        net->rx_ring = (volatile buf_entry_t *)(p + (RINGBUF_SIZE / 2));
    /*- else -*/
        net->tx_ring = (volatile buf_entry_t *)(p + (RINGBUF_SIZE / 2));
        net->rx_ring = (volatile buf_entry_t *)p;
    /*- endif -*/
    net->iobase = /*? iobase ?*/;
    vmm_io_port_add_handler(&vmm->io_port, /*? iobase ?*/, /*? iobase ?*/ + MASK(6), net, vmnet_io_in, vmnet_io_out, "VIRTIO PCI NET");
    ps_io_ops_t ioops;
    ioops.dma_manager = (ps_dma_man_t) {
        .cookie = NULL,
        .dma_alloc_fn = malloc_dma_alloc,
        .dma_free_fn = malloc_dma_free,
        .dma_pin_fn = malloc_dma_pin,
        .dma_unpin_fn = malloc_dma_unpin,
        .dma_cache_op_fn = malloc_dma_cache_op
    };
    net->emul = ethif_virtio_emul_init(vm, ioops, QUEUE_SIZE, &vm->mem.vm_vspace, emul_driver_init, net);
    assert(net->emul);
}

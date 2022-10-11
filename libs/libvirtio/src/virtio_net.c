/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
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

#include <sel4vm/guest_irq_controller.h>
#include <sel4vm/boot.h>

#include <sel4vmmplatsupport/drivers/virtio_net.h>
#include <sel4vmmplatsupport/device.h>
#include <sel4vmmplatsupport/arch/vpci.h>

#include <virtioarm/virtio.h>
#include <virtioarm/virtio_net.h>
#include <virtioarm/virtio_plat.h>

typedef struct virtio_net_cookie {
    virtio_net_t *virtio_net;
    virtio_net_callbacks_t callbacks;
    vm_t *vm;
} virtio_net_cookie_t;

static void (*get_mac_addr_callback)(uint8_t *mac) = NULL;

static void emul_raw_handle_irq(struct eth_driver *driver, int irq)
{
    virtio_net_cookie_t *virtio_cookie = (virtio_net_cookie_t *)driver->eth_data;
    if (!virtio_cookie) {
        ZF_LOGE("NULL virtio cookie given to raw irq handler");
        return;
    }
    int err = vm_inject_irq(virtio_cookie->vm->vcpus[BOOT_VCPU], VIRTIO_NET_PLAT_INTERRUPT_LINE);
    if (err) {
        ZF_LOGE("Failed to inject irq");
        return;
    }
    if (virtio_cookie->callbacks.irq_callback) {
        virtio_cookie->callbacks.irq_callback(irq, virtio_cookie->virtio_net);
    }
}

static int emul_raw_tx(struct eth_driver *driver, unsigned int num, uintptr_t *phys, unsigned int *len, void *cookie)
{
    bool complete = true;
    int err;
    virtio_net_cookie_t *virtio_cookie = (virtio_net_cookie_t *)driver->eth_data;
    if (!virtio_cookie) {
        ZF_LOGE("NULL virtio cookie given to raw tx");
        return -1;
    }

    for (int i = 0; i < num; i++) {
        char ethbuffer[MAX_MTU];
        if (len[i] > MAX_MTU) {
            ZF_LOGE("TX data exceeds MTU (%d) - truncating remaining data", MAX_MTU);
            complete = false;
            break;
        }
        memcpy((void *)ethbuffer, (void *)phys[i], len[i]);
        if (virtio_cookie->callbacks.tx_callback) {
            err = virtio_cookie->callbacks.tx_callback(ethbuffer, len[i], virtio_cookie->virtio_net);
            if (err) {
                ZF_LOGE("TX callback failed");
                complete = false;
                break;
            }
        }
    }

    return complete ? ETHIF_TX_COMPLETE : ETHIF_TX_FAILED;
}

int virtio_net_rx(char *data, size_t length, virtio_net_t *virtio_net)
{
    unsigned int len[1];
    len[0] = length;
    void *cookie;
    void *emul_buf = (void *)virtio_net->emul_driver->i_cb.allocate_rx_buf(virtio_net->emul_driver->cb_cookie, len[0],
                                                                           &cookie);
    if (emul_buf) {
        memcpy(emul_buf, (void *)data, len[0]);
        virtio_net->emul_driver->i_cb.rx_complete(virtio_net->emul_driver->cb_cookie, 1, &cookie, len);
    }
    return 0;
}

static void emul_low_level_init(struct eth_driver *driver, uint8_t *mac, int *mtu)
{
    get_mac_addr_callback(mac);
    *mtu = MAX_MTU_DATA;
}

static void virtio_net_ack(vm_vcpu_t *vcpu, int irq, void *token) {}

virtio_net_t *virtio_net_init(vm_t *vm, virtio_net_callbacks_t *callbacks,
                              vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports)
{
    virtio_net_cookie_t *driver_cookie;
    virtio_net_t *virtio_net;

    get_mac_addr_callback = callbacks->get_mac_addr_callback;

    struct raw_iface_funcs backend = virtio_net_default_backend();
    backend.raw_tx = emul_raw_tx;
    backend.low_level_init = emul_low_level_init;
    backend.raw_handleIRQ = emul_raw_handle_irq;

    driver_cookie = (virtio_net_cookie_t *)calloc(1, sizeof(struct virtio_net_cookie));
    if (driver_cookie == NULL) {
        ZF_LOGE("Failed to allocated virtio iface cookie");
        return NULL;
    }

    ioport_range_t virtio_port_range = {0, 0, VIRTIO_IOPORT_SIZE};
    virtio_net = common_make_virtio_net(vm, pci, io_ports, virtio_port_range, IOPORT_FREE,
                                        VIRTIO_INTERRUPT_PIN, VIRTIO_NET_PLAT_INTERRUPT_LINE, backend);
    if (virtio_net == NULL) {
        ZF_LOGE("Failed to initialise virtio net driver");
        return NULL;
    }
    driver_cookie->virtio_net = virtio_net;
    driver_cookie->vm = vm;
    int err =  vm_register_irq(vm->vcpus[BOOT_VCPU], VIRTIO_NET_PLAT_INTERRUPT_LINE, &virtio_net_ack, NULL);
    if (callbacks) {
        driver_cookie->callbacks.tx_callback = callbacks->tx_callback;
        driver_cookie->callbacks.irq_callback = callbacks->irq_callback;
    }
    virtio_net->emul_driver->eth_data = (void *)driver_cookie;
    return virtio_net;
}

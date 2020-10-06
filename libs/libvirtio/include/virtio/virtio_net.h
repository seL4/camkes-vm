/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#include <sel4vmmplatsupport/drivers/virtio_net.h>

/* Maximum transmission unit for Ethernet interface */
#define ETHERNET_HEADER_SIZE 14
#define MAX_MTU_DATA 1500
#define MAX_MTU MAX_MTU_DATA + ETHERNET_HEADER_SIZE

typedef struct virtio_net_callbacks {
    int (*tx_callback)(char *data, size_t length, virtio_net_t *virtio_net);
    void (*irq_callback)(int irq, virtio_net_t *virtio_net);
    void (*get_mac_addr_callback)(uint8_t *mac);
} virtio_net_callbacks_t;

int virtio_net_rx(char *data, size_t length, virtio_net_t *virtio_net);

virtio_net_t *virtio_net_init(vm_t *vm, virtio_net_callbacks_t *callbacks,
                              vmm_pci_space_t *pci, vmm_io_port_list_t *io_ports);

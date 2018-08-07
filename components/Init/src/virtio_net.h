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

#ifndef VM_INIT_VIRTIO_NET_H
#define VM_INIT_VIRTIO_NET_H

void make_virtio_net(vmm_t *vmm);
void virtio_net_notify(vmm_t *vmm);

typedef struct virtio_net {
    unsigned int iobase;
    ethif_virtio_emul_t *emul;
    struct eth_driver *emul_driver;
    struct raw_iface_funcs emul_driver_funcs;
    ps_io_ops_t ioops;
} virtio_net_t;

/**
 * Initialise a new virtio_net device with BARs starting at iobase and backend functions
 *
 * specified by the raw_iface_funcs struct.
 * @param vmm handle to vmm data
 * @param iobase starting BAR port for front end emulation to start from
 * @param backend function pointers to backend implementation. Can be initialised by
 *  virtio_net_default_backend for default methods.
 * @return pointer to an initialised virtio_net_t, NULL if error.
 */
virtio_net_t *common_make_virtio_net(vmm_t *vmm, unsigned int iobase, struct raw_iface_funcs backend);

/**
 * @return a struct with a default virtio_net backend. It is the responsibility of the caller to
 *  update these function pointers with its own custom backend.
 */
struct raw_iface_funcs virtio_net_default_backend(void);

#endif

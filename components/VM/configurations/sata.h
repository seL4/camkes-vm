/*
 * Copyright 2019, Dornerworks
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#define VM_INIT_SATA()                                                                                 \
    has mutex virtio_blk_mutex;                                                                        \
    uses SataserverInterface sataserver_iface;

#define SATA_COMPOSITION_DEF()                                                                         \
    component Sataserver sataserver;                                                                   \
    // connection seL4RPCCall sata_serial(from sataserver.putchar, to serial.processed_putchar);

/* Convenience wrapper for connecting VMs to the SataServer component
 * num: vm instance number
*/
#define VM_SATA_CONNECTIONS(num)                                                                       \
    connection seL4Sataserver sataservercon##num(from vm##num.sataserver_iface, to sataserver.client);

/* Convenience wrapper for configuring the sataserver
 */
#define VM_SATA_CONFIG()                                                                               \
    sataserver.simple = true;                /* Links component to component.simple.c */               \
    sataserver.cnode_size_bits = 16;         /* Changes cnode size from default 12 to 16 */            \
    sataserver.simple_untyped23_pool = 2;    /* Creates 2 untyped pools of size 2^23 */                \
    sataserver.heap_size = 0x30000;          /* RAM allocation available to sataserver component */    \
    sataserver.dma_pool = 0x20000;                                                                     \
    sataserver.putchar_attributes = 0;

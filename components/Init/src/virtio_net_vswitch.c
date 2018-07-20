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
#include <ethdrivers/sel4vswitch.h>

#include "vmm/vmm.h"
#include "vmm/driver/pci_helper.h"
#include "vmm/driver/virtio_emul.h"
#include "vmm/platform/ioports.h"
#include "vmm/platform/guest_vspace.h"

#include "vm.h"
#include "i8259.h"
#include "virtio_net.h"

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


static virtio_net_t *virtio_net = NULL;


static int emul_raw_tx(struct eth_driver *driver,
                       unsigned int num, uintptr_t *phys, unsigned int *len,
                       void *cookie)
{
    sel4vswitch_t *g_vswitch;
    size_t tot_len = 0;

    (void)tot_len;

    g_vswitch = vmm_sel4vswitch_get_global_inst();
    assert(g_vswitch != NULL);

    /* Copy to the buffqueue */
    for (int i = 0; i < num; i++) {
        sel4vswitch_mac802_addr_t *destaddr;
        int err, destnode_start_idx, destnode_n_idxs;

        /* Initialize a convenience pointer to the dest macaddr.
         * The dest MAC addr is the first member of an ethernet frame.
         */
        destaddr = (sel4vswitch_mac802_addr_t *)phys[i];

        /* Set up the bounds of the loop below that copies the frames into the
         * destination Guest's buffqueue.
         */
        if (mac802_addr_eq_bcast(destaddr)) {
            /* Send to all nodes on the VLAN if destaddr is bcast addr. */
            destnode_n_idxs = g_vswitch->n_connected;
            destnode_start_idx = 0;
        }
        else {
            /* Send only to the target node */
            destnode_n_idxs = 1;
            destnode_start_idx = sel4vswitch_get_destnode_index_by_macaddr(
                                                            g_vswitch,
                                                            destaddr);
            if (destnode_start_idx < 0) {
                ZF_LOGE("Unreachable dest macaddr " PR_MAC802_ADDR ". Dropping "
                        "frame.",
                        PR_MAC802_ADDR_ARGS(destaddr));

                /* This function seems to be pretending to send multiple frames
                 * at once, but in reality, it is only ever invoked with the
                 * "num" argment being "1".
                 *
                 * So return error instead of "continue"-ing here.
                 */
                return ETHIF_TX_FAILED;
            }
        }

        /* Copy the frame into the buffqueue of each of the targets we decided
         * upon.
         */
        for (int j=destnode_start_idx;
             j<destnode_start_idx + destnode_n_idxs; j++) {
            sel4vswitch_node_t *destnode;
            sel4buffqueue_buff_t *destbuff;

            destnode = sel4vswitch_get_destnode_by_index(g_vswitch, j);
            if (destnode == NULL) {
                /* This could happen in the broadcast case if there are holes in
                 * the array, though that would still be odd.
                 */
                ZF_LOGW("Found holes in node array while sending to dest MAC "
                        PR_MAC802_ADDR".",
                        PR_MAC802_ADDR_ARGS(destaddr));
                continue;
            }

            destbuff = sel4buffqueue_allocate_buff(destnode->buffqueue, len[i]);
            if (destbuff == NULL) {
                ZF_LOGW("Dropping eth frame to dest " PR_MAC802_ADDR ": no buff "
                        "available.",
                        PR_MAC802_ADDR_ARGS(destaddr));

                return ETHIF_TX_FAILED;
            };

            err = sel4buffqueue_buff_write(destbuff,
                                           (void *)phys[i], len[i]);
            if (err < len[i]) {
                ZF_LOGE("Unknown error while writing ethframe to windowqueue "
                        "for dest " PR_MAC802_ADDR ": wrote %d of %d bytes.",
                        PR_MAC802_ADDR_ARGS(destaddr),
                        err, len[i]);

                return ETHIF_TX_FAILED;
            }

            err = sel4buffqueue_signal(destnode->buffqueue);
            if (err != 0) {
                ZF_LOGE("Unknown error while signaling dest "
                        PR_MAC802_ADDR ".",
                        PR_MAC802_ADDR_ARGS(destaddr));

                return ETHIF_TX_FAILED;
            }

            tot_len += len[i];
        }
    }

    return ETHIF_TX_COMPLETE;
}


static void emul_low_level_init(struct eth_driver *driver, uint8_t *mac, int *mtu) {
    ethdriver_mac(&mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
    *mtu = 1500;
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

void make_virtio_net(vmm_t *vmm) {
    /* drain any existing packets */
    struct raw_iface_funcs backend = virtio_net_default_backend();
    backend.raw_tx = emul_raw_tx;
    backend.low_level_init = emul_low_level_init;
    virtio_net = common_make_virtio_net(vmm, 0x9000, backend);
    assert(virtio_net);
    int len;
    while (ethdriver_rx(&len) != -1);
}

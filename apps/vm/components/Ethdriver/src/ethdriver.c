/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#include <autoconf.h>

#include "vm.h"
#include <Ethdriver.h>
#include <platsupport/io.h>
#include <vka/vka.h>
#include <simple/simple.h>
#include <simple/simple_helpers.h>
#include <allocman/allocman.h>
#include <allocman/bootstrap.h>
#include <allocman/vka.h>
#include <sel4utils/vspace.h>
#include <ethdrivers/intel.h>
#include <ethdrivers/raw.h>
#include <sel4utils/iommu_dma.h>
#include <sel4platsupport/arch/io.h>

#define RX_BUFS 256

#define CLIENT_RX_BUFS 128
#define CLIENT_TX_BUFS 128

#define BUF_SIZE 2048

#define BRK_VIRTUAL_SIZE 400000000

reservation_t muslc_brk_reservation;
void *muslc_brk_reservation_start;
vspace_t  *muslc_this_vspace;
static sel4utils_res_t muslc_brk_reservation_memory;
static allocman_t *allocman;
static char allocator_mempool[8388608];
static simple_t camkes_simple;
static vka_t vka;
static vspace_t vspace;
static sel4utils_alloc_data_t vspace_data;
static struct eth_driver eth_driver;

void camkes_make_simple(simple_t *simple);

/* this flag indicates whether we or not we need to notify the client
 * if new data is received. We only notify once the client observes
 * the last packet */
static int should_notify_client = 1;

typedef struct pending_rx {
    void *buf;
    int len;
} pending_rx_t;

static int pending_client_rx_head = 0;
static int pending_client_rx_tail = 0;
static pending_rx_t pending_client_rx[CLIENT_RX_BUFS];

static int num_client_tx_bufs = 0;
static void *client_tx_bufs[CLIENT_TX_BUFS];

static int num_rx_bufs = 0;
static void *rx_bufs[RX_BUFS];

static int done_init = 0;

static void init_system(void) {
    int error;
    set_putchar(putchar_putchar);

    /* Camkes adds nothing to our address space, so this array is empty */
    void *existing_frames[] = {
        NULL
    };
    camkes_make_simple(&camkes_simple);

    /* Initialize allocator */
    allocman = bootstrap_use_current_1level(
            simple_get_cnode(&camkes_simple),
            simple_get_cnode_size_bits(&camkes_simple),
            simple_last_valid_cap(&camkes_simple) + 1,
            BIT(simple_get_cnode_size_bits(&camkes_simple)),
            sizeof(allocator_mempool), allocator_mempool
    );
    assert(allocman);
    error = allocman_add_simple_untypeds(allocman, &camkes_simple);
    allocman_make_vka(&vka, allocman);

    /* Initialize the vspace */
    error = sel4utils_bootstrap_vspace(&vspace, &vspace_data,
            simple_get_init_cap(&camkes_simple, seL4_CapInitThreadPD), &vka, NULL, NULL, existing_frames);
    assert(!error);

    sel4utils_reserve_range_no_alloc(&vspace, &muslc_brk_reservation_memory, BRK_VIRTUAL_SIZE, seL4_AllRights, 1, &muslc_brk_reservation_start);
    muslc_this_vspace = &vspace;
    muslc_brk_reservation = (reservation_t){.res = &muslc_brk_reservation_memory};
}

static void eth_tx_complete(void *iface, void *cookie) {
    client_tx_bufs[num_client_tx_bufs] = cookie;
    num_client_tx_bufs++;
}

static uintptr_t eth_allocate_rx_buf(void *iface, size_t buf_size, void **cookie) {
    if (buf_size > BUF_SIZE) {
        return 0;
    }
    if (num_rx_bufs == 0) {
        return 0;
    }
    num_rx_bufs--;
    void *buf = rx_bufs[num_rx_bufs];
    *cookie = buf;
    return (uintptr_t)buf;
}

static void eth_rx_complete(void *iface, unsigned int num_bufs, void **cookies, unsigned int *lens) {
    /* insert filtering here. currently everything just goes to one client */
    if (num_bufs != 1 || (pending_client_rx_head + 1) % CLIENT_RX_BUFS == pending_client_rx_tail) {
        /* abort and put all the bufs back */
        for (int i = 0; i < num_bufs; i++) {
            rx_bufs[num_rx_bufs] = cookies[i];
            num_rx_bufs++;
        }
        return;
    }
    pending_client_rx[pending_client_rx_head] = (pending_rx_t){cookies[0],lens[0]};
    pending_client_rx_head = (pending_client_rx_head + 1) % CLIENT_RX_BUFS;
    if (should_notify_client) {
        rx_ready_emit();
        should_notify_client = 0;
    }
}

static struct raw_iface_callbacks ethdriver_callbacks = {
    .tx_complete = eth_tx_complete,
    .rx_complete = eth_rx_complete,
    .allocate_rx_buf = eth_allocate_rx_buf
};

int client_rx(int *len) {
    if (!done_init) {
        return;
    }
    int ret;
    ethdriver_lock();
    if (pending_client_rx_head == pending_client_rx_tail) {
        should_notify_client = 1;
        ethdriver_unlock();
        return -1;
    }
    pending_rx_t rx = pending_client_rx[pending_client_rx_tail];
    pending_client_rx_tail = (pending_client_rx_tail + 1) % CLIENT_RX_BUFS;
    memcpy(packet, rx.buf, rx.len);
    *len = rx.len;
    if (pending_client_rx_tail == pending_client_rx_head) {
        should_notify_client = 1;
        ret = 0;
    } else {
        ret = 1;
    }
    rx_bufs[num_rx_bufs] = rx.buf;
    num_rx_bufs++;
    ethdriver_unlock();
    return ret;
}

void client_tx(int len) {
    if (!done_init) {
        return;
    }
    ethdriver_lock();
    if (len > BUF_SIZE) {
        len = BUF_SIZE;
    }
    if (len < 0) {
        len = 0;
    }
    if (num_client_tx_bufs == 0) {
        /* silently drop packets */
        ethdriver_unlock();
        return;
    }
    num_client_tx_bufs --;
    void *tx_buf = client_tx_bufs[num_client_tx_bufs];
    /* copy the packet over */
    memcpy(tx_buf, packet, len);
    /* queue up transmit */
    eth_driver.i_fn.raw_tx(&eth_driver, 1, (uintptr_t*)&tx_buf, (unsigned int*)&len, tx_buf);
    ethdriver_unlock();
}

static void eth_interrupt(void *cookie) {
    ethdriver_lock();
    eth_driver.i_fn.raw_handleIRQ(&eth_driver, 0);
    irq_reg_callback(eth_interrupt, NULL);
    ethdriver_unlock();
}

void client_mac(uint8_t *b1, uint8_t *b2, uint8_t *b3, uint8_t *b4, uint8_t *b5, uint8_t *b6) {
    ethdriver_lock();
    assert(done_init);
    uint8_t mac[6];
    int mtu;
    eth_driver.i_fn.low_level_init(&eth_driver, mac, &mtu);
    *b1 = mac[0];
    *b2 = mac[1];
    *b3 = mac[2];
    *b4 = mac[3];
    *b5 = mac[4];
    *b6 = mac[5];
    ethdriver_unlock();
}

void post_init(void) {
    int error;
    int iospace_id;
    int pci_bdf;
    ethdriver_lock();
    /* initialize seL4 allocators and give us a half sane environment */
    init_system();
    /* initialize the driver */
    ps_io_ops_t ioops;
    cspacepath_t iospace;
    error = vka_cspace_alloc_path(&vka, &iospace);
    assert(!error);
#define PER_ETH_CONFIG(num, iteration, data) \
    if (strcmp(get_instance_name(), BOOST_PP_STRINGIZE(ethdriver##iteration)) == 0) { \
        iospace_id = BOOST_PP_CAT(VM_ETHDRIVER_IOSPACE_,iteration)(); \
        pci_bdf = BOOST_PP_CAT(VM_ETHDRIVER_PCI_BDF_,iteration)(); \
    } \
    /**/
    BOOST_PP_REPEAT(VM_NUM_ETHDRIVERS, PER_ETH_CONFIG, _)
    /* get this from the configuration */
    error = simple_get_iospace(&camkes_simple, iospace_id, pci_bdf, &iospace);
    assert(!error);
    error = sel4utils_make_iommu_dma_alloc(&vka, &vspace, &ioops.dma_manager, 1, &iospace.capPtr);
    assert(!error);
    error = sel4platsupport_get_io_port_ops(&ioops.io_port_ops, &camkes_simple);
    assert(!error);
    /* preallocate buffers */
    for (int i = 0; i < RX_BUFS; i++) {
        void *buf = ps_dma_alloc(&ioops.dma_manager, BUF_SIZE, 4, 1, PS_MEM_NORMAL);
        assert(buf);
        uintptr_t phys = ps_dma_pin(&ioops.dma_manager, buf, BUF_SIZE);
        assert(phys == (uintptr_t)buf);
        rx_bufs[num_rx_bufs] = buf;
        num_rx_bufs++;
    }
    for (int i = 0; i < CLIENT_TX_BUFS; i++) {
        void *buf = ps_dma_alloc(&ioops.dma_manager, BUF_SIZE, 4, 1, PS_MEM_NORMAL);
        assert(buf);
        uintptr_t phys = ps_dma_pin(&ioops.dma_manager, buf, BUF_SIZE);
        assert(phys == (uintptr_t)buf);
        client_tx_bufs[num_client_tx_bufs] = buf;
        num_client_tx_bufs++;
    }
    eth_driver.cb_cookie = NULL;
    eth_driver.i_cb = ethdriver_callbacks;
    ethif_intel_config_t eth_config = (ethif_intel_config_t) {
        .bar0 = (void*)EthDriver
    };
    error = ethif_e82580_init(&eth_driver, ioops, &eth_config);
    assert(!error);
    done_init = 1;
    irq_reg_callback(eth_interrupt, NULL);
    ethdriver_unlock();
}

int __attribute__((weak)) packet_wrap_ptr(dataport_ptr_t *p, void *ptr) {
    return -1;
}

void * __attribute__((weak)) packet_unwrap_ptr(dataport_ptr_t *p) {
    return NULL;
}


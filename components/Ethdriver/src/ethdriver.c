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

#include <camkes.h>
#include <camkes/dma.h>

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
static seL4_CPtr (*original_vspace_get_cap)(vspace_t*, void*);

void camkes_make_simple(simple_t *simple);

typedef struct pending_rx {
    void *buf;
    int len;
    int client;
} pending_rx_t;

typedef struct eth_buf {
    void *buf;
    int len;
    int client;
} tx_buf_t;

typedef struct client {
    /* this flag indicates whether we or not we need to notify the client
     * if new data is received. We only notify once the client observes
     * the last packet */
    int should_notify;

    int rx_head;
    int rx_tail;
    pending_rx_t rx[CLIENT_RX_BUFS];

    int num_tx;
    tx_buf_t tx_mem[CLIENT_TX_BUFS];
    tx_buf_t *tx[CLIENT_TX_BUFS];

    /* mac address for this client */
    uint8_t mac[6];

    /* id for this client */
    int client_id;

    /* dataport for this client */
    void *dataport;
} client_t;

static int num_clients = 0;
static client_t *clients = NULL;

static int num_rx_bufs;
static void *rx_bufs[RX_BUFS];

static int done_init = 0;

/* Functions provided by the Ethdriver template */
void client_emit(unsigned int client_id);
unsigned int client_get_sender_id(void);
unsigned int client_num_badges(void);
unsigned int client_enumerate_badge(unsigned int i);
void *client_buf(unsigned int client_id);
void client_get_mac(unsigned int client_id, uint8_t *mac);

static void init_system(void) {
    int error;

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
    tx_buf_t *buf = (tx_buf_t*)cookie;
    client_t *client = &clients[buf->client];
    client->tx[client->num_tx] = buf;
    client->num_tx++;
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

static client_t *detect_client(void *buf, unsigned int len) {
    if (len < 6) {
        return NULL;
    }
    for (int i = 0; i < num_clients; i++) {
        if (memcmp(clients[i].mac, buf, 6) == 0) {
            return &clients[i];
        }
    }
    return NULL;
}

static int is_broadcast(void *buf, unsigned int len) {
    static uint8_t broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    if (len < 6) {
        return 0;
    }
    if (memcmp(buf, broadcast, 6) == 0) {
        return 1;
    }
    return 0;
}

static void give_client_buf(client_t *client, void *cookie, unsigned int len) {
    client->rx[client->rx_head] = (pending_rx_t){cookie,len, 0};
    client->rx_head = (client->rx_head + 1) % CLIENT_RX_BUFS;
    if (client->should_notify) {
        client_emit(client->client_id);
        client->should_notify = 0;
    }
}

static void eth_rx_complete(void *iface, unsigned int num_bufs, void **cookies, unsigned int *lens) {
    /* insert filtering here. currently everything just goes to one client */
    if (num_bufs != 1) {
        goto error;
    }
    client_t *client = detect_client(cookies[0], lens[0]);
    if (!client) {
        if (is_broadcast(cookies[0], lens[0])) {
            /* in a broadcast duplicate this buffer for every other client, we will fallthrough
             * to give the buffer to client 0 at the end */
            for (int i = 1; i < num_clients; i++) {
                client = &clients[i];
                if ((client->rx_head + 1) % CLIENT_RX_BUFS != client->rx_tail) {
                    void *cookie;
                    void *buf = (void*)eth_allocate_rx_buf(iface, lens[0], &cookie);
                    if (buf) {
                        memcpy(buf, cookies[0], lens[0]);
                        give_client_buf(client, cookie, lens[0]);
                    }
                }
            }
        } else {
            goto error;
        }
        client = &clients[0];
    }
    if ((client->rx_head + 1) % CLIENT_RX_BUFS == client->rx_tail) {
        goto error;
    }
    give_client_buf(client, cookies[0], lens[0]);
    return;
error:
    /* abort and put all the bufs back */
    for (int i = 0; i < num_bufs; i++) {
        rx_bufs[num_rx_bufs] = cookies[i];
        num_rx_bufs++;
    }
}

static struct raw_iface_callbacks ethdriver_callbacks = {
    .tx_complete = eth_tx_complete,
    .rx_complete = eth_rx_complete,
    .allocate_rx_buf = eth_allocate_rx_buf
};

int client_rx(int *len) {
    int UNUSED err;
    if (!done_init) {
        return -1;
    }
    int ret;
    int id = client_get_sender_id();
    client_t *client = NULL;
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].client_id == id) {
            client = &clients[i];
        }
    }
    assert(client);
    void *packet = client->dataport;
    err = ethdriver_lock();
    if (client->rx_head == client->rx_tail) {
        client->should_notify = 1;
        err = ethdriver_unlock();
        return -1;
    }
    pending_rx_t rx = client->rx[client->rx_tail];
    client->rx_tail = (client->rx_tail + 1) % CLIENT_RX_BUFS;
    memcpy(packet, rx.buf, rx.len);
    *len = rx.len;
    if (client->rx_tail == client->rx_head) {
        client->should_notify = 1;
        ret = 0;
    } else {
        ret = 1;
    }
    rx_bufs[num_rx_bufs] = rx.buf;
    num_rx_bufs++;
    err = ethdriver_unlock();
    return ret;
}

int client_tx(int len) {
    int UNUSED error;
    if (!done_init) {
        return -1;
    }
    if (len > BUF_SIZE) {
        len = BUF_SIZE;
    }
    if (len < 12) {
        return -1;
    }
    int err = ETHIF_TX_COMPLETE;
    int id = client_get_sender_id();
    client_t *client = NULL;
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].client_id == id) {
            client = &clients[i];
        }
    }
    assert(client);
    void *packet = client->dataport;
    error = ethdriver_lock();
    /* silently drop packets */
    if (client->num_tx != 0) {
        client->num_tx --;
        tx_buf_t *tx_buf = client->tx[client->num_tx];
        /* copy the packet over */
        memcpy(tx_buf->buf, packet, len);
        memcpy(tx_buf->buf + 6, client->mac, 6);
        /* queue up transmit */
        err = eth_driver.i_fn.raw_tx(&eth_driver, 1, (uintptr_t*)&(tx_buf->buf), (unsigned int*)&len, tx_buf);
        if (err != ETHIF_TX_ENQUEUED){
            /* Free the internal tx buffer in case tx fails. Up to the client to retry the trasmission */
            client->num_tx++; 
        }
    }
    error = ethdriver_unlock();

    return err;
}

void client_mac(uint8_t *b1, uint8_t *b2, uint8_t *b3, uint8_t *b4, uint8_t *b5, uint8_t *b6) {
    int UNUSED error;
    int id = client_get_sender_id();
    client_t *client = NULL;
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].client_id == id) {
            client = &clients[i];
        }
    }
    assert(client);
    assert(done_init);
    error = ethdriver_lock();
    *b1 = client->mac[0];
    *b2 = client->mac[1];
    *b3 = client->mac[2];
    *b4 = client->mac[3];
    *b5 = client->mac[4];
    *b6 = client->mac[5];
    error = ethdriver_unlock();
}

void irq_handle() {
    int UNUSED error;
    error = ethdriver_lock();
    eth_driver.i_fn.raw_handleIRQ(&eth_driver, 0);
    error = irq_acknowledge();
    error = ethdriver_unlock();
}

/* Returns the cap to the frame mapped to vaddr, assuming
 * vaddr points inside our dma pool. */
static seL4_CPtr get_dma_frame_cap(vspace_t *vspace, void *vaddr) {
    seL4_CPtr cap = camkes_dma_get_cptr(vaddr);
    if (cap == seL4_CapNull) {
        return original_vspace_get_cap(vspace, vaddr);
    }
    return cap;
}

/* Allocate a dma buffer backed by the component's dma pool */
static void* camkes_iommu_dma_alloc(void *cookie, size_t size,
        int align, int cached, ps_mem_flags_t flags) {

    // allocate buffer from the dma pool
    void* vaddr = camkes_dma_alloc(size, align);
    if (vaddr == NULL) {
        return NULL;
    }
    int error = sel4utils_iommu_dma_alloc_iospace(cookie, vaddr, size);
    if (error) {
        camkes_dma_free(vaddr, size);
        return NULL;
    }
    return vaddr;
}

/* depending on the actual component a different init function will be linked against */
int ethif_init(struct eth_driver *eth_driver, ps_io_ops_t io_ops, void *config);

void post_init(void) {
    int error;
    int pci_bdf_int;
    int bus, dev, fun;
    error = ethdriver_lock();
    /* initialize seL4 allocators and give us a half sane environment */
    init_system();
    /* initialize the driver */
    ps_io_ops_t ioops;
    cspacepath_t iospace;
    error = vka_cspace_alloc_path(&vka, &iospace);
    assert(!error);
    sscanf(pci_bdf, "%x:%x.%d", &bus, &dev, &fun);
    pci_bdf_int = bus * 256 + dev * 8 + fun;
    /* get this from the configuration */
    error = simple_get_iospace(&camkes_simple, iospace_id, pci_bdf_int, &iospace);
    assert(!error);

    /* Save a pointer to the original get_cap function for our vspace */
    original_vspace_get_cap = vspace.get_cap;

    /* The iommu driver needs the caps to frames backing the dma buffer.
     * It will invoke the get_cap method of its vspace to get these caps.
     * Since the vspace we give to the iommu driver wasn't used to allocate
     * the dma buffer, it doesn't know the caps to the frames backing the
     * buffer. CAmkES allocated the buffer statically, and so the caps are
     * known to it. Here, we override the get_cap method of our vspace to
     * return dma buffer frame caps provided by CAmkES. */
    vspace.get_cap = get_dma_frame_cap;

    error = sel4utils_make_iommu_dma_alloc(&vka, &vspace, &ioops.dma_manager, 1, &iospace.capPtr);
    assert(!error);
    ioops.dma_manager.dma_alloc_fn = camkes_iommu_dma_alloc;

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
    num_clients = client_num_badges();
    clients = calloc(num_clients, sizeof(client_t));
    for (int client = 0; client < num_clients; client++) {
        clients[client].should_notify = 1;
        clients[client].client_id = client_enumerate_badge(client);
        clients[client].dataport = client_buf(clients[client].client_id);
        client_get_mac(clients[client].client_id, clients[client].mac);
        for (int i = 0; i < CLIENT_TX_BUFS; i++) {
            void *buf = ps_dma_alloc(&ioops.dma_manager, BUF_SIZE, 4, 1, PS_MEM_NORMAL);
            assert(buf);
            uintptr_t phys = ps_dma_pin(&ioops.dma_manager, buf, BUF_SIZE);
            assert(phys == (uintptr_t)buf);
            tx_buf_t *tx_buf = &clients[client].tx_mem[clients[client].num_tx];
            *tx_buf = (tx_buf_t){.buf = buf, .len = BUF_SIZE, .client = client};
            clients[client].tx[clients[client].num_tx] = tx_buf;
            clients[client].num_tx++;
        }
    }
    eth_driver.cb_cookie = NULL;
    eth_driver.i_cb = ethdriver_callbacks;
    ethif_intel_config_t eth_config = (ethif_intel_config_t) {
        .bar0 = (void*)EthDriver
    };

    error = ethif_init(&eth_driver, ioops, &eth_config);

    assert(!error);
    done_init = 1;
    error = irq_acknowledge();
    error = ethdriver_unlock();
}

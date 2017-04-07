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

#include <autoconf.h>

#include <string.h>
#include <camkes.h>
#include <ethdrivers/pico_dev_eth.h>
#include <pico_stack.h>
#include <pico_socket.h>
#include <pico_addressing.h>
#include <pico_ipv4.h>
#include <sel4/sel4.h>
#include <sel4utils/sel4_zf_logif.h>

static void low_level_init(struct eth_driver *driver, uint8_t *mac, int *mtu) {
    // 1500 is the standard ethernet MTU at the network layer.
    *mtu = 1500;
    ethdriver_mac(&mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5]);
}

extern void *ethdriver_buf;

// Needed for the picotcp from pico_sel4.h
// Although time is not needed for UDP, it is used in the arp update.
volatile unsigned long int pico_ms_tick = 0;

static void raw_poll(struct eth_driver *driver) {
    int len;
    int status;
    status = ethdriver_rx(&len);
    while (status != -1) {
        void *buf;
        void *cookie;
        buf = (void*)driver->i_cb.allocate_rx_buf(driver->cb_cookie, len, &cookie);
        if (buf){
            // Only proceed if we successfully got a buffer. If not, the packet will simply be dropped
            // which is OK with UDP. This prevents crashing in a DDOS attack or malicious packet that is too large.
            memcpy(buf, (void*)ethdriver_buf, len);
            driver->i_cb.rx_complete(driver->cb_cookie, 1, &cookie, (unsigned int*)&len);
        } else {
            ZF_LOGE("Failed to allocate RX buffer");
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

static int raw_tx(struct eth_driver *driver, unsigned int num, uintptr_t *phys, unsigned int *len, void *cookie) {
    unsigned int total_len = 0;
    int i;
    void *p = (void*)ethdriver_buf;
    for (i = 0; i < num; i++) {
        memcpy(p + total_len, (void*)phys[i], len[i]);
        total_len += len[i];
    }
    ethdriver_tx(total_len);
    return ETHIF_TX_COMPLETE;
}

static void handle_irq(struct eth_driver *driver, int irq) {
    pico_stack_tick();
    raw_poll(driver);
}

static struct raw_iface_funcs iface_fns = {
    .raw_handleIRQ = handle_irq,
    .print_state = NULL,
    .low_level_init = low_level_init,
    .raw_tx = raw_tx,
    .raw_poll = raw_poll
};

static int ethdriver_init(struct eth_driver *eth_driver, ps_io_ops_t io_ops, void *config) {
    eth_driver->eth_data = NULL;
    eth_driver->dma_alignment = 1;
    eth_driver->i_fn = iface_fns;
    return 0;
}

static void* malloc_dma_alloc(void *cookie, size_t size, int align, int cached, ps_mem_flags_t flags) {
    assert(cached);
    int error;
    void *ret = malloc(size);
    if (ret == NULL){
        ZF_LOGE("ERR: Failed to allocate %d\n", size);
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

static ps_io_ops_t io_ops;
static pico_device_eth _picotcp_driver;

void pre_init(void) {
    struct pico_ip4 netmask, ipaddr, gw, multicast, zero = {0};
    pico_device_eth *pico_driver;
    memset(&io_ops, 0, sizeof(io_ops));
    io_ops.dma_manager = (ps_dma_man_t) {
        .cookie = NULL,
        .dma_alloc_fn = malloc_dma_alloc,
        .dma_free_fn = malloc_dma_free,
        .dma_pin_fn = malloc_dma_pin,
        .dma_unpin_fn = malloc_dma_unpin,
        .dma_cache_op_fn = malloc_dma_cache_op
    };

    /* Initialise the PicoTCP stack */ 
    pico_stack_init();

    /* Create a driver. This utilises preallocated buffers, backed up by malloc above */
    pico_driver = pico_eth_create_no_malloc("eth0", ethdriver_init, NULL, io_ops, &_picotcp_driver);   
    ZF_LOGF_IF(pico_driver == NULL, "Failed to create the PicoTCP Driver");

    pico_string_to_ipv4("0.0.0.0", &gw.addr);
    pico_string_to_ipv4(udp_ip_addr, &ipaddr.addr);
    pico_string_to_ipv4(multicast_addr, &multicast.addr);
    pico_string_to_ipv4("255.255.255.0", &netmask.addr);

    pico_ipv4_link_add(pico_driver, ipaddr, netmask);
    pico_ipv4_route_add(zero, zero, gw, 1, NULL);

    if (pico_ipv4_is_multicast(multicast.addr)) {
        ZF_LOGE("Multicast not yet implemented\n");
        // PicoTCP usually deals with multicast at the socket layer, using pico_socket_setoption. 
        // It can be done at the igmp level too by using igmp_state_change. See the picoTCP documentation
        // Eg: pico_igmp_state_change(&ipaddr, &multicast, .... );
    }
    
}

/* Provided by the Ethdriver template */
seL4_CPtr ethdriver_notification(void);

int run() {
    while(1) {
        // Ethdriver connecter does not have a callback or camkes event wait setup. 
        seL4_Wait(ethdriver_notification(), NULL);
        picotcp_lock();
        pico_stack_tick();
        
        // Picotcp needs a ms tick - which is just a number inside the stack that gets incremented. 
        // This value is used in places like the ARP request which times out or becomes stale after some time has passed. 
        // The tick at the moment is an arbitrary number, but some/any kind of increment needs to happen for arp to work.
        pico_ms_tick += 100; 
        ethif_pico_handle_irq(&_picotcp_driver, 0);
        pico_stack_tick();
        picotcp_unlock();
    }
}


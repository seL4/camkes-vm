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

#include <string.h>
#include <UDPServer.h>
#include <ethdrivers/lwip.h>
#include <lwip/udp.h>
#include <netif/etharp.h>
#include <lwip/init.h>

static void low_level_init(struct eth_driver *driver, uint8_t *mac, int *mtu) {
    *mtu = 1500;
    ethdriver_mac(&mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5]);
}

static void raw_poll(struct eth_driver *driver) {
    int len;
    int status;
    status = ethdriver_rx(&len);
    while (status != -1) {
        void *buf;
        void *cookie;
        buf = (void*)driver->i_cb.allocate_rx_buf(driver->cb_cookie, len, &cookie);
        assert(buf);
        memcpy(buf, packet, len);
        driver->i_cb.rx_complete(driver->cb_cookie, 1, &cookie, (unsigned int*)&len);
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
    void *p = (void*)packet;
    for (i = 0; i < num; i++) {
        memcpy(p + total_len, (void*)phys[i], len[i]);
        total_len += len[i];
    }
    ethdriver_tx(total_len);
    return ETHIF_TX_COMPLETE;
}

static void handle_irq(struct eth_driver *driver, int irq) {
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

static void have_packet(void *cookie) {
    lwip_iface_t *lwip_driver = (lwip_iface_t*)cookie;
    ethif_lwip_handle_irq(lwip_driver, 0);
    eth_rx_ready_reg_callback(have_packet, cookie);
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

static ps_io_ops_t io_ops;
static struct netif _netif;
static lwip_iface_t _lwip_driver;

void pre_init(void) {
    struct ip_addr netmask, ipaddr, gw;
    struct netif *netif;
    set_putchar(putchar_putchar);
    lwip_iface_t *lwip_driver;
    memset(&io_ops, 0, sizeof(io_ops));
    io_ops.dma_manager = (ps_dma_man_t) {
        .cookie = NULL,
        .dma_alloc_fn = malloc_dma_alloc,
        .dma_free_fn = malloc_dma_free,
        .dma_pin_fn = malloc_dma_pin,
        .dma_unpin_fn = malloc_dma_unpin,
        .dma_cache_op_fn = malloc_dma_cache_op
    };
    lwip_driver = ethif_new_lwip_driver_no_malloc(io_ops, &io_ops.dma_manager, ethdriver_init, NULL, &_lwip_driver);
    assert(lwip_driver);
    ipaddr_aton("0.0.0.0",      &gw);
    ipaddr_aton(udp_ip_addr,  &ipaddr);
    ipaddr_aton("255.255.255.0", &netmask);
    lwip_init();
    netif = netif_add(&_netif, &ipaddr, &netmask, &gw, lwip_driver, ethif_get_ethif_init(lwip_driver),
                       ethernet_input);
    assert(netif);
    netif_set_up(netif);
    netif_set_default(netif);
}

void post_init(void) {
    eth_rx_ready_reg_callback(have_packet, &_lwip_driver);
}

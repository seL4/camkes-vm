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

/* Based off UDPServer component.
 * A sample app that demonstrates the use of TCP using PicoTCP as a library.
 * The component utilises the network library to create a TCP socket and respond.
 *
 * There are two separate code examples/usages below:
 * - One is an echo component. This will simply listen for an incoming connection and reply.
 * - The other is a TCP client that
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
#include <utils/time.h>

#define PICO_TICK_MS 50

static void low_level_init(struct eth_driver *driver, uint8_t *mac, int *mtu) {
    *mtu = 1500;
    ethdriver_mac(&mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5]);
}

extern void *ethdriver_buf;

// Needed for the picotcp from pico_sel4.h
volatile unsigned long int pico_ms_tick = 0;

static void raw_poll(struct eth_driver *driver) {
    int len;
    int status;
    status = ethdriver_rx(&len);
    while (status != -1) {
        void *buf;
        void *cookie;
        buf = (void*)driver->i_cb.allocate_rx_buf(driver->cb_cookie, len, &cookie);
        if(buf){
            // Only proceed if there are buffers available. If not, the packet will be dropped.
            // In such sitations, the TCP stack will not receive the data or ack and should
            // automatically retry.
            memcpy(buf, (void*)ethdriver_buf, len);
            driver->i_cb.rx_complete(driver->cb_cookie, 1, &cookie, (unsigned int*)&len);
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
    assert(pico_driver);

    pico_string_to_ipv4("0.0.0.0", &gw.addr);
    pico_string_to_ipv4(server_ip_addr, &ipaddr.addr);
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

    /* Start the timer for tcp */
    // TCP runs off a tick for handling events and timeouts.
    timer_periodic(0, NS_IN_MS * PICO_TICK_MS);

    start_tcpecho();

}

/* Provided by the Ethdriver template */
seL4_CPtr ethdriver_notification(void);

// A callback is used to do timekeeping for TCP
void timer_complete_callback() {
    picotcp_lock();
    pico_stack_tick();
    pico_ms_tick += PICO_TICK_MS;
    ethif_pico_handle_irq(&_picotcp_driver, 0);
    pico_stack_tick();
    picotcp_unlock();
}

int run() {
    ZF_LOGD("Begin TCP server component");
}

/*
 * TCP code. For TCP, a socket is created and listens.
 * A callback occurs when the packet is received. The callback handles the different types of TCP events,
 * including the handshake protocol.
 * Based off the sample tcpecho app provided in the picotcp library.
 */
void cb_tcpecho(uint16_t ev, struct pico_socket *s)
{
    int r = 0;

    ZF_LOGD("tcpecho> wakeup ev=%u\n", ev);

    if (ev & PICO_SOCK_EV_RD) {
        char buf[100];
        r = pico_socket_read(s, buf, 100);
        pico_socket_write(s, buf, r); // Immediately echo back a response
    }

    if (ev & PICO_SOCK_EV_CONN) {
        uint32_t ka_val = 0;
        struct pico_socket *sock_peer;
        struct pico_ip4 orig = {0};
        uint16_t port = 0;
        char peer[30] = {0};
        int yes = 1;

        sock_peer = pico_socket_accept(s, &orig, &port);
        pico_ipv4_to_string(peer, orig.addr);
        ZF_LOGD("Connection established with %s:%d.\n", peer, short_be(port));
        pico_socket_setoption(sock_peer, PICO_TCP_NODELAY, &yes);
        /* Set keepalive options - from picoTCP documentation */
        ka_val = 5;
        pico_socket_setoption(sock_peer, PICO_SOCKET_OPT_KEEPCNT, &ka_val);
        ka_val = 30000;
        pico_socket_setoption(sock_peer, PICO_SOCKET_OPT_KEEPIDLE, &ka_val);
        ka_val = 5000;
        pico_socket_setoption(sock_peer, PICO_SOCKET_OPT_KEEPINTVL, &ka_val);
    }

    if (ev & PICO_SOCK_EV_FIN) {
        ZF_LOGD("Socket closed. Exit normally. \n");
    }

    if (ev & PICO_SOCK_EV_ERR) {
        ZF_LOGD("Socket error received: %s. Bailing out.\n", strerror(pico_err));
    }

    if (ev & PICO_SOCK_EV_CLOSE) {
        ZF_LOGD("Socket received close from peer.\n");
        pico_socket_shutdown(s, PICO_SHUT_WR);
    }

    if (ev & PICO_SOCK_EV_WR) {
        ZF_LOGD("TCP Write ack\n");
    }
}

void start_tcpecho()
{
    uint16_t listen_port = short_be(8777);
    int ret = 0, yes = 1;
    struct pico_socket *s = NULL;
    struct pico_ip4 ip4 = {0};

    s = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_TCP, &cb_tcpecho);

    if (!s) {
        ZF_LOGE("%s: error opening socket: %s\n", __FUNCTION__, strerror(pico_err));
    }

    pico_socket_setoption(s, PICO_TCP_NODELAY, &yes);

    ret = pico_socket_bind(s, &ip4, &listen_port);

    if (ret < 0) {
        ZF_LOGE("%s: error binding socket to port %u: %s\n", __FUNCTION__, short_be(listen_port), strerror(pico_err));
    }

    if (pico_socket_listen(s, 40) != 0) {
        ZF_LOGE("%s: error listening on port %u\n", __FUNCTION__, short_be(listen_port));
    }

    ZF_LOGD("Launching PicoTCP echo server\n");
}

// A test as a TCP client
// The client conencts to an external remote and sends some packets.

void cb_tcpclient(uint16_t ev, struct pico_socket *s)
{
    static int w_size = 0;
    static int r_size = 0;
    static int closed = 0;
    int r, w;
    char buf[1024];

    ZF_LOGD("tcpclient> wakeup %lu, event %u\n", count, ev);

    if (ev & PICO_SOCK_EV_RD) {
        do {
            r = pico_socket_read(s, buf, 1024);
            if (r > 0) {
                printf("READ >>");
                fwrite(buf, r, 1, stdout);
            }

            if (r < 0){
                ZF_LOGF("READ ERROR\n");
            }
        } while(r > 0);
    }

    if (ev & PICO_SOCK_EV_CONN) {
        ZF_LOGD("Connection established with server.\n");
    }

    if (ev & PICO_SOCK_EV_FIN) {
        ZF_LOGD("Socket closed. Exit normally. \n");
    }

    if (ev & PICO_SOCK_EV_ERR) {
        ZF_LOGD("Socket error received: %s. Bailing out.\n", strerror(pico_err));
    }

    if (ev & PICO_SOCK_EV_CLOSE) {
        ZF_LOGD("Socket received close from peer - Wrong case if not all client data sent!\n");
        pico_socket_close(s);
    }

    if (ev & PICO_SOCK_EV_WR) {
        ZF_LOGD("Pico socket write cb\n");
    }
}

void app_tcpclient()
{
    char *daddr = NULL, *dport = NULL;
    uint16_t send_port = 0, listen_port = short_be(5555);
    int i = 0, ret = 0, yes = 1;
    struct pico_socket *s = NULL;
    struct pico_ip4 dst = {0};
    struct pico_ip4 inaddr = {0};

    ZF_LOGD("Connecting to: %s:%d\n", daddr, short_be(send_port));

    s = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_TCP, &cb_tcpclient);

    if (!s) {
        ZF_LOGE("%s: error opening socket: %s\n", __FUNCTION__, strerror(pico_err));
    }

    pico_socket_setoption(s, PICO_TCP_NODELAY, &yes);

    ret = pico_socket_bind(s, &inaddr, &listen_port);

    if (ret < 0) {
        ZF_LOGE("%s: error binding socket to port %u: %s\n", __FUNCTION__, short_be(listen_port), strerror(pico_err));
    }

    // Set the destination address (string) above in the usual xxx.xxx.xxx.xxx ip4 way.
    pico_string_to_ipv4(daddr, &dst.addr);

    ret = pico_socket_connect(s, &dst, send_port);

    if (ret < 0) {
        ZF_LOGE("%s: error connecting to %s:%u: %s\n", __FUNCTION__, daddr, short_be(send_port), strerror(pico_err));
    }

    ZF_LOGD("TCP client connected\n");

}



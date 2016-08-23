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
#include <stdio.h>
#include <stdint.h>

#include <sel4/sel4.h>
#include <camkes.h>
#include "vm.h"

/* configuration */
#define BAUD_RATE 115200

/* register layout. Done by offset from base port */
#define THR_ADDR (0)
#define RBR_ADDR (0)
#define LATCH_LOW_ADDR (0)
#define LATCH_HIGH_ADDR (1)
#define IER_ADDR (1)
#define FCR_ADDR (2)
#define IIR_ADDR (2)
#define LCR_ADDR (3)
#define MCR_ADDR (4)
#define LSR_ADDR (5)
#define MSR_ADDR (6)

#define IER_RESERVED_MASK (BIT(6) | BIT(7))

#define FCR_ENABLE BIT(0)
#define FCR_CLEAR_RECEIVE BIT(1)
#define FCR_CLEAR_TRANSMIT BIT(2)
#define FCR_TRIGGER_16_1 (0)

#define LCR_DLAB BIT(7)

#define MCR_DTR BIT(0)
#define MCR_RTS BIT(1)
#define MCR_AO1 BIT(2)
#define MCR_AO2 BIT(3)

#define LSR_EMPTY_DHR BIT(6)
#define LSR_EMPTY_THR BIT(5)
#define LSR_DATA_READY BIT(0)

#define IIR_FIFO_ENABLED (BIT(6) | BIT(7))
#define IIR_REASON (BIT(1) | BIT(2) | BIT(3))
#define IIR_MSR (0)
#define IIR_THR BIT(1)
#define IIR_RDA BIT(2)
#define IIR_TIME (BIT(3) | BIT(2))
#define IIR_LSR (BIT(2) | BIT(1))
#define IIR_PENDING BIT(0)

/* No background */

// Standard colours
#define COLOUR_R "\033[31m"
#define COLOUR_G "\033[32m"
#define COLOUR_Y "\033[33m"
#define COLOUR_B "\033[34m"
#define COLOUR_M "\033[35m"
#define COLOUR_C "\033[36m"

// Bright colours
#define COLOUR_BR "\033[31;1m"
#define COLOUR_BG "\033[32;1m"
#define COLOUR_BY "\033[33;1m"
#define COLOUR_BB "\033[34;1m"
#define COLOUR_BM "\033[35;1m"
#define COLOUR_BC "\033[36;1m"


/* Grey background */

// Standard colours
#define COLOUR_BG_R "\033[31;40m"
#define COLOUR_BG_G "\033[32;40m"
#define COLOUR_BG_Y "\033[33;40m"
#define COLOUR_BG_B "\033[34;40m"
#define COLOUR_BG_M "\033[35;40m"
#define COLOUR_BG_C "\033[36;40m"

// Bright colours
#define COLOUR_BG_BR "\033[31;1;40m"
#define COLOUR_BG_BG "\033[32;1;40m"
#define COLOUR_BG_BY "\033[33;1;40m"
#define COLOUR_BG_BB "\033[34;1;40m"
#define COLOUR_BG_BM "\033[35;1;40m"
#define COLOUR_BG_BC "\033[36;1;40m"


#define COLOUR_RESET "\033[0m"

#define MAX_GUESTS 12
#define GUEST_OUTPUT_BUFFER_SIZE 256

typedef struct getchar_buffer {
    uint32_t head;
    uint32_t tail;
    char buf[4096 - 8];
} getchar_buffer_t;

compile_time_assert(getchar_buf_sized, sizeof(getchar_buffer_t) == sizeof(Buf));

typedef struct getchar_client {
    unsigned int client_id;
    volatile getchar_buffer_t *buf;
    uint32_t last_head;
} getchar_client_t;

static int last_out = -1;

static int fifo_depth = 1;
static int fifo_used = 0;

static uint8_t output_buffers[MAX_GUESTS * 2][GUEST_OUTPUT_BUFFER_SIZE];
static int output_buffers_used[MAX_GUESTS * 2] = { 0 };

static int done_output = 0;

static int has_data = 0;

static int num_getchar_clients = 0;
static getchar_client_t *getchar_clients = NULL;

/* We predefine output colours for clients */
const char *all_output_colours[MAX_GUESTS * 2] = {
    /* VMMs */
    COLOUR_R,
    COLOUR_G,
    COLOUR_B,
    COLOUR_M,
    COLOUR_Y,
    COLOUR_C,
    COLOUR_BR,
    COLOUR_BG,
    COLOUR_BB,
    COLOUR_BM,
    COLOUR_BY,
    COLOUR_BC,

    /* Guests */
    COLOUR_BG_R,
    COLOUR_BG_G,
    COLOUR_BG_B,
    COLOUR_BG_M,
    COLOUR_BG_Y,
    COLOUR_BG_C,
    COLOUR_BG_BR,
    COLOUR_BG_BG,
    COLOUR_BG_BB,
    COLOUR_BG_BM,
    COLOUR_BG_BY,
    COLOUR_BG_BC,
};

static inline void write_ier(uint8_t val) {
    serial_port_out8_offset(IER_ADDR, val);
}
static inline uint8_t read_ier() {
    return serial_port_in8_offset(IER_ADDR);
}

static inline void write_lcr(uint8_t val) {
    serial_port_out8_offset(LCR_ADDR, val);
}
static inline uint8_t read_lcr() {
    return serial_port_in8_offset(LCR_ADDR);
}

static inline void write_fcr(uint8_t val) {
    serial_port_out8_offset(FCR_ADDR, val);
}
/* you cannot read the fcr */

static inline void write_mcr(uint8_t val) {
    serial_port_out8_offset(MCR_ADDR, val);
}

static inline uint8_t read_lsr() {
    return serial_port_in8_offset(LSR_ADDR);
}

static inline uint8_t read_rbr() {
    return serial_port_in8_offset(RBR_ADDR);
}

static inline void write_thr(uint8_t val) {
    serial_port_out8_offset(THR_ADDR, val);
}

static inline uint8_t read_iir() {
    return serial_port_in8_offset(IIR_ADDR);
}

static inline uint8_t read_msr() {
    return serial_port_in8_offset(MSR_ADDR);
}

static void wait_for_fifo() {
    while(! (read_lsr() & (LSR_EMPTY_DHR | LSR_EMPTY_THR)));
    fifo_used = 0;
}

static void serial_putchar(int c) {
    /* check how much fifo we've used and if we need to drain it */
    if (fifo_used == fifo_depth) {
        wait_for_fifo();
    }
    write_thr((uint8_t)c);
    fifo_used++;
}

static void flush_buffer(int b) {
    const char *col = all_output_colours[b];
    int i;
    if (output_buffers_used[b] == 0) {
        return;
    }
    if (b != last_out) {
        printf("%s%s", COLOUR_RESET, col);
        last_out = b;
    }
    for (i = 0; i < output_buffers_used[b]; i++) {
        printf("%c", output_buffers[b][i]);
    }
    done_output = 1;
    output_buffers_used[b] = 0;
    fflush(stdout);
}

static int is_newline(const uint8_t *c) {
    return (c[0] == '\r' && c[1] == '\n') || (c[0] == '\n' && c[1] == '\r');
}

static void internal_putchar(int b, int c) {
    serial_lock();
    /* Add to buffer */
    int index = output_buffers_used[b];
    uint8_t *buffer = output_buffers[b];
    buffer[index] = (uint8_t)c;
    output_buffers_used[b]++;
    if (index + 1 == GUEST_OUTPUT_BUFFER_SIZE || (index >= 1 && is_newline(buffer + index - 1)) || last_out == b) {
        flush_buffer(b);
    }
    has_data = 1;
    serial_unlock();
}

static void internal_guest_putchar(int guest, int c) {
    getchar_client_t *client = &getchar_clients[guest];
    uint32_t next_tail = (client->buf->tail + 1) % sizeof(client->buf->buf);
    if ( next_tail == client->buf->head) {
        /* full */
        return;
    }
    uint32_t last_read_head = client->buf->head;
    client->buf->buf[client->buf->tail] = (uint8_t)c;
    /* no synchronize in here as we assume TSO */
    client->buf->tail = next_tail;
    __sync_synchronize();
    if (last_read_head != client->last_head) {
        getchar_emit(client->client_id);
        client->last_head = last_read_head;
    }
}

static int statemachine = 1;

static int active_guest = 0;

static void give_guest_char(uint8_t c) {
    internal_guest_putchar(active_guest, c);
}

static void handle_char(uint8_t c) {
    /* some manually written state machine magic to detect switching of input direction */
    switch (statemachine) {
    case 0:
        if (c == '\r' || c == '\n') {
            statemachine = 1;
        }
        give_guest_char(c);
        break;
    case 1:
        if (c == '~') {
            statemachine = 2;
        } else {
            statemachine = 0;
            give_guest_char(c);
        }
        break;
    case 2:
        if (c == '~') {
            statemachine = 0;
            give_guest_char(c);
        } else if (c >= '0' && c < '0' + VM_NUM_GUESTS) {
            last_out = -1;
            int guest = c - '0';
            printf(COLOUR_RESET "\r\nSwitching input to %d\r\n",guest);
            active_guest = guest;
            statemachine = 1;
        } else {
            statemachine = 0;
            give_guest_char('~');
            give_guest_char(c);
        }
        break;
    }
}

/* assume DLAB == 1*/
static inline void write_latch_high(uint8_t val) {
    serial_port_out8_offset(LATCH_HIGH_ADDR, val);
}
static inline void write_latch_low(uint8_t val) {
    serial_port_out8_offset(LATCH_LOW_ADDR, val);
}

static void set_dlab(int v) {
    if (v) {
        write_lcr(read_lcr() | LCR_DLAB);
    } else {
        write_lcr(read_lcr() & ~LCR_DLAB);
    }
}

static inline void write_latch(uint16_t val) {
    set_dlab(1);
    write_latch_high(val >> 8);
    write_latch_low(val & 0xff);
    set_dlab(0);
}

static void disable_interrupt() {
    write_ier(0);
}

static void disable_fifo() {
    /* first attempt to use the clear fifo commands */
    write_fcr(FCR_CLEAR_TRANSMIT | FCR_CLEAR_RECEIVE);
    /* now disable with a 0 */
    write_fcr(0);
}

static void set_baud_rate(uint32_t baud) {
    assert(baud != 0);
    assert(115200 % baud == 0);
    uint16_t divisor = 115200 / baud;
    write_latch(divisor);
}

static void reset_state() {
    /* clear internal global state here */
    fifo_used = 0;
}

static void enable_fifo() {
    /* check if there is a fifo and how deep it is */
    uint8_t info = read_iir();
    if ((info & IIR_FIFO_ENABLED) == IIR_FIFO_ENABLED) {
        fifo_depth = 16;
        write_fcr(FCR_TRIGGER_16_1 | FCR_ENABLE);
    } else {
        fifo_depth = 1;
    }
}

static void reset_lcr() {
    /* set 8-n-1 */
    write_lcr(3);
}

static void reset_mcr() {
    write_mcr(MCR_DTR | MCR_RTS | MCR_AO1 | MCR_AO2);
}

static void clear_iir() {
    uint8_t iir;
    while (! ((iir = read_iir()) & IIR_PENDING)) {
        switch(iir & IIR_REASON) {
        case IIR_MSR:
            read_msr();
            break;
        case IIR_THR:
            break;
        case IIR_RDA:
        case IIR_TIME:
            while (read_lsr() & LSR_DATA_READY) {
                handle_char(read_rbr());
            }
            break;
        case IIR_LSR:
            read_lsr();
            break;
        }
    }
}

static void enable_interrupt() {
    write_ier(1);
}

static void serial_irq(void *cookie) {
    serial_lock();
    clear_iir();
    serial_irq_reg_callback(serial_irq, cookie);
    serial_unlock();
}

static void timer_callback(void *data) {
    serial_lock();
    if (done_output) {
        done_output = 0;
    } else if (has_data) {
        /* flush everything if no writes since last callback */
        int i;
        for (i = 0; i < MAX_GUESTS * 2; i++) {
            flush_buffer(i);
        }
    }
    serial_unlock();
}

seL4_CPtr timeout_aep(void);

int run(void) {
    seL4_CPtr aep = timeout_aep();
    while(1) {
        seL4_Wait(aep, NULL);
        timer_callback(NULL);
    }
    return 0;
}

void pre_init(void) {
    serial_lock();
    // Initialize the serial port
    set_dlab(0); // we always assume the dlab is 0 unless we explicitly change it
    disable_interrupt();
    disable_fifo();
    reset_lcr();
    reset_mcr();
    clear_iir();
    set_baud_rate(BAUD_RATE);
    reset_state();
    enable_fifo();
    enable_interrupt();
    clear_iir();
    // all done
    /* query what getchar clients exist */
    num_getchar_clients = getchar_num_badges();
    getchar_clients = calloc(num_getchar_clients, sizeof(getchar_client_t));
    for (int i = 0; i < num_getchar_clients; i++) {
        unsigned int badge = getchar_enumerate_badge(i);
        assert(badge <= num_getchar_clients);
        getchar_clients[badge].client_id = badge;
        getchar_clients[badge].buf = getchar_buf(badge);
        getchar_clients[badge].last_head = -1;
    }
    set_putchar(serial_putchar);
    serial_irq_reg_callback(serial_irq, 0);
    /* Start regular heartbeat of 500ms */
    timeout_periodic(0, 500000000);
    serial_unlock();
}

seL4_Word vm_putchar_get_sender_id(void);

void vm_putchar_putchar(int c) {
    seL4_Word n = vm_putchar_get_sender_id();
    internal_putchar((int)n, c);
    if (c == '\n') {
        internal_putchar(n, '\r');
    }
}

seL4_Word guest_putchar_get_sender_id(void);

void guest_putchar_putchar(int c) {
    seL4_Word n = guest_putchar_get_sender_id();
    internal_putchar((int)n + MAX_GUESTS, c);
}

/* We had to define at least one function in the getchar RPC procedure
 * so now we need to implement it even though it is not used */
void getchar_foo(void) {
    assert(!"should not be reached");
}

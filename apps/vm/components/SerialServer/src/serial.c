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
#include <stdio.h>

#include <SerialServer.h>

/* configuration */
#define BAUD_RATE 115200
#define COM_BASE 0x3f8

/* register layout */
#define THR_ADDR (COM_BASE + 0)
#define RBR_ADDR (COM_BASE + 0)
#define LATCH_LOW_ADDR (COM_BASE + 0)
#define LATCH_HIGH_ADDR (COM_BASE + 1)
#define IER_ADDR (COM_BASE + 1)
#define FCR_ADDR (COM_BASE + 2)
#define IIR_ADDR (COM_BASE + 2)
#define LCR_ADDR (COM_BASE + 3)
#define MCR_ADDR (COM_BASE + 4)
#define LSR_ADDR (COM_BASE + 5)
#define MSR_ADDR (COM_BASE + 6)

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

#define COLOUR_R "\033[;1;31m"
#define COLOUR_G "\033[;1;32m"
#define COLOUR_Y "\033[;1;33m"
#define COLOUR_B "\033[;1;34m"
#define COLOUR_M "\033[;1;35m"
#define COLOUR_C "\033[;1;36m"
#define COLOUR_RESET "\033[0m"

#define GUEST_BUFFER_SIZE 64
#define NUM_GUESTS 1
#define GUEST_OUTPUT_BUFFER_SIZE 256

static int last_out = -1;

static int fifo_depth = 1;
static int fifo_used = 0;

static uint8_t guest_buffers[NUM_GUESTS][GUEST_BUFFER_SIZE];
static int guest_buffer_start[NUM_GUESTS] = { 0 };
static int guest_buffer_end[NUM_GUESTS] = { 0 };

static uint8_t output_buffers[NUM_GUESTS * 2][GUEST_OUTPUT_BUFFER_SIZE];
static int output_buffers_used[NUM_GUESTS * 2] = { 0 };

static int done_output = 0;

static int has_data = 0;

const char *output_colours[NUM_GUESTS * 2] = {
    /* VMMs */
    COLOUR_R,
    COLOUR_G,
    COLOUR_B,
    /* Guests */
    COLOUR_M,
    COLOUR_Y,
    COLOUR_C
};

static inline void write_ier(uint8_t val) {
    serial_port_out8(IER_ADDR, val);
}
static inline uint8_t read_ier() {
    return serial_port_in8(IER_ADDR);
}

static inline void write_lcr(uint8_t val) {
    serial_port_out8(LCR_ADDR, val);
}
static inline uint8_t read_lcr() {
    return serial_port_in8(LCR_ADDR);
}

static inline void write_fcr(uint8_t val) {
    serial_port_out8(FCR_ADDR, val);
}
/* you cannot read the fcr */

static inline void write_mcr(uint8_t val) {
    serial_port_out8(MCR_ADDR, val);
}

static inline uint8_t read_lsr() {
    return serial_port_in8(LSR_ADDR);
}

static inline uint8_t read_rbr() {
    return serial_port_in8(RBR_ADDR);
}

static inline void write_thr(uint8_t val) {
    serial_port_out8(THR_ADDR, val);
}

static inline uint8_t read_iir() {
    return serial_port_in8(IIR_ADDR);
}

static inline uint8_t read_msr() {
    return serial_port_in8(MSR_ADDR);
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
    const char *col = output_colours[b];
    int i;
    if (output_buffers_used[b] == 0) {
        return;
    }
    if (b != last_out) {
        printf("%s", col);
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

static int internal_pollchar(int guest, int *out) {
    int ret = 0;
    serial_lock();
    assert(guest < NUM_GUESTS);
    if (guest_buffer_start[guest] != guest_buffer_end[guest]) {
        ret = 1;
        *out = guest_buffers[guest][guest_buffer_start[guest]];
        guest_buffer_start[guest]++;
        guest_buffer_start[guest] %= GUEST_BUFFER_SIZE;
    }
    serial_unlock();
    return ret;
}

static void internal_guest_putchar(int guest, int c) {
    if ((guest_buffer_end[guest] + 1) % GUEST_BUFFER_SIZE != guest_buffer_start[guest]) {
        guest_buffers[guest][guest_buffer_end[guest]] = c;
        guest_buffer_end[guest]++;
        guest_buffer_end[guest] %= GUEST_BUFFER_SIZE;
        switch (guest) {
        case 0:
            guest0_input_signal_emit();
            break;
        default:
            assert(!"unknown guest");
        }
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
        if (c == '\r') {
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
        } else if (c >= '0' && c < '0' + NUM_GUESTS) {
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

void vm0_putchar(int c) {
    internal_putchar(0, c);
    if (c == '\n') {
        internal_putchar(0, '\r');
    }
}

void guest0_putchar(int c) {
    internal_putchar(0 + NUM_GUESTS, c);
}

/* assume DLAB == 1*/
static inline void write_latch_high(uint8_t val) {
    serial_port_out8(LATCH_HIGH_ADDR, val);
}
static inline void write_latch_low(uint8_t val) {
    serial_port_out8(LATCH_LOW_ADDR, val);
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

static void clear_iir(int read) {
    uint8_t iir;
    int count = 0;
    while (! ((iir = read_iir()) & IIR_PENDING)) {
        assert(count++ < 1000);
        switch(iir & IIR_REASON) {
        case IIR_MSR:
            read_msr();
            break;
        case IIR_THR:
            break;
        case IIR_RDA:
        case IIR_TIME:
            if (read) {
                read_rbr();
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
    while (read_lsr() & LSR_DATA_READY) {
        handle_char(read_rbr());
    }
    clear_iir(1);
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
        for (i = 0; i < NUM_GUESTS * 2; i++) {
            flush_buffer(i);
        }
    }
    timeout_complete_reg_callback(timer_callback, 0);
    serial_unlock();
}

void pre_init(void) {
    serial_lock();
    // Initialize the serial port
    set_dlab(0); // we always assume the dlab is 0 unless we explicitly change it
    disable_interrupt();
    disable_fifo();
    reset_lcr();
    reset_mcr();
    clear_iir(1);
    set_baud_rate(BAUD_RATE);
    reset_state();
    enable_fifo();
    enable_interrupt();
    clear_iir(1);
    // all done
    set_putchar(serial_putchar);
    serial_irq_reg_callback(serial_irq, 0);
    serial_unlock();
    /* Start regular heartbeat of 50ms */
    timeout_periodic(50000000);
    timeout_complete_reg_callback(timer_callback, 0);
}

int guest0_input_getchar(void) {
    assert(!"Use poll");
    return 0;
}

int guest0_input_pollchar(int *out) {
    return internal_pollchar(0, out);
}


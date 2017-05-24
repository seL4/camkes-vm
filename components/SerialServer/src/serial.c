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
#include <string.h>

#include <sel4/sel4.h>
#include <camkes.h>

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

#define ESCAPE_CHAR '@'
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
#define GUEST_OUTPUT_BUFFER_SIZE 4096

/* TODO: have the MultiSharedData template generate a header with these */
void getchar_emit(unsigned int id);
seL4_Word getchar_enumerate_badge(unsigned int id);
unsigned int getchar_num_badges();
void *getchar_buf(unsigned int id);
int getchar_largest_badge(void);

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
static uint16_t output_buffer_bitmask = 0;

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
    output_buffer_bitmask &= ~BIT(b);
    fflush(stdout);
}

static int debug = 0;

/* Try to flush up to the end of the line. */
static bool flush_buffer_line(int b) {
    if (output_buffers_used[b] == 0) {
        return 0;
    }
    uint8_t* nlptr = memchr(output_buffers[b], '\r', output_buffers_used[b]);
    if (nlptr == NULL) {
        nlptr = memchr(output_buffers[b], '\n', output_buffers_used[b]);
    }
    if (nlptr == NULL) {
        if (debug == 2) {
            ZF_LOGD("newline not found!\r\n");
        }
        return 0;
    }
    size_t length = (nlptr - &output_buffers[b][0]) + 1;
    if (length < output_buffers_used[b] && (output_buffers[b][length] == '\n' || output_buffers[b][length] == '\r')) {
        length++;               /* Include \n after \r if present */
    }
    if (length == 0) {
        if (debug == 2) {
            ZF_LOGD("0-length!\r\n");
        }
        return 0;
    }
    if (b != last_out) {
        printf("%s%s", COLOUR_RESET, all_output_colours[b]);
        last_out = b;
    }
    int i;
    for (i = 0; i < length; i++) {
        printf("%c", output_buffers[b][i]);
    }
    for (i = length; i < output_buffers_used[b]; i++) {
        output_buffers[b][i-length] = output_buffers[b][i];
    }
    output_buffers_used[b] -= length;
    if (output_buffers_used[b] == 0) {
        output_buffer_bitmask &= ~BIT(b);
    }
    return 1;
}

static int is_newline(const uint8_t *c) {
    return (c[0] == '\r' && c[1] == '\n') || (c[0] == '\n' && c[1] == '\r');
}

static int active_guest = 0;
static int active_multiguests = 0;

/* Try coalescing guest output. This is intended for use with
 * multi-input mode to all guests. */

/* (XXX) CAVEATS:
 *
 * - Has not been tested with more than 2 guests
 *
 * - Has not been tested with multi-input mode set not matching guest
 *   set
 *
 * - Does not handle ANSI codes, UTF-8 or other multibytes specially;
     may break them when coalescing starts/stops.
 *
 * - Still "fails" due to some timing/buffering issues, but these
 *   failures are sufficiently rare that this is still useful. */
static int try_coalesce_output() {
    size_t length = 0;
    size_t used[MAX_GUESTS * 2] = { 0 };
    size_t n_used = 0;
    for (size_t i = 0; i < MAX_GUESTS * 2; i++) {
        if (output_buffers_used[i] > 0) {
            used[n_used++] = i;
            if (length == 0 || length > output_buffers_used[i]) {
                length = output_buffers_used[i];
            }
            if (n_used > 1) {
                if (memcmp(output_buffers[used[0]], output_buffers[i], length) != 0) {
                    if (debug == 1) {
                        ZF_LOGD("\r\nDifferent contents '");
                        for (int j = 0; j < length; ++j) {
                            printf("%0hhx", output_buffers[used[0]][j]);
                        }
                        printf("' vs '");
                        for (int j = 0; j < length; ++j) {
                            printf("%0hhx", output_buffers[i][j]);
                        }
                        printf("'\r\n");
                    }
                    return -1; /* different contents, don't special-case */
                }
            }
        }
    }
    if (n_used > 1 && length > 0) {
        if (last_out != -1) {
            printf("%s", COLOUR_RESET);
        }
        for (int i = 0; i < length; i++) {
            printf("%c", output_buffers[used[0]][i]);
        }
        last_out = -1;
        fflush(stdout);
        for (int i = 0; i < n_used; i++) {
            output_buffers_used[used[i]] -= length;
            for (int j = 0; j < output_buffers_used[used[i]]; ++j) {
                output_buffers[used[i]][j] = output_buffers[used[i]][j+length];
            }
            if (output_buffers_used[used[i]] == 0) {
                output_buffer_bitmask &= ~BIT(used[i]);
            }
        }
        if (output_buffer_bitmask != 0) {
            has_data = 1;
        }
        return 0;               /* coalesced */
    }
    return 1;                   /* buffering */
}

static void internal_putchar(int b, int c) {
    int UNUSED error;
    error = serial_lock();
    /* Add to buffer */
    int index = output_buffers_used[b];
    uint8_t *buffer = output_buffers[b];
    buffer[index] = (uint8_t)c;
    output_buffers_used[b]++;
    int coalesce_status = -1;

    if (active_guest == -1) {
        /* Test for special case: multiple guests outputting the EXACT SAME THING. */
        coalesce_status = try_coalesce_output();
    }
    if (output_buffers_used[b] == GUEST_OUTPUT_BUFFER_SIZE) {
        /* Since we're violating contract anyway (flushing in the
         * middle of someone else's line), flush all buffers, so the
         * fastpath can be used again. */
        char is_done;
        int i;
        int prev_guest = last_out;
        if (prev_guest != -1) {
            flush_buffer_line(prev_guest);
        }
        while (!is_done) {
            is_done = 1;
            for (i = 0; i < MAX_GUESTS * 2; i++) {
                if (flush_buffer_line(i)) {
                    is_done = 0;
                }
            }
        }
        /* Flush the rest, if necessary. */
        if (output_buffers_used[b] == GUEST_OUTPUT_BUFFER_SIZE) {
            for (i = 0; i < MAX_GUESTS * 2; i++) {
                if (i != b) {
                    flush_buffer(i);
                }
            }
        }
        /* then set the colors back. If this VM's buffer overflowed,
         * it's probably going to overflow again, so let's avoid
         * that. */
        if (last_out != b) {
            printf("%s%s", COLOUR_RESET, all_output_colours[b]);
            last_out = b;
        }
    } else if ((index >= 1 && is_newline(buffer + index - 1) && coalesce_status == -1)
               || (last_out == b && output_buffer_bitmask == 0 && coalesce_status == -1)) {
        /* Allow fast output (newline or same-as-last-VM) if
         * multi-input is not enabled OR last coalescing attempt
         * failed due to a mismatch. This is important as VM output
         * may be delayed; coalescing failure due to empty buffer
         * should lead to further buffering rather than early flush,
         * in case we can coalesce later. */
        flush_buffer(b);
    } else {
        output_buffer_bitmask |= BIT(b);
    }
    has_data = 1;
    error = serial_unlock();
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

static void give_guest_char(uint8_t c) {
    if (active_guest >= 0) {
        internal_guest_putchar(active_guest, c);
    } else if (active_guest == -1) {
        for (int i = 0; i < MAX_GUESTS * 2; i++) {
            if ((active_multiguests & BIT(i)) == BIT(i))
                internal_guest_putchar(i, c);
        }
    }
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
        if (c == ESCAPE_CHAR) {
            statemachine = 2;
        } else {
            statemachine = 0;
            give_guest_char(c);
        }
        break;
    case 2:
        switch (c) {
        case ESCAPE_CHAR:
            statemachine = 0;
            give_guest_char(c);
            break;
        case 'm':
            statemachine = 3;
            active_multiguests = 0;
            active_guest = -1;
            last_out = -1;
            printf(COLOUR_RESET "\r\nMulti-guest input to guests: ");
            fflush(stdout);
            break;
        case 'd':
            debug = (debug + 1) % 3;
            printf(COLOUR_RESET "\r\nDebug: %i\r\n", debug);
            last_out = -1;
            statemachine = 1;
            break;
        case '?':
            last_out = -1;
            printf(COLOUR_RESET "\r\n --- SerialServer help ---"
                   "\r\n Escape char: %c"
                   "\r\n 0 - %-2d switches input to that VM"
                   "\r\n ?      shows this help"
                   "\r\n m      simultaneous multi-guest input"
                   "\r\n d      switch between debugging modes"
                   "\r\n          0: no debugging"
                   "\r\n          1: debug multi-input mode output coalescing"
                   "\r\n          2: debug flush_buffer_line"
                   "\r\n", ESCAPE_CHAR, getchar_largest_badge());
            statemachine = 1;
            break;
        default:
            if (c >= '0' && c < '0' + (getchar_largest_badge()+1)) {
                last_out = -1;
                int guest = c - '0';
                printf(COLOUR_RESET "\r\nSwitching input to %d\r\n",guest);
                active_guest = guest;
                statemachine = 1;
            } else {
                statemachine = 0;
                give_guest_char(ESCAPE_CHAR);
                give_guest_char(c);
            }
        }
        break;
    case 3:
        if (c >= '0' && c < '0' + (getchar_largest_badge()+1)) {
            printf(COLOUR_RESET "%s%d", (active_multiguests != 0 ? "," : "") ,(c - '0'));
            active_multiguests |= BIT(c - '0');
            last_out = -1;
            fflush(stdout);
        } else if (c == 'm' || c == 'M' || c == '\r' || c == '\n') {
            last_out = -1;
            printf(COLOUR_RESET "\r\nSwitching input to multi-guest. Output will be best-effort coalesced (colored white).\r\n");
            statemachine = 1;
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

void serial_irq_handle() {
    int UNUSED error;
    error = serial_lock();
    clear_iir();
    error = serial_irq_acknowledge();
    error = serial_unlock();
}

static void timer_callback(void *data) {
    int UNUSED error;
    error = serial_lock();
    if (done_output) {
        done_output = 0;
    } else if (has_data) {
        /* flush everything if no writes since last callback */
        int i;
        char is_done = 0;
        char succeeded = 0;
        while (!is_done) {
            is_done = 1;
            for (i = 0; i < MAX_GUESTS * 2; i++) {
                if (flush_buffer_line(i)) {
                    succeeded = 1;
                    is_done = 0;
                }
            }
        }
        if (!succeeded) {
            for (i = 0; i < MAX_GUESTS * 2; i++) {
                flush_buffer(i);
            }
        }
    }
    error = serial_unlock();
}

seL4_CPtr timeout_notification(void);

int run(void) {
    seL4_CPtr notification = timeout_notification();
    while(1) {
        seL4_Wait(notification, NULL);
        timer_callback(NULL);
    }
    return 0;
}

void pre_init(void) {
    int UNUSED error;
    error = serial_lock();
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
    error = serial_irq_acknowledge();
    /* Start regular heartbeat of 500ms */
    timeout_periodic(0, 500000000);
    error = serial_unlock();
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

/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <autoconf.h>

#include <camkes.h>
#include <string.h>

extern void *ethdriver_buf;
void *client_buf(int);
void client_emit(seL4_Word badge);

void client_mac(uint8_t *b1, uint8_t *b2, uint8_t *b3, uint8_t *b4, uint8_t *b5, uint8_t *b6)
{
    ethdriver_mac(b1, b2, b3, b4, b5, b6);
}

int client_tx(int len)
{
    memcpy(ethdriver_buf, client_buf(1), len);
    return ethdriver_tx(len);
}

int client_rx(int *len)
{
    int result = ethdriver_rx(len);
    if (result != -1) {
        memcpy(client_buf(1), ethdriver_buf, *len);
    }
    return result;
}

void ethdriver_has_data_callback(seL4_Word badge)
{
    client_emit(1);
}

/*
 * Copyright 2018, Data61
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

#include <camkes.h>
#include <string.h>

extern void *a_buf;
extern void *b_buf;


void handle_data(int (*rx)(int *),int(*tx)(int), void *in, void *out) {
    int len;
    amutex_lock();
    int result = rx(&len);
    while (result != -1) {
        if (len > PAGE_SIZE_4K) {
            ZF_LOGE("Dropping packet that is too long (>4k).");
        } else {
            memcpy(out, in, len);
            int tx_result = tx(len);
            if (tx_result) {
                ZF_LOGE("Could not tx packet");
            }
        }
        result = rx(&len);
    }
    amutex_unlock();

}

void a_has_data_callback(seL4_Word badge) {
   handle_data(a_rx, b_tx, a_buf, b_buf);
}


void b_has_data_callback(seL4_Word badge) {
    handle_data(b_rx, a_tx, b_buf, a_buf);
}

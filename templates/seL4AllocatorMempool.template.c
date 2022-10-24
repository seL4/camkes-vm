/*
 * Copyright 2022, DornerWorks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>

/*- set mempool_size = macros.ROUND_UP(configuration[me.name].get('allocator_mempool_size', 80 * 1024 * 1024), macros.PAGE_SIZE) -*/

static char allocator_mempool[/*? mempool_size ?*/] ALIGN(PAGE_SIZE_4K) SECTION("align_12bit");

char *get_allocator_mempool(void)
{
    return (char *)allocator_mempool;
}

size_t get_allocator_mempool_size(void)
{
    return /*? mempool_size ?*/;
}

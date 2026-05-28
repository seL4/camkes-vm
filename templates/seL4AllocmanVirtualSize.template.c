/*
 * Copyright 2023, DornerWorks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>

/* For every 500000000 bytes added to ALLOCMAN_VIRTUAL_SIZE,
 * the guest can map an additional 6960 MiBs. Dividing the two
 * values gives a scale of 71839, which we round up to 72000
 */

/*- set guest_ram_mb = configuration[me.name].get('guest_ram_mb', 4096) -*/
/*- set allocman_virtual_size = guest_ram_mb * 72000 -*/

size_t allocman_virtual_size(void)
{
    return /*? allocman_virtual_size ?*/;
}

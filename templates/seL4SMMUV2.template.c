/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <sel4/sel4.h>

/*# ARM smmu v2 cap allocation #*/
/*- set sid_cap = alloc(name='sid', type=seL4_ARMSID) -*/

/*- set cb_cap = alloc(name='cb', type=seL4_ARMCB) -*/

seL4_CPtr camkes_get_smmu_sid_cap(){
    return /*? sid_cap ?*/;
}

seL4_CPtr camkes_get_smmu_cb_cap(){
    return /*? cb_cap ?*/;
}
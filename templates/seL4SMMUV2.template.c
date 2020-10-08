/*
 * Copyright 2020, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
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
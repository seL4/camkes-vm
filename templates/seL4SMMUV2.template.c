/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <sel4/sel4.h>

/*# ARM smmu v2 cap allocation #*/
/*- if 'smmu_map' in configuration[me.name].keys() -*/
    /*- set caps_map = {} -*/
    /*- for map in configuration[me.name].get('smmu_map',[]) -*/
        /*- set sid_cap = alloc(name='sid_%d' % map["sid"], type=seL4_ARMSID, streamid=map["sid"]) -*/
        /*- set cb_cap = alloc(name='cb_%d' % map["cb"], type=seL4_ARMCB, bank=map["cb"]) -*/
        /*- do caps_map.update({map["sid"] : {"sid_cap": sid_cap, "cb_cap": cb_cap}}) -*/
    /*- endfor -*/

    /*- set num_caps = len(caps_map) -*/
/*- endif -*/

int camkes_get_smmu_num_caps(){
/*- if 'smmu_map' in configuration[me.name].keys() -*/
    return /*? num_caps ?*/;
/*- else -*/
    return 0;
/*- endif -*/
}

int camkes_get_sid_at_index(int index){
    switch(index){
/*- if 'smmu_map' in configuration[me.name].keys() -*/
    /*- for index, sid in enumerate(caps_map) -*/
        case /*? index ?*/:
            return /*? sid ?*/;
    /*- endfor -*/
/*- endif -*/
        default:
            return seL4_CapNull;
    }

}
seL4_CPtr camkes_get_smmu_sid_cap(int sid){
    switch(sid){
/*- if 'smmu_map' in configuration[me.name].keys() -*/
    /*- for sid in caps_map.keys() -*/
        case /*? sid ?*/:
            return /*? caps_map[sid]["sid_cap"] ?*/;
    /*- endfor -*/
/*- endif -*/
        default:
            return seL4_CapNull;
    }
}

seL4_CPtr camkes_get_smmu_cb_cap(int sid){
    switch(sid){
/*- if 'smmu_map' in configuration[me.name].keys() -*/
    /*- for sid in caps_map.keys() -*/
        case /*? sid ?*/:
            return /*? caps_map[sid]["cb_cap"] ?*/;
    /*- endfor -*/
/*- endif -*/
        default:
            return seL4_CapNull;
    }
}

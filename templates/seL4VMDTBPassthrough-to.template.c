/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <assert.h>
#include <camkes/dataport.h>
#include <camkes/irq.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <platsupport/io.h>
#include <platsupport/irq.h>
#include <utils/util.h>
#include <sel4/sel4.h>

#include <simple/simple.h>

/*- import 'dtb-query-common.template.c' as dtb_macros with context -*/

/*# Grab the DTB object made from the previous stages of the parsing #*/
/*- set configuration_name = '%s.%s' % (me.instance.name, me.interface.name) -*/
/*- set dtb_query = configuration[me.instance.name].get('dtb') -*/
/*- set dtb = dtb_query.get('query') -*/

/*- set untyped_dtb_mmio = [] -*/
/*- set dtb_irqs_map = {} -*/
/*- set dtb_irqs = [] -*/
/*- set dtb_node_paths = [] -*/

/*# Extract the relevant fields from the DTB (regs, interrupts, etc) #*/
/*- for i, node in enumerate(dtb) -*/

    /*? dtb_macros.parse_dtb_node_reg(node) ?*/
    /*- set reg_set = pop('reg_set') -*/

    /*- for paddr, size in reg_set -*/
        /*- set paddr = macros.ROUND_DOWN(paddr, 4096) -*/
        /*- set size = macros.ROUND_UP(size, 4096) -*/
        /*- for paddr_alloc, size_bits in macros.get_untypeds_from_range(paddr, size) -*/
            /*- set cap = alloc('dtb_untyped_cap_0x%x' % paddr_alloc, seL4_UntypedObject, paddr = paddr_alloc, size_bits = size_bits) -*/
            /*- do untyped_dtb_mmio.append( (paddr_alloc, size_bits, cap) ) -*/
        /*- endfor -*/
    /*- endfor -*/

    /*? dtb_macros.parse_dtb_node_interrupts(node, -1, options.architecture) ?*/
    /*- set irq_set = pop('irq_set') -*/
    /*- for irq in irq_set -*/
        /*- if irq['irq'] not in dtb_irqs_map -*/
            /*- if irq['trigger'] -*/
                /*- set irq_cap = alloc('%s_irq_%d' % (me.interface.name, irq['irq']), seL4_IRQHandler, number=irq['irq'], trigger=irq['trigger']) -*/
            /*- else -*/
                /*- set irq_cap = alloc('%s_irq_%d' % (me.interface.name, irq['irq']), seL4_IRQHandler, number=irq['irq']) -*/
            /*- endif -*/
            /*- do dtb_irqs.append( (irq['irq'], irq_cap) ) -*/
            /*- do dtb_irqs_map.update({irq['irq']: irq_cap}) -*/
        /*- endif -*/
    /*- endfor -*/

    /*- set dtb_path = node.get('this_node_path') -*/
    /*- do dtb_node_paths.append(dtb_path) -*/
/*- endfor -*/

/*- for irq in configuration[me.instance.name].get('dtb_irqs', []) -*/
    /*- if irq not in dtb_irqs_map -*/
        /*- set irq_cap = alloc('%s_irq_%d' % (me.interface.name, irq), seL4_IRQHandler, number=irq) -*/
        /*- do dtb_irqs.append( (irq, irq_cap) ) -*/
        /*- do dtb_irqs_map.update({irq: irq_cap}) -*/
    /*- endif -*/
/*- endfor -*/

/*- set self_cnode = alloc_cap('cnode', my_cnode, write=true) -*/

static int camkes_dtb_irqs[] = {
    /*- for (irq, cap) in dtb_irqs -*/
            /*? irq ?*/,
    /*- endfor -*/
};

static char *camkes_dtb_node_paths[] = {
    /*- for path in dtb_node_paths -*/
    /*- if path -*/
            "/*? path ?*/",
    /*- endif -*/
    /*- endfor -*/
};

/*- set plat_keep_devices = configuration[me.instance.name].get('plat_keep_devices') -*/

static char *camkes_dtb_keep_devices[] = {
    /*- if plat_keep_devices -*/
    /*- for path in plat_keep_devices -*/
            "/*? path ?*/",
    /*- endfor -*/
    /*- endif -*/
};

/*- set plat_keep_devices_and_subtree = configuration[me.instance.name].get('plat_keep_devices_and_subtree') -*/

static char *camkes_dtb_keep_devices_and_subtree[] = {
    /*- if plat_keep_devices_and_subtree -*/
    /*- for path in plat_keep_devices_and_subtree -*/
            "/*? path ?*/",
    /*- endfor -*/
    /*- endif -*/
};

char **camkes_dtb_get_node_paths(int *num_nodes) {
    *num_nodes = ARRAY_SIZE(camkes_dtb_node_paths);
    return camkes_dtb_node_paths;
}

char **camkes_dtb_get_plat_keep_devices(int *num_nodes) {
    *num_nodes = ARRAY_SIZE(camkes_dtb_keep_devices);
    return camkes_dtb_keep_devices;
}

char **camkes_dtb_get_plat_keep_devices_and_subtree(int *num_nodes) {
    *num_nodes = ARRAY_SIZE(camkes_dtb_keep_devices_and_subtree);
    return camkes_dtb_keep_devices_and_subtree;
}

int *camkes_dtb_get_irqs(int *num_irqs) {
    *num_irqs = ARRAY_SIZE(camkes_dtb_irqs);
    return camkes_dtb_irqs;
}

seL4_Error camkes_dtb_get_irq_cap(int irq, seL4_CNode cnode, seL4_Word index, uint8_t depth) {
    /*- if len(dtb_irqs) > 0 -*/
        switch(irq) {
        /*- for irq,cap in dtb_irqs -*/
            case /*? irq ?*/:
                return seL4_CNode_Copy(cnode, index, depth, /*? self_cnode ?*/, /*? cap ?*/, CONFIG_WORD_SIZE, seL4_AllRights);
        /*- endfor -*/
            default:
                return seL4_FailedLookup;
        }
    /*- else -*/
        return seL4_FailedLookup;
    /*- endif -*/
}

int camkes_dtb_untyped_count(void) {
    return /*? len(untyped_dtb_mmio) ?*/;
}

seL4_CPtr camkes_dtb_get_nth_untyped(int n, size_t *size_bits, uintptr_t *paddr) {
    switch(n) {
    /*- for i in range(0, len(untyped_dtb_mmio)) -*/
        /*- set (paddr,size_bits,cap)  = untyped_dtb_mmio[i] -*/
        case /*? i ?*/:
            *size_bits = (size_t)/*? size_bits ?*/;
            *paddr = /*? paddr ?*/;
            return /*? cap ?*/;
        /*- endfor -*/
        default:
            assert(!"Invalid untyped cap requested");
    }
    return 0;
}

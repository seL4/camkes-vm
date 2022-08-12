/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <assert.h>
#include <camkes/error.h>
#include <stdio.h>
#include <stdint.h>
#include <sel4/sel4.h>

/*- set config_irqs = configuration[me.name].get("vm_irqs") -*/
/*- set irqs = [] -*/
/*- set irqnotification_object = alloc_obj('irq_notification_obj', seL4_NotificationObject) -*/
/*- set irqnotification_object_cap = alloc_cap('irq_notification_obj', irqnotification_object, read=True) -*/
/*- if config_irqs is not none -*/
    /*- for irq in config_irqs -*/
        /*- set cap = alloc('irq_%d_%d' % (irq['ioapic'], irq['source']), seL4_IRQHandler, vector=irq['dest'], ioapic = irq['ioapic'], ioapic_pin = irq['source'], level = irq['level_trig'], polarity = irq['active_low'], notification=my_cnode[irqnotification_object_cap]) -*/
        /*- do irqs.append( (irq['name'].strip('"'), irq['ioapic'], irq['source'], irq['level_trig'], irq['active_low'], irq['dest'], cap) ) -*/
    /*- endfor -*/
/*- endif -*/

int irqs_num_irqs() {
    return /*? len(irqs) ?*/;
}

const char * irqs_get_irq(int irq, seL4_CPtr *irq_handler, uint8_t *ioapic, uint8_t *source, int *level_trig, int *active_low, uint8_t *dest) {
    /*- if len(irqs) == 0 -*/
        return NULL;
    /*- else -*/
        switch (irq) {
            /*- for name, ioapic, source, level_trig, active_low, dest, cap in irqs -*/
                case /*? loop.index0 ?*/:
                    *irq_handler = /*? cap ?*/;
                    *ioapic = /*? ioapic ?*/;
                    *source = /*? source ?*/;
                    *level_trig = /*? level_trig ?*/;
                    *active_low = /*? active_low ?*/;
                    *dest = /*? dest ?*/;
                    return "/*? name ?*/";
            /*- endfor -*/
        default:
            return NULL;
        }
    /*- endif -*/
}

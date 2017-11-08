/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#ifndef VM_INIT_I8259_H
#define VM_INIT_I8259_H

/* callback type for irq acknowledgement */
typedef void (*i8259_irq_ack_fn)(int irq, void *cookie);

/* register a callback to be run on interrupt acknowledgement. returns existing callback
 * will panic if called on invalid IRQs */
i8259_irq_ack_fn i8259_register_irq_ack_callback(int irq, i8259_irq_ack_fn fn, void *cookie);

/* helper callback that performs a seL4_IRQHandler_Ack on the cookie (after casting to a seL4_CPtr) */
void i8259_irq_ack_hw_irq_handler(int irq, void *cptr);

/* wrapper for register a callback that users a hw irq handler */
static inline i8259_irq_ack_fn i8259_register_hw_ack(int irq, seL4_CPtr cptr) {
    return i8259_register_irq_ack_callback(irq, i8259_irq_ack_hw_irq_handler, (void*)cptr);
}

/* Tell the i8259 to simulate a edge triggered interrupt */
void i8259_gen_irq(int irq);

/* Simulate level triggered interrupt states */
void i8259_level_raise(int irq);
void i8259_level_lower(int irq);
void i8259_level_set(int irq, int level);

/* Port in/out functions for i8259 emulation */
int i8259_port_in(void *cookie, unsigned int port_no, unsigned int size, unsigned int *result);
int i8259_port_out(void *cookie, unsigned int port_no, unsigned int size, unsigned int value);

/* Init function */
void i8259_pre_init(void);

/* Functions to retrieve interrupt state */
int i8259_get_interrupt();
int i8259_has_interrupt();

#endif

/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#ifndef VM_INIT_I8259_H
#define VM_INIT_I8259_H

/* Tell the i8259 to simulate a edge triggered interrupt */
void i8259_gen_irq(int irq);

#endif

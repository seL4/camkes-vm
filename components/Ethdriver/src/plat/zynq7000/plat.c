/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#include <camkes.h>
#include <camkes/io.h>
#include <camkes/dma.h>
#include <ethdrivers/raw.h>
#include <ethdrivers/zynq7000.h>
#include <platsupport/io.h>
#include <platsupport/clock.h>
#include <platsupport/irq.h>
#include <vka/vka.h>
#include <simple/simple.h>
#include <allocman/vka.h>
#include <sel4utils/vspace.h>

extern void *EthDriver_0;

int ethif_preinit(vka_t *vka, simple_t *camkes_simple, vspace_t *vspace,
                  ps_io_ops_t *io_ops)
{
    int ret = camkes_io_ops(io_ops);
    if (ret) {
        return ret;
    }
    // This may reinitialise the clocks, timeserver might not like this,
    // but zynq7000/uboot/zynq_gem.c requires access to the SLCR registers
    // to configure some clocks related to the Ethernet device.
    return clock_sys_init(io_ops, &io_ops->clock_sys);
}

int ethif_init(struct eth_driver *eth_driver, ps_io_ops_t *io_ops)
{
    struct arm_eth_plat_config eth_config = (struct arm_eth_plat_config) {
        .buffer_addr = (void *) EthDriver_0,
        .prom_mode = (uint8_t) promiscuous_mode
    };

    if (!eth_config.prom_mode) {
        for (int i = 0; i < 6; i++) {
            eth_config.mac_addr[i] = (uint8_t) mac[i];
        }
    }

    int error = ethif_zynq7000_init(eth_driver, *io_ops, (void *) &eth_config);
    if (error) {
        return error;
    }

    ps_irq_t irq = { .type = PS_INTERRUPT, .irq = { .number = ZYNQ7000_INTERRUPT_ETH0 }};
    return EthDriver_irq_acknowledge(&irq);
}

void EthDriver_irq_handle(ps_irq_t *irq)
{
    eth_irq_handle(EthDriver_irq_acknowledge, irq);
}

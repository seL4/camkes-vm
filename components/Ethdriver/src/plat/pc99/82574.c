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
#include <ethdrivers/raw.h>
#include <ethdrivers/intel.h>
#include <platsupport/io.h>
#include <vka/vka.h>
#include <simple/simple.h>
#include <allocman/vka.h>
#include <sel4utils/vspace.h>

int pc99_eth_setup(vka_t *vka, simple_t *camkes_simple, vspace_t *vspace, ps_io_ops_t *io_ops);

int ethif_preinit(vka_t *vka, simple_t *camkes_simple, vspace_t *vspace,
                  ps_io_ops_t *io_ops)
{
    int error = pc99_eth_setup(vka, camkes_simple, vspace, io_ops);
    if (error) {
        return error;
    }

    return 0;
}

int ethif_init(struct eth_driver *eth_driver, ps_io_ops_t *io_ops)
{
    ethif_intel_config_t eth_config = (ethif_intel_config_t) {
        /* Ethdriver component dataport */
        .bar0 = (void *)EthDriver
    };

    int error = ethif_e82574_init(eth_driver, *io_ops, &eth_config);
    if (error) {
        return error;
    }

    return irq_acknowledge();
}

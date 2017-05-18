/*
 * Copyright 2017, Data 61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(D61_GPL)
 */

#include <ethdrivers/intel.h>

int ethif_init(struct eth_driver *eth_driver, ps_io_ops_t io_ops, void *config) {
    return ethif_e82574_init(eth_driver, io_ops, config);
}


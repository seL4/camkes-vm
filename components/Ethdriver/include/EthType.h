/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */
#ifndef VMM_CONFIG_ETHTYPE_H
#define VMM_CONFIG_ETHTYPE_H

/* Device configuration file from the apps/<vm-application>/configurations/device_config.h */
#include "device_config.h"

typedef struct EthDriverMMIO {
    char buf[ETHDRIVER_MMIO_BUF_SZ];
} EthDriverMMIO_t;

#endif

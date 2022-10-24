/*
 * Copyright 2019, DornerWorks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define IRQ_SPI_OFFSET 32

/* 158 chosen because it doesn't correspond to a physical interrupt */
#define VIRTIO_NET_PLAT_INTERRUPT_LINE (158 + IRQ_SPI_OFFSET)
#define VIRTIO_CON_PLAT_INTERRUPT_LINE (158 + IRQ_SPI_OFFSET)

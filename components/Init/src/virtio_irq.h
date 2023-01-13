/*
 * Copyright 2022, UNSW (ABN 57 195 873 179)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

/**
 * Hardcoded IRQ numbers for virtual devices. These have to be unique
 * for each device.
 * Where Linux has a virtual IRQ number assigned (in `/proc/interrupts`)
 * we use that, otherwise pick a free one.
 *
 * @todo: Having hardcoded numbers for virtio devices might become
 * an issue as the list of virtual devices grows. Might be worth it
 * to have an IRQ number allocator.
 */
#define TIMER_IRQ                   0
/* #define CASCADE_IRQ                 2 */
#define TTYS0_IRQ                   4
#define VIRTIO_NET_IRQ              6
#define VIRTIO_BLK_IRQ              7
#define RTC_IRQ                     8
#define VIRTIO_CON_IRQ              9
#define VIRTIO_SCK_IRQ              10

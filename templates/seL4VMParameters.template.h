/*
 * Copyright 2023, DornerWorks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

extern const unsigned long ram_base;
extern const unsigned long ram_paddr_base;
extern const unsigned long ram_size;
extern const unsigned long ram_offset;
extern const unsigned long dtb_addr;
extern const unsigned long initrd_max_size;
extern const unsigned long initrd_addr;

extern const char *_kernel_name;
extern const char *_dtb_name;
extern const char *_initrd_name;
extern const char *kernel_bootcmdline;
extern const char *kernel_stdout;
extern const char *dtb_base_name;

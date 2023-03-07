/*
 * Copyright 2023, DornerWorks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

extern const unsigned long ram_base;
extern const unsigned long ram_paddr_base;
extern const unsigned long ram_size;
extern const unsigned long dtb_addr;

extern const unsigned long initrd_addr;
extern const unsigned long entry_addr;

/*- if not configuration[me.name].get('vm_address_config') -*/
extern const unsigned long initrd_max_size;
extern const unsigned long ram_offset;
/*- endif -*/

extern const int provide_initrd;
extern const int generate_dtb;
extern const int provide_dtb;
extern const int map_one_to_one;
extern const int clean_cache;

extern const char *_kernel_name;
extern const char *_dtb_name;
extern const char *_initrd_name;
extern const char *kernel_bootcmdline;
extern const char *kernel_stdout;
extern const char *dtb_base_name;

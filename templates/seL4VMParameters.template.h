/*
 * Copyright 2023, DornerWorks
 * Copyright 2023, Hensoldt Cyber
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {

    struct {
        uintptr_t phys_base;
        uintptr_t base;
        size_t size;
    } ram;

    bool provide_initrd;
    bool generate_dtb;
    bool provide_dtb;
    bool map_one_to_one;
    bool clean_cache;

    uintptr_t dtb_addr;
    uintptr_t initrd_addr;
    uintptr_t entry_addr;

    struct {
        char const *kernel;
        char const *initrd;
        char const *dtb;
        char const *dtb_base;
    } files;

    char const *kernel_bootcmdline;
    char const *kernel_stdout;

} vm_config_t;

extern const vm_config_t vm_config;

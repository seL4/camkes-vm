/*
 * Copyright 2023, DornerWorks
 * Copyright 2023, Hensoldt Cyber
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

typedef struct {

    struct {
        unsigned long phys_base;
        unsigned long base;
        unsigned long size;
    } ram;

    int provide_initrd;
    int generate_dtb;
    int provide_dtb;
    int map_one_to_one;
    int clean_cache;

    unsigned long dtb_addr;
    unsigned long initrd_addr;
    unsigned long entry_addr;

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

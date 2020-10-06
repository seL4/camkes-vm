/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#pragma once


/**
* generate a "memory" node
* @param fdt
* @param base, the base of the memory region
* @param size, the size of the memory region
* @return -1 on error, 0 otherwise
*/
int fdt_generate_memory_node(void *fdt, unsigned long base, size_t size);

/**
* generate a "chosen" node
* @param fdt
* @param stdout_path, the path of the stdout
* @param bootargs
* @param maxcpus
* @return -1 on error, 0 otherwise
*/
int fdt_generate_chosen_node(void *fdt, const char *stdout_path, const char *bootargs, const unsigned int maxcpus);

/**
* append the chosen node with initrd info
* @param fdt
* @param base, the base of the initrd image
* @param size, the size of the initrd image
* @return -1 on error, 0 otherwise
*/
int fdt_append_chosen_node_with_initrd_info(void *fdt, unsigned long base, size_t size);

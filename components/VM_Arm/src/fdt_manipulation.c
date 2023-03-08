/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <libfdt.h>
#include <utils/util.h>

static int append_prop_with_cells(void *fdt, int offset,  uint64_t val, int num_cells, const char *name)
{
    int err;
    if (num_cells == 2) {
        err = fdt_appendprop_u64(fdt, offset, name, val);
    } else if (num_cells == 1) {
        err = fdt_appendprop_u32(fdt, offset, name, val);
    } else {
        ZF_LOGF("non-supported arch");
    }

    return err;
}

int fdt_generate_memory_node(void *fdt, uintptr_t base, size_t size)
{
    int root_offset = fdt_path_offset(fdt, "/");
    int address_cells = fdt_address_cells(fdt, root_offset);
    int size_cells = fdt_size_cells(fdt, root_offset);

    int this = fdt_add_subnode(fdt, root_offset, "memory");
    if (this < 0) {
        return this;
    }
    int err = fdt_appendprop_string(fdt, this, "device_type", "memory");
    if (err) {
        return err;
    }
    err = append_prop_with_cells(fdt, this, base, address_cells, "reg");
    if (err) {
        return err;
    }
    err = append_prop_with_cells(fdt, this, size, size_cells, "reg");
    if (err) {
        return err;
    }

    return 0;
}

int fdt_generate_chosen_node(void *fdt, const char *stdout_path, const char *bootargs, const unsigned int maxcpus)
{
    int root_offset = fdt_path_offset(fdt, "/");
    int this = fdt_add_subnode(fdt, root_offset, "chosen");
    int err;

    if (stdout_path && strlen(stdout_path) > 0) {
        err = fdt_appendprop_string(fdt, this, "stdout-path", stdout_path);
        if (err) {
            return err;
        }
        err = fdt_appendprop_string(fdt, this, "linux,stdout-path", stdout_path);
        if (err) {
            return err;
        }
    }

    size_t bootargs_len = strlen(bootargs);
    /*  +3*sizeof(int) is a cheap approximated formula for maximum number of characters in a UINT_MAX
     *  +1 for null character, +9 for ' maxcpus='
     */
    size_t updated_bootargs_len = bootargs_len + 9 + (3 * sizeof(unsigned int) + 1);
    char *updated_bootargs = calloc(1, updated_bootargs_len);
    if (!updated_bootargs) {
        ZF_LOGE("Failed to generate chosen node: Unable to allocate updated bootargs");
        return err;
    }
    int res = snprintf(updated_bootargs, updated_bootargs_len, "%s maxcpus=%u", bootargs, maxcpus);
    if (res < 0) {
        ZF_LOGE("Failed to generate chosen node: Unable to allocate updated bootargs");
        free(updated_bootargs);
        return -1;
    }

    err = fdt_appendprop_string(fdt, this, "bootargs", updated_bootargs);
    if (err) {
        ZF_LOGE("Failed to generate chosen node: Unable to create updated bootargs");
        free(updated_bootargs);
        return err;
    }
    free(updated_bootargs);

    return 0;
}

int fdt_append_chosen_node_with_initrd_info(void *fdt, uintptr_t base, size_t size)
{
    int root_offset = fdt_path_offset(fdt, "/");
    int address_cells = fdt_address_cells(fdt, root_offset);
    int this = fdt_path_offset(fdt, "/chosen");
    int err = append_prop_with_cells(fdt, this, base, address_cells, "linux,initrd-start");
    if (err) {
        return err;
    }
    err = append_prop_with_cells(fdt, this, base + size, address_cells, "linux,initrd-end");
    if (err) {
        return err;
    }

    return 0;
}

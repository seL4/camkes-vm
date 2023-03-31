/*
 * Copyright 2023, Hensoldt Cyber
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <vmlinux.h>
#include <camkes.h>

#include <sel4vmmplatsupport/ioports.h>
#include <sel4vmmplatsupport/arch/vpci.h>
#include <sel4vmmplatsupport/drivers/pci_helper.h>

#include <libfdt.h>

extern vmm_io_port_list_t *io_ports;

/* Can't make this static, because others uses it */
vmm_pci_space_t *pci = NULL;

/* Various other modules can register PCI devices. Update the device tree
 * eventually.
 */
int vpci_update_dtb(vm_t *vm, const vm_config_t *vm_config, void *dtb,
                    void *fdt, char const *gic_node)
{
    if (!vm_config->generate_dtb) {
        return 0;
    }

    ZF_LOGF_IF(!fdt_ori, "fdt not set");

    int gic_offset = fdt_path_offset(fdt, gic_node);
    if (gic_offset < 0) {
        ZF_LOGE("Failed to find gic node from path: %s", gic_node);
        return -1;
    }
    int gic_phandle = fdt_get_phandle(fdt, gic_offset);
    if (0 == gic_phandle) {
        ZF_LOGE("Failed to find phandle in gic node (%d)", gic_phandle);
        return -1;
    }
    int err = fdt_generate_vpci_node(vm, pci, dtb, gic_phandle);
    if (err) {
        ZF_LOGE("Couldn't generate vpci_node (%d)", err);
        return -1;
    }

}

static void vpci_init_module(vm_t *vm, void *cookie)
{
    int err;

    err = vmm_pci_init(&pci);
    if (err) {
        ZF_LOGE("Failed to initialise vmm pci (%d)", err);
        return;
    }

    ZF_LOGF_IF(!io_ports, "io_ports not set");

    err = vm_install_vpci(vm, io_ports, pci);
    if (err) {
        ZF_LOGE("Failed to install VPCI device (%d)", err);
        return;
    }

}

DEFINE_MODULE(vpci, NULL, vpci_init_module)

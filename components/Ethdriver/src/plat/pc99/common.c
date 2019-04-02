/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#include <autoconf.h>
#include <camkes.h>
#include <camkes/dma.h>
#include <platsupport/io.h>
#include <vka/vka.h>
#include <simple/simple.h>
#include <simple/simple_helpers.h>
#include <allocman/vka.h>
#include <sel4utils/vspace.h>
#include <sel4utils/iommu_dma.h>
#include <sel4platsupport/arch/io.h>

#include "../../ethdriver.h"

seL4_CPtr (*original_vspace_get_cap)(vspace_t*, void*);

/* Returns the cap to the frame mapped to vaddr, assuming
 * vaddr points inside our dma pool. */
seL4_CPtr get_dma_frame_cap(vspace_t *vspace, void *vaddr) {
    seL4_CPtr cap = camkes_dma_get_cptr(vaddr);
    if (cap == seL4_CapNull) {
        return original_vspace_get_cap(vspace, vaddr);
    }
    return cap;
}

/* Allocate a dma buffer backed by the component's dma pool */
void* camkes_iommu_dma_alloc(void *cookie, size_t size,
        int align, int cached, ps_mem_flags_t flags) {

    // allocate buffer from the dma pool
    void* vaddr = camkes_dma_alloc(size, align);
    if (vaddr == NULL) {
        return NULL;
    }
    int error = sel4utils_iommu_dma_alloc_iospace(cookie, vaddr, size);
    if (error) {
        camkes_dma_free(vaddr, size);
        return NULL;
    }
    return vaddr;
}

int pc99_eth_setup(vka_t *vka, simple_t *camkes_simple, vspace_t *vspace, ps_io_ops_t *io_ops) {
    int error = 0;
    int pci_bdf_int = 0;
    int bus, dev, fun = 0;
    cspacepath_t iospace = {0};
    error = vka_cspace_alloc_path(vka, &iospace);
    if (error)
        return error;

    /* Ethdriver component attribute */
    sscanf(pci_bdf, "%x:%x.%d", &bus, &dev, &fun);
    pci_bdf_int = bus * 256 + dev * 8 + fun;
    /* get this from the configuration */
    error = simple_get_iospace(camkes_simple, iospace_id, pci_bdf_int, &iospace);
    if (error)
        return error;

    /* Save a pointer to the original get_cap function for our vspace */
    original_vspace_get_cap = vspace->get_cap;

    /* The iommu driver needs the caps to frames backing the dma buffer.
     * It will invoke the get_cap method of its vspace to get these caps.
     * Since the vspace we give to the iommu driver wasn't used to allocate
     * the dma buffer, it doesn't know the caps to the frames backing the
     * buffer. CAmkES allocated the buffer statically, and so the caps are
     * known to it. Here, we override the get_cap method of our vspace to
     * return dma buffer frame caps provided by CAmkES. */
    vspace->get_cap = get_dma_frame_cap;

    error = sel4utils_make_iommu_dma_alloc(vka, vspace, &io_ops->dma_manager, 1, &iospace.capPtr);
    if (error)
        return error;
    io_ops->dma_manager.dma_alloc_fn = camkes_iommu_dma_alloc;

    error = sel4platsupport_get_io_port_ops(&io_ops->io_port_ops, camkes_simple, vka);
    if (error)
        return error;

    return 0;
}

void irq_handle(void) {
    eth_irq_handle(irq_acknowledge, NULL);
}

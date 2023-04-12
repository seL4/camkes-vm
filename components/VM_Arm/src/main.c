/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2022, Technology Innovation Institute
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <autoconf.h>
#include <arm_vm/gen_config.h>
#include <sel4muslcsys/gen_config.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <setjmp.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <allocman/allocman.h>
#include <allocman/bootstrap.h>
#include <allocman/vka.h>
#include <vka/capops.h>
#include <vka/object.h>

#include <vspace/vspace.h>
#include <simple/simple.h>
#include <simple/simple_helpers.h>
#include <simple-default/simple-default.h>
#include <platsupport/io.h>
#include <platsupport/irq.h>
#include <sel4platsupport/platsupport.h>
#include <sel4platsupport/io.h>

#include <sel4vm/guest_vm.h>
#include <sel4vm/boot.h>
#include <sel4vm/guest_ram.h>
#include <sel4vm/guest_iospace.h>
#include <sel4vm/guest_irq_controller.h>

#include <sel4vmmplatsupport/drivers/virtio_con.h>

#include <sel4vmmplatsupport/arch/vusb.h>
#include <sel4vmmplatsupport/arch/vpci.h>
#include <sel4vmmplatsupport/guest_image.h>
#include <sel4vmmplatsupport/drivers/pci_helper.h>
#include <sel4vmmplatsupport/drivers/cross_vm_connection.h>
#include <sel4vmmplatsupport/arch/guest_boot_init.h>
#include <sel4vmmplatsupport/arch/guest_reboot.h>
#include <sel4vmmplatsupport/arch/guest_vcpu_fault.h>
#include <sel4vmmplatsupport/guest_vcpu_util.h>

#include <sel4utils/process.h>
#include <sel4utils/irq_server.h>
#include <dma/dma.h>

#include <elf/elf.h>

#include <camkes.h>
#include <camkes/tls.h>
#include <camkes/dataport.h>

#include <vmlinux.h>
#include "fsclient.h"

#include <libfdt.h>
#include <fdtgen.h>
#include "fdt_manipulation.h"

/* Do - Include prototypes to surpress compiler warnings
 * TODO: Add these to a template header */
seL4_CPtr notification_ready_notification(void);
int camkes_io_fdt(ps_io_fdt_t *io_fdt);
seL4_CPtr camkes_alloc(seL4_ObjectType type, size_t size, unsigned flags);
/* Done */

extern void *fs_buf;
int start_extra_frame_caps;

int VM_PRIO = 100;
int NUM_VCPUS = 1;

#define IRQSERVER_PRIO      (VM_PRIO + 1)
#define IRQ_MESSAGE_LABEL   0xCAFE

#define DMA_VSTART  0x40000000

#ifndef DEBUG_BUILD
#define seL4_DebugHalt() do{ printf("Halting...\n"); while(1); } while(0)
#endif

vka_t _vka;
simple_t _simple;
vspace_t _vspace;
sel4utils_alloc_data_t _alloc_data;
allocman_t *allocman;
seL4_CPtr _fault_endpoint;
irq_server_t *_irq_server;

vmm_pci_space_t *pci;
vmm_io_port_list_t *io_ports;
reboot_hooks_list_t reboot_hooks_list;

#define DTB_BUFFER_SIZE 0x50000
char gen_dtb_buf[DTB_BUFFER_SIZE]; /* accessed by modules */
static char gen_dtb_base_buf[DTB_BUFFER_SIZE];

void *fdt_ori;

struct ps_io_ops _io_ops;

static jmp_buf restart_jmp_buf;

void camkes_make_simple(simple_t *simple);

int WEAK camkes_dtb_untyped_count();
seL4_CPtr WEAK camkes_dtb_get_nth_untyped(int n, size_t *size_bits, uintptr_t *paddr);
seL4_Error WEAK camkes_dtb_get_irq_cap(int irq, seL4_CNode cnode, seL4_Word index, uint8_t depth);
simple_get_IRQ_handler_fn original_simple_get_irq_fn;
int *WEAK camkes_dtb_get_irqs(int *num_irqs);
char **WEAK camkes_dtb_get_node_paths(int *num_nodes);
char **WEAK camkes_dtb_get_plat_keep_devices(int *num_nodes);
char **WEAK camkes_dtb_get_plat_keep_devices_and_subtree(int *num_nodes);

#ifdef CONFIG_ARM_SMMU
seL4_CPtr camkes_get_smmu_cb_cap();
seL4_CPtr camkes_get_smmu_sid_cap();
#endif

int get_crossvm_irq_num(void)
{
    return free_plat_interrupts[0];
}

static int _dma_morecore(size_t min_size, int cached, struct dma_mem_descriptor *dma_desc)
{
    static uint32_t _vaddr = DMA_VSTART;
    struct seL4_ARM_Page_GetAddress getaddr_ret;
    seL4_CPtr frame;
    seL4_CPtr pd;
    vka_t *vka;
    int err;

    pd = simple_get_pd(&_simple);
    vka = &_vka;

    /* Create a frame */
    frame = vka_alloc_frame_leaky(vka, seL4_PageBits);
    assert(frame);
    if (!frame) {
        return -1;
    }

    /* Try to map the page */
    err = seL4_ARM_Page_Map(frame, pd, _vaddr, seL4_AllRights, 0);
    if (err) {
        seL4_CPtr pt;
        /* Allocate a page table */
        pt = vka_alloc_page_table_leaky(vka);
        if (!pt) {
            printf("Failed to create page table\n");
            return -1;
        }
        /* Map the page table */
        err = seL4_ARM_PageTable_Map(pt, pd, _vaddr, 0);
        if (err) {
            printf("Failed to map page table\n");
            return -1;
        }
        /* Try to map the page again */
        err = seL4_ARM_Page_Map(frame, pd, _vaddr, seL4_AllRights, 0);
        if (err) {
            printf("Failed to map page\n");
            return -1;
        }

    }

    /* Find the physical address of the page */
    getaddr_ret = seL4_ARM_Page_GetAddress(frame);
    assert(!getaddr_ret.error);
    /* Setup dma memory description */
    dma_desc->vaddr = _vaddr;
    dma_desc->paddr = getaddr_ret.paddr;
    dma_desc->cached = 0;
    dma_desc->size_bits = seL4_PageBits;
    dma_desc->alloc_cookie = (void *)frame;
    dma_desc->cookie = NULL;
    /* Advance the virtual address marker */
    _vaddr += SIZE_BITS_TO_BYTES(seL4_PageBits);
    return 0;
}

typedef struct vm_io_cookie {
    simple_t simple;
    vka_t vka;
    vspace_t vspace;
} vm_io_cookie_t;

static void *vm_map_paddr_with_page_size(vm_io_cookie_t *io_mapper, uintptr_t paddr, size_t size, int page_size_bits,
                                         int cached)
{

    vka_t *vka = &io_mapper->vka;
    vspace_t *vspace = &io_mapper->vspace;
    simple_t *simple = &io_mapper->simple;

    /* search at start of page */
    int page_size = BIT(page_size_bits);
    uintptr_t start = ROUND_DOWN(paddr, page_size);
    uintptr_t offset = paddr - start;
    size += offset;

    /* calculate number of pages */
    unsigned int num_pages = ROUND_UP(size, page_size) >> page_size_bits;
    assert(num_pages << page_size_bits >= size);
    seL4_CPtr frames[num_pages];
    seL4_Word cookies[num_pages];

    /* get all of the physical frame caps */
    for (unsigned int i = 0; i < num_pages; i++) {
        /* allocate a cslot */
        int error = vka_cspace_alloc(vka, &frames[i]);
        if (error) {
            ZF_LOGE("cspace alloc failed");
            assert(error == 0);
            /* we don't clean up as everything has gone to hell */
            return NULL;
        }

        /* create a path */
        cspacepath_t path;
        vka_cspace_make_path(vka, frames[i], &path);

        error = vka_utspace_alloc_at(vka, &path, kobject_get_type(KOBJECT_FRAME, page_size_bits), page_size_bits,
                                     start + (i * page_size), &cookies[i]);

        if (error) {
            cookies[i] = -1;
            error = simple_get_frame_cap(simple, (void *)start + (i * page_size), page_size_bits, &path);
            if (error) {
                /* free this slot, and then do general cleanup of the rest of the slots.
                 * this avoids a needless seL4_CNode_Delete of this slot, as there is no
                 * cap in it */
                vka_cspace_free(vka, frames[i]);
                num_pages = i;
                goto error;
            }
        }

    }

    /* Now map the frames in */
    void *vaddr = vspace_map_pages(vspace, frames, NULL, seL4_AllRights, num_pages, page_size_bits, cached);
    if (vaddr) {
        return vaddr + offset;
    }
error:
    for (unsigned int i = 0; i < num_pages; i++) {
        cspacepath_t path;
        vka_cspace_make_path(vka, frames[i], &path);
        vka_cnode_delete(&path);
        if (cookies[i] != -1) {
            vka_utspace_free(vka, kobject_get_type(KOBJECT_FRAME, page_size_bits), page_size_bits, cookies[i]);
        }
        vka_cspace_free(vka, frames[i]);
    }
    return NULL;
}

/* Force the _dataport_frames  section to be created even if no modules are defined. */
static USED SECTION("_dataport_frames") struct {} dummy_dataport_frame;
/* Definitions so that we can find the exposed dataport frames */
extern dataport_frame_t __start__dataport_frames[];
extern dataport_frame_t __stop__dataport_frames[];

static void *find_dataport_frame(uintptr_t paddr, uintptr_t size)
{
    for (dataport_frame_t *frame = __start__dataport_frames;
         frame < __stop__dataport_frames; frame++) {
        if (frame->paddr == paddr) {
            if (frame->size == size) {
                return (void *) frame->vaddr;
            } else {
                ZF_LOGF("ERROR: found mapping for %p, wrong size %zu, expected %zu", (void *) paddr, frame->size, size);
            }
        }
    }
    return NULL;
}

static void *vm_map_paddr(void *cookie, uintptr_t paddr, size_t size, int cached, ps_mem_flags_t flags)
{
    void *vaddr = find_dataport_frame(paddr, size);
    if (vaddr) {
        return vaddr;
    }
    vm_io_cookie_t *io_mapper = (vm_io_cookie_t *)cookie;

    int frame_size_index = 0;
    /* find the largest reasonable frame size */
    while (frame_size_index + 1 < SEL4_NUM_PAGE_SIZES) {
        if (size >> sel4_page_sizes[frame_size_index + 1] == 0) {
            break;
        }
        frame_size_index++;
    }

    /* try mapping in this and all smaller frame sizes until something works */
    for (int i = frame_size_index; i >= 0; i--) {
        void *result = vm_map_paddr_with_page_size(io_mapper, paddr, size, sel4_page_sizes[i], cached);
        if (result) {
            return result;
        }
    }
    ZF_LOGE("Failed to map address %p", (void *)paddr);
    return NULL;
}

static void vm_unmap_vaddr(void *cookie, void *vaddr, size_t size)
{
    ZF_LOGF("Not unmapping vaddr %p", vaddr);
}

static int vm_new_io_mapper(simple_t simple, vspace_t vspace, vka_t vka, ps_io_mapper_t *io_mapper)
{
    vm_io_cookie_t *cookie;
    cookie = (vm_io_cookie_t *)malloc(sizeof(*cookie));
    if (!cookie) {
        ZF_LOGE("Failed to allocate %zu bytes", sizeof(*cookie));
        return -1;
    }
    *cookie = (vm_io_cookie_t) {
        .vspace = vspace,
        .simple = simple,
        .vka = vka
    };
    *io_mapper = (ps_io_mapper_t) {
        .cookie = cookie,
        .io_map_fn = vm_map_paddr,
        .io_unmap_fn = vm_unmap_vaddr
    };
    return 0;
}

static seL4_Error vm_simple_get_irq(void *data, int irq, seL4_CNode cnode, seL4_Word index, uint8_t depth)
{
    seL4_Error res;
    res = original_simple_get_irq_fn(_simple.data, irq, cnode, index, depth);
    if (res == seL4_NoError) {
        return res;
    }
    if (camkes_dtb_get_irq_cap) {
        return camkes_dtb_get_irq_cap(irq, cnode, index, depth);
    } else {
        return seL4_FailedLookup;
    }
}


/* First try and allocate TCBs from the CAmkES static pool before retyping from Untypeds */
static vka_utspace_alloc_maybe_device_fn utspace_alloc_copy;
static vka_utspace_alloc_at_fn utspace_alloc_at_copy;

static int camkes_vm_utspace_alloc_maybe_device_fn(void *data, const cspacepath_t *dest, seL4_Word type,
                                                   seL4_Word size_bits, bool can_use_dev, seL4_Word *res)
{
    if (type == seL4_TCBObject) {
        seL4_CPtr cap = camkes_alloc(type, 0, 0);
        if (cap != seL4_CapNull) {
            cspacepath_t src;
            vka_cspace_make_path(&_vka, cap, &src);
            return vka_cnode_copy(dest, &src, seL4_AllRights);
        }
    }
    return utspace_alloc_copy(data, dest, type, size_bits, can_use_dev, res);
}

static int camkes_vm_utspace_alloc_at(void *data, const cspacepath_t *dest, seL4_Word type,
                                      seL4_Word size_bits, uintptr_t paddr, seL4_Word *res)
{
    if (type == seL4_ARM_SmallPageObject) {
        for (dataport_frame_t *frame = __start__dataport_frames;
             frame < __stop__dataport_frames; frame++) {
            if (frame->paddr == paddr) {
                if (frame->size == BIT(size_bits)) {
                    cspacepath_t src;
                    vka_cspace_make_path(&_vka, frame->cap, &src);
                    return vka_cnode_copy(dest, &src, seL4_AllRights);
                } else {
                    ZF_LOGF("ERROR: found mapping for %p, wrong size %zu, expected %zu", (void *) paddr, frame->size, BIT(size_bits));
                }
            }
        }

    }
    return utspace_alloc_at_copy(data, dest, type, size_bits, paddr, res);

}

static bool add_uts(const vm_config_t *vm_config, vka_t *vka, seL4_CPtr cap,
                    uintptr_t paddr, size_t size_bits, bool is_device)
{
    cspacepath_t path;
    vka_cspace_make_path(vka, cap, &path);

    /*
     * The general usage concept for the different UT pools is:
     *
     * ALLOCMAN_UT_KERNEL:
     *   Physical RAM mapped in the kernel windows. This can be used to create
     *   arbitrary kernel objects.
     *
     * ALLOCMAN_UT_DEV_MEM:
     *   UTs from a device region, which is also usable RAM. This exists because
     *   on 32-bit platforms the kernel window could be too small to map all
     *   physical RAM and thus is able to directly access it. Kernel objects can
     *   be created from these UTs, if the 'canBeDev' parameter in 'alloc' is
     *   set to true. However, such objects may have restrictions in what it can
     *   be used for, because the kernel cannot access the content directly.
     *
     * ALLOCMAN_UT_DEV:
     *   Device regions. Such UTs will never be used for an allocation, unless
     *   explicitly requested by physical address.
     *
     * ToDo: The practical distinction between ALLOCMAN_UT_DEV and
     *       ALLOCMAN_UT_DEV_MEM is, whether the UT can be requested from the
     *       allocator without providing it's backing physical address. Since
     *       UTs from the guest RAM region are supposed to be requested using
     *       the physical address anyway, there seem not reason why they can't
     *       be in the ALLOCMAN_UT_DEV pool, too.
     *       The only know use case where this matters is on the NVidia TK1
     *       platform. It has a SMMU, so in order to use RAM in a VM, there is
     *       no need for VMs to have their RAM addresses match the physical RAM
     *       addresses (VM config option "map_one_to_one"). However, it is a
     *       32-bit architecture and the kernel window is too small to cover all
     *       physical RAM. Thus, the additional RAM is made available via device
     *       UTs. The allocator needs to be told about this, which is what
     *       "ram_paddr_base" is used for and and why it's different from
     *       "ram_base".
     */

    bool is_guest_ram = (paddr >= vm_config->ram.phys_base) &&
                        ((paddr - vm_config->ram.phys_base) < vm_config->ram.size);

    int ut_type = !is_device ? ALLOCMAN_UT_KERNEL
                  : is_guest_ram ? ALLOCMAN_UT_DEV_MEM
                  : ALLOCMAN_UT_DEV;

    allocman_t *allocman = vka->data;

    return allocman_utspace_add_uts(allocman, 1, &path, &size_bits, &paddr,
                                    ut_type);
}

static int vmm_init(const vm_config_t *vm_config)
{
    vka_object_t fault_ep_obj;
    vka_t *vka;
    simple_t *simple;
    vspace_t *vspace;
    int err;

    vka = &_vka;
    vspace = &_vspace;
    simple = &_simple;
    fault_ep_obj.cptr = 0;

    /* Camkes adds nothing to our address space, so this array is empty */
    void *existing_frames[] = {
        NULL
    };

    camkes_make_simple(simple);
    original_simple_get_irq_fn = simple->arch_simple.irq;
    simple->arch_simple.irq = &vm_simple_get_irq;

    start_extra_frame_caps = simple_last_valid_cap(simple) + 1;

    /* Initialize allocator */
    allocman = bootstrap_use_current_1level(
                   simple_get_cnode(simple),
                   simple_get_cnode_size_bits(simple),
                   simple_last_valid_cap(simple) + 1 + num_extra_frame_caps,
                   BIT(simple_get_cnode_size_bits(simple)),
                   get_allocator_mempool_size(), get_allocator_mempool()
               );
    assert(allocman);

    allocman_make_vka(vka, allocman);

    /* Overwrite alloc function with a custom wrapper for statically allocated TCBs. */
    utspace_alloc_copy = vka->utspace_alloc_maybe_device;
    vka->utspace_alloc_maybe_device = camkes_vm_utspace_alloc_maybe_device_fn;
    utspace_alloc_at_copy = vka->utspace_alloc_at;
    vka->utspace_alloc_at = camkes_vm_utspace_alloc_at;

    int cnt = simple_get_untyped_count(simple);
    if (cnt < 0) {
        ZF_LOGE("Failed to get simple untyped count (%d)", cnt);
        return -1;
    }
    for (int i = 0; i < cnt; i++) {
        size_t size_bits;
        uintptr_t paddr;
        bool is_device;
        seL4_CPtr cap = simple_get_nth_untyped(simple, i, &size_bits, &paddr, &is_device);
        err = add_uts(vm_config, vka, cap, paddr, size_bits, is_device);
        assert(!err);
    }

    if (camkes_dtb_untyped_count) {
        cnt = camkes_dtb_untyped_count();
        if (cnt < 0) {
            ZF_LOGE("Failed to get CAmkES DTB untyped count (%d)", cnt);
            return -1;
        }
        for (int i = 0; i < cnt; i++) {
            size_t size_bits;
            uintptr_t paddr;
            seL4_CPtr cap = camkes_dtb_get_nth_untyped(i, &size_bits, &paddr);
            /* These UTs are considered device untypeds */
            err = add_uts(vm_config, vka, cap, paddr, size_bits, true);
            assert(!err);
        }
    }
    /* Initialize the vspace */
    err = sel4utils_bootstrap_vspace(vspace, &_alloc_data,
                                     simple_get_init_cap(simple, seL4_CapInitThreadPD), vka, NULL, NULL, existing_frames);
    assert(!err);

    /* Initialise device support */
    err = vm_new_io_mapper(*simple, *vspace, *vka,
                           &_io_ops.io_mapper);
    assert(!err);

    /* Initialise MUX subsystem for platforms that need it */
#ifdef CONFIG_PLAT_EXYNOS5410
    err = mux_sys_init(&_io_ops, NULL, &_io_ops.mux_sys);
    assert(!err);
#endif

    /* Initialise DMA */
    err = dma_dmaman_init(&_dma_morecore, NULL, &_io_ops.dma_manager);
    assert(!err);

    /* Allocate an endpoint for listening to events */
    err = vka_alloc_endpoint(vka, &fault_ep_obj);
    assert(!err);
    _fault_endpoint = fault_ep_obj.cptr;

    err = sel4platsupport_new_malloc_ops(&_io_ops.malloc_ops);
    assert(!err);

    /* Create an IRQ server */
    _irq_server = irq_server_new(vspace, vka, IRQSERVER_PRIO,
                                 simple, simple_get_cnode(simple), fault_ep_obj.cptr,
                                 IRQ_MESSAGE_LABEL, 256, &_io_ops.malloc_ops);
    assert(_irq_server);

    int num_pt_irqs = ARRAY_SIZE(linux_pt_irqs);

    if (camkes_dtb_get_irqs) {
        int num_dtb_irqs = 0;
        int *dtb_irqs = camkes_dtb_get_irqs(&num_dtb_irqs);
        num_pt_irqs += num_dtb_irqs;
    }

    /* Create threads for the IRQ server */
    size_t num_irq_threads = DIV_ROUND_UP(num_pt_irqs, seL4_BadgeBits);

    for (int i = 0; i < num_irq_threads; i++) {
        /* Create new IRQ server threads and have them allocate notifications for us */
        thread_id_t t_id = irq_server_thread_new(_irq_server, seL4_CapNull,
                                                 0, -1);
        assert(t_id >= 0);
    }

    return 0;
}

void restart_component(void)
{
    longjmp(restart_jmp_buf, 1);
}

extern char __bss_start[];
extern char _bss_end__[];
extern char __sysinfo[];
extern char __libc[];
extern char morecore_area[];
extern char morecore_size[];
extern uintptr_t morecore_top;

void reset_resources(void)
{
    simple_t simple;
    camkes_make_simple(&simple);
    int i;
    seL4_CPtr root = simple_get_cnode(&simple);
    int error;
    /* revoke any of our initial untyped resources */
    for (i = 0; i < simple_get_untyped_count(&simple); i++) {
        size_t size_bits;
        uintptr_t paddr;
        bool device;
        seL4_CPtr ut = simple_get_nth_untyped(&simple, i, &size_bits, &paddr, &device);
        error = seL4_CNode_Revoke(root, ut, 32);
        assert(error == seL4_NoError);
    }
    /* delete anything from any slots that should be empty */
    for (i = simple_last_valid_cap(&simple) + 1; i < BIT(simple_get_cnode_size_bits(&simple)); i++) {
        seL4_CNode_Delete(root, i, 32);
    }
    /* save some pieces of the bss that we actually don't want to zero */
    char save_sysinfo[4];
    char save_libc[34];
    char save_morecore_area[4];
    char save_morecore_size[4];
    memcpy(save_libc, __libc, 34);
    memcpy(save_sysinfo, __sysinfo, 4);
    memcpy(save_morecore_area, morecore_area, 4);
    memcpy(save_morecore_size, morecore_size, 4);
    /* zero the bss */
    memset(__bss_start, 0, (uintptr_t)_bss_end__ - (uintptr_t)__bss_start);
    /* restore these pieces */
    memcpy(__libc, save_libc, 34);
    memcpy(__sysinfo, save_sysinfo, 4);
    memcpy(morecore_area, save_morecore_area, 4);
    memcpy(morecore_size, save_morecore_size, 4);
    morecore_top = (uintptr_t) &morecore_area[CONFIG_LIB_SEL4_MUSLC_SYS_MORECORE_BYTES];
}

static seL4_CPtr restart_tcb;

static void restart_event(void *arg)
{
    int ret UNUSED = restart_event_reg_callback(restart_event, NULL);
    seL4_UserContext context = {
        .pc = (seL4_Word)restart_component,
    };
    seL4_TCB_WriteRegisters(restart_tcb, true, 0, 1, &context);
}


static void do_irq_server_ack(vm_vcpu_t *vcpu, int irq, void *token)
{
    assert(token);
    irq_token_t irq_token = token;
    /* If the acknowledge function pointer is NULL, this means that the actual
     * interrupt has not arrived/we have not handled it. So we defer it for
     * later.
     *
     * NOTE: this only happens with the arch timer, in that we receive
     * an EOI from Linux before we inject the VIRQ */
    if (irq_token->acknowledge_fn && irq_token->ack_data) {
        int err = irq_token->acknowledge_fn(irq_token->ack_data);
        assert(!err);
        irq_token->ack_data = NULL;
    }
}

static void irq_handler(void *data, ps_irq_acknowledge_fn_t acknowledge_fn, void *ack_data)
{
    /* We don't actually acknowledge the IRQ yet, this is done later when we update the VGIC's state. */
    assert(data);
    irq_token_t token = data;
    /* Fill in the rest of the details */
    token->acknowledge_fn = acknowledge_fn;
    token->ack_data = ack_data;
    int err;
    err = vm_inject_irq(token->vm->vcpus[BOOT_VCPU], token->virq);
    if (err) {
        ZF_LOGW("IRQ %d Dropped", token->virq);
    }
}


/* Force the _vmm_module  section to be created even if no modules are defined. */
static USED SECTION("_vmm_module") struct {} dummy_module;
extern vmm_module_t __start__vmm_module[];
extern vmm_module_t __stop__vmm_module[];

static int install_vm_devices(vm_t *vm, const vm_config_t *vm_config)
{
    int err;

    /* Install virtual devices */
    if (config_set(CONFIG_VM_PCI_SUPPORT)) {
        err = vm_install_vpci(vm, io_ports, pci);
        if (err) {
            ZF_LOGE("Failed to install VPCI device");
            return -1;
        }
    }

    int max_vmm_modules = (int)(__stop__vmm_module - __start__vmm_module);
    int num_vmm_modules = 0;
    for (vmm_module_t *i = __start__vmm_module; i < __stop__vmm_module; i++) {
        ZF_LOGE("module name: %s", i->name);
        i->init_module(vm, i->cookie);
        num_vmm_modules++;
    }

    return 0;

}

static int route_irq(int irq_num, vm_vcpu_t *vcpu, irq_server_t *irq_server)
{
    ps_irq_t irq = { .type = PS_INTERRUPT, .irq = { .number = irq_num }};
    irq_callback_fn_t handler = NULL;
    if (get_custom_irq_handler) {
        handler = get_custom_irq_handler(irq);
    }
    if (handler == NULL) {
        handler = &irq_handler;
    }

    irq_token_t token = calloc(1, sizeof(struct irq_token));
    if (token == NULL) {
        return -1;
    }

    int err = vm_register_irq(vcpu, irq.irq.number, &do_irq_server_ack, token);
    if (err == -1) {
        return -1;
    }

    token->virq = irq.irq.number;
    token->irq = irq;
    token->vm = vcpu->vm;

    irq_id_t irq_id = irq_server_register_irq(irq_server, irq, handler, token);
    if (irq_id < 0) {
        return -1;
    }

    return 0;
}

static int route_irqs(vm_vcpu_t *vcpu, irq_server_t *irq_server)
{
    int err;
    int i;
    for (i = 0; i < ARRAY_SIZE(linux_pt_irqs); i++) {
        int irq_num = linux_pt_irqs[i];
        err = route_irq(irq_num, vcpu, irq_server);
        if (err) {
            return err;
        }
    }
    if (camkes_dtb_get_irqs) {
        int num_dtb_irqs = 0;
        int *dtb_irqs = camkes_dtb_get_irqs(&num_dtb_irqs);
        for (i = 0; i < num_dtb_irqs; i++) {
            int irq_num = dtb_irqs[i];
            err = route_irq(irq_num, vcpu, irq_server);
            if (err) {
                return err;
            }
        }
    }
    return 0;
}

static int vm_dtb_init(vm_t *vm, const vm_config_t *vm_config)
{
    int err;

    /* Setup the base DTB. By default it's the one that CAmkES provides. */
    camkes_io_fdt(&(_io_ops.io_fdt));
    fdt_ori = (void *)ps_io_fdt_get(&_io_ops.io_fdt);
    ZF_LOGW_IF(!fdt_ori, "CAmkES did not provide a DTB");
    /* If explicitly requested, the CAmkES DTB can be ignored and one provided
     * by the file server can be used instead.
     */
    if ((NULL != vm_config->files.dtb_base) && ('\0' != vm_config->files.dtb_base[0])) {
        int dtb_fd = open(vm_config->files.dtb_base, 0);
        ZF_LOGI("using DTB file '%s' as base", vm_config->files.dtb_base);
        /* If dtb_base is in the file server, grab it and use it as a base */
        if (dtb_fd < 0) {
            ZF_LOGE("opening DTB file failed (%d)", dtb_fd);
        } else {
            size_t dtb_len = read(dtb_fd, gen_dtb_base_buf, DTB_BUFFER_SIZE);
            close(dtb_fd);
            if (dtb_len <= 0) {
                ZF_LOGE("reading DTB file failed (%d)", dtb_len);
            } else {
                /* overwrite */
                fdt_ori = gen_dtb_base_buf;
            }
        }
    }

    /* We are done if DTB generation is not enabled. */
    if (!vm_config->generate_dtb) {
        return 0;
    }

    ZF_LOGW_IF(!fdt_ori, "No base DTB set");

    fdtgen_context_t *context = fdtgen_new_context(gen_dtb_buf, sizeof(gen_dtb_buf));
    if (context == NULL) {
        ZF_LOGE("Couldn't create fdtgen context");
        return -1;
    }

    /* If VM has "plat_keep_devices" set, use it! Else, just use the default */
    int num_keep_devices = 0;
    char **keep_devices = NULL;
    if (camkes_dtb_get_plat_keep_devices) {
        keep_devices = camkes_dtb_get_plat_keep_devices(&num_keep_devices);
    }
    if (num_keep_devices) {
        fdtgen_keep_nodes(context, (const char **)keep_devices, num_keep_devices);
    } else {
        fdtgen_keep_nodes(context, plat_keep_devices, ARRAY_SIZE(plat_keep_devices));
    }

    /* If VM has "plat_keep_devices and subtree" set, use it! Else, just use the default */
    int num_keep_devices_and_subtree = 0;
    char **keep_devices_and_subtree;
    if (camkes_dtb_get_plat_keep_devices_and_subtree) {
        keep_devices_and_subtree = camkes_dtb_get_plat_keep_devices_and_subtree(&num_keep_devices_and_subtree);
    }
    if (num_keep_devices_and_subtree) {
        for (int i = 0; i < num_keep_devices_and_subtree; i++) {
            fdtgen_keep_node_subtree(context, fdt_ori, keep_devices_and_subtree[i]);
        }
    } else {
        for (int i = 0; i < ARRAY_SIZE(plat_keep_device_and_subtree); i++) {
            fdtgen_keep_node_subtree(context, fdt_ori, plat_keep_device_and_subtree[i]);
        }
    }

    for (int i = 0; i < ARRAY_SIZE(plat_keep_device_and_subtree_and_disable); i++) {
        fdtgen_keep_node_subtree_disable(context, fdt_ori, plat_keep_device_and_subtree_and_disable[i]);
    }
    fdtgen_keep_nodes_and_disable(context, plat_keep_device_and_disable, ARRAY_SIZE(plat_keep_device_and_disable));

    int num_paths = 0;
    char **paths = NULL;
    if (camkes_dtb_get_node_paths) {
        paths = camkes_dtb_get_node_paths(&num_paths);
    }
    fdtgen_keep_nodes(context, (const char **)paths, num_paths);

    /* build a DTB in gen_dtb_buf */
    err = fdtgen_generate(context, fdt_ori);
    fdtgen_free_context(context);
    if (err) {
        ZF_LOGE("Couldn't generate base DTB, error %d", err);
        return -1;
    }

    /* Now the DTB is in gen_dtb_buf and all manipulation must happen there. */

    /* generate a memory node */
    err = fdt_generate_memory_node(gen_dtb_buf, vm_config->ram.base,
                                   vm_config->ram.size);
    if (err) {
        ZF_LOGE("Couldn't generate memory_node (%d)\n", err);
        return -1;
    }

    return 0;
}

static int vm_dtb_finalize(vm_t *vm, const vm_config_t *vm_config)
{
    assert(vm_config->generate_dtb);

    if (config_set(CONFIG_VM_PCI_SUPPORT)) {
        /* Modules can add PCI devices, so the PCI device tree node can be
         * created only after all modules have been set up.
         */
        int gic_offset = fdt_path_offset(fdt_ori, GIC_NODE_PATH);
        if (gic_offset < 0) {
            ZF_LOGE("Failed to find gic node from path: %s", GIC_NODE_PATH);
            return -1;
        }
        int gic_phandle = fdt_get_phandle(fdt_ori, gic_offset);
        if (0 == gic_phandle) {
            ZF_LOGE("Failed to find phandle in gic node");
            return -1;
        }
        int err = fdt_generate_vpci_node(vm, pci, gen_dtb_buf, gic_phandle);
        if (err) {
            ZF_LOGE("Couldn't generate vpci_node (%d)", err);
            return -1;
        }
    }

    fdt_pack(gen_dtb_buf);
    return 0;
}

static int load_generated_dtb(vm_t *vm, uintptr_t paddr, void *addr, size_t size, size_t offset, void *cookie)
{
    ZF_LOGD("paddr: 0x%lx, addr: 0x%lx, size: 0x%lx, offset: 0x%lx", paddr, (seL4_Word) addr, size, offset);
    memcpy(addr, cookie + offset, size);
    return 0;
}

static int load_vm_images(vm_t *vm, const vm_config_t *vm_config)
{
    seL4_Word entry;
    seL4_Word dtb;
    int err;

    /* Load kernel */
    printf("Loading Kernel: \'%s\'\n", vm_config->files.kernel);
    guest_kernel_image_t kernel_image_info;
    err = vm_load_guest_kernel(vm, vm_config->files.kernel, vm_config->ram.base,
                               0, &kernel_image_info);
    entry = kernel_image_info.kernel_image.load_paddr;
    if (!entry || err) {
        return -1;
    }

    /* generate a chosen node */
    if (vm_config->generate_dtb) {
        err = fdt_generate_chosen_node(gen_dtb_buf, vm_config->kernel_stdout,
                                       vm_config->kernel_bootcmdline, NUM_VCPUS);
        if (err) {
            ZF_LOGE("Couldn't generate chosen_node (%d)\n", err);
            return -1;
        }
    }

    /* Attempt to load initrd if provided */
    guest_image_t initrd_image;
    if (vm_config->provide_initrd) {
        printf("Loading Initrd: \'%s\'\n", vm_config->files.initrd);
        err = vm_load_guest_module(vm, vm_config->files.initrd,
                                   vm_config->initrd_addr, 0, &initrd_image);
        void *initrd = (void *)initrd_image.load_paddr;
        if (!initrd || err) {
            return -1;
        }
        if (vm_config->generate_dtb) {
            err = fdt_append_chosen_node_with_initrd_info(gen_dtb_buf,
                                                          vm_config->initrd_addr,
                                                          initrd_image.size);
            if (err) {
                ZF_LOGE("Couldn't generate chosen_node_with_initrd_info (%d)\n", err);
                return -1;
            }
        }
    }

    if (vm_config->generate_dtb) {
        ZF_LOGW_IF(vm_config->provide_dtb,
                   "provide_dtb and generate_dtb are both set. The provided dtb will NOT be loaded");
        err = vm_dtb_finalize(vm, vm_config);
        if (err) {
            ZF_LOGE("Couldn't generate DTB (%d)\n", err);
            return -1;
        }
        printf("Loading Generated DTB\n");
        vm_ram_mark_allocated(vm, vm_config->dtb_addr, sizeof(gen_dtb_buf));
        vm_ram_touch(vm, vm_config->dtb_addr, sizeof(gen_dtb_buf), load_generated_dtb,
                     gen_dtb_buf);
        dtb = vm_config->dtb_addr;
    } else if (vm_config->provide_dtb) {
        printf("Loading DTB: \'%s\'\n", vm_config->files.dtb);

        /* Load device tree */
        guest_image_t dtb_image;
        err = vm_load_guest_module(vm, vm_config->files.dtb,
                                   vm_config->dtb_addr, 0, &dtb_image);
        dtb = dtb_image.load_paddr;
        if (!dtb || err) {
            return -1;
        }
    } else {
        ZF_LOGW("%s not given a DTB - This may be appropriate for your guest, but it " \
                "may also break things!", get_instance_name());
    }

    /* Set boot arguments */
    err = vcpu_set_bootargs(vm->vcpus[BOOT_VCPU], entry, MACH_TYPE, dtb);
    if (err) {
        printf("Error: Failed to set boot arguments\n");
        return -1;
    }

    return 0;
}

/* Async event handling registration implementation */
typedef struct async_event_handler {
    seL4_Word badge;
    async_event_handler_fn_t callback;
    void *cookie;
} async_event_handler_t;

static int callback_idx = 0;
static async_event_handler_t *callback_arr = NULL;

int register_async_event_handler(seL4_Word badge, async_event_handler_fn_t callback, void *cookie)
{
    if (callback_arr == NULL) {
        callback_arr = calloc(1, sizeof(*callback_arr));
    } else {
        callback_arr = realloc(callback_arr, (callback_idx + 1) * sizeof(*callback_arr));
    }
    if (callback_arr == NULL) {
        ZF_LOGE("Failed to allocate memory for callback_arr");
        return -1;
    }

    async_event_handler_t handler = {.badge = badge, .callback = callback, .cookie = cookie};
    callback_arr[callback_idx] = handler;
    callback_idx++;
    return 0;
}

static int handle_async_event(vm_t *vm, seL4_Word badge, seL4_MessageInfo_t tag, void *cookie)
{
    seL4_Word label = seL4_MessageInfo_get_label(tag);
    if (badge == 0) {
        if (label == IRQ_MESSAGE_LABEL) {
            irq_server_handle_irq_ipc(_irq_server, tag);
        } else {
            ZF_LOGE("Unknown label (%"SEL4_PRId_word")", label);
        }
#ifdef FEATURE_VUSB
    } else if (badge == VUSB_NBADGE) {
        vusb_notify();
#endif
    } else {
        bool found_handler = false;
        for (int i = 0; i < callback_idx; i++) {
            assert(callback_arr);
            if ((badge & callback_arr[i].badge) == callback_arr[i].badge) {
                callback_arr[i].callback(vm, callback_arr[i].cookie);
                found_handler = true;
            }
        }
        if (!found_handler) {
            ZF_LOGE("Unknown badge (%"SEL4_PRId_word")", badge);
        }
    }
    return 0;
}

static int alloc_vm_device_cap(uintptr_t addr, vm_t *vm, vm_frame_t *frame_result)
{
    int err;
    cspacepath_t frame;
    err = vka_cspace_alloc_path(vm->vka, &frame);
    if (err) {
        ZF_LOGE("Failed to allocate cslot\n");
        return -1;
    }
    seL4_Word cookie;
    err = vka_utspace_alloc_at(vm->vka, &frame, kobject_get_type(KOBJECT_FRAME, seL4_PageBits), seL4_PageBits, addr,
                               &cookie);
    if (err) {
        ZF_LOGV("Grabbing the entire cap for device memory");
        err = simple_get_frame_cap(vm->simple, (void *)addr, seL4_PageBits, &frame);
        if (err) {
            ZF_LOGV("Failed to grab the entire cap for addr 0x%"PRIxPTR, addr);
            return -1;
        }
    }
    frame_result->cptr = frame.capPtr;
    frame_result->rights = seL4_AllRights;
    frame_result->vaddr = addr;
    frame_result->size_bits = seL4_PageBits;
    return 0;
}

static int alloc_vm_ram_cap(uintptr_t addr, vm_t *vm, vm_frame_t *frame_result)
{
    int err;
    cspacepath_t frame;
    vka_object_t frame_obj;
    err = vka_alloc_frame_maybe_device(vm->vka, seL4_PageBits, true, &frame_obj);
    if (err) {
        ZF_LOGF("Failed vka_alloc_frame_maybe_device");
        return -1;
    }
    vka_cspace_make_path(vm->vka, frame_obj.cptr, &frame);
    frame_result->cptr = frame.capPtr;
    frame_result->rights = seL4_AllRights;
    frame_result->vaddr = addr;
    frame_result->size_bits = seL4_PageBits;
    return 0;
}

static vm_frame_t on_demand_iterator(uintptr_t addr, void *cookie)
{
    int err;
    uintptr_t paddr = PAGE_ALIGN(addr, SIZE_BITS_TO_BYTES(seL4_PageBits));
    vm_frame_t frame_result = { seL4_CapNull, seL4_NoRights, 0, 0 };
    vm_t *vm = (vm_t *)cookie;
    /* Attempt allocating device memory */
    err = alloc_vm_device_cap(paddr, vm, &frame_result);
    if (!err) {
        printf("OnDemandInstall: Created device-backed memory for addr 0x%"PRIxPTR"\n", addr);
        return frame_result;
    }
    /* Attempt allocating ram memory */
    err = alloc_vm_ram_cap(paddr, vm, &frame_result);
    if (err) {
        ZF_LOGE("Failed to create on demand memory for addr 0x%"PRIxPTR, addr);
    }
    printf("OnDemandInstall: Created RAM-backed memory for addr 0x%"PRIxPTR"\n", addr);
    return frame_result;
}

static memory_fault_result_t handle_on_demand_fault_callback(vm_t *vm, vm_vcpu_t *vcpu, uintptr_t fault_addr,
                                                             size_t fault_length,
                                                             void *cookie)
{
    ZF_LOGE("Fault for on demand memory region: 0x%"PRIxPTR, fault_addr);
    return FAULT_ERROR;
}

memory_fault_result_t unhandled_mem_fault_callback(vm_t *vm, vm_vcpu_t *vcpu,
                                                   uintptr_t paddr, size_t len, void *cookie)
{
#ifdef CONFIG_VM_ONDEMAND_DEVICE_INSTALL
    uintptr_t addr = PAGE_ALIGN(paddr, SIZE_BITS_TO_BYTES(seL4_PageBits));
    int mapped;
    vm_memory_reservation_t *reservation;
    switch (addr) {
    case 0:
        return FAULT_ERROR;
    default:
        reservation = vm_reserve_memory_at(vm, addr, SIZE_BITS_TO_BYTES(seL4_PageBits),
                                           handle_on_demand_fault_callback, NULL);
        mapped = vm_map_reservation(vm, reservation, on_demand_iterator, (void *)vm);
        if (!mapped) {
            return FAULT_RESTART;
        }
        ZF_LOGW("Unhandled fault on address 0x%"PRIxPTR, addr);
    }
#endif
    return FAULT_ERROR;
}

static int main_continued(void)
{
    vm_t vm;
    int err;

    /* setup for restart with a setjmp */
    while (setjmp(restart_jmp_buf) != 0) {
        err = vmm_process_reboot_callbacks(&vm, &reboot_hooks_list);
        if (err) {
            ZF_LOGF("vm_process_reboot_callbacks failed: %d", err);
        }
        reset_resources();
    }
    restart_tcb = camkes_get_tls()->tcb_cap;
    int ret UNUSED = restart_event_reg_callback(restart_event, NULL);

    /* install custom open/close/read implementations to redirect I/O from the VMM to
     * our file server */
    install_fileserver(FILE_SERVER_INTERFACE(fs));
    err = seL4_TCB_BindNotification(camkes_get_tls()->tcb_cap, notification_ready_notification());
    assert(!err);

    /* DTB initialization requires a running file server, as the DTB could be
     * based on a DTB file taken from there.
     */
    err = vm_dtb_init(&vm, &vm_config);
    if (err) {
        ZF_LOGE("Failed to init DTB (%d)", err);
        return -1;
    }

    err = vmm_pci_init(&pci);
    if (err) {
        ZF_LOGE("Failed to initialise vmm pci");
        return err;
    }

    err = vmm_io_port_init(&io_ports, FREE_IOPORT_START);
    if (err) {
        ZF_LOGE("Failed to initialise VM ioports");
        return err;
    }

    err = vmm_init(&vm_config);
    assert(!err);

    /* Create the VM */
    err = vm_init(&vm, &_vka, &_simple, _vspace, &_io_ops, _fault_endpoint, get_instance_name());
    assert(!err);
    err = vm_register_unhandled_mem_fault_callback(&vm, unhandled_mem_fault_callback, NULL);
    assert(!err);
    err = vm_register_notification_callback(&vm, handle_async_event, NULL);
    assert(!err);

    /* basic configuration flags */
    vm.entry = vm_config.entry_addr;
    vm.mem.clean_cache = vm_config.clean_cache;
    vm.mem.map_one_to_one = vm_config.map_one_to_one; /* Map memory 1:1 if configured to do so */

#ifdef CONFIG_TK1_SMMU
    /* install any iospaces */
    int iospace_caps;
    err = simple_get_iospace_cap_count(&_simple, &iospace_caps);
    if (err) {
        ZF_LOGF("Failed to get iospace count");
    }
    for (int i = 0; i < iospace_caps; i++) {
        seL4_CPtr iospace = simple_get_nth_iospace_cap(&_simple, i);
        err = vm_guest_add_iospace(&vm, &_vspace, iospace);
        if (err) {
            ZF_LOGF("Failed to add iospace");
        }
    }
#endif /* CONFIG_TK1_SMMU */
#ifdef CONFIG_ARM_SMMU
    /* configure the smmu */
    ZF_LOGD("Getting sid and cb caps");
    seL4_CPtr cb_cap = camkes_get_smmu_cb_cap();
    seL4_CPtr sid_cap = camkes_get_smmu_sid_cap();

    ZF_LOGD("Assigning vspace to context bank");
    err = seL4_ARM_CB_AssignVspace(cb_cap, vspace_get_root(&vm.mem.vm_vspace));
    ZF_LOGF_IF(err, "Failed to assign vspace to CB");

    ZF_LOGD("Binding stream id to context bank");
    err = seL4_ARM_SID_BindCB(sid_cap, cb_cap);
    ZF_LOGF_IF(err, "Failed to bind CB to SID");
#endif /* CONFIG_ARM_SMMU */

    err = vm_create_default_irq_controller(&vm);
    assert(!err);

    /* Create CPUs and DTB node */
    for (int i = 0; i < NUM_VCPUS; i++) {
        vm_vcpu_t *new_vcpu = create_vmm_plat_vcpu(&vm, VM_PRIO - 1);
        assert(new_vcpu);
    }
    if (vm_config.generate_dtb) {
        err = fdt_generate_plat_vcpu_node(&vm, gen_dtb_buf);
        if (err) {
            ZF_LOGE("Couldn't generate plat_vcpu_node (%d)", err);
            return -1;
        }
    }

    vm_vcpu_t *vm_vcpu = vm.vcpus[BOOT_VCPU];
    err = vm_assign_vcpu_target(vm_vcpu, 0);
    if (err) {
        return -1;
    }

    /* Route IRQs */
    err = route_irqs(vm_vcpu, _irq_server);
    if (err) {
        return -1;
    }

    /* Install devices */
    err = install_vm_devices(&vm, &vm_config);
    if (err) {
        ZF_LOGE("Error: Failed to install VM devices\n");
        seL4_DebugHalt();
        return -1;
    }

    /* Load system images */
    err = load_vm_images(&vm, &vm_config);
    if (err) {
        ZF_LOGE("Failed to load VM image\n");
        seL4_DebugHalt();
        return -1;
    }

    err = vcpu_start(vm_vcpu);
    if (err) {
        ZF_LOGE("Failed to start Boot VCPU");
        return -1;
    }

    while (1) {
        err = vm_run(&vm);
        if (err) {
            ZF_LOGE("Failed to run VM");
            seL4_DebugHalt();
            return -1;
        }
    }

    return 0;
}

/* base_prio and num_vcpus are optional attributes of the VM component. */
extern const int __attribute__((weak)) base_prio;
extern const int __attribute__((weak)) num_vcpus;

int run(void)
{
    /* if the base_prio attribute is set, use it */
    if (&base_prio != NULL) {
        VM_PRIO = base_prio;
    }
    /* if the num_vcpus attribute is set, try to use it */
    if (&num_vcpus != NULL) {
        if (num_vcpus > CONFIG_MAX_NUM_NODES) {
            ZF_LOGE("Invalid 'num_vcpus' attribute setting: Exceeds maximum number of supported nodes. Capping value to CONFIG_MAX_NUM_NODES (%d)",
                    CONFIG_MAX_NUM_NODES);
            NUM_VCPUS = CONFIG_MAX_NUM_NODES;
        } else if (num_vcpus <= 0) {
            ZF_LOGE("Invalid 'num_vcpus' attribute setting: Can't have 0 or negative amount of vcpus. Capping value to 1 vcpu (default value)");
        } else {
            NUM_VCPUS = num_vcpus;
        }
    }

    return main_continued();
}

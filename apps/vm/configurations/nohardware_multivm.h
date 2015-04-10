/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

/* Configuration for 2 VMs
    with a proof of concept component connected via vchan to vm0
*/

#define VM_NUM_GUESTS CONFIG_APP_CAMKES_VM_NUM_VM

#define PLAT_CONNECT_DEF() \
    connection seL4RPCCall vchan(from vm0.vchan_con, to vchan_0.vchan_com); \
    connection seL4SharedData vchan_sharemem_0(from vm0.share_mem, to vchan_0.share_mem); \
    connection seL4Asynch vchan_event(from vchan_0.vevent_sv, to hello.vevent); \
    connection seL4Asynch vchan_event_init(from vchan_0.vevent_cl, to vm0.vevent); \
    connection seL4RPCCall hvchan(from hello.vchan_con, to vchan_0.vchan_com); \
    connection seL4SharedData hvchan_sharemem_0(from hello.share_mem, to vchan_0.share_mem); \
    /**/ \
    connection seL4RPCCall hserial(from hello.putchar, to serial.vm0); \
    connection seL4RPCCall vchanserial(from vchan_0.putchar, to serial.vm0); \
    /**/ \
    connection seL4RPCCall vchan_1(from vm1.vchan_con, to vchan_0.vchan_com); \
    connection seL4SharedData vchan_sharemem_1(from vm1.share_mem, to vchan_0.share_mem); \
    connection seL4Asynch vchan_event_init_1(from vchan_0.vevent_cl, to vm1.vevent); \
    /**/

#define VM_CONFIGURATION_IOSPACES_0() \
    /**/

#define VM_CONFIGURATION_MMIO_0() \
    /**/

#define VM_CONFIGURATION_IOPORT_0() \
    /**/

#define VM_CONFIGURATION_IOSPACES_1() \
    /**/

#define VM_CONFIGURATION_MMIO_1() \
    /**/

#define VM_CONFIGURATION_IOPORT_1() \
    /**/

#define KERNEL_IMAGE "bzimage"
#define ROOTFS "rootfs.cpio"
#define VM_GUEST_CMDLINE "console=ttyS0,115200 console=tty0 root=/dev/mem i8042.nokbd=y i8042.nomux=y i8042.noaux=y io_delay=udelay noisapnp pci=nomsi"

#define PLAT_CONFIG_DEF() \
    vm0.simple_untyped24_pool = 6; \
    vm1.simple_untyped24_pool = 6; \
    vm0.kernel_cmdline = VM_GUEST_CMDLINE; \
    vm0.kernel_image = KERNEL_IMAGE; \
    vm0.kernel_relocs = KERNEL_IMAGE; \
    vm0.initrd_image = ROOTFS; \
    vm1.kernel_cmdline = VM_GUEST_CMDLINE; \
    vm1.kernel_image = KERNEL_IMAGE; \
    vm1.kernel_relocs = KERNEL_IMAGE; \
    vm1.initrd_image = ROOTFS; \
    vm0.iospace_domain = 0x0f; \
    vm1.iospace_domain = 0x10; \
    /**/

#define VM_GUEST_PASSTHROUGH_DEVICES_0() \
    /**/
#define VM_GUEST_PASSTHROUGH_DEVICES_1() \
    /**/

#define VM_INIT_COMPONENT() \
    component Init0 { \
        VM_INIT_DEF() \
        include "vmm/vchan_sharemem.h"; \
        uses VchanInterface vchan_con; \
        consumes VchanEvent vevent; \
        dataport vchan_headers_t share_mem; \
    } \
    component Init1 { \
        VM_INIT_DEF() \
        include "vmm/vchan_sharemem.h"; \
        uses VchanInterface vchan_con; \
        consumes VchanEvent vevent; \
        dataport vchan_headers_t share_mem; \
    } \
    /**/

#define VCHAN_COMPONENT_DEF() \
    static camkes_vchan_con_t vchan_camkes_component = { \
    .connect = &vchan_con_new_connection, \
    .disconnect = &vchan_con_rem_connection, \
    .get_buf = &vchan_con_get_buf, \
    .status = &vchan_con_status,\
    .alert_status = &vchan_con_alert_status, \
    .reg_callback = &vevent_reg_callback, \
    .alert = &vchan_con_ping, \
    .component_dom_num = 0, \
    }; \
    /**/

#define VCHAN_COMPONENT_INIT_MEM() \
    vchan_camkes_component.data_buf = (void *) share_mem; \
    init_camkes_vchan(&vchan_camkes_component); \
    /**/

#define VM_INIT_SOURCE_DEFS() \
    void __attribute__((weak)) vm0net_init(vmm_t *vmm) { assert(!"should not be called");} \


#define VM_ASYNC_DEVICE_BADGES_0() \
    /**/

#define VM_ASYNC_DEVICE_BADGES_1() \
    /**/

#define VM_ASYNC_DEVICE_BADGES_2() \
    /**/

#define VM_DEVICE_INIT_FN_0() \
    /**/

#define VM_DEVICE_INIT_FN_1() \
    /**/

#define VM_DEVICE_INIT_FN_2() \
    /**/

#define PLAT_COMPONENT_DEF() \
    component Vchan vchan_0; \
    component HelloWorld hello; \
    /**/

#define VM_ETHDRIVER_NUM_CLIENTS 0


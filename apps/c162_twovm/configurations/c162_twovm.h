/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

/* This entire configuration file is a bit of a mess. There is various duplication
 * of information, not to mention the fact that this is all done in macros.
 * Ideally most of the logic for the VMs configuration could be moved to
 * a template and camkes configurations could be used along with some slightly
 * nicer code generation.
 *
 * When modifying this it is important to remember that some macros just expand to
 * their contents, and some macros are expected to be used in boost list expansions
 * and care must be taken to get the syntax correct else difficult to debug build
 * errors will occur. For a boost list an empty list would be defined here as
   #define EMPTY_LIST()
 * Where as a list with one element is
   #define ONE_ELEMENT() \
        element \
   )
 * And multiple elements becomes
   #define MULTI_ELEMENT() \
        element1, \
        element2, \
        element3
   )
 * Importantly note that you cannot have a trailing comma on the last element
 * of a list
 *
 * Many options have _N varients. The N here refers to which VM number the option
 * applies to. This is the mechanism by which different VMs can be given different
 * resources */

#define VM_NUM_GUESTS 2

/* All our guests use the same kernel image, rootfs and cmdline */
#define C162_KERNEL_IMAGE "bzimage"
#define C162_ROOTFS "rootfs.cpio"
#define VM_GUEST_CMDLINE "console=ttyS0,115200 console=tty0 root=/dev/mem i8042.nokbd=y i8042.nomux=y i8042.noaux=y io_delay=udelay noisapnp pci=nomsi"

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

#define VM_ASYNC_DEVICE_BADGES_0() \
    /**/

#define VM_ASYNC_DEVICE_BADGES_1() \
    /**/

#define VM_INIT_SOURCE_DEFS() \
    /**/

#define VM_DEVICE_INIT_FN_0() \
    /**/

#define VM_DEVICE_INIT_FN_1() \
    /**/

/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#include <autoconf.h>
#include <boost/preprocessor/arithmetic.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/comparison.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/expand.hpp>
#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/control.hpp>

#define VM_NUM_GUESTS CONFIG_APP_CAMKES_VM_NUM_VM

#define VM_CONFIGURATION_HEADER() BOOST_PP_STRINGIZE(CAMKES_VM_CONFIG.h)
#include VM_CONFIGURATION_HEADER()

/* Now define a bunch of general definitions for constructing the VM */

#define CAT BOOST_PP_CAT

/* Each VM has 1 timer assigned to it, and the serial server also uses 1 */
#define VTIMER_FIRST 1
#define VTIMER_NUM   1
#define VTIMERNUM_I(t, n) BOOST_PP_INC(BOOST_PP_ADD(VTIMER_FIRST, BOOST_PP_ADD(t, BOOST_PP_MUL(VTIMER_NUM, n))))
#define VTIMERNUM(t, n) VTIMERNUM_I(t, n)
#define VTIMER(t, n) VTIMER_I(t, n)
#define VTIMER_I(t, n) BOOST_PP_CAT(timer, VTIMERNUM(t, n))

#define VM_NUM_TIMERS BOOST_PP_ADD(VTIMER_FIRST, BOOST_PP_MUL(VTIMER_NUM, VM_NUM_GUESTS))

#define VM_NUM_TIMER_CLIENTS VM_NUM_TIMERS

/* For all the async sources on the intready endpoint the high bit
 * is set to indicate that an async event occured, and the low bits
 * indicate which async events */

/* The timer completions are also on the interrupt manager badge */
#define VM_INIT_TIMER_BADGE 134217729 /* BIT(27) | BIT(0) */

#define VM_PIC_BADGE_IRQ_0 134217730 /* BIT(27) | BIT(1) */
#define VM_PIC_BADGE_IRQ_1 134217732 /* BIT(27) | BIT(2) */
#define VM_PIC_BADGE_IRQ_2 134217736 /* BIT(27) | BIT(3) */
#define VM_PIC_BADGE_IRQ_3 134217744 /* BIT(27) | BIT(4) */
#define VM_PIC_BADGE_IRQ_4 134217760 /* BIT(27) | BIT(5) */
#define VM_PIC_BADGE_IRQ_5 134217792 /* BIT(27) | BIT(6) */
#define VM_PIC_BADGE_IRQ_6 134217856 /* BIT(27) | BIT(7) */
#define VM_PIC_BADGE_IRQ_7 134217984 /* BIT(27) | BIT(8) */
#define VM_PIC_BADGE_IRQ_8 134218240 /* BIT(27) | BIT(9) */
#define VM_PIC_BADGE_IRQ_9 134218752 /* BIT(27) | BIT(10) */
#define VM_PIC_BADGE_IRQ_10 134219776 /* BIT(27) | BIT(11) */
#define VM_PIC_BADGE_IRQ_11 134221824 /* BIT(27) | BIT(12) */
#define VM_PIC_BADGE_IRQ_12 134225920 /* BIT(27) | BIT(13) */
#define VM_PIC_BADGE_IRQ_13 134234112 /* BIT(27) | BIT(14) */
#define VM_PIC_BADGE_IRQ_14 134250496 /* BIT(27) | BIT(15) */
#define VM_PIC_BADGE_IRQ_15 134283264 /* BIT(27) | BIT(16) */

#define VM_PIC_BADGE_SERIAL_HAS_DATA 134348800 /* BIT(27) | BIT(17) */

/* First available badge for user bits */
#define VM_FIRST_BADGE_BIT 18

/* Base definition of the Init component. This gets
 * extended in the per Vm configuration */
#define VM_INIT_DEF() \
    control; \
    uses PutChar putchar; \
    uses PutChar guest_putchar; \
    uses PCIConfig pci_config; \
    uses RTC system_rtc; \
    consumes HaveInterrupt intready; \
    uses Timer init_timer; \
    dataport Buf serial_buffer; \
    /**/

/* VM and per VM componenents */
#define VM_COMP_DEF(num) \
    component Init##num vm##num; \
    /**/

#define VM_CONNECT_DEF(num) \
    /* Connect all the components to the serial server */ \
    connection seL4RPCCall serial_vm##num(from vm##num.putchar, to serial.vm##num); \
    connection seL4RPCCall serial_guest_vm##num(from vm##num.guest_putchar, to serial.guest##num); \
    /* Connect the emulated serial input to the serial server */ \
    connection seL4ProdCon serial_input##num(from serial.CAT(guest##num,_buffer), to vm##num.serial_buffer); \
    connection seL4GlobalAsynch serial_input_ready##num(from serial.CAT(guest##num,_has_data), to vm##num.intready); \
    /* Temporarily connect the VM directly to the RTC */ \
    connection seL4RPCCall rtctest##num(from vm##num.system_rtc, to rtc.rtc); \
    /* Connect the VM to the timer server */ \
    connection seL4RPCCall CAT(pit##num,_timer)(from vm##num.init_timer, to time_server.the_timer); \
    connection seL4GlobalAsynch CAT(pit##num,_timer_interrupt)(from time_server.CAT(VTIMER(0, num),_complete), to vm##num.intready); \
    /* Connect config space to main VM */ \
    connection seL4RPCCall pciconfig##num(from vm##num.pci_config, to pci_config.pci_config); \
    /**/

#ifdef CONFIG_APP_CAMKES_VM_GUEST_DMA_ONE_TO_ONE
#define VM_MAYBE_ZONE_DMA(num) vm##num.mmio = "0x8000:0x97000:12";
#else
#define VM_MAYBE_ZONE_DMA(num)
#endif

/* If the platform configuration defined extra ram that we
 * generate the specifics of that generation for them */
#ifdef VM_CONFIGURATION_EXTRA_RAM
#define EXTRA_RAM_OUTPUT(a,b) BOOST_PP_STRINGIZE(a:b)
#define EXTRA_RAM_OUTPUT_(r, data, elem) vm##data.untyped_mmio = BOOST_PP_EXPAND(EXTRA_RAM_OUTPUT elem);
#define VM_MAYBE_EXTRA_RAM(num) BOOST_PP_LIST_FOR_EACH(EXTRA_RAM_OUTPUT_, num, BOOST_PP_TUPLE_TO_LIST(CAT(VM_CONFIGURATION_EXTRA_RAM_,num)()))
#else
#define VM_MAYBE_EXTRA_RAM(num)
#endif

/* Generate IOSpace capabilities if using the IOMMU */
#ifdef CONFIG_APP_CAMKES_VM_GUEST_DMA_IOMMU
#define IOSPACE_OUTPUT(r, data, elem) vm##data.iospace = elem;
#define VM_MAYBE_IOSPACE(num) BOOST_PP_LIST_FOR_EACH(IOSPACE_OUTPUT,num,BOOST_PP_TUPLE_TO_LIST(CAT(VM_CONFIGURATION_IOSPACES_,num)()))
#else
#define VM_MAYBE_IOSPACE(num)
#endif

#define MMIO_OUTPUT(r, data, elem) vm##data.mmio = elem;
#define VM_MMIO(num) BOOST_PP_LIST_FOR_EACH(MMIO_OUTPUT, num, BOOST_PP_TUPLE_TO_LIST(CAT(VM_CONFIGURATION_MMIO_, num)()))

#define IOPORT_OUTPUT(r, data, elem) vm##data.ioport = BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(0,elem):BOOST_PP_TUPLE_ELEM(1,elem));
#define VM_IOPORT(num) BOOST_PP_LIST_FOR_EACH(IOPORT_OUTPUT, num, BOOST_PP_TUPLE_TO_LIST(CAT(VM_CONFIGURATION_IOPORT_, num)()))

#define VM_IRQ_OUTPUT(r, data, elem) vm##data.irq = BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(0, elem));
#define VM_IRQS(num) BOOST_PP_LIST_FOR_EACH(VM_IRQ_OUTPUT, num, BOOST_PP_TUPLE_TO_LIST(CAT(VM_PASSTHROUGH_IRQ_, num)()))

#define VM_CONFIG_DEF(num) \
    vm##num.init_timer_attributes = BOOST_PP_STRINGIZE(VTIMERNUM(0, num)); \
    vm##num.intready_global_endpoint = BOOST_PP_STRINGIZE(vm##num); \
    serial.CAT(guest##num,_has_data_global_endpoint) = BOOST_PP_STRINGIZE(vm##num); \
    serial.CAT(guest##num,_has_data_badge) = BOOST_PP_STRINGIZE(VM_PIC_BADGE_SERIAL_HAS_DATA); \
    time_server.CAT(VTIMER(0, num),_complete_global_endpoint) = BOOST_PP_STRINGIZE(vm##num); \
    time_server.CAT(VTIMER(0, num),_complete_badge) = BOOST_PP_STRINGIZE(VM_INIT_TIMER_BADGE); \
    vm##num.cnode_size_bits = 21; \
    vm##num.simple = true; \
    VM_IRQS(num) \
    VM_MAYBE_ZONE_DMA(num) \
    VM_MAYBE_EXTRA_RAM(num) \
    VM_MAYBE_IOSPACE(num) \
    VM_MMIO(num) \
    VM_IOPORT(num) \
    /**/


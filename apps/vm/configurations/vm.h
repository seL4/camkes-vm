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

/* Include every possible configuration and assume they guard themselves
 * by insepcting the actual build configuration */
#define c162_threevm_testing 1
#include "c162_threevm_testing.h"
#define nohardware_onevm 2
#include "nohardware_onevm.h"

/* Now define a bunch of general definitions for constructing the VM */

#define CAT BOOST_PP_CAT

/* The timer layout is
 * timer 1 timer for serial server
 * blocks of 8 timers for each VM
 */
#define VTIMER_FIRST 2
#define VTIMER_NUM   8
#define VTIMERNUM_I(t, n) BOOST_PP_ADD(VTIMER_FIRST,BOOST_PP_ADD(t, BOOST_PP_MUL(VTIMER_NUM, n)))
#define VTIMERNUM(t, n) VTIMERNUM_I(t, n)
#define VTIMER(t, n) VTIMER_I(t, n)
#define VTIMER_I(t, n) BOOST_PP_CAT(timer, VTIMERNUM(t, n))
/* We start counting timers at 1 instead of 0 due to not wanting to use the 0
 * badge. Therefore we add 1 here and not VTIMER_FIRST */
#define VM_NUM_TIMERS BOOST_PP_ADD(1, BOOST_PP_MUL(VTIMER_NUM, VM_NUM_GUESTS))

/* The int manager async endpoint sets both the high and low bits of the badge
 * following standard protocal of high bit indicating some async message
 * low bit indicating which async event */
#define VM_INT_MAN_BADGE 134217729 /* BIT(27) | BIT(0) */

/* The PIT timer completion is also on the interrupt manager badge */
#define VM_PIT_TIMER_BADGE 134217730 /* BIT(27) | BIT(1) */

#define VM_PIC_BADGE_IRQ_0 134217732 /* BIT(27) | BIT(2) */
#define VM_PIC_BADGE_IRQ_1 134217736 /* BIT(27) | BIT(3) */
#define VM_PIC_BADGE_IRQ_2 134217744 /* BIT(27) | BIT(4) */
#define VM_PIC_BADGE_IRQ_3 134217760 /* BIT(27) | BIT(5) */
#define VM_PIC_BADGE_IRQ_4 134217792 /* BIT(27) | BIT(6) */
#define VM_PIC_BADGE_IRQ_5 134217856 /* BIT(27) | BIT(7) */
#define VM_PIC_BADGE_IRQ_6 134217984 /* BIT(27) | BIT(8) */
#define VM_PIC_BADGE_IRQ_7 134218240 /* BIT(27) | BIT(9) */
#define VM_PIC_BADGE_IRQ_8 134218752 /* BIT(27) | BIT(10) */
#define VM_PIC_BADGE_IRQ_9 134219776 /* BIT(27) | BIT(11) */
#define VM_PIC_BADGE_IRQ_10 134221824 /* BIT(27) | BIT(12) */
#define VM_PIC_BADGE_IRQ_11 134225920 /* BIT(27) | BIT(13) */
#define VM_PIC_BADGE_IRQ_12 134234112 /* BIT(27) | BIT(14) */
#define VM_PIC_BADGE_IRQ_13 134250496 /* BIT(27) | BIT(15) */
#define VM_PIC_BADGE_IRQ_14 134283264 /* BIT(27) | BIT(16) */
#define VM_PIC_BADGE_IRQ_15 134348800 /* BIT(27) | BIT(17) */

/* First available badge for user bits */
#define VM_FIRST_BADGE_BIT 18

/* VM and per VM componenents */
#define VM_COMP_DEF(num) \
    component Init vm##num; \
    component RTCEmulator RTCEmul##num; \
    component SerialEmulator SerialEmul##num; \
    /**/

#define VM_CONNECT_DEF(num) \
    /* Connect all the components to the serial server */ \
    connection seL4RPCCall serial_vm##num(from vm##num.putchar, to serial.vm##num); \
    connection seL4RPCCall serial_rtcemul##num(from RTCEmul##num.putchar, to serial.vm##num); \
    connection seL4RPCCall serial_serialemul##num(from SerialEmul##num.putchar, to serial.guest##num); \
    /* Connect the emulated serial input to the serial server */ \
    connection seL4RPCCall serial_input##num(from SerialEmul##num.getchar, to serial.CAT(guest##num,_input)); \
    connection seL4Asynch serial_input_ready##num(from serial.CAT(guest##num,_input_signal), to SerialEmul##num.getchar_signal); \
    /* Temporarily connect the VM directly to the RTC */ \
    connection seL4RPCCall rtctest##num(from vm##num.system_rtc, to rtc.rtc); \
    /* Connect the emulated serial to the VM */ \
    connection seL4RPCCall serial##num(from vm##num.serial, to SerialEmul##num.serialport); \
    /* Connect the emulated PIT to the timer server */ \
    connection seL4RPCCall CAT(pit##num,_timer)(from vm##num.pit_timer, to time_server.the_timer); \
    connection seL4AsynchBind CAT(pit##num,_timer_interrupt)(from time_server.CAT(VTIMER(0, num),_complete), to vm##num.intready); \
    /* Connect the emulated RTC to the timer server */ \
    connection seL4RPCCall CAT(rtc##num,_timer0)(from RTCEmul##num.rtc_periodic_timer, to time_server.the_timer); \
    connection seL4Asynch CAT(rtc##num,_timer0_interrupt)(from time_server.CAT(VTIMER(1, num),_complete), to RTCEmul##num.rtc_periodic_timer_interrupt); \
    connection seL4RPCCall CAT(rtc##num,_timer1)(from RTCEmul##num.rtc_coalesced_timer, to time_server.the_timer); \
    connection seL4Asynch CAT(rtc##num,_timer1_interrupt)(from time_server.CAT(VTIMER(2, num),_complete), to RTCEmul##num.rtc_coalesced_timer_interrupt); \
    connection seL4RPCCall CAT(rtc##num,_timer2)(from RTCEmul##num.rtc_second_timer, to time_server.the_timer); \
    connection seL4Asynch CAT(rtc##num,_timer2_interrupt)(from time_server.CAT(VTIMER(3, num),_complete), to RTCEmul##num.rtc_second_timer_interrupt); \
    connection seL4RPCCall CAT(rtc##num,_timer3)(from RTCEmul##num.rtc_second_timer2, to time_server.the_timer); \
    connection seL4Asynch CAT(rtc##num,_timer3_interrupt)(from time_server.CAT(VTIMER(4, num),_complete), to RTCEmul##num.rtc_second_timer2_interrupt); \
    /* Connect the emulated serial to the timer server */ \
    connection seL4RPCCall CAT(serial##num,_timer0)(from SerialEmul##num.fifo_timeout, to time_server.the_timer); \
    connection seL4Asynch CAT(serial##num,_timer0_interrupt)(from time_server.CAT(VTIMER(5,num),_complete), to SerialEmul##num.fifo_timeout_interrupt); \
    connection seL4RPCCall CAT(serial##num,_timer1)(from SerialEmul##num.transmit_timer, to time_server.the_timer); \
    connection seL4Asynch CAT(serial##num,_timer1_interrupt)(from time_server.CAT(VTIMER(6,num),_complete), to SerialEmul##num.transmit_timer_interrupt); \
    connection seL4RPCCall CAT(serial##num,_timer2)(from SerialEmul##num.modem_status_timer, to time_server.the_timer); \
    connection seL4Asynch CAT(serial##num,_timer2_interrupt)(from time_server.CAT(VTIMER(7,num),_complete), to SerialEmul##num.modem_status_timer_interrupt); \
    /* Connect the emulated RTC to the RTC component */ \
    connection seL4RPCCall cmosrtc_system##num(from RTCEmul##num.system_rtc, to rtc.rtc); \
    /* Connect the emulated RTC to the VM */ \
    connection seL4RPCCall cmosrtc##num(from vm##num.cmos, to RTCEmul##num.cmosport); \
    /* Connect config space to main VM */ \
    connection seL4RPCCall pciconfig##num(from vm##num.pci_config, to pci_config.pci_config); \
    /* Connect the emulated rtc to the PIC emulator */ \
    connection seL4RPCCall irq8_level_##num(from RTCEmul##num.rtc_irq, to vm##num.irq8_level); \
    /* Connect the emulated serial to the PIC emulator */ \
    connection seL4AsynchBind irq4_edge_##num(from SerialEmul##num.serial_edge_irq, to vm##num.intready); \
    /**/

#ifdef CONFIG_APP_CAMKES_VM_GUEST_DMA_ONE_TO_ONE
#define VM_MAYBE_ZONE_DMA(num) vm##num.mmio = "0x8000:0x97000:12";
#else
#define VM_MAYBE_ZONE_DMA(num)
#endif

/* If the platform configuration defined extra ram that we
 * generate the specifics of that generation for them */
#ifdef VM_CONFIGURATION_EXTRA_RAM
#define EXTRA_RAM_OUTPUT(a,b) BOOST_PP_STRINGIZE(a:b:12)
#define VM_MAYBE_EXTRA_RAM(num) vm##num.mmio = BOOST_PP_EXPAND(EXTRA_RAM_OUTPUT CAT(VM_CONFIGURATION_EXTRA_RAM_,num)()) ;
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
    vm##num.pit_timer_attributes = BOOST_PP_STRINGIZE(VTIMERNUM(0, num)); \
    RTCEmul##num.periodic_timer_attributes = BOOST_PP_STRINGIZE(VTIMERNUM(1, num)); \
    RTCEmul##num.coalesced_timer_attributes = BOOST_PP_STRINGIZE(VTIMERNUM(2, num)); \
    RTCEmul##num.second_timer_attributes = BOOST_PP_STRINGIZE(VTIMERNUM(3, num)); \
    RTCEmul##num.second_timer2_attributes = BOOST_PP_STRINGIZE(VTIMERNUM(4, num)); \
    SerialEmul##num.fifo_timeout_attributes = BOOST_PP_STRINGIZE(VTIMERNUM(5, num)); \
    SerialEmul##num.transmit_timer_attributes = BOOST_PP_STRINGIZE(VTIMERNUM(6, num)); \
    SerialEmul##num.modem_status_timer_attributes = BOOST_PP_STRINGIZE(VTIMERNUM(7, num)); \
    SerialEmul##num.serial_edge_irq_attributes = BOOST_PP_STRINGIZE(VM_PIC_BADGE_IRQ_4); \
    time_server.CAT(VTIMER(0, num),_complete_attributes) = BOOST_PP_STRINGIZE(VM_PIT_TIMER_BADGE); \
    vm##num.cnode_size_bits = 21; \
    vm##num.simple = true; \
    VM_IRQS(num) \
    VM_MAYBE_ZONE_DMA(num) \
    VM_MAYBE_EXTRA_RAM(num) \
    VM_MAYBE_IOSPACE(num) \
    VM_MMIO(num) \
    VM_IOPORT(num) \
    /**/


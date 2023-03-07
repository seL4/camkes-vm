/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#define _VAR_STRINGIZE(...) #__VA_ARGS__
#define VAR_STRINGIZE(...) _VAR_STRINGIZE(__VA_ARGS__)

/*
 * Calls macro f with each argument e.g. a,b,c,..
 */
#define __CALL1(f,a) f(a)
#define __CALL2(f,a,b) f(a) f(b)
#define __CALL3(f,a,b,c) f(a) f(b) f(c)
#define __CALL4(f,a,b,c,d) f(a) f(b) f(c) f(d)
#define __CALL5(f,a,b,c,d,e) f(a) f(b) f(c) f(d) f(e)
#define __CALL6(f,a,b,c,d,e,g) f(a) f(b) f(c) f(d) f(e) f(g)
#define __CALL7(f,a,b,c,d,e,g,h) f(a) f(b) f(c) f(d) f(e) f(g) f(h)
#define __CALL8(f,a,b,c,d,e,g,h,i) f(a) f(b) f(c) f(d) f(e) f(g) f(h) f(i)

#define __CALL_NARGS_X(a,b,c,d,e,f,g,h,n,...) n
#define __CALL_CONCAT_X(a,b) a##b
#define __CALL_CONCAT(a,b) __CALL_CONCAT_X(a,b)
#define __CALL_NARGS_FROM(...) __CALL_NARGS_X(__VA_ARGS__,8,7,6,5,4,3,2,1,)
#define __CALL_DISP_FROM(f, b,...) __CALL_CONCAT(b,__CALL_NARGS_FROM(__VA_ARGS__))(f, __VA_ARGS__)
#define __CALL(f, args...) __CALL_DISP_FROM(f, __CALL, args)

#if TK1DEVICEFWD
#define DEF_TK1DEVICEFWD \
    uses gen_fwd_inf uartfwd; \
    uses gen_fwd_inf clkcarfwd; \

#else
#define DEF_TK1DEVICEFWD
#endif

#if KERNELARMPLATFORM_EXYNOS5410
#define DEF_KERNELARMPLATFORM_EXYNOS5410 \
    uses pwm_inf pwm; \
    dataport Buf cmu_cpu; \
    dataport Buf cmu_top; \
    dataport Buf gpio_right; \
    dataport Buf cmu_core; \

#else
#define DEF_KERNELARMPLATFORM_EXYNOS5410
#endif

#define VM_INIT_DEF() \
    control; \
    uses FileServerInterface fs; \
    DEF_TK1DEVICEFWD \
    DEF_KERNELARMPLATFORM_EXYNOS5410 \
    maybe consumes restart restart_event; \
    has semaphore vm_sem; \
    maybe uses Batch batch; \
    maybe uses PutChar guest_putchar; \
    maybe uses GetChar serial_getchar; \
    maybe uses VirtQueueDev recv; \
    maybe uses VirtQueueDrv send; \
    consumes HaveNotification notification_ready; \
    emits HaveNotification notification_ready_connector; \
    maybe uses VMDTBPassthrough dtb_self; \
    provides VMDTBPassthrough dtb; \
    attribute int base_prio; \
    attribute int num_vcpus = 1; \
    attribute int num_extra_frame_caps; \
    attribute int extra_frame_map_address; \
    attribute { \
        string ram_base; \
        string ram_paddr_base; \
        string ram_size; \
        string dtb_addr; \
        string initrd_addr; \
        string kernel_entry_addr = "-1"; \
    } vm_address_config; \
    attribute { \
        string kernel_name = "linux"; \
        string dtb_name = "linux-dtb"; \
        string initrd_name = "linux-initrd"; \
        string kernel_bootcmdline = ""; \
        string kernel_stdout = ""; \
        string dtb_base_name = ""; \
        int provide_dtb = true; \
        int generate_dtb = false; \
        int provide_initrd = true; \
        int clean_cache = false; \
        int map_one_to_one = false; \
    } vm_image_config; \
    attribute { \
        string linux_ram_base; \
        string linux_ram_paddr_base; \
        string linux_ram_size; \
        string linux_ram_offset = "0"; /* obsolete */ \
        string dtb_addr; \
        string initrd_max_size = "-1"; /* obsolete */ \
        string initrd_addr; \
    } linux_address_config; \
    attribute { \
        string linux_name = "linux"; \
        string dtb_name = "linux-dtb"; \
        string initrd_name = "linux-initrd"; \
        string linux_bootcmdline = ""; \
        string linux_stdout = ""; \
        string dtb_base_name = ""; \
    } linux_image_config; \
    attribute { \
        int send_id; \
        int recv_id; \
    } serial_layout[] = []; \


#define VM_COMPONENT_DEF(num) \
    component VM vm##num; \

#define VM_COMPONENT_CONNECTIONS_DEF(num) \
    connection seL4RPCDataport fs##num(from vm##num.fs, to fserv.fs_ctrl); \
    connection seL4GlobalAsynch notify_ready_vm##num(from vm##num.notification_ready_connector, to vm##num.notification_ready); \

#define VM_GENERAL_COMPOSITION_DEF() \
    component FileServer fserv; \

#define VM_COMPOSITION_DEF(num) \
    VM_COMPONENT_DEF(num) \
    VM_COMPONENT_CONNECTIONS_DEF(num) \

#define VM_GENERAL_CONFIGURATION_DEF() \
    fserv.heap_size = 0x30000; \

#define VM_CONFIGURATION_DEF(num) \
    vm##num.fs_shmem_size = 0x100000; \
    vm##num.global_endpoint_base = 1 << 27; \
    vm##num.asid_pool = true; \
    vm##num.simple = true; \
    vm##num.base_prio = 100; \
    vm##num._priority = 101; \
    vm##num.sem_value = 0; \
    vm##num.heap_size = 0x300000;

#define VM_DOMAIN_CONFIGURATION_DEF(num, domain) \
    vm##num._domain = domain; \
    vm##num.tcb_pool = 2; \
    vm##num.tcb_pool_domains = [domain, domain];

#define VM_VIRTUAL_SERIAL_COMPONENTS_DEF() \
    component SerialServer serial; \
    component TimeServer time_server; \
    connection seL4TimeServer serialserver_timer(from serial.timeout, to time_server.the_timer); \

#define PER_VM_VIRTUAL_SERIAL_CONNECTIONS_DEF(num) \
    connection seL4SerialServer serial_vm##num(from vm##num.batch, to serial.processed_batch); \
    connection seL4SerialServer serial_input_vm##num(from vm##num.serial_getchar, to serial.getchar);

#define VM_VIRTUAL_SERIAL_COMPOSITION_DEF(vm_ids...) \
    VM_VIRTUAL_SERIAL_COMPONENTS_DEF() \
    __CALL(PER_VM_VIRTUAL_SERIAL_CONNECTIONS_DEF, vm_ids) \

#define VM_VIRTUAL_SERIAL_GENERAL_CONFIGURATION_DEF() \
    time_server.timers_per_client = 1; \
    time_server.priority = 255; \
    time_server.simple = true;

#define PER_VM_VIRTUAL_SERIAL_CONFIGURATION_DEF(num) \
    vm##num.serial_getchar_shmem_size = 0x1000; \
    vm##num.batch_shmem_size = 0x1000; \

#define VM_VIRTUAL_SERIAL_CONFIGURATION_DEF(vm_ids...) \
    VM_VIRTUAL_SERIAL_GENERAL_CONFIGURATION_DEF() \
    __CALL(PER_VM_VIRTUAL_SERIAL_CONFIGURATION_DEF, vm_ids) \


/*
 * Copyright 2018, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/*
 * This file contains macros to try and hide the boilerplate required to use crossvm connections.
 *
 * The following macros are private and used to implement the public macros at the end of this file.
 *
 * See the end of the file for a description of this _macro api_.
 */

/*
 * Calls macro f with each argument e.g. a,b,c,..
 */
#define __CALL1(f,a) f(a)
#define __CALL2(f,a,b) f(a) f(b)
#define __CALL3(f,a,b,c) f(a) f(b) f(c)
#define __CALL4(f,a,b,c,d) f(a) f(b) f(c) f(d)

/*
 * Calls macro f with p for each argument e.g. a,b,c,..
 */
#define __CALL_SINGLE1(f,p,a) f(p, a)
#define __CALL_SINGLE2(f,p,a,b) f(p, a) f(p, b)
#define __CALL_SINGLE3(f,p,a,b,c) f(p, a) f(p, b) f(p, c)
#define __CALL_SINGLE4(f,p,a,b,c,d) f(p, a) f(p, b) f(p, c) f(p, d)

/*
 * Calls macro f with p and an index for each argument e.g. a,b,c,..
 */
#define __CALL_NUM1(f,p,a) f(p, a, 0)
#define __CALL_NUM2(f,p,a,b) f(p, a, 0) f(p, b, 1)
#define __CALL_NUM3(f,p,a,b,c) f(p, a, 0) f(p, b, 1) f(p, c, 2)
#define __CALL_NUM4(f,p,a,b,c,d) f(p, a, 0) f(p, b, 1) f(p, c, 2) f(p, d, 3)

/*
 * Call macro utils
 */
#define __CALL_NARGS_X(a,b,c,d,e,f,g,h,n,...) n
#define __CALL_NARGS_FROM0(...) __CALL_NARGS_X(__VA_ARGS__,7,6,5,4,3,2,1,0,)
#define __CALL_NARGS_FROM1(...) __CALL_NARGS_X(__VA_ARGS__,8,7,6,5,4,3,2,1,)
#define __CALL_CONCAT_X(a,b) a##b
#define __CALL_CONCAT(a,b) __CALL_CONCAT_X(a,b)
#define __CALL_DISP_FROM0(f, b,...) __CALL_CONCAT(b,__CALL_NARGS_FROM0(__VA_ARGS__))(f, __VA_ARGS__)
#define __CALL_DISP_FROM1(f, b,...) __CALL_CONCAT(b,__CALL_NARGS_FROM1(__VA_ARGS__))(f, __VA_ARGS__)

#define __CALL(f, args...) __CALL_DISP_FROM1(f, __CALL, args)
#define __CALL_SINGLE(f, args...) __CALL_DISP_FROM0(f, __CALL_SINGLE, args)
#define __CALL_NUM(f, args...) __CALL_DISP_FROM0(f, __CALL_NUM, args)

/*
 * This defines the send and recv queues for a
 * virtio vswitch connection with a given vm ("vm_id")
 * Typically called in the Init definition
 */
#define __COMPONENT_DECL_ADD_INTERFACE_END(vm_id) \
    uses VirtQueueDrv ether_##vm_id##_send; \
    uses VirtQueueDev ether_##vm_id##_recv;

#define __COMPONENT_DECL(base_id, vm_id...) \
    __CALL(__COMPONENT_DECL_ADD_INTERFACE_END, vm_id)


/*
 * Expands the from ends of a connection between base_id vm and target vm
 */
#define __CONNECTION_ADD_INTERFACE_END(base_id, target_id) \
    from vm##base_id.ether_##target_id##_send, from vm##base_id.ether_##target_id##_recv,

#define __CONNECTION_PERVM_ADD_INTERFACES(base_id, vm_ids...) \
    __CALL_SINGLE(__CONNECTION_ADD_INTERFACE_END, base_id, vm_ids)

/**
 * Expands the config attributes of a VMs send and recv queue
 * Called once per connection per vm
 *_id is used for calling buffqueue_register
 *_attributes is used for shared memory connector.
 *  currently each chan gets its own shared memory region,
 *  keyed by base_id##target_id on send side and target_id##base_id on receive side
 *_global_endpoint refers to the notification object of the other vm
 *_badge refers to the badge that the other vm will receive on its notification object
 */
#define __CONFIG_PER_CONNECTION(base_id, target_id, idx) \
    vm##base_id.ether_##target_id##_send_id = idx *2; \
    vm##base_id.ether_##target_id##_send_attributes = VAR_STRINGIZE(base_id##target_id); \
    vm##base_id.ether_##target_id##_recv_id = idx *2 + 1; \
    vm##base_id.ether_##target_id##_recv_attributes = VAR_STRINGIZE(target_id##base_id); \
    vm##base_id.ether_##target_id##_send_shmem_size = 32768; \
    vm##base_id.ether_##target_id##_recv_shmem_size = 32768; \

// Add macaddress to virtqueue mapping. Called per connection per vm
#define __ADD_MACADDR_MAPPING(base_id, vm_id, idx) \
    {"mac_addr": VM##vm_id##_MACADDRESS, "send_id": idx*2, "recv_id":idx*2+1},

// Expand config section, called once per vm
#define __CONFIG_EXPAND_PERVM(base_id, vm_ids...) \
    __CALL_NUM(__CONFIG_PER_CONNECTION, base_id, vm_ids) \
    vm##base_id.vswitch_mac_address = VM##base_id##_MACADDRESS; \
    vm##base_id.vswitch_layout = [__CALL_NUM(__ADD_MACADDR_MAPPING, base_id, vm_ids)]; \


// Create a single virtqueue drv/dev pair.
#define __ADD_TOPOLOGY(base_id, target_id) \
    { "drv" : VAR_STRINGIZE(vm##base_id.ether_##target_id##_send) , "dev" : VAR_STRINGIZE(vm##target_id.ether_##base_id##_recv)},

// Connect the virtqueue ends together within the single connection instance.
#define __CONFIG_EXPAND_TOPOLOGY(base_id, vm_ids...) \
    __CALL_SINGLE(__ADD_TOPOLOGY, base_id, vm_ids)



/** PUBLIC FUNCTIONS
 * Types:
 * vm_id: A unique vm identifier. VMs should be numbered from 0.
 * f(base_id, vm_ids...) a varargs macro that is called with vm_id arguments.
 * topology(f): A macro that takes a function f, and calls it N times with each VM in the base_id param,
                and the vms it connects to in the following parameters.
                eg: #define topology_all(f) f(0,1,2) f(1,0,2) f(2,0,1)
                describes a topology where 3 vms are each connected to each other.
 * VM##vm_id##_##topology(f) A macro that calls f only for vm_id.
                eg: #define VM0_topology_all(f) f(0,1,2)
   Note it is possible/encouraged to define topology(f) in terms of VM##vm_id##_##topology(f)
   eg: #define topology_all(f) VM0_topology_all(f) VM1_topology_all(f) VM2_topology_all(f)
 * to_end: A component instance.interface that has been defined as: provides VirtQueue
 * VM##vm_id##_##MACADDRESS: The macaddress of vm_id. This will become the macaddress of the created VirtioNet device
 *
 * Functions:
 * VM_CONNECTION_COMPONENT_DEF(vm_id, topology): Defines interfaces for vm_id and topology.
 * VM_CONNECTION_CONNECT_VMS(to_end, topology): Creates connection between all instances of the given topology.
 * VM_CONNECTION_CONFIG(to_end, topology): Applies configuration for cross vm connection.
 * VM_CONNECTION_INIT_HANDLER(to_end, topology): Add init handler to vmm to create the cross_vm virtio_net device
 *
 */
#define VM_CONNECTION_COMPONENT_DEF(vm_id, topology) \
    VM##vm_id##_##topology(__COMPONENT_DECL)

/*
 * Defines a vswitch connection
 */
#define VM_CONNECTION_CONNECT_VMS(to_end, topology) \
    connection seL4VirtQueues topology##_conn(to to_end, topology(__CONNECTION_PERVM_ADD_INTERFACES));

#define VM_CONNECTION_CONFIG(to_end, topology) \
    topology(__CONFIG_EXPAND_PERVM) \
    to_end##_topology = [topology(__CONFIG_EXPAND_TOPOLOGY)];

#define __INIT_ADD_INTERFACE_END(base_id, target_id) \
    {"init":"make_virtio_net_vswitch_driver_dummy", "badge":"ether_" # target_id "_send_notification_badge()", "irq":"virtio_net_notify_vswitch"}, \
    {"init":"make_virtio_net_vswitch_driver_dummy", "badge":"ether_" # target_id "_recv_notification_badge()", "irq":"virtio_net_notify_vswitch"},

#define __CONNECTION_PERVM_ADD_INIT(base_id, vm_ids...) \
    __CALL_SINGLE(__INIT_ADD_INTERFACE_END, base_id, vm_ids)

#define VM_CONNECTION_INIT_HANDLER(vm_id, topology) \
    VM##vm_id##_##topology(__CONNECTION_PERVM_ADD_INIT) \
    {"init":"make_virtio_net_vswitch"}

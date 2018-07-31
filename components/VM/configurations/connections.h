/*
 * Copyright 2018, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
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
#define VM_CONNECTION_COMPONENT_DEF_PRIV(vm_id) \
    uses BuffQueueDrv ether_##vm_id##_send; \
    uses BuffQueueDev ether_##vm_id##_recv;

/*
 * Defines a vswitch connection
 */
#define VM_CONNECTION_CONNECTION_DEF_PRIV(connection_type, connection_name, connections...) \
    connection connection_type connection_name(connections);

#define VM_CONNECTION_COMPONENT_DEF(vm_id) \
    __CALL(VM_CONNECTION_COMPONENT_DEF_PRIV, vm_id)

/*
 * Expands the from ends of a connection between base_id vm and target vm
 */
#define EXPAND_NAME(base_id, target_id) \
    from vm##base_id.ether_##target_id##_send, from vm##base_id.ether_##target_id##_recv,

#define VM_CONNECTION_CONNECTION_EXPAND_VM(base_id, vm_ids...) \
    __CALL_SINGLE(EXPAND_NAME, base_id, vm_ids)

/*
*_id is used for calling buffqueue_register
 *_attributes is used for shared memory connector.
    currently each chan gets its own shared memory region,
    keyed by base_id##target_id on send side and target_id##base_id on receive side
 *_global_endpoint refers to the notification object of the other vm
 *_badge refers to the badge that the other vm will receive on its notification object
*/
#define BASE_BADGE 134217728
#define BADGE_NUMBER 1
#define CONNECTION_BADGE (BASE_BADGE | (1 << BADGE_NUMBER))

/*
 * Expands the config attributes of a VMs send and recv queue
 */
#define ADD_CONFIG(base_id, target_id, idx) \
    vm##base_id.ether_##target_id##_send_id = idx *2; \
    vm##base_id.ether_##target_id##_send_attributes = VAR_STRINGIZE(base_id##target_id); \
    vm##base_id.ether_##target_id##_send_global_endpoint = VAR_STRINGIZE(vm##target_id); \
    vm##base_id.ether_##target_id##_send_badge = CONNECTION_BADGE; \
    vm##base_id.ether_##target_id##_recv_id = idx *2 + 1; \
    vm##base_id.ether_##target_id##_recv_attributes = VAR_STRINGIZE(target_id##base_id); \
    vm##base_id.ether_##target_id##_recv_global_endpoint = VAR_STRINGIZE(vm##target_id); \
    vm##base_id.ether_##target_id##_recv_badge = CONNECTION_BADGE; \

#define VM_CONNECTION_CONFIG_EXPAND_VM(base_id, vm_ids...) \
    __CALL_NUM(ADD_CONFIG, base_id, vm_ids)

#define ADD_TOPOLOGY(base_id, target_id) \
    { "drv" : VAR_STRINGIZE(vm##base_id.ether_##target_id##_send) , "dev" : VAR_STRINGIZE(vm##target_id.ether_##base_id##_recv)},

#define VM_CONNECTION_CONFIG_TOPOLOGY_EXPAND_VM(base_id, vm_ids...) \
    __CALL_SINGLE(ADD_TOPOLOGY, base_id, vm_ids)

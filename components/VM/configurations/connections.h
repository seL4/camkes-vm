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

#define __call1(f,p,n) f(p, n)
#define __call2(f,p,n,a) f(p, n) f(p, a)
// #define __call3(f,p,n,a,b,c) f(n,__scc(a),__scc(b),__scc(c))
// #define __call4(f,p,n,a,b,c,d) f(n,__scc(a),__scc(b),__scc(c),__scc(d))
// #define __call5(f,p,n,a,b,c,d,e) f(n,__scc(a),__scc(b),__scc(c),__scc(d),__scc(e))
// #define __call6(f,p,n,a,b,c,d,e,f) f(n,__scc(a),__scc(b),__scc(c),__scc(d),__scc(e),__scc(f))
// #define __call7(f,p,n,a,b,c,d,e,f,g) f(n,__scc(a),__scc(b),__scc(c),__scc(d),__scc(e),__scc(f),__scc(g))

#define __call_num1(f,p,n) f(p, n, 0)
#define __call_num2(f,p,n,a) f(p,n, 0) f(p, a, 1)

#define __CALL_NARGS_X(a,b,c,d,e,f,g,h,n,...) n
#define __CALL_NARGS(...) __CALL_NARGS_X(__VA_ARGS__,7,6,5,4,3,2,1,0,)
#define __CALL_CONCAT_X(a,b) a##b
#define __CALL_CONCAT(a,b) __CALL_CONCAT_X(a,b)
#define __CALL_DISP(f, b,...) __CALL_CONCAT(b,__CALL_NARGS(__VA_ARGS__))(f, __VA_ARGS__)

#define __call(f, args...) __CALL_DISP(f, __call, args)
#define __call_num(f, args...) __CALL_DISP(f, __call_num, args)

#define VM_CONNECTION_COMPONENT_DEF_PRIV(prefix, vm_id) \
    uses BuffQueueDrv prefix##_##vm_id##_send; \
    uses BuffQueueDev prefix##_##vm_id##_recv;

#define VM_CONNECTION_CONNECTION_DEF_PRIV(connection_type, connection_name, connections...) \
    connection connection_type connection_name(connections);

#define VM_CONNECTION_COMPONENT_DEF(prefix, vm_id...) \
    __call(VM_CONNECTION_COMPONENT_DEF_PRIV, prefix, vm_id)

#define expand_name(base_id, target_id) \
    from vm##base_id.ether_##target_id##_send, from vm##base_id.ether_##target_id##_recv,

#define VM_CONNECTION_CONNECTION_EXPAND_VM(base_id, vm_ids...) \
    __call(expand_name, base_id, vm_ids)

// *_id is used for calling buffqueue_register
// *_attributes is used for shared memory connector.
//    currently each chan gets its own shared memory region,
//    keyed by base_id##target_id on send side and target_id##base_id on receive side
// *_global_endpoint refers to the notification object of the other vm
// *_badge refers to the badge that the other vm will receive on its notification object
#define BASE_BADGE 134217728
#define BADGE_NUMBER 1
#define CONNECTION_BADGE (BASE_BADGE | (1 << BADGE_NUMBER))
#define add_config(base_id, target_id, idx) \
    vm##base_id.ether_##target_id##_send_id = idx *2; \
    vm##base_id.ether_##target_id##_send_attributes = VAR_STRINGIZE(base_id##target_id); \
    vm##base_id.ether_##target_id##_send_global_endpoint = VAR_STRINGIZE(vm##target_id); \
    vm##base_id.ether_##target_id##_send_badge = CONNECTION_BADGE; \
    vm##base_id.ether_##target_id##_recv_id = idx *2 + 1; \
    vm##base_id.ether_##target_id##_recv_attributes = VAR_STRINGIZE(target_id##base_id); \
    vm##base_id.ether_##target_id##_recv_global_endpoint = VAR_STRINGIZE(vm##target_id); \
    vm##base_id.ether_##target_id##_recv_badge = CONNECTION_BADGE; \

#define VM_CONNECTION_CONFIG_EXPAND_VM(base_id, vm_ids...) \
    __call_num(add_config, base_id, vm_ids)

#define add_topology(base_id, target_id) \
    { "drv" : VAR_STRINGIZE(vm##base_id.ether_##target_id##_send) , "dev" : VAR_STRINGIZE(vm##target_id.ether_##base_id##_recv)},

#define VM_CONNECTION_CONFIG_TOPOLOGY_EXPAND_VM(base_id, vm_ids...) \
    __call(add_topology, base_id, vm_ids)

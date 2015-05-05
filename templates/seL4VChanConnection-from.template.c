/*#
 *# Copyright 2014, NICTA
 *#
 *# This software may be distributed and modified according to the terms of
 *# the BSD 2-Clause license. Note that NO WARRANTY is provided.
 *# See "LICENSE_BSD2.txt" for details.
 *#
 *# @TAG(NICTA_BSD)
 #*/

#include <vmm/vmm.h>
#include <vmm/vchan_component.h>

/*- include 'seL4RPCCall-from.template.c' -*/

/*- set domain = configuration[me.from_instance.name].get("%s_domain" % me.from_interface.name) -*/
/*- set shared_mem = configuration[me.from_instance.name].get("%s_dataport" % me.from_interface.name).strip('"') -*/

void */*? shared_mem ?*/;

void vchan_init_camkes(camkes_vchan_con_t vchan);

void /*? me.from_interface.name ?*/_init(vmm_t *vmm) {
    camkes_vchan_con_t vchan_camkes_component = {
        .connect = /*? me.from_interface.name ?*/_new_connection,
        .disconnect = /*? me.from_interface.name ?*/_rem_connection,
        .get_buf = /*? me.from_interface.name ?*/_get_buf,
        .status = /*? me.from_interface.name ?*/_status,
        .alert_status = /*? me.from_interface.name ?*/_alert_status,
        .alert = /*? me.from_interface.name ?*/_ping,
        .component_dom_num = /*? domain ?*/
    };
    vchan_camkes_component.data_buf = /*? shared_mem ?*/;
    vchan_init_camkes(vchan_camkes_component);
}

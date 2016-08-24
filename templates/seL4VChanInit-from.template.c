/*#
 *# Copyright 2016, NICTA
 *#
 *# This software may be distributed and modified according to the terms of
 *# the BSD 2-Clause license. Note that NO WARRANTY is provided.
 *# See "LICENSE_BSD2.txt" for details.
 *#
 *# @TAG(NICTA_BSD)
 #*/

#include <vmm/vmm.h>
#include <vmm/vchan_component.h>

/*- set domain = configuration[me.instance.name].get("%s_domain" % me.interface.name) -*/
/*- set shared_mem = configuration[me.instance.name].get("%s_dataport" % me.interface.name).strip('"') -*/
/*- set prefix = configuration[me.instance.name].get("%s_prefix" % me.interface.name).strip('"') -*/

void * /*? shared_mem ?*/;

int /*? prefix ?*/_new_connection(vchan_connect_t);
int /*? prefix ?*/_rem_connection(vchan_connect_t);
intptr_t /*? prefix ?*/_get_buf(vchan_ctrl_t, int);
int /*? prefix ?*/_status(vchan_ctrl_t);
int /*? prefix ?*/_alert_status(vchan_ctrl_t);
void /*? prefix ?*/_ping(void);

void vchan_init_camkes(camkes_vchan_con_t vchan);

void /*? me.interface.name ?*/_init(vmm_t *vmm) {
    camkes_vchan_con_t vchan_camkes_component = {
        .connect = /*? prefix ?*/_new_connection,
        .disconnect = /*? prefix ?*/_rem_connection,
        .get_buf = /*? prefix ?*/_get_buf,
        .status = /*? prefix ?*/_status,
        .alert_status = /*? prefix ?*/_alert_status,
        .alert = /*? prefix ?*/_ping,
        .component_dom_num = /*? domain ?*/
    };
    vchan_camkes_component.data_buf = /*? shared_mem ?*/;
    vchan_init_camkes(vchan_camkes_component);
}

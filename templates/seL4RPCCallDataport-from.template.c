/*#
 *# Copyright 2014, NICTA
 *#
 *# This software may be distributed and modified according to the terms of
 *# the BSD 2-Clause license. Note that NO WARRANTY is provided.
 *# See "LICENSE_BSD2.txt" for details.
 *#
 *# @TAG(NICTA_BSD)
 #*/

/*- set suffix = "_buf" -*/
/*- include 'seL4MultiSharedData-from.template.c' -*/

static void *get_buffer() {
    return (void*)/*? me.from_interface.name + suffix ?*/;
}

/*- set base = 'get_buffer()' -*/
/*- set userspace_ipc = True -*/
/*- set trust_partner = configuration[me.from_instance.name].get('trusted') == '"true"' -*/
/*- include 'rpc-connector-common-from.c' -*/

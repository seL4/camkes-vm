/*
 * Copyright 2018, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#include <sel4/sel4.h>
#include <camkes/dataport.h>
#include <camkes.h>
#include <camkes/virtqueue_template.h>
#include <virtqueue.h>
#include <ethdrivers/sel4vswitch.h>

/*- set suffix = "_buf" -*/
/*- include 'seL4MultiSharedData-from.template.c' -*/

/*- if len(me.parent.to_ends) != 1 -*/
    /*? raise(Exception('%s must only have 1 to end' % (me.parent.name))) ?*/
/*- endif -*/
/*- set to_end = me.parent.to_ends[0] -*/

/*- set interface_name =  me.interface.type.name -*/

/*- if interface_name == "VirtQueueDrv" -*/
    /*- set end_string = "drv" -*/
    /*- set other_end_string = "dev" -*/
/*- else -*/
    /*- set end_string = "dev" -*/
    /*- set other_end_string = "drv" -*/
/*- endif -*/

/*- set topology = configuration[to_end.instance.name].get("%s_topology" % to_end.interface.name) -*/
/*- set topology_entry = [] -*/

/*- for entry in topology -*/
    /*- if entry[end_string] == "%s.%s" % (me.instance.name, me.interface.name) -*/
        /*- do topology_entry.append(entry) -*/
    /*- endif -*/
/*- endfor -*/

/*- if len(topology_entry) != 1 -*/
    /*? raise(Exception('Could not find topology entry for: %s.%s' % (me.instance.name, me.interface.name))) ?*/
/*- endif -*/

/*- set other_interface_name = topology_entry[0][other_end_string] -*/

/*# We need to create a notification badge of their notificaion in our cspace #*/

//notifaction objects, dataport,
//func: return channel
/*- set is_reader = False -*/
/*- set instance = me.instance.name -*/
/*- set interface = me.interface.name -*/
/*- include 'global-endpoint.template.c' -*/
/*- set notification = pop('notification') -*/
seL4_CPtr /*? me.interface.name ?*/_notification(void) {
    return /*? notification ?*/;
}

void /*? me.interface.name ?*/_notify(void) {
    seL4_Signal(/*? notification ?*/);
}

/*- set interface_name =  me.interface.type.name -*/

/*- set queue_id = configuration[me.instance.name].get("%s_id" % me.interface.name) -*/
/*- if queue_id is none or not isinstance(queue_id, six.integer_types) -*/
  /*? raise(Exception('%s.%s_id must be set to a number' % (me.instance.name, me.interface.name))) ?*/
/*- endif -*/

//This is called by camkes runtime during init.
void /*? me.interface.name ?*/__init() {
/*- if interface_name == "VirtQueueDrv" -*/
    camkes_register_virtqueue_channel(/*? queue_id ?*/, /*? me.interface.name ?*/_get_size(), /*? me.interface.name ?*/_buf,  /*? me.interface.name ?*/_notify, VIRTQUEUE_DRIVER);
/*- else -*/
    camkes_register_virtqueue_channel(/*? queue_id ?*/, /*? me.interface.name ?*/_get_size(), /*? me.interface.name ?*/_buf,  /*? me.interface.name ?*/_notify, VIRTQUEUE_DEVICE);
/*- endif -*/
}

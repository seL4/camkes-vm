/*#
 *# Copyright 2019, DornerWorks
 *#
 *# SPDX-License-Identifier: BSD-2-Clause
 #*/

/*- include 'seL4RPCDataport-to.template.c' -*/

/*# Look through the composition and find all '-to' connectors that would be
 *# duplicates of this one
 #*/
/*- set badges = [] -*/
/*- set partitions = [] -*/
/*- for c in me.parent.from_ends -*/
    /*- set badge = configuration[c.instance.name].get('%s_attributes' % (c.interface.name)).strip('"') -*/
    /*- set partition = configuration[c.instance.name].get('%s_partitions' % (c.interface.name)) -*/
    /*- do badges.append(badge) -*/
    /*- do partitions.append( (badge, partition) ) -*/
/*- endfor -*/

/*- do badges.sort() -*/

void /*? me.interface.name ?*/_get_partitions(unsigned int badge, uint8_t *partition_list, uint8_t *num_partitions) {
    /*- if partitions -*/
        switch (badge) {
            /*- for badge,partition in partitions -*/
            case /*? badge ?*/: {
                uint8_t temp[] = {
                    /*- for num in partition -*/
                        /*? num ?*/,
                    /*- endfor -*/
                };
                memcpy(partition_list, temp, sizeof(temp));
                *num_partitions = sizeof(temp);
                break;
            }
            /*- endfor -*/
        }
    /*- endif -*/
}

/*
 * Copyright 2026, DornerWorks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/*- set alignment = 0x200000 -*/
/*- set reserved_frame_size = 0x1000 -*/

/*- set inst = configuration[me.name] -*/

/*- set kernel_size = inst.get('image_remap_kernel_size') -*/
/*- set rootfs_size = inst.get('image_remap_rootfs_size', 0) -*/
/*- set kernel_region_size = ((kernel_size + alignment - 1) // alignment) * alignment -*/
/*- set rootfs_region_size = ((rootfs_size + alignment - 1) // alignment) * alignment -*/
/*- set num_caps = (kernel_region_size + rootfs_region_size) // reserved_frame_size -*/

/*- set cnode_slots = 2 ** inst.get('cnode_size_bits') -*/
/*- set cnode_slot_start = inst.get('image_remap_cnode_slot_start') -*/

/*- if (cnode_slot_start + num_caps) >= cnode_slots -*/
    /*? raise(TemplateError('image_remap not enough slots in %s cnode' % me.name)) ?*/
/*- endif -*/

unsigned int vm_image_remap_get_cnode_slot_start(void)
{
    return /*? cnode_slot_start ?*/;
}

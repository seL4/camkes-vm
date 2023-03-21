/*
 * Copyright 2023, DornerWorks
 * Copyright 2023, Hensoldt Cyber
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <arm_vm/gen_config.h>

/*- set config = configuration[me.name] -*/
/*- if not config -*/
  /*? raise(Exception('Missing VM configuration')) ?*/
/*- endif -*/

/*- set vm_address_config = config.get('vm_address_config') -*/
/*- if not vm_address_config -*/
  /*? raise(Exception('Missing VM address configuration')) ?*/
/*- endif -*/

/*- set vm_image_config = config.get('vm_image_config') -*/
/*- if not vm_image_config -*/
  /*? raise(Exception('Missing VM image configuration')) ?*/
/*- endif -*/

const vm_config_t vm_config = {

    .ram = {
        .phys_base = /*? vm_address_config.get('ram_paddr_base') ?*/,
        .base = /*? vm_address_config.get('ram_base') ?*/,
        .size = /*? vm_address_config.get('ram_size') ?*/,
    },

    .dtb_addr = /*? vm_address_config.get('dtb_addr') ?*/,
    .initrd_addr = /*? vm_address_config.get('initrd_addr') ?*/,

/*- if vm_address_config.get('kernel_entry_addr') != '-1' -*/
    .entry_addr = /*? vm_address_config.get('kernel_entry_addr') ?*/,
/*- else -*/
    /*# For legacy compatibility, a fall back to the standard Linux entry exists. #*/
    /*- set is_64_bit = (8 == macros.get_word_size(options.architecture)) -*/
    /*- set entry_offset = 0x80000 if is_64_bit else 0x8000 -*/
#warning Using standard Linux entry point, please consider setting kernel_entry_addr explicitly.
    .entry_addr = /*? vm_address_config.get('ram_base') ?*/ + /*? '0x%x'%entry_offset ?*/,
/*- endif -*/

    .provide_initrd = /*? vm_image_config.get('provide_initrd') ?*/,
    .generate_dtb = /*? vm_image_config.get('generate_dtb') ?*/,
    .provide_dtb = /*? vm_image_config.get('provide_dtb') ?*/,
    .map_one_to_one = /*? vm_image_config.get('map_one_to_one') ?*/,
    .clean_cache = /*? vm_image_config.get('clean_cache') ?*/,

    .files = {
        .kernel = "/*? vm_image_config.get('kernel_name') ?*/",
        .initrd = "/*? vm_image_config.get('initrd_name', "") ?*/",
        .dtb = "/*? vm_image_config.get('dtb_name', "") ?*/",
        .dtb_base = "/*? vm_image_config.get('dtb_base_name', "") ?*/",
    },

    .kernel_bootcmdline = "/*? vm_image_config.get('kernel_bootcmdline', "") ?*/",
    .kernel_stdout = "/*? vm_image_config.get('kernel_stdout', "") ?*/",

};

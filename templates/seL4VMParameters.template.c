/*
 * Copyright 2023, DornerWorks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>

/*- set vm_address_config = configuration[me.name].get('vm_address_config') -*/
/*- set vm_image_config = configuration[me.name].get('vm_image_config') -*/
/*- set linux_address_config = configuration[me.name].get('linux_address_config') -*/
/*- set linux_image_config = configuration[me.name].get('linux_image_config') -*/

/*- if vm_address_config -*/

const unsigned long ram_base = /*? vm_address_config.get('ram_base') ?*/;
const unsigned long ram_paddr_base = /*? vm_address_config.get('ram_paddr_base') ?*/;
const unsigned long ram_size = /*? vm_address_config.get('ram_size') ?*/;
const unsigned long ram_offset = /*? vm_address_config.get('ram_offset') ?*/;
const unsigned long dtb_addr = /*? vm_address_config.get('dtb_addr') ?*/;
const unsigned long initrd_max_size = /*? vm_address_config.get('initrd_max_size') ?*/;
const unsigned long initrd_addr = /*? vm_address_config.get('initrd_addr') ?*/;

/*- else -*/

#warning You are using the deprecated linux_address_config structure. Please use the vm_address_config structure instead

const unsigned long ram_base = /*? linux_address_config.get('linux_ram_base') ?*/;
const unsigned long ram_paddr_base = /*? linux_address_config.get('linux_ram_paddr_base') ?*/;
const unsigned long ram_size = /*? linux_address_config.get('linux_ram_size') ?*/;
const unsigned long ram_offset = /*? linux_address_config.get('linux_ram_offset') ?*/;
const unsigned long dtb_addr = /*? linux_address_config.get('dtb_addr') ?*/;
const unsigned long initrd_max_size = /*? linux_address_config.get('initrd_max_size') ?*/;
const unsigned long initrd_addr = /*? linux_address_config.get('initrd_addr') ?*/;

/*- endif -*/

/*- if vm_image_config -*/

const char *_kernel_name = "/*? vm_image_config.get('kernel_name') ?*/";
const char *_dtb_name = "/*? vm_image_config.get('dtb_name', "") ?*/";
const char *_initrd_name = "/*? vm_image_config.get('initrd_name', "") ?*/";
const char *kernel_bootcmdline = "/*? vm_image_config.get('kernel_bootcmdline', "") ?*/";
const char *kernel_stdout = "/*? vm_image_config.get('kernel_stdout', "") ?*/";
const char *dtb_base_name = "/*? vm_image_config.get('dtb_base_name', "") ?*/";

/*- else -*/

#warning You are using the deprecated linux_image_config structure. Please use the vm_image_config structure instead

const char *_kernel_name = "/*? linux_image_config.get('linux_name') ?*/";
const char *_dtb_name = "/*? linux_image_config.get('dtb_name') ?*/";
const char *_initrd_name = "/*? linux_image_config.get('initrd_name') ?*/";
const char *kernel_bootcmdline = "/*? linux_image_config.get('linux_bootcmdline') ?*/";
const char *kernel_stdout = "/*? linux_image_config.get('linux_stdout') ?*/";
const char *dtb_base_name = "/*? linux_image_config.get('dtb_base_name') ?*/";

/*- endif -*/

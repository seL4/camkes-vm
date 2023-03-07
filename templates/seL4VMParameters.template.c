/*
 * Copyright 2023, DornerWorks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <arm_vm/gen_config.h>

/*- set vm_address_config = configuration[me.name].get('vm_address_config') -*/
/*- set vm_image_config = configuration[me.name].get('vm_image_config') -*/
/*- set linux_address_config = configuration[me.name].get('linux_address_config') -*/
/*- set linux_image_config = configuration[me.name].get('linux_image_config') -*/

/*# For legacy compatibility, a fall back to the standard Linux entry exists. #*/
/*- set is_64_bit = (8 == macros.get_word_size(options.architecture)) -*/
/*- set entry_offset = 0x80000 if is_64_bit else 0x8000 -*/

/*- if vm_address_config -*/

const unsigned long ram_base = /*? vm_address_config.get('ram_base') ?*/;
const unsigned long ram_paddr_base = /*? vm_address_config.get('ram_paddr_base') ?*/;
const unsigned long ram_size = /*? vm_address_config.get('ram_size') ?*/;
const unsigned long dtb_addr = /*? vm_address_config.get('dtb_addr') ?*/;
const unsigned long initrd_addr = /*? vm_address_config.get('initrd_addr') ?*/;

/*- if vm_address_config.get('kernel_entry_addr') != '-1' -*/
const unsigned long entry_addr = /*? vm_address_config.get('kernel_entry_addr') ?*/;
/*- else -*/
#warning Using standard Linux entry point, please consider setting kernel_entry_addr explicitly.
const unsigned long entry_addr = ram_base + /*? '0x%x'%entry_offset ?*/;
/*- endif -*/

/*- else -*/

#warning You are using the deprecated linux_address_config structure. Please use the vm_address_config structure instead

const unsigned long ram_base = /*? linux_address_config.get('linux_ram_base') ?*/;
const unsigned long ram_paddr_base = /*? linux_address_config.get('linux_ram_paddr_base') ?*/;
const unsigned long ram_size = /*? linux_address_config.get('linux_ram_size') ?*/;
const unsigned long dtb_addr = /*? linux_address_config.get('dtb_addr') ?*/;
const unsigned long initrd_addr = /*? linux_address_config.get('initrd_addr') ?*/;
/* Use standard Linux entry point. */
const unsigned long entry_addr = ram_base + /*? '0x%x'%entry_offset ?*/;

/*- endif -*/

/*- if vm_image_config -*/

const char *_kernel_name = "/*? vm_image_config.get('kernel_name') ?*/";
const char *_dtb_name = "/*? vm_image_config.get('dtb_name', "") ?*/";
const char *_initrd_name = "/*? vm_image_config.get('initrd_name', "") ?*/";
const char *kernel_bootcmdline = "/*? vm_image_config.get('kernel_bootcmdline', "") ?*/";
const char *kernel_stdout = "/*? vm_image_config.get('kernel_stdout', "") ?*/";
const char *dtb_base_name = "/*? vm_image_config.get('dtb_base_name', "") ?*/";

const int provide_initrd = /*? vm_image_config.get('provide_initrd') ?*/;
const int generate_dtb = /*? vm_image_config.get('generate_dtb') ?*/;
const int provide_dtb = /*? vm_image_config.get('provide_dtb') ?*/;
const int map_one_to_one = /*? vm_image_config.get('map_one_to_one') ?*/;
const int clean_cache = /*? vm_image_config.get('clean_cache') ?*/;

/*- else -*/

#warning "You are using the deprecated linux_image_config structure. The provide_initrd, generate_dtb, provide_dtb, \
    map_one_to_one, and clean_cache flags are set to replicate previous behavior, which may cause your configuration to break. \
    Please use the vm_image_config structure instead."

const char *_kernel_name = "/*? linux_image_config.get('linux_name') ?*/";
const char *_dtb_name = "/*? linux_image_config.get('dtb_name') ?*/";
const char *_initrd_name = "/*? linux_image_config.get('initrd_name') ?*/";
const char *kernel_bootcmdline = "/*? linux_image_config.get('linux_bootcmdline') ?*/";
const char *kernel_stdout = "/*? linux_image_config.get('linux_stdout') ?*/";
const char *dtb_base_name = "/*? linux_image_config.get('dtb_base_name') ?*/";

#ifdef CONFIG_VM_INITRD_FILE
#warning VmInitRdFile is a deprecated setting. Please remove and use vm_image_config.provide_initrd
const int provide_initrd = 1;
#else
const int provide_initrd = 0;
#endif

#ifdef CONFIG_VM_DTB_FILE
#warning VmDtbFile is a deprecated setting. Please remove and use vm_image_config.provide_dtb
const int provide_dtb = 1;
const int generate_dtb = 0;
#else
const int provide_dtb = 0;
const int generate_dtb = 1;
#endif

#if defined(CONFIG_PLAT_EXYNOS5) || defined(CONFIG_PLAT_QEMU_ARM_VIRT) || defined(CONFIG_PLAT_TX2)
const int map_one_to_one = 1;
#else
const int map_one_to_one = 0;
#endif

#if defined(CONFIG_PLAT_TX1) || defined(CONFIG_PLAT_TX2)
const int clean_cache = 1;
#else
const int clean_cache = 0;
#endif

/*- endif -*/

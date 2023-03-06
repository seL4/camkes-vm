<!--
     Copyright 2023, DornerWorks
     SPDX-License-Identifier: CC-BY-SA-4.0
-->

# VM Memory Configuration

This document overviews how the seL4 VMM provides guests with the a block of memory.

## Basic Configuration

In the platform specific `devices.camkes` file, there are macros that define the VM's RAM region:

```
#define VM_RAM_BASE 0x40000000
#define VM_RAM_SIZE 0x20000000

vm0.vm_address_config = {
    "ram_base" : VAR_STRINGIZE(VM_RAM_BASE),
    "ram_paddr_base" : VAR_STRINGIZE(VM_RAM_BASE),
    "ram_size" : VAR_STRINGIZE(VM_RAM_SIZE),
};

vm0.simple_untyped24_pool = 33;

```

This configuration lets VM0 see 512MB at 0x40000000. When running as a hypervisor, seL4 uses stage 2
mappings to map an Intermediate Physical Address (IPA) to a Physical Address (PA). This means that
0x40000000 is the IPA that the guest sees, and the associated PA could be any available page in the
system. The pages backing the guest mappings are provided by the `simple_untyped24_pool`. In order
to map 512MB, 32 untyped objects of size 24 are required. The additional object is provided for
overhead operations.

### What does this mean?

This default method is the most secure way of configuring the system. You can give two VMs the exact
same `RAM_BASE` and `RAM_SIZE`, and the MMU will protect against the VMs accessing the others
memory.

The only case where this fails is when you want to give a VM a DMA-backed hardware device. In this
specific case, you must provide the guest with the appropriate SMMU capabilities such that the SMMU
will back the VM's DMA transactions.

However, there are platforms that don't have a (supported) SMMU. What then?

## 1:1 Mapping Configuration

DMA transactions will still work in the guest if the IPA->PA mappings are 1:1. In the previous
example, a VM at 512MB at 0x40000000. In order for DMA to work, the VMM must use the page
capabilities for the specified address range to back the guests mappings. For example:

```
#define VM_RAM_BASE 0x40000000
#define VM_RAM_SIZE 0x20000000

vm0.vm_address_config = {
    "ram_base" : VAR_STRINGIZE(VM_RAM_BASE),
    "ram_paddr_base" : VAR_STRINGIZE(VM_RAM_BASE),
    "ram_size" : VAR_STRINGIZE(VM_RAM_SIZE),
};


vm0.vm_image_config = {
    "map_one_to_one" : true,
};

vm0.untyped_mmios = [
    "0x40000000:29", // Linux kernel memory regions
];

vm0.simple_untyped24_pool = 1;

```

In this example, the capability to the guest memory region is provided via the `untyped_mmios`
configuration option. The VMM knows to map 1:1 by the 'vm_image_config.map_one_to_one flag'. Also,
the `simple_untyped24_pool` was reduced to 1 because the VMM no longer needs the generic untyped
objects to back the guest's mappings.

### How to get the Capability List

Use the `CapDLLoaderPrintUntypeds` CMake option to have the capDL loader print out the untyped
objects list. Then place the objects associated with the guest memory region into the
`untyped_mmios` pool.

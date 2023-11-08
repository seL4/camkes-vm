/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <assert.h>
#include <camkes/error.h>
#include <sel4/sel4.h>
#include <stdint.h>
#include <stdio.h>

int ram_num_untypeds(void);
int ram_get_untyped(int ut, uintptr_t *paddr, int *size_bits, seL4_CPtr *cap);


int exclude_paddr_num_regions(void);
void exclude_paddr_get_region(int region_num, uintptr_t *paddr, size_t *bytes);

int init_cons_num_connections(void);
uintptr_t init_cons_init_function(int con);
int init_cons_has_interrupt(int con, uintptr_t *badge, uintptr_t *fun);


int ioports_num_pci_ioports(void);
int ioports_num_nonpci_ioports(void);
const char *ioports_get_pci_ioport(int num, seL4_CPtr *cap, uint16_t *start, uint16_t *end);
const char *ioports_get_nonpci_ioport(int num, seL4_CPtr *cap, uint16_t *start, uint16_t *end);
seL4_CPtr ioports_get_ioport(uint16_t start, uint16_t end);

int pci_devices_num_devices(void);
const char *pci_devices_get_device(int pci_dev, uint8_t *bus, uint8_t *dev, uint8_t *fun, seL4_CPtr *iospace_cap);
int pci_devices_num_device_mem(int pci_dev);
const char *pci_devices_get_device_irq(int pci_dev);
int pci_devices_get_device_mem(int pci_dev, int mem, uintptr_t *paddr, size_t *size, int *page_bits);
seL4_CPtr pci_devices_get_device_mem_frame(uintptr_t paddr);

int guest_mappings_num_guestmaps();
int guest_mappings_get_guest_map(int num, uint64_t *frame, uint64_t *size);
seL4_CPtr guest_mappings_get_mapping_mem_frame(uintptr_t paddr);

int irqs_ioapic_num_irqs(void);
int irqs_msi_num_irqs(void);
const char * irqs_ioapic_get_irq(int i, seL4_CPtr *irq_handler, uint8_t *ioapic, uint8_t *source, int *level_trig, int *active_low, uint8_t *dest);
const char * irqs_msi_get_irq(int i, seL4_CPtr *irq_handler, uint8_t* irq, uint8_t *dest);

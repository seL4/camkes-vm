/*#
 *# Copyright 2014, NICTA
 *#
 *# This software may be distributed and modified according to the terms of
 *# the BSD 2-Clause license. Note that NO WARRANTY is provided.
 *# See "LICENSE_BSD2.txt" for details.
 *#
 *# @TAG(NICTA_BSD)
 #*/

#include <assert.h>
#include <camkes/error.h>
#include <stdio.h>
#include <stdint.h>
#include <sel4/sel4.h>

/*? macros.show_includes(me.from_instance.type.includes) ?*/

/*- set vm_name = me.to_instance.name[0:me.to_instance.name.rfind('_config')] -*/
/*- set iospace_domain = configuration[vm_name].get('iospace_domain') -*/

/*- set config_pci = configuration[me.to_instance.name].get(me.to_interface.name) -*/
/*- set has_iospace = configuration[me.to_instance.name].get("%s_iospace" % me.to_interface.name) -*/
/*- set devices = [] -*/
/*- set device_mem = [] -*/

/*- set bits_to_frame_type = { 12:seL4_FrameObject, 20:seL4_ARM_SectionObject, 21:seL4_ARM_SectionObject } -*/

/*- if config_pci is not none -*/
    /*- for device in config_pci -*/
        /*- set bus = device['bus'] -*/
        /*- set dev = device['dev'] -*/
        /*- set fun = device['fun'] -*/
        /*- set pciid = bus * 256 + dev * 8 + fun -*/
        /*- set devid = iospace_domain * 65536 + pciid -*/
        /*- set iospace_cap = [] -*/
        /*- if has_iospace -*/
            /*- set cap = alloc('iospace_%d' % devid, seL4_IA32_IOSpace, domainID = iospace_domain, bus = bus, dev = dev, fun = fun) -*/
            /*- do iospace_cap.append(cap) -*/
        /*- endif -*/
        /*- set mem_ranges = [] -*/
        /*- for mem in device['memory'] -*/
            /*- for frame_offset in range(0, mem['size'], 2 ** mem['page_bits']) -*/
                /*- set frame = mem['paddr'] + frame_offset -*/
                /*- set object = alloc_obj('mmio_frame_%d' % frame, bits_to_frame_type[mem['page_bits']], paddr=frame) -*/
                /*- set cap = alloc_cap('mmio_frame_%d' % frame, object, read=true, write=true) -*/
                /*- do device_mem.append( (frame, cap) ) -*/
            /*- endfor -*/
            /*- do mem_ranges.append( (mem['paddr'], mem['size'], mem['page_bits']) ) -*/
        /*- endfor -*/
        /*- do devices.append( (device['name'], bus, dev, fun, iospace_cap, device['irq'], mem_ranges) ) -*/
    /*- endfor -*/
/*- endif -*/

int /*? me.from_interface.name ?*/_num_devices() {
    return /*? len(devices) ?*/;
}

const char */*? me.from_interface.name ?*/_get_device(int pci_dev, uint8_t *bus, uint8_t *dev, uint8_t *fun, seL4_CPtr *iospace_cap) {
    /*- if len(devices) == 0 -*/
        return NULL;
    /*- else -*/
        switch(pci_dev) {
            /*- for name, bus, dev, fun, iospace_cap, irq, mem in devices -*/
                case /*? loop.index0 ?*/:
                    *bus = /*? bus ?*/;
                    *dev = /*? dev ?*/;
                    *fun = /*? fun ?*/;
                    /*- if len(iospace_cap) == 0 -*/
                        *iospace_cap = 0;
                    /*- else -*/
                        *iospace_cap = /*? iospace_cap[0] ?*/;
                    /*- endif -*/
                    return "/*? name ?*/";
            /*- endfor -*/
            default:
                return NULL;
        }
    /*- endif -*/
}

int /*? me.from_interface.name ?*/_num_device_mem(int pci_dev) {
    /*- if len(devices) == 0 -*/
        return -1;
    /*- else -*/
        switch(pci_dev) {
            /*- for name, bus, dev, fun, iospace_cap, irq, mem in devices -*/
                case /*? loop.index0 ?*/:
                    return /*? len(mem) ?*/;
            /*- endfor -*/
            default:
                return -1;
        }
    /*- endif -*/
}

const char /*? me.from_interface.name ?*/_get_device_irq(int pci_dev) {
    /*- if len(devices) == 0 -*/
        return NULL;
    /*- else -*/
        switch(pci_dev) {
            /*- for name, bus, dev, fun, iospace_cap, irq, mem in devices -*/
                case /*? loop.index0 ?*/:
                    return "/*? irq ?*/";
            /*- endfor -*/
            default:
                return NULL;
        }
    /*- endif -*/
}

int /*? me.from_interface.name ?*/_get_device_mem(int pci_dev, int mem, uintptr_t *paddr, size_t *size, int *page_bits) {
    /*- if len(devices) == 0 -*/
        return -1;
    /*- else -*/
        switch(pci_dev) {
            /*- for name, bus, dev, fun, iospace_cap, irq, mem in devices -*/
                case /*? loop.index0 ?*/:
                    /*- if len(mem) == 0 -*/
                        return -1;
                    /*- else -*/
                        switch(mem) {
                            /*- for paddr, size, page_bits in mem -*/
                                case /*? loop.index0 ?*/:
                                    *paddr = /*? paddr ?*/;
                                    *size = /*? size ?*/;
                                    *page_bits = /*? page_bits ?*/;
                                    return 0;
                            /*- endfor -*/
                            default:
                                return -1;
                            }
                    /*- endif -*/
            /*- endfor -*/
            default:
                return -1;
        }
    /*- endif -*/
}

seL4_CPtr /*? me.from_interface.name ?*/_get_device_mem_frame(uintptr_t paddr) {
    /*- if len(device_mem) == 0 -*/
        return 0;
    /*- else -*/
        switch(paddr) {
            /*- for paddr, cap in device_mem -*/
                case /*? paddr ?*/:
                    return /*? cap ?*/;
            /*- endfor -*/
        default:
            return 0;
        }
    /*- endif -*/
}

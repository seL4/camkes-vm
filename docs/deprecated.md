<!--
     Copyright 2023, Technology Innovation Institute

     SPDX-License-Identifier: CC-BY-SA-4.0
-->

# Deprecated build configuration options

## VmInitRdFile

This option specified whether or not the guests were supplied the initial RAM
disk (initrd). The problem was that this applied to all guests at once. It was
not possible to supply some VMs initrd and leave it out for the other VMs.

Now initrd support is per-VM setting and is configured from within the CAmkES
configuration file, for example:

<pre>
assembly {
	configuration {
        vm0.linux_address_config = {
            "initrd_max_size" : VAR_STRINGIZE(VM_INITRD_MAX_SIZE),
            "initrd_addr" : VAR_STRINGIZE(VM_INITRD_ADDR)
        };
        vm0.linux_image_config = {
            "initrd_name" : "linux-initrd",
        };
    };
};
</pre>

It is mandatory to either specify all three attributes or to leave all three
unspecified. Otherwise a runtime error is raised.

### Changes required to existing code

Before the change ```initrd_name``` defaulted to ```linux-initrd```. Now it
defaults to an empty string, meaning the default is to not use initrd for the
given guest. Existing code must be modified to explicitly specify
```initrd_name``` as ```linux-initrd```, as shown in the snippet above. To make
sure everyone performs the needed modification, having the deprecated
```VmInitRdFile``` CMake variable turned on triggers an error. Typically in
```settings.cmake``` contains a line like this:

<pre>
set(VmInitRdFile ON CACHE BOOL "" FORCE)
</pre>

After making sure the ```initrd_name``` modification is in place, the above
line must be deleted from ```settings.cmake```.

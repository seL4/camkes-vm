<!--
     Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)

     SPDX-License-Identifier: CC-BY-SA-4.0
-->
camkes-vm
=========

The CAmkES VMM is actively targeted to run on the C162 platform from Aitech.
This repo provides various CAmkES components, templates, interfaces and libraries that can be used to create a CAmkES VMM application. This is short description of how to use and build with this project.

Build dependencies
------------------

To ensure you have the necessary dependencies for building the CAmkES VM project please refer to the [Host Dependencies page](https://docs.sel4.systems/HostDependencies.html).

## CMake
To build an application with this project we use CMake. This repo provides a series of CMake helper functions (found in `camkes_vm_helpers.cmake`) to assit you with defining your CAmkES VM application. Importing this file into your applications CMake configuration gives you access to the following helper functions:

- `DeclareCAmkESVM(init_component [SOURCES INCLUDES LIBS LD_FLAGS C_FLAGS])`: Function for declaring a CAmkESVM. This is called for each Init component in the defined in the applications `.camkes` file. The user can also pass in extra compilation sources, includes, libs and flags to be compiled with the component through additional arguments (`SOURCES`, `INCLUDES`, `LIBS`, `LD_FLAGS` and `C_FLAGS`)
- `DeclareCAmkESVMRootServer(camkes_config)`: Declares the CAmkESVM root server. This function takes the applications `.camkes` file as an argument (`camkes_config`).
- `AddToFileServer(filename_pref file_dest [DEPENDS])`: Function for adding a file/image to the vm file server. The caller specifies the name of they wish to refer to the image in the FileServer through the `filename_pref` parameter. `file_dest` is the file system location of the image the caller is adding. Additional dependencies to the image can be passed through the optional `DEPENDS` parameter.
- `DecompressLinuxKernel(decompress_target decompressed_kernel_image compressed_kernel_image [DEPENDS])`: Function for decompressing/extracting a vmlinux file from a given kernel image. The caller specifies a target name (`decompress_target`) for decompressing the kernel, the kernel image to decompress (`compressed_kernel_image`) and additional dependencies to the compressed image through the optional `DEPENDS` parameter. The location of the decompressed image is populated in the `decompressed_kernel_image` parameter passed by the caller.

## Items

This repo contains a series of CAmkES components, templates, libraries and images necessary to develop a CAmkES VM application. This project does not contain any CAmkES VM applications to build however. A set of example CAmkES VM applications can be found in the [camkes-vm-examples](https://github.com/seL4/camkes-vm-examples/blob/master/README.md) repo. This is a small description of the various items provided in this repository:

### CAmkES Components
This repo provides a series of useful CAmkES components that can be used to develop your VM platform, in addition to some demonstration applications. These components include:

- `Init`
- `Ethdriver`
- `StringReverse`
- `Echo`
- `Firewall`
- `StringReverse`
- `UDPServer`

### Linux Images
This repo provides Guest Linux VM images you can use for your CAmkES VM application, located in the `linux` directory. We provide a ready-to-use Rootfs (`linux/rootfs.cpio`) and Kernel image (`linux/bzimage`) built from the [buildroot](https://buildroot.org/) project.

Additionally you can develop your own applications and linux kernel modules to add to the Linux `rootfs.cpio` archive. After making changes and adding additional packages and modules you can run the provided `build-rootfs` tool to generate a new rootfs image.

### Libraries

#### libcrossvm

`libcrossvm` makes it possible connect processes in the guest linux to a regular CAmkES component. This is achieved through making dataports and event interfaces available to the guest VM. To utilise the CAmkES Cross VM connector implementation we provide the crossvm library.

To enable a guest Linux with crossvm functionality we can link the library to the desired VMs CMake declaration, for example:
    ```
	DeclareCAmkESVM(Init0
        EXTRA_LIBS crossvm
    )
    ```

Serial
------

The hardware COM1 is multiplexed to each VM such that output for each guest
and VM appears over COM1, and input can be directed at any of the guests. The
output of each guest and VM is colour coded.

By default input is sent to vm0, to switch input to a different VM use
@N where N is 0-9 (for VMs 0 to 9) or :,; for VMs 10 and 11
respectively. The escape character, tilde, is only recognized
following a newline. To get a list of all supported escapes, use `@?`.

It is also possible to send input to multiple VMs at the same time.
This mode is experimental. To set up multi-guest input, use `@m`
followed by one or more numbers or characters (as above), then press
Return to apply.

Multi-guest input mode also enables output coalescing, in order to
allow convenient use of the shell and other interactive terminal
applications (including text editors, if the editor state is exactly
the same in all guests). Coalesced output is drawn in white. Please
note that coalescing is experimental and best-effort; some
circumstances will result in coalescing failure, which is shown as
multiple colour-coded copies of the same output. In order to
ameliorate this, it is recommended to use a terminal multiplexer such
as `screen` inside the VMs, providing a redraw function.

Some debug features are also built in (activated via the `@d` escape);
see the online help (`@?`) and [the source code](components/Init/src/serial.c)
for details.

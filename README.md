camkes-vm
=========

The CAmkES VMM is actively targeted to run on the C162 platform from Aitech.
This is short description of how to build and run this configuration

Build dependencies
------------------

GHC and packages MissingH, data-ordlist and split (installable from cabal)
For example:

    apt-get install ghc
    apt-get install cabal-install
    cabal update
    cabal install MissingH
    cabal install data-ordlist
    cabal install split

Cabal packages get installed under the current user, so each user that wants to
build the VM must run the cabal steps

Python and packages jinja2, ply, pyelftools
Can be installed via pip, for example:

    apt-get install python-pip
    pip install pyelftools
    pip install ply
    pip install jinja2

If building on a 64bit system ensure 32bit compiler tools are installed, mainly:

    apt-get install lib32gcc1

And the correct version of multilib for your gcc, for example:

    apt-get install gcc-multilib

Building
--------

To build do

    make clean
    make c162_twovm_defconfig
    make silentoldconfig
    make

Then boot images/kernel-ia32-pc99 and images/capdl-loader-experimental-image-ia32-pc99
with the multiboot boot loader of your choice

For testing the C162 was configured to PXEboot (using firmware and instructions apc)
pxelinux, which then used the mboot.c32 module to load the seL4 kernel and user image

Configuration
-------------

Three Linux's are configured to start. Each uses the same kernel and initrd, but
has access to different hardware.

* vm0 - usb
* vm1 - com2, com4, canbus and SMBus (I2C)

The to use is chosen by selecting the desired application in

    make menuconfig

Currently the two valid choices are

* VMM for C162 with two guests
* VMM for Optiplex 9020 machine with one guest

Descriptions in the applications CAmkES files and Makefiles determine the full
configuration. This includes how many VMs, what hardware each VM has access to,
the name of Linux kernel and initrd to use etc. It also defines any additional
components and connections.

Serial
------

The hardware COM1 is multiplexed to each VM such that output for each guest
and VM appears over COM1, and input can be directed at any of the guests. The
output of each guest and VM is colour coded.

By default input is sent to vm0, to switch input to a different VM use ~N where
N is 0, 1 or 2. The escape character, tilde, is only recognized following a newline.

C++
---

An example of using C++ in a component can be found in the [cxx app](/apps/cxx/),
which can be selected by

    make simple_cxx_defconfig

There is a separate [README for it](/apps/cxx/README)

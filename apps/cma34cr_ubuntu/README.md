<!--
  Copyright 2017, Data61
  Commonwealth Scientific and Industrial Research Organisation (CSIRO)
  ABN 41 687 119 230.

 This software may be distributed and modified according to the terms of
 the GNU General Public License version 2. Note that NO WARRANTY is provided.
 See "LICENSE_GPLv2.txt" for details.

 @TAG(DATA61_GPL)
 -->
Demonstration of Cross VM Connections
=====================================

This app contains a vmm component that boots a guest linux in a vm. An
additional component contains logic for reversing a string in a dataport,
whose actions are coordinated by events. This component is connected to linux
userland, and an application in the linux filesystem uses this component to
reverse strings, demonstrating cross vm connections.

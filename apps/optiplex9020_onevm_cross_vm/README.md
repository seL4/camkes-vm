Demonstration of Cross VM Connections
=====================================

This application demonstrates a vm with a linux guest whose processes can
connect to interfaces of camkes components.
The linux filesystem contains some programs for interacting with camkes interfaces:

 - `dataport_init` creates device files associated with dataports. Call this
   once per boot.
 - `dataport_write` writes its standard input to a dataport
 - `dataport_read` reads a dataport to its standard output
 - `dataport_test` runs the above scripts to initialise, write to and read from
   dataports. It creates files /dev/dp1 and /dev/dp2, each representing a
   dataport. The sizes of dataports created by this script correspond to the
   sizes of dataports defined in the camkes spec for this application. It's an
   error to create dataports using `dataport_init` that don't match the
   dataports declared in the camkes spec.

The camkes component with which the dataport is shared is continuously polling
the 4096th byte of /dev/dp2. When this byte is changed, the camkes component
prints the current contents of /dev/dp2. This is demonstrated by
`dataport_test`.

Demonstration of Networking in Camkes using PicoTCP
===================================================

This app demonstrates the use of networking in camkes using the 
PicoTCP network stack on seL4. The PicoUDPServer provides a UDP
interface to other components to send/receive packets. 
The example usage is an echo server that listens for incoming 
packets and replies back to the sender. 

## TCP Server
The TCP server (PicoTCPServer) component has a demonstration of
usage of a TCP connection, where the network stack and the app
are in the same component. 
The interface to the ethdriver is identical, so any usage of 
PicoUDPServer in the cma34cr_picotcp.camkes app can be replaced with 
a PicoTCPServer one. Both components can also coexist in one app, or 
multiple instances set up, given that they are not configured with 
confliting IP addresses.

There are two apps inside:
* Echo will launch a server and wait for incoming connections, replying to any data being sent. 
* TCP client will connect to a waiting server, and print any data that is sent during the connection.

## To use:
Add the library into libs/libpicotcp. There is a repo
that is compatible with the seL4 build system. 
In the menuconfig, build the picoTCP library and libethdrivers. 
The number of preallocated bufs may need to be reduced (usually 64 works) in the 
menuconfig of libethdrivers, or the ram size in camkes app config for the 
UDP/TCP server could be increased. 


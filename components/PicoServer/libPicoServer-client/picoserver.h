/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#pragma once

#include <pico_socket.h>
#include <pico_addressing.h>
#include <pico_ipv4.h>
#include <picoserver_event.h>
#include <picoserver_peer.h>

/*
 * Reference only the necessary constants and functions to clients that are
 * connected the PicoServer component.
 *
 * These constants and functions cover all the functionality that the
 * PicoServer currently provides.
 */
#define PICOSERVER_READ PICO_SOCK_EV_RD
#define PICOSERVER_WRITE PICO_SOCK_EV_WR
#define PICOSERVER_CONN PICO_SOCK_EV_CONN 
#define PICOSERVER_CLOSE PICO_SOCK_EV_CLOSE
#define PICOSERVER_FIN PICO_SOCK_EV_FIN
#define PICOSERVER_ERR PICO_SOCK_EV_ERR

#define PICOSERVER_SHUT_RD PICO_SHUT_RD
#define PICOSERVER_SHUT_WR PICO_SHUT_WR
#define PICOSERVER_SHUT_RDWR PICO_SHUT_RDWR

#define PICOSERVER_ANY_ADDR_IPV4 PICO_IPV4_INADDR_ANY

/*
 * These functions will convert an IP address in string form to a integer
 * bitfield in network order format (1.2.3.4 -> 0x04030201), and vice versa.
 * 
 * When converting a IPv4 address to a string, depending on the IP address, the
 * length of the buffer will have to be at most 16 bytes long which includes
 * the NULL byte at the end.
 */
int pico_ipv4_to_string(char *ipbuf, const uint32_t ip); int
pico_string_to_ipv4(const char *ipstr, uint32_t *ip);

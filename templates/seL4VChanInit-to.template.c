/*#
 *# Copyright 2016, NICTA
 *#
 *# This software may be distributed and modified according to the terms of
 *# the BSD 2-Clause license. Note that NO WARRANTY is provided.
 *# See "LICENSE_BSD2.txt" for details.
 *#
 *# @TAG(NICTA_BSD)
 #*/

/* This file is intentionally left blank. This side of the connector exists
 * so vchan components can be seen to expose an "init" interface that a VMM
 * can connect to in order to perform some initialisation. In reality, all
 * the logic for initialising a vchan connection is in the from side of this
 * connection.

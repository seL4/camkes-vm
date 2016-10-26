/*
 * Copyright 2016, Data 61
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(D61_GPL)
 */

#ifndef __CONSUMES_EVENT_H
#define __CONSUMES_EVENT_H

int consumes_event_wait(int fd);
int consumes_event_poll(int fd);

#endif

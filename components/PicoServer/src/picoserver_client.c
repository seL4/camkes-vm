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

#include <limits.h>
#include <picoserver_client.h>

static picoserver_client_t **clients = NULL;
static khash_t(socket_addr) *socket_addr_table = NULL;

static khint32_t find_free_id(seL4_Word client_id) {
    /* Client ID has been checked already */
    khint32_t curr = clients[client_id]->bump_index;
    khint32_t old_index = curr;
    while (1) {
        khint_t iter = kh_get(socket, clients[client_id]->socket_table, curr);
        if (iter == kh_end(clients[client_id]->socket_table)) {
            clients[client_id]->bump_index = curr + 1;
            return curr;
        }
        curr++;
        if (curr == (khint32_t) INT_MIN) {
            /* Sockets are 'int' types, and there likely won't be more than INT_MAX sockets ever created.
             * We also don't use the 0 index, as that's a return code */
            curr = 1;
        }
        /* We've loop around and couldn't find any free indexes */
        if (old_index == curr) {
            return 0;
        }
    }
}

void picoserver_clients_init(void) {
    clients = calloc(num_clients, sizeof(picoserver_client_t *));
    ZF_LOGF_IF(clients == NULL, "Failed to allocate memory for the picoserver_client_t struct array");
    for (int i = 0; i < num_clients; i++) {
        clients[i] = calloc(1, sizeof(picoserver_client_t));
        ZF_LOGF_IF(clients[i] == NULL, "Failed to allocate memory for the %d'th picoserver_client_t struct", i);
        clients[i]->socket_table = kh_init(socket);
        ZF_LOGF_IF(clients[i]->socket_table == NULL, "Failed to initialise client %d's socket table", i + 1);
        clients[i]->socket_event_set = kh_init(socket_event);
        ZF_LOGF_IF(clients[i]->socket_event_set == NULL, "Failed to initialise client %d's socket event set", i + 1);
        /* Set the bump index to the first free, 0 is reserved for error code */
        clients[i]->bump_index = 1;
    }
    socket_addr_table = kh_init(socket_addr);
    ZF_LOGF_IF(socket_addr_table == NULL, "Failed to initialise the socket address table");
}

uint32_t client_get_num_sockets(seL4_Word client_id) {
    picoserver_client_t *client = clients[client_id];
    return kh_size(client->socket_table);
}

picoserver_socket_t* client_get_socket(seL4_Word client_id, int socket_id) {
    picoserver_client_t *client = clients[client_id];
    khint_t k = kh_get(socket, client->socket_table, socket_id);  
    /* Check if the key actually exists in the HT */
    if (k == kh_end(client->socket_table)) {
        return NULL;
    }
    return kh_val(client->socket_table, k);
}

picoserver_socket_t* client_get_socket_by_addr(struct pico_socket *socket_addr) {
    khint_t k = kh_get(socket_addr, socket_addr_table, (khint64_t) socket_addr);
    /* Check if the key actually exists in the HT */
    if (k == kh_end(socket_addr_table)) {
        return NULL;
    }
    return kh_val(socket_addr_table, k);
}

int client_put_socket(seL4_Word client_id, picoserver_socket_t *new_socket) {
    ZF_LOGF_IF(client_id > num_clients, "Client ID is greater than the number of clients registered");
    /* Sanity check, just in case */
    ZF_LOGF_IF(new_socket == NULL, "Trying to insert an empty picoserver_socket into the socket table");
    picoserver_client_t *client = clients[client_id];
    khint_t socket_iter;
    khint_t socket_addr_iter;

    /* Insert the entry into the client's socket table */
    khint32_t free_id = find_free_id(client_id);
    /* Didn't find any free ID */
    if (free_id == 0) {
        ZF_LOGE("Couldn't find a free ID");
        return -1;
    }
    int ret;
    socket_iter = kh_put(socket, client->socket_table, free_id, &ret);
    /* Failed to insert a new entry */
    if (ret == -1) {
        ZF_LOGE("Failed to insert a new entry in");
        return -1;
    }
    /* Trying to replace an entry */
    if (ret == 0) {
        ZF_LOGF("Tried to replace an entry in the socket table for client %d", client_id + 1);
    }
    kh_val(client->socket_table, socket_iter) = new_socket;

    /* Insert the entry into the socket address table */
    socket_addr_iter = kh_put(socket_addr, socket_addr_table, (khint64_t) (new_socket->socket), &ret);
    /* Failed to insert a new entry */
    if (ret == -1) {
        ZF_LOGE("Failed to insert the entry into the socket address table");
        /* Remove the socket table entry from above */
        kh_del(socket, client->socket_table, socket_iter);
        return -1;
    }
    /* Trying to replace an entry */
    if (ret == 0) {
        /* Make sure that the old entry is the same as the new one */
        picoserver_socket_t *old_socket = kh_val(socket_addr_table, socket_addr_iter);
        if (old_socket->socket == new_socket->socket) {
            /* PicoTCP decided to reuse a pico_socket struct, invalidate the old socket FD */
            int old_socket_fd = old_socket->socket_fd;
            khint_t socket_iter;
            khint_t socket_event_iter;
            /* Remove it from the client's socket table */
            socket_iter = kh_get(socket, client->socket_table, old_socket_fd);
            ZF_LOGF_IF(socket_iter == kh_end(client->socket_table), 
                       "Failed to invalidate old picoserver_socket_t struct");
            kh_del(socket, client->socket_table, old_socket_fd);
            old_socket->socket = NULL;
            free(old_socket);
            /* Remove any events from the client's event set, if any */
            socket_event_iter = kh_get(socket_event, client->socket_event_set, old_socket_fd);
            if (socket_event_iter != kh_end(client->socket_event_set)) {
                kh_del(socket_event, client->socket_event_set, socket_event_iter);
            }
        } else {
            ZF_LOGF("Tried to replace an entry in the socket addr table");
        }
    }
    kh_val(socket_addr_table, socket_addr_iter) = new_socket;

    return free_id;
}

/*
 * We are making an assumption here that we don't insert NULL entries.
 */
int client_delete_socket(seL4_Word client_id, int socket_id) {
    picoserver_client_t *client = clients[client_id];
    picoserver_socket_t *client_socket;
    khint_t socket_iter;
    khint_t socket_addr_iter;
    khint_t socket_event_iter;

    socket_iter = kh_get(socket, client->socket_table, socket_id);
    /* Check if the socket actually exists in the table */
    if (socket_iter == kh_end(client->socket_table)) {
        return -1;
    }

    client_socket = kh_val(client->socket_table, socket_iter);
    /* Get the iterator for the socket address table */
    socket_addr_iter = kh_get(socket_addr, socket_addr_table, (khint64_t) client_socket->socket);
    ZF_LOGF_IF(socket_addr_iter == kh_end(socket_addr_table), 
               "Corresponding entry in the socket address table doesn't exist!");

    pico_socket_close(client_socket->socket);
    free(client_socket);
    /* Get the iterator for socket event set, if any */
    socket_event_iter = kh_get(socket_event, client->socket_event_set, socket_id);

    /* Delete the entries from the hash tables */
    kh_del(socket, client->socket_table, socket_iter);
    kh_del(socket_addr, socket_addr_table, socket_addr_iter);

    /* Delete any outstanding events for this socket, if any */
    if (socket_event_iter != kh_end(client->socket_event_set)) {
        kh_del(socket_event, client->socket_event_set, socket_event_iter);
    }

    return 0;
}

void client_get_event(seL4_Word client_id, picoserver_event_t *ret_event) {
    picoserver_client_t *client = clients[client_id];
    khash_t(socket_event) *client_event_set = client->socket_event_set;
    int socket_id = 0;
    /* Sanity check */
    ZF_LOGF_IF(ret_event == NULL, "Passing a null container");
    khint_t iter = kh_begin(client_event_set);

    /* Find the first socket in the set */
    for (; iter < kh_end(client_event_set); iter++) {
        if (kh_exist(client_event_set, iter)) {
            socket_id = kh_key(client_event_set, iter);
            kh_del(socket_event, client_event_set, iter);
            break;
        }
    }
    
    if (socket_id == 0) {
        return;
    }

    /* Retrieve the socket and get the event information */
    picoserver_socket_t *client_socket = client_get_socket(client_id, socket_id);
    ZF_LOGF_IF(client_socket == NULL, 
               "Could not find picoserver_socket struct for client id %u and socket %d", 
               client_id + 1, socket_id);
    ret_event->socket_fd = socket_id;
    ret_event->events = client_socket->events;
    ret_event->num_events_left = kh_size(client_event_set);

    /* Clear the event bits of the socket now that we've retrieved them */
    client_socket->events = 0;

    return;
}

int client_put_event(seL4_Word client_id, int socket_id, uint16_t event) {
    picoserver_client_t *client = clients[client_id];
    /* Fetch the socket from the client and add the event */
    picoserver_socket_t *client_socket = client_get_socket(client_id, socket_id);
    if (client_socket == NULL) {
        return -1;
    }
    client_socket->events |= event;

    /* Add the socket to the set if not already there */
    int ret = 0;
    kh_put(socket_event, client->socket_event_set, socket_id, &ret);
    if (ret == -1) {
        return -1;
    }

    return 0;
}

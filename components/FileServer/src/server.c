/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(NICTA_GPL)
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <cpio/cpio.h>
#include <sel4utils/mapping.h>

#include <sel4/sel4.h>

#include <FileServer.h>

#define BUF_SIZE PAGE_SIZE_4K
#define FS_ERR_NOFILE -1
#define FS_ERR_BLOCKED_PORT -2

/* Files are loaded from the cpio archive */
extern char _cpio_archive[];

static seL4_Word busy_with_request = -1;

typedef struct cpio_entry {
    const char *name;
    unsigned long size;
    void *file;
} cpio_entry_t;

static cpio_entry_t *cpio_file_list = NULL;
static struct cpio_info cinfo;

/* Function pointers for managing instances, valid clients */
static void *server_get_file_dataport(void);
static void init_cpio_list(void);
static cpio_entry_t *get_cpio_entry(int fd);

static void *fs_dataport;

/*
    Initialise a list of files from the cpio
        that can be searched and indexed
*/
static void init_cpio_list(void) {
    int error;
    error = cpio_info(_cpio_archive, &cinfo);
    assert(error == 0);
    cpio_file_list = malloc(sizeof(cpio_entry_t) * cinfo.file_count);
    assert(cpio_file_list != NULL);
    for(int i = 0; i < cinfo.file_count; i++) {
        cpio_entry_t *ent = &(cpio_file_list[i]);
        ent->file = cpio_get_entry(_cpio_archive, i, &ent->name, &ent->size);
        assert(ent->file != NULL);
    }
}

void pre_init(void) {
    fs_dataport = (void *) fs_mem;
    init_cpio_list();
}

/*
    Return the address of a client/guest vm's shared dataport
*/
static void *server_get_file_dataport() {
    return fs_dataport;
}

/*
    Lookup a file on the fileserver and return an index number to that file
*/
int fs_ctrl_lookup(const char *name) {
    for(int i = 0; i < cinfo.file_count; i++) {
        cpio_entry_t *ent = &(cpio_file_list[i]);
        if(strncmp(name, ent->name, strlen(name)) == 0) {
            return i;
        }
    }
    return FS_ERR_NOFILE;
}

seL4_Word fs_ctrl_get_badge(void);

/*
    Ends a server-client lock
*/
void fs_ctrl_read_complete() {
    seL4_Word lock = fs_ctrl_get_badge();
    if(lock == busy_with_request)
        busy_with_request = -1;
}

size_t fs_ctrl_filesize(int fd) {
    cpio_entry_t *ent = get_cpio_entry(fd);
    if(ent == NULL)
        return 0;
    return ent->size;
}

/*
    Writes some data into a clients dataport, up to the maximum size for that dataport
*/
int fs_ctrl_read(int fd, off_t offset, size_t size) {
    cpio_entry_t *ent = get_cpio_entry(fd);
    if(ent == NULL)
        return FS_ERR_NOFILE;

    seL4_Word lock = fs_ctrl_get_badge();
    if(busy_with_request == -1) {
        busy_with_request = lock;
    } else if(busy_with_request != lock) {
        return FS_ERR_BLOCKED_PORT;
    }

    if(offset >= ent->size) {
        return 0;
    }

    void *dataport = server_get_file_dataport();
    if(dataport == NULL) {
        return -1;
    }

    size = MIN(size, BUF_SIZE);
    memcpy(dataport, ent->file + offset, size);
    return size;
}

static cpio_entry_t *get_cpio_entry(int fd) {
    if(fd < 0)
        return NULL;
    if(fd >= cinfo.file_count)
        return NULL;
    return &cpio_file_list[fd];
}


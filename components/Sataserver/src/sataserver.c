/*
 * Copyright 2019, Dornerworks
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#include <camkes.h>
#include <camkes/io.h>
#include <camkes/dma.h>
#include <platsupport/io.h>
#include <vka/vka.h>
#include <simple/simple.h>
#include <simple/simple_helpers.h>
#include <allocman/allocman.h>
#include <allocman/bootstrap.h>
#include <allocman/vka.h>
#include <sel4utils/vspace.h>
#include <sel4utils/iommu_dma.h>
#include <sel4platsupport/arch/io.h>
#include <satadrivers/common.h>
#include <satadrivers/ide.h>
#include <satadrivers/ahci.h>
#include <sataserver.h>

#include <sataserver/gen_config.h>

#define MAX_PARTITIONS    4
#define VIRT_START_SECTOR 64
#define BUF_SIZE          4096
#define BRK_VIRTUAL_SIZE  400000000

#define MAX_NUM_CYL       1023
#define MAX_NUM_HEAD      255
#define MAX_NUM_SECT      63
#define START_SECTOR      0

#define ALLOCATOR_POOL_SIZE 8388608

/* Global variables to create the seL4 CSpace/Vspace environment for DMA */
extern reservation_t muslc_brk_reservation;
extern void *muslc_brk_reservation_start;
extern vspace_t  *muslc_this_vspace;
static sel4utils_res_t muslc_brk_reservation_memory;
static allocman_t *allocman;
static char allocator_mempool[ALLOCATOR_POOL_SIZE];
static simple_t camkes_simple;
static vka_t vka;
static vspace_t vspace;
static sel4utils_alloc_data_t vspace_data;
static ps_io_ops_t ioops;
static seL4_CPtr(*original_vspace_get_cap)(vspace_t *, void *);

static sata_driver_t sata_driver;

void camkes_make_simple(simple_t *simple);

uint8_t assigned_parts[MAX_PARTITIONS] = {0};

unsigned char g_part_data[SATA_BLK_SIZE] = {0};
partition_table_t phys_partition_tables[MAX_PARTITIONS] = {0};

/* Variable to let the functions know initialization has completed */
static int done_init = 0;
static int invalid_config = 0;

typedef struct client {

    /* id for this client */
    int client_id;

    /* Partitions allocated to this client */
    uint8_t partitions[MAX_PARTITIONS];

    /* Number of partitions allocated to this client */
    uint8_t num_partitions;

    /* Virtual Partition Tables to copy to the guest */
    partition_table_t partition_tables[MAX_PARTITIONS];

    /* Capacity for the virtual Hard Drive */
    uint64_t capacity;

    /* dataport for this client */
    void *dataport;

} client_t;

/* Global Client Variables */
static int num_clients = 0;
static client_t *clients = NULL;

/* Functions provided by the Sataserver template */
unsigned int client_get_sender_id(void);
unsigned int client_num_badges(void);
unsigned int client_enumerate_badge(unsigned int i);
void *client_buf(unsigned int client_id);
void client_get_partitions(unsigned int badge, uint8_t *partition_list, uint8_t *num_partitions);

/*
 * Purpose: Initialize seL4 allocators for DMA IO Operations
 *
 * Inputs: void
 *
 * Returns: void
 *
 */
static void init_system(void)
{
    int error;

    /* Camkes adds nothing to our address space, so this array is empty */
    void *existing_frames[] = {
        NULL
    };
    camkes_make_simple(&camkes_simple);

    /* Initialize allocator */
    allocman = bootstrap_use_current_1level(
                   simple_get_cnode(&camkes_simple),
                   simple_get_cnode_size_bits(&camkes_simple),
                   simple_last_valid_cap(&camkes_simple) + 1,
                   BIT(simple_get_cnode_size_bits(&camkes_simple)),
                   sizeof(allocator_mempool), allocator_mempool
               );
    assert(allocman);
    error = allocman_add_simple_untypeds(allocman, &camkes_simple);
    allocman_make_vka(&vka, allocman);

    /* Initialize the vspace */
    error = sel4utils_bootstrap_vspace(&vspace, &vspace_data,
                                       simple_get_init_cap(&camkes_simple, seL4_CapInitThreadPD),
                                       &vka, NULL, NULL, existing_frames);
    assert(!error);

    sel4utils_reserve_range_no_alloc(&vspace, &muslc_brk_reservation_memory, BRK_VIRTUAL_SIZE,
                                     seL4_AllRights, 1, &muslc_brk_reservation_start);
    muslc_this_vspace = &vspace;
    muslc_brk_reservation = (reservation_t) {
        .res = &muslc_brk_reservation_memory
    };
}

/*
 * Purpose: Calculate the physical sector offset
 *
 * Inputs:
 *   - *client: client structure information
 *   - *offset: pointer to calculated offset
 *   - sector: virtual sector
 *
 * Returns: 1 - Success; 0 - Failure
 *
 */
static uint8_t calulate_sector_offset(client_t *client, int *offset, uint32_t sector)
{
    uint8_t found = 0;
    uint32_t virt_start;
    uint8_t part_idx;

    /* Calculate physical sector from virtual sector (given) */
    int i = 0;
    while (!found && i < client->num_partitions) {
        virt_start = client->partition_tables[i].start_lba;
        if ((sector >= virt_start) && (sector < (virt_start + client->partition_tables[i].num_sectors))) {
            /* partitions start counting at 1, index starts at 0 */
            part_idx = client->partitions[i] - 1;
            *offset = (int)phys_partition_tables[part_idx].start_lba - (int)virt_start;
            found = 1;
        }
        i++;
    }

    return found;
}

/*
 * Purpose: Receive data (read)
 *
 * Inputs:
 *   - sector: sector to transmit data to
 *   - len: number of bytes to read
 *
 * Returns: Number of bytes read
 *
 */
int client_rx(unsigned int sector, unsigned int len)
{
    /* We need to finish the init process before reading data */
    if (!done_init) {
        return 0;
    }

    if (len > BUF_SIZE) {
        return 0;
    }

    int err;

    /* Lock the sataserver so it can't deal with other clients */
    err = sataserver_mux_lock();

    int offset;
    uint8_t found;
    int ret = len;
    unsigned char part_data[SATA_BLK_SIZE] = {0};

    /* we need to determine which client we're dealing with */
    int id = client_get_sender_id();

    /* Copy the proper client id structure into a local copy */
    client_t *client = NULL;
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].client_id == id) {
            client = &clients[i];
        }
    }
    assert(client);

    /* Get the location of where to put the information */
    void *packet = client->dataport;

    /* If sector is 0, guest is looking for partition tables so we need to copy in the virtual tables */
    if (START_SECTOR == sector) {
        if ((NULL == g_part_data) || (NULL == part_data)) {
            ret = 0;
        } else {
            memcpy(part_data, g_part_data, SATA_BLK_SIZE);
            memcpy(&part_data[PART_OFFSET], client->partition_tables, sizeof(client->partition_tables));
            memcpy(packet, part_data, SATA_BLK_SIZE);
        }
    }

    /* Just return zeros for anything between Sector 1 and VIRT_START_SECTOR */
    else if (sector < VIRT_START_SECTOR) {
        memset(packet, 0, len);
    }

    /* Read from the sector */
    else {
        found = calulate_sector_offset(client, &offset, (uint32_t)sector);
        if (found) {
            err = sata_read_sectors(&sata_driver, drive, len / SATA_BLK_SIZE, sector + offset, packet);
            if (err) {
                ret = 0;
            }
        } else {
            ret = 0;
        }
    }

    err = sataserver_mux_unlock();
    return ret;
}

/*
 * Purpose: Transmit data (read)
 *
 * Inputs:
 *   - sector: sector to transmit data to
 *   - len: number of bytes to write
 *
 * Returns: Number of bytes written
 *
 */
int client_tx(unsigned int sector, unsigned int len)
{
    /* We need to finish the init process before transmitting data */
    if (!done_init) {
        return 0;
    }

    if (len > BUF_SIZE) {
        return 0;
    }

    int err;

    /* Lock the sataserver so it can't deal with other clients */
    err = sataserver_mux_lock();

    int offset;
    uint8_t found;
    int ret = len;

    /* we need to determine which client we're dealing with */
    int id = client_get_sender_id();

    /* Copy the proper client id structure into a local copy */
    client_t *client = NULL;
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].client_id == id) {
            client = &clients[i];
        }
    }
    assert(client);

    /* Get the location of the information to transmit from the shared dataport */
    void *packet = client->dataport;

    /* Only write data that starts at or after VIRT_START_SECTOR */
    if (sector >= VIRT_START_SECTOR) {
        /* Calculate the physical sector to write (if it exists) */
        found = calulate_sector_offset(client, &offset, sector);
        if (found) {
            err = sata_write_sectors(&sata_driver, drive, len / SATA_BLK_SIZE, sector + offset, packet);
            if (err) {
                ret = 0;
            }
        } else {
            /* Error handling */
            ret = 0;
        }
    }

    err = sataserver_mux_unlock();
    return ret;
}

/*
 * Purpose: Get the capacity of the client
 *
 * Inputs: void
 *
 * Returns: The capacity of the client, or 0 if the init is incomplete
 *
 */
unsigned int client_get_capacity(void)
{
    if (!done_init) {
        return 0;
    }
    /*
     * Because there are multiple clients connected to the server,
     * we need to determine the ID of the sender before returning
     * its capacity
     */
    int id = client_get_sender_id();

    /* Blank client structure */
    client_t *client = NULL;

    /* Run through each client and check the ID. Then copy its structure */
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].client_id == id) {
            client = &clients[i];
        }
    }
    assert(client);

    return client->capacity;
}

unsigned int client_get_status(void)
{
    int status = SATASERVER_STATUS_GOOD;

    if (invalid_config) {
        status = SATASERVER_STATUS_INVALID_CONF;
    } else if (!done_init) {
        status = SATASERVER_STATUS_NOT_DONE;
    }

    return status;
}

/*
 * Purpose: Check to see whether the partition is assigned yet.
 *          If it isn't, then assign the partition.
 *
 * Inputs:
 *   - partition: disk partition to check/assign
 *
 * Returns: 0: Success; -1 Failure
 *
 */
int check_part_is_assigned(uint8_t partition)
{
    /* partitions start counting at 1, index starts at 0 */
    uint8_t part = partition - 1;
    int ret = 0;

    /*
     * We need to check 2 things before assigning. First, whether
     * the partition is outside the allowable partitions, and
     * second, whether the partition is already marked as assigned.
     * If those conditions are passed, then set the partition as
     * assigned.
     *
     */
    if ((part >= MAX_PARTITIONS) || assigned_parts[part]) {
        ret = -1;
    } else {
        assigned_parts[part] = 1;
        ret = 0;
    }
    return ret;
}

/* Returns the cap to the frame mapped to vaddr, assuming
 * vaddr points inside our dma pool. */
static seL4_CPtr get_dma_frame_cap(vspace_t *vspace, void *vaddr)
{
    seL4_CPtr cap = camkes_dma_get_cptr(vaddr);
    if (cap == seL4_CapNull) {
        return original_vspace_get_cap(vspace, vaddr);
    }
    return cap;
}

/* Allocate a dma buffer backed by the component's dma pool */
static void *camkes_iommu_dma_alloc(void *cookie, size_t size,
                                    int align, int cached, ps_mem_flags_t flags)
{
    // allocate buffer from the dma pool
    void *vaddr = camkes_dma_alloc(size, align, cached);
    if (vaddr == NULL) {
        ZF_LOGE("Failed to alloc DMA buf");
        return NULL;
    }
    int error = sel4utils_iommu_dma_alloc_iospace(cookie, vaddr, size);
    if (error) {
        ZF_LOGE("failed to alloc for iospace");
        camkes_dma_free(vaddr, size);
        return NULL;
    }
    return vaddr;
}

void post_init(void)
{
    int error;
    int pci_bdf_int;
    int bus, dev, fun;

    /* Connect to the Serial Server */
    set_putchar(putchar_putchar);

    error = sataserver_mux_lock();

    /* initialize seL4 allocators and give us a half sane environment */
    init_system();

    error = camkes_ps_malloc_ops(&ioops.malloc_ops);
    assert(!error);

#ifdef CONFIG_SATASERVER_USE_AHCI
    ZF_LOGF_IF(num_bdfs <= 0, "Please configure num_bdfs to be > 0");

    /* initialize the driver */
    seL4_CPtr *iospace_caps = (seL4_CPtr *)malloc(num_bdfs * sizeof(seL4_CPtr));
    ZF_LOGF_IF(NULL == iospace_caps, "Failed to allocate iospace_caps array");

    cspacepath_t *iospaces = (cspacepath_t *)malloc(num_bdfs * sizeof(cspacepath_t));
    ZF_LOGF_IF(NULL == iospaces, "Failed to allocate iospaces array");

    ZF_LOGF_IF(NULL == pci_bdfs, "Please set pci bdfs\n");

    char *local_pci_bdfs = (char *)malloc(strlen(pci_bdfs));
    ZF_LOGF_IF(NULL == local_pci_bdfs, "Please set pci bdfs\n");

    /* Make a copy of the pci_bdfs string for strtok to manipulate */
    strncpy(local_pci_bdfs, pci_bdfs, strlen(pci_bdfs));

    char *pci_bdf = strtok(local_pci_bdfs, ",");

    for (int i = 0; i < num_bdfs; i++) {
        error = vka_cspace_alloc_path(&vka, &iospaces[i]);
        assert(!error);

        if (i != 0) {
            pci_bdf = strtok(NULL, ",");
        }

        ZF_LOGF_IF(NULL == pci_bdf, "Failed to find pci_bdf in loop %d", i);

        sscanf(pci_bdf, "%x:%x.%d", &bus, &dev, &fun);
        pci_bdf_int = bus * 256 + dev * 8 + fun;

        /* get this from the configuration */
        error = simple_get_iospace(&camkes_simple, iospace_id, pci_bdf_int, &iospaces[i]);
        assert(!error);

        iospace_caps[i] = iospaces[i].capPtr;
    }

    /* Save a pointer to the original get_cap function for our vspace */
    original_vspace_get_cap = vspace.get_cap;

    /* The iommu driver needs the caps to frames backing the dma buffer.
     * It will invoke the get_cap method of its vspace to get these caps.
     * Since the vspace we give to the iommu driver wasn't used to allocate
     * the dma buffer, it doesn't know the caps to the frames backing the
     * buffer. CAmkES allocated the buffer statically, and so the caps are
     * known to it. Here, we override the get_cap method of our vspace to
     * return dma buffer frame caps provided by CAmkES. */
    vspace.get_cap = get_dma_frame_cap;
    error = sel4utils_make_iommu_dma_alloc(&vka, &vspace, &ioops.dma_manager, num_bdfs, iospace_caps);
    assert(!error);
    ioops.dma_manager.dma_alloc_fn = camkes_iommu_dma_alloc;
    error = sel4platsupport_get_io_port_ops(&ioops.io_port_ops, &camkes_simple, &vka);
    assert(!error);

    /* preallocate buffers */
    int clb_buf_size = BUF_SIZE;
    void *clb_buf = ps_dma_alloc(&ioops.dma_manager, clb_buf_size, 4, 0, PS_MEM_NORMAL);
    assert(clb_buf);
    uintptr_t phys = ps_dma_pin(&ioops.dma_manager, clb_buf, clb_buf_size);
    assert(phys == (uintptr_t)clb_buf);

    int ctba_buf_size = BUF_SIZE * 16;
    void *ctba_buf = ps_dma_alloc(&ioops.dma_manager, ctba_buf_size, 4, 0, PS_MEM_NORMAL);
    assert(ctba_buf);
    uintptr_t phys2 = ps_dma_pin(&ioops.dma_manager, ctba_buf, ctba_buf_size);
    assert(phys2 == (uintptr_t)ctba_buf);

    int fb_buf_size = BUF_SIZE;
    void *fb_buf = ps_dma_alloc(&ioops.dma_manager, fb_buf_size, 4, 0, PS_MEM_NORMAL);
    assert(fb_buf);
    uintptr_t phys3 = ps_dma_pin(&ioops.dma_manager, fb_buf, fb_buf_size);
    assert(phys3 == (uintptr_t)fb_buf);

    int data_buf_size = BUF_SIZE;
    void *data_buf = ps_dma_alloc(&ioops.dma_manager, data_buf_size, 4, 0, PS_MEM_NORMAL);
    assert(data_buf);
    uintptr_t phys4 = ps_dma_pin(&ioops.dma_manager, data_buf, data_buf_size);
    assert(phys4 == (uintptr_t)data_buf);

    /* Initialize the device */
    ahci_intel_config_t ahci_config = (ahci_intel_config_t) {
        .bar0      = (void *)ahcidriver,
        .clb       = clb_buf,
        .clb_size  = clb_buf_size,
        .ctba      = ctba_buf,
        .ctba_size = ctba_buf_size,
        .fb        = fb_buf,
        .fb_size   = fb_buf_size,
        .data      = data_buf,
        .data_size = data_buf_size
    };

    error = sata_init(&ioops, &sata_driver, AHCI, &ahci_config);
    assert(!error);
#else
    error = sata_init(&ioops, &sata_driver, IDE, NULL);
    assert(!error);
#endif

    /* Get the SATA device partition tables, We assume drive 0 for now.
     *    TBD: Handle different/dynamic drives
     */
    sata_get_partition_tables(&sata_driver, drive, phys_partition_tables, g_part_data);

    uint8_t partition;

    /* Determine the number of clients and allocate memory for their structures */
    num_clients = client_num_badges();
    clients = calloc(num_clients, sizeof(client_t));

    /* Get and validate client specific data for each client connected to the sataserver */
    for (int client = 0; client < num_clients; client++) {

        /* Get the information from the configuration */
        clients[client].client_id = client_enumerate_badge(client);
        clients[client].dataport = client_buf(clients[client].client_id);
        client_get_partitions(clients[client].client_id, clients[client].partitions, &clients[client].num_partitions);
        ZF_LOGI("SATASERVER: Client %u Partitions: ", clients[client].client_id);

        /* We need to ensure the number of partitions assigned to the client are valid */
        if (clients[client].num_partitions > MAX_PARTITIONS) {
            invalid_config = 1;
            goto out;
        }

        /* Go through each partition assigned to the client and check to make
         * sure the part isn't assigned */
        for (int i = 0; i < clients[client].num_partitions; i++) {
            partition = clients[client].partitions[i];
            error = check_part_is_assigned(partition);
            if (error) {
                invalid_config = 1;
                goto out;
            }
            ZF_LOGI("\t%u ", partition);
        }
    }

    int part_idx;
    uint32_t sectors = VIRT_START_SECTOR;
    uint8_t head;
    uint8_t sec;
    uint16_t cyl;
    uint32_t end_lba = 0;

    /* Run through each client's partitions and copy physical information */
    for (int client = 0; client < num_clients; client++) {

        sectors = VIRT_START_SECTOR;

        /* Make sure virtual partition tables are cleared */
        memset(clients[client].partition_tables, 0, sizeof(clients[client].partition_tables));

        /* Go through each clients assigned partitions */
        for (int i = 0; i < clients[client].num_partitions; i++) {
            /* partitions start counting at 1, index starts at 0 */
            part_idx = clients[client].partitions[i] - 1;

            /* Copy the values that stay the same */
            clients[client].partition_tables[i].boot = phys_partition_tables[part_idx].boot;
            clients[client].partition_tables[i].sys_id = phys_partition_tables[part_idx].sys_id;
            clients[client].partition_tables[i].num_sectors = phys_partition_tables[part_idx].num_sectors;

            /* Current virtual partition should start right after the last
             * partition or at the VIRT_START_SECTOR. This will create no holes
             * in the virtual disk
             */
            clients[client].partition_tables[i].start_lba = sectors;

            if ((clients[client].partition_tables[i].start_lba / (MAX_NUM_HEAD * MAX_NUM_SECT)) > MAX_NUM_CYL) {
                cyl = MAX_NUM_CYL;
                head = MAX_NUM_HEAD;
                sec = MAX_NUM_SECT;
            } else {
                // convert LBA to CHS
                cyl = clients[client].partition_tables[i].start_lba / (MAX_NUM_HEAD * MAX_NUM_SECT);
                head = (clients[client].partition_tables[i].start_lba / MAX_NUM_SECT) % MAX_NUM_HEAD;
                sec = ((clients[client].partition_tables[i].start_lba % MAX_NUM_SECT) + 1);
            }
            clients[client].partition_tables[i].head = head;
            // The MBR specifies the cylinders as a 10 bit value and the sectors as a 6 bit value
            // These values are packed in the MBR with the upper 2 bits of the cylinders value
            // tacked onto the upper 2 bits of the sectors byte
            clients[client].partition_tables[i].sec_cyl = sec | ((cyl & 0x0300) >> 2) | ((cyl & 0x00FF) << 8);

            end_lba = clients[client].partition_tables[i].start_lba + clients[client].partition_tables[i].num_sectors - 1;
            if ((end_lba / (MAX_NUM_HEAD * MAX_NUM_SECT)) > MAX_NUM_CYL) {
                cyl = MAX_NUM_CYL;
                head = MAX_NUM_HEAD;
                sec = MAX_NUM_SECT;
            } else {
                // convert LBA to CHS
                cyl = end_lba / (MAX_NUM_HEAD * MAX_NUM_SECT);
                head = (end_lba / MAX_NUM_SECT) % MAX_NUM_HEAD;
                sec = ((end_lba % MAX_NUM_SECT) + 1);
            }
            clients[client].partition_tables[i].end_head = head;
            clients[client].partition_tables[i].end_sec_cyl = sec | ((cyl & 0x0300) >> 2) | ((cyl & 0x00FF) << 8);

            /* Increment sectors to figure out the starting LBA for the next partition */
            sectors += clients[client].partition_tables[i].num_sectors;

            ZF_LOGI("Client(%u): Virtual Partition %i:", clients[client].client_id, i);
            ZF_LOGI("\tStart: %u, Sectors: %u", clients[client].partition_tables[i].start_lba,
                    clients[client].partition_tables[i].num_sectors);
            ZF_LOGI("\tBoot: %u, System ID: 0x%X", clients[client].partition_tables[i].boot,
                    clients[client].partition_tables[i].sys_id);
        }

        clients[client].capacity = sectors;
        ZF_LOGI("\n\t\tCapacity: %llu", clients[client].capacity);
    }

out:
    done_init = 1;
    if (invalid_config) {
        ZF_LOGE("SATASERVER: Client config is invalid");
    }
    error = sataserver_mux_unlock();
}

/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <autoconf.h>
#include <arm_vm/gen_config.h>
#include <vmlinux.h>

#include <camkes.h>
#include <camkes/dataport.h>

#include <sel4vmmplatsupport/drivers/virtio_net.h>
#include <virtioarm/virtio_net.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#define IPV4_LENGTH 4
#define ARP_REQUEST 0x01
#define ARP_REPLY 0x02
#define HW_TYPE 1

extern vmm_pci_space_t *pci;
extern vmm_io_port_list_t *io_ports;

void self_mac(uint8_t *mac)
{
    struct ether_addr res;
    struct ether_addr *resp;
    resp = ether_aton_r(vswitch_mac_address, &res);
    if (resp == NULL) {
        ZF_LOGF("Failed to get MAC address");
    }
    memcpy(mac, res.ether_addr_octet, ETH_ALEN);
}

static int arping_reply(char *eth_buffer, size_t length, virtio_net_t *virtio_net)
{
    int err;
    if (eth_buffer == NULL) {
        return -1;
    }
    struct ethhdr *rcv_req = (struct ethhdr *) eth_buffer;
    struct ether_arp *arp_req = (struct ether_arp *)(eth_buffer + sizeof(struct ethhdr));
    if (ntohs(rcv_req->h_proto) == ETH_P_ARP) {
        unsigned char reply_buffer[1500];
        struct ethhdr *send_reply = (struct ethhdr *) reply_buffer;
        struct ether_arp *arp_reply = (struct ether_arp *)(reply_buffer + sizeof(struct ethhdr));

        memcpy(send_reply->h_dest, arp_req->arp_sha, ETH_ALEN);
        send_reply->h_proto = htons(ETH_P_ARP);

        // MAC Address
        memcpy(arp_reply->arp_tha, arp_req->arp_sha, ETH_ALEN);
        memcpy(arp_reply->arp_sha, arp_req->arp_sha, ETH_ALEN);
        arp_reply->arp_sha[5] = arp_reply->arp_sha[5] + 2;

        memcpy(send_reply->h_source, arp_reply->arp_sha, ETH_ALEN);
        // IP Addresss
        memcpy(arp_reply->arp_spa, (void *)arp_req->arp_tpa, IPV4_LENGTH);
        memcpy(arp_reply->arp_tpa, (void *)arp_req->arp_spa, IPV4_LENGTH);
        // Misc arp fields
        arp_reply->ea_hdr.ar_hrd = htons(HW_TYPE);
        arp_reply->ea_hdr.ar_pro = htons(ETH_P_IP);
        arp_reply->ea_hdr.ar_op = htons(ARP_REPLY);
        arp_reply->ea_hdr.ar_hln = ETH_ALEN;
        arp_reply->ea_hdr.ar_pln = IPV4_LENGTH;

        err = virtio_net_rx(reply_buffer, sizeof(struct ethhdr) + sizeof(struct ether_arp), virtio_net);
        if (err) {
            ZF_LOGE("Unable to perform virtio net rx");
            return -1;
        }
    }
    return 0;
}

static virtio_net_t *virtio_net = NULL;

void make_arping_virtio_net(vm_t *vm, void *cookie)
{
    virtio_net_callbacks_t callbacks;
    callbacks.tx_callback = arping_reply;
    callbacks.irq_callback = NULL;
    callbacks.get_mac_addr_callback = self_mac;
    virtio_net = virtio_net_init(vm, &callbacks, pci, io_ports);
}

DEFINE_MODULE(virtio_net, NULL, make_arping_virtio_net)

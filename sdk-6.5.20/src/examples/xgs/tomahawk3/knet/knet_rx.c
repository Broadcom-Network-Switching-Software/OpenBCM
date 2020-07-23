/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <signal.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <features.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ether.h>

#define RCPU_SIGNATURE 0xB840
#define RX_MAX  9100

struct L2header {
    uint8_t     dest_mac[6];
    uint8_t     src_mac[6];
    uint32_t    tag;
    uint16_t    ether_type;
};

struct RCPUheader {
    uint16_t    signature;
    uint8_t     opcode;
    uint8_t     flags;
    uint16_t    trans_id;
    uint16_t    data_len;
    uint16_t    reply_len;
    uint32_t    reserved;
};

/* DEFINE for little endian processors */
/* #define LE_HOST */

/*
 * DMA Control Block - Type 21
 * Used on 56840 devices
 * 16 words
 */
struct dcb21_s {
#ifdef REMOVE
    uint32_t    addr;   /* T21.0: physical address */
    /* T21.1: Control 0 */
#ifdef  LE_HOST
    uint32_t    c_count:16,     /* Requested byte count */
                c_chain:1,      /* Chaining */
                c_sg:1,         /* Scatter Gather */
                c_reload:1,     /* Reload */
                c_hg:1,         /* Higig (TX) */
                c_stat:1,       /* Update stats (TX) */
                c_pause:1,      /* Pause packet (TX) */
                c_purge:1,      /* Purge packet (TX) */
    :           9;              /* Don't care */
#else
                uint32_t:9,     /* Don't care */
                c_purge:1,      /* Purge packet (TX) */
                c_pause:1,      /* Pause packet (TX) */
                c_stat:1,       /* Update stats (TX) */
                c_hg:1,         /* Higig (TX) */
                c_reload:1,     /* Reload */
                c_sg:1,         /* Scatter Gather */
                c_chain:1,      /* Chaining */
                c_count:16;     /* Requested byte count */
#endif /* LE_HOST */
#endif
    uint32_t    mh0;    /* T21.2: Module Header word 0 */
    uint32_t    mh1;    /* T21.3: Module Header word 1 */
    uint32_t    mh2;    /* T21.4: Module Header word 2 */
    uint32_t    mh3;    /* T21.5: Module Header word 3 */
#ifdef  LE_HOST
    /* T21.6 */
                uint32_t:3,     /* Reserved */
                mtp_index:5,    /* MTP index */
                cpu_cos:8,      /* Queue number for CPU packets */
                inner_vid:12,   /* Inner VLAN ID */
                inner_cfi:1,    /* Inner Canoncial Format Indicator */
                inner_pri:3;    /* Inner priority */

    /* T21.7 */
    uint32_t    reason_hi:16,   /* CPU opcode (high bits) */
                pkt_len:14,     /* Packet length */
    :           2;              /* Reserved */

    /* T21.8 */
    uint32_t    reason; /* CPU opcode (low bits) */

    /* T21.9 */
    uint32_t    dscp:6,         /* New DSCP */
    :           2,              /* Reserved */
                chg_tos:1,      /* DSCP has been changed by HW */
                decap_tunnel_type:4,    /* Tunnel type that was decapsulated */
                regen_crc:1,    /* Packet modified and needs new CRC */
    :           2,              /* Reserved */
                outer_vid:12,   /* Outer VLAN ID */
                outer_cfi:1,    /* Outer Canoncial Format Indicator */
                outer_pri:3;    /* Outer priority */

    /* T21.10 */
    uint32_t    timestamp;      /* Timestamp */

    /* T21.11 */
                uint32_t:12,    /* Reserved */
                ecn:2,          /* New ECN value */
                chg_ecn:1,      /* ECN changed */
                mcq:1,          /* Multicast queue indicator */
                vfi:12,         /* Internal VFI value */
                vfi_valid:1,    /* Validates VFI field */
                dvp_nhi_sel:1,  /* DVP overlay indicator */
                vntag_action:2; /* VN tag action */

    /* T21.12 */
    uint32_t    srcport:8,      /* Source port number */
                hgi:2,          /* Higig Interface Format Indicator */
                itag_status:2,  /* Ingress Inner tag status */
                otag_action:2,  /* Ingress Outer tag action */
                itag_action:2,  /* Ingress Inner tag action */
                service_tag:1,  /* SD tag present */
                switch_pkt:1,   /* Switched packet */
                hg_type:1,      /* 0: Higig+, 1: Higig2 */
                src_hg:1,       /* Source is Higig */
                l3routed:1,     /* Any IP routed packet */
                l3only:1,       /* L3 only IPMC packet */
                replicated:1,   /* Replicated copy */
    :           2,              /* Reserved */
                do_not_change_ttl:1,    /* Do not change TTL */
                bpdu:1,         /* BPDU Packet */
    :           5;              /* Reserved */

    /* T21.13 */
    uint32_t    uc_cos:4,       /* COS value for unicast packet */
                eh_seg_sel:3,   /* Extended Higig Seg Sel */
                next_pass_cp:2, /* Next pass code point */
                oam_pkt:1,      /* OAM packet */
    :           2,              /* Reserved */
                eh_tag_type:2,  /* Extended Higig tag type */
                eh_tm:1,        /* Extended Higig traffic mgr ctrl */
                eh_queue_tag:16,/* Extended Higig queue tag */
                hg2_ext_hdr:1;  /* Extended Higig2 header valid */

    /* T21.14 */
    uint32_t    l3_intf:14,     /* L3 Intf num / Next hop idx */
    :           2,              /* Reserved */
                match_rule:8,   /* Matched FP rule */
                mc_cos2:4,      /* COS2 value for multicast packets */
                mc_cos1:4;      /* COS1 value for multicast packets */

    /* T21.15: DMA Status 0 */
    uint32_t    count:16,       /* Transferred byte count */
                end:1,          /* End bit (RX) */
                start:1,        /* Start bit (RX) */
                error:1,        /* Cell Error (RX) */
    :           12,             /* Reserved */
                done:1;         /* Descriptor Done */
#else
    /* T21.6 */
    uint32_t    inner_pri:3,    /* Inner priority */
                inner_cfi:1,    /* Inner Canoncial Format Indicator */
                inner_vid:12,   /* Inner VLAN ID */
                cpu_cos:8,      /* Queue number for CPU packets */
                mtp_index:5,    /* MTP index */
    :           3;              /* Reserved */

    /* T21.7 */
                uint32_t:2,     /* Reserved */
                pkt_len:14,     /* Packet length */
                reason_hi:16;   /* CPU opcode (high bits) */

    /* T21.8 */
    uint32_t    reason;         /* CPU opcode (low bits) */

    /* T21.9 */
    uint32_t    outer_pri:3,    /* Outer priority */
                outer_cfi:1,    /* Outer Canoncial Format Indicator */
                outer_vid:12,   /* Outer VLAN ID */
    :           2,              /* Reserved */
                regen_crc:1,    /* Packet modified and needs new CRC */
                decap_tunnel_type:4,    /* Tunnel type that was decapsulated */
                chg_tos:1,      /* DSCP has been changed by HW */
    :           2,              /* Reserved */
                dscp:6;         /* New DSCP */

    /* T21.10 */
    uint32_t    timestamp;      /* Timestamp */

    /* T21.11 */
    uint32_t    vntag_action:2, /* VN tag action */
                dvp_nhi_sel:1,  /* DVP overlay indicator */
                vfi_valid:1,    /* Validates VFI field */
                vfi:12,         /* Internal VFI value */
                mcq:1,          /* Multicast queue indicator */
                chg_ecn:1,      /* ECN changed */
                ecn:2,          /* New ECN value */
    :           12,             /* Reserved */
      /* T21.12 */
                uint32_t:5,     /* Reserved */
                bpdu:1,         /* BPDU Packet */
                do_not_change_ttl:1,    /* Do not change TTL */
    :           2,              /* Reserved */
                replicated:1,   /* Replicated copy */
                l3only:1,       /* L3 only IPMC packet */
                l3routed:1,     /* Any IP routed packet */
                src_hg:1,       /* Source is Higig */
                hg_type:1,      /* 0: Higig+, 1: Higig2 */
                switch_pkt:1,   /* Switched packet */
                service_tag:1,  /* SD tag present */
                itag_action:2,  /* Ingress Inner tag action */
                otag_action:2,  /* Ingress Outer tag action */
                itag_status:2,  /* Ingress Inner tag status */
                hgi:2,          /* Higig Interface Format Indicator */
                srcport:8;      /* Source port number */

    /* T21.13 */
    uint32_t    hg2_ext_hdr:1,  /* Extended Higig2 header valid */
                eh_queue_tag:16,/* Extended Higig queue tag */
                eh_tm:1,        /* Extended Higig traffic mgr ctrl */
                eh_tag_type:2,  /* Extended Higig tag type */
    :           2,              /* Reserved */
                oam_pkt:1,      /* OAM packet */
                next_pass_cp:2, /* Next pass code point */
                eh_seg_sel:3,   /* Extended Higig Seg Sel */
                uc_cos:4;       /* COS value for unicast packet */

    /* T21.14 */
    uint32_t    mc_cos1:4,      /* COS1 value for multicast packets */
                mc_cos2:4,      /* COS2 value for multicast packets */
                match_rule:8,   /* Matched FP rule */
    :           2,              /* Reserved */
                l3_intf:14;     /* L3 Intf num / Next hop idx */

    /* T21.15: DMA Status 0 */
    uint32_t    done:1,     /* Descriptor Done */
    :           12,         /* Reserved */
                error:1,    /* Cell Error (RX) */
                start:1,    /* Start bit (RX) */
                end:1,      /* End bit (RX) */
                count:16;   /* Transferred byte count */
#endif
};

static void
unpackL2header(unsigned char const *const pkt, struct L2header *header)
{
    unsigned char const *vbuf;
    int         ix;

    vbuf = &pkt[0];
    for (ix = 0; ix < 6; ix++) {
        header->dest_mac[ix] = vbuf[ix];
    }
    vbuf = &pkt[6];
    for (ix = 0; ix < 6; ix++) {
        header->src_mac[ix] = vbuf[ix];
    }
    vbuf = &pkt[12];
    header->tag = (vbuf[0] << 24) | (vbuf[1] << 16) | (vbuf[2] << 8) | vbuf[3];
    vbuf = &pkt[16];
    header->ether_type = (vbuf[0] << 8) | vbuf[1];
}

static void
printL2header(unsigned char const *const pkt)
{
    struct L2header header;

    unpackL2header(pkt, &header);
    printf("\nL2 header:\n");
    printf("  Dst MAC:   %02x:%02x:%02x:%02x:%02x:%02x\n",
           header.dest_mac[0], header.dest_mac[1], header.dest_mac[2],
           header.dest_mac[3], header.dest_mac[4], header.dest_mac[5]);
    printf("  Src MAC:   %02x:%02x:%02x:%02x:%02x:%02x\n", header.src_mac[0],
           header.src_mac[1], header.src_mac[2], header.src_mac[3], header.src_mac[4],
           header.src_mac[5]);
    printf("  Tag:       0x%08x\n", header.tag);
    printf("  EtherType: 0x%04x\n", header.ether_type);
}

static void
unpackRCPUheader(unsigned char const *const pkt, struct RCPUheader *header)
{
    unsigned char const *vbuf;

    vbuf = &pkt[18];

    header->signature = (vbuf[0] << 8) | vbuf[1];
    header->opcode = vbuf[2];
    header->flags = vbuf[3];
    header->trans_id = (vbuf[4] << 8) | vbuf[5];
    header->data_len = (vbuf[6] << 8) | vbuf[7];
    header->reply_len = (vbuf[8] << 8) | vbuf[9];
    header->reserved = (vbuf[10] << 24) | (vbuf[11] << 16) | (vbuf[12] << 8) | vbuf[13];
}

static void
printRCPUheader(unsigned char const *const pkt)
{
    struct RCPUheader header;

    unpackRCPUheader(pkt, &header);

    printf("\nRCPU Header:\n");
    printf("  Signature: 0x%04x; %s\n", header.signature,
           (header.signature == RCPU_SIGNATURE) ? "OK" : "INVALID");
    printf("  Opcode:    0x%02x\n", header.opcode);
    printf("  Flags:     0x%02x\n", header.flags);
    printf("  Trans ID:  0x%04x\n", header.trans_id);
    printf("  Data len:  %d\n", header.data_len);
    printf("  Reply len: %d\n", header.reply_len);
    printf("  Reserved:  0x%08x\n", header.reserved);
}

static void
printMetaData(unsigned char *pkt)
{
    uint32_t   *wbuf;
    int         ix;
    struct dcb21_s *dcbHack;

    /*
     * The Rx meta data is essentially the architecture specific DCB.
     * We skip the first 8 bytes since that information isn't very useful.
     * This means the metadata for may differ from one device type to another.
     */

    printf("\nRCPU Meta Data:\n");
    wbuf = (uint32_t *) & pkt[0];
#if 0
    dcbHack = (struct dcb21_s *) &pkt[32];
    printf("DCB: Packet Len: %d;\n", dcbHack->pkt_len);
    printf("DCB: Reason Hi: %X; Low: %X\n", dcbHack->reason_hi, dcbHack->reason);
    printf("DCB: Outer VLAN: %d; Outer CFI: %d; Outer Priority: %d\n",
           dcbHack->outer_vid, dcbHack->outer_cfi, dcbHack->outer_pri);
    printf("DCB: BPDU Packet: %d\n", dcbHack->bpdu);
    printf("DCB: Do not change TTL: %d\n", dcbHack->do_not_change_ttl);
    printf("DCB: Replicated copy: %d\n", dcbHack->replicated);
    printf("DCB: L3 only IPMC packet: %d\n", dcbHack->l3only);
    printf("DCB: Any IP routed packet: %d\n", dcbHack->l3routed);
    printf("DCB: Source is Higig: %d\n", dcbHack->src_hg);
    printf("DCB: 0: Higig+, 1: Higig2: %d\n", dcbHack->hg_type);
    printf("DCB: Switched packet: %d\n", dcbHack->switch_pkt);
    printf("DCB: SD tag present: %d\n", dcbHack->service_tag);
    printf("DCB: Ingress Inner tag action: %d\n", dcbHack->itag_action);
    printf("DCB: Ingress Outer tag action: %d\n", dcbHack->otag_action);
    printf("DCB: Ingress Inner tag status: %d\n", dcbHack->itag_status);
    printf("DCB: Higig Interface Format Indicator: %d\n", dcbHack->hgi);
    printf("DCB: Source port number: %d\n", dcbHack->srcport);
#endif

    for (ix = 0; ix < 16; ix++) {
        if ((ix & 1) == 0) {
            printf("  Word %2d: 0x%08X", ix, wbuf[ix]);
        } else {
            printf("; 0x%08X\n", wbuf[ix]);
        }
    }
}

static void
dump_raw(uint8_t * rx_packet, int rx_len)
{
    int         ix;

    printf("\nRaw Packet Dump (%d bytes):\n", rx_len);
    for (ix = 0; ix < rx_len; ix++) {
        if ((ix & 0xf) == 0) {
            printf("0x%04x:", ix);
        } else if ((ix & 0xf) == 8) {
            printf(" -");
        }
        printf(" 0x%02x", rx_packet[ix]);
        if ((ix & 0xf) == 0xf) {
            printf("\n");
        }
    }
    if ((ix & 0xf) != 0) {
        printf("\n");
    }
}

static int
create_raw_socket(int protocol_to_sniff)
{
    int         rawsock;

    if ((rawsock = socket(PF_PACKET, SOCK_RAW, htons(protocol_to_sniff))) < 0) {
        perror("Error creating raw socket: ");
        return rawsock;
    }

    return rawsock;
}

static int
bind_raw_socket(char *device, int rawsock, int protocol)
{

    struct sockaddr_ll sll;
    struct ifreq ifr;

    bzero(&sll, sizeof(sll));
    bzero(&ifr, sizeof(ifr));

    /* First Get the Interface Index  */
    strncpy((char *) ifr.ifr_name, device, IFNAMSIZ);
    if ((ioctl(rawsock, SIOCGIFINDEX, &ifr)) == -1) {
        printf("Error getting interface index for %s\n", device);
        return -1;
    }

    /* Bind our raw socket to this interface */
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(protocol);

    if ((bind(rawsock, (struct sockaddr *) &sll, sizeof(sll))) == -1) {
        perror("Error binding raw socket to interface\n");
        return -1;
    }

    return 0;
}

void
dump_rcpu_encap(unsigned char *pkt)
{
    printL2header(pkt);
    printRCPUheader(pkt);
    printMetaData(pkt);
}

static int  rawsock = -1;
static int  rx_packets = 0;
static char *ifname = NULL;

static void
sigint(int signum)
{
    /*Clean up....... */

    struct ifreq ifr;

    if ((rawsock == -1) || (ifname == NULL)) {
        return;
    }

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ioctl(rawsock, SIOCGIFFLAGS, &ifr);
    ifr.ifr_flags &= ~IFF_PROMISC;
    ioctl(rawsock, SIOCSIFFLAGS, &ifr);
    close(rawsock);

    printf("RX terminating ...\n");

    printf("Total packets received: %d\n", rx_packets);
    exit(0);
}

static void
usage(const char *const prog)
{
    printf("Usage %s [-v] [-vv] interface [maxRxPackets]\n", prog);
    printf("-r            ; RCPU mode\n");
    printf("-v            ; Be verbose\n");
    printf("-vv           ; Be very verbose\n");
    printf("interface     ; ethernet interface (eth5)\n");
    printf("maxRxPackets  ; Exit after receiving this many packets\n");
}

int
main(int argc, char **argv)
{
    int         rcpu = 0;
    int         ax;
    int         max_packets = -1;
    int         rx_len;
    int         verbose = 0;
    int         vverbose = 0;
    socklen_t   len;
    struct sockaddr_ll skaddr;
    uint8_t     rx_packet[RX_MAX];

    for (ax = 1; ax < argc; ax++) {
        if (argv[ax][0] == '-') {
            char       *flag = &argv[ax][1];

            if (strcmp(flag, "v") == 0) {
                verbose = 1;
            } else if (strcmp(flag, "vv") == 0) {
                verbose = 1;
                vverbose = 1;
            } else if (strcmp(flag, "r") == 0) {
                rcpu = 1;
            } else {
                fprintf(stderr, "ERROR: unknown switch %s\n", argv[ax]);
                usage(argv[0]);
                return -2;
            }
        } else {
            if (ifname == NULL) {
                ifname = argv[ax];
            } else if (max_packets < 0) {
                max_packets = atoi(argv[ax]);
                if (max_packets == 0) {
                    printf("Bogus packet count\n");
                    return -2;
                }
            } else {
                fprintf(stderr, "ERROR: unknown argument %s\n", argv[ax]);
                usage(argv[0]);
                return -2;
            }
        }
    }

    if (ifname == NULL) {
        usage(argv[0]);
        return -1;
    }

    /*establish signal handler */
    if (SIG_ERR == signal(SIGINT, sigint)) {
        return 2;
    }

    if (max_packets <= 0) {
        max_packets = 1000000;
    }

    /* Create the raw socket */
    rawsock = create_raw_socket(ETH_P_ALL);

    if (rawsock < 0) {
        return 1;
    }

    /* Bind raw socket to interface */
    if (bind_raw_socket(ifname, rawsock, ETH_P_ALL) < 0) {
        return 2;
    }

    rx_packets = 0;
    while (rx_packets < max_packets) {

        if (vverbose) {
            printf("\nWaiting on packet %d\n", rx_packets + 1);
        }

        memset(&skaddr, 0, sizeof(skaddr));
        skaddr.sll_protocol = htons(ETH_P_ALL);
        len = sizeof(struct sockaddr);

        if ((rx_len = recvfrom(rawsock, rx_packet, RX_MAX, 0,
                               (struct sockaddr *) &skaddr, &len)) > 0) {
            int         packet_type = ntohs(skaddr.sll_pkttype);

            if (skaddr.sll_pkttype == PACKET_OUTGOING) {
                if (vverbose) {
                    printf("Skipping %d byte outgoing packet;\n", rx_len);
                }
                continue;
            }
            rx_packets++;
            if (verbose) {
                printf
                  ("\n--------------------------------------------------------------------------------\n");
            }
            if (verbose) {
                printf("Received packet %d (%d bytes)\n", rx_packets, rx_len);
                #if 0
                if (vverbose) {
                    static const char *packet_types[] =
                      { "HOST", "BROADCAST", "MULTICAST", "OTHERHOST", "OUTGOING",
                        "LOOPBACK", "FASTROUTE"
                    };
                    printf("    Interface Index = %d; Packet Type = %s\n",
                           skaddr.sll_ifindex, packet_types[packet_type]);
                }
		#endif
		if (rcpu) {
                    dump_rcpu_encap(rx_packet);
                } else {
                    printf("Dump L2 Header\n");
                    printL2header(rx_packet);
                }
                if (vverbose) {
                    printf("Dump Raw Packet\n");
                    dump_raw(rx_packet, rx_len);
                }
            } else {
                if ((rx_packets & 0x1FFF) == 0) {
                    printf("Rx Packet Count: %8d\n", rx_packets);
                }
            }
        }
        if (rx_len < 0) {
            perror("recvfrom()");
        }
    }
    printf("Received %d packets, DONE\n", max_packets);

    close(rawsock);

    return 0;
}


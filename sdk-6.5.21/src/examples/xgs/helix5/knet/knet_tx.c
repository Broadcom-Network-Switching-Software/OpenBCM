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

#define SRC_ETHER_ADDR "10:11:12:13:14:15"
#define DST_ETHER_ADDR "00:01:02:03:04:05"

#define RCPU_SIGNATURE 0xB840
#define TX_MAX         1514     /* Assuming no jumbo */

#define INT_MAX 10

#define L2HEADER_SIZE 18
/* Starts at packet byte 0 */
struct L2header {
    uint8_t     dest_mac[6];
    uint8_t     src_mac[6];
    uint32_t    tag;
    uint16_t    ether_type;
};

#define RCPUHEADER_SIZE 14
/* Starts at packet byte 18 */
struct RCPUheader {
    uint16_t    signature;
    uint8_t     opcode;
    uint8_t     flags;
    uint16_t    trans_id;
    uint16_t    data_len;
    uint16_t    reply_len;
    uint32_t    reserved;
};

#define RCPUMETADATA_SIZE 32

/*
 * The RCPU Metadata is hardware/chip specific. This example was derived
 * from soc_pbsmh_v5_hdr_t in "include/soc/pgsmh.h". Not all fields have
 * been tested with this particular utility.
 */
struct RCPUmetadata {
                              /* Byte Position */
    uint32_t start:8;         /* 0  Start of frame indicator (must be set to 0xFF for SOBMH packets). */
    uint32_t _rsvd0:24;       /* 1-3 */
    uint32_t ipcf_ptr:8;      /* 4 Pointer to cell buffer of this SOBMH cell. (Set by hardware) */
    uint32_t spid_override:1; /* 5.0 For PBI.SPID_Override */
    uint32_t _rsvd2:3;        /* 5.1-3 */
    uint32_t tx_ts:1;         /* 5.4 Indicates for TS packet transmitted from CPU into IP that the outgoing packet needs to have its transmit timestamp captured by the port." */
    uint32_t _rsvd1:3;        /* 5.5-7 */
    uint32_t unicast:1;       /* 6.0 Indicates that PBI.UNICAST should be set to queue as unicast packet */
    uint32_t set_l2bm:1;      /* 6.1 Indicates that PBI.L2_BITMAP should be set (to queue as L2MC packet) */
    uint32_t set_l3bm:1;      /* 6.2 Indicates that PBI.L3_BITMAP should be set (to queue as IPMC packet). */
    uint32_t _rsvd3:1;        /* 6.3 */
    uint32_t spap:2;          /* 6.4-5 For PBI.SPAP - Service Pool Priority (color) */
    uint32_t spid:2;          /* 6.6-7 For PBI.SPID - Service Pool ID */
    uint32_t src_mod:8;       /* 7 Source module ID, must be programmed to MY_MODID */
    uint32_t input_pri:4;     /* 8.0-3 Traffic priority to be applied to MMU via PBI.INPUT_PRIORITY. */
    uint32_t _rsvd4:4;        /* 8.4-7 */
    uint32_t queue_num:7;     /* 9.0-6 Queue number to be used for unicast queuing and CPU queue (CPU_COS). */
    uint32_t _rsvd5:1;        /* 9.7 */
    uint32_t cos:4;           /* 10.0-3 Class of service for MMU queueing for this packet - sets COS values, PBI.UC_COS, PBI.MC_COS1, and PBI.MC_COS2. */
    uint32_t _rsvd6:4;        /* 10.4-7 */
    uint32_t dst_port:7;      /* 11.0-6 Indicates the local port to send a SOBMH packet out. */
    uint32_t _rsvd7:1;        /* 11.7 */
};

#define RCPUENCAP_SIZE (L2HEADER_SIZE + RCPUHEADER_SIZE + RCPUMETADATA_SIZE)

#define MAX_PAYLOAD    (TX_MAX - sizeof(struct ethhdr) - RCPUENCAP_SIZE)
#define MIN_PAYLOAD    (50)

static void
packL2header(uint8_t * const pkt, struct L2header *header)
{
    uint8_t    *vbuf;
    int         ix;

    vbuf = &pkt[0];
    for (ix = 0; ix < 6; ix++) {
        vbuf[ix] = header->dest_mac[ix];
    }
    vbuf = &pkt[6];
    for (ix = 0; ix < 6; ix++) {
        vbuf[ix] = header->src_mac[ix];
    }
    vbuf = &pkt[12];
    vbuf[0] = (header->tag >> 24) & 0xFF;
    vbuf[1] = (header->tag >> 16) & 0xFF;
    vbuf[2] = (header->tag >> 8) & 0xFF;
    vbuf[3] = header->tag & 0xFF;
    vbuf = &pkt[16];
    vbuf[0] = (header->ether_type >> 8) & 0xFF;
    vbuf[1] = header->ether_type & 0xFF;
}

static void
packRCPUheader(uint8_t * const pkt, struct RCPUheader *header)
{
    uint8_t    *vbuf;

    vbuf = &pkt[18];

    vbuf[0] = (header->signature >> 8) & 0xFF;
    vbuf[1] = header->signature & 0xFF;
    vbuf[2] = header->opcode;
    vbuf[3] = header->flags;
    vbuf[4] = (header->trans_id >> 8) & 0xFF;
    vbuf[5] = header->trans_id & 0xFF;
    vbuf[6] = (header->data_len >> 8) & 0xFF;
    vbuf[7] = header->data_len & 0xFF;
    vbuf[8] = (header->reply_len >> 8) & 0xFF;
    vbuf[9] = header->reply_len & 0xFF;
    vbuf[10] = (header->reserved >> 24) & 0xFF;
    vbuf[11] = (header->reserved >> 16) & 0xFF;
    vbuf[12] = (header->reserved >> 8) & 0xFF;
    vbuf[13] = header->reserved & 0xFF;
}

static void
packMetadata(uint8_t * const pkt, struct RCPUmetadata *metadata)
{
    uint8_t    *vbuf;

    vbuf = &pkt[L2HEADER_SIZE + RCPUHEADER_SIZE];
    /* Add meta data */
    memset(vbuf, 0, RCPUMETADATA_SIZE);

    vbuf[0]  =  metadata->start;         /* 0 */
    vbuf[4]  =  metadata->ipcf_ptr;      /* 4 */
    vbuf[5]  =  metadata->spid_override; /* 5.0 */
    vbuf[5] |=  metadata->tx_ts << 4;    /* 5.4 */
    vbuf[6]  =  metadata->unicast;       /* 6.0 */
    vbuf[6] |=  metadata->set_l2bm << 1; /* 6.1 */
    vbuf[6] |=  metadata->set_l3bm << 2; /* 6.2 */
    vbuf[6] |=  metadata->spap << 4;     /* 6.4-5 */
    vbuf[6] |=  metadata->spid << 6;     /* 6.6-7 */
    vbuf[7]  =  metadata->src_mod;       /* 7 */
    vbuf[8]  =  metadata->input_pri;     /* 8 */
    vbuf[9]  =  metadata->queue_num;     /* 9 */
    vbuf[10] =  metadata->cos;           /* 10 */
    vbuf[11] =  metadata->dst_port;      /* 11 */
}

static void
buildEthernetHeader(uint8_t const *const pkt, char *src_mac, char *dst_mac,
                    int protocol)
{
    struct ethhdr *ethernet_header = (struct ethhdr *) pkt;

    /* copy the Src mac addr */
    memcpy(ethernet_header->h_source, (void *) ether_aton(src_mac), 6);

    /* copy the Dst mac addr */
    memcpy(ethernet_header->h_dest, (void *) ether_aton(dst_mac), 6);

    /* copy the protocol */
    ethernet_header->h_proto = htons(protocol);
}

static void
buildL2Header(struct L2header *const l2header, const char *const src_mac,
              const char *const dst_mac, const int tag, const int ether_type)
{
    memcpy(l2header->dest_mac, (void *) ether_aton(DST_ETHER_ADDR), 6);
    memcpy(l2header->src_mac, (void *) ether_aton(SRC_ETHER_ADDR), 6);

    l2header->tag = tag;
    l2header->ether_type = ether_type;
}

static void
buildRCPUheader(struct RCPUheader *const rcpuHeader, const int signature,
                const int opcode, const int flags, const int trans_id,
                const int data_len, const int reply_len, const int reserved)
{
    rcpuHeader->signature = signature;
    rcpuHeader->opcode = opcode;
    rcpuHeader->flags = flags;
    rcpuHeader->trans_id = trans_id;
    rcpuHeader->data_len = data_len;
    rcpuHeader->reply_len = reply_len;
    rcpuHeader->reserved = reserved;
}

static void
buildRCPUmetadata(struct RCPUmetadata *metadata, int port)
{
    memset(metadata, 0, sizeof(struct RCPUmetadata));

    metadata->start = 0xFF;     /* set SOBMH start */
    metadata->unicast = 1;      /* set SOBMH: unicast */
    metadata->dst_port = port;  /* SOBMH: dst_port */
}

static void
relocateEthernetHeader(uint8_t * const pkt, const int payload_len)
{
    int         ix;

    /* Move Ethernet header to encapsulated frame */
    memcpy(&pkt[RCPUENCAP_SIZE], pkt, 12);
    ix = RCPUENCAP_SIZE + 12;
    /* Add VLAN tag */
    pkt[ix++] = 0x81;
    pkt[ix++] = 0x00;
    pkt[ix++] = 0x00;
    pkt[ix++] = 0x01;
    /* Add length */
    pkt[ix++] = payload_len >> 8;
    pkt[ix++] = payload_len & 0xff;
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

int
send_raw_packet(int rawsock, unsigned char *pkt, int pkt_len)
{
    int         sent = 0;

    /* A simple write on the socket ..thats all it takes ! */
    if ((sent = write(rawsock, pkt, pkt_len)) != pkt_len) {
        /* Error */
        if (sent < 0) {
            printf("Send (%d bytes) failed\n", pkt_len);
            return 1;
        } else {
            printf("Could only send %d bytes of packet of length %d\n", sent, pkt_len);
            return 2;
        }
    }
    return 0;
}

static void
dump_packet(unsigned char *pkt, int len)
{
    int         ix;

    printf("Dump Packet: %d bytes\n", len);
    for (ix = 0; ix < len; ix++) {
        if ((ix & 0xf) == 0) {
            printf("%04x: ", ix);
        } else if ((ix & 0xf) == 8) {
            printf("- ");
        }
        printf("%02x ", pkt[ix]);
        if ((ix & 0xf) == 0xf) {
            printf("\n");
        }
    }
    if ((ix & 0xf) != 0) {
        printf("\n");
    }
}

static int  rawsock = -1;
static int  packet_count = 0;
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

    printf("TX terminating ...\n");

    printf("Total packets transmitted: %d\n", packet_count);
    exit(0);
}

static void
usage(const char *const prog)
{
    printf
      ("Usage %s [-v] [-vv] [-p port] [-ipd delay] [-s size] interface [maxTxPackets]\n",
       prog);
    printf("-r            ; Interface is in RCPU mode\n");
    printf("-p <port>     ; Tx on specified port\n");
    printf("-ipd <delay>  ; inter packet delay (usecs)\n");
    printf("-s <psize>    ; Payload size in bytes or MIN, MAX, RAND\n");
    printf("-v            ; Be verbose\n");
    printf("-vv           ; Be very verbose\n");
    printf("interface     ; ethernet interface (eth5)\n");
    printf("maxTxPackets  ; Transmit specified number of packets\n");
}

int
main(int argc, char **argv)
{
    int         ax;
    int         inter_packet_delay = 0;
    int         max_packets = -1;
    int         payload_len = 110;
    int         port = 1;
    int         random_packet_sizes = 0;
    int         rcpu = 0;
    int         rcpu_len = 0;
    int         tag_len;
    int         tx_len;
    int         verbose = 0;
    int         vverbose = 0;
    uint8_t     tx_packet[TX_MAX];

    memset(tx_packet, 0xA5, sizeof(tx_packet));
    for (ax = 1; ax < argc; ax++) {
        if (argv[ax][0] == '-') {
            char       *flag = &argv[ax][1];

            if (strcmp(flag, "v") == 0) {
                verbose = 1;
            } else if (strcmp(flag, "vv") == 0) {
                vverbose = 1;
                verbose = 1;
            } else if (strcmp(flag, "r") == 0) {
                rcpu = 1;
            } else if (strcmp(flag, "ipd") == 0) {
                ax++;
                if (ax == argc) {
                    fprintf(stderr, "Missing inter packet delay\n");
                    usage(argv[0]);
                    return -2;
                }
                if (argv[ax] != NULL) {
                    char       *end;

                    inter_packet_delay = strtol(argv[ax], &end, 10);
                    if ((*end != 0) || (inter_packet_delay < 0)
                        || (inter_packet_delay > 1000000)) {
                        fprintf(stderr, "Invalid inter packet delay: %s\n", argv[ax]);
                        usage(argv[0]);
                        return -2;
                    }
                }
            } else if (strcmp(flag, "p") == 0) {
                ax++;
                if (ax == argc) {
                    fprintf(stderr, "Missing port\n");
                    usage(argv[0]);
                    return -2;
                }
                if (argv[ax] != NULL) {
                    char       *end;

                    port = strtol(argv[ax], &end, 10);
                    if ((*end != 0) || (port < 0) || (port > 128)) {
                        fprintf(stderr, "Invalid port: %s\n", argv[ax]);
                        usage(argv[0]);
                        return -2;
                    }
                }
            } else if (strcmp(flag, "s") == 0) {
                ax++;
                if (ax == argc) {
                    fprintf(stderr, "Missing payload size\n");
                    usage(argv[0]);
                    return -2;
                }
                if (argv[ax] != NULL) {
                    if (strcmp(argv[ax], "MAX") == 0) {
                        payload_len = MAX_PAYLOAD;
                    } else if (strcmp(argv[ax], "MIN") == 0) {
                        payload_len = MIN_PAYLOAD;
                    } else if (strcmp(argv[ax], "RAND") == 0) {
                        random_packet_sizes = 1;
                        srandom(MAX_PAYLOAD);   /* Any number will do */
                        payload_len = MAX_PAYLOAD;
                    } else {
                        char       *end;

                        payload_len = strtol(argv[ax], &end, 10);
                        if ((*end != 0) || (payload_len < MIN_PAYLOAD)
                            || (payload_len > MAX_PAYLOAD)) {
                            fprintf(stderr, "Invalid payload size: %s\n", argv[ax]);
                            usage(argv[0]);
                            return -2;
                        }
                    }
                }
            } else {
                fprintf(stderr, "ERROR: unknown switch %s\n", argv[ax]);
                usage(argv[0]);
                return -2;
            }
        } else {
            if (ifname == NULL) {
                ifname = argv[ax];
            } else if (max_packets < 0) {
                if (strcmp(argv[ax], "MAX") == 0) {
                    max_packets = INT_MAX;
                } else {
                    char       *end;

                    max_packets = strtol(argv[ax], &end, 10);
                    if ((*end != 0) || (max_packets <= 0) || (max_packets > INT_MAX)) {
                        fprintf(stderr, "Invalid packet count: %s\n", argv[ax]);
                        usage(argv[0]);
                        return -2;
                    }
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
        max_packets = 100;
    }

    /* RCPU encapsulation */
    rcpu_len = rcpu ? RCPUENCAP_SIZE : 0;

    tag_len = rcpu ? 4 : 0;

    tx_len = sizeof(struct ethhdr) + payload_len;

    /* Create the raw socket */
    rawsock = create_raw_socket(ETH_P_ALL);

    if (rawsock < 0) {
        return 1;
    }

    /* Bind raw socket to interface */
    if (bind_raw_socket(ifname, rawsock, ETH_P_ALL) < 0) {
        return 2;
    }

    buildEthernetHeader(tx_packet, SRC_ETHER_ADDR, DST_ETHER_ADDR,
                        payload_len + rcpu_len);

    if (rcpu) {
        struct L2header l2Header;
        struct RCPUheader rcpuHeader;
        struct RCPUmetadata metadata;

        relocateEthernetHeader(tx_packet, payload_len);
        buildL2Header(&l2Header, SRC_ETHER_ADDR, DST_ETHER_ADDR, 0x81000001, 0xDE08);
        buildRCPUheader(&rcpuHeader, RCPU_SIGNATURE, 0x20, (port >= 0) ? 0x04 : 0x00, 0,
                        tx_len, 0, 0);
        buildRCPUmetadata(&metadata, port);

        packL2header(tx_packet, &l2Header);
        packRCPUheader(tx_packet, &rcpuHeader);
        packMetadata(tx_packet, &metadata);

        tx_len += RCPUENCAP_SIZE;
    }
    if (verbose) {
        dump_packet(tx_packet, tx_len);
    }
    if (verbose) {
        printf("Transmitting %d %d byte packets; payload len=%d; port=%d\n",
               max_packets, tx_len, payload_len, port);
    }
    for (packet_count = 0; packet_count < max_packets; packet_count++) {
        int         this_tx_len =
          random_packet_sizes ? (tx_len - ((random() % (MAX_PAYLOAD - MIN_PAYLOAD)) +
                                           MIN_PAYLOAD)) : tx_len;
        tx_packet[22 + tag_len + rcpu_len] = 0x80 + (packet_count & 0x7f);
        if ((send_raw_packet(rawsock, tx_packet, this_tx_len) == 0) && (vverbose)) {
            printf("Packet #%d (%d bytes) sent successfully\n", packet_count + 1,
                   this_tx_len);
        }
        if (inter_packet_delay) {
            usleep(inter_packet_delay);
        }
    }
    printf("Transmitted %d packets, DONE\n", max_packets);

    close(rawsock);

    return 0;
}

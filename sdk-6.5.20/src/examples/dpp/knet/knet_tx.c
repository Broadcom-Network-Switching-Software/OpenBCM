/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *File: knet_tx.c
 * Purpose: This CINT script is a program used to send packets over KNET interface.
 *
 *    Prerequistes:
 *    =============
 *      a) Build SDK with KNET feature enabled
 *      b) Insert KNET kernel module before launching SDK.
 *      c) Requried SOC proterties:
 *          tm_port_header_type_in_200.0=INJECTED_2_PP
 *
 *    Steps:
 *    =============
 *      a) Build $SDK/src/examples/dpp/knet/knet_tx.c for customer target CPU:
 *          gcc knet_tx.c -o knet_tx
 *      b) Create KNET interface refering to $SDK/src/examples/dpp/knet/knet.c
 *      c) Go to Linux shell prompt using "shell" command from BCM diag shell
 *      d) Configure IP address to knet interface, for example:
 *          ifconfig virt-intf0 192.168.2.254 netmask 255.255.255.0 up
 *      e) Run knet_rx in backround, please see $SDK/src/examples/dpp/knet/knet_rx.c
 *      e) Run knet_tx to send packets.
 *          if KNET interface is in CPU Ingress mode, Run knet_tx "./knet_tx -vv virt-intf0 5"
 *          if KNET interface is in Local Port mode, Run knet_tx "./knet_tx -vv virt-intf0 5"
 *          if KNET interface is in RCPU mode, Run knet_tx "./knet_tx -vv virt-intf0 5 -r -m RxPort -ssp 200 -channel 8"
 *      f) Check dump from knet_rx
 *
 *    Usage of options:
 *    =============
 *      Usage knet_tx [-v] [-vv] [-dest port] [-ssp ssp] [-channel chan] [-ipd delay] [-s size] interface [maxTxPackets]
 *      -r            ; RCPU mode
 *      -v            ; Be verbose
 *      -vv           ; Be very verbose
 *      interface     ; ethernet interface (eth5)
 *      maxRxPackets  ; Exit after receiving this many packets
 *      Usage knet_tx [-v] [-vv] [-dest port] [-ssp ssp] [-channel chan] [-ipd delay] [-s size] interface [maxTxPackets]
 *      -r            ; Interface is in RCPU mode
 *      -m            ; Specify injection mode
 *      -dest <port>  ; Tx on specified port
 *      -ssp <ssp>    ; Specify source system port when tx through ingress logic
 *      -channel <chan>   ; Specify ingress CPU channel
 *      -ipd <delay>  ; inter packet delay (usecs)*
 *      -s <psize>    ; Payload size in bytes or MIN, MAX, RAND
 *      -v            ; Be verbose
 *      -vv           ; Be very verbose
 *      interface     ; ethernet interface (eth5)
 *      maxTxPackets  ; Transmit specified number of packets
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

/* Define RCPU signature per Device, 0x8670 for Jericho by default */
#define RCPU_SIGNATURE 0x8670
/* Define Extension of ITMH Base Ext header, not defined by default */
/* #define ITMH_BASE_EXT_EN */
/* Define Extension of ITMH Base Ext header additional bytes, not defined by default */
/* #define ITMH_BASE_EXT_ADDITIONAL_BYTES_EN */
/* Define type of ITMH Base Ext header when additional bytes exists, 6 by default */
#define ITMH_BASE_EXT_ADDITIONAL_BYTES_TYPE 0x6
/* ITMH PMF Extension existence is configurable per In-PP-Port, Not defined  by default */
/* #define ITMH_PMF_EXT_EN */

#define SRC_ETHER_ADDR "10:11:12:13:14:15"
#define DST_ETHER_ADDR "00:01:02:03:04:05"
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

/* No meta data in TX direction */
#define RCPUMETADATA_SIZE 0

/* Total length of RCPU encapsulation */
#define RCPUENCAP_SIZE (L2HEADER_SIZE + RCPUHEADER_SIZE + RCPUMETADATA_SIZE)

/* Port Termination Control Header */
#define PTCH_SIZE 2
/* Starts at packet byte 32 */
struct PTCH {
    uint8_t     parser_program_control;
    uint8_t     opaque_pt_attributes;
    uint8_t     reserved;
    uint16_t    ssp;
};

/* ITMH */
#define ITMH_BASE_SIZE 4
#define ITMH_BASE_EXT_SIZE 3
#define ITMH_BASE_EXT_ADDITIONAL_BTYES_SIZE 3
#define ITMH_PMF_EXT_SIZE 3
/* Starts at packet byte 34 */
struct ITMH {
    uint8_t     pph_type;
    uint8_t     in_mirror_disable;
    uint8_t     fwd_dp;
    uint32_t    fwd_destnation_info;
    uint8_t     snoop_cmd;
    uint8_t     fwd_traffic_class;
    uint8_t     itmh_base_ext_exist;
    uint8_t     itmh_base_ext_type;
    uint8_t     itmh_base_ext_reserved;
    uint32_t    itmh_base_ext_value1;
    uint32_t    itmh_base_ext_additional_bytes; /* 3 additional bytes if ITMH-Base-Extension.Type = 0x6/0x7 */
    uint32_t    itmh_pmf_extension;
};

/* PPH */
#define PPH_BASE_SIZE 7
/* Starts at packet byte 34 + ITMH length */
struct PPHBase {
    uint8_t     ext2_valid;
    uint8_t     ext3_valid;
    uint8_t     field1;
    uint8_t     reserved1;
    uint8_t     oam_offset;
    uint32_t    reserved2;
    uint32_t    reserved3;
};

/* PPH Ext 1*/
#define PPH_EXT1_SIZE 3
struct PPHExt1 {
    uint16_t    oam_id;
    uint8_t     trap_code;
};

/* PPH Ext 2*/
#define PPH_EXT2_SIZE 3
struct PPHExt2 {
    uint32_t    reserved;
};


/* PPH Ext 3*/
#define PPH_EXT3_SIZE 5
struct PPHExt3 {
    uint32_t    reserved1;
    uint32_t    reserved2;
};

#define MAX_PAYLOAD    (TX_MAX - sizeof(struct ethhdr) - RCPUENCAP_SIZE)
#define MIN_PAYLOAD    (50)

#define RX_PORT_INJECTION 1
#define TX_WIRE_INJECTION 2
#define EGRESS_PP_INJECTION 3

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
packPTCH(uint8_t * const pkt, struct PTCH *ptch)
{
    uint8_t    *vbuf;

    vbuf = &pkt[L2HEADER_SIZE + RCPUHEADER_SIZE];
    memset(vbuf, 0, PTCH_SIZE);

    vbuf[0]  =  (ptch->parser_program_control & 0x1) << 7;
    vbuf[0]  |=  (ptch->opaque_pt_attributes & 0x7) << 4;
    vbuf[0]  |=  ptch->reserved & 0xf;
    vbuf[1]  =  ptch->ssp & 0xff;
}

static int
packITMH(uint8_t * const pkt, struct ITMH *itmh)
{
    uint8_t    *vbuf;
    uint32_t   destination_system_port_agr = 0;
    int        length = 0;

    vbuf = &pkt[L2HEADER_SIZE + RCPUHEADER_SIZE + PTCH_SIZE];
    memset(vbuf, 0, ITMH_BASE_SIZE);

    vbuf[0]  =  (itmh->pph_type & 0x3) << 6;
    vbuf[0]  |=  (itmh->in_mirror_disable & 0x1)<< 5;
    vbuf[0]  |=  (itmh->fwd_dp & 0x3)<< 3;
    /*
     * Encode destination, only SPA format is supported currently
     * The format is 0x1(3) + SPA(16)
     */
    destination_system_port_agr = (0x1 << 16) | (itmh->fwd_destnation_info & 0xffff);
    vbuf[0]  |= (destination_system_port_agr >> 16) & 0x7;
    vbuf[1]  =  (destination_system_port_agr >> 8) & 0xff;
    vbuf[2]  =  destination_system_port_agr & 0xff;
    vbuf[3]  =  (itmh->snoop_cmd & 0xf) << 4;
    vbuf[3]  |= (itmh->fwd_traffic_class & 0x7) << 1;
    vbuf[3]  |= itmh->itmh_base_ext_exist & 0x1;

    length += ITMH_BASE_SIZE;
#ifdef ITMH_BASE_EXT_EN
    if (itmh->itmh_base_ext_exist) {
        vbuf = &pkt[L2HEADER_SIZE + RCPUHEADER_SIZE + PTCH_SIZE + ITMH_BASE_EXT_SIZE];
        memset(vbuf, 0, ITMH_BASE_EXT_SIZE);
        vbuf[0]  =  (itmh->itmh_base_ext_type & 0x7) << 5;
        vbuf[1]  =  (itmh->itmh_base_ext_reserved & 0x1) << 4;
        vbuf[1]  |=  (itmh->itmh_base_ext_value1 >> 16) & 0xf;
        vbuf[2]  =  (itmh->itmh_base_ext_value1 >> 8) & 0xff;
        vbuf[3]  =  itmh->itmh_base_ext_value1 & 0xff;
        length += ITMH_BASE_EXT_SIZE;
#ifdef ITMH_BASE_EXT_ADDITIONAL_BYTES_EN
        if(itmh->itmh_base_ext_type == ITMH_BASE_EXT_ADDITIONAL_BYTES_TYPE) {
            vbuf = &pkt[L2HEADER_SIZE + RCPUHEADER_SIZE + PTCH_SIZE + ITMH_BASE_EXT_SIZE + ITMH_BASE_EXT_SIZE];
            memset(vbuf, 0, ITMH_BASE_EXT_ADDITIONAL_BTYES_SIZE);
            vbuf[0]  =  (itmh->itmh_base_ext_additional_bytes >> 16) & 0xff;
            vbuf[1]  =  (itmh->itmh_base_ext_additional_bytes >> 8) & 0xff;
            vbuf[2]  =  itmh->itmh_base_ext_additional_bytes & 0xff;
            length += ITMH_BASE_EXT_ADDITIONAL_BTYES_SIZE;
        }
#endif
    }
#endif
#ifdef ITMH_PMF_EXT_EN
    vbuf = &pkt[L2HEADER_SIZE + RCPUHEADER_SIZE + PTCH_SIZE + length];
    memset(vbuf, 0, ITMH_PMF_EXT_SIZE);
    vbuf[0]  =  (itmh->itmh_pmf_extension >> 16) & 0xff;
    vbuf[1]  =  (itmh->itmh_pmf_extension >> 8) & 0xff;
    vbuf[2]  =  itmh->itmh_pmf_extension & 0xff;
    length += ITMH_PMF_EXT_SIZE;
#endif

    return length;
}

static void
packPPHBase(uint8_t * const pkt, struct PPHBase *pph , const int itmh_length)
{
    uint8_t    *vbuf;

    vbuf = &pkt[L2HEADER_SIZE + RCPUHEADER_SIZE + PTCH_SIZE + itmh_length];
    memset(vbuf, 0, PPH_BASE_SIZE);

    vbuf[0]  =  (pph->ext2_valid & 0x1) << 7;
    vbuf[0]  |=  (pph->ext3_valid & 0x1) << 6;
    vbuf[0]  |=  (pph->field1 & 0x3) << 4;
    vbuf[0]  |=  pph->reserved1 & 0xf;
    vbuf[1]  =  (pph->oam_offset & 0x7f) << 1;
    /* reserved2 and reserved3 are supposed to be 0*/
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
buildPTCH(
    struct PTCH *ptch,
    const uint8_t parser_program_control,
    const uint8_t opaque_pt_attributes,
    const uint16_t ssp)
{
    memset(ptch, 0, sizeof(struct PTCH));

    ptch->parser_program_control = parser_program_control;
    ptch->opaque_pt_attributes = opaque_pt_attributes;
    ptch->ssp = ssp;
}

static void
buildITMH(
    struct ITMH *itmh,
    const uint8_t pph_type,
    const uint8_t in_mirror_disable,
    const uint8_t fwd_dp,
    const uint32_t destination,
    const uint8_t snoop_cmd,
    const uint8_t fwd_traffic_class,
    const uint8_t itmh_base_ext_exist,
    const uint8_t itmh_base_ext_type,
    const uint8_t itmh_base_ext_reserved,
    const uint8_t itmh_base_ext_value1,
    const uint32_t itmh_base_ext_additional_bytes,
    const uint32_t itmh_pmf_extension
    )
{
    memset(itmh, 0, sizeof(struct ITMH));

    itmh->pph_type = pph_type;
    itmh->in_mirror_disable = in_mirror_disable;
    itmh->fwd_dp = fwd_dp;
    itmh->fwd_destnation_info = destination;
    itmh->snoop_cmd = snoop_cmd;
    itmh->fwd_traffic_class = fwd_traffic_class;
    itmh->itmh_base_ext_exist = itmh_base_ext_exist;
    itmh->itmh_base_ext_type = itmh_base_ext_type;
    itmh->itmh_base_ext_reserved = itmh_base_ext_reserved;
    itmh->itmh_base_ext_value1 = itmh_base_ext_value1;
    itmh->itmh_pmf_extension = itmh_pmf_extension;
}

static void
buildPPHBase(
    struct PPHBase *pph,
    const uint8_t ext2_valid,
    const uint8_t ext3_valid,
    const uint8_t field1,
    const uint8_t reserved1,
    const uint8_t oam_offset,
    const uint32_t reserved2,
    const uint32_t reserved3
)
{
    memset(pph, 0, sizeof(struct PPHBase));

    pph->ext2_valid = ext2_valid;
    pph->ext3_valid = ext3_valid;
    pph->field1 = field1;
    pph->reserved1 = reserved1;
    pph->oam_offset = oam_offset;
    pph->reserved2 = reserved2;
    pph->reserved3 = reserved3;
}

static void
relocateEthernetHeader(uint8_t * const pkt, const int protocol, const int syshdr_len)
{
    int         ix;

    /* Move Ethernet header to encapsulated frame */
    memcpy(&pkt[RCPUENCAP_SIZE + syshdr_len], pkt, 12);
    ix = RCPUENCAP_SIZE + syshdr_len + 12;
    /* Add VLAN tag */
    pkt[ix++] = 0x81;
    pkt[ix++] = 0x00;
    pkt[ix++] = 0x00;
    pkt[ix++] = 0x01;
    /* Add length */
    pkt[ix++] = protocol >> 8;
    pkt[ix++] = protocol & 0xff;
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
      ("Usage %s [-v] [-vv] [-dest port] [-ssp ssp] [-channel chan] [-ipd delay] [-s size] interface [maxTxPackets]\n",
       prog);
    printf("-r            ; Interface is in RCPU mode\n");
    printf("-m            ; Specify injection mode\n");
    printf("-dest <port>  ; Tx on specified port\n");
    printf("-ssp <ssp>    ; Specify source system port when tx through ingress logic\n");
    printf("-channel <chan>   ; Specify ingress CPU channel\n");
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
    int         port = -1;
    int         ssp = -1;
    int         cpu_channel = -1;
    int         mode = RX_PORT_INJECTION;
    int         parser_program_control = 1;
    int         pph_type = 0;
    int         itmh_length = 0;
    int         random_packet_sizes = 0;
    int         rcpu = 0;
    int         rcpu_len = 0;
    int         syshdr_len = 0;
    int         tag_len;
    int         tx_len;
    int         verbose = 0;
    int         vverbose = 0;
    uint8_t     tx_packet[TX_MAX];
    uint8_t     itmh_base_ext_exist;
    uint8_t     itmh_base_ext_type;

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
            } else if (strcmp(flag, "m") == 0) {
                ax++;
                if (ax == argc) {
                    fprintf(stderr, "Missing injection mode\n");
                    usage(argv[0]);
                    return -2;
                }
                if (argv[ax] != NULL) {
                    if (strcmp(argv[ax], "RxPort") == 0) {
                        mode = RX_PORT_INJECTION;
                    } else if (strcmp(argv[ax], "TxWire") == 0) {
                        mode = TX_WIRE_INJECTION;
                    } else if (strcmp(argv[ax], "EgressPP") == 0) {
                        mode = EGRESS_PP_INJECTION;
                    } else {
                        fprintf(stderr, "Invalid mode: %s\n", argv[ax]);
                        usage(argv[0]);
                        return -2;
                    }
                }
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
            } else if (strcmp(flag, "dest") == 0) {
                ax++;
                if (ax == argc) {
                    fprintf(stderr, "Missing destination port\n");
                    usage(argv[0]);
                    return -2;
                }
                if (argv[ax] != NULL) {
                    char       *end;

                    port = strtol(argv[ax], &end, 10);
                    if ((*end != 0) || (port < 0) || (port > 0xffff)) {
                        fprintf(stderr, "Invalid port: %s\n", argv[ax]);
                        usage(argv[0]);
                        return -2;
                    }
                }
            } else if (strcmp(flag, "ssp") == 0) {
                ax++;
                if (ax == argc) {
                    fprintf(stderr, "Missing source system port\n");
                    usage(argv[0]);
                    return -2;
                }
                if (argv[ax] != NULL) {
                    char       *end;

                    ssp = strtol(argv[ax], &end, 10);
                    if ((*end != 0) || (ssp < 0) || (ssp > 255)) {
                        fprintf(stderr, "Invalid source system port: %s\n", argv[ax]);
                        usage(argv[0]);
                        return -2;
                    }
                }
            }  else if (strcmp(flag, "channel") == 0) {
                ax++;
                if (ax == argc) {
                    fprintf(stderr, "Missing CPU channel\n");
                    usage(argv[0]);
                    return -2;
                }
                if (argv[ax] != NULL) {
                    char       *end;

                    cpu_channel = strtol(argv[ax], &end, 10);
                    if ((*end != 0) || (cpu_channel < 0) || (cpu_channel > 255)) {
                        fprintf(stderr, "Invalid CPU channel: %s\n", argv[ax]);
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
        struct PTCH ptch;
        struct ITMH itmh;
        struct PPHBase pph;

        switch (mode) {
            case RX_PORT_INJECTION:
                parser_program_control = 1;
                pph_type = 0;
                syshdr_len = PTCH_SIZE;
                break;
            case TX_WIRE_INJECTION:
                parser_program_control = 0;
                pph_type = 0;
                syshdr_len = PTCH_SIZE + ITMH_BASE_SIZE;
#ifdef ITMH_BASE_EXT_EN
                itmh_base_ext_exist = 1;
                syshdr_len += ITMH_BASE_EXT_SIZE;
#ifdef ITMH_BASE_EXT_ADDITIONAL_BYTES_EN
                itmh_base_ext_type = ITMH_BASE_EXT_ADDITIONAL_BYTES_TYPE;
                syshdr_len += ITMH_BASE_EXT_ADDITIONAL_BTYES_SIZE;
#endif
#endif
                break;
            case EGRESS_PP_INJECTION:
                parser_program_control = 0;
                /* PPH base exists */
                pph_type = 1;
                syshdr_len = PTCH_SIZE + ITMH_BASE_SIZE;
#ifdef ITMH_BASE_EXT_EN
                itmh_base_ext_exist = 1;
                syshdr_len += ITMH_BASE_EXT_SIZE;
#ifdef ITMH_BASE_EXT_ADDITIONAL_BYTES_EN
                itmh_base_ext_type = ITMH_BASE_EXT_ADDITIONAL_BYTES_TYPE;
                syshdr_len += ITMH_BASE_EXT_ADDITIONAL_BTYES_SIZE;
#endif
#endif
                syshdr_len += PPH_BASE_SIZE;
                break;
            default:
                break;
        }
        relocateEthernetHeader(tx_packet, payload_len + rcpu_len, syshdr_len);
        buildL2Header(&l2Header, SRC_ETHER_ADDR, DST_ETHER_ADDR, 0x81000001, 0xDE08);
        buildRCPUheader(&rcpuHeader, RCPU_SIGNATURE, 0x20, (port >= 0) ? 0x04 : 0x00, 0,
                        tx_len, 0, cpu_channel);
        buildPTCH(&ptch,
            parser_program_control,
            0, /* opaque_pt_attributes */
            ssp);
        /* parser_program_control: 0 indicates ITMH is present */
        if (parser_program_control == 0) {
            buildITMH(&itmh,
                    pph_type,
                    0, /*in_mirror_disable*/
                    0, /*fwd_dp*/
                    port,
                    0, /*snoop_cmd*/
                    0, /*fwd_traffic_class*/
                    itmh_base_ext_exist,
                    itmh_base_ext_type,
                    0, /*itmh_base_ext_reserved*/
                    0, /*itmh_base_ext_value1*/
                    0, /*itmh_base_ext_additional_bytes*/
                    0 /*itmh_pmf_extension*/
                    );
            if (pph_type) {
                buildPPHBase(&pph,
                        0, /* ext2_valid */
                        0, /* ext3_valid */
                        0, /* field1 */
                        0, /* reserved1 */
                        0, /* oam_offset */
                        0, /* reserved2 */
                        0  /* reserved3 */
                        );
            }
        }
        packL2header(tx_packet, &l2Header);
        packRCPUheader(tx_packet, &rcpuHeader);
        packPTCH(tx_packet, &ptch);
        if (parser_program_control == 0) {
            itmh_length = packITMH(tx_packet, &itmh);
            if (pph_type) {
                packPPHBase(tx_packet, &pph, itmh_length);
            }
        }
        tx_len += RCPUENCAP_SIZE;
        tx_len += syshdr_len;
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

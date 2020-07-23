/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *File: knet_rx.c
 * Purpose: This CINT script is a program used to receive packets over KNET interface.
 *
 *    Prerequistes:
 *    =============
 *      a) Build SDK with KNET feature enabled
 *      b) Insert KNET kernel module before launching SDK.
 *      c) Requried SOC proterties:
 *          tm_port_header_type_out_202.0=CPU
 *
 *    Steps:
 *    =============
 *      a) Build $SDK/src/examples/dpp/knet/knet_rx.c for customer target CPU:
 *          gcc knet_rx.c -o knet_rx
 *      b) Create KNET interface and filters refering to $SDK/src/examples/dpp/knet/knet.c
 *      c) Go to Linux shell prompt using "shell" command from BCM diag shell
 *      d) Configure IP address to knet interface, for example:
 *          ifconfig virt-intf0 192.168.2.254 netmask 255.255.255.0 up
 *      e) Run knet_rx in backround.
 *          if KNET interface is in CPU Ingress mode, Run knet_rx "./knet_rx -vv virt-intf0 5 &"
 *          if KNET interface is in Local Port mode, Run knet_rx "./knet_rx -vv virt-intf0 5 &"
 *          if KNET interface is in RCPU mode, Run knet_rx "./knet_rx -r -vv virt-intf0 5 &"
 *      f) Inject traffic
 *
 *    Usage of options:
 *    =============
 *      Usage ./knet_rx [-v] [-vv] interface [maxRxPackets]
 *      -r            ; RCPU mode
 *      -v            ; Be verbose
 *      -vv           ; Be very verbose
 *      interface     ; ethernet interface (eth5)
 *      maxRxPackets  ; Exit after receiving this many packets
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
/* Define Endian accordingly, little endian by default */
#define LE_HOST
/* Define the exist of FTMH LB KEY Extension header per SOC property system_ftmh_load_balancing_ext_mode, Not defined  by default */
/* #define FTMH_LB_KEY_EXT_EN */
/* Define the exist of FTMH Stacking Extension header per SOC property stacking_enable and stacking_extension_enable, Not defined by default */
/* #define FTMH_STACKING_EXT_EN */
/* Define the size of UDH header per SOC property field_class_id_size, Not defined by default */
/* #define UDH_EN */
/* Define the size of UDH header per SOC property field_class_id_size, 0 by default */
#define UDH_SIZE 0

/* Size of FTMH Base */
#define FTMH_BASE_SIZE 9
/* Size of FTMH LB key in bytes */
#define FTMH_LB_EXT_SIZE 1
/* Size of FTMH DSP in bytes */
#define FTMH_DSP_EXT_SIZE 2
/* Size of FTMH Stacking in bytes */
#define FTMH_STACKING_EXT_SIZE 2
/* Size of OAM Time-Stamp Header in bytes */
#define OTSH_SIZE 6
/* Type OAM for OTSH */
#define OTSH_TYPE_OAM 0
/* Size of Internal Header in bytes */
#define PPH_SIZE 7
/* Size of Internel Explicit Editing Inforamtion extension header in bytes */
#define PPH_EXT2_SIZE 3
/* Size of Internel Learn extension header in bytes */
#define PPH_EXT3_SIZE 5

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

struct FTMHBase{
#if defined(LE_HOST)
    uint32_t    packet_size_hi:8;           /* 0 */
    uint32_t    traffic_class_hi:2;         /* 1 */
    uint32_t    packet_size_lo:6;
    uint32_t    src_sys_port_hi:7;          /* 2 */
    uint32_t    traffic_class_lo:1;
    uint32_t    src_sys_port_mi:8;          /* 3 */
    uint32_t    pp_dsp_hi:7;                /* 4 */
    uint32_t    src_sys_port_lo:1;
    uint32_t    out_mirror_disable:1;       /* 5 */
    uint32_t    pph_type:2;
    uint32_t    action_type:2;
    uint32_t    dp:2;
    uint32_t    pp_dsp_lo:1;
    uint32_t    mc_id_or_outlif_hi:7;       /* 6 */
    uint32_t    tm_action_is_mc:1;
    uint32_t    mc_id_or_outlif_mi:8;       /* 7 */
    uint32_t    exclude_source:1;           /* 8 */
    uint32_t    ecn_enable:1;
    uint32_t    cni:1;
    uint32_t    dsp_ext_present:1;
    uint32_t    mc_id_or_outlif_lo:4;
#else
    uint32_t    packet_size_hi:8;           /* 0 */
    uint32_t    packet_size_lo:6;           /* 1 */
    uint32_t    traffic_class_hi:2;
    uint32_t    traffic_class_lo:1;         /* 2 */
    uint32_t    src_sys_port_hi:7;
    uint32_t    src_sys_port_mi:8;          /* 3 */
    uint32_t    src_sys_port_lo:1;          /* 4 */
    uint32_t    pp_dsp_hi:7;
    uint32_t    pp_dsp_lo:1;                /* 5 */
    uint32_t    dp:2;
    uint32_t    action_type:2;
    uint32_t    pph_type:2;
    uint32_t    out_mirror_disable:1;
    uint32_t    tm_action_is_mc:1;          /* 6 */
    uint32_t    mc_id_or_outlif_hi:7;
    uint32_t    mc_id_or_outlif_mi:8;       /* 7 */
    uint32_t    mc_id_or_outlif_lo:4;       /* 8 */
    uint32_t    dsp_ext_present:1;
    uint32_t    cni:1;
    uint32_t    ecn_enable:1;
    uint32_t    exclude_source:1;
#endif /* defined(LE_HOST) */
};

struct OTSH_OAM {
#if defined(LE_HOST)
    uint32_t    oam_ts_data_0:2;            /* 0 */
    uint32_t    mep_type:1;
    uint32_t    oam_sub_type:3;
    uint32_t    type:2;
    uint32_t    oam_ts_data_1:8;            /* 1 */
    uint32_t    oam_ts_data_2:8;            /* 2 */
    uint32_t    oam_ts_data_3:8;            /* 3 */
    uint32_t    oam_ts_data_4:8;            /* 4 */
    uint32_t    oam_ts_data_5:8;            /* 5 */
#else
    uint32_t    type:2;                     /* 0 */
    uint32_t    oam_sub_type:3;
    uint32_t    mep_type:1;
    uint32_t    oam_ts_data_0:2;
    uint32_t    oam_ts_data_1:8;            /* 1 */
    uint32_t    oam_ts_data_2:8;            /* 2 */
    uint32_t    oam_ts_data_3:8;            /* 3 */
    uint32_t    oam_ts_data_4:8;            /* 4 */
    uint32_t    oam_ts_data_5:8;            /* 5 */
#endif /* defined(LE_HOST) */
};

struct PPHBase {
#if defined(LE_HOST)
    uint32_t    reserved1:4;                /* 0 */
    uint32_t    field1:2;
    uint32_t    ext3_present:1;
    uint32_t    ext2_present:1;
    uint32_t    reserved2_hi:1;             /* 1 */
    uint32_t    oam_offset:7;
    uint32_t    reserved2_mi:6;             /* 2 */
    uint32_t    reserved2_lo:8;             /* 3 */
    uint32_t    reserved3_hi:2;             /* 4 */
    uint32_t    reserved3_mi:8;             /* 5 */
    uint32_t    reserved3_lo:8;             /* 6 */
#else
    uint32_t    ext2_present:1;             /* 0 */
    uint32_t    ext3_present:1;
    uint32_t    field1:2;
    uint32_t    reserved1:4;
    uint32_t    oam_offset:7;               /* 1 */
    uint32_t    reserved2_hi:1;
    uint32_t    reserved2_mi:8;             /* 2 */
    uint32_t    reserved2_lo:8;             /* 3 */
    uint32_t    reserved3_hi:6;             /* 4 */
    uint32_t    reserved3_mi:8;             /* 5 */
    uint32_t    reserved3_lo:8;             /* 6 */
#endif /* defined(LE_HOST) */
};

struct PPHExt1{
    uint32_t    oam_id_hi:8;                 /* 0 */
    uint32_t    oam_id_lo:8;                 /* 1 */
    uint32_t    trap_code:8;                 /* 2 */
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
    struct FTMHBase *ftmhbase;
    struct OTSH_OAM *otsh;
    struct PPHBase *pph;
    struct PPHExt1 *pph_ext1;
    uint8_t *header_start = NULL;
    uint8_t *header_offset = NULL;

    printf("\nRCPU Meta Data:\n");
    wbuf = (uint32_t *) & pkt[32];
    for (ix = 0; ix < 16; ix++) {
        if ((ix & 1) == 0) {
            printf("  Word %2d: 0x%08X", ix, wbuf[ix]);
        } else {
            printf("; 0x%08X\n", wbuf[ix]);
        }
    }

    printf("\nSystem Headers:\n");
    header_start = header_offset = (uint8_t *)&pkt[32];
    /* FTMH Base */
    ftmhbase = (struct FTMHBase *) header_offset;
    printf("  FTMHBase: Packet Size        : %d\n", (ftmhbase->packet_size_hi << 6) | ftmhbase->packet_size_lo);
    printf("  FTMHBase: Traffic Class      : %d\n", (ftmhbase->traffic_class_hi << 1) | ftmhbase->traffic_class_lo);
    printf("  FTMHBase: SSPA               : %d\n", (ftmhbase->src_sys_port_hi << 9) | (ftmhbase->src_sys_port_mi << 1) | ftmhbase->src_sys_port_lo);
    printf("  FTMHBase: PP DSP             : %d\n", (ftmhbase->pp_dsp_hi << 1) | (ftmhbase->pp_dsp_lo));
    printf("  FTMHBase: Drop Precedence    : %d\n", ftmhbase->dp);
    printf("  FTMHBase: TM Action Type     : %d\n", ftmhbase->action_type);
    printf("  FTMHBase: Internal Type      : %d\n", ftmhbase->pph_type);
    printf("  FTMHBase: Out Mirror Disable : %d\n", ftmhbase->out_mirror_disable);
    printf("  FTMHBase: TM Action Is MC    : %d\n", ftmhbase->tm_action_is_mc);
    printf("  FTMHBase: MC ID or OUTLIF    : %d\n", (ftmhbase->mc_id_or_outlif_hi << 12) | (ftmhbase->mc_id_or_outlif_mi<< 4) | ftmhbase->mc_id_or_outlif_lo);
    printf("  FTMHBase: DSP EXT Present    : %d\n", ftmhbase->dsp_ext_present);
    printf("  FTMHBase: CNI                : %d\n", ftmhbase->cni);
    printf("  FTMHBase: ECN Enable         : %d\n", ftmhbase->ecn_enable);
    printf("  FTMHBase: Exclude Source     : %d\n", ftmhbase->exclude_source);

    header_offset += FTMH_BASE_SIZE;

    /* FTMH LB-Key Extension */
#ifdef FTMH_LB_KEY_EXT_EN
    header_offset += FTMH_LB_EXT_SIZE;
#endif

    /* FTMH DSP Extension */
    if (ftmhbase->dsp_ext_present)
    {
        header_offset += FTMH_DSP_EXT_SIZE;
    }

    /* FTMH Stacking Extension */
#ifdef FTMH_STACKING_EXT_EN
    header_offset += FTMH_STACKING_EXT_SIZE;
#endif

    /* OTSH */
    if (ftmhbase->pph_type & 0x2)
    {
        otsh = (struct OTSH_OAM*) header_offset;
        if (otsh->type == OTSH_TYPE_OAM)
        {
            printf("  OTSH: Type                  : %d\n", otsh->type);
            printf("  OTSH: OAM Sub Type          : %d\n", otsh->oam_sub_type);
            printf("  OTSH: Mep Type              : %d\n", otsh->mep_type);
            printf("  OTSH: TS Data               : 0x%08x - %08x\n",
                ((otsh->oam_ts_data_0 << 8) | otsh->oam_ts_data_1),
                ((otsh->oam_ts_data_2 << 24) | (otsh->oam_ts_data_3 << 16) | (otsh->oam_ts_data_4 << 8) | otsh->oam_ts_data_5));
        }
        header_offset += OTSH_SIZE;
    }
    /* PPH Header */
    if (ftmhbase->pph_type & 0x1)
    {
        pph = (struct PPHBase*) header_offset;
        printf("  PPHBase: Field1              : %d\n", pph->field1);
        printf("  PPHBase: OAM Offset          : %d\n", pph->oam_offset);
        header_offset += PPH_SIZE;
        /* Advance header according to FHEI Trap extension */
        switch(pph->field1) {
            case 1:
                /* 3B FHEI Extension: do nothing, header poniter is in the right location */
                break;
            case 2:
                /* 5B FHEI Extension: adavance header pointer in 2B */
                header_offset += 2;
                break;
            case 3:
                /* 8B FHEI Extension: adavance header pointer in 5B */
                header_offset += 5;
                break;
            default:
                break;
        }
        if (pph->field1 == 1) {
            pph_ext1 = (struct PPHExt1*) header_offset;
            printf("  PPH EXT1: OAM ID             : %d\n", (pph_ext1->oam_id_hi << 8) | pph_ext1->oam_id_lo);
            printf("  PPH EXT1: Trap Code          : %d\n", pph_ext1->trap_code);
        }
        /* Move forward to end of FHEI Trap extension */
        if (pph->field1) {
            header_offset += 3;
        }
        /* EEI extension */
        if (pph->ext2_present) {
            header_offset += PPH_EXT2_SIZE;
        }
        /* Learn extension */
        if (pph->ext3_present) {
            header_offset += PPH_EXT3_SIZE;
        }

        /* UDH */
#ifdef UDH_EN
        header_offset += UDH_SIZE;
#endif
    }

    printf("  Total Size                   : %d\n", (header_offset - header_start));
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

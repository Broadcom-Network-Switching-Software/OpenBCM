/*  Feature  : KNET
 *
 *  Usage    : % th4_hsdk_knet_tx [-count <pkts>] <interface>
 *
 *  config   : N/A
 *
 *  Log file : N/A
 *
 *  Test Topology : N/A
 *
 *  Summary:
 *  ========
 * CAUTION: This is a "C" source file, not CINT. It must be compiled for
 * the same target as "bcm.user".
 *
 * This is a utility program designed to send packets on a specified KNET
 * virtual interface. It sends alternating BPDU and ARP packets until
 * the number of packets specified is reached. Once packet transmission is
 * complete, the programs sends one more special status packet that includes
 * the total count of packets sent (not including the special packet).
 *
 * This program should be used with the appropriate CINT script that configures
 * KNET interfaces and processes the status packets sent by this program.
 *
 *    Prerequistes:
 *    =============
 *      1) Build HSDK (bcm.user), this will be used to create and configured
 *      the KNET interfaces used by this program.
 *      2) Compile this program for the same target as bcm.user
 *
 *    Detailed steps performed by this program
 *    =====================================================
 *    Step 1: Parse command line parameters (function: main)
 *    Step 2: Open a raw socket (function: create_raw_socket)
 *    Step 3: Bind raw socket to interface (function: bind_raw_socket)
 *    Step 4: Send specified number of packets on socket using write().
 *    Step 5: Send a special status packet on the socket using write().
 *    Step 6: Exit
 */

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/if_packet.h>

#include <signal.h>

#define MAX_INTERFACE_NAME	32
#define MAC_ADDR_LEN		6

/* Define a "knet_tx" object type, used to contain test parameters etc. */
struct knet_tx_s {
    int                 sock_raw;
    char                interface[MAX_INTERFACE_NAME];
    unsigned char       mac_addr[MAC_ADDR_LEN];
    unsigned int        packets_to_send;
};

/* Static function prototypes */
static int          bind_raw_socket(char *intf, int rawsock, int protocol);
static int          get_if_mac(int raw_sock, char *intf, unsigned char *mac);
static int          knet_tx_init(struct knet_tx_s *knet_tx, char *interface);
static int          knet_tx_write(struct knet_tx_s *knet_tx);
static int          knet_tx_start(struct knet_tx_s *knet_tx, char *interface);
static void         sigint(int signum);

/* Begin static functions */

/*
 * Function: bind_raw_socket
 *
 * Assign the interface specified to by "name" and "protocol" to the socket
 * referred to by "rawsock".
 *
 * Parameters:
 *   intf: Ethernet interface name.
 *   rawsock: file descriptor returned from socket(2)
 *   protocol: socket protocol, typically ETH_P_ALL.
 *
 * Returns 0 on success, -1 othewise
 */
static int
bind_raw_socket(char *intf, int rawsock, int protocol)
{

    struct ifreq        ifr;
    struct sockaddr_ll  sll;

    /* First Get the Interface Index  */
    memset(&ifr, 0, sizeof(ifr));
    strncpy((char *) ifr.ifr_name, intf, IFNAMSIZ);
    if ((ioctl(rawsock, SIOCGIFINDEX, &ifr)) == -1) {
        fprintf(stderr, "Failed to get interface index for interface \"%s\"\n", intf);
        return -1;
    }

    /* Bind our raw socket to this interface */
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(protocol);

    if ((bind(rawsock, (struct sockaddr *) &sll, sizeof(sll))) == -1) {
        fprintf(stderr, "Failed to bind interface \"%s\" to socket\n", intf);
        return -1;
    }

    return 0;
}

/*
 * Function: get_if_mac
 *
 * Get the MAC address associated with "raw_sock" and "intf".
 *
 * Parameters:
 *   raw_sock: socket file descriptor
 *   intf: interface name
 *   mac: returned MAC address
 *
 * Returns 0 on success, -1 othewise
 */
static int
get_if_mac(int raw_sock, char *intf, unsigned char *mac)
{
    struct ifreq        ifr;

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, intf);

    if (ioctl(raw_sock, SIOCGIFHWADDR, &ifr) < 0) {
        return -1;
    }

    memcpy(mac, ifr.ifr_hwaddr.sa_data, MAC_ADDR_LEN);

    return 0;
}

/*
 * Function: knet_tx_init
 *
 * Initialize a "knet_tx" object. This includes setting up the raw socket.
 *
 * Parameters:
 *   knet_tx: pointer to knet_tx object
 *   interface: interface name for new knet_tx
 *
 * Returns 0 on success, 1 othewise
 */
static int
knet_tx_init(struct knet_tx_s *knet_tx, char *interface)
{
    const int           protocol = ETH_P_ALL;

    if ((knet_tx->sock_raw = socket(AF_PACKET, SOCK_RAW, htons(protocol))) < 0) {
        fprintf(stderr, "Failed to open raw socket\n");
        return 1;
    }

    if (bind_raw_socket(interface, knet_tx->sock_raw, protocol) < 0) {
        fprintf(stderr, "Failed to bind on %s\n", interface);
        return 1;
    }
    if (get_if_mac(knet_tx->sock_raw, interface, knet_tx->mac_addr) < 0) {
        fprintf(stderr, "Failed to get interface MAC address for %s\n", interface);
        return 1;
    }
    strncpy(knet_tx->interface, interface, MAX_INTERFACE_NAME);
    return 0;
}

/* Define a BPDU packet */
static const unsigned char BPDU[] = {

/*
  00    01    02    03    04    05    06    07    08    09    10    11    12    13    14    15*/
0x01, 0x80, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x81, 0x00, 0xC0, 0x01,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x14,
0x00, 0x02, 0x00, 0x0F, 0x00, 0x42, 0x72, 0x6F, 0x61, 0x64, 0x63, 0x6F, 0x6D, 0x20, 0x4C, 0x74,
0x64, 0x2E, 0x20, 0x2D, 0x2D, 0x42, 0x72, 0x6F, 0x61, 0x64, 0x63, 0x6F, 0x6D, 0x20, 0x4C, 0x74,
0x64, 0x2E, 0x20, 0x2D, 0x2D, 0x42, 0x72, 0x6F, 0x61, 0x64, 0x63, 0x6F, 0x6D, 0x20, 0x4C, 0x74,
0x64, 0x2E, 0x20, 0x2D, 0x2D, 0x42, 0x72, 0x6F, 0x61, 0x64, 0x63, 0x6F, 0x6D, 0x20, 0x4C, 0x74,
0x64, 0x2E, 0x20, 0x2D, 0x2D, 0x42, 0x72, 0x6F, 0x61, 0x64, 0x63, 0x6F, 0x6D, 0x20, 0x4C, 0x74,
0x64, 0x2E, 0x20, 0x2D, 0x2D, 0x42, 0x72, 0x6F, 0x61, 0x64, 0x63, 0x6F, 0x6D, 0x20, 0x4C, 0x74,
0x64, 0x2E, 0x20, 0x2D, 0x2D, 0x42, 0x72, 0x6F, 0x61, 0x64, 0x63, 0x6F, 0x6D, 0x20, 0x4C, 0x74,
0x64, 0x2E, 0x20, 0x2D, 0x2D, 0x42, 0x72, 0x6F, 0x61, 0x64, 0x63, 0x6F, 0x6D, 0x20, 0x4C, 0x74,
0x64, 0x2E, 0x20, 0x2D, 0x2D,
};

static const int    BPDU_SIZEOF = sizeof(BPDU);

/* Define an ARP request packet */
static unsigned char ARP[] = {

/*
  00    01    02    03    04    05    06    07    08    09    10    11    12    13    14    15*/
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x08, 0x06, 0x00, 0x01,
0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x26, 0xB9, 0x78, 0xF8, 0xB0, 0x0A, 0x10, 0x40, 0xB4,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x72, 0x70, 0x52, 0x65, 0x71,
0x75, 0x65, 0x73, 0x74, 0x2D, 0x41, 0x72, 0x70, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x2D,
0x41, 0x72, 0x70, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x2D, 0x41, 0x72, 0x70, 0x52, 0x65,
0x71, 0x75, 0x65, 0x73, 0x74, 0x2D,
};

static const int    ARP_SIZEOF = sizeof(ARP);

/*
 * Define a special status packet to send back to switch
 *
 * Destination MAC: 88:88:88:88:88:88
 * Source MAC: 98:98:98:98:98:98
 * VLAN tag: 0x8100:0x0001
 * Ethertype: 0x9999
 *
 * Insert TX packet count at status_pkt[19:18]
 */

static unsigned char status_pkt[] = {

/*
  00    01    02    03    04    05    06    07    08    09    10    11    12    13    14    15*/
0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x81, 0x00, 0x00, 0x01,
0x99, 0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};

static const int    STATUS_SIZEOF = sizeof(status_pkt);

/*
 * Function: knet_tx_write
 *
 * Write a specified number of packets to a socket. When complete, send a
 * status packet that indicates how many packets were written.
 *
 * Parameters:
 *   knet_tx: pointer to knet_tx object
 *
 * Returns 0 on success, 1 othewise
 */
static int
knet_tx_write(struct knet_tx_s *knet_tx)
{
    const int           packet_count = 2;
    const unsigned char *packet_list[2] = { BPDU, ARP };
    const int           packet_sizes[2] = { BPDU_SIZEOF, ARP_SIZEOF };

    unsigned int        tx_count;
    int                 sent;

    for (tx_count = 0; tx_count < knet_tx->packets_to_send; tx_count++) {
        const int           packet_idx = tx_count % packet_count;
        const int           this_packet_size = packet_sizes[packet_idx];
        const unsigned char *this_packet = packet_list[packet_idx];

        /* A simple write on the socket, thats all it takes ! */
        if ((sent =
             write(knet_tx->sock_raw, this_packet,
                   this_packet_size)) != this_packet_size) {
            /* Error */
            if (sent < 0) {
                fprintf(stderr, "Send (%d bytes) failed\n", this_packet_size);
                return 1;
            } else {
                fprintf(stderr, "Could only send %d bytes of packet of length %d\n",
                        sent, this_packet_size);
                return 1;
            }
        }
        usleep(200 * 1000);     /* 200 msecs between packets */
    }
    usleep(500 * 1000);     /* 500 msecs after packets */
    printf("TX Done; Send status packet\n");
    status_pkt[18] = (tx_count >> 8) & 0xFF;
    status_pkt[19] = tx_count & 0xFF;
    if ((sent =
         write(knet_tx->sock_raw, status_pkt, STATUS_SIZEOF)) != STATUS_SIZEOF) {
        /* Error */
        if (sent < 0) {
            fprintf(stderr, "Send (%d bytes) failed\n", STATUS_SIZEOF);
            return 1;
        } else {
            fprintf(stderr, "Could only send %d bytes of packet of length %d\n",
                    sent, STATUS_SIZEOF);
            return 1;
        }
    }
    return 0;
}

/*
 * Function: knet_tx_start
 *
 * Intitialize KNET interface and send some packets on it.
 *
 * Parameters:
 *   knet_tx: pointer to knet_tx object
 *   interface: KNET virtual interface name
 *
 * Returns 0 on success, 1 othewise
 */
static int
knet_tx_start(struct knet_tx_s *knet_tx, char *interface)
{
    if (knet_tx_init(knet_tx, interface)) {
        return 1;
    }
    return knet_tx_write(knet_tx);
}

/*
 * Function: sigint
 *
 * Signal handler for SIGINT signals. Called in interrupt context in response
 * to control-C.
 *
 * Parameters:
 *   signum: Signal number
 *
 * No return, calls exit(3)
 */
static void
sigint(int signum)
{
    fprintf(stderr, "\nTX terminating ...\n");
    exit(0);
}

/*******************************************************************************
 * Function: main
 *
 * Entry point for this program
 *
 * Parameters:
 *   argc: Number of arguments on command line
 *   argv: Command line argument list
 *
 * Returns:
 *   N/A
 */
int
main(int argc, char **argv)
{
    int                 ax;
    struct knet_tx_s    knet_tx;
    unsigned int        packets_to_send = 20;
    char               *intf = NULL;

    /* Parse commandline parameters */
    for (ax = 1; ax < argc; ax++) {
        if (argv[ax][0] == '-') {
            char               *flag = &argv[ax][1];

            if (strcmp(flag, "h") == 0) {
                printf("usage: %s [-count <pkts>] intf\n", argv[0]);
                return 0;
            } else if (strcmp(flag, "count") == 0) {
                ax++;
                if (ax == argc) {
                    fprintf(stderr, "Missing packet count\n");
                    return 1;
                }
                if (argv[ax] != NULL) {
                    char               *end;

                    packets_to_send = strtol(argv[ax], &end, 10);
                    if ((*end != 0) || (packets_to_send < 1)
                        || (packets_to_send > 1000000)) {
                        fprintf(stderr, "Invalid packet count: %s\n", argv[ax]);
                        return 1;
                    }
                }
            } else {
                fprintf(stderr, "ERROR: unknown switch %s\n", argv[ax]);
                return 1;
            }
        } else {
            if (intf == NULL) {
                intf = argv[ax];
            } else {
                fprintf(stderr, "Unknown parameter %s\n", argv[ax]);
            }
        }
    }

    if (intf == NULL) {
        fprintf(stderr, "Missing interface name\n");
        return 1;
    }
    /* Establish signal handler */
    if (SIG_ERR == signal(SIGINT, sigint)) {
        return 1;
    }

    knet_tx.packets_to_send = packets_to_send;

    if (knet_tx_start(&knet_tx, intf)) {
        return 2;
    }
    return 0;
}

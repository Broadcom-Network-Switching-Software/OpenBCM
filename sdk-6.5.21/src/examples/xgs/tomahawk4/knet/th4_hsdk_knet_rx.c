/*  Feature  : KNET
 *
 *  Usage    : % th4_hsdk_knet_rx [-timeout <timeout>] <interface> [<max_packets>]
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
 * This is a utility program designed to read packets from a specified KNET
 * virtual interface.  It keeps track of the number of packets received and
 * when completed, either by reaching the specied number of packets or timing,
 * sends a status packet to a special Ethernet address. It is used to verify
 * virtual network interface behavior and as an example of how to write an
 * application that operates on one.
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
 *    Step 4: Read packets from socket using recvfrom(). Continue to
 *      read packets until either a read timeout occurs on the socket or the
 *      specified number of packets is reached.
 *    Step 5: Send a special status packet on the socket using write().
 *    Step 6: exit
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

/* Maximum packet size */
#define RX_MAX  9100

/* Local function prototypes */
static void         dump_raw(uint8_t * rx_packet, int rx_len);
static int          create_raw_socket(int protocol_to_sniff);
static int          bind_raw_socket(char *device, int rawsock, int protocol,
                                    int timeout_in_seconds);
static void         usage(const char *const prog);

/*******************************************************************************
 * Function: dump_raw
 *
 * Print a packet buffer.
 *
 * Parameters:
 *   rx_packet: Pointer to packet buffer
 *   rx_len: Number of bytes to print
 *
 * Returns:
 *   N/A
 */
static void
dump_raw(uint8_t * rx_packet, int rx_len)
{
    int                 ix;

    for (ix = 0; ix < rx_len; ix++) {
        if ((ix & 0xf) == 0) {
            printf("0x%04x:", ix);
        } else if ((ix & 0xf) == 8) {
            printf(" -");
        }
        printf(" %02X", rx_packet[ix]);
        if ((ix & 0xf) == 0xf) {
            printf("\n");
        }
    }
    if ((ix & 0xf) != 0) {
        printf("\n");
    }
}

/*******************************************************************************
 * Function: create_raw_socket
 *
 * Create an endpoint for communication and returns a descriptor.
 *
 * The socket is configured allow access to low level packets (no protocol)
 * using raw network protocol access. All Ethernet packets are accepted.
 *
 * Parameters:
 *   protocol_to_sniff: Typically ETH_P_ALL
 *
 * Returns:
 *   Socket file descriptor on success, negative number on failure.
 */
static int
create_raw_socket(int protocol_to_sniff)
{
    int                 rawsock;

    if ((rawsock = socket(PF_PACKET, SOCK_RAW, htons(protocol_to_sniff))) < 0) {
        perror("Error creating raw socket: ");
        return rawsock;
    }
    return rawsock;
}

/*******************************************************************************
 * Function: bind_raw_socket
 *
 * Bind socket to an interface.
 *
 * Parameters:
 *   interface_name: Name of Ethernet interface
 *   rawsock: Socket descriptor
 *   protocol: Typically ETH_P_ALL
 *   timeout: Read timeout for socket
 *
 * Returns:
 *   0 on success, -1 on failure
 */
static int
bind_raw_socket(char *interface_name, int rawsock, int protocol, int timeout_in_seconds)
{
    struct timeval      tv;
    struct sockaddr_ll  sll;
    struct ifreq        ifr;

    bzero(&sll, sizeof(sll));
    bzero(&ifr, sizeof(ifr));

    /* First get the interface index based in interface name.  */
    strncpy((char *) ifr.ifr_name, interface_name, IFNAMSIZ);
    if ((ioctl(rawsock, SIOCGIFINDEX, &ifr)) == -1) {
        printf("Error getting interface index for %s\n", interface_name);
        return -1;
    }

    /*
     * Set a timeout on the interface so we don't block forever on recvfrom.
     */
    tv.tv_sec = timeout_in_seconds;
    tv.tv_usec = 0;
    setsockopt(rawsock, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(tv));

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

/*
 * Define a special status packet to send back to switch
 *
 * Destination MAC: 88:88:88:88:88:88
 * Source MAC: 94:94:94:94:94:94
 * VLAN tag: 0x8100:0x0001
 * Ethertype: 0x9999
 *
 * Insert RX packet count at status_pkt[19:18]
 */
static unsigned char status_pkt[] = {

/*
  00    01    02    03    04    05    06    07    08    09    10    11    12    13    14    15*/
0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x81, 0x00, 0x00, 0x01,
0x99, 0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};

static const int    STATUS_SIZEOF = sizeof(status_pkt);

/*******************************************************************************
 * Function: usage
 *
 * Print usage when an error is encountered parsing the command line
 * parameters.
 *
 * Parameters:
 *   prog: Name of this program
 *
 * Returns:
 *   N/A
 */
static void
usage(const char *const prog)
{
    printf("Usage %s [-v] [-timeout <timeoutsecs>] interface [maxRxPackets]\n", prog);
    printf("-timeout:     : Seconds to wait for a packet before exiting\n");
    printf("-v:           : verbose mode, dump received packets to console\n");
    printf("interface     ; KNET virtual interface)\n");
    printf("maxRxPackets  ; Exit after receiving this many packets\n");
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
    char               *ifname = NULL;  /* required parameter */
    int                 ax;
    int                 max_packets = -1;
    int                 rawsock;
    int                 rx_len;
    int                 rx_packets;
    int                 sent;
    int                 timeout = 30;   /* default timeout 30 seconds */
    int                 verbose = 0;
    socklen_t           len;
    struct sockaddr_ll  skaddr;
    uint8_t             rx_packet[RX_MAX];      /* Packet buffer */

    /* Parse the command line, look for arguments and switches */
    for (ax = 1; ax < argc; ax++) {
        if (argv[ax][0] == '-') {
            char               *flag = &argv[ax][1];

            if (strcmp(flag, "v") == 0) {
                verbose = 1;
            } else if (strcmp(flag, "timeout") == 0) {
                ax++;
                if (ax == argc) {
                    fprintf(stderr, "Missing timeout\n");
                    return 1;
                }
                if (argv[ax] != NULL) {
                    char               *end;

                    timeout = strtol(argv[ax], &end, 10);
                    if ((*end != 0) || (timeout < 1)
                        || (timeout > 3600)) {
                        fprintf(stderr, "Invalid timeout: %s\n", argv[ax]);
                        return 1;
                    }
                }
            } else {
                fprintf(stderr, "ERROR: unknown switch %s\n", argv[ax]);
                usage(argv[0]);
                return 1;
            }
        } else {
            if (ifname == NULL) {
                ifname = argv[ax];
            } else if (max_packets < 0) {
                max_packets = atoi(argv[ax]);
                if (max_packets == 0) {
                    printf("Invalid packet count: %s\n", argv[ax]);
                    return 1;
                }
            } else {
                fprintf(stderr, "ERROR: unknown argument %s\n", argv[ax]);
                usage(argv[0]);
                return 1;
            }
        }
    }

    /* Must have an interface name */
    if (ifname == NULL) {
        usage(argv[0]);
        return 1;
    }

    /* Set max packets if none found on command line */
    if (max_packets <= 0) {
        max_packets = 100;
    }

    /* Create the raw socket */
    rawsock = create_raw_socket(ETH_P_ALL);
    if (rawsock < 0) {
        return 1;
    }

    /* Bind raw socket to interface */
    if (bind_raw_socket(ifname, rawsock, ETH_P_ALL, timeout) < 0) {
        return 1;
    }

    rx_packets = 0;
    while (rx_packets < max_packets) {
        memset(&skaddr, 0, sizeof(skaddr));
        skaddr.sll_protocol = htons(ETH_P_ALL);
        len = sizeof(struct sockaddr);

        if ((rx_len = recvfrom(rawsock, rx_packet, RX_MAX, 0,
                               (struct sockaddr *) &skaddr, &len)) > 0) {
            char               *packet_type;

            switch (skaddr.sll_pkttype) {
              case PACKET_OUTGOING:
                  packet_type = "outgoing";
                  break;
              case PACKET_HOST:
                  packet_type = "host";
                  break;
              case PACKET_BROADCAST:
                  packet_type = "broadcast";
                  break;
              case PACKET_MULTICAST:
                  packet_type = "multicast";
                  break;
              case PACKET_OTHERHOST:
                  packet_type = "other host";
                  break;
              default:
                  packet_type = "unknown";
                  break;
            }
            if (skaddr.sll_pkttype == PACKET_OUTGOING) {
                /* Don't processing outgoing packets on socket */
                continue;
            }
            rx_packets++;
            if (verbose) {
                printf
                  ("\n--------------------------------------------------------------------------------\n");
            }
            printf("%s: Received %s packet %d (%d bytes)\n", argv[0], packet_type,
                   rx_packets, rx_len);
            if (verbose) {
                dump_raw(rx_packet, rx_len);
            }
        } else if (rx_len < 0) {
            if (errno == EAGAIN) {
                if (rx_packets > 1) {
                    printf("%s: Timed out after %d seconds\n", argv[0], timeout);
                    break;
                }
            } else {
                perror("recvfrom()");
                break;
            }
        }
    }
    printf("%s: DONE; Received %d packets, Sending status packet\n", argv[0],
           rx_packets);

    /* Insert packet count into status packet */
    status_pkt[18] = (rx_packets >> 8) & 0xFF;
    status_pkt[19] = rx_packets & 0xFF;
    if ((sent = write(rawsock, status_pkt, STATUS_SIZEOF)) != STATUS_SIZEOF) {
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

    close(rawsock);

    return 0;
}

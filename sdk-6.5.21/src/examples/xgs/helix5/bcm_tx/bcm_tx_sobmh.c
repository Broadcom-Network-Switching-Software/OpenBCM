/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Bcm Tx sobmh
 *
 *  Usage    : BCM.0> cint bcm_tx_sobmh.c
 *
 *  config   : bcm_tx_config.bcm
 *
 *  Log file : bcm_tx_sobmh_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+  egress_port1
 *                   |                              +-----------------+
 *                   |                              |
 *                   |                              |
 *  cpu_port         |                              |  egress_port2
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |  egress_port3
 *                   |                              +-----------------+
 *                   |                              |
 *                   +------------------------------+
 *  Summary:                                                           
 *  ========
 *    This CINT script demonstrates how to send packets from application to particular
 *    port bypassing the ingress pipeline.
 *    This example shows how application can send packets using bcm_tx() API
 *
 *    Detailed steps done in the CINT script:
 *    =====================================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ====================================
 *      a) Select three egress port and configure them in loopback mode.
 *      b) Add static L2 entry on cpu port with mac=80:80:80:11:22:33 and vlan=1
 *      c) start Packet Watcher diag app (PW start)
 *
 *    2) Step2 - Configuration(Done in tx_setup())
 *    ===================================================
 *      a) There is nothing to be done in tx_setup
 *
 *    3) Step3 - Verification(Done in verify())
 *    ==========================================
 *      a) Transmit 2 packets per egress port using bcm_tx() API
 *      b) Expected Result:
 *         ================
 *         The packets are looped back on egress ports and are received by CPU port
 *         PW will dump the recieved packets on console.
 */


cint_reset();
bcm_port_t egress_port[3];
/*
 * sobmhTx()
 *
 * Create and send an L2/L3 packet directly to a specific port.
 *
 * The user specifies packet length, source and destination MAC adresses
 * and the desired egress port. The routine allocates and initializes the
 * packet payload of requested size with a simple UDP header.
 *
 * Parameters:
 *   unit: tx device unit number
 *   vtag: VLAN tag (VLAN ID + Pkt Pri)
 *   mac_dest: Destination MAC address for L2 header
 *   mac_src: Source MAC address for L2 header
 *   port: egress port for packet
 *   len: size (in bytes) of packet
 */
int
sobmhTx(int unit, int vtag, bcm_mac_t mac_dest, bcm_mac_t mac_src,
         bcm_port_t port, int len)
{
    /* Optionally print packet contents before transmitting. */
    const int   echo_packet = 0;

    /* Simple IPv4 UDP header */
    uint8       L3_header[] = {
        0x08, 0x00, 0x45, 0x00, 0x00, 0x1C, 0x00, 0x01, 0x00, 0x00, 0x40, 0x11, 0xF7,
        0x13, 0xC0, 0xA8, 0x01, 0x04, 0xC0, 0xA8, 0x01, 0x68, 0x00, 0x14, 0x04, 0xD2,
        0x00, 0x40, 0x77, 0x03
    };
    const int         L3_header_len = sizeof(L3_header);

    bcm_pkt_t  *txPkt;
    uint8      *myData;
    int         packet_len;
    int         i;
    int         rv;

    if (len < 68) {
	/* Don't send runt packets */
        return BCM_E_PARAM;
    }

    /*
     * Basic, single block packet allocation, allocate a packet with DMA-able
     * data buffer. Allocate additiona space for CRC and regenerate CRC. No
     * VLAN tag.
     */
    rv = bcm_pkt_alloc(unit, len, BCM_TX_CRC_REGEN, &txPkt);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_pkt_alloc(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /*
     * By setting the destination port and NOT using the BCM_TX_ETHER
     * flag, indicate that this packet should be sent directly to the
     * destination port and should bypass all ingress pipeline processing.
     */
    BCM_PBMP_PORT_SET(txPkt->tx_pbmp, port);

    /* Set up the source and destination MAC addresses in the L2 header. */
    BCM_PKT_HDR_SMAC_SET(&txPkt, (auto) mac_src);
    BCM_PKT_HDR_DMAC_SET(&txPkt, (auto) mac_dest);

    /*
     * These operations are incompatible between C and CINT (bogus pointer check in CINT).
     */
    BCM_PKT_HDR_TPID_SET(&txPkt, 0x8100);
    BCM_PKT_HDR_VTAG_CONTROL_SET(&txPkt, &vtag);

    /* Get a pointer to the payload area of packet */
    if ((BCM_E_NONE ==
         bcm_pkt_byte_index(txPkt, BCM_IEEE_HDR_BYTES, &packet_len, NULL, &myData))
        && (myData != 0)) {
        /* Initialize payload (IPv4/UDP header) */
        for (i = 0; i < L3_header_len; i++) {
            myData[i] = L3_header[i];
        }
        /* Initialize rest of packet (size less FCS) */
        for (; i < packet_len - 4; i++) {
            myData[i] = i & 0xFF;
        }
    }

    if (echo_packet) {
        printf("Sending Packet:\n");
        for (i = 0; i < txPkt->_pkt_data.len; i++) {
            printf(" %02X", txPkt->_pkt_data.data[i] & 0xFF);
            if ((i & 0xF) == 0xF) {
                printf("\n");
            }
        }
        printf("\n");
    }

    /*
     * Send the packet directly to egress port, skip all pipeline processing
     */
    rv = bcm_tx(unit, txPkt, NULL);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_tx(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    bcm_pkt_free(unit, txPkt);
    return rv;
}
/* This function is written so that hardcoding of port
   numbers in Cint scripts is removed. This function gives
   required number of ports
*/
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{

  int i=0,port=0,rv=0;
  bcm_port_config_t configP;
  bcm_pbmp_t ports_pbmp;

  rv = bcm_port_config_get(unit,&configP);
  if(BCM_FAILURE(rv)) {
    printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
    return rv;
  }

  ports_pbmp = configP.e;
  for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
    if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
      port_list[port]=i;
      port++;
    }
  }

  if (( port == 0 ) || ( port != num_ports )) {
      printf("portNumbersGet() failed \n");
      return -1;
  }

  return BCM_E_NONE;

}

bcm_error_t test_setup(int unit)
{
    const bcm_vlan_t vlan = 1;

    /* Pick arbitrary dest MAC Address */
    const bcm_mac_t dest = "80:80:80:11:22:33";

    /* Test case variables used to find a suitable test port list */
    bcm_l2_addr_t l2addr;

    int port_list[3];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    egress_port[0] = port_list[0];
    egress_port[1] = port_list[1];
    egress_port[2] = port_list[2];

    /* Put all egress ports into loopback */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port[0], BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port[1], BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port[2], BCM_PORT_LOOPBACK_MAC));

    /* Add L2 entry to direct traffic to CPU port */
    bcm_l2_addr_t_init(&l2addr, dest, vlan);
    l2addr.port = BCM_GPORT_LOCAL_CPU;
    l2addr.flags = BCM_L2_STATIC;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));

    /* Start the packet watcher (quiet mode) */
    bshell(unit, "pw start report +raw +decode");
    
    return BCM_E_NONE;
}
/*
 * Test Harness Follows
 *
 * Packets of specified size are
 * transmitted to each egress_port in order.
 */
int
test_harness(int unit)
{
    const int   packet_len = 100;
    const int   egress_port_count = 3;
    const int   tx_count = egress_port_count * 2;
    const bcm_vlan_t vlan = 1;

    /* Pick arbitrary MAC Addresses */
    const bcm_mac_t dest = "80:80:80:11:22:33";
    const bcm_mac_t source = "40:40:40:44:55:66";

    int         idx;

    /* Transmit packet across all egress ports */
    for (idx = 0; idx < tx_count; idx++) {
        const int   pkt_pri = idx & 0x7;
        const int   vtag = (pkt_pri << 13) | vlan;

        /* Send a packet to the designated port */
        BCM_IF_ERROR_RETURN(sobmhTx
                            (unit, vtag, dest, source,
                             egress_port[idx % egress_port_count], packet_len));
    }

    return BCM_E_NONE;
}

/* Tx Setup */
bcm_error_t tx_setup(int unit)
{
    return BCM_E_NONE;
}

/* Run the test, save and possibly display error status */
void verify(int unit)
{
    bcm_error_t rv;
    if (BCM_FAILURE(rv = test_harness(0))) {
        printf("Tx verification failed: %s\n", bcm_errmsg(rv));
    }
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit =0;
  bshell(unit, "config show; a ; version");
  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }

  if (BCM_FAILURE((rv = tx_setup(unit)))) {
    printf("Tx Setup Failed\n");
    return -1;
  }

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


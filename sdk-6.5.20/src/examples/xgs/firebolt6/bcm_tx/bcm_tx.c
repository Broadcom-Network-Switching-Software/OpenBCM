/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Bcm Tx 
 *
 *  Usage    : BCM.0> cint bcm_tx.c
 *
 *  config   : bcm_tx_config.bcm
 *
 *  Log file : bcm_tx_log.txt
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
 *    This CINT script demonstrates how to send packets from application which will 
 *    go through the ingress pipeline and pipeline decides the egress port to send out the packet.
 *    This example shows how application can send packets using bcm_tx() API
 *
 *    Detailed steps done in the CINT script:
 *    =====================================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select three egress port and configure them in loopback mode.
 *      b) Create Vlan 10 and add egress port 1 and CPU port to vlan 10.
 *      c) Create Vlan 20 and add egress port 2 and CPU port to vlan 20.
 *      d) Create Vlan 30 and add egress port 3 and CPU port to vlan 30.
 *      e) start Packet Watcher diag app (PW start)
 *
 *    2) Step2 - Configuration (Done in tx_setup())
 *    ============================================
 *      a) There is nothing to be done in tx_setup
 *
 *    3) Step3 - Verification(Done in verify())
 *    ==========================================
 *      a) Transmit one packet with vlan=10 using bcm_tx() API
 *      b) Transmit one packet with vlan=20 using bcm_tx() API
 *      c) Transmit one packet with vlan=30 using bcm_tx() API
 *      d) Expected Result:
 *      ===================
 *         The packets are looped back on egress ports and are received by CPU port.
 *         PW will dump the recieved packets on console.
 */

cint_reset();
bcm_port_t egress_port[3];


/*
 * Simplest example of the bcm_tx() API.
 *
 * Sends a single packet to the switch subject to ordinary pipeline processing.
 *
 * Parameters:
 *   unit: tx device unit number
 *   pktData: Complete packet
 *   length: Size of commplete packet in bytes.
 */
bcm_error_t
tx_pkt_data(int unit, uint8 *pktData, int length)
{
    bcm_pkt_t  *pkt;
    bcm_error_t rv;

    /*
     * Allocate a packet structure and associated DMA packet data buffer.
     * Set Tx flags:
     *    BCM_TX_CRC_REGEN: Regenerate packet CRC.
     *    BCM_TX_ETHER: Inject fully formed packet into the ingress
     *                  pipeline. Packet will be subject to all pipeline processing.
     */
    rv = bcm_pkt_alloc(unit, length, BCM_TX_CRC_REGEN | BCM_TX_ETHER, &pkt);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_pkt_alloc(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Copy packet data from caller into DMA packet data buffer and set
     * packet length. Copy offset is 0, ignore return value which is number
     * of bytes copied.
     */
    bcm_pkt_memcpy(pkt, 0, pktData, length);

    /*
     * Transmit packet, wait for DMA to complete before returning. Success
     * status only indicates packet was transferred to switch, it does not
     * ensure that packet will not be dropped.
     */
    rv = bcm_tx(unit, pkt, NULL);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_tx(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
 
    /* Free packet resources */
    bcm_pkt_free(unit, pkt);

    /* return status from bcm_tx */
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
    const bcm_vlan_t vlan[3] = {10, 20 , 30};
    int i=0;
    char        command[128];   /* Buffer for diagnostic shell commands */

    /* Pick arbitrary dest MAC Address */
    const bcm_mac_t dest[3] = {"08:08:08:11:22:33" , "09:09:09:11:22:33", "0a:0a:0a:11:22:33"};

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

    /* Create vlan and add ports */
    for (i=0; i < 3; i++)
    {
        sprintf(command, "vlan create %d PortBitMap=cpu0,%d", vlan[i],egress_port[i]);
        bshell(unit, command);
    }

    /* Start the packet watcher (quiet mode) */
    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;

}

/* Tx Setup */
bcm_error_t tx_setup(int unit)
{
    return BCM_E_NONE;
}


/*******************************************************************************
 * Test Harness Follows
 *
 * Define the test packet for transmission:
 *   Singled tagged, VLAN=10, packet priority=3
 *   Singled tagged, VLAN=20, packet priority=3
 *   Singled tagged, VLAN=30, packet priority=3
 */
const uint8 pkt1[] = {
/*
  00    01    02    03    04    05    06    07    08    09    10    11    12    13    14    15*/
0x08, 0x08, 0x08, 0x11, 0x22, 0x33, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x01, 0x81, 0x00, 0x60, 0x0A,
0x08, 0x00, 0x45, 0x00, 0x00, 0x8A, 0x00, 0x01, 0x00, 0x00, 0x40, 0x11, 0xE1, 0x0E, 0xC0, 0xA8,
0x0C, 0x02, 0xC0, 0xA8, 0x0C, 0x01, 0x00, 0x07, 0x00, 0x07, 0x00, 0x76, 0x8D, 0xC8, 0x2A, 0x53,
0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45,
0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A,
0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C,
0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47,
0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47,
0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41,
0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x8D, 0x49, 0x42, 0x7E
};

const uint8 pkt2[] = {
/*
  00    01    02    03    04    05    06    07    08    09    10    11    12    13    14    15*/
0x09, 0x09, 0x09, 0x11, 0x22, 0x33, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x02, 0x81, 0x00, 0x60, 0x14,
0x08, 0x00, 0x45, 0x00, 0x00, 0x8A, 0x00, 0x01, 0x00, 0x00, 0x40, 0x11, 0xE1, 0x0E, 0xC0, 0xA8,
0x0C, 0x02, 0xC0, 0xA8, 0x0C, 0x01, 0x00, 0x07, 0x00, 0x07, 0x00, 0x76, 0x8D, 0xC8, 0x2A, 0x53,
0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45,
0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A,
0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C,
0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47,
0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47,
0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41,
0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x8D, 0x49, 0x42, 0x7E
};

const uint8 pkt3[] = {
/*
  00    01    02    03    04    05    06    07    08    09    10    11    12    13    14    15*/
0x0a, 0x0a, 0x0a, 0x11, 0x22, 0x33, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x03, 0x81, 0x00, 0x60, 0x1E,
0x08, 0x00, 0x45, 0x00, 0x00, 0x8A, 0x00, 0x01, 0x00, 0x00, 0x40, 0x11, 0xE1, 0x0E, 0xC0, 0xA8,
0x0C, 0x02, 0xC0, 0xA8, 0x0C, 0x01, 0x00, 0x07, 0x00, 0x07, 0x00, 0x76, 0x8D, 0xC8, 0x2A, 0x53,
0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45,
0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A,
0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C,
0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47,
0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47,
0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41,
0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x8D, 0x49, 0x42, 0x7E
};

/* Use "sizeof" to get packet size */
const int   pkt1_SIZEOF = sizeof(pkt1);
const int   pkt2_SIZEOF = sizeof(pkt2);
const int   pkt3_SIZEOF = sizeof(pkt3);

/* Simple wrapper function used to transmit test packet */
bcm_error_t
test_harness(int unit)
{
    bcm_error_t rv=0; 
    rv = tx_pkt_data(unit, pkt1, pkt1_SIZEOF);
    rv = tx_pkt_data(unit, pkt2, pkt2_SIZEOF);
    rv = tx_pkt_data(unit, pkt3, pkt3_SIZEOF);
    return rv;
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

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : Cpu Tx Proc DGPP 
 *
 *  Usage    : BCM.0> cint cpu_tx_proc_dgpp.c
 *
 *  config   : cpu_tx_proc_config.bcm
 *
 *  Log file : cpu_tx_proc_dgpp_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+  ingress_port
 *                   |                              +-----------------+
 *                   |                              |
 *                   |                              |
 *  cpu_port         |                              |  
 *  +----------------+          SWITCH              +
 *                   |                              |
 *                   |                              |
 *                   |                              |  egress_port
 *                   |                              +-----------------+
 *                   |                              |
 *                   +------------------------------+
 *  Summary:
 *  ========
 *    This CINT script demonstrates how to send packets using cpu_tx_proc header.
 *    This script will send packet from CPU using cpu_tx_proc header with destination 
 *    type as "DGPP". This will mimic as if packet is ingress from a front panel 
 *    port and egress out of another front panel.
 *
 *    Detailed steps done in the CINT script:
 *    =====================================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one ingress, one egress port and configure them in loopback mode.
 *      b) Create Vlan 10 and add ingress, egress port and CPU port to vlan 10.
 *
 *    2) Step2 - Configuration (Done in cpu_tx_proc_dgpp_setup())
 *    ============================================
 *      a) There is nothing to be done in tx_setup
 *
 *    3) Step3 - Verification(Done in verify())
 *    ==========================================
 *      a) Transmit one packet with vlan=10 using bcm_tx() API
 *      b) Expected Result:
 *      ===================
 *         The packet will be injected into pipeline with ingress port as source port and 
 *         transmitted to egress ports.
 *         CPU_MASQUERADE_COUNTER_DEST_TYPE_PIPE0(1) register count will increment by 1.  
 *         show c command will dump packet count on ingress and egress port.
*/

cint_reset();
bcm_port_t egress_port, ingress_port;


/*
 *
 * Sends a single packet with cpu_tx_proc header
 * to the switch subject to ordinary pipeline processing.
 *
 * Parameters:
 *   unit: tx device unit number
 *   pktData: Complete packet
 *   length: Size of commplete packet in bytes.
 */
bcm_error_t
cpu_tx_proc_dgpp_pkt_send(int unit, uint8 *pktData, int length)
{
    bcm_pkt_t  *pkt;
    bcm_error_t rv = BCM_E_NONE;

    /*
     * Allocate a packet structure and associated DMA packet data buffer.
     * Set Tx flags:
     *    BCM_TX_CRC_REGEN: Regenerate packet CRC.
     */
    rv = bcm_pkt_alloc(unit, length, BCM_TX_CRC_REGEN, &pkt);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_pkt_alloc(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Add cpu_tx_proc header by setting flags2 to BCM_PKT_F2_CPU_TX_PROC
       and set destination type and destination port, source port */
    pkt->flags2 = BCM_PKT_F2_CPU_TX_PROC;
    pkt->flags &= (~BCM_TX_ETHER);
    pkt->txprocmh_destination_type = 0; /*SOC_TX_PROC_OP_DGPP*/
    pkt->rx_port = ingress_port;
    pkt->txprocmh_destination = egress_port;

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

    int i=0,port=0;
    bcm_error_t rv = BCM_E_NONE;
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

    return rv;

}

bcm_error_t test_setup(int unit)
{
    const bcm_vlan_t vlan = 10;
    char        command[128];   /* Buffer for diagnostic shell commands */
    bcm_error_t rv = BCM_E_NONE;

    /* Pick arbitrary dest MAC Address */
    const bcm_mac_t dest = {0x08, 0x08, 0x08, 0x11, 0x22, 0x33};
    int port_list[2];

    rv = portNumbersGet(unit, port_list, 2);
 
    if (BCM_FAILURE(rv)) {
        printf("portNumbersGet() failed\n");
        return rv;
    }

    egress_port = port_list[0];
    ingress_port = port_list[1];

    /* Put all ports into loopback */ 
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_MAC));

    /* Create vlan and add ports */
    sprintf(command, "vlan create %d PortBitMap=cpu,%d", vlan,egress_port);
    bshell(unit, command);

    return rv;
}

/* Tx Setup */
bcm_error_t cpu_tx_proc_dgpp_setup(int unit)
{
    return BCM_E_NONE;
}


/*******************************************************************************
 * Test Harness Follows
 *
 * Define the test packet for transmission:
 *   Singled tagged, VLAN=10, packet priority=3
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


/* Use "sizeof" to get packet size */
const int   pkt1_SIZEOF = sizeof(pkt1);

/* Simple wrapper function used to transmit test packet */
bcm_error_t
test_harness(int unit)
{
    bcm_error_t rv=0; 
    rv = cpu_tx_proc_dgpp_pkt_send(unit, pkt1, pkt1_SIZEOF);
    return rv;
}

/* Run the test, save and possibly display error status */
bcm_error_t verify(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    rv = test_harness(0);
    bshell(unit, "g CPU_MASQUERADE_COUNTER_DEST_TYPE_PIPE0(1)");
    sal_usleep(1000000);
    bshell(unit, "show c");
    return rv;
}

bcm_error_t execute()
{
    bcm_error_t rv = BCM_E_NONE;
    int unit =0;
    bshell(unit, "config show; a ; version");
    rv = test_setup(unit); 
    if (BCM_FAILURE(rv)) {
        printf("test_setup() failed.\n");
        return rv;
    }
    
    rv = cpu_tx_proc_dgpp_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("Cpu Tx Proc DGPP Setup Failed\n");
        return rv;
    }
    
    rv = verify(unit);
    if (BCM_FAILURE(rv)) {
        printf("Cpu Tx Proc DGPP verify Failed\n");
        return rv;
    }

    return BCM_E_NONE;
}

print execute();


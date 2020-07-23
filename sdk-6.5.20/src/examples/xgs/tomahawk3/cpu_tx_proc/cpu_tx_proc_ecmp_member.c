/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : Cpu Tx Proc ECMP member 
 *                                   
 *  Usage    : BCM.0> cint cpu_tx_proc_ecmp_member.c
 *                                                 
 *  config   : cpu_tx_proc_config.bcm              
 *                                                 
 *  Log file : cpu_tx_proc_ecmp_member_log.txt      
 *                                                 
 *  Test Topology :                                
 *                                                 
 *                   +------------------------------+  ingress_port
 *                   |                              +-----------------+
 *                   |                              |                  
 *                   |                              |                  
 *  cpu_port         |                              |  egress_port_1   
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |                  
 *                   |                              |  egress_port_2   
 *                   |                              +-----------------+
 *                   |                              |                  
 *                   |                              |  egress_port_3_  
 *                   |                              +-----------------+
 *                   +------------------------------+                  
 *  Summary:                                                           
 *  ========                                                           
 *    This CINT script demonstrates how to send packets using cpu_tx_proc header.
 *    This script will send packet from CPU using cpu_tx_proc header with destination
 *    type as "ECMP Member". This will mimic as if packet is ingress from a front panel
 *    port destined for ECMP Member and ingress pipeline will send out of that particular ECMP
 *    member egress port.                                                               
 *                                                                                      
 *    Detailed steps done in the CINT script:                                           
 *    =====================================================                             
 *    1) Step1 - Test Setup (Done in test_setup())                                      
 *    ============================================                                      
 *      a) Select one ingress, three egress ports and configure them in loopback mode.  
 *      b) Create Vlan 10 and add 3 egress ports and CPU port.
 *      c) Create ECMP group with 3 egress port as members.                             
 *                                                                                      
 *    2) Step2 - Configuration (Done in cpu_tx_proc_ecmp_member_setup())                 
 *    ============================================                                      
 *      a) There is nothing to be done in cpu_tx_proc_ecmp_member_setup                  
 *                                                                                      
 *    3) Step3 - Verification(Done in verify())                                         
 *    ==========================================                                        
 *      a) Transmit one packet with dest_ip=10.10.10.10, vlan=10 and destination type as "ecmp member"
 *         using bcm_tx() API                                                                        
 *      b) Transmit one packet with dest_ip=10.10.10.11, vlan=10 and destination type as "ecmp member"
 *         using bcm_tx() API                                                                        
 *      c) Transmit one packet with dest_ip=10.10.10.12, vlan=10 and destination type as "ecmp member"
*         using bcm_tx() API                                                                        
 *      d) Expected Result:                                                                          
 *      ===================                                                                          
 *         All 3 packets will be injected into pipeline with ingress port as source port and         
 *         transmitted to "ecmp group". The HW logic will pick the same ecmp member(2) as egress port.                                                                               
 *         CPU_MASQUERADE_COUNTER_DEST_TYPE_PIPE0(4) register count will increment by 3.                 
 *         show c command will dump packet count on ingress and egress port.                             
*/                                                                                                       

cint_reset();
bcm_port_t egress_port[3], ingress_port;
uint32 ecmp_group_id;                   

/*
 *
 * Sends a single packet with cpu_tx_proc header
 * to the switch subject to ordinary pipeline processing.
 *                                                       
 * Parameters:                                           
 *   unit: tx device unit number                         
 *   ecmp_group_id : ECMP group index
 *   ecmp_member : ECMP group memebr id.                                
 *   pktData: Complete packet                            
 *   length: Size of commplete packet in bytes.          
 */                                                      
bcm_error_t                                              
cpu_tx_proc_ecmp_member_pkt_send(int unit, uint32 ecmp_group_id, 
                              uint32 ecmp_member, uint8 *pktData, int length)
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
    pkt->flags |= BCM_PKT_F_ROUTED;                                      
    pkt->txprocmh_destination_type = 3; /*SOC_TX_PROC_OP_ECMP_MEMBER*/           
    pkt->rx_port = ingress_port;
    pkt->txprocmh_ecmp_group_index = ecmp_group_id;                                         
    pkt->txprocmh_ecmp_member_index = ecmp_member;                           

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
    bcm_if_t intf_id = 0;
    bcm_l3_intf_t l3_intf[3];
    bcm_vrf_t vrf = 0;
    bcm_l3_egress_t l3_egress[3];
    bcm_if_t egr_obj[3];
    bcm_l3_egress_ecmp_t  ecmp_group;
                                
    int port_list[4];

    rv = portNumbersGet(unit, port_list, 4);
                                            
    if (BCM_FAILURE(rv)) {
        printf("portNumbersGet() failed\n");
        return rv;
    }

    egress_port[0] = port_list[0];
    egress_port[1] = port_list[1];
    egress_port[2] = port_list[2];
    ingress_port = port_list[3];

    /* Put all ports into loopback */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port[0], BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port[1], BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port[2], BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_MAC));

    /* Create vlan and add ports */
    sprintf(command, "vlan create %d PortBitMap=cpu,%d,%d,%d",
            vlan,egress_port[0],egress_port[1],egress_port[2]);
    bshell(unit, command);
   
    sprintf(command, "port %d,%d,%d discard=all", egress_port[0], egress_port[1], egress_port[2]); 
    bshell(unit, command); 
   /* Enable egress mode */
    bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1);

    /* create L3 interfaces*/
    bcm_l3_intf_t_init(&l3_intf[0]);
    l3_intf[0].l3a_mac_addr[0] = 0x0;
    l3_intf[0].l3a_mac_addr[1] = 0x0;
    l3_intf[0].l3a_mac_addr[2] = 0x0;
    l3_intf[0].l3a_mac_addr[3] = 0x0;
    l3_intf[0].l3a_mac_addr[4] = 0xaa;
    l3_intf[0].l3a_mac_addr[5] = 0xaa;
    l3_intf[0].l3a_vid = vlan;
    l3_intf[0].l3a_vrf = vrf;
    rv = bcm_l3_intf_create(unit, &l3_intf[0]);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_intf_create() failed\n");
        return rv;
    }

    bcm_l3_intf_t_init(&l3_intf[1]);
    l3_intf[1].l3a_mac_addr[0] = 0x0;
    l3_intf[1].l3a_mac_addr[1] = 0x0;
    l3_intf[1].l3a_mac_addr[2] = 0x0;
    l3_intf[1].l3a_mac_addr[3] = 0x0;
    l3_intf[1].l3a_mac_addr[4] = 0xbb;
    l3_intf[1].l3a_mac_addr[5] = 0xbb;
    l3_intf[1].l3a_vid = vlan;
    l3_intf[1].l3a_vrf = vrf;
    rv = bcm_l3_intf_create(unit, &l3_intf[1]);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_intf_create() failed\n");
        return rv;
    }
     
    bcm_l3_intf_t_init(&l3_intf[2]);
    l3_intf[2].l3a_mac_addr[0] = 0x0;
    l3_intf[2].l3a_mac_addr[1] = 0x0;
    l3_intf[2].l3a_mac_addr[2] = 0x0;
    l3_intf[2].l3a_mac_addr[3] = 0x0;
    l3_intf[2].l3a_mac_addr[4] = 0xcc;
    l3_intf[2].l3a_mac_addr[5] = 0xcc;
    l3_intf[2].l3a_vid = vlan;        
    l3_intf[2].l3a_vrf = vrf;         
    rv = bcm_l3_intf_create(unit, &l3_intf[2]);
    if (BCM_FAILURE(rv)) {                     
        printf("bcm_l3_intf_create() failed\n");
        return rv;                              
    }                                           


    /* Create Egress objects */
    bcm_l3_egress_t_init(&l3_egress[0]);
    l3_egress[0].intf     = l3_intf[0].l3a_intf_id;
    l3_egress[0].mac_addr[0] =0x0;                 
    l3_egress[0].mac_addr[1] =0x0;                 
    l3_egress[0].mac_addr[2] =0x0;                 
    l3_egress[0].mac_addr[3] =0x0;                 
    l3_egress[0].mac_addr[4] =0x11;                
    l3_egress[0].mac_addr[5] =0x11;                
    l3_egress[0].port     =  egress_port[0];       
    rv = bcm_l3_egress_create(unit, 0 , &l3_egress[0], &egr_obj[0]);
    if (BCM_FAILURE(rv)) {                                          
        printf("bcm_l3_egress_create() failed\n");                  
        return rv;                                                  
    }                                                               

    bcm_l3_egress_t_init(&l3_egress[1]);
    l3_egress[1].intf     = l3_intf[1].l3a_intf_id;
    l3_egress[1].mac_addr[0] =0x0;                 
    l3_egress[1].mac_addr[1] =0x0;                 
    l3_egress[1].mac_addr[2] =0x0;                 
    l3_egress[1].mac_addr[3] =0x0;                 
    l3_egress[1].mac_addr[4] =0x22;                
    l3_egress[1].mac_addr[5] =0x22;                
    l3_egress[1].port     =  egress_port[1];       
    rv = bcm_l3_egress_create(unit, 0 , &l3_egress[1], &egr_obj[1]);
    if (BCM_FAILURE(rv)) {                                          
        printf("bcm_l3_egress_create() failed\n");                  
        return rv;                                                  
    }                                                               

    bcm_l3_egress_t_init(&l3_egress[2]);
    l3_egress[2].intf     = l3_intf[2].l3a_intf_id;
    l3_egress[2].mac_addr[0] =0x0;                 
    l3_egress[2].mac_addr[1] =0x0;                 
    l3_egress[2].mac_addr[2] =0x0;                 
    l3_egress[2].mac_addr[3] =0x0;                 
    l3_egress[2].mac_addr[4] =0x33;                
    l3_egress[2].mac_addr[5] =0x33;                
    l3_egress[2].port     =  egress_port[2];       
    rv = bcm_l3_egress_create(unit, 0 , &l3_egress[2], &egr_obj[2]);
    if (BCM_FAILURE(rv)) {                                          
        printf("bcm_l3_egress_create() failed\n");                  
        return rv;                                                  
    }                                                               

    /* Create ECMP ggroup */
    bcm_l3_egress_ecmp_t_init(&ecmp_group);
    ecmp_group.dynamic_mode=0;             
    ecmp_group.max_paths = 44;             
    rv = bcm_l3_egress_ecmp_create(unit,&ecmp_group,3,egr_obj);
    if (BCM_FAILURE(rv)) {                                     
        printf("bcm_l3_egress_ecmp_create() failed\n");        
        return rv;                                             
    }                                                          
    ecmp_group_id = ecmp_group.ecmp_intf;                      

    /* Create L3 route */
    bcm_l3_route_t   route;
    bcm_l3_route_t_init(&route);
    route.l3a_flags = BCM_L3_MULTIPATH;
    route.l3a_vrf = 0;                 
    route.l3a_intf = ecmp_group.ecmp_intf;
    route.l3a_subnet = 0x0a0a0a00;        
    route.l3a_ip_mask = 0xffffff00;       
    rv =  bcm_l3_route_add(unit, &route); 
    if (BCM_FAILURE(rv)) {                
        printf("bcm_l3_route_add() failed\n");
        return rv;                            
    }                                         

    return rv;

}

/* Tx Setup */
bcm_error_t cpu_tx_proc_ecmp_member_setup(int unit)
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
0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x01, 0x81, 0x00, 0x60, 0x0a,
0x08, 0x00, 0x45, 0x00, 0x00, 0x8A, 0x00, 0x01, 0x00, 0x00, 0x40, 0x11, 0xE1, 0x0E, 0x0a, 0x0a, 
0x0a, 0x0a, 0xC0, 0xA8, 0x0C, 0x01, 0x00, 0x07, 0x00, 0x07, 0x00, 0x76, 0x8D, 0xC8, 0x2A, 0x53,
0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 
0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 
0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 
0x0a, 0x0a, 0x0a, 0x0a, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 
0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 
0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 
0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x8D, 0x49, 0x42, 0x7E                          
};                                                                                              

const uint8 pkt2[] = {
/* 
  00    01    02    03    04    05    06    07    08    09    10    11    12    13    14    15*/
0x00, 0x00, 0x00, 0x00, 0x00, 0xbb, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x01, 0x81, 0x00, 0x60, 0x0a,
0x08, 0x00, 0x45, 0x00, 0x00, 0x8A, 0x00, 0x01, 0x00, 0x00, 0x40, 0x11, 0xE1, 0x0E, 0x0a, 0x0a,
0x0a, 0x0b, 0xC0, 0xA8, 0x0C, 0x01, 0x00, 0x07, 0x00, 0x07, 0x00, 0x76, 0x8D, 0xC8, 0x2A, 0x53,
0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45,
0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A,
0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C,
0x0a, 0x0a, 0x0a, 0x0a, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47,
0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47,
0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41,
0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x8D, 0x49, 0x42, 0x7E                         
};                                                                                             
const uint8 pkt3[] = {                                                                         
/*                                                                                             
  00    01    02    03    04    05    06    07    08    09    10    11    12    13    14    15*/
0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x01, 0x81, 0x00, 0x60, 0x0a,
0x08, 0x00, 0x45, 0x00, 0x00, 0x8A, 0x00, 0x01, 0x00, 0x00, 0x40, 0x11, 0xE1, 0x0E, 0x0a, 0x0a, 
0x0a, 0x0c, 0xC0, 0xA8, 0x0C, 0x01, 0x00, 0x07, 0x00, 0x07, 0x00, 0x76, 0x8D, 0xC8, 0x2A, 0x53,
0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 
0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 
0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 
0x0a, 0x0a, 0x0a, 0x0a, 0x47, 0x2A, 0x53, 0x49, 0x4E, 0x47, 0x4C, 0x45, 0x2D, 0x54, 0x41, 0x47, 
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
    rv = cpu_tx_proc_ecmp_member_pkt_send(unit, ecmp_group_id, 2, pkt1, pkt1_SIZEOF);
    rv = cpu_tx_proc_ecmp_member_pkt_send(unit, ecmp_group_id, 2, pkt2, pkt2_SIZEOF);
    rv = cpu_tx_proc_ecmp_member_pkt_send(unit, ecmp_group_id, 2, pkt3, pkt3_SIZEOF);
    return rv;                                                  
} 

/* Run the test, save and possibly display error status */
bcm_error_t verify(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    rv = test_harness(0);
    bshell(unit, "g CPU_MASQUERADE_COUNTER_DEST_TYPE_PIPE0(4)");
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

    rv = cpu_tx_proc_ecmp_member_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("Cpu Tx Proc ECMP Group Setup Failed\n");
        return rv;
    }

    rv = verify(unit);
    if (BCM_FAILURE(rv)) {
        printf("Cpu Tx Proc ECMP Group verify Failed\n");
        return rv;
    }

    return BCM_E_NONE;
}

print execute();



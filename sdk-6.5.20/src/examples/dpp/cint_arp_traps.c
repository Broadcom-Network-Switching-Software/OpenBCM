/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


/* 
how to run:
 
cint;
cint_reset();
exit;
cint ../../../../src/examples/dpp/cint_trap_mgmt.c
cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
cint ../../../../src/examples/dpp/cint_ip_route.c
cint ../../../../src/examples/dpp/cint_arp_traps.c
cint
 
trap ARP reply on specific incomming router interface 
arp_reply_with_mymac(unit,13,BCM_GPORT_BLACK_HOLE);
 
 Temp fixes 
mod IHB_FLP_PROGRAM_SELECTION_CAM 12 1 program=1
mod IHP_VTT_2ND_KEY_PROG_SEL_TCAM 21 1 PROGRAM=11 PACKET_FORMAT_QUALIFIER_2_MASK=0x7ff PACKET_FORMAT_QUALIFIER_1=0x500
mod IHP_VTT_2ND_LOOKUP_PROGRAM_1 11 1 PROCESSING_CODE=7
 
trap ARP request on specific incomming port
arp_request(unit,13,BCM_GPORT_BLACK_HOLE,1);
 
trap ARP request with TPA = MY-IP 
arp_reply_request_with_my_ip(unit,13,BCM_GPORT_BLACK_HOLE);
 
trap any ARP packet on specific incomming port
arp_request(unit,13,BCM_GPORT_BLACK_HOLE,0);
 
 
*/ 


/* 
BCM API: 
 
RX-traps: 
    bcmRxTrapArpReply: ARP reply with My MAC
    bcmRxTrapArp: ARP request, if DA ignored then capture also ARP request
    bcmRxTrapArpMyIp: ARP request with TPA = My-IP
 
    Global switch controls

    bcmSwitchArpReplyMyStationL2ToCPU: ignore ARP trap, relevant for bcmRxTrapArp.
    bcmSwitchArpRequestMyStationIPToCPU: add IP address to be as considered my IP for ARP requests. relevant for bcmRxTrapArpMyIp.
    bcmSwitchArpRequestMyStationIPFwd: remove above IP .
    
    port control:
    bcmSwitchArpRequestToCpu: enable ARP trap on incomming port relevant for bcmRxTrapArp,bcmRxTrapArpMyIp.
 
 */

/* debug prints */



/* 
 * trap ARP reply to BLACK_HOLE: 
 *  identification:
 *      ARP-packet: Ethernet packet with Ether-Type = 0x806.
 *      Recieved on L3-interface with My-MAC i.e. has My-MAC on the resolved VSI.
 *  
 *  handling:
 *      - using rx-trap bcmRxTrapArpReply
 */
int arp_reply_with_mymac(int unit, int in_port, int dest){
    int rv;
    int ing_intf_in;
    int in_vlan = 15;
    int vrf = 1;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    int trap_strength = 6;/* set stronger than arp_request, as pkt may captured with arp_request() when only_bc_pkts = 0*/

    /*** create ingress router interface: identified according to <VLAN,MAC> ***/
    rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
        return rv;
    }
    
    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, create egress interface-1, in_sysport=%d, in unit %d \n", in_sysport, unit);
        return rv;
    }
    
    /* configure ARP reply trap to trap to dest */
    rv = trap_type_set(unit, bcmRxTrapArpReply, trap_strength, dest);
    if (rv != BCM_E_NONE) {
        printf("Error, setting trap: bcmRxTrapArpReply, \n");
        return rv;
    }
    
    return rv;
}



/* 
 * trap ARP request to BLACK_HOLE: 
 *  identification:
 *      ARP-packet: Ethernet packet with Ether-Type = 0x806, 
 *      trap enabled in in_port.
 *      if only_bc_pkts = 1 --> DA = BC (all ones)
 *      if only_bc_pkts = 0 --> ANY DA (CAPTURE BOTH REQUEST AND REPLY)
 *  
 *  handling:
 *      - using rx-trap bcmRxTrapArpReply
 */
int arp_request(int unit, int in_port, int dest, int only_bc_pkts){
    int rv;
    int trap_strength = 5;
    int enable = 1;
    
    /* enable ARP trap on incomming port */
    rv = bcm_switch_control_port_set( unit, in_port, bcmSwitchArpRequestToCpu, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_port_set \n");
        return rv;
    }

    /* configure ARP request to trap to dest */
    rv = trap_type_set(unit, bcmRxTrapArp, trap_strength, dest);
    if (rv != BCM_E_NONE) {
        printf("Error, setting trap: bcmRxTrapArp, \n");
        return rv;
    }

    /* trap ARP packet regardless DA */
    rv = bcm_switch_control_set( unit, bcmSwitchArpReplyMyStationL2ToCPU, only_bc_pkts);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }

    return rv;
}


/* 
 * trap ARP request with TPA = myIP
 *  identification:
 *      ARP-packet: Ethernet packet with Ether-Type = 0x806, DA = BC (all ones)
 *      trap enabled in in_port (same enable as arp_reply_request)
 *      ARP.TPA = specific IP address
 *  handling:
 *      - using rx-trap bcmRxTrapArpMyIp
 */
int arp_reply_request_with_my_ip(int unit, int in_port, int dest){
    int rv;
    int trap_strength = 6;
    int my_ip = 0x0a000505; /* 10.0.5.5 */
    int enable = 1;
    
    /* enable ARP request trap on incomming port */
    rv = bcm_switch_control_port_set( unit, in_port, bcmSwitchArpRequestToCpu, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_port_set \n");
        return rv;
    }

    /* enable ARP request trap on incomming port */
    rv = bcm_switch_control_set( unit, bcmSwitchArpRequestMyStationIPToCPU, my_ip);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_port_set \n");
        return rv;
    }

    /* configure ARP request to trap to out-port */
    rv = trap_type_set(unit, bcmRxTrapArpMyIp, trap_strength, dest);
    if (rv != BCM_E_NONE) {
        printf("Error, setting trap: bcmRxTrapArp, \n");
        return rv;
    }

    return rv;
}


/*
Function which destroys the configured trap.
*/
int
arp_trap_destroy(int unit, bcm_rx_trap_t trap_type) {
   	int rv = BCM_E_NONE;
    int trap_id;

    /*get trap id*/
    rv = bcm_rx_trap_type_get(unit, 0, trap_type, &trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_rx_trap_type_get, returned %d\n", rv);
        return rv;
    }

    /*delete trap*/
	rv = bcm_rx_trap_type_destroy(unit, trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_rx_trap_type_destroy, returned %d\n", rv);
        return rv;
    }

    return rv;
}
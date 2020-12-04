/* 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                        . . .   . . .      . . .   . . .        |
 *  |                               . . .   .     . .     .   .       .       .      |                     
 *  |   Access               . .   .      .                 .                  .     |                      
 *  |                      .     .                                              .    |                      
 *  |                    .                    +--------+                        .    |                                 
 *  |   +--------+     .                   / /        /|                         .   |
 *  |  /        /|    .                  /  +--------+ |                         .   |
 *  | +--------+ |    .                /    |  P1    | |                        .    |
 *  | |  C1    | |\    .             /      |        |/                         .    |
 *  | |        |/  \    +--------+ /        +--------+                          .    |
 *  | +--------+    \  /        /|          +--------+                         .     |        
 *  |                \+--------+ |         /        /|                          .    |                   
 *  |                 |  PE    | |        +--------+ |    Provider Cloud        .    |       
 *  |                 |        |/ ------- |  P2    | |                          .    |       
 *  |                 +--------+ \        |        |/                             .  |        
 *  |                 /.          \       +--------+                              .  |             
 *  |                / .           \                                             .   |
 *  |               / .             \  +--------+                                .   |
 *  |              /  .              \/        /|                                .   |   
 *  |             /    . . .         +--------+ |                               .    |
 *  |   +--------+         .         |  P3    | |                   .           .    |
 *  |  /        /|          .     .  |        |/                  .   .       .      |                  
 *  | +--------+ |            . .   .+--------+       .          .      . . .        |  
 *  | |  C2    | |                   . . .    .     .   .       .                    | 
 *  | |        |/                         . .   . .       . . .                      |  
 *  | +--------+                                                                     |  
 *  |                                                                                |
 *  |                                                                                |  
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/* 
 * The below CINT demonstrates IPMC over MPLS L3VPN with BCM886XX being PE-based MLDP.
 * Available for 88660_A0 and above.
 * The MLDP model consists of three kind of devices:
 * 1. Customer Edge device (C): Resides in customer network and connected with one or more interfaces to provide edge device.
 * The C device is not familiar with the mpls application.
 * 2. Provide Edge device (PE): Resides in service provider network and connects to one or more C devices.
 * PE is responsible for receiving and sending traffic from / to the core network between the C and P.
 * 3. Provider device (P): Backbone router in a service provider network. Only in charge of  mpls routing in the backbone and not familiar with the IPMC processing.
 *
 *
 * We are configuring PE. 

 * 
 * 
 * Traffic:
 *   - MC packets:
 *     - Packet Incoming from mpls network P1, to P2, P3, C1, C2 (Bud node)
 *     - Packet Incoming from mpls network, to P2, P3 (Continue)
 *     - Packet Incoming from mpls network, to C1, C2 (Leaf node)
 *
 * Packet Incoming from provider network P1, to P2, P3, C1, C2 (Bud node: provider network mpls MC and access IPMC)
 * Purpose: - check mpls MC is done at provider metwork 
 *          - check tunnel termination (of mpls) is done, to perform IPMC at the access network
 *          - check IPMC is done at access network for IP compatible MC at the inner IP.
 *    Send:  
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | p1_vlan_my_mac    | p1_mac            | p1_vlan       |
 *             | 00:00:00:00:00:13 | 00:00:00:00:00:12 | 20            |
 *             ----------------------------------------------------------
 *                 --------------
 *         mpls:   |   Label     | 
 *                 --------------
 *                 |   5000      |
 *                 ---------------
 *                  
 *         ipmc: 
 *                   
 *                       --------------------------    
 *                   ip: |   SIP       | DIP       |    
 *                       --------------------------    
 *                       |some host ip | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------    
 *   Receive:
 *
 *    - receive 2 packets at P2 and P3:
 *    P2:
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | p2_mac            | p2_p3_vlan_my_mac | p2_p3_vlan     |
 *             | 00:00:00:00:00:14 | 00:00:00:00:00:14 | 25             |
 *             ----------------------------------------------------------  
 *                 -------------- --------------
 *         mpls:   |   Label 1   |   Label 2    |
 *                 -------------- --------------
 *                 |   6000      |    300       |
 *                 -----------------------------
 *
 *                  
 *                   
 *                       --------------------------    
 *         ipmc:         |   SIP       | DIP       |    
 *                       --------------------------    
 *                       |some host ip | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------    
 *    P3:
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | p3_mac            | p2_p3_vlan_my_mac | p2_p3_vlan     |
 *             | 00:00:00:00:00:15 | 00:00:00:00:00:14 | 25             |
 *             ----------------------------------------------------------  
 *                 -------------- --------------
 *         mpls:   |   Label 1   |   Label 2    |
 *                 -------------- --------------
 *                 |   6000      |    200       |
 *                 -----------------------------
 *
 *                  
 *                   
 *                       --------------------------    
 *         ipmc:         |   SIP       | DIP       |    
 *                       --------------------------    
 *                       |some host ip | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------     
 *     receive 2 packets at C1 and C2
 *             -----------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN            |
 *             -----------------------------------------------------------
 *             | access_MC         | c1_c2_vlan_my_mac | c1_c2_vlan      |
 *             | 01:00:5E:06:06:06 | 00:00:00:00:00:19 | 30              |
 *             ----------------------------------------------------------- 
 *                
 *                  --------------------------           
 *              ip: |   SIP     | DIP         |           
 *                  --------------------------           
 *                  | host90_ip |  access_MC  |
 *                  | 90.90.90.90 | 224.6.6.6 | 
 *                  --------------------------        
 *
 * Packet flow:
 * - get the VSI from the VLAN
 * - identify mpls MC packet (according to ILM (ingress label mapping) entry) 
 * - packet is forwarded according to mpls MC
 * - packet is replicated to network: port + outrif 
 *   - new link layer is built (DA is p2,p3 mac addresses, SA is p2_p3 vlan mymac, VLAN = VSI = outRif is p2_p3 vlan)
 * - packet is replicated to recycle port: port + outRif.
 *   - at 2nd pass:
 *     - get the VSI from the VLAN                                                                                           
 *     - mpls tunnel termination.                                
 *     - get the inRif (=VSI)                                                                                                
 *     - packet is forward according to MC                                                                                   
 *     - packet is replicated to access: port + outRif                                                                       
 *     - new link layer is built (DA compatible MC is built from DIP, SA is built from outRif.myMac, VLAN = VSI = outRif)    
 *
 *
 *
 *
 *
 * Packet Incoming from provider network, to P2, P3 (Continue: provider to provider) 
 * Purpose: - check MC is done at provider metwork for mpls MC
 *
 *
 *    Send:  
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | p1 mac            | p1_vlan_my_mac    | p1_vlan       |
 *             | 00:00:00:00:00:12 | 00:00:00:00:00:13 | 20            |
 *             ----------------------------------------------------------
 *                 --------------
 *         mpls:   |   Label     | 
 *                 --------------
 *                 |   5001      |
 *                 ---------------
 *                  
 *         ipmc: 
 *                   
 *                       --------------------------    
 *                   ip: |   SIP       | DIP       |    
 *                       --------------------------    
 *                       |some host ip | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------   
  *   Receive:
 *
 *    - receive 2 packets at P2 and P3:
 *    P2:
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | p2_mac            | p2_p3_vlan_my_mac | p2_p3_vlan     |
 *             | 00:00:00:00:00:14 | 00:00:00:00:00:14 | 25             |
 *             ----------------------------------------------------------  
 *                 -------------- --------------
 *         mpls:   |   Label 1   |   Label 2    |
 *                 -------------- --------------
 *                 |   6000      |    300       |
 *                 -----------------------------
 *
 *                  
 *                   
 *                       --------------------------    
 *         ipmc:         |   SIP       | DIP       |    
 *                       --------------------------    
 *                       |some host ip | access_MC |
 *                       | 90.90.90.90 | 224.7.7.7|
 *                       --------------------------    
 *    P3:
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | p3_mac            | p2_p3_vlan_my_mac | p2_p3_vlan     |
 *             | 00:00:00:00:00:15 | 00:00:00:00:00:14 | 25             |
 *             ----------------------------------------------------------  
 *                 -------------- --------------
 *         mpls:   |   Label 1   |   Label 2    |
 *                 -------------- --------------
 *                 |   6000      |    200       |
 *                 -----------------------------
 *
 *                  
 *                   
 *                       --------------------------    
 *         ipmc:         |   SIP       | DIP       |    
 *                       --------------------------    
 *                       |some host ip | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       -------------------------- 
 *
 * Packet flow:
 * - get the VSI from the VLAN
 * - identify mpls MC packet (according to ILM (ingress label mapping) entry)
 * - packet is forward according to mpls MC
 * - packet is replicated to network: port + outrif 
 *   - new link layer is built (DA is p2,p3 mac addresses, SA is p2_p3 vlan mymac, VLAN = VSI = outRif is p2_p3 vlan)
 *
 *
 *
 * 
 *
 *
 * Packet Incoming from provider network, to C1, C2 (leaf node: provider to access):
 *  Purpose: - check mpls tunnel termination  is done, to perform IPMC at the access network
 *           - check IPMC is done at access network for IP compatible MC at the inner IP
 *           - check no replication is done for provider network
 * 
 *    Send:  
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | p1 mac            | p1_vlan_my_mac    | p1_vlan       |
 *             | 00:00:00:00:00:12 | 00:00:00:00:00:13 | 20            |
 *             ----------------------------------------------------------
 *                 --------------
 *         mpls:   |   Label     | 
 *                 --------------
 *                 |   5002      |
 *                 ---------------
 *                  
 *         ipmc: 
 *                   
 *                       --------------------------    
 *                   ip: |   SIP       | DIP       |    
 *                       --------------------------    
 *                       |some host ip | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       -------------------------- 
 *
 *  Receive:
 *     receive 2 packets at C1 and C2
 *             -----------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN            |
 *             -----------------------------------------------------------
 *             | access_MC         | c1_c2_vlan_my_mac | c1_c2_vlan      |
 *             | 01:00:5E:06:06:06 | 00:00:00:00:00:19 | 30              |
 *             ----------------------------------------------------------- 
 *                
 *                  --------------------------           
 *              ip: |   SIP     | DIP         |           
 *                  --------------------------           
 *                  | host90_ip |  access_MC  |
 *                  | 90.90.90.90 | 224.6.6.6 | 
 *                  --------------------------      
 *      
 *
 * Packet flow:
 * - get the VSI from the VLAN
 * - identify mpls MC packet (according to ILM (ingress label mapping) entry)
 * - packet is forwarded according to mpls MC
 * - packet is replicated to recycle port: port + outRif
 *   - at 2nd pass:
 *     - get the VSI from the VLAN                                                                                           
 *     - mpls tunnel termination                                  
 *     - get the inRif (=VSI)                                                                                                
 *     - packet is forwarded according to IPMC                                                                                   
 *     - packet is replicated to access: port + outRif                                                                       
 *     - new link layer is built (DA compatible MC is built from DIP, SA is built from outRif.myMac, VLAN = VSI = outRif)
 *
 * How to run: 
 *  soc properties:
 * tm_port_header_type_in_40.BCM88650=ETH
 * tm_port_header_type_out_40.BCM88650=ETH
 * ucode_port_40.BCM88650=RCY.0
 * default_logical_interface_mpls_1_label_bud_multicast=0x5004
 * default_logical_interface_mpls_2_labels_bud_multicast=0x5006
 * 
 * how to run:
 * cint utility/cint_utils_multicast.c
 * cint cint_ipmc_flows.c
 * cint cint_mldp.c
 * cint 
 * int unit = 0; 
 * mldp_run_with_defaults(unit); 
 */
struct cint_mldp_info_s {
    int c1_port; /* c1 port (access)*/
    int c2_port; /* c2 port (access) */
    int p1_port; /* p1 port (provider) */
    int p2_port; /* p2 port (provider) */
    int p3_port; /* p3 port (provider) */
    int recycle_port; /* recycle port */
    int p1_vlan; /* p1's vlan (provider) */
    int p2_p3_vlan; /* p2,p3 vlan (provider) */
    int c1_c2_vlan; /* c1,c2 vlan (access) */
    bcm_mac_t p1_mac; /* p1's mac address*/
    bcm_mac_t p1_vlan_my_mac ; /* PE's my mac facing p1's vlan*/
    bcm_mac_t p2_mac; /* p2's mac address*/
    bcm_mac_t p3_mac; /* p3's mac address*/
    bcm_mac_t p2_p3_vlan_my_mac; /* PE's my  mac facing p2,p3's vlan */
    bcm_mac_t c1_mac; /* c1's mac address*/
    bcm_mac_t c2_mac; /* c2's mac address*/
    bcm_mac_t c1_c2_vlan_my_mac; /* PE's my  mac facing c1, c2's vlan */
    bcm_if_t p1_intf; /* rif facing p1's vlan*/
    bcm_if_t p2_p3_intf; /* rif facing p2, p3's vlan*/
    int internal_vlan;     /* this vlan is internal: used to build overlay vlan for packet that goes to 2nd pass.
                            * from vlan at the 2nd pass: get vsi, get inRif. This inRif at frwrd.   */
    int ipmc_index;  /* ipmc index base. (Each MC group creation increment this index) */
    int in_label; /* incoming mpls label */
    int swap_label;  /* label that swaps in_label */
    int encap_id1; /* first encap id */
    int encap_id2; /* second encap id */
    int out_label1; /* pushed out label on first mpls multicast outgoing packet*/
    int out_label2; /* pushed out label on second mpls multicast outgoing packet*/

};

cint_mldp_info_s cint_mldp_info = 
/* ports :
   c1 port | c2 port | p1 port | p2 port | p3 port port  | recycle port */
{200,            201,                202 ,              202,              203,                    40, 
/* vlans:
   p1 vlan | p2, p3 vlan | c1, c2 vlan  */
    20,            25,                     30,           
/* p1 mac | p1_vlan_my_mac | p2_mac | p3_mac | p2_p3_vlan_my_mac | c1_mac | c2_mac | c1_c2_vlan_my_mac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x12}, {0x00, 0x00, 0x00, 0x00, 0x00, 0x13}, {0x00, 0x00, 0x00, 0x00, 0x00, 0x14},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x15}, {0x00, 0x00, 0x00, 0x00, 0x00, 0x16}, {0x00, 0x00, 0x00, 0x00, 0x00, 0x17}, 
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18}, {0x00, 0x00, 0x00, 0x00, 0x00, 0x19},
/* p1_intf | p2_p3_intf */
    0,    0,
/* internal vlan */
11,
/* ipmc index base */
15960,
/* in_label | swap_label */
5000, 6000,
/* encap_id1 | encap_id2 */
0, 0,
/* out_label1, out_label2 */
300, 200
};

uint8 mldp_pipe_mode_exp_set = 0;

int mldp_exp = 0;

int mldp_mpls_pipe_mode_exp_set(int unit) {

    int rv = BCM_E_NONE;

    if (pipe_mode_exp_set) {
        rv = bcm_switch_control_set(unit, bcmSwitchMplsPipeTunnelLabelExpSet, pipe_mode_exp_set);
    }
    
    return rv;                
}


void mldp_info_init(int unit,
                    int access_port,
                    int access_port2, 
                    int provider_port,
                    int provider_port2
                    ) {
    cint_mldp_info.c1_port = access_port; 
    cint_mldp_info.c2_port = access_port2; 
    cint_mldp_info.p1_port = provider_port; 
    cint_mldp_info.p2_port = provider_port;
    cint_mldp_info.p3_port = provider_port2; 
}



/*  Configure multicast application for bud node.
    Upon receiving of mpls packet with label 5000, we exit with 2 mpls packets to ports p2, p3 and recycle port.
 */
int mldp_bud_provider_mc(int unit) {
	int mc_group;
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t label_array[2];
    int ingress_intf;
    int flags = BCM_L3_EGRESS_ONLY;

    /* open MC-group */
	egress_mc = 0;
	mc_group = cint_mldp_info.ipmc_index;
    rv = multicast__open_mc_group(unit, &mc_group, BCM_MULTICAST_TYPE_EGRESS_OBJECT);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group\n");
        return rv;
    }

    printf("MC configured \n ");


    rv = mldp_mpls_mc_config(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mldp_mpls_mc_config\n");
        return rv;
    }

    rv = mldp_mpls_add_encap_dummy(unit, flags, cint_mldp_info.internal_vlan /* outRif */, cint_mldp_info.p2_p3_vlan_my_mac/* mymac */, &ingress_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, in mldp_mpls_add_encap_dummy intf = %d\n", ingress_intf);
        return rv;
    }

 
    /* Connect multicast group to ingress_intf */
    rv = mldp_multicast_mpls_encap_add(unit, cint_mldp_info.ipmc_index, cint_mldp_info.recycle_port, ingress_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast_mpls_encap_add intf = %d\n", ingress_intf);
        return rv;
    }

    return rv; 
}



/*  Configure multicast application for trunk node.
    Upon receiving of mpls packet with label 5001, we exit with 2 mpls packets to ports p2, p3 .
*/
int mldp_continue_mc(int unit) {
	int mc_group;
    int rv = BCM_E_NONE;

    /* open MC-group */
    egress_mc = 0;
	mc_group = cint_mldp_info.ipmc_index;
    rv = multicast__open_mc_group(unit, &mc_group, BCM_MULTICAST_TYPE_EGRESS_OBJECT);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group\n");
        return rv;
    }
   

    rv = mldp_mpls_mc_config(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mldp_mpls_mc_config\n");
        return rv;
    }    

    return rv; 
}

/*  Configure multicast application for bud node.
    Upon receiving of mpls packet with label 5002, we exit with 1 mpls packet to the recycle port .
*/
int mldp_leaf_mc(int unit) {
    int rv = BCM_E_NONE;
    int multicast = 1;
    int ingress_intf;
    int flags = BCM_L3_EGRESS_ONLY;
	int mc_group;

    /* open MC-group */
	egress_mc = 0;
	mc_group = cint_mldp_info.ipmc_index;
    rv = multicast__open_mc_group(unit, &mc_group, BCM_MULTICAST_TYPE_EGRESS_OBJECT);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group\n");
        return rv;
    }    

    /* Add ilm entry for mpls multicast, swap label */
    rv = mldp_mpls_add_switch_entry(unit, cint_mldp_info.in_label, cint_mldp_info.swap_label, cint_mldp_info.ipmc_index, multicast);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_switch_entry\n");
        return rv;
    }

    rv = mldp_mpls_add_encap_dummy(unit, flags, cint_mldp_info.internal_vlan /* outRif */, cint_mldp_info.p2_p3_vlan_my_mac/* mymac */, &ingress_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, in mldp_mpls_add_encap_dummy intf = %d\n", ingress_intf);
        return rv;
    }

 
    /* Connect multicast group to ingress_intf */
    rv = mldp_multicast_mpls_encap_add(unit, cint_mldp_info.ipmc_index, cint_mldp_info.recycle_port, ingress_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast_mpls_encap_add intf = %d\n", ingress_intf);
        return rv;
    }

    return rv; 
}


/* Configure IPMC application for a bud/leaf node facing the access.:
 * This is employed in the second pass of the packet (packet comes through recycle port).
 */
int mldp_bud_leaf_access_mc(int unit) {

    int rv = BCM_E_NONE;
    int members_of_mc[2] = {cint_mldp_info.c1_port, cint_mldp_info.c2_port};
    int nof_member_of_mc = 2; 
    bcm_ip_t mc_ip = 0xe0060606;
    bcm_ip_t src_ip = 0x5a5a5a5a;


    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, cint_mldp_info.ipmc_index); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf("MC configured \n ");

    /* add members to MC group: access ports */

    rv = add_ingress_multicast_forwarding(unit,
                                          cint_mldp_info.ipmc_index, 
                                          members_of_mc, /* ports to add to MC group */
                                          nof_member_of_mc,  /* nof ports to add to MC group */
                                          cint_mldp_info.c1_c2_vlan /* outRif */
                                          ); 
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding\n");
        return rv;
    }

    /* add routing table entry, point to MC */
    rv = add_ip4mc_entry(unit, mc_ip, src_ip, cint_mldp_info.internal_vlan, cint_mldp_info.ipmc_index);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ip4mc_entry\n");
        return rv;
    }

    return rv;
}


int mldp_mpls_add_encap_dummy(int unit, int flags, int vlan /* outRif */, bcm_mac_t mymac, int* ingress_intf) {
    int rv;
    create_l3_egress_s l3eg; 

    sal_memcpy(l3eg.next_hop_mac_addr, mymac, 6);
    l3eg.vlan   = vlan;
    l3eg.arp_encap_id = *ingress_intf;
    l3eg.allocation_flags = flags;
    l3eg.out_tunnel_or_rif = 0;

    rv = l3__egress_only_encap__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
      print rv;
      printf(" error, in l3__egress_only_encap__create\n");
      return rv;
    }
	*ingress_intf = l3eg.arp_encap_id;
    printf("create encap_id:%08x\n", l3eg.arp_encap_id);
    printf("configured dummy encap linklayer %d \n ", *ingress_intf);
    return rv;
}


/* Main function. 1) Configure recycle port. 2) Define vsi's relating to the vlans (+ inLif relating to relevant vlans and ports).
   3) Define 
    
*/
int mldp_run(int unit) {

    int rv = BCM_E_NONE;
    bcm_l3_intf_t intf; 
    bcm_pbmp_t pbmp, ubmp;
    bcm_vlan_port_t vp1;       
    bcm_vlan_port_t_init(&vp1);

    /* ports configuration: add a recycle port */
    rv = bcm_port_control_set(unit,cint_mldp_info.recycle_port, bcmPortControlOverlayRecycle, 1);  
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set\n");
        return rv;
    }
    printf("recycle port configured \n ");

    /* AC configuration:
     * - at the 2nd pass, we got the VSI from port.default_vlan. (don't allocate lif)
     *   
     * - the rif resolved at AC is the rif used for the inner IP.
     * (no special configuration here, at the 2nd pass, dummy lif for IP tunnel termination
     *  will use the AC rif instead of resolving his own) 
     */

    /* add AC lif for the overlay at the 2nd pass */
    /* the match criteria is port */
    vp1.criteria = BCM_VLAN_PORT_MATCH_PORT;                                                                                                 
    vp1.port = cint_mldp_info.recycle_port;                                                                                                       
    vp1.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_VSI_BASE_VID;                                                                                                                                
    rv = bcm_vlan_port_create(unit,&vp1);                                                                                                         
    if (rv != BCM_E_NONE) {                                                                                                                       
        printf("Error, bcm_vlan_port_create\n");                                                                                                  
        return rv;                                                                                                                                
    }                                                                                                                                             
    printf("AC lif for the overlay at the 2nd pass configured: port(%d) lif(%d)\n ", vp1.port,vp1.vlan_port_id); 

    /* create  vlans - incoming,outgoing and add ports to them */
    rv = bcm_vlan_create(unit, cint_mldp_info.p1_vlan);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vlan_create with vid = %d\n", cint_mldp_info.p1_vlan);
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, cint_mldp_info.p1_port);
    rv = bcm_vlan_port_add(unit, cint_mldp_info.p1_vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }


    rv = bcm_vlan_create(unit,cint_mldp_info.p2_p3_vlan);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vlan_create with vid = %d\n", mpls_mc_info.eg_vid_1);
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, cint_mldp_info.p2_port); 
    rv = bcm_vlan_port_add(unit, cint_mldp_info.p2_p3_vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }  

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, cint_mldp_info.p3_port);
    rv = bcm_vlan_port_add(unit, cint_mldp_info.p2_p3_vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    rv = bcm_vlan_create(unit, cint_mldp_info.c1_c2_vlan);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vlan_create with vid = %d\n", cint_mldp_info.p1_vlan);
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, cint_mldp_info.c1_port);
    rv = bcm_vlan_port_add(unit, cint_mldp_info.c1_c2_vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, cint_mldp_info.c2_port);
    rv = bcm_vlan_port_add(unit, cint_mldp_info.c1_c2_vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    /* Create relevant router interfaces */
    create_l3_intf_s intf_create;
    intf_create.vsi = cint_mldp_info.p1_vlan;
    intf_create.my_global_mac = cint_mldp_info.p1_vlan_my_mac;
    intf_create.my_lsb_mac = cint_mldp_info.p1_vlan_my_mac;
    rv = l3__intf_rif__create(unit, &intf_create);
    cint_mldp_info.p1_intf = intf_create.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }
    
    intf_create.vsi = cint_mldp_info.p2_p3_vlan;
    intf_create.my_global_mac = cint_mldp_info.p2_p3_vlan_my_mac;
    intf_create.my_lsb_mac = cint_mldp_info.p2_p3_vlan_my_mac;
    rv = l3__intf_rif__create(unit, &intf_create);
    cint_mldp_info.p2_p3_intf = intf_create.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }

   bcm_l3_intf_t_init(&intf); 
   rv = create_rif(cint_mldp_info.p2_p3_vlan_my_mac, 
                   unit, 
                   cint_mldp_info.internal_vlan, /* rif_id (rif_id = vsi = vlan) */
                   &intf, 
                   0);

   /* enable multicast on this rif */
    rv = enable_rif_ipmc(unit,&intf, 1); 
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }
    printf("ipmc is enabled for RIF \n "); 

    bcm_l3_intf_t_init(&intf); 
    rv = create_rif(cint_mldp_info.c1_c2_vlan_my_mac, 
                    unit, 
                    cint_mldp_info.c1_c2_vlan, /* rif_id (rif_id = vsi = vlan) */
                    &intf, 
                    0); 
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_with_vrf\n");
        return rv;
    }
    /* enable multicast on this rif */
    rv = enable_rif_ipmc(unit,&intf, 1); 
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }
    printf("ipmc is enabled for RIF \n ");

    /* Create bud application */
    rv = mldp_bud_provider_mc(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mldp_bud_provider_mc\n");
        return rv;
    }

    cint_mldp_info.ipmc_index++;
    cint_mldp_info.encap_id1 = cint_mldp_info.encap_id2 = 0;
    cint_mldp_info.in_label++; /* in label oncreases by one */
    /* Create trunk application */
    rv = mldp_continue_mc(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mldp_continue_mc\n");
        return rv;
    }

    cint_mldp_info.ipmc_index++;
    cint_mldp_info.encap_id1 = cint_mldp_info.encap_id2 = 0;
    cint_mldp_info.in_label++;
    /* Create drop application */
    rv = mldp_leaf_mc(unit);  
    if (rv != BCM_E_NONE) {
        printf("Error, mldp_leaf_mc\n");
        return rv;
    }

    cint_mldp_info.ipmc_index++;
    cint_mldp_info.encap_id1 = cint_mldp_info.encap_id2 = 0;
    /* Create ipmc facing access side for bud and leaf */
    rv = mldp_bud_leaf_access_mc(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mldp_bud_leaf_access_mc\n");
        return rv;
    }

    return rv; 
}


int 
mldp_run_defaults_with_ports(int unit, int accessPort, int accessPort2, int networkPort, int networkPort2) {

    mldp_info_init(unit, 
                   accessPort, /* access port */
                   accessPort2, /* another access port */
                   networkPort,  /* provider port */
                   networkPort2 /* another provider port */
                   ); 
    return mldp_run(unit);
}


int 
mldp_run_with_defaults(int unit) {
   return mldp_run_defaults_with_ports(unit, 200, 201, 202, 203);
}

/* Add ilm entry for mpls multicast, swap label */
int
mldp_mpls_add_switch_entry(int unit, int in_label, int eg_label, int mc_index, int multicast)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    
    printf("Running with cint_mldp mldp_mpls_add_switch_entry\n");
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = (eg_label == 0) ? BCM_MPLS_SWITCH_ACTION_NOP:BCM_MPLS_SWITCH_ACTION_SWAP;
    /* TTL decrement has to be present */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;
    
    /* incoming label */
    printf("in label: %d\n", in_label);
    entry.label = in_label;
    entry.egress_label.label = eg_label;


    if (multicast) {
        /* add multicast group to entry */
        entry.flags |= BCM_MPLS_SWITCH_P2MP;
        entry.mc_group = mc_index;
        printf("entry.mc_group:%08x\n", entry.mc_group);
    }

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

/* Configure ilm entry for mpls multicast (upon receiving in_label), swap the label and push a new tunnel.
   First tunnel is for p2, second tunnel is for p3.             
 */
int 
mldp_mpls_mc_config(int unit){

    int CINT_NO_FLAGS = 0;
    int rv;
    int tmp_itf;
    bcm_pbmp_t pbmp, ubmp;
    int multicast = 1;
    bcm_mpls_egress_label_t label_array[1];
    int egid = 0;
  
    /* Add ilm entry for mpls multicast, swap label */
    rv = mldp_mpls_add_switch_entry(unit, cint_mldp_info.in_label, cint_mldp_info.swap_label, cint_mldp_info.ipmc_index, multicast);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_switch_entry\n");
        return rv;
    }

    /* set 2 mpls tunnels, one for each egress_intf */

    /* tunnel 1 is for 1 label */
    bcm_mpls_egress_label_t_init(&label_array[0]);

    label_array[0].exp = mldp_exp; 
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    if (!is_device_or_above(unit,ARAD_PLUS) || mldp_pipe_mode_exp_set) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    label_array[0].label = cint_mldp_info.out_label1;
    label_array[0].ttl = 30;

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    /* Connect Egress MPLS tunnel to encap_id (LL) */
    create_l3_egress_s l3eg; 

    sal_memcpy(l3eg.next_hop_mac_addr, cint_mldp_info.p2_mac, 6);
    l3eg.vlan   = cint_mldp_info.p2_p3_vlan;
    l3eg.arp_encap_id = cint_mldp_info.encap_id1;
    l3eg.out_tunnel_or_rif = label_array[0].tunnel_id;

    rv = l3__egress_only_encap__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
      print rv;
      printf(" error, in l3__egress_only_encap__create\n");
      return rv;
    }
	cint_mldp_info.encap_id1 = l3eg.arp_encap_id;
    printf("create encap_id:%08x\n", l3eg.arp_encap_id);
 

    /* add gport of type vlan-port to the multicast */
    rv = mldp_multicast_mpls_encap_add(unit, cint_mldp_info.ipmc_index, cint_mldp_info.p2_port, label_array[0].tunnel_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast_mpls_encap_add intf = %d\n", label_array[0].tunnel_id);
        return rv;
    }  

    /* tunnel 2 is for 2 labels */
    bcm_mpls_egress_label_t_init(&label_array[0]);

    label_array[0].exp = mldp_exp; 
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    if (!is_device_or_above(unit,ARAD_PLUS) || mldp_pipe_mode_exp_set) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    label_array[0].label = cint_mldp_info.out_label2;
    label_array[0].ttl = 20;

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    /* Connect Egress MPLS tunnel to encap_id (LL) */
    create_l3_egress_s l3eg1; 

    sal_memcpy(l3eg1.next_hop_mac_addr, cint_mldp_info.p3_mac, 6);
    l3eg1.vlan   = cint_mldp_info.p2_p3_vlan;
    /* l3eg.out_gport   = cint_mldp_info.p3_port,; - to be inserted if failing */
    l3eg1.arp_encap_id = cint_mldp_info.encap_id2;
    l3eg1.out_tunnel_or_rif = label_array[0].tunnel_id;

    rv = l3__egress_only_encap__create(unit,&l3eg1);
    if (rv != BCM_E_NONE) {
      print rv;
      printf(" error, in l3__egress_only_encap__create\n");
      return rv;
    }
	cint_mldp_info.encap_id2 = l3eg1.arp_encap_id;
    printf("create encap_id:%08x\n", l3eg1.arp_encap_id);

    /* add gport of type vlan-port to the multicast */
    rv = mldp_multicast_mpls_encap_add(unit, cint_mldp_info.ipmc_index, cint_mldp_info.p3_port, label_array[0].tunnel_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast_mpls_encap_add intf = %d\n", label_array[0].tunnel_id);
        return rv;
    } 
 
    return rv;
}

/*
 * add gport of type vlan-port to the multicast
 */
int mldp_multicast_mpls_encap_add(int unit, int mc_group_id, int sys_port, bcm_if_t egress_itf){

  int encap_id;
  int rv;

  rv = bcm_multicast_egress_object_encap_get(unit, mc_group_id, egress_itf, &encap_id);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_multicast_egress_object_encap_get mc_group_id:  0x%08x   gport:  0x%08x \n", mc_group_id, egress_itf);
    return rv;
  }

  rv = bcm_multicast_ingress_add(unit, mc_group_id, sys_port, encap_id);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_multicast_ingress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, sys_port, encap_id);
    return rv;
  }

  printf("encap_id:%08x\n", encap_id);
  printf("mc_group_id:%08x\n", mc_group_id);
  
  return rv;
}



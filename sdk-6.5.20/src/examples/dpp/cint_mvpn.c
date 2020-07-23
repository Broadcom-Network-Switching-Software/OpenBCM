/* 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/* 
 * The below CINT demonstrates IP-GRE L3VPN which show BCM886XX being PE-based MVPN.
 * Available for 88660_A0 and above.
 * MVPN model consists of three kind of devices:
 * 1. Customer Edge device (CE): Resides on customer network and connected with one or more interfaces to provide edge device.
 * The CE is not familiar with the existance of VPN and not with the IP core application.
 * 2. Provide Edge device (PE): Resides on service provider network and connects to one or more CEs.
 * PE is responsible on receiving and sending traffic from / to the core network between the CE and P.
 * PE is in charge of all the VPN processing.
 * 3. Provider device (P): Backbone router on a service provider network. Only in charge of the IP routing in the backbone and not familiar with the VPN processing.
 * It may be that PE device will fucntional as P for other VPN processing of not directly connected CE to backbone.
 *
 *
 * We are configuring PE. 

 * 
 * 
 * Traffic:
 *   - MC packets:
 *     - Packet Incoming from IP-GRE network P1, to P2, P3, CE1, CE2 (Bud node)
 *     - Packet Incoming from IP-GRE network, to PE2 (Continue)
 *     - Packet Incoming from IP-GRE network, to CE1, CE2 (Leaf node)
 *     - Packet Incoming from access side, to CE2, P1
 *
 * Packet Incoming from provider network P1, to P2, P3, CE1, CE2 (Bud node: provider network MC and access MC)
 * Purpose: - check MC is done at provider metwork for IP compatible MC at the IP of the tunnel
 *          - check tunnel termination (of the IP of the overlay) is done, to perform MC at the access network
 *          - check MC is done at access network for IP compatible MC at the inner IP
 *    Send:  
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | provider_MC       | routerP1_mac      | routerP1_vlan |
 *             | 01:00:5E:05:05:05 | 00:00:00:00:00:12 | 20            |
 *             ----------------------------------------------------------
 *                 ------------------------------
 *             ip: |   SIP        | DIP         | 
 *                 ------------------------------
 *                 | routerCE9_ip | provider_MC |
 *                 | 0.0.233.233 | 224.5.5.5    |
 *                 ------------------------------
 *                  
 *                GRE 4B
 *                   
 *                       --------------------------    
 *                   ip: |   SIP       | DIP       |    
 *                       --------------------------    
 *                       | host90_ip   | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------    
 *   Receive:
 *
 *    - receive 2 packets to P2 and P3
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | provider_MC       | routerPE1_mac     | routerP23_vlan |
 *             | 01:00:5E:05:05:05 | 00:0C:00:02:00:00 | 25             |
 *             ----------------------------------------------------------  
 *                 ------------------------------
 *             ip: |   SIP        | DIP         | 
 *                 ------------------------------
 *                 | routerCE9_ip | provider_MC |
 *                 | 0.0.233.233 | 224.5.5.5    |
 *                 ------------------------------
 *                  
 *                GRE 4B
 *                   
 *                       --------------------------    
 *                   ip: |   SIP       | DIP       |    
 *                       --------------------------    
 *                       | host90_ip   | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------    
 * 
 *    - receive 2 packets to CE1 and CE2
 *             -----------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN            |
 *             -----------------------------------------------------------
 *             | access_MC         | routerPE1_mac     | routerCE_vlan |
 *             | 01:00:5E:06:06:06 | 00:0C:00:02:00:00 | 30              |
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
 * - identify MC packet
 * - get the inRif (=VSI) 
 * - packet is forward according to MC
 * - packet is replicated to network: port + outrif 
 *   - new link layer is built (DA compatible MC is built from DIP, SA is built from outRif.myMac, VLAN = VSI = outRif)
 * - packet is replicated to recycle port: port + outRif
 *   - new link layer is built (DA compatible MC is built from DIP, SA is built from outRif.myMac, VLAN = VSI = outRif)
 *   - at 2nd pass:
 *     - get the VSI from the VLAN                                                                                           
 *     - tunnel termination (by soc property default_logical_interface_ip_tunnel_overlay_mc)                                 
 *     - get the inRif (=VSI)                                                                                                
 *     - packet is forward according to MC                                                                                   
 *     - packet is replicated to access: port + outRif                                                                       
 *     - new link layer is built (DA compatible MC is built from DIP, SA is built from outRif.myMac, VLAN = VSI = outRif)    
 *
 *
 *
 *
 *
 * Packet Incoming from provider network, to PE2 (Continue: provider to provider) 
 * Purpose: - check MC is done at provider metwork for IP compatible MC at the IP of the tunnel
 *
 *
 *    Send:  
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | provider_MC       | routerP1_mac      | routerP1_vlan |
 *             | 01:00:5E:07:07:07 | 00:00:00:00:00:12 | 20            |
 *             ----------------------------------------------------------
 *                 ------------------------------
 *             ip: |   SIP        | DIP         | 
 *                 ------------------------------
 *                 | routerCE9_ip | provider_MC |
 *                 | 0.0.233.233 | 224.7.7.7    |
 *                 ------------------------------
 *                  
 *                GRE 4B
 *                   
 *                       --------------------------    
 *                   ip: |   SIP       | DIP       |    
 *                       --------------------------    
 *                       | host90_ip   | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------    
 *
 *  Receive:
 *    receive 2 packets to P2 and P3
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | provider_MC       | routerPE1_mac     | routerP23_vlan |
 *             | 01:00:5E:07:07:07 | 00:0C:00:02:00:00 | 25             |
 *             ----------------------------------------------------------  
 *                 ------------------------------
 *             ip: |   SIP        | DIP         | 
 *                 ------------------------------
 *                 | routerCE9_ip | provider_MC |
 *                 | 0.0.233.233 | 224.7.7.7    |
 *                 ------------------------------
 *                  
 *                GRE 4B
 *                   
 *                       --------------------------    
 *                   ip: |   SIP       | DIP       |    
 *                       --------------------------    
 *                       | host90_ip   | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------    
 * 
 *
 * Packet flow:
 * - get the VSI from the VLAN
 * - identify MC packet
 * - get the inRif (=VSI) 
 * - packet is forward according to MC
 * - packet is replicated to network: port + outrif 
 *   - new link layer is built (DA compatible MC is built from DIP, SA is built from outRif.myMac, VLAN = VSI = outRif)
 *
 *
 *
 * 
 *
 *
 * Packet Incoming from provider network, to CE2, CE3 (leaf node: provider to access):
 *  Purpose: - check tunnel termination (of the IP of the overlay) is done, to perform MC at the access network
 *           - check MC is done at access network for IP compatible MC at the inner IP
 *           - check no replication is done for provider network
 * 
 *    Send:  
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | provider_MC       | routerP1_mac      | routerP1_vlan |
 *             | 01:00:5E:08:08:08 | 00:00:00:00:00:12 | 20            |
 *             ----------------------------------------------------------
 *                 ------------------------------
 *             ip: |   SIP        | DIP         | 
 *                 ------------------------------
 *                 | routerCE9_ip | provider_MC |
 *                 | 0.0.233.233 | 224.8.8.8    |
 *                 ------------------------------
 *                  
 *                GRE 4B
 *                   
 *                       --------------------------    
 *                   ip: |   SIP       | DIP       |    
 *                       --------------------------    
 *                       | host91_ip   | access_MC |
 *                       | 91.91.91.91 | 224.6.6.6 |
 *                       --------------------------    
 *
 *  Receive:
 *    - receive 2 packets to CE1 and CE2
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN          |
 *             ---------------------------------------------------------
 *             | access_MC         | routerPE1_mac     | routerCE_vlan |
 *             | 01:00:5E:09:09:09 | 00:00:00:00:CE:01 | 30            |
 *             --------------------------------------------------------- 
 *                
 *                  ----------------------------           
 *              ip: |   SIP       | DIP         |           
 *                  ----------------------------           
 *                  | host90_ip   |  access_MC  |
 *                  | 91.91.91.91 | 224.6.6.6   | 
 *                  ----------------------------        
 *      
 *
 * Packet flow:
 * - get the VSI from the VLAN
 * - identify MC packet
 * - get the inRif (=VSI) 
 * - packet is forward according to MC
 * - packet is replicated to recycle port: port + outRif
 *   - new link layer is built (DA compatible MC is built from DIP, SA is built from outRif.myMac, VLAN = VSI = outRif)
 *   - at 2nd pass:
 *     - get the VSI from the VLAN                                                                                           
 *     - tunnel termination (by soc property default_logical_interface_ip_tunnel_overlay_mc)                                 
 *     - get the inRif (=VSI)                                                                                                
 *     - packet is forward according to MC                                                                                   
 *     - packet is replicated to access: port + outRif                                                                       
 *     - new link layer is built (DA compatible MC is built from DIP, SA is built from outRif.myMac, VLAN = VSI = outRif)    
 *
 *
 *
 * Packet Incoming from access network, to CE2, P1 (leaf node: access to access and provider):
 *  Purpose: - check MC is done at access network for IP compatible MC at the IP
 *           - check repalication at the access
 *           - check replication at the provider with overlay layers: GREoIPoE
 *
 *
 *    Send:  
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN          |
 *             ---------------------------------------------------------
 *             | access_MC         | routerCE1_mac     | routerCE_vlan |
 *             | 01:00:5E:09:09:09 | 00:00:00:00:CE:01 | 30            |
 *             --------------------------------------------------------- 
 *                
 *                  ----------------------------           
 *              ip: |   SIP       | DIP         |           
 *                  ----------------------------           
 *                  | host11_ip   |  access_MC  |
 *                  | 11.11.11.11 | 224.9.9.9   | 
 *                  ----------------------------    
 *
 *
 *   Receive: 
 *      receive 1 packets to CE4
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ---------------------------------------------------------
 *             | access_MC         | routerPE1_mac     | routerCE4_vlan |
 *             | 01:00:5E:09:09:09 | 00:0C:00:02:00:00 | 25             |
 *             --------------------------------------------------------- 
 *                
 *                  ----------------------------           
 *              ip: |   SIP       | DIP         |           
 *                  ----------------------------           
 *                  | host11_ip   |  access_MC  |
 *                  | 11.11.11.11 | 224.9.9.9   | 
 *                  ----------------------------   
 *
 *  Receive:
 *    receive 1 packets to P1  
 *             ---------------------------------------------------------
 *        eth: |    DA             |   SA              | VLAN           |
 *             ----------------------------------------------------------
 *             | provider_MC       | routerPE1_mac     | routerP1_vlan |
 *             | 01:00:5E:09:09:09 | 00:0C:00:02:00:00 | 20            |
 *             ----------------------------------------------------------  
 *                 ------------------------------
 *             ip: |   SIP        | DIP         | 
 *                 ------------------------------
 *                 | routerCE9_ip | provider_MC |
 *                 | 0.0.233.233 | 224.7.7.7    |
 *                 ------------------------------
 *                  
 *                GRE 4B
 *                   
 *                       --------------------------    
 *                   ip: |   SIP       | DIP       |    
 *                       --------------------------    
 *                       | host90_ip   | access_MC |
 *                       | 90.90.90.90 | 224.6.6.6 |
 *                       --------------------------    
 * 
 * Packet flow:
 * - get the VSI from the VLAN
 * - identify MC packet
 * - get the inRif (=VSI) 
 * - packet is forward according to MC
 * - packet is replicated to access port: port + outRif
 *   - new link layer is built (DA compatible MC is built from DIP, SA is built from outRif.myMac, VLAN = VSI = outRif)
 * - packet is repclicated to provider port: port + outRif
 *     - packet is encapsulated with L2gre IP tunnel
 * 
 *
 * How to run: 
 *  soc properties:
 * tm_port_header_type_in_40.BCM88650=ETH
 * tm_port_header_type_out_40.BCM88650=ETH
 * ucode_port_40.BCM88650=RCY.0
 * default_logical_interface_ip_tunnel_overlay_mc=16484
 * 
 * how to run:
 * cint utility/cint_utils_l3.c
 * cint cint_ip_route.c
 * cint cint_ip_tunnel.c 
 * cint cint_ip_tunnel_term.c  
 * cint cint_mact.c
 * cint cint_vswitch_metro_mp.c 
 * cint cint_multi_device_utils.c 
 * cint cint_ipmc_flows.c
 * cint cint_mvpn.c
 * cint 
 * int unit = 0; 
 * mvpn_run_with_defaults(unit); 
 */



struct cint_mvpn_info_s {
    int access_port; /* access port */
    int access_port2; /* another access port */
    int provider_port; /* provider port */
    int provider_port2; /* another provider port */
    int recycle_port; /* recycle port */
    int provider_vlan; /* vlan at the l2gre header at provider side */
    int provider_vlan2; /* another vlan at the l2gre header at provider side */
    int access_vlan; /* vlan at the access side */
    int access_vlan2; /* another vlan at the access side */
    bcm_mac_t my_mac; /* my mac */
    int internal_vlan;     /* this vlan is internal: used to build overlay vlan for packet that goes to 2nd pass.
                            * from vlan at the 2nd pass: get vsi, get inRif. This inRif at frwrd.   */
    int ipmc_index;  /* ipmc index base. (Each MC group creation increment this index) */

};

cint_mvpn_info_s cint_mvpn_info = 
/* ports :
   access port | another access port | provider port | another provider port | recycle port */
{200,            201,                  202 ,           203,                    40, 
/* vlans:
   provider vlan | another provider vlan | access vlan | another access vlan */
    20,            25,                     30,           35, 
/* my mac */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, 
/* internal vlan */
11,
/* ipmc index base */
5000
};

void mvpn_info_init(int unit,
                    int access_port,
                    int access_port2, 
                    int provider_port,
                    int provider_port2
                    ) {
    cint_mvpn_info.access_port = access_port; 
    cint_mvpn_info.access_port2 = access_port2; 
    cint_mvpn_info.provider_port = provider_port; 
    cint_mvpn_info.provider_port2 = provider_port2; 
}



/*  multicast configuration:
 *  - add MC for the IP of the overlay
 *  - add ports to MC: provider ports and recycle
 *  - add routing entry to the MC (224.5.5.5 -> MC)
 */
int mvpn_bud_provider_mc(int unit) {

    int rv = BCM_E_NONE;

    int ipmc_index = cint_mvpn_info.ipmc_index++;  

    /* MC for the IP of the overlay:
     * multicast to provider and recycle
       For 1st pass */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, ipmc_index); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf("MC configured \n ");

    /* add members to MC group: add provider ports  */    
    int members_of_mc[2] = {cint_mvpn_info.provider_port, cint_mvpn_info.provider_port2};
    int nof_member_of_mc = 2; 
    rv = add_ingress_multicast_forwarding(unit,ipmc_index, members_of_mc, nof_member_of_mc, cint_mvpn_info.provider_vlan2 /* outRif */); 
    if (rv != BCM_E_NONE) {                                                                                  
        printf("Error, add_ingress_multicast_forwarding\n");                                                 
        return rv;                                                                                           
    }                                                                                                        

    /* add members to MC group: add recycle port */
    members_of_mc[0] = cint_mvpn_info.recycle_port; 
    nof_member_of_mc = 1; 
    rv = add_ingress_multicast_forwarding(unit,
                                          ipmc_index, 
                                          members_of_mc, /* ports to add to MC group */
                                          nof_member_of_mc, /* nof ports to add to MC group */
                                          cint_mvpn_info.internal_vlan /* outRif */
                                          ); 
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding\n");
        return rv;
    }
    printf("members added to MC\n ");

    /* add routing table entry, point to MC */
    bcm_ip_t mc_ip = 0xe0050505; 
    rv = create_forwarding_entry_dip_sip(unit, 
                                         mc_ip, 
                                         0,  /* src ip */
                                         ipmc_index, 
                                         cint_mvpn_info.provider_vlan /* inRif */
                                         ); 
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip\n");
        return rv;
    }
    printf("mc group entry added to routing table \n ");

    return rv; 
}



/*  multicast configuration:
 *  - add MC for the IP of the overlay
 *  - add ports to MC: provider ports 
 *  - add routing entry to the MC (224.7.7.7 -> MC)
 */
int mvpn_continue_mc(int unit) {

    int rv = BCM_E_NONE;

    int ipmc_index = cint_mvpn_info.ipmc_index++; 

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, ipmc_index); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf("MC configured \n ");

    /* add members to MC group: add provider ports  */    
    int members_of_mc[2] = {cint_mvpn_info.provider_port, cint_mvpn_info.provider_port2};
    int nof_member_of_mc = 2; 
    rv = add_ingress_multicast_forwarding(unit,
                                          ipmc_index, 
                                          members_of_mc, /* ports to add to MC group */
                                          nof_member_of_mc, /* nof ports to add to MC group */
                                          cint_mvpn_info.provider_vlan2 /* outRif */); 
    if (rv != BCM_E_NONE) {                                                                                  
        printf("Error, add_ingress_multicast_forwarding\n");                                                 
        return rv;                                                                                           
    }                                                                                                        
    printf("members added to MC\n ");

    /* add routing table entry, point to MC */
    bcm_ip_t mc_ip = 0xe0070707; 
    rv = create_forwarding_entry_dip_sip(unit, 
                                         mc_ip, 
                                         0, /* src ip */
                                         ipmc_index, 
                                         cint_mvpn_info.provider_vlan /* inRif */
                                         ); 
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip\n");
        return rv;
    }
    printf("mc group entry added to routing table \n ");

    return rv; 
}

/* multicast configuration:
 * - add MC for the IP of the access
 * - add ports to MC: access ports
 * - add routing entry to the MC (224.6.6.6 -> MC)
 */
int mvpn_bud_leaf_access_mc(int unit) {
    int rv = BCM_E_NONE;

    int ipmc_index = cint_mvpn_info.ipmc_index++; 

    /* MC for the IP of the access */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, ipmc_index); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf("MC configured for access \n ");

    /* add members to MC group: access ports */
    int members_of_mc[2] = {cint_mvpn_info.access_port, cint_mvpn_info.access_port2};
    int nof_member_of_mc = 2; 
    rv = add_ingress_multicast_forwarding(unit,
                                          ipmc_index, 
                                          members_of_mc, /* ports to add to MC group */
                                          nof_member_of_mc,  /* nof ports to add to MC group */
                                          cint_mvpn_info.access_vlan /* outRif */
                                          ); 
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding\n");
        return rv;
    }
    printf("members added to MC for access \n ");

    /* add routing table entry, point to MC */
    bcm_ip_t mc_ip = 0xe0060606;
    rv = create_forwarding_entry_dip_sip(unit, 
                                         mc_ip, 
                                         0, /* src ip */
                                         ipmc_index, 
                                         cint_mvpn_info.internal_vlan /* inRif */
                                         ); 
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip\n");
        return rv;
    }
    printf("mc group entry for access added to routing table \n ");

    return rv; 
}

/* multicast configuration:
 * - add MC for the IP of the overlay
 * - add port to MC: recycle port
 * - add routing entry to the MC (224.8.8.8 -> MC)
 */
int mvpn_leaf_mc(int unit) {
    int rv = BCM_E_NONE;

    int ipmc_index = cint_mvpn_info.ipmc_index++; 

    /* MC for the IP of the overlay:
     * multicast to recycle
       For 1st pass */

    /* create MC group with id=40960:  */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, ipmc_index); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf("MC configured \n ");

    /* add members to MC group: add recycle port */
    int members_of_mc[1] = {cint_mvpn_info.recycle_port};
    int nof_member_of_mc = 1; 
    rv = add_ingress_multicast_forwarding(unit,
                                          ipmc_index, 
                                          members_of_mc, /* ports to add to MC group */
                                          nof_member_of_mc, /* nof ports to add to MC group */
                                          cint_mvpn_info.internal_vlan /* outRif */
                                          ); 
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding\n");
        return rv;
    }
    printf("members added to MC\n ");

    /* add routing table entry, point to MC */
    bcm_ip_t mc_ip = 0xe0080808;
    rv = create_forwarding_entry_dip_sip(unit, 
                                         mc_ip, 
                                         0,  /* src ip */
                                         ipmc_index, 
                                         cint_mvpn_info.provider_vlan /* inRif */
                                         ); 
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip\n");
        return rv;
    }
    printf("mc group entry added to routing table \n ");

    return rv; 
}


/* configure l2gre ip tunnel, ready to be added to MC group using encap_id.
 * DA: 01:00:5E:09:09:09
 * IP: 224.9.9.9
 */
int mvpn_l2gre_ip_tunnel(int unit, int *encap) {

    int rv = BCM_E_NONE;

    /* init ip tunnel info */
    bcm_mac_t next_hop_da = {0x01, 0x00, 0x5E, 0x09, 0x09, 0x09}; 
    sal_memcpy(ip_tunnel_glbl_info.tunnel_1.da, next_hop_da, 6);
    ip_tunnel_glbl_info.tunnel_1.dip = 0xE0090909; 
    ip_tunnel_glbl_info.vlan = cint_mvpn_info.provider_vlan; /* ip tunnel is built with provider vlan */

    /* build tunnel initiators */
    bcm_gport_t out_tunnel_gports[2];
    int ll_encap_id;
    bcm_if_t out_tunnel_intf_ids[2];/* out tunnels interfaces
                                  out_tunnel_intf_ids[0] : is tunnel-interface-id
                                  out_tunnel_intf_ids[1] : is egress-object (FEC) points to tunnel */
    rv = ipv4_tunnel_build_tunnels(unit, cint_mvpn_info.provider_port, out_tunnel_intf_ids,out_tunnel_gports, &ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }
    if(verbose >= 2){
        printf("Open out-tunnel 1 gport-id: 0x%08x (No FEC)\n\r", out_tunnel_gports[0]);
    }
 
    rv = bcm_multicast_egress_object_encap_get(unit, -1 /* mc: not used */, out_tunnel_intf_ids[0], encap);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_l3_encap_get \n");
        return rv;
    }


    return rv; 

}

/* multicast configuration:
 * - add MC for the IP of the access 
 * - add port to MC:
 *   - access port
 *   - configure l2gre ip tunnel
 *   - add provider "l2gre ip tunnel" port
 * - add routing entry to the MC (224.9.9.9 -> MC)
 */
int mvpn_access_mc(int unit) {

    int rv = BCM_E_NONE;

    int ipmc_index = cint_mvpn_info.ipmc_index++; 

    /* MC for the IP of the native IP:
     * multicast to provider and access */

    /* create MC group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, ipmc_index); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf("MC configured \n ");

    /* add members to MC group: add l2gre ip tunnel port  */    

    /* configure l2gre IP tunnel: with compatible MC addresses */
    int encap_id;
    rv = mvpn_l2gre_ip_tunnel(unit, &encap_id); 
    if (rv != BCM_E_NONE) {                                                                                                 
        printf("Error, in mvpn_l2gre_ip_tunnel\n");                                                                
        return rv;                                                                                                          
    }                                                                                                                       

    rv = bcm_multicast_ingress_add(unit, ipmc_index, cint_mvpn_info.provider_port,  encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_ingress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", ipmc_index, cint_mvpn_info.provider_port, encap_id);
        return rv;
    }

    int members_of_mc[1] = {cint_mvpn_info.access_port2};
    int nof_member_of_mc = 1;                                                                                                   
    rv = add_ingress_multicast_forwarding(unit,ipmc_index, members_of_mc, nof_member_of_mc, cint_mvpn_info.provider_vlan2);
    if (rv != BCM_E_NONE) {                                                                                                 
        printf("Error, add_ingress_multicast_forwarding\n");                                                                
        return rv;                                                                                                          
    }                                                                                                                       

    /* add routing table entry, point to MC */
    bcm_ip_t mc_ip = 0xe0090909; 
    rv = create_forwarding_entry_dip_sip(unit, 
                                         mc_ip, 
                                         0,  /* src ip */
                                         ipmc_index, 
                                         cint_mvpn_info.access_vlan /* inRif */
                                         ); 
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip\n");
        return rv;
    }
    printf("mc group entry added to routing table \n ");

    return rv; 
}


int mvpn_run(int unit) {

    int rv = BCM_E_NONE;

    /* ports configuration: add a recycle port */

    rv = bcm_port_control_set(unit,cint_mvpn_info.recycle_port, bcmPortControlOverlayRecycle, 1);  
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
     *  will use the AC rif instead of resolve his own) 
     */

    /* add AC lif for the overlay at the 2nd pass */
    bcm_vlan_port_t vp1;       
    bcm_vlan_port_t_init(&vp1);
    /* the match criteria is port */
    vp1.criteria = BCM_VLAN_PORT_MATCH_PORT;                                                                                                 
    vp1.port = cint_mvpn_info.recycle_port;                                                                                                       
    vp1.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_VSI_BASE_VID;                                                                                                                                
    rv = bcm_vlan_port_create(unit,&vp1);                                                                                                         
    if (rv != BCM_E_NONE) {                                                                                                                       
        printf("Error, bcm_vlan_port_create\n");                                                                                                  
        return rv;                                                                                                                                
    }                                                                                                                                             
    printf("AC lif for the overlay at the 2nd pass configured: port(%d) lif(%d)\n ", vp1.port,vp1.vlan_port_id); 

    /* routing interface for IP of the overlay*/

    /* create RIF for IP routing for IP of the overlay:
       inRif allow the packet to be routed */
    int vrf = 0; 
    bcm_l3_intf_t intf;
    bcm_l3_intf_t_init(&intf); 
    rv = create_rif(cint_mvpn_info.my_mac, 
                    unit, 
                    cint_mvpn_info.provider_vlan, /* rif_id (rif_id = vsi = vlan) */
                    &intf,  
                    vrf);
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_with_vrf\n");
        return rv;
    }
     printf("rif configured \n ");

    /* enable ipmc for this RIF */
    rv = enable_rif_ipmc(unit,&intf, 1); 
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }
    printf("ipmc is enabled for this RIF  \n ");


    /* create RIF for IP routing for IP of the overlay:
       outRif build the LL: SA and VLAN */
    bcm_l3_intf_t_init(&intf); 
    rv = create_rif(cint_mvpn_info.my_mac, 
                    unit, 
                    cint_mvpn_info.provider_vlan2, /* rif_id (rif_id = vsi = vlan) */
                    &intf,  
                    vrf);
    printf("rif configured \n ");

    /* routing interface for IP of the overlay for 2nd pass */

    /* create RIF for IP routing at the IP of the overlay, for the 2nd pass:
     * outRif builds the LL: SA and VLAN (at the 1st pass)
     * inRif allows packet to be routed (at the 2nd pass) */
    bcm_l3_intf_t_init(&intf); 
    rv = create_rif(cint_mvpn_info.my_mac, 
                    unit, 
                    cint_mvpn_info.internal_vlan, /* rif_id (rif_id = vsi = vlan) */
                    &intf, 
                    vrf); 
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_with_vrf\n");
        return rv;
    }
    printf("rif configured \n ");

    /* enable ipmc for this RIF */
    rv = enable_rif_ipmc(unit,&intf, 1); 
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }
    printf("ipmc is enabled for RIF \n ");


    /* create RIF for IP routing at the IP of the access, for the 2nd pass:
     * outRif is used to build the LL: SA and VLAN */
    bcm_l3_intf_t_init(&intf); 
    rv = create_rif(cint_mvpn_info.my_mac, 
                    unit, 
                    cint_mvpn_info.access_vlan, /* rif_id (rif_id = vsi = vlan) */
                    &intf, 
                    vrf); 
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_with_vrf\n");
        return rv;
    }

    /* bud node (drop and continue) MC configuration:
     * - configure MC and routing table entry at provider side
       - configure MC and routing table entry at access side */
    rv = mvpn_bud_provider_mc(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_bud_provider_mc\n");
        return rv;
    }

    /* we configure only 1 MC group at access. (used for bud node and leaf node)*/
    rv = mvpn_bud_leaf_access_mc(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_bud_leaf_access_mc\n");
        return rv;
    }

    /* "continue" node MC configuration:
     * - configure MC and routing table entry at provider side */
    rv = mvpn_continue_mc(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_continue_mc\n");
        return rv;
    }

    /* leaf node MC configuration:
     * - configure MC and routing table entry at provider side* 
       - configure MC at access side: see mvpn_bud_leaf_access_mc */
    rv = mvpn_leaf_mc(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_leaf_mc\n");
        return rv;
    }

    /* access node MC configuration:
     * - configure MC and routing table entry at access side: to access and provider
       - for provider: configure l2gre ip tunnel. */
    rv = mvpn_access_mc(unit); 
    if (rv != BCM_E_NONE) {
        printf("Error, mvpn_access_mc\n");
        return rv;
    }

    return rv; 
}


int 
mvpn_run_defaults_with_ports(int unit, int accessPort, int accessPort2, int networkPort, int networkPort2) {

    mvpn_info_init(unit, 
                   accessPort, /* access port */
                   accessPort2, /* another access port */
                   networkPort,  /* provider port */
                   networkPort2 /* another provider port */
                   ); 
    return mvpn_run(unit);
}


int 
mvpn_run_with_defaults(int unit) {
   return mvpn_run_defaults_with_ports(unit, 200, 201, 202, 203);
}

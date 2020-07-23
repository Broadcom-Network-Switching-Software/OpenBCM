/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Multi-Dev Vxlan ROO MC~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_multi_device_vxlan_roo_mc.c
 * Purpose: Demo Vxlan ROO MC under multi device system
 *
 * Multi-Device system require the ingress/egress configuration separately and relatively. 
 * This cint demo how to configurer the Vxlan ROO service under multi-device system.
 *  1. Create overlay L3 intf/arp;
 *  2. Create overlay Vxlan tunnel termination and encapsulation;
 *  3. Create native rif/arp;
 *  4. Create FEC and FWD entry in both direction;
 *  5. Send IPv4 mc traffic into PE1 to verify
 *  6. Verify the replication, 2 copies generated:
 *      1st copy, to local CE(Out-of-Overlay)
 *      2nd copy, to remote PE2(Into-Overlay)
 *
 *
 *  2 packet are received:
 *-----------------------------------------------------------------------
 *      Incoming                    |      Outgoing
 *-----------------------------------------------------------------------
 *
 *   IPv4oEthoVxlanoUDPoIPv4oEth--->|-------->IPv4oEth(out of overlay)
 *                                  |
 *                                  |-------->IPv4oEthoVxlanoUDPoIPv4oEth(into overlay)
 */


 struct cint_multi_dev_vxlan_roo_mc_info_s 
{
    /*Vxlan incoming traffic*/
    int vxlan_network_group_id;             /* ingress and egress orientation for VXLAN */
    bcm_ip_t tunnel_dip;                    /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;               /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                    /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;               /* tunnel SIP mask*/    
    int vxlan_tunnel_if;                    /* Tunnel Interface */
    int tunnel_termination_vrf;
    int pe1_vxlan_tunnel_id;

    int vpn_id;                             /* VPN ID */
    int vni;                                /* VNI of the VXLAN */
    int ce_port;                            /* outgoing port to access side */
    int pe1_port;                          /* incoming port from PE1 core side */
    int pe2_port;                          /* incoming port from PE2 core side */

    int pe1_native_eth_rif;                /* core native RIF */
    int pe1_native_eth_vrf;                /* core native VRF */
    int pe1_overlay_eth_rif;               /* core overlay RIF */
    int pe1_overlay_eth_vrf;               /* core overlay VRF */
    bcm_mac_t pe1_native_eth_rif_mac;      /* mac for core native RIF */
    bcm_mac_t pe1_overlay_eth_rif_mac;     /* mac for core overlay RIF */
    bcm_ip_t dip_into_mc;                 /* dip */
    bcm_ip_t host_ip;                           /* sip */

    int mc_group;

    /*1st copy, to local CE*/
    int ce_eth_rif;                     /* access RIF */
    int ce_eth_vrf;                     /* access vrf */
    int ce_arp_id;                      /* Id for access ARP entry */
    bcm_mac_t ce_eth_rif_mac;           /* mac for access RIF */
    bcm_mac_t ce_next_hop_mac;          /* mac for next access hop */

    /*2nd copy ,to remote PE2*/
    int pe2_native_eth_rif;                /* PE2 core native RIF */
    int pe2_native_eth_vrf;                /* PE2 core native vrf */

    int pe2_native_arp_id;                      /* Id for access ARP entry */
    bcm_mac_t pe2_native_eth_rif_mac;           /* mac for access RIF */
    bcm_mac_t pe2_native_next_hop_mac;          /* mac for next access hop */

    int pe2_overlay_eth_rif;                /* PE2 core native RIF */
    int pe2_overlay_eth_vrf;                /* PE2 core native vrf */

    int pe2_overlay_arp_id;                      /* Id for access ARP entry */
    bcm_mac_t pe2_overlay_eth_rif_mac;           /* mac for access RIF */
    bcm_mac_t pe2_overlay_next_hop_mac;          /* mac for next access hop */


    bcm_ip_t pe2_tunnel_dip;                    /* tunnel DIP */
    bcm_ip_t pe2_tunnel_sip;                    /* tunnel SIP */
    bcm_gport_t pe2_vxlan_tunnel_id;                  /* Tunnel ID */
    int pe2_vxlan_tunnel_if;                    /* Tunnel Interface */
    int pe2_vni_id;
};
 
 
 cint_multi_dev_vxlan_roo_mc_info_s cint_vxlan_roo_mc_info=
{

    /*-----------------------------------------------------------------------*/
    /*Incomping traffic */
    /*-----------------------------------------------------------------------*/
    /*
    * VXLAN Orientation,
    */
    1,
    /*
    * tunnel DIP
    */
    0xAB000001, /* 171.0.0.1 */
    /*
    * tunnel DIP mask
    */
    0xffffffff,
    /*
    * tunnel SIP
    */
    0xAC000001, /* 172.0.0.1 */
    /*
    * tunnel SIP mask
    */
    0xffffffff,
    /*
    *vxlan tunnel_if
    */
    10, 
    /*
    *tunnel_termination_vrf
    */
    5, 
    /*
    *pe1_vxlan_tunnel_id
    */
    0x3004,


    /*
    * VPN ID
    */
    15,
    /*
    * VNI
    */
    5000,
    /*
    * ports : ce_port | pe1_port | pe2 port 
    */
    0, 0, 0,
    /*
    * pe1_native_eth_rif, pe1_native_eth_vrf
    */
    15,30,
    /*
    * pe1_overlay_eth_rif, pe1_overlay_eth_vrf
    */
    20, 5,
    /*
    * pe1_native_eth_rif_mac, 
    * pe1_overlay_eth_rif_mac
    */

    {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x22},
    {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x33},

    /*
    * dip to mc
    */
    0xE0020202 /** 224.2.2.2 */ ,

    /*
    * host_ip
    */
    0x0A0A0A02 /** 10.10.10.2 */ ,

    /*
    * mc_group
    */
    6000 ,



    /*-----------------------------------------------------------------------*/
    /*1ST copy */
    /*-----------------------------------------------------------------------*/

    /*
    * ce_eth_rif,ce_eth_vrf, ce_arp_id
    */
    10,0, 0x3000,
    /*
    * ce_eth_rif_mac, 
    * ce_next_hop_mac
    */
    {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x44},
    {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1D},



    /*-----------------------------------------------------------------------*/
    /*2nd copy*/
    /*-----------------------------------------------------------------------*/

    /*Native Link layer header*/
    /*
    * pe2_native_eth_rif,pe2_native_eth_vrf, pe2_native_arp_id
    */
    11, 0, 0x3001,
    /*
    * pe2_native_eth_rif_mac, 
    * pe2_native_next_hop_mac
    */
    {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x45},
    {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1e},


    /*Overlay Link layer header*/

    /*
    * pe2_overlay_eth_rif,pe2_overlay_eth_vrf, pe2_overlay_arp_id
    */
    12, 0, 0x3002,
    /*
    * pe2_native_eth_rif_mac, 
    * pe2_native_next_hop_mac
    */
    {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x46},
    {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1f},

    /*Overlay IP tunnel info*/

    /*
    * pe2_tunnel_dipP
    */
    0xAD000001, /* 173.0.0.1 */
    /*
    * pe2_tunnel_sip
    */
    0xAE000001, /* 174.0.0.1 */

    /*
     * pe2_vxlan_tunnel_id | pe2_vxlan_tunnel_if | pe2_vni_id
     */
    0x3003,11, 5001

};

/**
 * Main entrance for vlxan roo configuration
 * INPUT:
 *   ingress_unit- traffic incoming unit 
 *   egress_unit- traffic outoging unit 
 *   pe1_sys_port- traffic incoming sys port 
 *   ce_sys_port- CE traffic outoging sys port 
 *   pe2_sys_port- PE2 traffic outoging sys port 

*/
 int
 multi_dev_vxlan_roo_mc_init(
    int ingress_unit,
    int egress_unit,
    int pe1_sys_port,
    int pe2_sys_port,
    int ce_sys_port)
{
    int rv;
    int local_port,is_local;

    /*
     * 0: Convert the sysport to local port
     */
    multi_dev_system_port_to_local(ingress_unit,pe1_sys_port,&cint_vxlan_roo_mc_info.pe1_port);

    multi_dev_system_port_to_local(egress_unit,ce_sys_port,&cint_vxlan_roo_mc_info.ce_port);
    multi_dev_system_port_to_local(egress_unit,pe2_sys_port,&cint_vxlan_roo_mc_info.pe2_port);


    bcm_vxlan_init(ingress_unit);
    bcm_vxlan_init(egress_unit);
 
    return rv;

}


/**
 * L3 intf init
 * INPUT:
 *   ingress_unit- traffic incoming unit 
 *   egress_unit- traffic outoging unit 
*/
 int
 multi_dev_vxlan_roo_mc_l3_intf_configuration(
    int ingress_unit,
    int egress_unit)
{
    int rv;
    bcm_l3_intf_t l3_intf;

    /*Incoming traffic */

    /*1.PE1 native-in-rif*/
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_vxlan_roo_mc_info.pe1_native_eth_rif;
    rv = bcm_l3_intf_get(ingress_unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        sand_utils_l3_eth_rif_s eth_rif_structure;
        
        sand_utils_l3_eth_rif_s_common_init(ingress_unit, 0, &eth_rif_structure, cint_vxlan_roo_mc_info.pe1_native_eth_rif, 0, 0, cint_vxlan_roo_mc_info.pe1_native_eth_rif_mac, cint_vxlan_roo_mc_info.pe1_native_eth_vrf);
        rv = sand_utils_l3_eth_rif_create(ingress_unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
         printf("Error, sand_utils_l3_eth_rif_create pe1_native_eth_rif\n");
         return rv;
        }
    }
    
    /*2.pe1 overlay-in-rif*/
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_vxlan_roo_mc_info.pe1_overlay_eth_rif;
    rv = bcm_l3_intf_get(ingress_unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        sand_utils_l3_eth_rif_s eth_rif_structure;
        
        sand_utils_l3_eth_rif_s_common_init(ingress_unit, 0, &eth_rif_structure, cint_vxlan_roo_mc_info.pe1_overlay_eth_rif, 0, 0, cint_vxlan_roo_mc_info.pe1_overlay_eth_rif_mac, cint_vxlan_roo_mc_info.pe1_overlay_eth_vrf);
        rv = sand_utils_l3_eth_rif_create(ingress_unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
         printf("Error, sand_utils_l3_eth_rif_create pe1_overlay_eth_rif\n");
         return rv;
        }
    }
    

    /*Outgoing traffic */

    /*3.access side In-RIF*/
    
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_vxlan_roo_mc_info.ce_eth_rif;
    rv = bcm_l3_intf_get(egress_unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        sand_utils_l3_eth_rif_s eth_rif_structure;
        
        sand_utils_l3_eth_rif_s_common_init(egress_unit, 0, &eth_rif_structure, cint_vxlan_roo_mc_info.ce_eth_rif, 0, 0, cint_vxlan_roo_mc_info.ce_eth_rif_mac, cint_vxlan_roo_mc_info.ce_eth_vrf);
        rv = sand_utils_l3_eth_rif_create(egress_unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
         printf("Error, sand_utils_l3_eth_rif_create ce_eth_rif\n");
         return rv;
        }
    }
    
    /*4.PE2 native-in-rif*/
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_vxlan_roo_mc_info.pe2_native_eth_rif;
    rv = bcm_l3_intf_get(egress_unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        sand_utils_l3_eth_rif_s eth_rif_structure;
        
        sand_utils_l3_eth_rif_s_common_init(egress_unit, 0, &eth_rif_structure, cint_vxlan_roo_mc_info.pe2_native_eth_rif, 0, 0, cint_vxlan_roo_mc_info.pe2_native_eth_rif_mac, cint_vxlan_roo_mc_info.pe2_native_eth_vrf);
        rv = sand_utils_l3_eth_rif_create(egress_unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
         printf("Error, sand_utils_l3_eth_rif_create pe2_native_eth_rif\n");
         return rv;
        }
    }
    
    /*5.pe2 overlay-in-rif*/
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_vxlan_roo_mc_info.pe2_overlay_eth_rif;
    rv = bcm_l3_intf_get(egress_unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        sand_utils_l3_eth_rif_s eth_rif_structure;
        
        sand_utils_l3_eth_rif_s_common_init(egress_unit, 0, &eth_rif_structure, cint_vxlan_roo_mc_info.pe2_overlay_eth_rif, 0, 0, cint_vxlan_roo_mc_info.pe2_overlay_eth_rif_mac, cint_vxlan_roo_mc_info.pe2_overlay_eth_vrf);
        rv = sand_utils_l3_eth_rif_create(egress_unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
         printf("Error, sand_utils_l3_eth_rif_create pe2_overlay_eth_rif\n");
         return rv;
        }
    }
    
    return rv;

}

/**
 * ARP init
 * INPUT:
 *   ingress_unit- traffic incoming unit 
 *   egress_unit- traffic outoging unit 
*/
 int
 multi_dev_vxlan_roo_mc_arp_configuration(
    int ingress_unit,
    int egress_unit)
{
    int rv;

    sand_utils_l3_arp_s arp_structure;



    /*1st copy*/
    /*1.access side arp*/
    sand_utils_l3_arp_s_common_init(egress_unit, 0, &arp_structure, cint_vxlan_roo_mc_info.ce_arp_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION, cint_vxlan_roo_mc_info.ce_next_hop_mac, cint_vxlan_roo_mc_info.ce_eth_rif);
    rv = sand_utils_l3_arp_create(egress_unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }
    cint_vxlan_roo_mc_info.ce_arp_id = arp_structure.l3eg_arp_id;



    /*2st copy*/
    /*1.pe2 native arp*/

    sand_utils_l3_arp_s_common_init(egress_unit, 0, &arp_structure, cint_vxlan_roo_mc_info.pe2_native_arp_id, is_device_or_above(egress_unit,JERICHO_PLUS)?BCM_L3_NATIVE_ENCAP:0, 0, cint_vxlan_roo_mc_info.pe2_native_next_hop_mac, cint_vxlan_roo_mc_info.pe2_native_eth_rif);
    rv = sand_utils_l3_arp_create(egress_unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }
    cint_vxlan_roo_mc_info.pe2_native_arp_id = arp_structure.l3eg_arp_id;

    /*2.pe2 overlay-arp*/
    if (!is_device_or_above(egress_unit,JERICHO_PLUS)) {
        bcm_l2_egress_t l2_egress_overlay;
        
        bcm_l2_egress_t_init(&l2_egress_overlay);
        l2_egress_overlay.flags    = BCM_L2_EGRESS_WITH_ID;         
        BCM_L3_ITF_SET(l2_egress_overlay.encap_id, BCM_L3_ITF_TYPE_LIF, cint_vxlan_roo_mc_info.pe2_overlay_arp_id);
        sal_memcpy(l2_egress_overlay.dest_mac, cint_vxlan_roo_mc_info.pe2_overlay_next_hop_mac, 6);
        sal_memcpy(l2_egress_overlay.src_mac, cint_vxlan_roo_mc_info.pe2_overlay_eth_rif_mac, 6);
        l2_egress_overlay.outer_vlan = cint_vxlan_roo_mc_info.pe2_overlay_eth_rif; 
        l2_egress_overlay.ethertype  = 0x0800;
        l2_egress_overlay.outer_tpid = 0x8100;
        l2_egress_overlay.vlan_qos_map_id = 0;
    
        rv = bcm_l2_egress_create(egress_unit, &l2_egress_overlay);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_egress_create\n");
            return rv;
        }
        cint_vxlan_roo_mc_info.pe2_overlay_arp_id = l2_egress_overlay.encap_id;        
    } else {
        sand_utils_l3_arp_s_common_init(egress_unit, 0, &arp_structure, cint_vxlan_roo_mc_info.pe2_overlay_arp_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION, cint_vxlan_roo_mc_info.pe2_overlay_next_hop_mac, cint_vxlan_roo_mc_info.pe2_overlay_eth_rif);
        rv = sand_utils_l3_arp_create(egress_unit, &arp_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create AC egress object ARP only\n");
            return rv;
        }
        cint_vxlan_roo_mc_info.pe2_overlay_arp_id = arp_structure.l3eg_arp_id;
    }



    return rv;

}



/**
 * Core side routing entry, used for traffic core---->access direction
 * INPUT:
 *   ingress_unit- traffic incoming unit 
 *   egress_unit- traffic outoging unit 
*/
 int
multi_dev_vxlan_roo_mc_forward_entry_configuration(
    int ingress_unit,
    int egress_unit)
{
    int rv;
    int pp_db_replication_list[2];
    int pp_db_rep_index = 0x300000;
    int l3_mc_id;

    /*
     * 1. Add replication into the group
     */
    bcm_multicast_replication_t replications[5];
    int rep_count = 2;


    /* 1st copy: to local CE*/
    bcm_multicast_replication_t_init(&replications[0]);
    replications[0].encap1 =  BCM_L3_ITF_VAL_GET(cint_vxlan_roo_mc_info.ce_arp_id);
    replications[0].port = cint_vxlan_roo_mc_info.ce_port;


    /* 2nd copy: to remote PE2 */
    bcm_multicast_replication_t_init(&replications[1]);
    replications[1].port = cint_vxlan_roo_mc_info.pe2_port;
    /*JR2 use PP DB to carry 2 pointers*/
    if (is_device_or_above(egress_unit, JERICHO2)) {
        BCM_L3_ITF_SET(pp_db_replication_list[0],BCM_L3_ITF_TYPE_LIF,cint_vxlan_roo_mc_info.pe2_native_eth_rif);
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(pp_db_replication_list[1],cint_vxlan_roo_mc_info.pe2_vxlan_tunnel_id);
        
        rv = bcm_multicast_encap_extension_create(egress_unit,
                                 BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID,
                                 &pp_db_rep_index, 
                                 2, pp_db_replication_list);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function bcm_multicast_encap_extension_create, \n");
            return rv;
        }

        replications[1].encap1 = pp_db_rep_index;
    
    } else {
        replications[1].encap1 = BCM_L3_ITF_VAL_GET(cint_vxlan_roo_mc_info.pe2_native_eth_rif);
        replications[1].encap2 = BCM_GPORT_TUNNEL_ID_GET(cint_vxlan_roo_mc_info.pe2_vxlan_tunnel_id);
        replications[1].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;
    }

    rv = multi_dev_ipmc_create_multicast(ingress_unit, egress_unit, replications,rep_count, cint_vxlan_roo_mc_info.mc_group,2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function create_multicast, \n");
        return rv;
    }

    /*
     * 2. Add IPv4 IPMC entry
     */
    BCM_MULTICAST_L3_SET(l3_mc_id,cint_vxlan_roo_mc_info.mc_group);
    rv = add_ipv4_ipmc(ingress_unit, cint_vxlan_roo_mc_info.dip_into_mc, 0xFFFFFFFF, cint_vxlan_roo_mc_info.host_ip, 0xFFFFFFFF,
                       cint_vxlan_roo_mc_info.pe1_native_eth_rif, cint_vxlan_roo_mc_info.pe1_native_eth_vrf,
                       (is_device_or_above(ingress_unit, JERICHO2)?cint_vxlan_roo_mc_info.mc_group:l3_mc_id), 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_ipv4_ipmc to LEM table, \n",proc_name);
        return rv;
    }

    return rv;


}


/**
 * mpls tunnel termination entry adding
 * INPUT:
 *   ingress_unit    - traffic incoming unit 
 *   egress_unit    - traffic outgoing unit 
*/
 int
multi_dev_vxlan_roo_mc_tunnel_termination_configuration(
    int ingress_unit,
    int egress_unit)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /*PE1 incoming traffic*/

    /* Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.type = bcmTunnelTypeVxlan;
    tunnel_term.dip = cint_vxlan_roo_mc_info.tunnel_dip;
    tunnel_term.dip_mask = cint_vxlan_roo_mc_info.tunnel_dip_mask;
    tunnel_term.sip = cint_vxlan_roo_mc_info.tunnel_sip;
    tunnel_term.sip_mask = cint_vxlan_roo_mc_info.tunnel_sip_mask;
    tunnel_term.vrf = cint_vxlan_roo_mc_info.pe1_overlay_eth_vrf;


    rv = bcm_tunnel_terminator_create(ingress_unit,&tunnel_term);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }
    /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
     bcm_l3_ingress_t ing_intf;
     bcm_l3_ingress_t_init(&ing_intf);
     bcm_if_t intf_id;

     ing_intf.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
     ing_intf.vrf = cint_vxlan_roo_mc_info.tunnel_termination_vrf;

     if (is_device_or_above(ingress_unit, JERICHO2)) {
         /* Convert tunnel's GPort ID to intf ID */
          BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);
          rv = bcm_l3_ingress_create(ingress_unit, ing_intf, intf_id);
          if (rv != BCM_E_NONE)
          {   
              printf("Error, bcm_l3_ingress_create\n");
              return rv;
          }
     }
    
    cint_vxlan_roo_mc_info.vxlan_tunnel_if = tunnel_term.tunnel_id;

    /*PE2 incoming direction, just for configuration completely*/
    bcm_tunnel_terminator_t_init(&tunnel_term);

    tunnel_term.type = bcmTunnelTypeVxlan;
    tunnel_term.dip = cint_vxlan_roo_mc_info.pe2_tunnel_dip;
    tunnel_term.dip_mask = cint_vxlan_roo_mc_info.tunnel_dip_mask;
    tunnel_term.sip = cint_vxlan_roo_mc_info.pe2_tunnel_sip;
    tunnel_term.sip_mask = cint_vxlan_roo_mc_info.tunnel_sip_mask;
    tunnel_term.vrf = cint_vxlan_roo_mc_info.pe2_overlay_eth_vrf;


    rv = bcm_tunnel_terminator_create(egress_unit,&tunnel_term);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }
    /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
     bcm_l3_ingress_t_init(&ing_intf);
     intf_id = 0;

     ing_intf.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
     ing_intf.vrf = cint_vxlan_roo_mc_info.pe2_overlay_eth_vrf;

     if (is_device_or_above(egress_unit, JERICHO2)) {
         /* Convert tunnel's GPort ID to intf ID */
          BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);
          rv = bcm_l3_ingress_create(egress_unit, ing_intf, intf_id);
          if (rv != BCM_E_NONE)
          {   
              printf("Error, bcm_l3_ingress_create\n");
              return rv;
          }
     }
    
    cint_vxlan_roo_mc_info.pe2_vxlan_tunnel_if = tunnel_term.tunnel_id;

    return rv;

}

/**
 * mpls tunnel encapsulation entry adding
 * INPUT:
 *   ingress_unit    - PE2 traffic incoming unit 
 *   egress_unit    - PE2 traffic outgoing unit 
*/
 int
multi_dev_vxlan_roo_mc_tunnel_encap_configuration(
    int ingress_unit,
    int egress_unit)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    /*PE2 outgoing direction*/
    bcm_l3_intf_t_init(&l3_intf);
    /* Create IP tunnel initiator for encapsulating Vxlan tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.type = bcmTunnelTypeVxlan;
    tunnel_init.dip = cint_vxlan_roo_mc_info.pe2_tunnel_dip; /* default: 173.0.0.1 */
    tunnel_init.sip = cint_vxlan_roo_mc_info.pe2_tunnel_sip; /* default: 174.0.0.1 */
    tunnel_init.flags = BCM_TUNNEL_WITH_ID;
    tunnel_init.dscp = 10; /* default: 10 */
     if (is_device_or_above(unit, JERICHO2)) {
        tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    } else {
        tunnel_init.dscp_sel = bcmTunnelDscpAssign;  
    }
    tunnel_init.l3_intf_id = cint_vxlan_roo_mc_info.pe2_overlay_arp_id;
    tunnel_init.ttl = 64; /* default: 64 */
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    tunnel_init.encap_access = bcmEncapAccessTunnel2;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init.tunnel_id, cint_vxlan_roo_mc_info.pe2_vxlan_tunnel_id);

    rv = bcm_tunnel_initiator_create(egress_unit,&l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
        return rv;
    }

    cint_vxlan_roo_mc_info.pe2_vxlan_tunnel_id = tunnel_init.tunnel_id;

    /*PE1 outgoing direction, just for configuration completely*/

    bcm_l3_intf_t_init(&l3_intf);
    /* Create IP tunnel initiator for encapsulating Vxlan tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.type = bcmTunnelTypeVxlan;
    tunnel_init.dip = cint_vxlan_roo_mc_info.tunnel_dip; /* default: 173.0.0.1 */
    tunnel_init.sip = cint_vxlan_roo_mc_info.tunnel_sip; /* default: 174.0.0.1 */
    tunnel_init.flags = BCM_TUNNEL_WITH_ID;
    tunnel_init.dscp = 10; /* default: 10 */
     if (is_device_or_above(unit, JERICHO2)) {
        tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    } else {
        tunnel_init.dscp_sel = bcmTunnelDscpAssign;  
    }
    tunnel_init.ttl = 64; /* default: 64 */
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    tunnel_init.encap_access = bcmEncapAccessTunnel2;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init.tunnel_id, cint_vxlan_roo_mc_info.pe1_vxlan_tunnel_id);

    rv = bcm_tunnel_initiator_create(ingress_unit,&l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
        return rv;
    }

    cint_vxlan_roo_mc_info.pe1_vxlan_tunnel_id = tunnel_init.tunnel_id;


    return rv;

}


/**
 * configuration from access---->core

 * INPUT:
 *   ingress_unit- traffic incoming unit 
 *   egress_unit- traffic outoging unit 
*/
 int
 multi_dev_vxlan_roo_mc_main_configuration(
    int ingress_unit,
    int egress_unit)
{
    int rv;
    int vxlan_port_id = 0;
    /*
     * 1.Basic ingress L2 configuration(Port default LIF)
     */
    rv = multi_dev_ingress_port_configuration(ingress_unit,cint_vxlan_roo_mc_info.pe1_port,cint_vxlan_roo_mc_info.pe1_overlay_eth_rif);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ingress_port_configuration unit=%d, port=%d\n",ingress_unit,cint_vxlan_roo_mc_info.pe1_port);
     return rv;
    }

    rv = multi_dev_egress_port_configuration(egress_unit,cint_vxlan_roo_mc_info.ce_port);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_EXISTS))
    {
     printf("Error, multi_dev_egress_port_configuration unit=%d, port=%d\n",egress_unit,cint_vxlan_roo_mc_info.ce_port);
     return rv;
    }

    rv = multi_dev_egress_port_configuration(egress_unit,cint_vxlan_roo_mc_info.pe2_port);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_EXISTS))
    {
     printf("Error, multi_dev_egress_port_configuration unit=%d, port=%d\n",egress_unit,cint_vxlan_roo_mc_info.pe2_port);
     return rv;
    }


    /*
     * 2. vxlan vpn creation, put it here just because this routing will reset the nativ-rif property.
     *    So call this first.
     */
    rv = vxlan_roo_basic_vxlan_open_vpn(ingress_unit, cint_vxlan_roo_mc_info.vpn_id, cint_vxlan_roo_mc_info.vni);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_roo_basic_vxlan_open_vpn, vpn=%d, \n", proc_name, cint_vxlan_roo_mc_info.vpn_id);
        return rv;
    }

    rv = vxlan_roo_basic_vxlan_open_vpn(egress_unit, cint_vxlan_roo_mc_info.pe2_native_eth_rif, cint_vxlan_roo_mc_info.pe2_vni_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_roo_basic_vxlan_open_vpn, vpn=%d, \n", proc_name, cint_vxlan_roo_mc_info.pe2_vni_id);
        return rv;
    }



    /*
     * 4.L3 configuration, this part is shared with out-of-overlay direction
     */


    rv = multi_dev_vxlan_roo_mc_l3_intf_configuration(ingress_unit,egress_unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_mc_l3_intf_configuration\n");
     return rv;
    }

    /*
     * 5.arp configuration
     */
    rv = multi_dev_vxlan_roo_mc_arp_configuration(ingress_unit,egress_unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_mc_arp_configuration\n");
     return rv;
    }

    /*
     * 6.arp configuration
     */
    rv = multi_dev_vxlan_roo_mc_tunnel_termination_configuration(ingress_unit,egress_unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_mc_tunnel_termination_configuration\n");
     return rv;
    }


    /*
     * 7.Vxlan tunnel encapsulation
     */

    rv = multi_dev_vxlan_roo_mc_tunnel_encap_configuration(ingress_unit,egress_unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_mc_tunnel_encap_configuration\n");
     return rv;
    }
   

    /* 
     * 8. Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    rv = vxlan_port_add(ingress_unit, cint_vxlan_roo_mc_info.pe1_port, cint_vxlan_roo_mc_info.vxlan_tunnel_if, cint_vxlan_roo_mc_info.pe1_vxlan_tunnel_id, &vxlan_port_id);
    if (rv != BCM_E_NONE) 
    {
        printf("%s(): Error, vxlan_port_add \n", proc_name);
        return rv;
    }

    rv = vxlan_port_add(egress_unit, cint_vxlan_roo_mc_info.pe2_port, cint_vxlan_roo_mc_info.pe2_vxlan_tunnel_if, cint_vxlan_roo_mc_info.pe2_vxlan_tunnel_id, &vxlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_port_add \n", proc_name);
        return rv;
    }


    /*
     * 9. FWD configuration(IPMC)
     */
    rv = multi_dev_vxlan_roo_mc_forward_entry_configuration(ingress_unit,egress_unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_mc_forward_entry_configuration\n");
     return rv;
    }

    return rv;
}

/**
 * Main entrance for IPv4 UC on multi-device
 * INPUT:
 *   ingress_unit- traffic incoming unit 
 *   egress_unit- traffic outoging unit 
 *   pe1_sys_port- traffic incoming sys port 
 *   ce_sys_port- CE traffic outoging sys port 
 *   pe2_sys_port- PE2 traffic outoging sys port 
*/
 int
 multi_dev_vxlan_roo_mc_example(
    int ingress_unit,
    int egress_unit,
    int pe1_sys_port,
    int ce_sys_port,
    int pe2_sys_port)
{
    int rv;

    rv = multi_dev_vxlan_roo_mc_init(ingress_unit,egress_unit,pe1_sys_port,ce_sys_port,pe2_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_init\n");
     return rv;
    }


    rv = multi_dev_vxlan_roo_mc_main_configuration(ingress_unit,egress_unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_into_overlay_configuration\n");
     return rv;
    }

    return rv;
}

/**
 * example for VXLAN ROO IPMC 2nd PASS
 * INPUT:
 *   1st pass: ingress_unit- IPMC lookup -> recycle port
 *   2nd pass: VXLAN tunnel termination -> IPMC lookup -> out port
 *   pe1_sys_port- traffic incoming sys port 
 *   ce_sys_port- CE traffic outoging sys port 
*/
 int
 multi_dev_vxlan_roo_mc_2nd_pass_example(
    int ingress_unit,
    int egress_unit,
    int pe1_sys_port,
    int ce_sys_port,
    int pe2_sys_port)
{
    int rv;
    int vxlan_port_id = 0;
    ip_tunnel_basic_info.provider_eth_vrf = cint_vxlan_roo_mc_info.pe1_overlay_eth_vrf;
    ip_tunnel_basic_info.provider_eth_rif = cint_vxlan_roo_mc_info.pe1_overlay_eth_rif;
    ip_tunnel_basic_info.provider_eth_rif_mac=cint_vxlan_roo_mc_info.pe1_overlay_eth_rif_mac;
    ip_tunnel_basic_info.encap_arp_id[0] = 0;
    int l3_mc_id;
    bcm_multicast_replication_t replications[1];
    int rep_count = 1;
    int tunnel_idx = 0;
    ip_tunnel_basic_info.term_bud=1;
    ip_tunnel_basic_info.dip[0] = g_vxlan_roo_mc.overlay_mc_dip;
    ip_tunnel_basic_info.tunnel_type[0] = bcmTunnelTypeVxlan;
    ip_tunnel_basic_info.nof_tunnels = 1;

    rv = multi_dev_vxlan_roo_mc_init(ingress_unit,egress_unit,pe1_sys_port,ce_sys_port,pe2_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_init\n");
     return rv;
    }

    /*
     * Basic ingress L2 configuration(Port default LIF)
     */
    rv = multi_dev_ingress_port_configuration(ingress_unit,cint_vxlan_roo_mc_info.pe1_port,cint_vxlan_roo_mc_info.pe1_overlay_eth_rif);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ingress_port_configuration unit=%d, port=%d\n",ingress_unit,cint_vxlan_roo_mc_info.pe1_port);
     return rv;
    }

    rv = multi_dev_egress_port_configuration(egress_unit,cint_vxlan_roo_mc_info.ce_port);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_EXISTS))
    {
     printf("Error, multi_dev_egress_port_configuration unit=%d, port=%d\n",egress_unit,cint_vxlan_roo_mc_info.ce_port);
     return rv;
    }

    /*
     * vxlan vpn creation, put it here just because this routing will reset the nativ-rif property.
     *    So call this first.
     */
    rv = sand_utils_vxlan_open_vpn(ingress_unit, cint_vxlan_roo_mc_info.vpn_id, 0, cint_vxlan_roo_mc_info.vni);
    if (rv != BCM_E_NONE) 
    {
        printf("Error(%d), in sand_utils_vxlan_open_vpn, vpn=%d, vdc=%d \n", rv, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_vdc);
        return rv;
    }

    /*
     * L3 configuration, this part is shared with out-of-overlay direction
     */
    rv = multi_dev_vxlan_roo_mc_l3_intf_configuration(ingress_unit,egress_unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_mc_l3_intf_configuration\n");
     return rv;
    }

    /*
     * 2nd PASS tunnel termination lookup and the tunnel termination inlif
     */
    rv = ip_tunnel_term_create(ingress_unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in function ip_tunnel_term_create \n", rv);
        return rv;
    }

    /*
     * Create the tunnel initiator
     */
    rv = ip_tunnel_encap_create(egress_unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in ip_tunnel_encap_create \n", rv);
        return rv;
    }

    /*
     * Configure ip tunnel termination ingress interfaces and ip tunnel encapsulation egress interfaces
     */
    rv = vxlan_roo_mc_open_route_interfaces_provider_dummy(ingress_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vxlan_roo_mc_open_route_interfaces_provider_dummy \n");
        return rv;
    }

    /* configure recycle port */
    rv = bcm_port_control_set(ingress_unit,g_vxlan_roo_mc.recycle_port, bcmPortControlOverlayRecycle, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set\n");
        return rv;
    }

    /*
    * 1st PASS IPMC to recycle port and encap
    */
    uint32 flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;

    /* create mc group */
    if (!is_device_or_above(ingress_unit, JERICHO2)) {
        flags |= BCM_MULTICAST_TYPE_L3;
    }
    rv = bcm_multicast_create(ingress_unit, flags, &g_vxlan_roo_mc.overlay_ipmc);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    if (!is_device_or_above(ingress_unit, JERICHO2)) {
        rv = bcm_multicast_ingress_add(ingress_unit, g_vxlan_roo_mc.overlay_ipmc, g_vxlan_roo_mc.recycle_port, g_vxlan_roo_mc.provider_eth_dummy_rif);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_ingress_add mc_group_id:  0x%08x  port:  0x%08x  encap_id:  0x%08x \n", g_vxlan_roo_mc.overlay_ipmc, g_vxlan_roo_mc.recycle_port, g_vxlan_roo_mc.provider_eth_dummy_rif);
            return rv;
        }
    } else {
        bcm_multicast_replication_t_init(&replications);
        replications.encap1 = g_vxlan_roo_mc.recycle_entry_encap_id & 0x3fffff;
        replications.port = g_vxlan_roo_mc.recycle_port;
        rv = bcm_multicast_add(ingress_unit, g_vxlan_roo_mc.overlay_ipmc, BCM_MULTICAST_INGRESS_GROUP, 1, &replications);
        if (rv != BCM_E_NONE)
        {
          printf("Error, in bcm_multicast_add mc_group_id:  0x%08x  port:  0x%08x  encap_id:  0x%08x \n", g_vxlan_roo_mc.overlay_ipmc, g_vxlan_roo_mc.recycle_port, g_vxlan_roo_mc.recycle_entry_encap_id);
          return rv;
        }
    }

    /* Add IPv4 IPMC */
    bcm_ipmc_addr_t data;
    bcm_ipmc_addr_t_init(&data);
    data.flags = 0x0;
    data.group = g_vxlan_roo_mc.overlay_ipmc;  
    data.mc_ip_addr = g_vxlan_roo_mc.overlay_mc_dip;
    data.mc_ip_mask = g_vxlan_roo_mc.overlay_mc_dip_mask;
    data.vrf = ip_tunnel_basic_info.provider_eth_vrf;
    if (is_device_or_above(ingress_unit, JERICHO2)) {
        data.vid = ip_tunnel_basic_info.provider_eth_rif;
    }
    rv = bcm_ipmc_add(ingress_unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add IPv4\n");
        return rv;
    }

    /*
    * IPMC copy to out port, in this example, use ingress MC, so set out port to system port
    */
    bcm_multicast_replication_t_init(&replications[0]);
    replications[0].encap1 = cint_vxlan_roo_mc_info.ce_eth_rif;
    replications[0].port = ce_sys_port;

    rv = multi_dev_ipmc_create_multicast(ingress_unit, egress_unit, replications,rep_count, cint_ipmc_route_info.mc_group,cint_ipmc_route_info.mc_mode);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function create_multicast, \n");
        return rv;
    }

    /*
     * Add IPv4 IPMC entry
     */
    BCM_MULTICAST_L3_SET(l3_mc_id,cint_ipmc_route_info.mc_group);
    rv = add_ipv4_ipmc(ingress_unit, cint_ipmc_route_info.mc_group_ip, 0xFFFFFFFF, cint_ipmc_route_info.host_ip, 0xFFFFFFFF,
                       cint_vxlan_roo_mc_info.pe1_native_eth_rif, cint_vxlan_roo_mc_info.pe1_native_eth_vrf,
                       (is_device_or_above(ingress_unit, JERICHO2)?cint_ipmc_route_info.mc_group:l3_mc_id), 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_ipv4_ipmc to LEM table, \n",proc_name);
        return rv;
    }

    /*
     * For JR1, set VXLAN tunnel termination
     */
    rv = vxlan_roo_mc_vxlan_dummy_port_add(ingress_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), vxlan_roo_mc_vxlan_dummy_port_add \n", rv);
        return rv;
    }

    return rv;
}


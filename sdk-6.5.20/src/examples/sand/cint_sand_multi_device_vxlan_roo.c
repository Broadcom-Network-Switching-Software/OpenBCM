/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Multi-Dev Vxlan ROO~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_multi_device_vxlan_roo.c
 * Purpose: Demo Vxlan ROO under multi device system
 *
 * Multi-Device system require the ingress/egress configuration separately and relatively. 
 * This cint demo how to configurer the Vxlan ROO service under multi-device system.
 *  1. Create overlay L3 intf/arp;
 *  2. Create overlay Vxlan tunnel termination and encapsulation;
 *  3. Create native rif/arp;
 *  4. Create FEC and FWD entry in both direction;
 *  5. Send IPv4 traffic to verify
 *
 *
 *  2 cases are tested:
 *   1.IPv4oEthoVxlanoUDPoIPv4oEth-------->IPv4oEth(core--->access,out of overlay)
 *   2.IPv4oEth---------->IPv4oEthoVxlanoUDPoIPv4oEth(access--->core,into overlay)
 */


 struct cint_multi_dev_vxlan_roo_info_s 
{
    int vxlan_network_group_id;             /* ingress and egress orientation for VXLAN */
    bcm_ip_t tunnel_dip;                    /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;               /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                    /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;               /* tunnel SIP mask*/    
    bcm_gport_t vxlan_tunnel_id;                  /* Tunnel ID */
    int vxlan_tunnel_if;                          /* Tunnel Interface */
    int vpn_id;                             /* VPN ID */
    int vni;                                /* VNI of the VXLAN */
    int access_port;                        /* incoming port from access side */
    int core_port;                          /* incoming port from core side */
    int core_native_eth_rif;                /* core native RIF */
    int core_native_eth_vrf;                /* core native VRF */
    int core_overlay_eth_rif;               /* core overlay RIF */
    int core_overlay_eth_vrf;               /* core overlay VRF */
    int access_eth_rif;                     /* access RIF */
    int access_eth_vrf;                     /* access VRF */
    int core_native_arp_id;                 /* Id for core native ARP entry */
    int core_overlay_arp_id;                /* Id for core overlay ARP entry */
    int access_arp_id;                      /* Id for access ARP entry */
    int core_native_fec_id;                 /* core native fec id for native outrif and native arp */
    int core_overlay_fec_id;                /* core overlay fec id for overlay */
    int access_fec_id;                      /* access fec id for access outrif and access arp */
    bcm_mac_t access_eth_fwd_mac;           /* mac for bridge fwd */
    bcm_mac_t core_native_eth_rif_mac;      /* mac for core native RIF */
    bcm_mac_t core_overlay_eth_rif_mac;     /* mac for core overlay RIF */
    bcm_mac_t access_eth_rif_mac;           /* mac for access RIF */
    bcm_mac_t core_native_next_hop_mac;     /* mac for next core native hop */
    bcm_mac_t core_overlay_next_hop_mac;    /* mac for next core overlay hop */
    bcm_mac_t access_next_hop_mac;          /* mac for next access hop */
    bcm_ip_t route_dip_into_overlay;        /* first dip */
    bcm_ip_t route_dip_out_of_overlay;      /* second dip */
    int tunnel_termination_vrf;
    bcm_if_t core_overlay_encap_tunnel_intf;
};
 
 
 cint_multi_dev_vxlan_roo_info_s cint_vxlan_roo_info=
{
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
	  * vxlan tunnel id | vxlan tunnel_if
	  */
	 0x3003, 10, 
	 /*
	  * VPN ID
	  */
	 15,
	 /*
	  * VNI
	  */
	 5000,
	 /*
	  * ports : access_port | core_port 
	  */
	 0, 0,
	 /*
	  * core_native_eth_rif,core_native_eth_vrf
	  */
	 15,30,
	 /*
	  * core_overlay_eth_rif, core_overlay_eth_vrf
	  */
	 20, 5,
	 /*
	  * access_eth_rif, access_eth_vrf
	  */
	 10, 1,
	 /*
	  * core_native_arp_id | core_overlay_arp_id | access_arp_id
	  */
	 0x3000, 0x3001, 0x3002,
	 /*
	  * core_native_fec_id | core_overlay_fec_id | access_fec_id 
	  */
	 0xa001, 0x10001, 0xa002,
	 /*
	  * access_eth_fwd_mac | core_native_eth_rif_mac | core_overlay_eth_rif_mac | access_eth_rif_mac
	  */
	 {0x00, 0x00, 0xA5, 0x5A, 0xA5, 0x5A},
	 {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x22},
	 {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x33},
	 {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x44},
	 /*
	  * core_native_next_hop_mac |	core_overlay_next_hop_mac | access_next_hop_mac
	  */
	 {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1D},
	 {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1E},
	 {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1F},
	 /*
	  *route_dip_into_overlay | route_dip_out_of_overlay
	  127.255.255.02 , 127.255.255.03
	  */
	 0x7fffff02, 0x7fffff03,
	 /*
	  *tunnel_termination_vrf | core_overlay_encap_tunnel_intf
	  */
	 5, 0


};

/**
 * Main entrance for vlxan roo configuration
 * INPUT:
 *   core_side_unit- core side unit 
 *   access_side_unit- access side unit 
 *   core_side_sys_port- core side sys port 
 *   access_side_sys_port- access side sys port 
*/
 int
 multi_dev_vxlan_roo_init(
    int core_side_unit,
    int access_side_unit,
    int core_side_sys_port,
    int access_side_sys_port)
{
    int rv;
    int local_port,is_local;

    /*
     * 0: Convert the sysport to local port
     */
    multi_dev_system_port_to_local(core_side_unit,core_side_sys_port,&cint_vxlan_roo_info.core_port);

    multi_dev_system_port_to_local(access_side_unit,access_side_sys_port,&cint_vxlan_roo_info.access_port);


    if (!is_device_or_above(access_side_unit, JERICHO2)) {
        cint_vxlan_roo_info.core_native_fec_id = 0x2000;
        cint_vxlan_roo_info.core_overlay_fec_id = 0x5000;
    }
    else
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(access_side_unit, 0, &cint_vxlan_roo_info.core_native_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }

        rv = get_first_fec_in_range_which_not_in_ecmp_range(access_side_unit, 1, &cint_vxlan_roo_info.core_overlay_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }

    if(is_device_or_above(core_side_unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(core_side_unit, 0, &cint_vxlan_roo_info.access_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        cint_vxlan_roo_info.access_fec_id++;
    }


    bcm_vxlan_init(core_side_unit);
    bcm_vxlan_init(access_side_unit);
 
    return rv;

}


/**
 * L3 intf init
 * INPUT:
 *   core_side_unit- core side unit 
 *   access_side_unit- access side unit 
 *   core_side_sys_port- core side sys port 
 *   access_side_sys_port- access side sys port
*/
 int
 multi_dev_vxlan_roo_l3_intf_configuration(
    int core_side_unit,
    int access_side_unit,
    int core_side_sys_port,
    int access_side_sys_port)
{
    int rv;
    bcm_l3_intf_t l3_intf;

    /*1.access side In-RIF*/
    
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_vxlan_roo_info.access_eth_rif;
    rv = bcm_l3_intf_get(access_side_unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        sand_utils_l3_eth_rif_s eth_rif_structure;
        
        sand_utils_l3_eth_rif_s_common_init(access_side_unit, 0, &eth_rif_structure, cint_vxlan_roo_info.access_eth_rif, 0, 0, cint_vxlan_roo_info.access_eth_rif_mac, cint_vxlan_roo_info.access_eth_vrf);
        rv = sand_utils_l3_eth_rif_create(access_side_unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
         printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
         return rv;
        }
    }
    
    
    /*2.core side native-out-rif*/
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_vxlan_roo_info.core_native_eth_rif;
    rv = bcm_l3_intf_get(core_side_unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        sand_utils_l3_eth_rif_s eth_rif_structure;
        
        sand_utils_l3_eth_rif_s_common_init(core_side_unit, 0, &eth_rif_structure, cint_vxlan_roo_info.core_native_eth_rif, 0, 0, cint_vxlan_roo_info.core_native_eth_rif_mac, cint_vxlan_roo_info.core_native_eth_vrf);
        rv = sand_utils_l3_eth_rif_create(core_side_unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
         printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
         return rv;
        }
    }
    
    /*3.core side overlay-out-rif*/
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_vxlan_roo_info.core_overlay_eth_rif;
    rv = bcm_l3_intf_get(core_side_unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        sand_utils_l3_eth_rif_s eth_rif_structure;
        
        sand_utils_l3_eth_rif_s_common_init(core_side_unit, 0, &eth_rif_structure, cint_vxlan_roo_info.core_overlay_eth_rif, 0, 0, cint_vxlan_roo_info.core_overlay_eth_rif_mac, cint_vxlan_roo_info.core_overlay_eth_vrf);
        rv = sand_utils_l3_eth_rif_create(core_side_unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
         printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
         return rv;
        }
    }
    
    return rv;

}

/**
 * ARP init
 * INPUT:
 *   core_side_unit- core side unit 
 *   access_side_unit- access side unit 
 *   core_side_sys_port- core side sys port 
 *   access_side_sys_port- access side sys port
 *   into_overlay - 1: access-->core; 0: core--->access 
*/
 int
 multi_dev_vxlan_roo_arp_configuration(
    int core_side_unit,
    int access_side_unit,
    int core_side_sys_port,
    int access_side_sys_port,
    int into_overlay)
{
    int rv;

    sand_utils_l3_arp_s arp_structure;


    if (into_overlay) {

        /*1.core side native-arp*/

        
        int device_is_jericho2;
        rv = is_device_jericho2(core_side_unit, &device_is_jericho2);
        /* in Jericho2, for ROO application, context selection expects a native ARP in result type: ETPS_ARP
           ARP entry with result type ETPS_ARP doesn't have eth_rif field (VSI in dbal table) */
        int native_eth_rif = 0;  
        sand_utils_l3_arp_s_common_init(core_side_unit, 0, &arp_structure, cint_vxlan_roo_info.core_native_arp_id, 
                                        is_device_or_above(core_side_unit,JERICHO_PLUS)?BCM_L3_NATIVE_ENCAP:0, 0, 
                                        cint_vxlan_roo_info.core_native_next_hop_mac, native_eth_rif);
        rv = sand_utils_l3_arp_create(core_side_unit, &arp_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create AC egress object ARP only\n");
            return rv;
        }
        cint_vxlan_roo_info.core_native_arp_id = arp_structure.l3eg_arp_id;

        /*2.core side overlay-arp*/
        if (!is_device_or_above(core_side_unit,JERICHO_PLUS)) {
            bcm_l2_egress_t l2_egress_overlay;
            
            bcm_l2_egress_t_init(&l2_egress_overlay);
            l2_egress_overlay.flags    = BCM_L2_EGRESS_WITH_ID;         
            BCM_L3_ITF_SET(l2_egress_overlay.encap_id, BCM_L3_ITF_TYPE_LIF, cint_vxlan_roo_info.core_overlay_arp_id);
            sal_memcpy(l2_egress_overlay.dest_mac, cint_vxlan_roo_info.core_overlay_next_hop_mac, 6);
            sal_memcpy(l2_egress_overlay.src_mac, cint_vxlan_roo_info.core_overlay_eth_rif_mac, 6);
            l2_egress_overlay.outer_vlan = cint_vxlan_roo_info.core_overlay_eth_rif; 
            l2_egress_overlay.outer_tpid = 0x8100;
            l2_egress_overlay.ethertype  = 0x0800;
            l2_egress_overlay.vlan_qos_map_id = 0;
        
            rv = bcm_l2_egress_create(core_side_unit, &l2_egress_overlay);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_l2_egress_create\n");
                return rv;
            }
            cint_vxlan_roo_info.core_overlay_arp_id = l2_egress_overlay.encap_id;        
        } else {
            sand_utils_l3_arp_s_common_init(core_side_unit, 0, &arp_structure, cint_vxlan_roo_info.core_overlay_arp_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION, cint_vxlan_roo_info.core_overlay_next_hop_mac, cint_vxlan_roo_info.core_overlay_eth_rif);
            rv = sand_utils_l3_arp_create(core_side_unit, &arp_structure);
            if (rv != BCM_E_NONE)
            {
                printf("Error, create AC egress object ARP only\n");
                return rv;
            }
            cint_vxlan_roo_info.core_overlay_arp_id = arp_structure.l3eg_arp_id;
        }


    } else {
        /*1.access side arp*/
        sand_utils_l3_arp_s_common_init(access_side_unit, 0, &arp_structure, cint_vxlan_roo_info.access_arp_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION, cint_vxlan_roo_info.access_next_hop_mac, cint_vxlan_roo_info.access_eth_rif);
        rv = sand_utils_l3_arp_create(access_side_unit, &arp_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create AC egress object ARP only\n");
            return rv;
        }
        cint_vxlan_roo_info.access_arp_id = arp_structure.l3eg_arp_id;

    }

    return rv;

}


/**
 * Core side FEC, used for traffic core---->access direction
 * INPUT:
 *   core_side_unit- core side unit 
 *   access_side_unit- access side unit 
 *   core_side_sys_port- core side sys port 
 *   access_side_sys_port- access side sys port
 *   into_overlay - 1: access-->core; 0: core--->access 
*/
 int
 multi_dev_vxlan_roo_fec_entry_configuration(
    int core_side_unit,
    int access_side_unit,
    int core_side_sys_port,
    int access_side_sys_port,
    int into_overlay)
{
    int rv;
    sand_utils_l3_fec_s fec_structure;

    if (into_overlay) {

        /*1. FEC for Vxlan tunnel*/
        sand_utils_l3_fec_s_init(access_side_unit, 0x0, &fec_structure);
        fec_structure.destination  = core_side_sys_port;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(fec_structure.tunnel_gport,cint_vxlan_roo_info.vxlan_tunnel_id);
        fec_structure.flags        = BCM_L3_2ND_HIERARCHY;
        fec_structure.allocation_flags= BCM_L3_WITH_ID;
        
        fec_structure.fec_id       = cint_vxlan_roo_info.core_overlay_fec_id;
        rv = sand_utils_l3_fec_create(access_side_unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object hierarchy 2 FEC 0x%d only\n",fec_structure.fec_id);
            return rv;
        }
        cint_vxlan_roo_info.core_overlay_fec_id = fec_structure.l3eg_fec_id;

        /*2. FEC for L3 route result*/
        sand_utils_l3_fec_s_init(access_side_unit, 0x0, &fec_structure);
        BCM_GPORT_FORWARD_PORT_SET(fec_structure.destination,cint_vxlan_roo_info.core_overlay_fec_id);
        fec_structure.tunnel_gport = cint_vxlan_roo_info.core_native_eth_rif;
        fec_structure.tunnel_gport2 = cint_vxlan_roo_info.core_native_arp_id;

        fec_structure.allocation_flags = BCM_L3_WITH_ID;
        fec_structure.fec_id      = cint_vxlan_roo_info.core_native_fec_id;
        rv = sand_utils_l3_fec_create(access_side_unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object hierarchy 1 FEC 0x%d only\n",fec_structure.fec_id );
            return rv;
        }
        cint_vxlan_roo_info.core_native_fec_id = fec_structure.l3eg_fec_id;
        
    } else {

        /*FEC for L3 route result*/
        sand_utils_l3_fec_s_init(core_side_unit, 0x0, &fec_structure);
        fec_structure.destination = access_side_sys_port;
        fec_structure.tunnel_gport = cint_vxlan_roo_info.access_eth_rif;
        fec_structure.tunnel_gport2 = cint_vxlan_roo_info.access_arp_id;

        fec_structure.allocation_flags = BCM_L3_WITH_ID;
        fec_structure.fec_id      = cint_vxlan_roo_info.access_fec_id;
        rv = sand_utils_l3_fec_create(core_side_unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC 0x%d only\n",fec_structure.fec_id);
            return rv;
        }
        cint_vxlan_roo_info.access_fec_id = fec_structure.l3eg_fec_id;
    }

    return rv;
}


/**
 * Core side routing entry, used for traffic core---->access direction
 * INPUT:
 *   core_side_unit- core side unit 
 *   access_side_sys_port- access side sys port 
*/
 int
multi_dev_vxlan_roo_forward_entry_configuration(
    int core_side_unit,
    int access_side_unit,
    int core_side_sys_port,
    int access_side_sys_port,
    int into_overlay)
{
    int rv;

    if (into_overlay) {
    /*
     *  Access side routing, used for traffic access---->core direction
     */
        rv = add_host_ipv4(access_side_unit, cint_vxlan_roo_info.route_dip_into_overlay, cint_vxlan_roo_info.access_eth_vrf, cint_vxlan_roo_info.core_native_fec_id, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in add_host_ipv4\n", rv);
            return rv;
        }

    } else {

    /*
     * Core side routing, used for traffic core---->access direction
     */
        rv = add_host_ipv4(core_side_unit, cint_vxlan_roo_info.route_dip_out_of_overlay, cint_vxlan_roo_info.core_native_eth_vrf, cint_vxlan_roo_info.access_fec_id, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in add_host_ipv4\n", rv);
            return rv;
        }

    }

    return rv;

}


/**
 * mpls tunnel termination entry adding
 * INPUT:
 *   core_side_unit    - traffic incoming unit 
*/
 int
multi_dev_vxlan_roo_tunnel_termination_configuration(
    int core_side_unit)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /* Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.type = bcmTunnelTypeVxlan;
    tunnel_term.dip = cint_vxlan_roo_info.tunnel_dip;
    tunnel_term.dip_mask = cint_vxlan_roo_info.tunnel_dip_mask;
    tunnel_term.sip = cint_vxlan_roo_info.tunnel_sip;
    tunnel_term.sip_mask = cint_vxlan_roo_info.tunnel_sip_mask;
    tunnel_term.vrf = cint_vxlan_roo_info.core_overlay_eth_vrf;


    rv = bcm_tunnel_terminator_create(core_side_unit,&tunnel_term);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }
    /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
     bcm_l3_ingress_t ing_intf;
     bcm_l3_ingress_t_init(&ing_intf);
     bcm_if_t intf_id;

     ing_intf.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
     ing_intf.vrf = cint_vxlan_roo_info.tunnel_termination_vrf;

     if (is_device_or_above(core_side_unit, JERICHO2)) {
         /* Convert tunnel's GPort ID to intf ID */
          BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);
          rv = bcm_l3_ingress_create(core_side_unit, ing_intf, intf_id);
          if (rv != BCM_E_NONE)
          {   
              printf("Error, bcm_l3_ingress_create\n");
              return rv;
          }
     }
    
    cint_vxlan_roo_info.vxlan_tunnel_if = tunnel_term.tunnel_id;
    return rv;

}

/**
 * mpls tunnel encapsulation entry adding
 * INPUT:
 *   core_side_unit    - traffic incoming unit 
*/
 int
multi_dev_vxlan_roo_tunnel_encap_configuration(
    int core_side_unit)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    /* Create IP tunnel initiator for encapsulating Vxlan tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.type = bcmTunnelTypeVxlan;
    tunnel_init.dip = cint_vxlan_roo_info.tunnel_dip; /* default: 171.0.0.1 */
    tunnel_init.sip = cint_vxlan_roo_info.tunnel_sip; /* default: 172.0.0.1 */
    tunnel_init.flags = BCM_TUNNEL_WITH_ID;
    tunnel_init.dscp = 10; /* default: 10 */
    if (is_device_or_above(unit, JERICHO2)) {
        tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    } else {
        tunnel_init.dscp_sel = bcmTunnelDscpAssign;  
    }
      
    tunnel_init.l3_intf_id = cint_vxlan_roo_info.core_overlay_arp_id;
    tunnel_init.ttl = 64; /* default: 64 */
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    tunnel_init.encap_access = bcmEncapAccessTunnel2;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init.tunnel_id, cint_vxlan_roo_info.vxlan_tunnel_id);

    rv = bcm_tunnel_initiator_create(core_side_unit,&l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
        return rv;
    }

    cint_vxlan_roo_info.vxlan_tunnel_id = tunnel_init.tunnel_id;
    return rv;

}


/**
 * configuration from access---->core

 * INPUT:
 *   core_side_unit- core side unit 
 *   access_side_unit- access side unit 
 *   core_side_sys_port- core side sys port 
 *   access_side_sys_port- access side sys port
 *   into_overlay - 1: access-->core; 0: core--->access 
*/
 int
 multi_dev_vxlan_roo_into_overlay_configuration(
    int core_side_unit,
    int access_side_unit,
    int core_side_sys_port,
    int access_side_sys_port)
{
    int rv;
    int into_overlay = 1;

    /*
     * 1.Basic ingress L2 configuration(Port default LIF)
     */
    rv = multi_dev_ingress_port_configuration(access_side_unit,cint_vxlan_roo_info.access_port,cint_vxlan_roo_info.access_eth_rif);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ingress_port_configuration\n");
     return rv;
    }

    rv = multi_dev_egress_port_configuration(core_side_unit,cint_vxlan_roo_info.core_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ingress_port_configuration\n");
     return rv;
    }

    /*
     * 2. vxlan vpn creation, put it here just because this routing will reset the nativ-rif property.
     *    So call this first.
     */
    rv = vxlan_roo_basic_vxlan_open_vpn(core_side_unit, cint_vxlan_roo_info.vpn_id, cint_vxlan_roo_info.vni);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_roo_basic_vxlan_open_vpn, vpn=%d, \n", proc_name, cint_vxlan_roo_info.vpn_id);
        return rv;
    }

    int device_is_jericho2;
    rv = is_device_jericho2(core_side_unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2\n", proc_name);
        return rv;
    }

    if (device_is_jericho2) {
        /* configure entry: VNI+AC
           At ESEM for vsi, netork_domain -> VNI, AC information
           create a virtual AC which represent the AC information in ESEM entry.
           The virtual AC save the key to access the ESEM entry. 
           In this case: vsi, network domain
           */
        bcm_vlan_port_t vlan_port; 
        bcm_vlan_port_t_init(&vlan_port); 
        vlan_port.vsi = cint_vxlan_roo_info.vpn_id; /* vsi, part of the key in ESEM lookup */
        vlan_port.match_class_id = 0; /* network domain, part of the key in ESEM lookup */
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI; 
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION; 
        rv = bcm_vlan_port_create(core_side_unit, vlan_port); 
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, bcm_vlan_port_create \n", proc_name);
            return rv;
        }

        /* 
           vlan port translation set
           */
        int vlan_edit_profile = 3; 
        bcm_vlan_port_translation_t vlan_port_translation; 
        bcm_vlan_port_translation_t_init(&vlan_port_translation); 
        vlan_port_translation.flags = BCM_VLAN_ACTION_SET_EGRESS; 
        vlan_port_translation.gport = vlan_port.vlan_port_id; 
        vlan_port_translation.new_outer_vlan = 0;  
        vlan_port_translation.new_inner_vlan = 0; 
        vlan_port_translation.vlan_edit_class_id = vlan_edit_profile; 
        rv = bcm_vlan_port_translation_set(core_side_unit, &vlan_port_translation); 
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, bcm_vlan_port_translation_set \n", proc_name);
            return rv;
        }
        /*
         * Create action ID
         */
        int action_id; 
        rv = bcm_vlan_translate_action_id_create(core_side_unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }
        /*
         * Set translation action
         */
        bcm_vlan_action_set_t action;
        bcm_vlan_action_set_t_init(&action);
        action.outer_tpid = 0x8100;
        action.inner_tpid = 0x8100;
        action.dt_outer = bcmVlanActionMappedAdd;
        action.dt_inner = bcmVlanActionNone;
        rv = bcm_vlan_translate_action_id_set(core_side_unit,
                                              BCM_VLAN_ACTION_SET_EGRESS,
                                              action_id, &action);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_id_set\n");
            return rv;
        }
        /*
         * Set translation action class (map edit_profile & tag_format to action_id)
         */
        int tag_format_untag = 0;
        bcm_vlan_translate_action_class_t action_class;
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = vlan_edit_profile;
        action_class.tag_format_class_id = tag_format_untag;
        action_class.vlan_translation_action_id = action_id;
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_translate_action_class_set(core_side_unit, &action_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }
    }


    /*
     * 3.L3 configuration, this part is shared with out-of-overlay direction
     *    access side In-RIF,
     *    core side native-out-rif,
     *    core side overlay-out-rif
     */


    rv = multi_dev_vxlan_roo_l3_intf_configuration(core_side_unit,access_side_unit,core_side_sys_port,access_side_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_l3_intf_configuration\n");
     return rv;
    }

    /*
     * 4.arp configuration
     *    core side native-arp
     *    core side overlay-arp
     */
    rv = multi_dev_vxlan_roo_arp_configuration(core_side_unit,access_side_unit,core_side_sys_port,access_side_sys_port,into_overlay);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_arp_configuration\n");
     return rv;
    }

    /*
     * 5.Vxlan tunnel encapsulation
     */	

    rv = multi_dev_vxlan_roo_tunnel_encap_configuration(core_side_unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_tunnel_encap_configuration\n");
     return rv;
    }
   

    /*
     * 6. FEC configuration
     *    FEC for Vxlan tunnel
     *    FEC for L3 route result
     */
    rv = multi_dev_vxlan_roo_fec_entry_configuration(-1,access_side_unit,core_side_sys_port,-1,into_overlay);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_fec_entry_configuration\n");
     return rv;
    }

    /*
     * 7. FWD configuration(L3 route)
     */
    rv = multi_dev_vxlan_roo_forward_entry_configuration(-1,access_side_unit,core_side_sys_port,-1,into_overlay);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_forward_entry_configuration\n");
     return rv;
    }

    return rv;
}

/**
 * configuration from core---->access
 * INPUT:
 *   core_side_unit- core side unit 
 *   access_side_unit- access side unit 
 *   core_side_sys_port- core side sys port 
 *   access_side_sys_port- access side sys port
 *   into_overlay - 1: access-->core; 0: core--->access 
*/
 int
 multi_dev_vxlan_roo_out_of_overlay_configuration(
    int core_side_unit,
    int access_side_unit,
    int core_side_sys_port,
    int access_side_sys_port)
{
    int rv;
    int into_overlay = 0;
    int vxlan_port_id;

    /*
     * 1.Basic ingress L2 configuration(Port default LIF)
     */
    rv = multi_dev_ingress_port_configuration(core_side_unit,cint_vxlan_roo_info.core_port,cint_vxlan_roo_info.core_overlay_eth_rif);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ingress_port_configuration\n");
     return rv;
    }

    rv = multi_dev_egress_port_configuration(access_side_unit,cint_vxlan_roo_info.access_port);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
     printf("Error, multi_dev_ingress_port_configuration\n");
     return rv;
    }

    /*
     * 2.arp configuration
     *    access side arp
     */
    rv = multi_dev_vxlan_roo_arp_configuration(core_side_unit,access_side_unit,core_side_sys_port,access_side_sys_port,into_overlay);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_arp_configuration\n");
     return rv;
    }

    /*
     * 3.Vxlan tunnel termination
     */

    rv = multi_dev_vxlan_roo_tunnel_termination_configuration(core_side_unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_tunnel_termination_configuration\n");
     return rv;
    }
   

    /*
     * 4. FEC configuration
     *    FEC for L3 route result
     */
    rv = multi_dev_vxlan_roo_fec_entry_configuration(core_side_unit,-1,-1,access_side_sys_port,into_overlay);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_fec_entry_configuration\n");
     return rv;
    }

    /*
     * 5. FWD configuration(L3 route)
     */
    rv = multi_dev_vxlan_roo_forward_entry_configuration(core_side_unit,-1,-1,access_side_sys_port,into_overlay);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_forward_entry_configuration\n");
     return rv;
    }

 
    /* 
     * 6. Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    rv = vxlan_port_add(core_side_unit, core_side_sys_port, cint_vxlan_roo_info.vxlan_tunnel_if, cint_vxlan_roo_info.vxlan_tunnel_id, &vxlan_port_id);
    if (rv != BCM_E_NONE) 
    {
        printf("%s(): Error, vxlan_port_add \n", proc_name);
        return rv;
    }


   return rv;

}



/**
 * Main entrance for IPv4 UC on multi-device
 * INPUT:
 *   core_side_unit- core side unit 
 *   access_side_unit- access side unit 
 *   core_side_sys_port- core side sys port 
 *   access_side_sys_port- access side sys port
*/
 int
 multi_dev_vxlan_roo_example(
    int core_side_unit,
    int access_side_unit,
    int core_side_sys_port,
    int access_side_sys_port)
{
    int rv;

    rv = multi_dev_vxlan_roo_init(core_side_unit,access_side_unit,core_side_sys_port,access_side_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_init\n");
     return rv;
    }


    rv = multi_dev_vxlan_roo_into_overlay_configuration(core_side_unit,access_side_unit,core_side_sys_port,access_side_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_into_overlay_configuration\n");
     return rv;
    }

    rv = multi_dev_vxlan_roo_out_of_overlay_configuration(core_side_unit,access_side_unit,core_side_sys_port,access_side_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vxlan_roo_out_of_overlay_configuration\n");
     return rv;
    }
    return rv;

}

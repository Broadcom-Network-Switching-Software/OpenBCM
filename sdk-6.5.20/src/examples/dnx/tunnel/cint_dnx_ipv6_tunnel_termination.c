/*
 * cint_dnx_ipv6_tunnel_termination.c
 *
 *  Created on: Nov 1, 2017
 *      Author: rs016217
 *
 *Examples of Jericho2-only tunnel termination
 *
 * Scenarios configured in this CINT:
 *
 *     1. IPv6 tunnel termination with GRE4 or GRE8 additional header for IPv4 forwarding header
 *        ( ipv6_tunnel_termination_generic(unit, provider_port, access_port, is_mp, bcmIPv6TerminationForwardTypeIPv4, bcmTunnelTypeGreAnyIn6) )
 *
 *     2. IPv6 tunnel termination with GRE4 or GRE8 additional header for IPv6 forwarding heaer
 *        ( ipv6_tunnel_termination_generic(unit, provider_port, access_port, is_mp, bcmIPv6TerminationForwardTypeIPv6, bcmTunnelTypeGreAnyIn6) )
 *
 *     3. IPv6 tunnel termination with GRE4 or GRE8 additional header for MPLS forwarding heaer
 *        ( ipv6_tunnel_termination_generic(unit, provider_port, access_port, is_mp, bcmIPv6TerminationForwardTypeMPLS, bcmTunnelTypeGreAnyIn6) )
 *
 *     4. IPv6 tunnel termination with UDP and VxLAN_GPE additional headers for bridging over overlay
 *        ( ipv6_tunnel_termination_generic(unit, provider_port, access_port, is_mp, bcmIPv6TerminationForwardTypeBridge, bcmTunnelTypeVxlan6) )
 *
 *     5. IPv6 tunnel termination with GRE8 additional headers for IPv4 routing with TNI look-up
 *        ( ipv6_tunnel_termination_generic(unit, provider_port, access_port, is_mp, bcmIPv6TerminationForwardTypeIPv4TNI, bcmTunnelTypeGreAnyIn6) )
 *
 *     6. IPv6 tunnel termination with GRE8 additional headers for IPv6 routing with TNI look-up
 *        ( ipv6_tunnel_termination_generic(unit, provider_port, access_port, is_mp, bcmIPv6TerminationForwardTypeIPv6TNI, bcmTunnelTypeGreAnyIn6) )
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/dnx/tunnel/cint_dnx_ipv6_tunnel_termination.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int provider_port = 200;
 * int access_port = 201;
 * rv = ipv6_tunnel_termination(unit, provider_port, access_port, is_mp,
 *    [bcmIPv6TerminationForwardTypeIPv4 | 
 *     bcmIPv6TerminationForwardTypeIPv6 | 
 *     bcmIPv6TerminationForwardTypeMPLS | 
 *     bcmIPv6TerminationForwardTypeBridge | 
 *     bcmIPv6TerminationForwardTypeIPv4TNI | 
 *     bcmIPv6TerminationForwardTypeIPv6TNI])
 * print rv;
 */

/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_dnx_ipv6_tunnel_termination.c Purpose: Various examples for IPv6 Tunnels.
 */

 enum ipv6_term_forward_type_e {
   bcmIPv6TerminationForwardTypeIPv4 = 0,
   bcmIPv6TerminationForwardTypeIPv6 = 1,
   bcmIPv6TerminationForwardTypeMPLS = 2,
   bcmIPv6TerminationForwardTypeBridge = 3,
   bcmIPv6TerminationForwardTypeIPv4TNI = 4,
   bcmIPv6TerminationForwardTypeIPv6TNI = 5
 };

struct cint_ipv6_tunnel_info_s {
    int eth_rif_intf_provider; /* in RIF */
    int eth_rif_intf_access; /* out RIF */
    int tunnel_if; /* RIF, property of the tunnel termination inLif.*/
    bcm_mac_t intf_provider_mac_address; /* mac for in RIF */
    bcm_mac_t intf_access_mac_address; /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac; /* mac for next hop */
    int access_vrf; /* router instance VRF resolved at after tunnel termination*/
    int provider_vrf; /*router instance VRF resolved at VTT1*/
    int fec_id; /* FEC id */
    int arp_id; /* Id for ARP entry */
    bcm_ip_t route_ipv4_dip; /* Route DIP */
    uint32 route_ipv4_dip_mask; /* mask for route DIP */
    int route_mpls_label;         /* MPLS label used for forwarding */

    bcm_ip6_t route_ipv6_dip; /* Route DIP */
    bcm_ip6_t route_ipv6_dip_mask; /* Route DIP mask */

    bcm_ip6_t tunnel_ipv6_dip; /* tunnel DIP */
    bcm_ip6_t tunnel_ipv6_dip_mask; /* tunnel DIP mask*/
    bcm_ip6_t tunnel_ipv6_sip; /* tunnel SIP */
    bcm_ip6_t tunnel_ipv6_sip_mask; /* tunnel SIP mask */
    bcm_tunnel_type_t tunnel_type; /* bcmTunnelType */
    int tunnel_class; /* the intermediate object that connects the two steps of tunnel termination */

    int vxlan_network_group_id; /* ingress and egress VLAN orientation for bridging over VxLAN IPv6 */
    int vpn_id;                 /* VPN ID for bridging over VxLAN IPv6 */
    int vni;                    /* VNI of the VXLAN for bridging over VxLAN IPv6 */

    int tni_vrf; /* when there is TNI look-up in the VTE */
    int esp_term; /* ESP header should also be terminated */
};


cint_ipv6_tunnel_info_s cint_ipv6_tunnel_info = {
/*
 * eth_rif_intf_provider | eth_rif_intf_access | tunnel_if
 */
15, 100, 20,
/*
 * intf_provider_mac_address | intf_access_mac_address | arp_next_hop_mac |
 */
{ 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 }, { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 }, { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d },
/*
 * Access VRF, Provider VRF
 */
5, 1,
/*
 * fec_id
 */
45001,
/*
 * arp_id
 */
0,
/*
 * route_ipv4_dip
 */
0xA0A1A1A2 /* 160.161.161.162 */,
/*
 * route_ipv4_dip_mask
 */
0xfffffff0,
/* route_mpls_label*/
0x5A5A,
/* 
 * route_ipv6_dip
 */
{ 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x66, 0x77 },
/*
 * route_ipv6_dip_mask
 */
{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
/*
 * tunnel DIP
 */
{ 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x55 },
/*
 * tunnel DIP mask
 */
{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
/*
 * tunnel SIP
 */
{ 0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1 },
/*
 * tunnel SIP mask
 */
{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
/*
 * bcmTunnelType
 */
bcmTunnelTypeGreAnyIn6,
/*tunnel_class*/
7,
/*
* vxlan_group_id | VPN_ID | VNI
*/
1, 15, 5000,
/* VRF from TNI look-up */
9,
/* esp_term */
0
};


/*
 * Create the tunnel terminator.
 * is_mp is 1 for MP tunnel and 0 for a P2P tunnel.
 * tunnel_term_id - the ID of the tunnel termination in-lif.
 */
int ipv6_tunnel_terminator_create(int unit, int is_mp, int *tunnel_term_id)
{

    int rv;
    bcm_tunnel_terminator_t tunnel_termination_obj;
    bcm_tunnel_terminator_config_action_t p2p_tunnel_terminator_config_action;
    bcm_tunnel_terminator_config_key_t p2p_tunnel_terminator_config_key;

    /* Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_termination_obj);
    tunnel_termination_obj.dip6 = cint_ipv6_tunnel_info.tunnel_ipv6_dip;
    tunnel_termination_obj.dip6_mask = cint_ipv6_tunnel_info.tunnel_ipv6_dip_mask;
    tunnel_termination_obj.sip6 = cint_ipv6_tunnel_info.tunnel_ipv6_sip;
    tunnel_termination_obj.sip6_mask = cint_ipv6_tunnel_info.tunnel_ipv6_sip_mask;
    tunnel_termination_obj.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    /*
     * Check if the user wants to create P2P tunnel termination
     */
    if(is_mp == 0)
    {
        tunnel_termination_obj.flags |=  BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
    }
    else
    {
        tunnel_termination_obj.priority =  4;
    }

    BCM_GPORT_TUNNEL_ID_SET(tunnel_termination_obj.tunnel_id, 7007);

    /* for jericho 2, the vrf is part of the key */
    tunnel_termination_obj.vrf = cint_ipv6_tunnel_info.provider_vrf;
    tunnel_termination_obj.type = cint_ipv6_tunnel_info.tunnel_type;

    if(cint_ip_tunnel_basic_info.esp_term)
    {
        tunnel_termination_obj.type = bcmTunnelTypeEsp6;
    } else {
        tunnel_termination_obj.type = cint_ipv6_tunnel_info.tunnel_type;
    }

    p2p_tunnel_terminator_config_action.tunnel_class = cint_ipv6_tunnel_info.tunnel_class;
    tunnel_termination_obj.tunnel_class = p2p_tunnel_terminator_config_action.tunnel_class;

    p2p_tunnel_terminator_config_key.dip6 = tunnel_termination_obj.dip6;
    p2p_tunnel_terminator_config_key.dip6_mask = tunnel_termination_obj.dip6_mask;
    p2p_tunnel_terminator_config_key.flags = tunnel_termination_obj.flags;
    p2p_tunnel_terminator_config_key.type = tunnel_termination_obj.type;
    p2p_tunnel_terminator_config_key.vrf = tunnel_termination_obj.vrf;

    /*
     * Check if the user wants to create P2P tunnel
     */
    if(is_mp == 0)
    {
        /* Flags parameter is not used in the function. */
        bcm_tunnel_terminator_config_add(unit, 0, p2p_tunnel_terminator_config_key, p2p_tunnel_terminator_config_action);
    }

    /* Call the actual API method for tunnel termination creation */
    rv = bcm_tunnel_terminator_create(unit, &tunnel_termination_obj);
    if (rv != BCM_E_NONE)
    {
        printf("Error: ipv6_tunnel_terminator_create failed to create tunnel termination object -> bcm_tunnel_terminator_create() failed. \n");
        return rv;
    }
    *tunnel_term_id = tunnel_termination_obj.tunnel_id;

    /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
    bcm_l3_ingress_t ingress_in_lif;
    bcm_l3_ingress_t_init(&ingress_in_lif);
    bcm_if_t in_lif_id;

    ingress_in_lif.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
    ingress_in_lif.vrf = cint_ipv6_tunnel_info.access_vrf;

    if(cint_ipv6_tunnel_info.esp_term)
    {
        ingress_in_lif.flags |= BCM_L3_INGRESS_ROUTE_ENABLE_UNKNOWN;
    }

    /* Convert tunnel's GPort ID to intf ID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(in_lif_id, tunnel_termination_obj.tunnel_id);

    rv = bcm_l3_ingress_create(unit, ingress_in_lif, in_lif_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error: ipv6_tunnel_terminator_create failed -> bcm_l3_ingress_create() failed.\n");
        return rv;
    }
    return rv;
}

/*
 * Main function for IPv6 tunnel termination.
 * provider_port - the source port of entry
 * access port - destination port for the traffic
 * is_mp is 1 for MP tunnel and 0 for a P2P tunnel.
 * forwarding_type - the type of forwarding (or bridging):
 * bcm_tunnel_type - the type of the tunnel, passed to ipv6_tunnel_terminator_create.
 */
int ipv6_tunnel_termination_generic(int unit, int provider_port, int access_port, int is_mp, int forwarding_type, int bcm_tunnel_type)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    int mpls_tunnel_id;
    int term_tunnel_id;

    proc_name = "ipv6_tunnel_termination_generic";

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ipv6_tunnel_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    if (forwarding_type == bcmIPv6TerminationForwardTypeBridge)
    {
       proc_name = "vxlan_tunnel_termination_tcam";

       /*
        * Build L2 VPN
        */
       rv = vxlan_open_vpn(unit, cint_ipv6_tunnel_info.vpn_id, cint_ipv6_tunnel_info.vni);
       if (rv != BCM_E_NONE)
       {
           printf("%s(): Error, vxlan_open_vpn, vpn=%d, \n", proc_name, cint_ipv6_tunnel_info.vpn_id);
           return rv;
       }
    }
    rv = ip_tunnel_termination_open_route_interfaces(unit, provider_port, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): ERROR : ip_tunnel_termination_open_route_interfaces failed to execute! \n", proc_name);
        return rv;
    }

    /** Select the bcmTunnelTypes and create the tunnel terminator */
    cint_ipv6_tunnel_info.tunnel_type = bcm_tunnel_type;


    /** Create the tunnel termination lookup and the tunnel termination inlif */
    rv = ipv6_tunnel_terminator_create(unit, is_mp, &term_tunnel_id);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): ERROR : ipv6_tunnel_terminator_create failed to execute! \n", proc_name);
        return rv;
    }
    /* Check if the method is invoked for IPv4 or IPv6 forwarding header*/
    if(forwarding_type == bcmIPv6TerminationForwardTypeIPv4 || forwarding_type == bcmIPv6TerminationForwardTypeIPv6)
    {
         
        /*
         * Create ARP entry and set its properties
         */

        rv = l3__egress_only_encap__create(unit, 0, &(cint_ipv6_tunnel_info.arp_id),
                cint_ipv6_tunnel_info.arp_next_hop_mac,
                cint_ipv6_tunnel_info.eth_rif_intf_access);

        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error: create egress object ARP only.\n", proc_name);
            return rv;
        }

        /*
         * Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF
         */
        rv = l3__egress_only_fec__create(unit,
                cint_ipv6_tunnel_info.fec_id,
                cint_ipv6_tunnel_info.eth_rif_intf_access,
                cint_ipv6_tunnel_info.arp_id,
                access_port,
                0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error: create egress object FEC only\n", proc_name);
            return rv;
        }

        if(forwarding_type == bcmIPv6TerminationForwardTypeIPv4)
        {
            /*
             * Add Route of IPv4 header entry
             */
            rv = add_route_ipv4(unit, cint_ipv6_tunnel_info.route_ipv4_dip,
                    cint_ipv6_tunnel_info.route_ipv4_dip_mask,
                    cint_ipv6_tunnel_info.access_vrf,
                    cint_ipv6_tunnel_info.fec_id);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error: add_route_ipv4() failed.\n", proc_name);
                return rv;
            }
        }
        else
        {
            /*
             * Add Route of IPv6 header entry
             */
            rv = add_route_ipv6(unit, cint_ipv6_tunnel_info.route_ipv6_dip,
                    cint_ipv6_tunnel_info.route_ipv6_dip_mask,
                    cint_ipv6_tunnel_info.access_vrf,
                    cint_ipv6_tunnel_info.fec_id);
            if(rv != BCM_E_NONE){
                printf("%s(): Error: add_route_ipv6() failed.\n", proc_name);
                return rv;
            }
        }
    }
    /*
     * MPLS forwarding
     */
    else if (forwarding_type == bcmIPv6TerminationForwardTypeMPLS)
    {

        /* 
         * Add MPLS route to IP-Tunnel.
         */

        rv = ip_tunnel_termination_create_mpls_swap_tunnel(unit, &mpls_tunnel_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, ip_tunnel_termination_create_mpls_swap_tunnel\n", proc_name);
            return rv;
        }

        /*
         * Create ARP entry and set its properties
         */
        rv = l3__egress_only_encap__create(unit, 0, &(cint_ipv6_tunnel_info.arp_id),
                cint_ipv6_tunnel_info.arp_next_hop_mac,
                cint_ipv6_tunnel_info.eth_rif_intf_access);

        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error: create egress object ARP only.\n", proc_name);
            return rv;
        }
        /*
         * Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF
         */
        rv = l3__egress_only_fec__create(unit, cint_ipv6_tunnel_info.fec_id, mpls_tunnel_id, 0, access_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
            return rv;
        }
        rv = ip_tunnel_termination_ilm_switch_tunnel_create(unit, cint_ipv6_tunnel_info.route_mpls_label,
                cint_ipv6_tunnel_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in ip_tunnel_termination_ilm_switch_tunnel_create \n");
            return rv;
        }
    } 

    /*
    * Bridging over overlay
    */
    else if (forwarding_type == bcmIPv6TerminationForwardTypeBridge)
    {

       bcm_if_t encap_tunnel_intf;
       bcm_gport_t vxlan_port_id;

       proc_name = "vxlan_tunnel_termination_tcam";

       /*
        * Create ARP entry and set its properties
        */
       rv = l3__egress_only_encap__create(unit, 0, &(cint_ipv6_tunnel_info.arp_id),
                                          cint_ipv6_tunnel_info.intf_access_mac_address,
                                          cint_ipv6_tunnel_info.eth_rif_intf_access);
       if (rv != BCM_E_NONE)
       {
           printf("%s(): Error, create egress object ARP only\n", proc_name);
           return rv;
       }

       /*
        * Create FEC and configure its: Outlif (ARP), Destination port
        */
       rv = l3__egress_only_fec__create(unit, cint_ipv6_tunnel_info.fec_id, encap_tunnel_intf,
                                        0, access_port, 0);
       if (rv != BCM_E_NONE)
       {
           printf("%s(): Error, create egress object FEC only\n", proc_name);
           return rv;
       }

       /*
        * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
        */
       rv = vxlan_port_add(unit, provider_port, term_tunnel_id, &vxlan_port_id);
       if (rv != BCM_E_NONE)
       {
           printf("%s(): Error, vxlan_port_add \n", proc_name);
       }

       /*
        * Create an MACT entry for VXLAN tunnel, mapped to the VPN
        */
       rv = mact_entry_add(unit, access_port);
       if (rv != BCM_E_NONE)
       {
           printf("%s(): Error, mact_entry_add\n", proc_name);
           return rv;
       }

       /* 
        * IPv4 forwarding with TNI look-up
        */
    } else if (forwarding_type == bcmIPv6TerminationForwardTypeTNI) {

        /*
         * Create ARP entry and set its properties
         */

        rv = l3__egress_only_encap__create(unit, 0, &(cint_ipv6_tunnel_info.arp_id),
                cint_ipv6_tunnel_info.arp_next_hop_mac,
                cint_ipv6_tunnel_info.eth_rif_intf_access);

        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error: create egress object ARP only.\n", proc_name);
            return rv;
        }

        /*
         * Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF
         */
        rv = l3__egress_only_fec__create(unit,
                cint_ipv6_tunnel_info.fec_id,
                cint_ipv6_tunnel_info.eth_rif_intf_access,
                cint_ipv6_tunnel_info.arp_id,
                access_port,
                0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error: create egress object FEC only\n", proc_name);
            return rv;
        }

        /*
         * Add Route of IPv4 header entry
         */
        rv = add_route_ipv4(unit, cint_ipv6_tunnel_info.route_ipv4_dip,
                cint_ipv6_tunnel_info.route_ipv4_dip_mask,
                cint_ipv6_tunnel_info.tni_vrf,
                cint_ipv6_tunnel_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error: add_route_ipv4() failed.\n", proc_name);
            return rv;
        }
                
    }
    return rv;
}

/* 
 * Configure the bridging 
 */
int
mact_entry_add(
    int unit,
    int vxlan_port_id)
{
    int rv;
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr,  cint_ipv6_tunnel_info.arp_next_hop_mac, cint_ipv6_tunnel_info.vpn_id);
    l2addr.port = vxlan_port_id;
    l2addr.mac = cint_ipv6_tunnel_info.arp_next_hop_mac;
    l2addr.vid = cint_ipv6_tunnel_info.vpn_id;
    l2addr.l2mc_group = cint_ipv6_tunnel_info.vpn_id;
    l2addr.flags = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_l2_addr_add \n", proc_name);
        return rv;
    }
    return rv;
}

/*
 * Connext the VxLAN port to the termination LIF
 */
int
vxlan_port_add(
    int unit,
    int core_port,
    int tunnel_term_id,
    bcm_gport_t * vxlan_port_id)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    vxlan_port_add_s vxlan_port_add;
    vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn = cint_ipv6_tunnel_info.vpn_id;
    vxlan_port_add.in_port = core_port;
    vxlan_port_add.in_tunnel = tunnel_term_id;
    BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, cint_ipv6_tunnel_info.fec_id);
    vxlan_port_add.flags = 0;
    vxlan_port_add.network_group_id = cint_ipv6_tunnel_info.vxlan_network_group_id;

    
    rv = vxlan__vxlan_port_add(unit, vxlan_port_add);
    *vxlan_port_id = vxlan_port_add.vxlan_port_id;

    return BCM_E_NONE;
}

/* 
 * Main function for configuring the VXLAN VPN
 */
int
vxlan_open_vpn(
    int unit,
    int vpn_id,
    int vni)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "vxlan_open_vpn";
    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn_id;
    vpn_config.broadcast_group = vpn_id;
    vpn_config.unknown_unicast_group = vpn_id;
    vpn_config.unknown_multicast_group = vpn_id;
    vpn_config.vnid = vni;
    rv = bcm_vxlan_vpn_create(unit, &vpn_config);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_vxlan_vpn_create \n", proc_name);
        return rv;
    }
    return rv;
}

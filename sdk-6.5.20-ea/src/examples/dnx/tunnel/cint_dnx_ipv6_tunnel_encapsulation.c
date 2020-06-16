
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_dnx_ipv6_tunnel_encapsulation.c Purpose: Various examples for IPv6 Tunnels.
 */

/*
 * Examples of Jericho2-only tunnel termination
 *
 * Scenarios configured in this CINT:
 *
 *     1. IPv6 tunnel encapsulation with GRE4 additional header for IPv4 forwarding header
 *        ( ipv6_tunnel_encapsulation(unit, access_port, provider_port, bcmIPv6EncapsulationForwardTypeIPv4) )
 *
 *     2. IPv6 tunnel encapsulation with GRE4 additional header for IPv6 forwarding heaer
 *        ( ipv6_tunnel_encapsulation(unit, access_port, provider_port, bcmIPv6EncapsulationForwardTypeIPv6) )
 *
 *     3. IPv6 tunnel encapsulation with GRE4 additional header for MPLS forwarding heaer
 *        ( ipv6_tunnel_encapsulation(unit, access_port, provider_port, bcmIPv6EncapsulationForwardTypeMPLS) )
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/dnx/tunnel/cint_dnx_ipv6_tunnel_encapsulation.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int provider_port = 200;
 * int access_port = 201;
 * rv = ipv4_tunnel_termination_with_mc_rpf(unit, provider_port, access_port_1, access_port_2, vid);
 * rv = ipv6_tunnel_encapsulation(unit, access_port, provider_port, [bcmIPv6EncapsulationForwardTypeIPv4 | bcmIPv6EncapsulationForwardTypeIPv6 | bcmIPv6EncapsulationForwardTypeMPLS])
 * print rv;
 */

int cint_dnx_ipv6_tunnel_encapsulation_vxlan_domained_needed = TRUE;

enum ipv6_encap_forward_type_e
{
    bcmIPv6EncapsulationForwardTypeIPv4 = 0,
    bcmIPv6EncapsulationForwardTypeIPv6 = 1,
    bcmIPv6EncapsulationForwardTypeMPLS = 2
};

enum ipv6_encap_gre_type_e
{
    bcmIPv6EncapsulationGRE4 = 0,
    bcmIPv6EncapsulationGRE8 = 1
};

struct cint_ipv6_tunnel_info_s
{
    int eth_rif_intf_provider;            /* out RIF */
    int eth_rif_intf_access;              /* in RIF */
    bcm_mac_t intf_access_mac_address;    /* mac for in RIF */
    bcm_mac_t tunnel_next_hop_mac;        /* mac for next hop */
    int access_vrf;                       /* VRF, resolved as Tunnel-InLif property*/
    int tunnel_fec_id;                    /* FEC id */
    int tunnel_arp_id;                    /* Id for ARP entry */
    bcm_ip_t     route_ipv4_dip;          /* Route DIP */
    uint32       route_ipv4_dip_mask;     /* mask for route DIP */
    bcm_ip6_t    route_ipv6_dip;          /* Route IPv6 DIP */
    bcm_ip6_t    route_ipv6_dip_mask;     /* Route IPv6 DIP Mask*/
    int          route_mpls_label;        /* Route MPLS label */
    bcm_ip6_t    tunnel_ipv6_dip;         /* tunnel IPv6 DIP */
    bcm_ip6_t    tunnel_ipv6_sip;         /* tunnel IPv6 SIP */
    bcm_tunnel_type_t tunnel_type;        /* tunnel type */
    int provider_vlan;                    /* VID assigned to the outgoing packet */
    int tunnel_ttl;                       /* Ip tunnel header TTL */
    int tunnel_dscp;                      /* Ip tunnel header differentiated services code point */
    int gre4_lb_key_enable;               /* Enable LB-Key on GRE IP tunnel */
    int svtag_enable;                     /* enable SVTAG lookup */
};

cint_ipv6_tunnel_info_s cint_ipv6_tunnel_info =
{
        /*
         * eth_rif_intf_provider | eth_rif_intf_access
         */
        100,                    15,
        /*
         * intf_access_mac_address             | tunnel_next_hop_mac
         */
        {0x00, 0x0c, 0x00, 0x02, 0x00, 0x56}, {0x00, 0x02, 0x00, 0x00, 0xcd, 0x1d},
        /*
         * Access VRF
         */
        1,
        /*
         * tunnel_fec_id
         */
        40961,
        /*
         * tunnel_arp_id
         */
        5050,
        /*
         * route_ipv4_dip
         */
        0x7fffff03 /* 127.255.255.03 */,
        /*
         * route_ipv4_dip_mask
         */
        0xfffffff0,
        /* route_ipv6_dip */
        {0x74, 0x50, 0xA5, 0x08, 0xEE, 0x90, 0x39, 0x1F, 0x69, 0x57, 0xBF, 0x33, 0x19, 0x44, 0xA2, 0xC0},
        /* route_ipv6_dip_mask */
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        /*
         * route_mpls_label
         */
        0xA5A5,
        /*
         * tunnel IPv6 DIP
         */
        { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x55 },
        /*
         * tunnel IPv6 SIP
         */
        { 0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1 },
        /*
         * tunnel type
         */
        bcmTunnelTypeGreAnyIn6,
        /*
         * provider vlan
         */
        100,
        /*
         * tunnel_ttl
         */
        64,
        /*
         * tunnel_dscp
         */
        10,
        /*
         * gre4_lb_key_enable
         */
        0,
        /*
         * svtag_enable
         */
        0
};

int
mpls_swap_tunnel_create(
    int unit,
    int mpls_fwd_label,
    bcm_if_t tunnel_intf,
    bcm_if_t * mpls_intf_id)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;

    bcm_mpls_egress_label_t_init(&label_array[0]);

    /*
     * Pushed or Swapping label.
     * The label will be pushed for IP forwarding context and swapped for
     * MPLS forwarding context.
     */
    label_array[0].label = mpls_fwd_label;
    /*
     * Set the next pointer of this entry to be the IP tunnel. This configuration is new compared to
     * Jericho, where the arp pointer used to be connected to the EEDB entry via
     * bcm_l3_egress_create (with egress flag indication).
     */
    label_array[0].l3_intf_id = tunnel_intf;

    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label_array[0].encap_access = bcmEncapAccessTunnel1;

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *mpls_intf_id = label_array[0].tunnel_id;

    return rv;
}

int mpls_switch_tunnel_create(
        int unit,
        bcm_mpls_label_t label,
        int fec_id)
{

    int rv;
    bcm_mpls_tunnel_switch_t entry;

    /*
     * No flag is needed to indicate the action, since the information for the swap/php action is
     * included in the EEDB, pointed from the ILM.
     */

    bcm_mpls_tunnel_switch_t_init(&entry);

    /** incoming label */
    entry.label = label;
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    /** FEC entry pointing to port */
    BCM_GPORT_FORWARD_PORT_SET(entry.port, fec_id);

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    return rv;
}


int ipv6_tunnel_encapsulation_ipv6_header_only(int unit, int access_port, int provider_port, int forward_type)
{
    cint_ipv6_tunnel_info.tunnel_type = bcmTunnelTypeIpAnyIn6;
    ipv6_tunnel_encapsulation(unit, access_port, provider_port, forward_type, bcmIPv6EncapsulationGRE4);
}

int ipv6_tunnel_initiator_create(
    int unit,
    bcm_if_t arp_id,
    bcm_if_t *tunnel_inlif,
    bcm_gport_t *tunnel_id,
    ipv6_encap_gre_type_e gre_type
    )
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);

    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip6            = cint_ipv6_tunnel_info.tunnel_ipv6_dip;
    tunnel_init.sip6            = cint_ipv6_tunnel_info.tunnel_ipv6_sip;
    tunnel_init.flags           = 0;
    tunnel_init.dscp            = cint_ipv6_tunnel_info.tunnel_dscp;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.type            = cint_ipv6_tunnel_info.tunnel_type;
    tunnel_init.ttl             = cint_ipv6_tunnel_info.tunnel_ttl;
    tunnel_init.encap_access    = bcmEncapAccessTunnel2;
    tunnel_init.l3_intf_id      = arp_id;
    if(gre_type == bcmIPv6EncapsulationGRE8)
    {
        tunnel_init.flags           = BCM_TUNNEL_INIT_GRE_KEY_USE_LB;
    }

    if(cint_ipv6_tunnel_info.svtag_enable) {
        tunnel_init.flags |= BCM_TUNNEL_INIT_SVTAG_ENABLE;
    }

    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_create \n");
    }

    *tunnel_inlif = l3_intf.l3a_intf_id;

    *tunnel_id = tunnel_init.tunnel_id;
    return rv;
}

int ipv6_tunnel_encapsulation(int unit, int access_port, int provider_port, int forward_type, int gre_type)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    bcm_if_t tunnel_outlif;
    bcm_gport_t tunnel_id;
    l3_ingress_intf ingr_inlif;
    l3_ingress_intf_init(&ingr_inlif);

    proc_name = "ipv6_tunnel_encapsulation";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: is_device_jericho2 device_is_jericho2 \n", proc_name);
        return rv;
    }
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ipv6_tunnel_info.tunnel_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, access_port, cint_ipv6_tunnel_info.eth_rif_intf_access);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: in_port_intf_set failed to execute! \n", proc_name);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: out_port_set failed to execute! \n", proc_name);
        return rv;
    }

    /*
     * Create routing interface for the routing's IP.
     * We use it as ingress ETH-RIF to perform ETH termination (my-mac procedure), to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_ipv6_tunnel_info.eth_rif_intf_access, cint_ipv6_tunnel_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: intf_eth_rif_create failed to execute! \n", proc_name);
        return rv;
    }

    /*
     * Create egress routing interface used for routing after the tunnel encapsulation.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_ipv6_tunnel_info.eth_rif_intf_provider, cint_ipv6_tunnel_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create failed to execute! \n", proc_name);
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     */
    ingr_inlif.intf_id = cint_ipv6_tunnel_info.eth_rif_intf_access;
    ingr_inlif.vrf = cint_ipv6_tunnel_info.access_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_inlif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set eth_rif_intf_access\n", proc_name);
    }

    /*
     * Create ARP entry and set its properties.
     */
    rv = l3__egress_only_encap__create(unit, 0,&(cint_ipv6_tunnel_info.tunnel_arp_id), cint_ipv6_tunnel_info.tunnel_next_hop_mac,
            cint_ipv6_tunnel_info.provider_vlan);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel initiator
     * and set next-lif-pointer to the ARP entry
     */
    rv = ipv6_tunnel_initiator_create(unit, cint_ipv6_tunnel_info.tunnel_arp_id, &tunnel_outlif, &tunnel_id, gre_type);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ipv6_tunnel_initiator_create failed to execute! \n", proc_name);
        return rv;
    }

    if(forward_type == bcmIPv6EncapsulationForwardTypeIPv4 || forward_type == bcmIPv6EncapsulationForwardTypeIPv6)
    {
        /*
         * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
         */
        rv = l3__egress_only_fec__create(unit, cint_ipv6_tunnel_info.tunnel_fec_id, tunnel_outlif, 0, provider_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create failed to execute! \n", proc_name);
        }

        /*
         * Add Host entry
         */
        int fec_id;
        BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ipv6_tunnel_info.tunnel_fec_id);
        if(forward_type == bcmIPv6EncapsulationForwardTypeIPv4)
        {
            rv = add_host_ipv4(unit, cint_ipv6_tunnel_info.route_ipv4_dip, cint_ipv6_tunnel_info.access_vrf, fec_id, 0,0);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, add_host_ipv4 failed to execute! \n", proc_name);
                return rv;
            }
        } else {
            rv = add_host_ipv6(unit, cint_ipv6_tunnel_info.route_ipv6_dip, cint_ipv6_tunnel_info.access_vrf, fec_id);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, add_route_ipv6 failed to execute! \n", proc_name);
                return rv;
            }
        }

    }
    else if(forward_type == bcmIPv6EncapsulationForwardTypeMPLS)
    {
        /*
         * Add MPLS route to IP-Tunnel.
         */
        bcm_if_t mpls_intf_id;
        rv = mpls_swap_tunnel_create(unit, cint_ipv6_tunnel_info.route_mpls_label, tunnel_outlif, &mpls_intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, mpls_swap_tunnel_create failed to execute.\n", proc_name);
            return rv;
        }

        /*
         * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
         */
        rv = l3__egress_only_fec__create(unit, cint_ipv6_tunnel_info.tunnel_fec_id, mpls_intf_id, 0, provider_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error: l3__egress_only_fec__create failed to execute.\n", proc_name);
            return rv;
        }
        /*
         * Create mpls LSF for port pointed by FEC
         */
        rv = mpls_switch_tunnel_create(unit, cint_ipv6_tunnel_info.route_mpls_label, cint_ipv6_tunnel_info.tunnel_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, mpls_switch_tunnel_create failed to execute! \n", proc_name);
            return rv;
        }
    }

    if(cint_dnx_ipv6_tunnel_encapsulation_vxlan_domained_needed)
    {
        rv = vxlan_network_domain_management_l3_egress(unit);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, vxlan_network_domain_management_l3_egress failed to execute! \n", proc_name);
            return rv;
        }
    }
    return rv;

}

int
vxlan_network_domain_management_l3_egress(
    int unit)
{
    int rv;
    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VNI->VRF
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING;
    config.vrf              = cint_ipv6_tunnel_info.access_vrf;
    config.vni              = 5000;
    config.network_domain   = 0;

    rv = bcm_vxlan_network_domain_config_add(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }
    return rv;
}


/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_dnx_ipv6_tunnel_encapsulation.c Purpose: Various examples for IPv6 Tunnels.
 */

/*
 * Examples of Jericho2-only tunnel encapsulation
 *
 * Scenarios configured in this CINT:
 *
 *     1. IPv6 tunnel encapsulation with GRE4 additional header for IPv4 forwarding header
 *        ( ipv6_tunnel_encapsulation(unit, access_port, provider_port, IPv6EncapsulationForwardTypeIPv4, IPv6EncapsulationGRE4) )
 *
 *     2. IPv6 tunnel encapsulation with GRE4 additional header for IPv6 forwarding heaer
 *        ( ipv6_tunnel_encapsulation(unit, access_port, provider_port, IPv6EncapsulationForwardTypeIPv6, IPv6EncapsulationGRE4) )
 *
 *     3. IPv6 tunnel encapsulation with GRE4 additional header for MPLS forwarding heaer
 *        ( ipv6_tunnel_encapsulation(unit, access_port, provider_port, IPv6EncapsulationForwardTypeMPLS, IPv6EncapsulationGRE4) )
 *
 * Configuration:
 *
 * Test Scenario - start
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/tunnel/cint_dnx_ipv6_tunnel_encapsulation.c
  cint
  int unit = 0;
  int rv = 0;
  int provider_port = 200;
  int access_port = 201;
  rv = ipv6_tunnel_encapsulation(unit, access_port, provider_port, IPv6EncapsulationForwardTypeIPv4, IPv6EncapsulationGRE8);
  print rv;
 * Test Scenario - end
 *
 */

int cint_dnx_ipv6_tunnel_encapsulation_vxlan_domained_needed = TRUE;

enum ipv6_encap_forward_type_e
{
    IPv6EncapsulationForwardTypeIPv4 = 0,
    IPv6EncapsulationForwardTypeIPv6 = 1,
    IPv6EncapsulationForwardTypeMPLS = 2
};

enum ipv6_encap_gre_type_e
{
    IPv6EncapsulationGRE4 = 0,
    IPv6EncapsulationGRE8 = 1
};

struct cint_ipv6_tunnel_info_s
{
    int eth_rif_intf_provider;            /* out RIF */
    int eth_rif_intf_access;              /* in RIF */
    bcm_mac_t intf_access_mac_address;    /* mac for in RIF */
    bcm_mac_t tunnel_next_hop_mac;        /* mac for next hop */
    int access_vrf;                       /* VRF, resolved as Tunnel-InLif property*/
    int access_vrf_to_vni;                /* VRF, used for VRF->VNI mapping, set to different from access_vrf to test vrf->vni miss*/
    int tunnel_fec_id;                    /* FEC id */
    int tunnel_arp_id;                    /* Id for ARP entry */
    int tunnel_intf;                      /* Tunnel Interface */
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
    int flags;                            /* Tunnel initiator flags */
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
         * Access VRF | Access VRF to VNI
         */
        1, 1,
        /*
         * tunnel_fec_id
         */
        40961,
        /*
         * tunnel_arp_id
         */
        5050,
        /*
         * tunnel_intf
         */
        0,
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
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00},
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
        0,
        /*
         * tunnel initiator flags
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

    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array), "");

    *mpls_intf_id = label_array[0].tunnel_id;

    return BCM_E_NONE;
}

int mpls_switch_tunnel_create(
        int unit,
        bcm_mpls_label_t label,
        int fec_id)
{

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

    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_switch_create(unit, &entry), "");

    return BCM_E_NONE;
}


int ipv6_tunnel_encapsulation_ipv6_header_only(int unit, int access_port, int provider_port, int forward_type)
{
    cint_ipv6_tunnel_info.tunnel_type = bcmTunnelTypeIpAnyIn6;
    ipv6_tunnel_encapsulation(unit, access_port, provider_port, forward_type, IPv6EncapsulationGRE4);
}

int ipv6_tunnel_initiator_create(
    int unit,
    bcm_if_t arp_id,
    bcm_if_t *tunnel_inlif,
    bcm_gport_t *tunnel_id,
    ipv6_encap_gre_type_e gre_type
    )
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);

    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip6            = cint_ipv6_tunnel_info.tunnel_ipv6_dip;
    tunnel_init.sip6            = cint_ipv6_tunnel_info.tunnel_ipv6_sip;
    tunnel_init.flags           = cint_ipv6_tunnel_info.flags;
    tunnel_init.dscp            = cint_ipv6_tunnel_info.tunnel_dscp;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.type            = cint_ipv6_tunnel_info.tunnel_type;
    tunnel_init.ttl             = cint_ipv6_tunnel_info.tunnel_ttl;
    tunnel_init.encap_access    = bcmEncapAccessTunnel2;
    tunnel_init.l3_intf_id      = arp_id;
    if(gre_type == IPv6EncapsulationGRE8)
    {
        tunnel_init.flags           = BCM_TUNNEL_INIT_GRE_KEY_USE_LB;
    }

    if(cint_ipv6_tunnel_info.svtag_enable) {
        tunnel_init.flags |= BCM_TUNNEL_INIT_SVTAG_ENABLE;
    }

    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init), "");

    *tunnel_inlif = l3_intf.l3a_intf_id;

    *tunnel_id = tunnel_init.tunnel_id;
    return BCM_E_NONE;
}

int ipv6_tunnel_encapsulation(int unit, int access_port, int provider_port, int forward_type, int gre_type)
{
    bcm_if_t tunnel_outlif;
    bcm_gport_t tunnel_id;
    l3_ingress_intf ingr_inlif;
    l3_ingress_intf_init(&ingr_inlif);


    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_ipv6_tunnel_info.tunnel_fec_id), "");

    /*
     * Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, access_port, cint_ipv6_tunnel_info.eth_rif_intf_access),
        "failed to execute!");

    /*
     * Set Out-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, provider_port), "failed to execute!");

    /*
     * Create routing interface for the routing's IP.
     * We use it as ingress ETH-RIF to perform ETH termination (my-mac procedure), to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_ipv6_tunnel_info.eth_rif_intf_access, cint_ipv6_tunnel_info.intf_access_mac_address),
        "failed to execute!");

    /*
     * Create egress routing interface used for routing after the tunnel encapsulation.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_ipv6_tunnel_info.eth_rif_intf_provider, cint_ipv6_tunnel_info.intf_access_mac_address),
        "failed to execute!");

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     */
    ingr_inlif.intf_id = cint_ipv6_tunnel_info.eth_rif_intf_access;
    ingr_inlif.vrf = cint_ipv6_tunnel_info.access_vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_inlif), "eth_rif_intf_access");

    /*
     * Create ARP entry and set its properties.
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0,&(cint_ipv6_tunnel_info.tunnel_arp_id), cint_ipv6_tunnel_info.tunnel_next_hop_mac,
            cint_ipv6_tunnel_info.provider_vlan), "create egress object ARP only");

    /*
     * Create the tunnel initiator
     * and set next-lif-pointer to the ARP entry
     */
    BCM_IF_ERROR_RETURN_MSG(ipv6_tunnel_initiator_create(unit, cint_ipv6_tunnel_info.tunnel_arp_id, &tunnel_outlif, &tunnel_id, gre_type),
        "failed to execute!");
    cint_ipv6_tunnel_info.tunnel_intf = tunnel_outlif;

    if(forward_type == IPv6EncapsulationForwardTypeIPv4 || forward_type == IPv6EncapsulationForwardTypeIPv6)
    {
        /*
         * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
         */
        int fec_flags2 = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_ipv6_tunnel_info.tunnel_fec_id, tunnel_outlif, 0, provider_port, 0, fec_flags2),
            "failed to execute!");

        /*
         * Add Host entry
         */
        int fec_id;
        BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ipv6_tunnel_info.tunnel_fec_id);
        if(forward_type == IPv6EncapsulationForwardTypeIPv4)
        {
            BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_ipv6_tunnel_info.route_ipv4_dip, cint_ipv6_tunnel_info.access_vrf, fec_id, 0,0),
                "failed to execute!");
        } else {
            BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, cint_ipv6_tunnel_info.route_ipv6_dip, cint_ipv6_tunnel_info.access_vrf, fec_id),
                "failed to execute!");
        }

    }
    else if(forward_type == IPv6EncapsulationForwardTypeMPLS)
    {
        /*
         * Add MPLS route to IP-Tunnel.
         */
        bcm_if_t mpls_intf_id;
        BCM_IF_ERROR_RETURN_MSG(mpls_swap_tunnel_create(unit, cint_ipv6_tunnel_info.route_mpls_label, tunnel_outlif, &mpls_intf_id),
            "failed to execute.");

        /*
         * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
         */
        int fec_flags2 = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_ipv6_tunnel_info.tunnel_fec_id, mpls_intf_id, 0, provider_port, 0, fec_flags2),
            "failed to execute.");
        /*
         * Create mpls LSF for port pointed by FEC
         */
        BCM_IF_ERROR_RETURN_MSG(mpls_switch_tunnel_create(unit, cint_ipv6_tunnel_info.route_mpls_label, cint_ipv6_tunnel_info.tunnel_fec_id),
            "failed to execute!");
    }

    if(cint_dnx_ipv6_tunnel_encapsulation_vxlan_domained_needed)
    {
        BCM_IF_ERROR_RETURN_MSG(vxlan_network_domain_management_l3_egress(unit), "failed to execute!");
    }
    return BCM_E_NONE;

}

int
vxlan_network_domain_management_l3_egress(
    int unit)
{
    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VRF -> VNI
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING;
    config.vrf              = cint_ipv6_tunnel_info.access_vrf_to_vni;
    config.vni              = 5000;
    config.network_domain   = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &config), "");
    return BCM_E_NONE;
}



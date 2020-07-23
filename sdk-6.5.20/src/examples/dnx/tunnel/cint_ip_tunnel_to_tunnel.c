/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_ip_tunnel_to_tunnel.c Purpose: Various scenarios of IP tunnel to tunnel configurations.
 */

/*
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/dnx/cint_ip_tunnel_to_tunnel.c
 * cint ../../../../src/examples/sand/cint_ecmp_basic.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int in_provider_port = 200;
 * int out_provider_port = 201;
 * rv = ip_tunnel_to_tunnel_gre_mpls_fwd(unit, in_provider_port, out_provider_port);
 * print rv;
 */
struct cint_ip_tunnel_basic_info_s
{
    int eth_rif_intf_in_provider;            /* in RIF */
    int eth_rif_intf_out_provider;           /* out RIF */
    int tunnel_if;                           /* RIF, property of the tunnel termination inLif. */
    bcm_mac_t intf_in_provider_mac_address;  /* mac for in RIF */
    bcm_mac_t intf_out_provider_mac_address; /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac;              /* mac for next hop */
    int out_provider_vrf;                    /* router instance VRF resolved at VTT1 */
    int in_provider_vrf;                     /* VRF after termination, resolved as Tunnel-InLif property */
    int fec_id;                              /* FEC id */
    int arp_id;                              /* Id for ARP entry */
    bcm_ip_t route_dip;                      /* Route DIP */
    uint32 mask;                             /* mask for route DIP */
    bcm_ip_t in_tunnel_dip;                  /* tunnel DIP */
    bcm_ip_t in_tunnel_dip_mask;             /* tunnel DIP mask */
    bcm_ip_t in_tunnel_sip;                  /* tunnel SIP */
    bcm_ip_t in_tunnel_sip_mask;             /* tunnel SIP mask */
    int in_tunnel_type;                      /* bcmTunnelType ingress tunnel */
    int gre_lb_key_enable;                   /* Enable LB-Key on GRE IP tunnel */
    int out_tunnel_type;                     /* bcmTunnelType egress tunnel */
    bcm_ip_t out_tunnel_dip;                 /* tunnel DIP */
    bcm_ip_t out_tunnel_dip_mask;            /* tunnel DIP mask */
    bcm_ip_t out_tunnel_sip;                 /* tunnel SIP */
    bcm_ip_t out_tunnel_sip_mask;            /* tunnel SIP mask */
    int out_provider_vlan;                   /* VID assigned to the outgoing packet */
    int out_tunnel_ttl;                      /* Ip tunnel header TTL */
    int out_tunnel_dscp;                     /* Ip tunnel header differentiated services code point */
    int mpls_fwd_label;                      /* MPLS label used for forwarding */
    int ecmp_id;                             /* ECMP ID */
    int ecmp_group_members_num;              /* ECMP group members number */
};

cint_ip_tunnel_basic_info_s cint_ip_tunnel_basic_info = {
    /*
     * eth_rif_intf_in_provider | eth_rif_intf_out_provider | tunnel_if
     */
    15, 100, 20,
    /*
     * intf_in_provider_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /*
     * intf_out_provider_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01},
    /*
     * arp_next_hop_mac
     */
    {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * out_provider_vrf VRF, in_provider_vrf VRF
     */
    5, 1,
    /*
     * fec id
     */
    40961,
    /*
     * arp_id
     */
    0,
    /*
     * route_dip
     */
    0xA0A1A1A2 /* 160.161.161.162 */ ,
    /*
     * mask
     */
    0xfffffff0,
    /*
     * in_tunnel_dip
     */
    0xAB000001, /* 171.0.0.1 */
    /*
     * in_tunnel_dip_mask
     */
    0xffffffff,
    /*
     * in_tunnel_sip
     */
    0xAC000001, /* 172.0.0.1 */
    /*
     * in_tunnel_sip_mask
     */
    0xffffffff,
    /*
     * in_tunnel_type
     */
    bcmTunnelTypeGreAnyIn4,
    /*
     * gre_lb_key_enable
     */
    0,
    /*
     * out_tunnel_type
     */
    bcmTunnelTypeVxlanGpe,
    /*
     * out_tunnel_dip
     */
    0xB5000001, /* 181.0.0.1 */
    /*
     * out_tunnel_dip_mask
     */
    0xffffffff,
    /*
     * out_tunnel_sip
     */
    0xB6000001, /* 182.0.0.1 */
    /*
     * out_tunnel_sip_mask
     */
    0xffffffff,
    /*
     * out_provider_vlan
     */
    100,
    /*
     * out tunnel_ttl
     */
    64,
    /*
     * out tunnel_dscp
     */
    10,
    /*
     * mpls_fwd_label
     */
    1456,
    /*
     * ecmp_id
     */
    50,
    /*
     * ecmp_group_members_num
     */
    1
};

/*  Scenario configured in this cint:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) IP Tunnel to Tunnel with MPLS forwarding
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  A GRE4oIPv4 tunnel header is being terminated and Vxlan tunnel is encapsulated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##########################################################################################################
 *  Sending packet from  in provider port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-++-
 *   |    |      DA      | SA           || IPv4 tunnel SIP || IPv4 tunnel DIP ||  GRE  ||     MPLS Label    ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||    172.0.0.1    ||    171.0.0.1    ||       ||        1456       ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-++-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on out provider port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   |    |      DA      | SA           || IPv4 tunnel SIP || IPv4 tunnel DIP ||  UDP dest port:  ||         VXLAN-GPE         ||     MPLS Label    ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||    182.0.0.1    ||    181.0.0.1    ||     0x5555       ||  VNI flag=1, VNI=0x1388   ||        1456       ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ####################################################################################################################################################
 *
 * Function description:
 *
 * Main function for ip tunnel to tunnel configuration.
 * The function calls the basic ip_tunnel_to_tunnel_open_route_interfaces,
 * then creates the tunnel terminator - bcmTunnelTypeGreAnyIn4 and initiator with bcmTunnelTypeVxlan type.
 * Adds MPLS as a forwarding entry.
 * Inputs:
 * unit - relevant unit;
 * in_provider_port - incoming port;
 * out_provider_port - outgoing port;
 */
int
ip_tunnel_to_tunnel_gre_termination_encapsulation_mpls_fwd(
    int unit,
    int in_provider_port,
    int out_provider_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    bcm_if_t tunnel_intf;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_if_t mpls_intf_id;

    proc_name = "ip_tunnel_to_tunnel_gre_termination_encapsulation_mpls_fwd";

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    ip_tunnel_to_tunnel_open_route_interfaces(unit, in_provider_port, out_provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_to_tunnel_open_route_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    cint_ip_tunnel_basic_info.in_tunnel_dip_mask = 0xFFF00000;
    rv = tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tunnel_terminator_create\n");
        return rv;
    }

    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.arp_id, &tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_initiator_create\n", proc_name);
        return rv;
    }

    /*
     * Add MPLS route to IP-Tunnel.
     */
    rv = mpls_swap_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label, tunnel_intf,
                                 &mpls_intf_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_swap_tunnel_create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port
     */
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, mpls_intf_id,
                                     0, out_provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
        return rv;
    }

    /*
     * Create an ECMP group
     */
    rv = l3__ecmp_create(unit, cint_ip_tunnel_basic_info.ecmp_id,
                         cint_ip_tunnel_basic_info.ecmp_group_members_num,
                         cint_ip_tunnel_basic_info.fec_id, BCM_L3_WITH_ID,
                         BCM_L3_ECMP_DYNAMIC_MODE_DISABLED, &ecmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3__ecmp_create\n");
        return rv;
    }

    /*
     * Create mpls LSF for port pointed by FEC
     */
    rv = mpls_switch_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label,
                                   cint_ip_tunnel_basic_info.ecmp_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_switch_tunnel_create\n", proc_name);
        return rv;
    }

    rv = vxlan_network_domain_management_l3_egress(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function vxlan_network_domain_management_l3_egress, \n");
        return rv;
    }

    return rv;
}


int
ip_tunnel_to_tunnel_vxlan_gpe_tt_gre4_encap_ipv4_fwd (
    int unit,
    int in_provider_port,
    int out_provider_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    bcm_if_t out_tunnel_intf;
    cint_ip_tunnel_basic_info.in_tunnel_type    = bcmTunnelTypeVxlanGpe;
    cint_ip_tunnel_basic_info.out_tunnel_type   = bcmTunnelTypeGreAnyIn4;

    proc_name = "ip_tunnel_to_tunnel_vxlan_gpe_tt_gre4_encap_ipv4_fwd";

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    rv = ip_tunnel_to_tunnel_open_route_interfaces(unit, in_provider_port, out_provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_to_tunnel_open_route_interfaces\n", proc_name);
        return rv;
    }


     cint_ip_tunnel_basic_info.in_tunnel_sip_mask = 0x00000000; 

    /*
     * Create VXLAN-GPE IP tunnel terminator
     */
    rv = tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_terminator_create\n", proc_name);
        return rv;
    }

    /*
     * Create GRE IP tunnel
     */
    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.arp_id, &out_tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_initiator_create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, out_tunnel_intf,
                                     0, out_provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
        return rv;
    }

    /*
     * Add Host entry
     */
    int fec_id;
    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ip_tunnel_basic_info.fec_id);
    rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_dip,
                       cint_ip_tunnel_basic_info.out_provider_vrf, fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_host_ipv4\n");
        return rv;
    }

    rv = vxlan_network_domain_management_l3_ingress(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vxlan_network_domain_management_l3_ingress\n");
        return rv;
    }
    return rv;
}

/*
 * Main function for VXLAN-GPE IP tunnel termination to GRE4 tunnel encapsulation with MPLS
 * forwarding on Jericho 2
 * The function configures VXLANoUDPoIPv4 tunnel termination and GRE4oIPv4 tunnel encapsulation
 * on MPLSoVXLANoUDPoIPv4oETH packet.
 * Inputs: unit              - relevant unit;
 *         in_provider_port  - incoming port;
 *         out_provider_port - outgoing port;
 */
int
ip_tunnel_to_tunnel_vxlan_gpe_tt_gre4_encap_mpls_fwd (
    int unit,
    int in_provider_port,
    int out_provider_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    bcm_if_t out_tunnel_intf;
    bcm_if_t mpls_intf_id;
    bcm_l3_egress_ecmp_t ecmp;
    cint_ip_tunnel_basic_info.in_tunnel_type    = bcmTunnelTypeVxlanGpe;
    cint_ip_tunnel_basic_info.out_tunnel_type   = bcmTunnelTypeGreAnyIn4;

    proc_name = "ip_tunnel_to_tunnel_vxlan_gpe_tt_gre4_encap_mpls_fwd";

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    rv = ip_tunnel_to_tunnel_open_route_interfaces(unit, in_provider_port, out_provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_to_tunnel_open_route_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Create VXLAN-GPE IP tunnel terminator
     */
    rv = tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_terminator_create\n", proc_name);
        return rv;
    }

    /*
     * Create GRE IP tunnel
     */
    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.arp_id, &out_tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_initiator_create\n", proc_name);
        return rv;
    }

    /*
     * Add MPLS route to IP-Tunnel.
     */
    rv = mpls_swap_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label, out_tunnel_intf,
                                 &mpls_intf_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_swap_tunnel_create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, mpls_intf_id,
                                     0, out_provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
        return rv;
    }

    /*
     * Create an ECMP group
     */
    rv = l3__ecmp_create(unit, cint_ip_tunnel_basic_info.ecmp_id,
                         cint_ip_tunnel_basic_info.ecmp_group_members_num,
                         cint_ip_tunnel_basic_info.fec_id, BCM_L3_WITH_ID,
                         BCM_L3_ECMP_DYNAMIC_MODE_DISABLED, &ecmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3__ecmp_create\n");
        return rv;
    }

    /*
     * Create mpls LSF for port pointed by FEC
     */
    rv = mpls_switch_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label,
                                   cint_ip_tunnel_basic_info.ecmp_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_switch_tunnel_create\n", proc_name);
        return rv;
    }

    rv = vxlan_network_domain_management_l3_ingress(unit);
    return rv;
}

int
vxlan_network_domain_management_l3_ingress(
    int unit)
{
    int rv;
    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VNI->VRF
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING;
    config.vrf              = cint_ip_tunnel_basic_info.out_provider_vrf;
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
/*
 * Main function for UDP IP tunnel termination to GRE8 with LB-Key tunnel encapsulation with IPv4
 * forwarding on Jericho 2
 * The function configures UDPoIPv4 tunnel termination and GRE8oIPv4 tunnel encapsulation
 * on IPv4oUDPoIPv4oETH packet.
 * Inputs: unit              - relevant unit;
 *         in_provider_port  - incoming port;
 *         out_provider_port - outgoing port;
 */
int
ip_tunnel_to_tunnel_udp_tt_gre8_encap_ipv4_fwd (
    int unit,
    int in_provider_port,
    int out_provider_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    bcm_if_t out_tunnel_intf;
    cint_ip_tunnel_basic_info.in_tunnel_type    = bcmTunnelTypeUdp;
    cint_ip_tunnel_basic_info.out_tunnel_type   = bcmTunnelTypeGreAnyIn4;
    cint_ip_tunnel_basic_info.gre_lb_key_enable = 1;

    proc_name = "ip_tunnel_to_tunnel_udp_tt_gre8_encap_ipv4_fwd";
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    rv = ip_tunnel_to_tunnel_open_route_interfaces(unit, in_provider_port, out_provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_to_tunnel_open_route_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Create VXLAN-GPE IP tunnel terminator
     */
     /** Mask Tunnel SIP - TT Lookup is in VTT3 SEM */
    cint_ip_tunnel_basic_info.in_tunnel_sip_mask=0x00000000;
    rv = tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_terminator_create\n", proc_name);
        return rv;
    }

    /*
     * Create GRE IP tunnel
     */
    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.arp_id, &out_tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_initiator_create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, out_tunnel_intf,
                                     0, out_provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
        return rv;
    }

    /*
     * Add Host entry
     */
    int fec_id;
    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ip_tunnel_basic_info.fec_id);
    rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_dip,
                       cint_ip_tunnel_basic_info.out_provider_vrf, fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_host_ipv4\n");
        return rv;
    }

    return rv;
}

/*
 * Main function for UDP IP tunnel termination to GRE8 with LB-Key tunnel encapsulation with MPLS
 * forwarding on Jericho 2
 * The function configures UDPoIPv4 tunnel termination and GRE8oIPv4 tunnel encapsulation
 * on MPLSoUDPoIPv4oETH packet.
 * Inputs: unit              - relevant unit;
 *         in_provider_port  - incoming port;
 *         out_provider_port - outgoing port;
 */
int
ip_tunnel_to_tunnel_udp_tt_gre8_encap_mpls_fwd (
    int unit,
    int in_provider_port,
    int out_provider_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    bcm_if_t out_tunnel_intf;
    bcm_if_t mpls_intf_id;
    bcm_l3_egress_ecmp_t ecmp;
    cint_ip_tunnel_basic_info.in_tunnel_type    = bcmTunnelTypeUdp;
    cint_ip_tunnel_basic_info.out_tunnel_type   = bcmTunnelTypeGreAnyIn4;
    cint_ip_tunnel_basic_info.gre_lb_key_enable = 1;
    proc_name = "ip_tunnel_to_tunnel_udp_tt_gre8_encap_mpls_fwd";

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    rv = ip_tunnel_to_tunnel_open_route_interfaces(unit, in_provider_port, out_provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_to_tunnel_open_route_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Create VXLAN-GPE IP tunnel terminator
     */
    cint_ip_tunnel_basic_info.in_tunnel_dip_mask = 0xFFFF0000;
    rv = tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_terminator_create\n", proc_name);
        return rv;
    }

    /*
     * Create GRE IP tunnel
     */
    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.arp_id, &out_tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_initiator_create\n", proc_name);
        return rv;
    }

    /*
     * Add MPLS route to IP-Tunnel.
     */
    rv = mpls_swap_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label, out_tunnel_intf,
                                 &mpls_intf_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_swap_tunnel_create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, mpls_intf_id,
                                     0, out_provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
        return rv;
    }

    /*
     * Create an ECMP group
     */
    rv = l3__ecmp_create(unit, cint_ip_tunnel_basic_info.ecmp_id,
                         cint_ip_tunnel_basic_info.ecmp_group_members_num,
                         cint_ip_tunnel_basic_info.fec_id, BCM_L3_WITH_ID,
                         BCM_L3_ECMP_DYNAMIC_MODE_DISABLED, &ecmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3__ecmp_create\n");
        return rv;
    }

    /*
     * Create mpls LSF for port pointed by FEC
     */
    rv = mpls_switch_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label,
                                   cint_ip_tunnel_basic_info.ecmp_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_switch_tunnel_create\n", proc_name);
        return rv;
    }

    return rv;
}


/*
 * Main function for ip tunnel termination scenarios in Jericho 2
 * The function ip_tunnel_termination_open_route_interfaces implements the basic scenario for ingress and egress interfaces
 * Inputs:
 * unit - relevant unit;
 * in_provider_port - incoming port;
 * out_provider_port - outgoing port;
 */
int
ip_tunnel_to_tunnel_open_route_interfaces(
    int unit,
    int in_provider_port,
    int out_provider_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "ip_tunnel_to_tunnel_open_route_interfaces";

    /*
     * Configure ingress interfaces
     */
    rv = ip_tunnel_termination_open_route_interfaces_provider(unit, in_provider_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_termination_open_route_interfaces_provider\n", proc_name);
        return rv;
    }

    /*
     * Configure egress interfaces
     */
    rv = ip_tunnel_encapsulation_open_route_interfaces_provider(unit, out_provider_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_encapsulation_open_route_interfaces_provider\n", proc_name);
        return rv;
    }

    /*
     * Create ARP entry and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_ip_tunnel_basic_info.arp_id),
                                       cint_ip_tunnel_basic_info.arp_next_hop_mac,
                                       cint_ip_tunnel_basic_info.out_provider_vlan);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Configure Ingress interfaces
 * Inputs: in_provider_port - incoming port;
 *         *proc_name - main function name;
 */
int
ip_tunnel_termination_open_route_interfaces_provider(
    int unit,
    int in_provider_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_provider_port, cint_ip_tunnel_basic_info.eth_rif_intf_in_provider);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set eth_rif_intf_in_provider\n", proc_name);
        return rv;
    }

    /*
     * Create routing interface for the tunnel's IP.
     * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure),
     * to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_in_provider,
                             cint_ip_tunnel_basic_info.intf_in_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_provider\n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_ip_tunnel_basic_info.eth_rif_intf_in_provider;
    ingr_itf.vrf     = cint_ip_tunnel_basic_info.in_provider_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set eth_rif_intf_in_provider\n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Configure Egress interfaces
 * Inputs: out_provider_port - outgoing port;
 *         *proc_name - main function name;
 */
int
ip_tunnel_encapsulation_open_route_interfaces_provider(
    int unit,
    int out_provider_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, out_provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set out_provider_port\n", proc_name);
        return rv;
    }
    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_out_provider,
                             cint_ip_tunnel_basic_info.intf_out_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_out_provider\n", proc_name);
        return rv;
    }
    return rv;
}

int
tunnel_terminator_create(
    int unit)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /*
     * Create IP tunnel terminator for SIP,DIP, VRF lookup 
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip        = cint_ip_tunnel_basic_info.in_tunnel_dip;
    tunnel_term.dip_mask   = cint_ip_tunnel_basic_info.in_tunnel_dip_mask;
    tunnel_term.sip        = cint_ip_tunnel_basic_info.in_tunnel_sip;
    tunnel_term.sip_mask   = cint_ip_tunnel_basic_info.in_tunnel_sip_mask;
    tunnel_term.vrf        = cint_ip_tunnel_basic_info.in_provider_vrf;
    tunnel_term.type       = cint_ip_tunnel_basic_info.in_tunnel_type;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    /*
     * In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API
     */
    bcm_l3_ingress_t ing_intf;
    bcm_l3_ingress_t_init(&ing_intf);
    ing_intf.flags = BCM_L3_INGRESS_WITH_ID;        /* must, as we update exist RIF */
    ing_intf.vrf   = cint_ip_tunnel_basic_info.out_provider_vrf;

    /*
     * Convert tunnel's GPort ID to intf ID 
     */
    bcm_if_t intf_id;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);
    rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    return rv;
}

/* Create IP tunnel
 * Parameters: arp_itf - tunnel ARP interface Id
 *             *tunnel_intf - tunnel interface
 */
int
tunnel_initiator_create(
    int unit,
    bcm_if_t arp_itf,
    bcm_if_t * tunnel_intf)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_info;
    bcm_l3_intf_t l3_intf;

    /*
     * Create IP tunnel initiator for encapsulating tunnel header
     */
    bcm_tunnel_initiator_t_init(&tunnel_info);
    tunnel_info.dip                     = cint_ip_tunnel_basic_info.out_tunnel_dip;
    tunnel_info.sip                     = cint_ip_tunnel_basic_info.out_tunnel_sip;
    if (cint_ip_tunnel_basic_info.gre_lb_key_enable) {
        tunnel_info.flags  = BCM_TUNNEL_INIT_GRE_KEY_USE_LB;
    } else {
        tunnel_info.flags  = 0;
    }
    tunnel_info.dscp                    = cint_ip_tunnel_basic_info.out_tunnel_dscp;
    tunnel_info.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_info.type                    = cint_ip_tunnel_basic_info.out_tunnel_type;
    tunnel_info.ttl                     = cint_ip_tunnel_basic_info.out_tunnel_ttl;
    tunnel_info.l3_intf_id              = arp_itf;
    tunnel_info.encap_access            = bcmEncapAccessTunnel4;
    tunnel_info.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;

    bcm_l3_intf_t_init(&l3_intf);
    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }

    *tunnel_intf = l3_intf.l3a_intf_id;
    return rv;
}

/*
 * Add MPLS route to IP-Tunnel.
 */
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
    label_array[0].flags     |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *mpls_intf_id = label_array[0].tunnel_id;

    return rv;
}

/*
 * Create mpls LSF for port pointed by FEC
 */
int
mpls_switch_tunnel_create(
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
    entry.label  = label;
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.flags  = BCM_MPLS_SWITCH_TTL_DECREMENT;

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
    config.vrf              = cint_ip_tunnel_basic_info.out_provider_vrf;
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

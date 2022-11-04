/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_ip_tunnel_to_tunnel.c Purpose: Various scenarios of IP tunnel to tunnel configurations.
 */

/*
 *
 * Configuration:
 *
 * Test Scenario - start
 * cint;
 * cint_reset();
 * exit;
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
  cint ../../../../src/examples/sand/cint_ip_route_basic.c
  cint ../../../../src/examples/dnx/cint_ip_tunnel_to_tunnel.c
  cint ../../../../src/examples/sand/cint_ecmp_basic.c
  cint
  int unit = 0;
  int rv = 0;
  int in_provider_port = 200;
  int out_provider_port = 201;
  rv = ip_tunnel_to_tunnel_gre_mpls_fwd(unit, in_provider_port, out_provider_port);
  print rv;
 * Test Scenario - end
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
    int out_udp_src_port;                    /* udp src port for IP tunnel encapsulation */
    int out_udp_dst_port;                    /* udp dst port for IP tunnel encapsulation */
    int mpls_fwd_label;                      /* MPLS label used for forwarding */
    int ecmp_id;                             /* ECMP ID */
    int ecmp_group_members_num;              /* ECMP group members number */
    uint8 is_udp_tunnel_dst_nat;             /* indicate if udp tunnel src nat */
    bcm_gport_t tunnel_id;                   /* tunnel gport of tunnel terminator */

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
     * out tunnel udp src port
     */
    0,
    /*
     * out tunnel udp dst port
     */
    0,
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
    1,
    /*
     * indicate if udp tunnel dst nat
     */
    0
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
    bcm_if_t tunnel_intf;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_if_t mpls_intf_id;


    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_ip_tunnel_basic_info.fec_id), "");
    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    BCM_IF_ERROR_RETURN_MSG(ip_tunnel_to_tunnel_open_route_interfaces(unit, in_provider_port, out_provider_port), "");

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    cint_ip_tunnel_basic_info.in_tunnel_dip_mask = 0xFFF00000;
    BCM_IF_ERROR_RETURN_MSG(tunnel_terminator_create(unit), "");

    BCM_IF_ERROR_RETURN_MSG(tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.arp_id, &tunnel_intf), "");

    /*
     * Add MPLS route to IP-Tunnel.
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_swap_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label, tunnel_intf,
                                 &mpls_intf_id), "");

    /*
     * Create FEC and configure its: Outlif, Destination port
     */
    int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, mpls_intf_id,
                                     0, out_provider_port, 0, fwd_flag), "");

    /*
     * Create an ECMP group
     */
    BCM_IF_ERROR_RETURN_MSG(l3__ecmp_create(unit, cint_ip_tunnel_basic_info.ecmp_id,
                         cint_ip_tunnel_basic_info.ecmp_group_members_num,
                         cint_ip_tunnel_basic_info.fec_id, BCM_L3_WITH_ID,
                         BCM_L3_ECMP_DYNAMIC_MODE_DISABLED, &ecmp, 0), "");

    /*
     * Create mpls LSF for port pointed by FEC
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_switch_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label,
                                   cint_ip_tunnel_basic_info.ecmp_id), "");

    BCM_IF_ERROR_RETURN_MSG(vxlan_network_domain_management_l3_egress(unit), "");

    return BCM_E_NONE;
}

/*
 * Main function for VXLAN-GPE tunnel Termination to GRE4 tunnel encapsulation
 * Test name: AT_Dnx_Cint_l3_ip_tunnel_to_tunnel_vxlan_gpe_to_gre4_ipv4_fwd
 *
 * Scenario configured in this cint:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) VXLAN to GRE Tunnel with Ipv4 forwarding
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  A VXLAN-GPEoUDPoIPv4 tunnel header is being terminated and GRE4oIpv4 tunnel is encapsulated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ###################################################################################################################################################################
 *  Sending packet from  in provider port == 200 :   ----------->
  *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-++-+-+-+-++
 *   |    |      DA      | SA           || IPv4 tunnel SIP || IPv4 tunnel DIP ||  UDP dest port:  ||         VXLAN-GPE         || IPv4 native SIP || IPv4 native DIP ||
 *   |    |0c:00:02:00:00|00:07:00:01:00||    172.0.0.1    ||    171.0.0.1    ||     0x5555       ||  VNI flag=1, VNI=0x1388   ||   192.128.1.1   || 160.161.161.162 ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on out provider port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-++-+-+-+-++
 *   |    |      DA      | SA           || IPv4 tunnel SIP || IPv4 tunnel DIP ||  GRE  || IPv4 native SIP || IPv4 native DIP ||
 *   |    |00:00:00:cd:1d|0c:00:02:00:01||    182.0.0.1    ||    181.0.0.1    ||       ||   192.128.1.1   || 160.161.161.162 ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-++-+-+-+-++
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ############################################################################################################################
 *
 * Function description:
 *
 * Main function for ip tunnel to tunnel configuration.
 * The function calls the basic ip_tunnel_to_tunnel_open_route_interfaces,
 * then creates the tunnel terminator - bcmTunnelTypeVxlan and initiator with bcmTunnelTypeGreAnyIn4 type.
 * Adds Ipv4.as forwarding entry.
 * Inputs:
 * unit              - relevant unit;
 * in_provider_port  - incoming port;
 * out_provider_port - outgoing port;
 */
int
ip_tunnel_to_tunnel_vxlan_gpe_tt_gre4_encap_ipv4_fwd (
    int unit,
    int in_provider_port,
    int out_provider_port)
{
    bcm_if_t out_tunnel_intf;
    cint_ip_tunnel_basic_info.in_tunnel_type    = bcmTunnelTypeVxlanGpe;
    cint_ip_tunnel_basic_info.out_tunnel_type   = bcmTunnelTypeGreAnyIn4;


    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_ip_tunnel_basic_info.fec_id), "");
    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    BCM_IF_ERROR_RETURN_MSG(ip_tunnel_to_tunnel_open_route_interfaces(unit, in_provider_port, out_provider_port), "");


     cint_ip_tunnel_basic_info.in_tunnel_sip_mask = 0x00000000; 

    /*
     * Create VXLAN-GPE IP tunnel terminator
     */
    BCM_IF_ERROR_RETURN_MSG(tunnel_terminator_create(unit), "");

    /*
     * Create GRE IP tunnel
     */
    BCM_IF_ERROR_RETURN_MSG(tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.arp_id, &out_tunnel_intf), "");

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, out_tunnel_intf,
                                     0, out_provider_port, 0, fwd_flag), "");

    /*
     * Add Host entry
     */
    int fec_id;
    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ip_tunnel_basic_info.fec_id);
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_dip,
                       cint_ip_tunnel_basic_info.out_provider_vrf, fec_id, 0, 0), "");

    BCM_IF_ERROR_RETURN_MSG(vxlan_network_domain_management_l3_ingress(unit), "");
    return BCM_E_NONE;
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
    bcm_if_t out_tunnel_intf;
    bcm_if_t mpls_intf_id;
    bcm_l3_egress_ecmp_t ecmp;
    cint_ip_tunnel_basic_info.in_tunnel_type    = bcmTunnelTypeVxlanGpe;
    cint_ip_tunnel_basic_info.out_tunnel_type   = bcmTunnelTypeGreAnyIn4;


    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_ip_tunnel_basic_info.fec_id), "");
    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    BCM_IF_ERROR_RETURN_MSG(ip_tunnel_to_tunnel_open_route_interfaces(unit, in_provider_port, out_provider_port), "");

    /*
     * Create VXLAN-GPE IP tunnel terminator
     */
    BCM_IF_ERROR_RETURN_MSG(tunnel_terminator_create(unit), "");

    /*
     * Create GRE IP tunnel
     */
    BCM_IF_ERROR_RETURN_MSG(tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.arp_id, &out_tunnel_intf), "");

    /*
     * Add MPLS route to IP-Tunnel.
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_swap_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label, out_tunnel_intf,
                                 &mpls_intf_id), "");

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, mpls_intf_id,
                                     0, out_provider_port, 0, fwd_flag), "");

    /*
     * Create an ECMP group
     */
    BCM_IF_ERROR_RETURN_MSG(l3__ecmp_create(unit, cint_ip_tunnel_basic_info.ecmp_id,
                         cint_ip_tunnel_basic_info.ecmp_group_members_num,
                         cint_ip_tunnel_basic_info.fec_id, BCM_L3_WITH_ID,
                         BCM_L3_ECMP_DYNAMIC_MODE_DISABLED, &ecmp, 0), "");

    /*
     * Create mpls LSF for port pointed by FEC
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_switch_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label,
                                   cint_ip_tunnel_basic_info.ecmp_id), "");

    BCM_IF_ERROR_RETURN_MSG(vxlan_network_domain_management_l3_ingress(unit), "");
    return BCM_E_NONE;
}

int
vxlan_network_domain_management_l3_ingress(
    int unit)
{
    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VNI->VRF
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING;
    config.vrf              = cint_ip_tunnel_basic_info.out_provider_vrf;
    config.vni              = 5000;
    config.network_domain   = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &config), "");
    return BCM_E_NONE;
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
    bcm_if_t out_tunnel_intf;
    cint_ip_tunnel_basic_info.in_tunnel_type    = bcmTunnelTypeUdp;
    cint_ip_tunnel_basic_info.out_tunnel_type   = bcmTunnelTypeGreAnyIn4;
    cint_ip_tunnel_basic_info.gre_lb_key_enable = 1;

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_ip_tunnel_basic_info.fec_id), "");

    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    BCM_IF_ERROR_RETURN_MSG(ip_tunnel_to_tunnel_open_route_interfaces(unit, in_provider_port, out_provider_port), "");

    /*
     * Create VXLAN-GPE IP tunnel terminator
     */
     /** Mask Tunnel SIP - TT Lookup is in VTT3 SEM */
    cint_ip_tunnel_basic_info.in_tunnel_sip_mask=0x00000000;
    BCM_IF_ERROR_RETURN_MSG(tunnel_terminator_create(unit), "");

    /*
     * Create GRE IP tunnel
     */
    BCM_IF_ERROR_RETURN_MSG(tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.arp_id, &out_tunnel_intf), "");

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, out_tunnel_intf,
                                     0, out_provider_port, 0, fwd_flag), "");

    /*
     * Add Host entry
     */
    int fec_id;
    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ip_tunnel_basic_info.fec_id);
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_dip,
                       cint_ip_tunnel_basic_info.out_provider_vrf, fec_id, 0, 0), "");

    return BCM_E_NONE;
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
    bcm_if_t out_tunnel_intf;
    bcm_if_t mpls_intf_id;
    bcm_l3_egress_ecmp_t ecmp;
    cint_ip_tunnel_basic_info.in_tunnel_type    = bcmTunnelTypeUdp;
    cint_ip_tunnel_basic_info.out_tunnel_type   = bcmTunnelTypeGreAnyIn4;
    cint_ip_tunnel_basic_info.gre_lb_key_enable = 1;

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_ip_tunnel_basic_info.fec_id), "");
    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    BCM_IF_ERROR_RETURN_MSG(ip_tunnel_to_tunnel_open_route_interfaces(unit, in_provider_port, out_provider_port), "");

    /*
     * Create VXLAN-GPE IP tunnel terminator
     */
    cint_ip_tunnel_basic_info.in_tunnel_dip_mask = 0xFFFF0000;
    BCM_IF_ERROR_RETURN_MSG(tunnel_terminator_create(unit), "");

    /*
     * Create GRE IP tunnel
     */
    BCM_IF_ERROR_RETURN_MSG(tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.arp_id, &out_tunnel_intf), "");

    /*
     * Add MPLS route to IP-Tunnel.
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_swap_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label, out_tunnel_intf,
                                 &mpls_intf_id), "");

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, mpls_intf_id,
                                     0, out_provider_port, 0, fwd_flag), "");

    /*
     * Create an ECMP group
     */
    BCM_IF_ERROR_RETURN_MSG(l3__ecmp_create(unit, cint_ip_tunnel_basic_info.ecmp_id,
                         cint_ip_tunnel_basic_info.ecmp_group_members_num,
                         cint_ip_tunnel_basic_info.fec_id, BCM_L3_WITH_ID,
                         BCM_L3_ECMP_DYNAMIC_MODE_DISABLED, &ecmp, 0), "");

    /*
     * Create mpls LSF for port pointed by FEC
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_switch_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label,
                                   cint_ip_tunnel_basic_info.ecmp_id), "");

    return BCM_E_NONE;
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
ip_tunnel_to_tunnel_nat_identification_nat_udp_encap_nat_fwd (
    int unit,
    int in_provider_port,
    int out_provider_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    bcm_if_t out_tunnel_intf;

    cint_ip_tunnel_basic_info.is_udp_tunnel_dst_nat = TRUE;
    cint_ip_tunnel_basic_info.in_tunnel_type    = bcmTunnelTypeIpAnyIn4;
    cint_ip_tunnel_basic_info.out_tunnel_type   = bcmTunnelTypeUdp;
    cint_ip_tunnel_basic_info.out_udp_src_port = 2426;
    cint_ip_tunnel_basic_info.out_udp_dst_port = 2426;
    cint_ip_tunnel_basic_info.route_dip = cint_ip_tunnel_basic_info.in_tunnel_dip;

    proc_name = "ip_tunnel_to_tunnel_nat_identification_nat_udp_encap_nat_fwd";
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_ip_tunnel_basic_info.fec_id);
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
    int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, out_tunnel_intf,
                                     0, out_provider_port, 0, fwd_flag);
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
    /* the vrf is not updated by any tunnel. So we're using the vrf identified by inRif*/
    rv = add_route_ipv4_encap_dest(unit, cint_ip_tunnel_basic_info.route_dip, 0xffffffff,
            cint_ip_tunnel_basic_info.in_provider_vrf, fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_host_ipv4\n");
        return rv;
    }


    return rv;
}


int
ip_tunnel_add_tunneled_nat(
    int unit,
    int ipv4_inlif,
    int udp_port,
    int exp_udp_dport,
    int nat_ip,
    int run_With_kbp)
{
    uint32 entry_handle_id;
    uint32 u_udp_port = udp_port;
    uint32 u_exp_udp_dport = exp_udp_dport;
    uint32 u_nat_ip = nat_ip;
    uint32 u_ipv4_inlif=BCM_GPORT_TUNNEL_ID_GET(ipv4_inlif);

    if (run_With_kbp)
    {
        BCM_IF_ERROR_RETURN_MSG(dnx_dbal_entry_handle_take(unit, "KBP_TUNNELED_NAT", &entry_handle_id), "");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(dnx_dbal_entry_handle_take(unit, "TUNNELED_NAT", &entry_handle_id), "");
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "GLOB_IN_LIF", &u_ipv4_inlif), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "L4_PORT", &u_udp_port), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "IPV4", 0, &u_nat_ip), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "L4_PORT", 0, &u_exp_udp_dport), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT"), "[TUNNELED_NAT]");
    BCM_IF_ERROR_RETURN_MSG(dnx_dbal_entry_handle_free(unit,entry_handle_id), "");

    return BCM_E_NONE;
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

    /*
     * Configure ingress interfaces
     */
    BCM_IF_ERROR_RETURN_MSG(ip_tunnel_termination_open_route_interfaces_provider(unit, in_provider_port), "");

    /*
     * Configure egress interfaces
     */
    BCM_IF_ERROR_RETURN_MSG(ip_tunnel_encapsulation_open_route_interfaces_provider(unit, out_provider_port), "");

    /*
     * Create ARP entry and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &(cint_ip_tunnel_basic_info.arp_id),
                                       cint_ip_tunnel_basic_info.arp_next_hop_mac,
                                       cint_ip_tunnel_basic_info.out_provider_vlan),
                                       "create egress object ARP only");

    return BCM_E_NONE;
}

/*
 * Configure Ingress interfaces
 * Inputs: in_provider_port - incoming port;
 */
int
ip_tunnel_termination_open_route_interfaces_provider(
    int unit,
    int in_provider_port)
{
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    /*
     * Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_provider_port, cint_ip_tunnel_basic_info.eth_rif_intf_in_provider),
        "eth_rif_intf_in_provider");

    /*
     * Create routing interface for the tunnel's IP.
     * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure),
     * to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_in_provider,
                             cint_ip_tunnel_basic_info.intf_in_provider_mac_address), "eth_rif_intf_provider");

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_ip_tunnel_basic_info.eth_rif_intf_in_provider;
    ingr_itf.vrf     = cint_ip_tunnel_basic_info.in_provider_vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "eth_rif_intf_in_provider");

    return BCM_E_NONE;
}

/*
 * Configure Egress interfaces
 * Inputs: out_provider_port - outgoing port;
 */
int
ip_tunnel_encapsulation_open_route_interfaces_provider(
    int unit,
    int out_provider_port)
{

    /*
     * Set Out-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_provider_port), "out_provider_port");
    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_out_provider,
                             cint_ip_tunnel_basic_info.intf_out_provider_mac_address), "eth_rif_intf_out_provider");
    return BCM_E_NONE;
}

int
tunnel_terminator_create(
    int unit)
{
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

    if (cint_ip_tunnel_basic_info.is_udp_tunnel_dst_nat)
    {
        tunnel_term.flags |= BCM_TUNNEL_TERM_TUNNEL_DST_NAT;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term), "");

    /*
     * In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API
     * For Vxlan-gpe tunnels, vrf is not a tunnel in-lif property,
     * vrf is resolved by additional lookup: VNI, network domain -> FODO.
     *
     */
    /* we don't need to update the VRF in case of DST NAT */
    if (cint_ip_tunnel_basic_info.in_tunnel_type != bcmTunnelTypeVxlanGpe
        && cint_ip_tunnel_basic_info.in_tunnel_type != bcmTunnelTypeVxlan6Gpe
        && !cint_ip_tunnel_basic_info.is_udp_tunnel_dst_nat) {
       bcm_l3_ingress_t ing_intf;
       bcm_l3_ingress_t_init(&ing_intf);
       ing_intf.flags = BCM_L3_INGRESS_WITH_ID;        /* must, as we update exist RIF */
       ing_intf.vrf   = cint_ip_tunnel_basic_info.out_provider_vrf;

       /*
        * Convert tunnel's GPort ID to intf ID
        */
       bcm_if_t intf_id;
       BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);
       BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_create(unit, ing_intf, intf_id), "");
    }

    cint_ip_tunnel_basic_info.tunnel_id = tunnel_term.tunnel_id;

    return BCM_E_NONE;
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

    tunnel_info.udp_src_port = cint_ip_tunnel_basic_info.out_udp_src_port;
    tunnel_info.udp_dst_port = cint_ip_tunnel_basic_info.out_udp_dst_port;

    bcm_l3_intf_t_init(&l3_intf);
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_info), "");

    *tunnel_intf = l3_intf.l3a_intf_id;
    return BCM_E_NONE;
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

    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array), "");

    *mpls_intf_id = label_array[0].tunnel_id;

    return BCM_E_NONE;
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

    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_switch_create(unit, &entry), "");

    return BCM_E_NONE;
}

int
vxlan_network_domain_management_l3_egress(
    int unit)
{
    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VNI->VRF
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING;
    config.vrf              = cint_ip_tunnel_basic_info.out_provider_vrf;
    config.vni              = 5000;
    config.network_domain   = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &config), "");
    return BCM_E_NONE;
}

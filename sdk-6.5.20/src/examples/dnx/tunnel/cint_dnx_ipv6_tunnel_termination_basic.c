/*
 * cint_dnx_ipv6_tunnel_termination.c
 *
 *  Created on: Aug 23, 2017
 *      Author: stand
 */

/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_dnx_ipv6_tunnel_termination.c Purpose: Various examples for IPTunnels.
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
 * cint ../../../../src/examples/dnx/cint_dnx_ipv6_tunnel_termination_basic.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int provider_port = 200;
 * int access_port = 201;
 * rv = ipv6_tunnel_term_basic(unit,provider_port,access_port);
 * print rv;
 *
 *
 *  Scenarios configured in this cint:
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) Basic IPv6 Tunnel termination
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  A GRE4oIPv6 tunnel header is being terminated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ###############################################################################################################################
 *  Sending packet from  provider port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-++-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+++
 *   |    |      DA      | SA           ||             IPv6 tunnel SIP                   ||                 IPv6 tunnel DIP               ||  GRE  ||      IPv4 SIP     ||      IPv4 DIP      ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||    ECEF:EEED:ECEB:EAE9:E8E7:E6E5:E4E3:E2E1    ||    2001:0DB8:0000:0000:0211:22FF:FE33:4455    ||       ||  192.128.1.1      ||    160.161.161.162 ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-++-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+++
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on access port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-
 *   |    |      DA      | SA           ||      IPv4 SIP     ||      IPv4 DIP      ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||  160.161.161.162  ||    192.128.1.1     ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ################################################################################################################################
 */

struct cint_ipv6_tunnel_info_s {
    int eth_rif_intf_provider; /* in RIF */
    int eth_rif_intf_access; /* out RIF */
    int tunnel_if; /* RIF, property of the tunnel termination inLif.*/
    bcm_mac_t intf_provider_mac_address; /* mac for in RIF */
    bcm_mac_t intf_access_mac_address; /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac; /* mac for next hop */
    int access_vrf; /* router instance VRF resolved at VTT1*/
    int provider_vrf; /* VRF after termination, resolved as Tunnel-InLif property */
    int fec_id; /* FEC id */
    int arp_id; /* Id for ARP entry */
    bcm_ip_t route_ipv4_dip; /* Route DIP */
    uint32 route_ipv4_dip_mask; /* mask for route DIP */

    bcm_ip6_t tunnel_ipv6_dip; /* tunnel DIP */
    bcm_ip6_t tunnel_ipv6_dip_mask; /* tunnel DIP mask*/
    bcm_ip6_t tunnel_ipv6_sip; /* tunnel SIP */
    bcm_ip6_t tunnel_ipv6_sip_mask; /* tunnel SIP mask */
    bcm_tunnel_type_t tunnel_type; /* bcmTunnelType */
    int tunnel_class;
    int udp_dst_value; /* The UDP destination value */
};

cint_ipv6_tunnel_info_s cint_ipv6_basic_tunnel_info = {
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
5001,
/*
 * arp_id
 */
9000,
/*
 * route_ipv4_dip
 */
0xA0A1A1A2 /* 160.161.161.162 */,
/*
 * route_ipv4_dip_mask
 */
0xfffffff0,
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
/*udp_dst_value*/
0
};

int ipv6_tunnel_terminator_create(int unit)
{

    int rv;
    bcm_tunnel_terminator_t tunnel_termination_obj;
    bcm_tunnel_terminator_config_action_t p2p_tunnel_terminator_config_action;
    bcm_tunnel_terminator_config_key_t p2p_tunnel_terminator_config_key;

    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error: ipv6_tunnel_terminator_create is invoked for device which is not Jericho2 \n");
        return rv;
    }

    /* Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_termination_obj);
    tunnel_termination_obj.dip6 = cint_ipv6_basic_tunnel_info.tunnel_ipv6_dip;
    tunnel_termination_obj.dip6_mask = cint_ipv6_basic_tunnel_info.tunnel_ipv6_dip_mask;
    tunnel_termination_obj.sip6 = cint_ipv6_basic_tunnel_info.tunnel_ipv6_sip;
    tunnel_termination_obj.sip6_mask = cint_ipv6_basic_tunnel_info.tunnel_ipv6_sip_mask;
    tunnel_termination_obj.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID | BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_termination_obj.tunnel_id, 50000);

    /* for jericho 2, the vrf is part of the key */
    tunnel_termination_obj.vrf = cint_ipv6_basic_tunnel_info.provider_vrf;
    tunnel_termination_obj.type = cint_ipv6_basic_tunnel_info.tunnel_type;

    p2p_tunnel_terminator_config_action.tunnel_class = cint_ipv6_basic_tunnel_info.tunnel_class;
    tunnel_termination_obj.tunnel_class = p2p_tunnel_terminator_config_action.tunnel_class;
    tunnel_termination_obj.udp_dst_port = cint_ipv6_basic_tunnel_info.udp_dst_value;

    p2p_tunnel_terminator_config_key.dip6 = tunnel_termination_obj.dip6;
    p2p_tunnel_terminator_config_key.dip6_mask = tunnel_termination_obj.dip6_mask;
    p2p_tunnel_terminator_config_key.flags = tunnel_termination_obj.flags;
    p2p_tunnel_terminator_config_key.type = tunnel_termination_obj.type;
    p2p_tunnel_terminator_config_key.vrf = tunnel_termination_obj.vrf;
    p2p_tunnel_terminator_config_key.udp_dst_port = tunnel_termination_obj.udp_dst_port;

    /*
     * Flags parameter is not used in the function.
     */
    bcm_tunnel_terminator_config_add(unit, 0, p2p_tunnel_terminator_config_key, p2p_tunnel_terminator_config_action);

    rv = bcm_tunnel_terminator_create(unit, &tunnel_termination_obj);
    if (rv != BCM_E_NONE)
    {
        printf(
                "Error: ipv6_tunnel_terminator_create failed to create tunnel termination object -> bcm_tunnel_terminator_create() failed. \n");
        return rv;
    }

    /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
    bcm_l3_ingress_t ingress_in_lif;
    bcm_l3_ingress_t_init(&ingress_in_lif);
    bcm_if_t in_lif_id;

    ingress_in_lif.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
    ingress_in_lif.vrf = cint_ipv6_basic_tunnel_info.access_vrf;

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

int ipv6_tunnel_term_basic(int unit, int provider_port, int access_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    l3_ingress_intf ingress_in_lif;
    l3_ingress_intf_init(&ingress_in_lif);

    proc_name = "ipv6_tunnel_term_ipv4_in_ipv6";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: Started for non Jericho2 device. \n", proc_name);
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, provider_port, cint_ipv6_basic_tunnel_info.eth_rif_intf_provider);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: in_port_intf_set() failed.\n", proc_name);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: out_port_set() failed.\n", proc_name);
        return rv;
    }

    /*
     * Create routing interface for the tunnel's IP.
     * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure), to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_ipv6_basic_tunnel_info.eth_rif_intf_provider,
            cint_ipv6_basic_tunnel_info.intf_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: intf_eth_rif_create() failed.\n", proc_name);
        return rv;
    }

    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_ipv6_basic_tunnel_info.eth_rif_intf_access,
            cint_ipv6_basic_tunnel_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: intf_eth_rif_create() failed.\n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingress_in_lif.intf_id = cint_ipv6_basic_tunnel_info.eth_rif_intf_provider;
    ingress_in_lif.vrf = cint_ipv6_basic_tunnel_info.provider_vrf;

    rv = intf_ingress_rif_set(unit, &ingress_in_lif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: intf_ingress_rif_set() failed.\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = ipv6_tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: ipv6_tunnel_terminator_create() failed.\n", proc_name);
        return rv;
    }

    cint_ipv6_basic_tunnel_info.arp_id = 5050;
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ipv6_basic_tunnel_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF
     */
    rv = l3__egress_only_fec__create(unit,
            cint_ipv6_basic_tunnel_info.fec_id,
            cint_ipv6_basic_tunnel_info.eth_rif_intf_access,
            cint_ipv6_basic_tunnel_info.arp_id,
            access_port,
            0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: create egress object FEC only\n", proc_name);
        return rv;
    }

    /*
     * Create ARP entry and set its properties
     */

    rv = l3__egress_only_encap__create(unit, 0, &(cint_ipv6_basic_tunnel_info.arp_id),
            cint_ipv6_basic_tunnel_info.arp_next_hop_mac,
            cint_ipv6_basic_tunnel_info.eth_rif_intf_access);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: create egress object ARP only.\n", proc_name);
        return rv;
    }

    /*
     * Add Route of IPv4 header entry
     */
    rv = add_route_ipv4(unit, cint_ipv6_basic_tunnel_info.route_ipv4_dip,
            cint_ipv6_basic_tunnel_info.route_ipv4_dip_mask,
            cint_ipv6_basic_tunnel_info.access_vrf,
            cint_ipv6_basic_tunnel_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: add_route_ipv4() failed.\n", proc_name);
        return rv;
    }

    return rv;
}

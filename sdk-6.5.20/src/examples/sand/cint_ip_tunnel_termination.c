/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_ip_tunnel_termination.c Purpose: Various examples for IPTunnels.
 */

/*
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/cint_ip_tunnel_termination.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int provider_port = 200;
 * int access_port = 201;
 * rv = ip_tunnel_term_ipany_in4_example(unit,provider_port,access_port);
 * print rv;
 */
struct cint_ip_tunnel_basic_info_s
{
    int eth_rif_intf_provider;            /* in RIF */
    int eth_rif_intf_access;              /* out RIF */
    int tunnel_if;                        /* RIF, property of the TT inLif. */
    bcm_mac_t intf_provider_mac_address;  /* mac for in RIF */
    bcm_mac_t intf_access_mac_address;    /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac;           /* mac for next hop */
    int access_vrf;                       /* router instance VRF resolved at VTT1 */
    int provider_vrf;                     /* VRF after TT, resolved as Tunnel-InLif property */
    int access_vlan;                      /* VID assigned to the outgoing packet */
    int fec_id;                           /* FEC id */
    int arp_id;                           /* Id for ARP entry */
    bcm_ip_t route_dip;                   /* Route DIP */
    bcm_ip_t mask;                        /* mask for route DIP */
    bcm_ip_t tunnel_dip;                  /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;             /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                  /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;             /* tunnel SIP mask */
    int tunnel_type;                      /* bcmTunnelType */
    int udp_tunnel_ipv4_dst_port;         /* UDP tunnel ipv4 destination port */
    int udp_tunnel_ipv6_dst_port;         /* UDP tunnel ipv6 destination port */
    bcm_ip6_t ipv6_dip;                   /* ipv6 forwarding dip */
    bcm_ip6_t ipv6_mask;                  /* ipv6 forwarding route mask */
    int udp_tunnel_mpls_dst_port;         /* UDP tunnel mpls destination port */
    int mpls_fwd_label;                   /* MPLS label used for forwarding */
    int udp_term_dst_port;                /* UDP tunnel dst_port used for TT */
/** Extra faulty-case info */
    int vrf_faulty;
    int eth_rif_provider_faulty;
    int arp_faulty;
    int fec_faulty;
    bcm_mac_t arp_next_hop_mac_provider_faulty;
    int access_vlan_faulty;
    bcm_ip_t tunnel_dip_faulty;
    bcm_ip_t tunnel_sip_faulty;
    int esp_term;                       /* ESP header should also be terminated */
};

cint_ip_tunnel_basic_info_s cint_ip_tunnel_basic_info = {
    /*
     * eth_rif_intf_provider | eth_rif_intf_access | tunnel_if
     */
    15, 100, 20,
    /*
     * intf_provider_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /*
     * intf_access_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01},
    /*
     * arp_next_hop_mac
     */
    {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * access_vrf | provider_vrf
     */
    5, 1,
    /*
     * access_vlan
     */
    100,
    /*
     * fec_id
     */
    5001,
    /*
     * arp_id
     */
    0,
    /*
     * route_dip (Ipv4)
     */
    0xA0A1A1A2, /* 160.161.161.162 */
    /*
     * mask (Ipv4 route entry )
     */
    0xfffffff0,
    /*
     * tunnel_dip
     */
    0xAB000001, /* 171.0.0.1 */
    /*
     * tunnel_dip_mask
     */
    0xffffffff,
    /*
     * tunnel_sip
     */
    0xAC000001, /* 172.0.0.1 */
    /*
     * tunnel_sip_mask
     */
    0xffffffff,
    /*
     * tunnel_type
     */
    bcmTunnelTypeGreAnyIn4,
    /*
     * udp_tunnel_ipv4_dst_port
     */
    0x6605,
    /*
     * udp_tunnel_ipv6_dst_port
     */
    0x6615,
    /*
     * ipv6_dip
     */
    {
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xff, 0x13},
    /*
     * ipv6_mask
     */
    {
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0},
    /*
     * udp_tunnel_mpls_dst_port
     */
    0x6635,
    /*
     * mpls_fwd_label
     */
    1456,
    /*
     * udp_term_dst_port
     */
    0x6605,     /* default IPv4 */
    /*
     * vrf_faulty
     */
    15,
    /*
     * eth_rif_provider_faulty
     */
    25,
    /*
     * arp_faulty
     */
    31693,
    /*
     * fec_faulty
     */
    6001,
    /*
     * arp_next_hop_mac_provider_faulty
     */
    {0x00, 0x00, 0x00, 0x00, 0xcd, 0x11},
    /*
     * access_vlan_faulty
     */
    120,
    /*
     * tunnel_dip_faulty
     */
    0xAD000001, /* 173.0.0.1 */
    /*
     * tunnel_sip_faulty
     */
    0xAE000001,  /* 174.0.0.1 */
    /*
     * esp_term
     */
    0
};

/**  Scenarios configured in this cint:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) IPv4oIPv4oETH0 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  An IPv4 tunnel header is being terminated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  #######################################################################################################################
 *  Sending packet on provider port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   |    |      DA      | SA           || IPv4 tunnel SIP || IPv4 tunnel DIP ||   IPv4 route SIP  ||   IPv4 route DIP   ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||    172.0.0.1    ||    171.0.0.1    ||    192.128.1.1    ||  160.161.161.162   ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on access port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   |    |      DA      | SA           ||   IPv4 route SIP  ||   IPv4 route DIP   ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||    192.128.1.1    ||  160.161.161.162   ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ########################################################################################################################
 *
 * Main function for ip tunnel termination of Ipv4 tunnel
 * The function calls the basic ip_tunnel_termination_open_route_interfaces, then creates the tunnel
 * terminator with bcmTunnelTypeIpAnyIn4 type and adds route entries for forwarding the inner Ipv4 
 * and Ipv6 headers.
 *
 * The configuration also sets up interfaces and adds forwarding entries for faulty case scenarios
 * where the incoming packet arrives through a port and In-ETH-RIF configured for a different VRF
 * than the one configured in the tunnel termination In-LIF. The tunneled packet is forwarded 
 * onwards on the tunnel IPv4 DIP to a different next-hop-mac.
 *
 * Inputs: unit          - relevant unit;
 *         provider_port - incoming port;
 *         access_port   - outgoing port;
 */
int
ip_tunnel_term_ipany_in4_example_em_basic(
    int unit,
    int provider_port,
    int provider_port_faulty,
    int access_port)
{
    int rv = BCM_E_NONE;

    char *proc_name;
    proc_name = "ip_tunnel_term_ipany_in4_example_em_basic";

    cint_ip_tunnel_basic_info.tunnel_sip_mask  = 0xFFFFFFFF;  /*default 255.255.255.255 */
    cint_ip_tunnel_basic_info.tunnel_dip_mask  = 0xFFFFFFFF;  /*default 255.255.255.255 */

    rv = ip_tunnel_term_ipany_in4_example(unit, provider_port, provider_port_faulty, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_term_ipany_in4_example\n", proc_name);
        return rv;
    }

    return rv;
}

int
ip_tunnel_term_ipany_in4_example_tcam_subnet(
    int unit,
    int provider_port,
    int provider_port_faulty,
    int access_port)
{
    int rv = BCM_E_NONE;

    char *proc_name;
    proc_name = "ip_tunnel_term_ipany_in4_example_tcam_subnet";

    cint_ip_tunnel_basic_info.tunnel_sip_mask  = 0xFFFE0000;  /* subnet 255.254.0.0 */
    cint_ip_tunnel_basic_info.tunnel_dip_mask  = 0xFFF00000;  /* subnet 255.240.0.0 */

    rv = ip_tunnel_term_ipany_in4_example(unit, provider_port, provider_port_faulty, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_term_ipany_in4_example\n", proc_name);
        return rv;
    }

    return rv;
}

int
ip_tunnel_term_ipany_in4_example(
    int unit,
    int provider_port,
    int provider_port_faulty,
    int access_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    proc_name = "ip_tunnel_term_ipany_in4_example";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2, device_is_jericho2\n", proc_name);
        return rv;
    }

    /** Initialize ports, and create Ingress and Egress LIFs (ETH-RIFs), ARP */
    rv = ip_tunnel_termination_open_route_interfaces(unit, provider_port, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): ip_tunnel_termination_open_route_interfaces \n", proc_name);
        return rv;
    }

    /** Select the bcmTunnelTypes */
    cint_ip_tunnel_basic_info.tunnel_type = bcmTunnelTypeIpAnyIn4;

    /** Create the tunnel termination lookup and the tunnel termination inlif*/
    rv = tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function tunnel_terminator_create \n");
        return rv;
    }

    /** Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF */
    if (device_is_jericho2)
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id,
                                     cint_ip_tunnel_basic_info.eth_rif_intf_access,
                                     cint_ip_tunnel_basic_info.arp_id, access_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create \n", proc_name);
        return rv;
    }

    /** Initialize ports, logical interfaces and ARP for faulty termination testing */
    rv = ip_tunnel_termination_open_route_interfaces_faulty(unit, provider_port_faulty,
                                                            cint_ip_tunnel_basic_info.vrf_faulty);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_termination_open_route_interfaces_faulty \n", proc_name);
        return rv;
    }

    /** Add IPv4 and IPv6 routes to IP tunnel */
    if (device_is_jericho2)
    {
        /** IPv4 native */
        rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_dip,
                           cint_ip_tunnel_basic_info.access_vrf, cint_ip_tunnel_basic_info.fec_id, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_host_ipv4, route_dip \n");
            return rv;
        }
        /** IPv6 native */
        rv = add_host_ipv6(unit, cint_ip_tunnel_basic_info.ipv6_dip,
                           cint_ip_tunnel_basic_info.access_vrf, cint_ip_tunnel_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_host_ipv6, ipv6_dip \n");
            return rv;
        }
        /** IPv4 tunnel DIP host entry for faulty case: wrong VRF -> NO TEMINATION*/
        rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.tunnel_dip,
                           cint_ip_tunnel_basic_info.vrf_faulty,
                           cint_ip_tunnel_basic_info.eth_rif_intf_access,
                           cint_ip_tunnel_basic_info.arp_faulty, access_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_host_ipv4, tunnel_dip \n");
            return rv;
        }
    }
    else
    {
        /** IPv4 */
        rv = add_route_ipv4(unit, cint_ip_tunnel_basic_info.route_dip,
                            cint_ip_tunnel_basic_info.mask,
                            cint_ip_tunnel_basic_info.access_vrf, cint_ip_tunnel_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv4 \n");
            return rv;
        }
        /** IPv6 */
        rv = add_route_ipv6(unit, cint_ip_tunnel_basic_info.ipv6_dip,
                            cint_ip_tunnel_basic_info.ipv6_mask,
                            cint_ip_tunnel_basic_info.access_vrf, cint_ip_tunnel_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv6 \n");
            return rv;
        }
        /** Create FEC for faulty-case scenarios to route on tunnel DIP*/
        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_faulty,
                                         cint_ip_tunnel_basic_info.access_vlan_faulty,
                                         cint_ip_tunnel_basic_info.arp_faulty, access_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create \n", proc_name);
            return rv;
        }
        /** IPv4 tunnel DIP route entry for faulty case: wrong VRF -> NO TEMINATION*/
        rv = add_route_ipv4(unit, cint_ip_tunnel_basic_info.tunnel_dip,
                            cint_ip_tunnel_basic_info.tunnel_dip_mask,
                            cint_ip_tunnel_basic_info.vrf_faulty, cint_ip_tunnel_basic_info.fec_faulty);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_host_ipv4, tunnel_dip \n");
            return rv;
        }
    }

    return rv;
}


/**  Scenario configured in this cint:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) IPv6oGRE4oIPv4oETH0
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  A GRE4oIPv4 tunnel header is being terminated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##################################################################################################################################################################################
 *  Sending packet on provider port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA      | SA           || IPv4 tunnel SIP || IPv4 tunnel DIP ||  GRE  ||                 IPv6 route SIP              ||              IPv6 route DIP                ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||    172.0.0.1    ||    171.0.0.1    ||       ||  3ffe:0000:0000:0000:fa4f:57ff:fe72:0000    ||  e1e2:e3e4:e5e6:e7e8:e9ea:ebec:edee:efec   ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on access port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA      | SA           ||                 IPv6 route SIP              ||              IPv6 route DIP                ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||  3ffe:0000:0000:0000:fa4f:57ff:fe72:0000    ||  e1e2:e3e4:e5e6:e7e8:e9ea:ebec:edee:efec   ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ####################################################################################################################################
 * Inputs: unit          - relevant unit;
 *         provider_port - incoming port;
 *         access_port   - outgoing port;
 */
int
ip_tunnel_gre4_term_ipv6_fwd_example_em_basic(
    int unit,
    int provider_port,
    int access_port)
{
    int rv = BCM_E_NONE;

    char *proc_name;
    proc_name = "ip_tunnel_gre4_term_ipv6_fwd_example_em_basic";

    cint_ip_tunnel_basic_info.tunnel_sip_mask  = 0xFFFFFFFF;  /*default 255.255.255.255 */
    cint_ip_tunnel_basic_info.tunnel_dip_mask  = 0xFFFFFFFF;  /*default 255.255.255.255 */

    rv = ip_tunnel_gre4_term_ipv6_fwd_example(unit, provider_port, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_gre4_term_ipv6_fwd_example\n", proc_name);
        return rv;
    }

    return rv;
}

int
ip_tunnel_gre4_term_ipv6_fwd_example_tcam_subnet(
    int unit,
    int provider_port,
    int access_port)
{
    int rv = BCM_E_NONE;

    char *proc_name;
    proc_name = "ip_tunnel_gre4_term_ipv6_fwd_example_tcam_subnet";

    cint_ip_tunnel_basic_info.tunnel_sip_mask  = 0xFFFE0000;  /* subnet 255.254.0.0 */
    cint_ip_tunnel_basic_info.tunnel_dip_mask  = 0xFFF00000;  /* subnet 255.240.0.0 */

    rv = ip_tunnel_gre4_term_ipv6_fwd_example(unit, provider_port, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_gre4_term_ipv6_fwd_example\n", proc_name);
        return rv;
    }
    return rv;
}

int
ip_tunnel_gre4_term_ipv6_fwd_example(
    int unit,
    int provider_port,
    int access_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    proc_name = "ip_tunnel_term_greany_in4_example";

    bcm_gport_t gport;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2, device_is_jericho2 \n", proc_name);
        return rv;
    }

    /** Initialize ports, and create Ingress and Egress LIFs (ETH-RIFs), ARP */
    rv = ip_tunnel_termination_open_route_interfaces(unit, provider_port, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): ip_tunnel_termination_open_route_interfaces \n", proc_name);
        return rv;
    }

    /** Create the tunnel termination lookup and the tunnel termination inlif*/
    rv = tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tunnel_terminator_create \n");
        return rv;
    }

    /** Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF */
    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id,
                                     cint_ip_tunnel_basic_info.eth_rif_intf_access,
                                     cint_ip_tunnel_basic_info.arp_id, access_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create \n", proc_name);
        return rv;
    }

    /** Initialize ports, logical interfaces and ARP for faulty termination testing */
    rv = ip_tunnel_termination_open_route_interfaces_faulty(unit, provider_port,
                                                            cint_ip_tunnel_basic_info.provider_vrf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_termination_open_route_interfaces_faulty \n", proc_name);
        return rv;
    }

    /** Add IPv4 and IPv6 routes to IP tunnel*/
    if (device_is_jericho2)
    {
        /** IPv6 native */
        rv = add_host_ipv6(unit, cint_ip_tunnel_basic_info.ipv6_dip,
                           cint_ip_tunnel_basic_info.access_vrf, cint_ip_tunnel_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_host_ipv6 \n");
            return rv;
        }
        /** IPv4 tunnel DIP host entry for faulty case: wrong tunnel IPv4 DIP -> NO TEMINATION*/
        rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.tunnel_dip_faulty,
                           cint_ip_tunnel_basic_info.provider_vrf,
                           cint_ip_tunnel_basic_info.eth_rif_intf_access,
                           cint_ip_tunnel_basic_info.arp_faulty, access_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_host_ipv4, tunnel_dip \n");
            return rv;
        }
        /** IPv4 tunnel DIP host entry for faulty case: wrong tunnel IPv4 SIP -> NO TEMINATION*/
        rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.tunnel_dip,
                           cint_ip_tunnel_basic_info.provider_vrf,
                           cint_ip_tunnel_basic_info.eth_rif_intf_access,
                           cint_ip_tunnel_basic_info.arp_faulty, access_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_host_ipv4, tunnel_dip \n");
            return rv;
        }
    }
    else
    {
        /** IPv6 */
        rv = add_route_ipv6(unit, cint_ip_tunnel_basic_info.ipv6_dip,
                            cint_ip_tunnel_basic_info.ipv6_mask,
                            cint_ip_tunnel_basic_info.access_vrf, cint_ip_tunnel_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv6 \n");
            return rv;
        }
        /** Create FEC for faulty-case scenarios to route on tunnel DIP */
        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_faulty,
                                         cint_ip_tunnel_basic_info.access_vlan_faulty,
                                         cint_ip_tunnel_basic_info.arp_faulty, access_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
            return rv;
        }
        /** IPv4 tunnel DIP route entry for faulty case: wrong tunnel IPv4 DIP -> NO TEMINATION*/
        rv = add_route_ipv4(unit, cint_ip_tunnel_basic_info.tunnel_dip_faulty,
                            cint_ip_tunnel_basic_info.tunnel_dip_mask,
                            cint_ip_tunnel_basic_info.provider_vrf, cint_ip_tunnel_basic_info.fec_faulty);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv4, tunnel_dip \n");
            return rv;
        }
        /** IPv4 tunnel DIP route entry for faulty case: wrong tunnel IPv4 SIP -> NO TEMINATION*/
        rv = add_route_ipv4(unit, cint_ip_tunnel_basic_info.tunnel_dip,
                            cint_ip_tunnel_basic_info.tunnel_dip_mask,
                            cint_ip_tunnel_basic_info.provider_vrf, cint_ip_tunnel_basic_info.fec_faulty);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv4, tunnel_dip \n");
            return rv;
        }
    }

    return rv;
}

/**  Scenario configured in this cint:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) MPLSoGre4oIPv4oETH0 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  A Gre4oIPv4 tunnel header is being terminated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  #######################################################################################################################
 *  Sending packet from  provider port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   |    |      DA      |      SA      || IPv4 tunnel SIP || IPv4 tunnel DIP ||   GRE4   ||   MPLS Label   ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||    172.0.0.1    ||    171.0.0.1    ||          ||      1456      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on access port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   |    |      DA      |      SA      ||   MPLS Label   ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||      1456      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ########################################################################################################################
 * Main function for ip tunnel termination of Gre4oIpv4 tunnel
 * The function calls the basic ip_tunnel_termination_open_route_interfaces, then creates the tunnel terminator with bcmTunnelTypeGreAnyIn4 type.
 * Adds MPLS entry for forwarding on the MPLS header.
 * Inputs:
 * unit          - relevant unit;
 * provider_port - incoming port;
 * access_port   - outgoing port;
 */
int
gre_tunnel_termination_tcam_mpls_fwd(
    int unit,
    int provider_port,
    int provider_port_faulty,
    int access_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    bcm_if_t mpls_tunnel_id;

    proc_name = "gre_tunnel_termination_tcam_mpls_fwd";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2, device_is_jericho2 \n", proc_name);
        return rv;
    }
    cint_ip_tunnel_basic_info.tunnel_dip_mask = 0xFFFF0000;
    cint_ip_tunnel_basic_info.tunnel_type = bcmTunnelTypeGreAnyIn4;

    rv = ip_tunnel_termination_open_route_interfaces(unit, provider_port, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_termination_open_route_interfaces \n");
        return rv;
    }

    /** Create the tunnel termination lookup and the tunnel termination inlif*/
    rv = tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function tunnel_terminator_create \n");
        return rv;
    }

    /** Initialize ports, logical interfaces and ARP for faulty termination testing */
    rv = ip_tunnel_termination_open_route_interfaces_faulty(unit, provider_port_faulty,
                                                            cint_ip_tunnel_basic_info.vrf_faulty);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_termination_open_route_interfaces_faulty \n", proc_name);
        return rv;
    }

    if (!device_is_jericho2)
    {
        /** Add MPLS routing to FEC interface */
        rv = add_route_mpls(unit, cint_ip_tunnel_basic_info.arp_id, cint_ip_tunnel_basic_info.mpls_fwd_label, &mpls_tunnel_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, add_route_mpls \n", proc_name);
            return rv;
        }
                /** Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF */
        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, mpls_tunnel_id, 0, access_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create \n", proc_name);
            return rv;
        }
        /** Create FEC for faulty-case scenarios to route on tunnel DIP*/
        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_faulty,
                                         cint_ip_tunnel_basic_info.access_vlan_faulty,
                                         cint_ip_tunnel_basic_info.arp_faulty, access_port, 
                                         (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0)?BCM_L3_ENCAP_SPACE_OPTIMIZED:0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create \n", proc_name);
            return rv;
        }
        /** IPv4 tunnel DIP route entry for faulty case: wrong VRF -> NO TEMINATION*/
        rv = add_route_ipv4(unit, cint_ip_tunnel_basic_info.tunnel_dip,
                            cint_ip_tunnel_basic_info.tunnel_dip_mask,
                            cint_ip_tunnel_basic_info.vrf_faulty, cint_ip_tunnel_basic_info.fec_faulty);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv4, tunnel_dip \n");
            return rv;
        }
    }
    else
    {
        /** Add MPLS route to IP-Tunnel. */
        rv = ip_tunnel_termination_create_mpls_swap_tunnel(unit, &mpls_tunnel_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, ip_tunnel_termination_create_mpls_swap_tunnel \n", proc_name);
            return rv;
        }
        /** Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF */

        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }


        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, mpls_tunnel_id, 0, access_port, 
                                        (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0)?BCM_L3_ENCAP_SPACE_OPTIMIZED:0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
            return rv;
        }
        rv = ip_tunnel_termination_ilm_switch_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label,
                                                            cint_ip_tunnel_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in ip_tunnel_termination_ilm_switch_tunnel_create \n");
            return rv;
        }
        /** IPv4 tunnel DIP host entry for faulty case: wrong VRF -> NO TEMINATION*/
        rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.tunnel_dip,
                           cint_ip_tunnel_basic_info.vrf_faulty,
                           cint_ip_tunnel_basic_info.eth_rif_intf_access,
                           cint_ip_tunnel_basic_info.arp_faulty, access_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_host_ipv4, tunnel_dip \n");
            return rv;
        }
    }
    return rv;
}

int
ip_tunnel_termination_ilm_switch_tunnel_create(
    int unit,
    bcm_mpls_label_t label,
    int fec_id)
{

    int rv;
    bcm_mpls_tunnel_switch_t entry;

    /*
     * No flag is needed to indicate the action, since the information for the swap action is included in the EEDB,
     * pointed from the ILM.
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
        printf("Error, bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    return rv;
}

int
ip_tunnel_termination_create_mpls_swap_tunnel(
    int unit,
    int *mpls_tunnel_id)
{

    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;
    int rv;

    bcm_mpls_egress_label_t_init(&label_array[0]);

    /*
     * Swapping label.
     * The label swapped for MPLS forwarding context.
     */
    label_array[0].label = cint_ip_tunnel_basic_info.mpls_fwd_label;
    /*
     * Set the next pointer of this entry to be the arp. This configuration is new compared to Jericho, where the arp
     * pointer used to be connected to the EEDB entry via bcm_l3_egress_create (with egress flag indication).
     */
    label_array[0].l3_intf_id = cint_ip_tunnel_basic_info.arp_id;
    label_array[0].flags     |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if (!is_device_or_above(unit,JERICHO2))
    {
        label_array[0].flags     |= BCM_MPLS_EGRESS_LABEL_TTL_COPY | BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *mpls_tunnel_id = label_array[0].tunnel_id;

    return rv;
}

int
mpls_encapsulation_basic_create_php_tunnel(
    int unit,
    int *mpls_tunnel_id)
{

    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;
    int rv;

    bcm_mpls_egress_label_t_init(&label_array[0]);

    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label_array[0].action = BCM_MPLS_EGRESS_ACTION_PHP;
    label_array[0].l3_intf_id = cint_ip_tunnel_basic_info.arp_id;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *mpls_tunnel_id = label_array[0].tunnel_id;

    return rv;

}

/**  Scenario configured in this cint:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *   IPvXoUDPoIPv4oETH
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  An UDPoIPv4 tunnel header is being terminated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##########################################################################################################################################
 *  Sending packet from  provider port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   |    |      DA      | SA           || IPv4 tunnel SIP || IPv4 tunnel DIP ||    UDP dst port   || IPv4 route SIP  ||   IPv4 route DIP   ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||    172.0.0.1    ||    171.0.0.1    ||      0x6605       ||  192.128.1.1    ||  160.161.161.162   ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on access port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   |    |      DA      | SA           ||   IPv4 route SIP  ||   IPv4 route DIP   ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||    192.128.1.1    ||  160.161.161.162   ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Sending packet from  provider port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   |    |      DA      | SA           || IPv4 tunnel SIP || IPv4 tunnel DIP ||    UDP dst port   ||                 IPv6 route SIP              ||              IPv6 route DIP                ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||    172.0.0.1    ||    171.0.0.1    ||      0x6615       ||  3ffe:0000:0000:0000:fa4f:57ff:fe72:0000    ||  e1e2:e3e4:e5e6:e7e8:e9ea:ebec:edee:efec   ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on access port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   |    |      DA      | SA           ||                 IPv6 route SIP              ||              IPv6 route DIP                ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||  3ffe:0000:0000:0000:fa4f:57ff:fe72:0000    ||  e1e2:e3e4:e5e6:e7e8:e9ea:ebec:edee:efec   ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ############################################################################################################################################
 * Main function for basic udp  tunnel termination scenarios in Jericho 2
 * The function basic_udp_tunnel_terminator implements the basic tunnel termination scenario.
 * The scenario supports termination of UDP tunnel and routing over Ipv4 and Ipv6.
 * Inputs:
 * unit - relevant unit;
 * provider_port - incoming port;
 * access_port - outgoing port;
 */
int
basic_udp_tunnel_terminator(
    int unit,
    int provider_port,
    int access_port)
{
    int rv;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2 device_is_jericho2 \n", proc_name);
        return rv;
    }
    rv = ip_tunnel_termination_open_route_interfaces(unit, provider_port, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_termination_open_route_interfaces \n");
        return rv;
    }

    /** Create UDP Tunnel Terminator with Ipv4 UDP Destination Port*/
    cint_ip_tunnel_basic_info.tunnel_type = bcmTunnelTypeUdp;
    if (!device_is_jericho2)
    {
        cint_ip_tunnel_basic_info.udp_term_dst_port = cint_ip_tunnel_basic_info.udp_tunnel_ipv4_dst_port;
    }
    rv = udp_tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, udp_tunnel_terminator_create, ipv4 destination port \n");
        return rv;
    }

    /** In J2 we don't use udp_dest port as part of the key, this is covered by first additional header - udp*/
    if (!device_is_jericho2)
    {
        /** Create UDP Tunnel Terminator with Ipv6 UDP Destination Port*/
        cint_ip_tunnel_basic_info.udp_term_dst_port = cint_ip_tunnel_basic_info.udp_tunnel_ipv6_dst_port;
        rv = udp_tunnel_terminator_create(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, udp_tunnel_terminator_create ipv6 destination port \n");
            return rv;
        }
    }
    else
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }

    /** Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF */
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id,
                                     cint_ip_tunnel_basic_info.eth_rif_intf_access,
                                     cint_ip_tunnel_basic_info.arp_id, access_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create \n", proc_name);
        return rv;
    }

    /** Add IPv6 Route entry, used to identify the forwarding header at FWD1 stage */
    rv = add_route_ipv6(unit, cint_ip_tunnel_basic_info.ipv6_dip, cint_ip_tunnel_basic_info.ipv6_mask,
                        cint_ip_tunnel_basic_info.access_vrf, cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_route_ipv6 \n");
        return rv;
    }

    /** Add IPv4 Route entry, used to identify the forwarding header at FWD1 stage */
    rv = add_route_ipv4(unit, cint_ip_tunnel_basic_info.route_dip, cint_ip_tunnel_basic_info.mask,
                        cint_ip_tunnel_basic_info.access_vrf, cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_route_ipv4 \n");
        return rv;
    }

    return rv;
}

/**  Scenario configured in this cint:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *   MPLSoUDPoIPv4oETH
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  An UDPoIPv4 tunnel header is being terminated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ################################################################################################################
 *  Sending packet from  provider port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA      | SA           || IPv4 tunnel SIP || IPv4 tunnel DIP ||    UDP dst port   ||  MPLS LABEL  |
 *   |    |0C:00:02:00:00|11:00:00:01:12||    172.0.0.1    ||    171.0.0.1    ||      0x6605       ||     1456     |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on access port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA      | SA           ||  MPLS LABEL  ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||     1456     ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ##################################################################################################################
 * Main function for udp  tunnel termination of mpls-forwarded packet scenario
 * The function ip_udp_tunnel_termination_mpls_fwd configures the device for termination of UDPoIPv4
 * header. The scenario supports termination of UDP tunnel and forwarding over MPLS.
 * Inputs: unit - relevant unit;
 *                provider_port - incoming port;
 *                access_port - outgoing port;
 */
int
ip_tunnel_udp_termination_mpls_fwd(
    int unit,
    int provider_port,
    int access_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    bcm_if_t mpls_intf_id;
    int device_is_jericho2;
    int mpls_tunnel_id;
    proc_name = "ip_udp_tunnel_termination_mpls_fwd";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2 device_is_jericho2 \n", proc_name);
        return rv;
    }

    /** Initialize ports, LIFs, Eth-RIFs etc */
    rv = ip_tunnel_termination_open_route_interfaces(unit, provider_port, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_termination_open_route_interfaces \n", proc_name);
        return rv;
    }

    /** Create UDP Tunnel Terminator with MPLS UDP Destination Port */
    cint_ip_tunnel_basic_info.tunnel_type = bcmTunnelTypeUdp;
    cint_ip_tunnel_basic_info.udp_term_dst_port = cint_ip_tunnel_basic_info.udp_tunnel_mpls_dst_port;
    /** Mask Tunnel SIP - TT Lookup is in VTT3 SEM */
    cint_ip_tunnel_basic_info.tunnel_sip_mask=0x00000000;
    rv = udp_tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, udp_tunnel_terminator_create \n", proc_name);
        return rv;
    }
    if (!device_is_jericho2)
    {
        /** Add MPLS routing to FEC interface */
        rv = add_route_mpls(unit, cint_ip_tunnel_basic_info.arp_id, cint_ip_tunnel_basic_info.mpls_fwd_label, &mpls_intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, add_route_mpls \n", proc_name);
            return rv;
        }

        /** Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF */
        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, mpls_intf_id, 0, access_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
            return rv;
        }
    }
    else
    {
        rv = ip_tunnel_termination_create_mpls_swap_tunnel(unit, &mpls_tunnel_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, ip_tunnel_termination_create_mpls_swap_tunnel\n", proc_name);
            return rv;
        }
        /** Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF */
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }

        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, mpls_tunnel_id, 0, access_port, 
                                        (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0)?BCM_L3_ENCAP_SPACE_OPTIMIZED:0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
            return rv;
        }
        rv = ip_tunnel_termination_ilm_switch_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label,
                                                            cint_ip_tunnel_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in ip_tunnel_termination_ilm_switch_tunnel_create \n");
            return rv;
        }
    }
    return rv;
}

/*
 * Interfaces initialization function for ip tunnel termination scenarios
 * The function ip_tunnel_termination_open_route_interfaces implements the basic configuration,
 * on top of witch all tunnel terminators are built. All tunnel termination scenarios will call this.
 * Inputs:   unit          - relevant unit;
 *           provider_port - incoming port;
 *           access_port   - outgoing port;
 */
int
ip_tunnel_termination_open_route_interfaces(
    int unit,
    int provider_port,
    int access_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;

    proc_name = "ip_tunnel_termination_open_route_interfaces";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2, device_is_jericho2 \n", proc_name);
        return rv;
    }

    if (device_is_jericho2)
    {
        cint_ip_tunnel_basic_info.fec_id = 40961;
    }

    /*
     * Configure ingress interfaces
     */
    rv = ip_tunnel_termination_open_route_interfaces_provider(unit, provider_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_termination_open_route_interfaces_provider \n", proc_name);
        return rv;
    }

    /*
     * Configure egress interfaces
     */
    rv = ip_tunnel_termination_open_route_interfaces_access(unit, access_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_termination_open_route_interfaces_access \n", proc_name);
        return rv;
    }

    /*
     * Create ARP entry and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_ip_tunnel_basic_info.arp_id),
                                       cint_ip_tunnel_basic_info.arp_next_hop_mac,
                                       cint_ip_tunnel_basic_info.access_vlan);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_encap__create \n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Configure Ingress interfaces
 * Inputs: provider_port - incomming port;
 *         *proc_name    - main function name;
 */
int
ip_tunnel_termination_open_route_interfaces_provider(
    int unit,
    int provider_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    l3_ingress_intf ingr_itf;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2, device_is_jericho2 \n", proc_name);
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, provider_port, cint_ip_tunnel_basic_info.eth_rif_intf_provider);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set, eth_rif_intf_provider \n", proc_name);
        return rv;
    }

    /*
     * Create routing interface for the tunnel's IP.
     * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure), to enable
     * IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_provider,
                             cint_ip_tunnel_basic_info.intf_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create, eth_rif_intf_provider \n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = cint_ip_tunnel_basic_info.eth_rif_intf_provider;
    ingr_itf.vrf = cint_ip_tunnel_basic_info.provider_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set \n", proc_name);
        return rv;
    }

    /*
     * In DPP (JR1) architecture IP-Tunnel LIF retrieve its RIF properties according to RIF-ID object (from tunnel_if).
     * While in DNX architecture (JR2) RIF properties are retrieved directly from IP-Tunnel LIF.
     */
    if (!device_is_jericho2)
    {
        /*
         * Create ingress Tunnel interface.
         * The function is used forlegacy devices only.
         */
        rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.tunnel_if,
                                 cint_ip_tunnel_basic_info.intf_provider_mac_address);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, intf_eth_rif_create, tunnel_if \n", proc_name);
            return rv;
        }
        /*
         * Set Incoming Tunnel-RIF properties
         */
        ingr_itf.intf_id = cint_ip_tunnel_basic_info.tunnel_if;
        ingr_itf.vrf = cint_ip_tunnel_basic_info.access_vrf;
        rv = intf_ingress_rif_set(unit, &ingr_itf);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, intf_ingress_rif_set, tunnel_if \n", proc_name);
            return rv;
        }
    }

    return rv;
}

/*
 * Configure Egress interfaces
 * Inputs: access_port - outgoing port;
 *         *proc_name  - main function name;
 */
int
ip_tunnel_termination_open_route_interfaces_access(
    int unit,
    int access_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set, access port \n", proc_name);
        return rv;
    }
    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_access,
                             cint_ip_tunnel_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create, eth_rif_intf_access \n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * NO-TERMINATION:
 * Extra Interfaces initialization function for faulty ip tunnel termination scenarios
 * The function ip_tunnel_termination_open_route_interfaces implements the basic configuration, 
 * on top of witch unsuccessful tunnel termination scenarios are built. Forwarding is to be done
 * on the tunnel dip in the specific tests configurations.
 * Inputs:   unit                 - relevant unit;
 *           provider_port_faulty - incoming port
 */
int
ip_tunnel_termination_open_route_interfaces_faulty(
    int unit,
    int provider_port,
    int provider_vrf)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    proc_name = "ip_tunnel_termination_open_route_interfaces_faulty";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2, device_is_jericho2 \n", proc_name);
        return rv;
    }

    /** Set In-Port to In ETh-RIF for Tunnel Routing */
    rv = in_port_intf_set(unit, provider_port, cint_ip_tunnel_basic_info.eth_rif_provider_faulty);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set, eth_rif_provider_faulty \n", proc_name);
        return rv;
    }

    /** Create routing interface for the tunnel's IP */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_provider_faulty,
                             cint_ip_tunnel_basic_info.intf_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create, eth_rif_provider_faulty \n", proc_name);
        return rv;
    }

    /** Set In-ETH-RIF properties. VRF, used for IP-Tunnel-Term.VRF lookup key, is updated. */
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = cint_ip_tunnel_basic_info.eth_rif_provider_faulty;
    ingr_itf.vrf = provider_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set eth_rif_provider_faulty \n", proc_name);
        return rv;
    }

    if (!device_is_jericho2)
    {
        /*
         * Create egress routing interface used for routing after the tunnel termination.
         * We are using it as egress ETH-RIF, providing the link layer SA.
         */
        rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.access_vlan_faulty,
                                 cint_ip_tunnel_basic_info.intf_access_mac_address);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, intf_eth_rif_create, intf_access_mac_address \n", proc_name);
            return rv;
        }
    }

    /** Create ARP entry and set its properties */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_ip_tunnel_basic_info.arp_faulty),
                                       cint_ip_tunnel_basic_info.arp_next_hop_mac_provider_faulty,
                                       cint_ip_tunnel_basic_info.access_vlan_faulty);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_encap__create \n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Function that creates the general-case tunnel terminator.
 * Inputs: unit - relevant unit;
 */
int
tunnel_terminator_create(
    int unit)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2, device_is_jericho2 \n");
        return rv;
    }

    /** Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = cint_ip_tunnel_basic_info.tunnel_dip;
    tunnel_term.dip_mask = cint_ip_tunnel_basic_info.tunnel_dip_mask;
    tunnel_term.sip = cint_ip_tunnel_basic_info.tunnel_sip;
    tunnel_term.sip_mask = cint_ip_tunnel_basic_info.tunnel_sip_mask;
    if (!device_is_jericho2)
    {
        tunnel_term.tunnel_if = cint_ip_tunnel_basic_info.tunnel_if;
    }
    tunnel_term.vrf = cint_ip_tunnel_basic_info.provider_vrf;

    if (tunnel_term.dip_mask != 0xFFFFFFFF)
    {
        tunnel_term.priority = 2;
    }

    if(cint_ip_tunnel_basic_info.esp_term)
    {
        tunnel_term.type = bcmTunnelTypeEsp;
    } else {
        tunnel_term.type = cint_ip_tunnel_basic_info.tunnel_type;
    }

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    if (device_is_jericho2)
    {
        /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
        bcm_l3_ingress_t ing_intf;
        bcm_l3_ingress_t_init(&ing_intf);

        ing_intf.flags = BCM_L3_INGRESS_WITH_ID;        /* must, as we update exist RIF */
        ing_intf.vrf = cint_ip_tunnel_basic_info.access_vrf;

        /** Convert tunnel's GPort ID to intf ID */
        bcm_if_t intf_id;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);

        if(cint_ip_tunnel_basic_info.esp_term)
        {
            ing_intf.flags |= BCM_L3_INGRESS_ROUTE_ENABLE_UNKNOWN;
        }

        rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_ingress_create \n");
            return rv;
        }
    }

    return rv;
}

/*
 * Function that creates the udp tunnel terminator.
 * Inputs: unit - relevant unit;
 */
int
udp_tunnel_terminator_create(
    int unit)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2, device_is_jericho2 \n");
        return rv;
    }

    /** Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = cint_ip_tunnel_basic_info.tunnel_dip;
    tunnel_term.dip_mask = cint_ip_tunnel_basic_info.tunnel_dip_mask;
    tunnel_term.sip = cint_ip_tunnel_basic_info.tunnel_sip;
    tunnel_term.sip_mask = cint_ip_tunnel_basic_info.tunnel_sip_mask;
    if (!device_is_jericho2)
    {
        tunnel_term.tunnel_if = cint_ip_tunnel_basic_info.tunnel_if;
        tunnel_term.udp_dst_port = cint_ip_tunnel_basic_info.udp_term_dst_port;
    }
    tunnel_term.vrf = cint_ip_tunnel_basic_info.provider_vrf;
    tunnel_term.type = cint_ip_tunnel_basic_info.tunnel_type;

    if(cint_ip_tunnel_basic_info.esp_term)
    {
        tunnel_term.udp_dst_port = 500;
    }

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    if (device_is_jericho2)
    {
        /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
        bcm_l3_ingress_t ing_intf;
        bcm_l3_ingress_t_init(&ing_intf);
        ing_intf.flags = BCM_L3_INGRESS_WITH_ID;        /* must, as we update exist RIF */
        ing_intf.vrf = cint_ip_tunnel_basic_info.access_vrf;

        /*
         * Convert tunnel's GPort ID to intf ID 
         */
        bcm_if_t intf_id;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);

        if(cint_ip_tunnel_basic_info.esp_term)
        {
            ing_intf.flags |= BCM_L3_INGRESS_ROUTE_ENABLE_UNKNOWN;
        }

        rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_ingress_create \n");
            return rv;
        }
    }

    return rv;
}

/* add MPLS route - J1-ONLY
 * Parameters: in_port - in port
 *             intf_id - ARP entry
 *             mpls_label - mpls forwarding label
 *             mpls_intf_id - tunnel ID of the mpls initiator
 */
int
add_route_mpls(
    int unit,
    bcm_if_t intf_id,
    int mpls_label,
    bcm_if_t *mpls_intf_id)
{
    int rv = BCM_E_NONE;
    bcm_mpls_label_t label_val;
    label_val = mpls_label;

    bcm_mpls_egress_label_t label;
    bcm_mpls_egress_label_t_init(&label);

    label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label.action = BCM_MPLS_EGRESS_ACTION_SWAP;
    label.label = label_val;
    label.l3_intf_id = intf_id;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, &label);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_mpls_tunnel_initiator_create \n");
        return rv;
    }
     *mpls_intf_id = label.tunnel_id;

    bcm_mpls_tunnel_switch_t tun_switch;
    bcm_mpls_tunnel_switch_t_init(&tun_switch);

    tun_switch.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    tun_switch.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    tun_switch.egress_if = cint_ip_tunnel_basic_info.fec_id;
    tun_switch.egress_label = label;
    tun_switch.label = label_val;

    rv = bcm_mpls_tunnel_switch_create(unit, tun_switch);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_mpls_tunnel_switch_create \n");
        return rv;
    }

    return rv;
}



/* Update UDP destination port.
   Part of the tunnel termination key for Jr1 devices
   Ignored by Jr2 devices */
int 
update_udp_dest_port_jr1(int unit, int udp_tunnel_ipv4_dst_port, 
                     int udp_tunnel_ipv6_dst_port, int udp_tunnel_mpls_dst_port) {
    int rv = BCM_E_NONE;

    cint_ip_tunnel_basic_info.udp_tunnel_ipv4_dst_port = udp_tunnel_ipv4_dst_port;
    cint_ip_tunnel_basic_info.udp_tunnel_ipv6_dst_port = udp_tunnel_ipv6_dst_port;
    cint_ip_tunnel_basic_info.udp_tunnel_mpls_dst_port = udp_tunnel_mpls_dst_port;

    return rv;

}

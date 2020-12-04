/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_ip_tunnel_encapsulation.c Purpose: Various examples for IPTunnels.
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
 * cint ../../../../src/examples/sand/cint_ip_tunnel_encapsulation.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int access_port = 200;
 * int provider_port = 201;
 * rv = ip_tunnel_gre4_encap_with_mpls_fwd(unit,access_port,provider_port);
 * print rv;
 *
 *
 *  Scenarios configured in this cint:
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) GRE4 IP Tunnel encapsulation of MPLS-forwarded packet
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  A GRE4oIPv4oETH tunnel header is being encapsulated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ###############################################################################################################################
 *  Receiving packet on access port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         ||   MPLS LABEL    ||
 *   |    |00:0C:00:02:00:56||00:50:00:00:12:34||     1456        ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Sending packet on provider port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         || IPv4 tunnel SIP || IPv4 tunnel DIP ||  GRE  ||   MPLS LABEL    ||
 *   |    |00:02:00:00:CD:1D||00:0C:00:02:00:56||    161.0.0.17   ||    160.0.0.17   ||   4   ||     1456        ||
 *   |    |                 ||                 ||                 ||                 ||  800  ||                 ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  ###############################################################################################################################
 *
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  2) GRE4 IP Tunnel encapsulation of IPv6-forwarded packet
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  A GRE4oIPv4oETH tunnel header is being encapsulated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ###############################################################################################################################
 *  Receiving packet on access port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         ||               IPv6 SIP                ||               IPv6 DIP                ||
 *   |    |00:0C:00:02:00:56||00:50:00:00:12:34||3ffe:0000:0000:0000:fa4f:57ff:fe72:0000||e1e2:e3e4:e5e6:e7e8:e9ea:ebec:edee:efec||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Sending packet on provider port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-
 *   |    |      DA         ||      SA         || IPv4 tunnel SIP || IPv4 tunnel DIP || GRE || MPLS LABEL ||       IPvX DIP       ||
 *   |    |00:02:00:00:CD:1D||00:0C:00:02:00:56||    161.0.0.17   ||    160.0.0.17   ||  4  ||   1456     || e1e2:e3e4:e5e6:e7e8: ||
 *   |    |                 ||                 ||                 ||                 || 800 ||            || :e9ea:ebec:edee:efec ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  ################################################################################################################################
 *
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  3) GRE8 IP Tunnel with load-balancing key encapsulation of MPLS-forwarded packet
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  A GRE8oIPv4oETH tunnel header is being encapsulated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################################################
 *  Receiving packet on access port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         ||   MPLS LABEL    ||
 *   |    |00:0C:00:02:00:56||00:50:00:00:12:34||     1456        ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Sending packet on provider port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         || IPv4 tunnel SIP || IPv4 tunnel DIP ||  GRE  ||   MPLS LABEL    ||
 *   |    |00:02:00:00:CD:1D||00:0C:00:02:00:56||    161.0.0.17   ||    160.0.0.17   ||   8   ||     1456        ||
 *   |    |                 ||                 ||    A0.00.00.11  ||    A1.00.00.11  ||  800  ||                 ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  ###############################################################################################################################
 *
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  4) GRE8 IP Tunnel with load-balancing key encapsulation of IPv4 and IPv6 forwarded packets
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  A GRE8oIPv4oETH tunnel header is being encapsulated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################################################
 *  Receiving packet on access port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         ||               IPvX SIP                ||               IPvX DIP                ||
 *   |    |00:0C:00:02:00:56||00:50:00:00:12:34||             192.128.1.4               ||            127.255.255.03             ||
 *   |    |                 ||                 ||3ffe:0000:0000:0000:fa4f:57ff:fe72:0000||e1e2:e3e4:e5e6:e7e8:e9ea:ebec:edee:efec||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Sending packet on provider port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         ||IPv4 tunnel SIP||IPv4 tunnel DIP|| GRE ||               IPvX DIP                ||
 *   |    |00:02:00:00:CD:1D||00:0C:00:02:00:56||   161.0.0.17  ||   160.0.0.17  ||  8  ||            127.255.255.03             ||
 *   |    |                 ||                 ||   A0.00.00.11 ||   A1.00.00.11 || 800 ||e1e2:e3e4:e5e6:e7e8:e9ea:ebec:edee:efec||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  ###############################################################################################################################
 *
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  5) MPLSoGRE4 IP Tunnel encapsulation of IPv4 and IPv6 forwarded packets
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  A MPLSoGRE4oIPv4oETH tunnel header is being encapsulated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################################################
 *  Receiving packet on access port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         ||       IPvX SIP       ||       IPvX DIP       ||
 *   |    |00:0C:00:02:00:56||00:50:00:00:12:34||     192.128.1.4      ||    127.255.255.03    ||
 *   |    |                 ||                 || 3ffe:0000:0000:0000: || e1e2:e3e4:e5e6:e7e8: ||
 *   |    |                 ||                 || :fa4f:57ff:fe72:0000 || :e9ea:ebec:edee:efec ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Sending packet on provider port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         ||IPv4 tunnel SIP||IPv4 tunnel DIP|| GRE || MPLS LABEL ||       IPvX DIP       ||
 *   |    |00:02:00:00:CD:1D||00:0C:00:02:00:56||   161.0.0.17  ||   160.0.0.17  ||  4  ||     200    ||    127.255.255.03    ||
 *        |                 ||                 ||   A0.00.00.11 ||   A1.00.00.11 || 800 ||            || e1e2:e3e4:e5e6:e7e8: ||
 *        |                 ||                 ||               ||               ||     ||            || :e9ea:ebec:edee:efec ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  ###############################################################################################################################
 */

struct cint_ip_tunnel_basic_info_s
{
    int eth_rif_intf_provider;  /* out RIF */
    int eth_rif_intf_access;    /* in RIF */
    bcm_mac_t intf_access_mac_address;  /* mac for in RIF */
    bcm_mac_t tunnel_next_hop_mac;      /* mac for next hop */
    int access_vrf;             /* VRF, resolved as Tunnel-InLif property */
    int tunnel_fec_id;          /* FEC id */
    int tunnel_arp_id;          /* Id for ARP entry */
    bcm_ip_t route_ipv4_dip;    /* Route DIP IPv4 */
    uint32 route_ipv4_mask;     /* mask for route DIP IPv4 */
    bcm_ip_t tunnel_dip;        /* tunnel DIP */
    bcm_ip_t tunnel_sip;        /* tunnel SIP */
    bcm_tunnel_type_t tunnel_type;      /* tunnel type */
    int provider_vlan;          /* VID assigned to the outgoing packet */
    int mpls_fwd_label;         /* MPLS label used for forwarding */
    int gre_lb_key_enable;      /* Enable LB-Key on GRE IP tunnel */
    int tunnel_ttl;             /* Ip tunnel header TTL */
    int tunnel_dscp;            /* Ip tunnel header differentiated services code point */
    int tunnel_intf;            /* Tunnel Interface */
    bcm_ip6_t route_ipv6_dip;   /* ipv6 forwarding dip */
    bcm_ip6_t route_ipv6_mask;  /* ipv6 forwarding mask */
    int mpls_tunnel_ttl;        /* MPLS tunnel Time-to-live */
    int mpls_tunnel_action;     /* MPLS tunnel forwarding action */
    int mpls_tunnel_flags_j1;   /* MPLS tunnel flags */
    int mpls_tunnel_flags_j2;   /* MPLS tunnel flags */
    int mpls_tunnel_label;      /* MPLS tunnel label */
    int ecmp_id;                /* ECMP ID */
    int ecmp_group_members_num; /* ECMP group members number */
    int svtag_lookup;           /* Perform an SVTAG lookup */
    bcm_gport_t tunnel_id;      /* Tunnel id of IP tunnel*/
    int svtag_lookup_mpls;      /* Perform an SVTAG lookup for mpls tunnel */
};

cint_ip_tunnel_basic_info_s cint_ip_tunnel_basic_info = {
    /*
     * eth_rif_intf_provider | eth_rif_intf_access
     */
    100, 15,
    /*
     * intf_access_mac_address             | tunnel_next_hop_mac
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x56}, {0x00, 0x02, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * access_vrf
     */
    1,
    /*
     * tunnel_fec_id
     */
    5000,
    /*
     * tunnel_arp_id
     */
       0,
    /*
     * route_ipv4_dip
     */
    0x7fffff03, /* 127.255.255.03 */
    /*
     * route_ipv4_mask
     */
    0xfffffff0,
    /*
     * tunnel_dip
     */
    0xA1000011, /* 161.0.0.17 */
    /*
     * tunnel_sip
     */
    0xA0000011, /* 160.0.0.17 */
    /*
     * tunnel_type
     */
    bcmTunnelTypeGreAnyIn4,
    /*
     * provider_vlan
     */
    100,
    /*
     * mpls_fwd_label
     */
    1456,
    /*
     * gre_lb_key_enable
     */
    0,
    /*
     * tunnel_ttl
     */
    56,
    /*
     * tunnel_dscp
     */
    10,
    /*
     * tunnel_intf
     */
    0,
    /*
     * route_ipv6_dip
     */
    {
     0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8,
     0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xec},
    /*
     * route_ipv6_mask
     */
    {
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    /*
     * mpls_tunnel_ttl
     */
    0x43,
    /*
     * mpls_tunnel_action
     */
    BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH,
    /*
     * mpls_tunnel_flags_j1
     */
    BCM_MPLS_EGRESS_LABEL_TTL_SET,
    /*
     * mpls_tunnel_flags_j2
     */
       BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT,
     /*
      * mpls_tunnel_label
      */
        200,
     /*
      * ecmp_id
      */
        50,
     /*
      * ecmp_group_members_num
      */
        1,
     /*
      * svtag_lookup
      */
        0,
     /*
      * tunnel_id
      */
      0,
      /*
       * svtag enable for mpls tunnel
       */
      0
 };

/*
 * Ports and LIFs Initialization
 * The function ip_tunnel_encapsulation_open_route_interfaces serves as a configuration template to
 * more advanced tests
 * Inputs: unit - relevant unit;
 *         access_port - incoming port;
 *         provider_port - outgoing port;
 *         *proc_name - main function name;
 */
int
ip_tunnel_encapsulation_open_route_interfaces(
    int unit,
    int access_port,
    int provider_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2\n", proc_name);
        return rv;
    }

    if (device_is_jericho2)
    {
        cint_ip_tunnel_basic_info.tunnel_fec_id = 40961;
    }

    /*
     * Configure ingress interfaces
     */
    rv = ip_tunnel_encapsulation_open_route_interfaces_access(unit, access_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_encapsulation_open_route_interfaces_access\n", proc_name);
        return rv;
    }

    /*
     * Configure egress interfaces
     */
    rv = ip_tunnel_encapsulation_open_route_interfaces_provider(unit, provider_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_encapsulation_open_route_interfaces_provider\n", proc_name);
        return rv;
    }

    /*
     * Create ARP entry and set its properties.
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_ip_tunnel_basic_info.tunnel_arp_id),
                                       cint_ip_tunnel_basic_info.tunnel_next_hop_mac,
                                       cint_ip_tunnel_basic_info.provider_vlan);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_encap__create\n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Configure Ingress interfaces
 * Inputs: access_port - incomming port;
 *         *proc_name - main function name;
 */
int
ip_tunnel_encapsulation_open_route_interfaces_access(
    int unit,
    int access_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, access_port, cint_ip_tunnel_basic_info.eth_rif_intf_access);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set, access_port\n", proc_name);
        return rv;
    }

    /*
     * Create routing interface for the routing's IP.
     * We use it as ingress ETH-RIF to perform ETH termination (my-mac procedure),
     * to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_access,
                             cint_ip_tunnel_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create, eth_rif_intf_access)\n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     */
    ingr_itf.intf_id = cint_ip_tunnel_basic_info.eth_rif_intf_access;
    ingr_itf.vrf = cint_ip_tunnel_basic_info.access_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set, eth_rif_intf_access\n", proc_name);
    }

    return rv;
}

/*
 * Configure Egress interfaces
 * Inputs: access_port - outgoing port;
 *         *proc_name - main function name;
 */
int
ip_tunnel_encapsulation_open_route_interfaces_provider(
    int unit,
    int provider_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set\n", proc_name);
        return rv;
    }

    /*
     * Create egress routing interface used for routing after the tunnel encapsulation.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_provider,
                             cint_ip_tunnel_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create\n", proc_name);
    }

    return rv;
}

/*
 * Main function for Gre4 IP tunnel encapsulation with MPLS forwarding scenario in J2
 * Inputs: unit - relevant unit;
 *         access_port - incoming port;
 *         provider_port - outgoing port;
 */
int
ip_tunnel_gre4_encap_with_mpls_fwd(
    int unit,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    proc_name = "ip_tunnel_gre4_encap_with_mpls_fwd";
    int GRE_PROTOCOL_TYPE_MPLS = 0x8847;


    bcm_if_t tunnel_intf;
    bcm_l3_egress_ecmp_t ecmp;
    cint_ip_tunnel_basic_info.gre_lb_key_enable = 0;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2\n");
        return rv;
    }

    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    rv = ip_tunnel_encapsulation_open_route_interfaces(unit, access_port, provider_port, *proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_encapsulation_open_route_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Create GRE IP tunnel
     */
    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.tunnel_arp_id, &tunnel_intf,
                                 GRE_PROTOCOL_TYPE_MPLS);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_initiator_create\n", proc_name);
        return rv;
    }

    bcm_if_t mpls_intf_id;
    if (!device_is_jericho2)
    {
        /*
         * Enable mpls forwarding
         */
        rv = add_mpls_route(unit, access_port, cint_ip_tunnel_basic_info.tunnel_fec_id,
                            cint_ip_tunnel_basic_info.tunnel_intf,
                            cint_ip_tunnel_basic_info.mpls_fwd_label, &mpls_intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, add_mpls_route\n", proc_name);
            return rv;
        }
        /*
         * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
         */
        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.tunnel_fec_id, tunnel_intf,
                                         0, provider_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
            return rv;
        }
    }
    else
    {
        /*
         * Add MPLS route to IP-Tunnel.
         */
        rv = mpls_swap_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label, tunnel_intf, &mpls_intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, mpls_swap_tunnel_create\n", proc_name);
            return rv;
        }
        /*
         * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
         */
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.tunnel_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.tunnel_fec_id, mpls_intf_id,
                             0, provider_port,
                            (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0)?BCM_L3_ENCAP_SPACE_OPTIMIZED:0);
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
                             cint_ip_tunnel_basic_info.tunnel_fec_id, BCM_L3_WITH_ID,
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
    }

    return rv;
}

/*
 * Main function for Gre4 IP tunnel encapsulation with Ipv6 forwarding scenario in J2
 * Inputs: unit          - relevant unit;
 *         access_port   - incoming port;
 *         provider_port - outgoing port;
 */
int
ip_tunnel_gre4_encap_with_ipv6_fwd(
    int unit,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    proc_name = "ip_tunnel_gre4_encap_with_ipv6_fwd";
    int GRE_PROTOCOL_TYPE_IPV6 = 0x86DD;


    bcm_if_t tunnel_intf;
    bcm_l3_egress_ecmp_t ecmp;
    cint_ip_tunnel_basic_info.gre_lb_key_enable = 0;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE) {
        printf ("Error, is_device_jericho2\n");
        return rv;
    }

    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    rv = ip_tunnel_encapsulation_open_route_interfaces(unit, access_port, provider_port, *proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_encapsulation_open_route_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Create GRE IP tunnel
     */
    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.tunnel_arp_id, &tunnel_intf,
                                 GRE_PROTOCOL_TYPE_IPV6);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_initiator_create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.tunnel_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.tunnel_fec_id, tunnel_intf,
                                     0, provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and add ipv4, ipv6 and mpls routes
     */
    rv = add_ipvx_routes(unit, tunnel_intf, access_port, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_ipvx_routes\n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Main function for Gre8 tunnel encapsulation of MPLSoETH packets scenario in J2
 * Inputs:
 * unit - relevant unit;
 * access_port - incoming port;
 * provider_port - outgoing port;
 */
int
ip_tunnel_gre8_lb_key_encap_with_mpls_fwd(
    int unit,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    proc_name = "ip_tunnel_gre8_lb_key_encap_with_mpls_fwd";
    int GRE_PROTOCOL_TYPE_MPLS = 0x8847;


    bcm_if_t tunnel_intf;
    bcm_l3_egress_ecmp_t ecmp;
    cint_ip_tunnel_basic_info.gre_lb_key_enable = 1;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE) {
        printf ("Error, is_device_jericho2\n");
        return rv;
    }

    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    rv = ip_tunnel_encapsulation_open_route_interfaces(unit, access_port, provider_port, *proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_encapsulation_open_route_interfaces\n", proc_name);
    }

    /*
     * Create GRE IP tunnel
     */
    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.tunnel_arp_id, &tunnel_intf,
                                 GRE_PROTOCOL_TYPE_MPLS);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_initiator_create\n", proc_name);
        return rv;
    }

    bcm_if_t mpls_intf_id;
    if (!device_is_jericho2)
    {
        /*
         * Add MPLS route to IP tunnel
         */
         rv = add_mpls_route(unit, access_port, cint_ip_tunnel_basic_info.tunnel_fec_id,
                             cint_ip_tunnel_basic_info.tunnel_intf,
                             cint_ip_tunnel_basic_info.mpls_fwd_label, &mpls_intf_id);
         if (rv != BCM_E_NONE)
         {
             printf("%s(): Error, add_mpls_route\n", proc_name);
             return rv;
         }
        /*
         * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
         */
        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.tunnel_fec_id, tunnel_intf,
                                         0, provider_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
            return rv;
        }
    }
    else
    {
        /*
         * Add MPLS route to IP-Tunnel.
         */
        rv = mpls_swap_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label, tunnel_intf, &mpls_intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, mpls_swap_tunnel_create\n", proc_name);
            return rv;
        }
        /*
         * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
         */

        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.tunnel_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }

        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.tunnel_fec_id,
                                         mpls_intf_id, 0, provider_port,
                             (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0)?BCM_L3_ENCAP_SPACE_OPTIMIZED:0);
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
                             cint_ip_tunnel_basic_info.tunnel_fec_id, BCM_L3_WITH_ID,
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
    }

    return rv;
}



/*
 * Main function for Gre8 tunnel encapsulation of IPvX packets scenarios in J2
 * Inputs:
 * unit - relevant unit;
 * access_port - incoming port;
 * provider_port - outgoing port;
 */
int
ip_tunnel_gre8_lb_key_encap_with_ipvx_fwd(
    int unit,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "ip_tunnel_gre8_lb_key_encap_with_ipvx_fwd";
    int GRE_PROTOCOL_TYPE_IPV4 = 0x0800;

    bcm_if_t tunnel_intf;
    cint_ip_tunnel_basic_info.gre_lb_key_enable = 1;

    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    rv = ip_tunnel_encapsulation_open_route_interfaces(unit, access_port, provider_port,
                                                       *proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_encapsulation_open_route_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Create GRE IP tunnel
     */
    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.tunnel_arp_id, &tunnel_intf,
                                 GRE_PROTOCOL_TYPE_IPV4);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_initiator_create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.tunnel_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.tunnel_fec_id, tunnel_intf,
                                     0, provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and add ipv4, ipv6 routes
     */
    rv = add_ipvx_routes(unit, tunnel_intf, access_port, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_ipvx_routes\n", proc_name);
        return rv;
    }

    return rv;
}



/*
 * Main function for MPLSoGRE4oIPv4 IP tunnel encapsulation of IPvX packets scenarios in J2
 * Inputs:
 *      unit - relevant unit;
 *      access_port - incoming port;
 *      provider_port - outgoing port;
 */
int
ip_tunnel_mpls_o_gre_encap_with_ipvx_fwd(
    int unit,
    int access_port,
    int provider_port,
    int lb_key_en)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "ip_tunnel_mpls_o_gre4_encap_with_ipvx_fwd";
    int GRE_PROTOCOL_TYPE_MPLS = 0x8847;


    bcm_if_t gre_tunnel_intf;
    bcm_if_t mpls_tunnel_intf;

    if (lb_key_en !=0)
    {
        cint_ip_tunnel_basic_info.gre_lb_key_enable = 1;
    }

    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    rv = ip_tunnel_encapsulation_open_route_interfaces(unit, access_port, provider_port,
                                                       *proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_encapsulation_open_route_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Create GRE IP tunnel
     */
    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.tunnel_arp_id, &gre_tunnel_intf,
                                 GRE_PROTOCOL_TYPE_MPLS);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_initiator_create\n", proc_name);
        return rv;
    }

    /*
     * Create MPLS tunnel inititator pointing to previously created GRE tunnel
     */
    rv = mpls_tunnel_encap_create(unit, gre_tunnel_intf, &mpls_tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_tunnel_encap_create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.tunnel_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.tunnel_fec_id, mpls_tunnel_intf,
                                     0, provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and add ipv4, ipv6 and mpls routes
     */
    rv = add_ipvx_routes(unit, mpls_tunnel_intf, access_port, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_ipvx_routes\n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Add IPv4 and IPv6 routing entries
 * Parameters: tunnel_intf - tunnel interface pointing to the tunnel's route entry
 *             access_port - incomming port
 *             provider_port - outgoing port
 */
int
add_ipvx_routes(
    int unit,
    bcm_if_t tunnel_intf,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    proc_name = "gre8_with_lb_key_encap_add_ipvx_routes";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2\n");
        return rv;
    }

    /*
     * Add IPv4 and IPv6 routes to IP tunnel
     */
    if (device_is_jericho2)
    {
        int fec_id;
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.tunnel_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }

        BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ip_tunnel_basic_info.tunnel_fec_id);
        /*
         * IPv4
         */
        rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_ipv4_dip,
                           cint_ip_tunnel_basic_info.access_vrf, fec_id, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_host_ipv4\n");
            return rv;
        }
        /*
         * IPv6
         */
        rv = add_host_ipv6(unit, cint_ip_tunnel_basic_info.route_ipv6_dip,
                           cint_ip_tunnel_basic_info.access_vrf, fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_host_ipv6\n");
            return rv;
        }
    }
    else
    {
        /*
         * IPv4
         */
        rv = add_ipv4_route(unit, cint_ip_tunnel_basic_info.tunnel_fec_id,
                            cint_ip_tunnel_basic_info.route_ipv4_dip,
                            cint_ip_tunnel_basic_info.route_ipv4_mask, cint_ip_tunnel_basic_info.access_vrf);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, add_ipv4_route\n", proc_name);
            return rv;
        }
        /*
         * IPv6
         */
        rv = add_ipv6_route(unit, cint_ip_tunnel_basic_info.tunnel_fec_id,
                            cint_ip_tunnel_basic_info.route_ipv6_dip,
                            cint_ip_tunnel_basic_info.route_ipv6_mask, cint_ip_tunnel_basic_info.access_vrf);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, add_ipv6_route\n", proc_name);
            return rv;
        }
    }

    return rv;
}

/* Create IP tunnel
 * Parameters: arp_itf - tunnel ARP interface Id
 *             *tunnel_intf - tunnel interface
 *             gre_protocol_type - configure global GRE.protocol (when not IPvX or Eth)
 *             is_gre_with_lb_key - boolean indicating GRE8 with lb key
 */
int
tunnel_initiator_create(
    int unit,
    bcm_if_t arp_itf,
    bcm_if_t * tunnel_intf,
    int next_protocol_type)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_info;
    bcm_l3_intf_t l3_intf;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);

    if (!device_is_jericho2)
    {
        rv = bcm_switch_control_set(unit, bcmSwitchL2GreProtocolType, next_protocol_type);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_switch_control_set\n");
            return rv;
        }
    }

    /* Create IP tunnel initiator for encapsulating GRE4oIPv4 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_info);
    tunnel_info.dip        = cint_ip_tunnel_basic_info.tunnel_dip;
    tunnel_info.sip        = cint_ip_tunnel_basic_info.tunnel_sip;

    tunnel_info.flags  = 0;

    if (cint_ip_tunnel_basic_info.gre_lb_key_enable) {
        tunnel_info.flags  |= BCM_TUNNEL_INIT_GRE_KEY_USE_LB;
    }

    if (cint_ip_tunnel_basic_info.svtag_lookup) {
        tunnel_info.flags  |= BCM_TUNNEL_INIT_SVTAG_ENABLE;
    }
    tunnel_info.dscp       = cint_ip_tunnel_basic_info.tunnel_dscp;
    tunnel_info.type       = cint_ip_tunnel_basic_info.tunnel_type;
    tunnel_info.ttl        = cint_ip_tunnel_basic_info.tunnel_ttl;
    tunnel_info.l3_intf_id = arp_itf;
    if (device_is_jericho2)
    {
        tunnel_info.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        tunnel_info.encap_access = bcmEncapAccessTunnel4;
    }
    else
    {
         /*In Jr2, vlan is an ARP property */
        tunnel_info.dscp_sel = bcmTunnelDscpAssign;
        tunnel_info.vlan   = cint_ip_tunnel_basic_info.provider_vlan;
    }
    tunnel_info.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;

    bcm_l3_intf_t_init(&l3_intf);
    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }

    *tunnel_intf = l3_intf.l3a_intf_id;
    cint_ip_tunnel_basic_info.tunnel_id = l3_intf.l3a_intf_id;

    return rv;
}

/*
 * Create MPLS tunnel
 * Parameters: out_intf_ids - MPLS tunnel will point to this intf
 *             mpls_intf_id - interface id of the created MPLS tunnel
 */
int
mpls_tunnel_encap_create(
    int unit,
    bcm_if_t out_intf_id,
    bcm_if_t * mpls_intf_id)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2\n");
        return rv;
    }

    bcm_mpls_egress_label_t label;
    label.label = cint_ip_tunnel_basic_info.mpls_tunnel_label;
    label.ttl = cint_ip_tunnel_basic_info.mpls_tunnel_ttl;
    if (device_is_jericho2)
    {
        label.flags = cint_ip_tunnel_basic_info.mpls_tunnel_flags_j2;
        if (cint_ip_tunnel_basic_info.mpls_tunnel_flags_j2 & BCM_MPLS_EGRESS_LABEL_TTL_SET)
        {
            label.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
            label.flags &= ~BCM_MPLS_EGRESS_LABEL_TTL_SET;
        }
        if (cint_ip_tunnel_basic_info.mpls_tunnel_flags_j2 & BCM_MPLS_EGRESS_LABEL_EXP_SET)
        {
            label.egress_qos_model.egress_qos= bcmQosEgressModelPipeNextNameSpace;
            label.flags &= ~BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
        label.action = 0;
    }
    else
    {
        label.flags = cint_ip_tunnel_basic_info.mpls_tunnel_flags_j1;
        label.action = cint_ip_tunnel_basic_info.mpls_tunnel_action;
    }
    if (out_intf_id != -1)
    {
        label.l3_intf_id = out_intf_id;
    }

    if (cint_ip_tunnel_basic_info.svtag_lookup_mpls) {
        label.flags2 |= BCM_MPLS_EGRESS_LABEL_FLAGS2_SVTAG_ENABLE;
    }

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, &label);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *mpls_intf_id = label.tunnel_id;

    return rv;
}

/* add MPLS route
 * Parameters: in_port - in port
 *             intf_id - routing FEC
               tunnel_intf - RIF or TUNNEL interface
 *             mpls_label - mpls forwarding label
 */
int
add_mpls_route(
    int unit,
    int in_port,
    bcm_if_t intf_id,
    bcm_if_t tunnel_intf,
    int mpls_label,
    bcm_if_t * mpls_intf_id)
{
    int rv = BCM_E_NONE;
    bcm_mpls_label_t label_val;
    label_val = mpls_label;
    int device_is_jericho2;

    bcm_mpls_egress_label_t label;
    bcm_mpls_egress_label_t_init(&label);

    label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label.label = label_val;
    label.action = BCM_MPLS_EGRESS_ACTION_SWAP;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, &label);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *mpls_intf_id = label.tunnel_id;

    bcm_mpls_tunnel_switch_t tun_switch;
    bcm_mpls_tunnel_switch_t_init(&tun_switch);

    tun_switch.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    tun_switch.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    tun_switch.egress_if = intf_id;
    tun_switch.port = in_port;
    tun_switch.egress_label = label;

    tun_switch.label = label_val;

    rv = bcm_mpls_tunnel_switch_create(unit, tun_switch);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    return rv;
}

/* add IPv4 route
 * Parameters: intf_id - routing FEC
 *             forwarding_dip - destination IP address
 *             vrf - Out-Eth-RIF VRF
 */
int
add_ipv4_route(
    int unit,
    bcm_if_t intf_id,
    bcm_ip_t forwarding_dip,
    int forwarding_mask,
    int vrf)
{
    int rv = BCM_E_NONE;

    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_flags = BCM_L3_RPF;
    l3rt.l3a_subnet = forwarding_dip;
    l3rt.l3a_ip_mask = forwarding_mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf_id;
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;

    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_route_add\n");
        return rv;
    }

    return rv;
}

/* add IPv6 route
 * Parameters: intf_id - routing FEC
 *             forwarding_dip - ipv6 forwarding address
 *             forwarding_mask - ipv6 mask
 *             vrf - Out-Eth-RIF VRF
 */
int
add_ipv6_route(
    int unit,
    bcm_if_t intf_id,
    bcm_ip6_t forwarding_dip,
    bcm_ip6_t forwarding_mask,
    int vrf)
{
    int rv = BCM_E_NONE;

    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_flags = BCM_L3_IP6;
    l3rt.l3a_ip6_net = forwarding_dip;
    l3rt.l3a_ip6_mask = forwarding_mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf_id;
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;
    if (soc_property_get(unit, "enhanced_fib_scale_prefix_length", 0))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_SCALE_ROUTE;
    }

    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_route_add\n");
        return rv;
    }

    return rv;
}

/*
 *  ###############################################################################################################################
 *  Receiving packet on access port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         ||   MPLS LABEL    ||
 *   |    |00:0C:00:02:00:56||00:50:00:00:12:34||     1456        ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Receiving packet on provider port == 203:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         || IPv4 tunnel SIP || IPv4 tunnel DIP ||  UDP Dest Port  ||   MPLS LABEL    ||
 *   |    |00:02:00:00:CD:1D||00:0C:00:02:00:56||    161.0.0.17   ||    160.0.0.17   || 0x6635          ||     1456        ||
 *        |                 ||                 ||                 ||                 ||                 ||                 ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ################################################################################################################################
 *
 * Main function for UDP tunnel encapsulation with MPLS forwarding test plan scenario in Jericho 2
 * Inputs:
 * unit - relevant unit;
 * access_port - incoming port;
 * provider_port - outgoing port;
 */
int
ip_tunnel_udp_encap_mpls_forwarding(
    int unit,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    proc_name = "ip_tunnel_udp_encap_ip_forwarding";
    int GRE_PROTOCOL_TYPE_MPLS = 0x8847;


    bcm_if_t tunnel_intf;
    bcm_l3_egress_ecmp_t ecmp;
    cint_ip_tunnel_basic_info.tunnel_type = bcmTunnelTypeUdp;


    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2\n");
        return rv;
    }

    /*
     * Initialize ports, LIFs, Eth-RIFs etc
     */
    rv = ip_tunnel_encapsulation_open_route_interfaces(unit, access_port, provider_port,
                                                       *proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_encapsulation_open_route_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Create UDP IP tunnel
     */
    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.tunnel_arp_id, &tunnel_intf,
                                 GRE_PROTOCOL_TYPE_MPLS);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_initiator_create\n", proc_name);
        return rv;
    }

    bcm_if_t mpls_intf_id;
    if (!device_is_jericho2)
    {
        /*
         * Enable mpls forwarding
         */
        rv = add_mpls_route(unit, access_port, cint_ip_tunnel_basic_info.tunnel_fec_id,
                            cint_ip_tunnel_basic_info.tunnel_intf,
                            cint_ip_tunnel_basic_info.mpls_fwd_label, &mpls_intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, add_mpls_route\n", proc_name);
            return rv;
        }
        /*
         * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
         */
        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.tunnel_fec_id, tunnel_intf,
                                         0, provider_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
            return rv;
        }
    }
    else
    {
        /*
         * Add MPLS route to IP-Tunnel.
         */
        rv = mpls_swap_tunnel_create(unit, cint_ip_tunnel_basic_info.mpls_fwd_label, tunnel_intf, &mpls_intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, mpls_swap_tunnel_create\n", proc_name);
            return rv;
        }
        /*
         * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
         */
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.tunnel_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.tunnel_fec_id, mpls_intf_id,
                                         0, provider_port,
                                         (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0)?BCM_L3_ENCAP_SPACE_OPTIMIZED:0);
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
                             cint_ip_tunnel_basic_info.tunnel_fec_id, BCM_L3_WITH_ID,
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
    }

    return rv;
}

/*
 *  ##################################################################################################################################################
 *  Receiving packet on access port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         ||   IPV4 route DIP    ||   IPV4 route SIP    ||
 *   |    |00:0C:00:02:00:56||00:50:00:00:12:34||   127.255.255.03    ||     192.168.0.1     ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Receiving packet on provider port == 203:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         || IPv4 tunnel SIP || IPv4 tunnel DIP ||  UDP Dest Port  ||   IPV4 route DIP    ||   IPV4 route SIP    ||
 *   |    |00:02:00:00:CD:1D||00:0C:00:02:00:56||    161.0.0.17   ||    160.0.0.17   ||     0x6605      ||   127.255.255.03    ||     192.168.0.1     ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
  *  Receiving packet on access port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         ||                  IPV6 route DIP              ||               IPV6 route SIP            ||
 *   |    |00:0C:00:02:00:56||00:50:00:00:12:34||   e1e2:e3e4:e5e6:e7e8:e9ea:ebec:edee:efec    || 3ffe:0000:0000:0000:fa4f:57ff:fe72:0000 ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Receiving packet on provider port == 203:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         || IPv4 tunnel SIP || IPv4 tunnel DIP ||  UDP Dest Port  ||                  IPV6 route DIP            ||
 *   |    |00:02:00:00:CD:1D||00:0C:00:02:00:56||    161.0.0.17   ||    160.0.0.17   ||     0x6615      ||   e1e2:e3e4:e5e6:e7e8:e9ea:ebec:edee:efec  ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  ####################################################################################################################################################
 * Main function for UDP tunnel encapsulation with IPvX forwarding test plan scenario in Jericho 2
 * Inputs:
 * unit - relevant unit;
 * access_port - incoming port;
 * provider_port - outgoing port;
 */
int
ip_tunnel_udp_encap_ip_forwarding(
    int unit,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "ip_tunnel_udp_encap_ip_forwarding";
    int GRE_PROTOCOL_TYPE_IPV4 = 0x0800;

    bcm_if_t tunnel_intf;
    cint_ip_tunnel_basic_info.tunnel_type = bcmTunnelTypeUdp;

    rv = ip_tunnel_encapsulation_open_route_interfaces(unit, access_port, provider_port, *proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_encapsulation_open_route_interfaces\n", proc_name);
        return rv;
    }

    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.tunnel_arp_id, &tunnel_intf,
                                 GRE_PROTOCOL_TYPE_IPV4);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_initiator_create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.tunnel_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }

    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.tunnel_fec_id, tunnel_intf,
                                     0, provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and add ipv4, ipv6 and mpls routes
     */
    rv = add_ipvx_routes(unit, tunnel_intf, access_port, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_ipvx_routes\n", proc_name);
        return rv;
    }

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
        printf("Error, bcm_mpls_tunnel_initiator_create\n");
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
        printf("Error, bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    return rv;
}



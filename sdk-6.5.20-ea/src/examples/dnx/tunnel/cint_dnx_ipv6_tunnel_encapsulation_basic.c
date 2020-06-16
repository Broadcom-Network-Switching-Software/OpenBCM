
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_ip_tunnel_encap_basic.c Purpose: Various examples for IPTunnels.
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
 * cint ../../../../src/examples/sand/cint_dnx_ipv6_tunnel_encapsulation_basic.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int provider_port = 201;
 * int access_port = 200;
 * rv = ipv6_basic_tunnel_encapsulation(unit,access_port,provider_port);
 * print rv;
 *
 *
 *  Scenarios configured in this cint:
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) Basic IPv6 Tunnel encapsulation
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  A GRE4oIPv6oETH tunnel header is being encapsulated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ###############################################################################################################################
 *  Receiving packet on access port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         ||      IPv4 SIP     ||      IPv4 DIP      ||
 *   |    |00:0C:00:02:00:56||00:50:00:00:12:34||       1.2.3.4     ||    127.255.255.03  ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Sending packet on provider port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-++-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+++
 *   |    |      DA         ||      SA         ||           IPv6 tunnel SIP                   ||               IPv6 tunnel DIP               ||  GRE  ||  IPv4 SIP   ||   IPv4 DIP     ||
 *   |    |00:02:00:00:CD:1D||00:0C:00:02:00:56||  ECEF:EEED:ECEB:EAE9:E8E7:E6E5:E4E3:E2E1    ||    2001:0DB8:0000:0000:0211:22FF:FE33:4455  ||   4   ||   1.2.3.4   || 127.255.255.03 ||
 *   |    |                 ||                 ||                                             ||                                             ||  800  || 01.02.03.04 ||  7f.ff.ff.03   ||
 *   |    +-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-++-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+++
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  ###############################################################################################################################
 */
struct cint_ipv6_tunnel_basic_info_s
{
    int eth_rif_intf_provider;            /* out RIF */
    int eth_rif_intf_access;              /* in RIF */
    bcm_mac_t intf_access_mac_address;    /* mac for in RIF */
    bcm_mac_t tunnel_next_hop_mac;        /* mac for next hop */
    int access_vrf;                       /* VRF, resolved as Tunnel-InLif property*/
    int ipv6_tunnel_id;
    int tunnel_fec_id;                    /* FEC id */
    int tunnel_arp_id;                    /* Id for ARP entry */
    bcm_ip_t    route_ipv4_dip;                   /* Route DIP */
    uint32      route_ipv4_dip_mask;                /* mask for route DIP */
    bcm_ip6_t    tunnel_ipv6_dip;                  /* tunnel IPv6 DIP */
    bcm_ip6_t    tunnel_ipv6_sip;                  /* tunnel IPv6 SIP */
    bcm_tunnel_type_t tunnel_type;        /* tunnel type */
    int provider_vlan;                    /* VID assigned to the outgoing packet */
    int tunnel_ttl;                       /* Ip tunnel header TTL */
    int tunnel_dscp;                      /* Ip tunnel header differentiated services code point */
    int gre4_lb_key_enable;               /* Enable LB-Key on GRE IP tunnel */
    int svtag_enable;                     /* enable SVTAG lookup */
};

cint_ipv6_tunnel_basic_info_s cint_ipv6_tunnel_basic_info =
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
     * ipv6_tunnel_id
     */
       0,
    /*
     * tunnel_fec_id
     */
       5000,
    /*
     * tunnel_arp_id
     */
       9000,
    /*
     * route_ipv4_dip
     */
       0x7fffff03 /* 127.255.255.03 */,
    /*
     * route_ipv4_dip_mask
     */
       0xfffffff0,
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


int ipv6_basic_tunnel_initiator_create(
    int unit,
    bcm_if_t arp_id,
    bcm_if_t *tunnel_inlif,
    int is_stat_pp)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);

    /* Create IPv6 tunnel initiator for encapsulating GRE4oIPv6 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip6            = cint_ipv6_tunnel_basic_info.tunnel_ipv6_dip;
    tunnel_init.sip6            = cint_ipv6_tunnel_basic_info.tunnel_ipv6_sip;
    if (is_stat_pp) {
        tunnel_init.flags           = BCM_TUNNEL_INIT_STAT_ENABLE;
    }
    tunnel_init.dscp            = cint_ipv6_tunnel_basic_info.tunnel_dscp;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.type            = cint_ipv6_tunnel_basic_info.tunnel_type;
    tunnel_init.ttl             = cint_ipv6_tunnel_basic_info.tunnel_ttl;
    tunnel_init.encap_access    = bcmEncapAccessTunnel2;
    tunnel_init.l3_intf_id      = arp_id;

    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_create \n");
    }

    *tunnel_inlif = l3_intf.l3a_intf_id;
    cint_ipv6_tunnel_basic_info.ipv6_tunnel_id = *tunnel_inlif;

    return rv;
}

int ipv6_basic_tunnel_encapsulation(int unit, int access_port, int provider_port, int is_stat_pp)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    bcm_if_t tunnel_inlif;
    l3_ingress_intf ingr_inlif;
    l3_ingress_intf_init(&ingr_inlif);

    proc_name = "ipv6_basic_tunnel_encapsulation";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: is_device_jericho2 device_is_jericho2 \n", proc_name);
        return rv;
    }

    cint_ipv6_tunnel_basic_info.tunnel_arp_id = 5050;
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ipv6_tunnel_basic_info.tunnel_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, access_port, cint_ipv6_tunnel_basic_info.eth_rif_intf_access);
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
    rv = intf_eth_rif_create(unit, cint_ipv6_tunnel_basic_info.eth_rif_intf_access, cint_ipv6_tunnel_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: intf_eth_rif_create failed to execute! \n", proc_name);
        return rv;
    }

    /*
     * Create egress routing interface used for routing after the tunnel encapsulation.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_ipv6_tunnel_basic_info.eth_rif_intf_provider, cint_ipv6_tunnel_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create failed to execute! \n", proc_name);
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     */
    ingr_inlif.intf_id = cint_ipv6_tunnel_basic_info.eth_rif_intf_access;
    ingr_inlif.vrf = cint_ipv6_tunnel_basic_info.access_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_inlif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set eth_rif_intf_access\n", proc_name);
    }

    /*
     * Create ARP entry and set its properties.
     */
    rv = l3__egress_only_encap__create(unit, 0,&(cint_ipv6_tunnel_basic_info.tunnel_arp_id), cint_ipv6_tunnel_basic_info.tunnel_next_hop_mac,
            cint_ipv6_tunnel_basic_info.provider_vlan);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel initiator
     * and set next-lif-pointer to the ARP entry
     */
    rv = ipv6_basic_tunnel_initiator_create(unit, cint_ipv6_tunnel_basic_info.tunnel_arp_id, &tunnel_inlif, is_stat_pp);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ipv6_basic_tunnel_initiator_create failed to execute! \n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    rv = l3__egress_only_fec__create(unit, cint_ipv6_tunnel_basic_info.tunnel_fec_id, tunnel_inlif, 0, provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create failed to execute! \n", proc_name);
    }

    /*
     * Add Host entry
     */
    int fec_id;
    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ipv6_tunnel_basic_info.tunnel_fec_id);
    rv = add_host_ipv4(unit, cint_ipv6_tunnel_basic_info.route_ipv4_dip, cint_ipv6_tunnel_basic_info.access_vrf, fec_id, 0,0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_host_ipv4 failed to execute! \n", proc_name);
        return rv;
    }

    return rv;

}

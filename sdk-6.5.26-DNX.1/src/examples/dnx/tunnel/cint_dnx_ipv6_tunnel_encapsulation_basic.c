
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_dnx_ipv6_tunnel_encapsulation_basic.c Purpose: Various examples for IPTunnels.
 */

/*
 * Test Scenario - start
 *
 * ./bcm.user
 * cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../../../src/examples/dnx/tunnel/cint_dnx_ipv6_tunnel_encapsulation_basic.c
 * cint
 * ipv6_basic_tunnel_encapsulation(0,201,202);
 * exit;
 *
 * // Ipv4oETH0
 * tx 1 psrc=201 data=0x000c00020056005000001234080045000035000000008000a7b80a0a0a047fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * // Received packets on unit 0 should be:
 * // Source port: 0, Destination port: 0
 * // Data: 0x00020000cd1d000c000200568100a06486dd60a36c7300392f40ecefeeedecebeae9e8e7e6e5e4e3e2e120010db800000000021122fffe3344550000080045000035000000007f00a8b80a0a0a047fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Test Scenario - end
*/

struct cint_ipv6_tunnel_basic_info_s
{
    int eth_rif_intf_provider;          /* out RIF */
    int eth_rif_intf_access;            /* in RIF */
    bcm_mac_t intf_access_mac_address;  /* mac for in RIF */
    bcm_mac_t tunnel_next_hop_mac;      /* mac for next hop */
    int access_vrf;                     /* VRF, resolved as Tunnel-InLif property*/
    int ipv6_tunnel_id; 
    int tunnel_fec_id;                  /* FEC id */
    int tunnel_arp_id;                  /* Id for ARP entry */
    bcm_ip_t route_ipv4_dip;            /* Route DIP */
    uint32 route_ipv4_dip_mask;         /* mask for route DIP */
    bcm_ip6_t tunnel_ipv6_dip;          /* tunnel IPv6 DIP */
    bcm_ip6_t tunnel_ipv6_sip;          /* tunnel IPv6 SIP */
    bcm_tunnel_type_t tunnel_type;      /* tunnel type */
    int provider_vlan;                  /* VID assigned to the outgoing packet */
    int tunnel_ttl;                     /* Ip tunnel header TTL */
    int tunnel_dscp;                    /* Ip tunnel header differentiated services code point */
    int gre4_lb_key_enable;             /* Enable LB-Key on GRE IP tunnel */
    int svtag_enable;                   /* enable SVTAG lookup */
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
    bcm_if_t *tunnel_inlif)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);

    /* Create IPv6 tunnel initiator for encapsulating GRE4oIPv6 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip6            = cint_ipv6_tunnel_basic_info.tunnel_ipv6_dip;
    tunnel_init.sip6            = cint_ipv6_tunnel_basic_info.tunnel_ipv6_sip;
    tunnel_init.flags           = BCM_TUNNEL_INIT_STAT_ENABLE;
    tunnel_init.dscp            = cint_ipv6_tunnel_basic_info.tunnel_dscp;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.type            = cint_ipv6_tunnel_basic_info.tunnel_type;
    tunnel_init.ttl             = cint_ipv6_tunnel_basic_info.tunnel_ttl;
    tunnel_init.encap_access    = bcmEncapAccessTunnel2;
    tunnel_init.l3_intf_id      = arp_id;
    if(cint_ipv6_tunnel_basic_info.svtag_enable)
    {
        tunnel_init.flags |= BCM_TUNNEL_INIT_SVTAG_ENABLE;
    }

    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_create \n");
    }

    *tunnel_inlif = l3_intf.l3a_intf_id;
    cint_ipv6_tunnel_basic_info.ipv6_tunnel_id = *tunnel_inlif;

    return rv;
}

int ipv6_basic_tunnel_encapsulation(int unit, int access_port, int provider_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    bcm_if_t tunnel_inlif;
    l3_ingress_intf ingr_inlif;
    l3_ingress_intf_init(&ingr_inlif);

    proc_name = "ipv6_basic_tunnel_encapsulation";

    cint_ipv6_tunnel_basic_info.tunnel_arp_id = 5050;
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_ipv6_tunnel_basic_info.tunnel_fec_id);
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
    rv = ipv6_basic_tunnel_initiator_create(unit, cint_ipv6_tunnel_basic_info.tunnel_arp_id, &tunnel_inlif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ipv6_basic_tunnel_initiator_create failed to execute! \n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    rv = l3__egress_only_fec__create(unit, cint_ipv6_tunnel_basic_info.tunnel_fec_id, tunnel_inlif, 0, provider_port, 0, fwd_flag);
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

    rv = add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, cint_ipv6_tunnel_basic_info.access_vrf, fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in add_host_ipv6\n", proc_name);
    }

    return rv;

}

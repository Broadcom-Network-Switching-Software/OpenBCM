/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_dnx_ip_route_vrf_redirect.c Purpose: examples for vrf redirect.
 */

/* 
 * Vrf redirect
 * 
 * 
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_util_rch.c
 * cint ../../src/./examples/dnx/cint_dnx_ip_route_vrf_redirect.c
 * cint
 * ip_route_vrf_redirect_main(0,201,203,40);
 * exit;
 *
 * tx 1 psrc=201 data=0x000c00020056005000001234080045000035000000008000b7c0010203047fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be: 
 * Data: 0x00020000cd1d000c0002005681000064080045000035000000007f00b8c0010203047fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 */
struct cint_ip_tunnel_basic_info_s
{
    int eth_rif_intf_provider;            /* out RIF */
    int eth_rif_intf_access;              /* in RIF */
    bcm_mac_t intf_access_mac_address;    /* mac for in RIF */
    bcm_mac_t tunnel_next_hop_mac;        /* mac for next hop */
    int access_vrf;                       /* VRF, resolved as Tunnel-InLif property*/
    int arp_fec_id;                       /* FEC id */
    int arp_id;                           /* Id for ARP entry */
    bcm_ip_t route_dip;                   /* Route DIP */
    int provider_vlan;                    /* VID assigned to the outgoing packet */
};

cint_ip_tunnel_basic_info_s cint_ip_tunnel_basic_info =
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
     * access_vrf
     */
       1,
    /*
     * arp_fec_id      
     */
       45000,
    /*
     * arp_id
     */
       0,
    /*
     * route_dip
     */
       0x7fffff03 /* 127.255.255.03 */,
    /*
     * tunnel type
     */
       bcmTunnelTypeGreAnyIn4,
    /*
     * provider vlan
     */
       100
};


/*
 * Main function for ip routing with vrf redirect in Jericho 2
 * The function ip_route_vrf_redirect_main implements ip routing to recycle port and recycle outlif at 1st pass, 
 * The recycle outlif updates the VRF with a new value. 
 * At 2nd pass, perform ip routing according to the new vrf.
 * Inputs:
 * unit - relevant unit;
 * access_port - incoming port;
 * provider_port - outgoing port; 
 * recycle_port - recycle port 
 * Note that l3__egress_only_encap__create() is always invoked with flags set
 * to not NOT load GLEM when creating ARP entry
 */
int
ip_route_vrf_redirect_main(
    int unit,
    int access_port,
    int provider_port, 
    int recycle_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    l3_ingress_intf ingr_itf;
    uint32 allocation_flags;
    int fec;

    l3_ingress_intf_init(&ingr_itf);
    allocation_flags = 0;
    proc_name = "ip_tunnel_encap_basic";

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    cint_ip_tunnel_basic_info.arp_fec_id       += fec;

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, access_port, cint_ip_tunnel_basic_info.eth_rif_intf_access);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set\n", proc_name);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set provider_port\n", proc_name);
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
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_access\n", proc_name);
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
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_provider\n", proc_name);
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     */
    ingr_itf.intf_id = cint_ip_tunnel_basic_info.eth_rif_intf_access;
    ingr_itf.vrf = cint_ip_tunnel_basic_info.access_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set eth_rif_intf_access\n", proc_name);
    }

    rv = l3__egress_only_encap__create(unit, allocation_flags,&(cint_ip_tunnel_basic_info.arp_id),
                                       cint_ip_tunnel_basic_info.tunnel_next_hop_mac,
                                       cint_ip_tunnel_basic_info.provider_vlan);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n", proc_name);
        return rv;
    }


    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.arp_fec_id      , cint_ip_tunnel_basic_info.eth_rif_intf_provider,
                                     cint_ip_tunnel_basic_info.arp_id, provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
    }
 
    int recycle_entry_encap_id;
    bcm_vrf_t new_vrf = 11;
    recycle_entry_params_s recycle_entry_params;
    recycle_entry_params.recycle_app = bcmL2EgressRecycleAppRedirectVrf;
    recycle_entry_params.vrf_redirect_params.vrf = new_vrf;
    rv = create_vrf_redirect_recycle_entry(unit, recycle_entry_params, recycle_entry_encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create_vrf_redirect_recycle_entry\n", proc_name);
    }

    /*
     * Add Host entry for 1st pass (go to recycle port)
     */
    rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_dip,
                       cint_ip_tunnel_basic_info.access_vrf, 0, recycle_entry_encap_id, recycle_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4, \n");
        return rv;
    }

    /* Add host entry for 2nd pass (do routing) */
    int fec_id;
    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ip_tunnel_basic_info.arp_fec_id      );
    rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_dip,
                       new_vrf, fec_id, 0,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4, \n");
        return rv;
    }

    return rv;
}

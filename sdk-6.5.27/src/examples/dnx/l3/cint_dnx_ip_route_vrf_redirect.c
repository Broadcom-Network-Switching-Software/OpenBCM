/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_dnx_ip_route_vrf_redirect.c Purpose: examples for vrf redirect.
 */

/* 
 * Vrf redirect
 * At 1st pass the packet will be routed to recycle port and RCH outlif, its VRF will be updated with the VRF value from the RCH EEDB entry.
 * At 2nd pass the routing will be performed using the new VRF.
 * 
 * Test Scenario 
 *
 * Test Scenario - start
  config add tm_port_header_type_in_40=RCH_0
 * or IBCH1_MODE, if supported
  config add tm_port_header_type_out_40=ETH
  config add ucode_port_40=RCY.1:core_0.40
  tr 141
  cint ../../../../src/examples/sand/cint_ip_route_basic.c
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_util_rch.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_vrf_redirect.c
  cint
  ip_route_vrf_redirect_main(0,201,203,40);
  exit;
 *
  tx 1 psrc=201 data=0x000c00020056005000001234080045000035000000008000b7c0010203047fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be: 
 * Data: 0x00020000cd1d000c0002005681000064080045000035000000007f00b8c0010203047fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 * Test Scenario - end
 */
struct cint_ip_tunnel_basic_info_s
{
    int eth_rif_intf_provider;            /* out RIF */
    int eth_rif_intf_access;              /* in RIF */
    bcm_mac_t intf_access_mac_address;    /* mac for in RIF */
    bcm_mac_t tunnel_next_hop_mac;        /* mac for next hop */
    int access_vrf;                       /* VRF, resolved as Tunnel-InLif property*/
    int redirected_vrf;                          /* VRF, resolved at 1st pass, used as VRF at 2nd pass. This is the VRF we redirect to  */
    int arp_fec_id;                       /* FEC id */
    int arp_id;                           /* Id for ARP entry */
    bcm_ip_t route_dip;                   /* Route DIP */
    bcm_ip_t route_dip_mask;                   /* Route DIP mask */
    bcm_ip6_t route_dip6;                 /* Route ipv6 DIP */
    bcm_ip6_t route_dip6_mask;            /* Route ipv6 DIP mask */
    int provider_vlan;                    /* VID assigned to the outgoing packet */
    int recycle_entry_encap_id;           /* recycle entry encap id */
    int recycle_entry_fec_id;             /* FEC id, contains the recycle entry and the recycle port */
    int control_lif_short_pipe;           /* control lif configured with short pipe */
    int control_lif_uniform;              /* control lif configured with uniform */

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
     * redirected_vrf
     */
       11,
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
     * route dip mask
     */
       0xffffffff /* 255.255.255.255 */,
    /*
     * route ipv6 dip
     */
      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 },
    /*
     * route ipv6 dip mask
     */
      { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
    /*
     * provider vlan
     */
       100,
    /* recycle_entry_encap_id */
       0,
    /* recycle entry fec id */
       46000,
    /* control lif short pipe */
       0,
       /* control lif uniform */
       0
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
    char *proc_name = "ip_route_vrf_redirect_main";
    l3_ingress_intf ingr_itf;
    uint32 allocation_flags;
    int fec_id;

    l3_ingress_intf_init(&ingr_itf);
    allocation_flags = 0;

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec_id), "");

    cint_ip_tunnel_basic_info.arp_fec_id += fec_id;

    cint_ip_tunnel_basic_info.recycle_entry_fec_id += fec_id;

    /*
     * Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, access_port, cint_ip_tunnel_basic_info.eth_rif_intf_access), "");

    /*
     * Set Out-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, provider_port), "provider_port");

    /*
     * Create routing interface for the routing's IP.
     * We use it as ingress ETH-RIF to perform ETH termination (my-mac procedure),
     * to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_access,
                             cint_ip_tunnel_basic_info.intf_access_mac_address), "eth_rif_intf_access");

    /*
     * Create egress routing interface used for routing after the tunnel encapsulation.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_provider,
                             cint_ip_tunnel_basic_info.intf_access_mac_address), "eth_rif_intf_provider");

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     */
    ingr_itf.intf_id = cint_ip_tunnel_basic_info.eth_rif_intf_access;
    ingr_itf.vrf = cint_ip_tunnel_basic_info.access_vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "eth_rif_intf_access");

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, allocation_flags, &(cint_ip_tunnel_basic_info.arp_id),
                                       cint_ip_tunnel_basic_info.tunnel_next_hop_mac,
                                       cint_ip_tunnel_basic_info.provider_vlan), "create egress object ARP only");

    /* Create FEC for 2nd pass and configure its: ARP Outlif, Destination port, Global Out-EthRIF */
    BCM_IF_ERROR_RETURN_MSG(create_2nd_pass_fec(unit, provider_port), "");

    BCM_IF_ERROR_RETURN_MSG(dnx_create_recycle_entry(unit), "");

    /* Create FEC for 1st pass and configure its: recycle entry Outlif, Destination recycle port */
    BCM_IF_ERROR_RETURN_MSG(create_1st_pass_fec(unit, recycle_port), "");

    BCM_IF_ERROR_RETURN_MSG(add_1st_pass_routes(unit), "");
    BCM_IF_ERROR_RETURN_MSG(add_2nd_pass_routes(unit), "");

    return BCM_E_NONE;
}

/*
 * Create FEC for 1st pass and configure its: recycle entry Outlif, Destination recycle port
 */
int
create_1st_pass_fec(
        int unit,
        int recycle_port)
{
    char *proc_name = "create_1st_pass_fec";
    bcm_gport_t gport = 0;
    int fec_flags2 = 0;


    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_GPORT_LOCAL_SET(gport, recycle_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.recycle_entry_fec_id, 0,
            cint_ip_tunnel_basic_info.recycle_entry_encap_id, gport, 0, fec_flags2), "create egress object FEC only");
    return BCM_E_NONE;
}

/*
 * Create FEC for 2nd pass and configure its: ARP Outlif, Destination port, Global Out-EthRIF
 */
int
create_2nd_pass_fec(
        int unit,
        int provider_port)
{
    char *proc_name = "create_1st_pass_fec";
    bcm_gport_t gport = 0;
    int fec_flags2 = 0;


    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_GPORT_LOCAL_SET(gport, provider_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.arp_fec_id, cint_ip_tunnel_basic_info.eth_rif_intf_provider,
                                     cint_ip_tunnel_basic_info.arp_id, gport, 0, fec_flags2), "create egress object FEC only");
    return BCM_E_NONE;
}

int
add_1st_pass_routes(int unit)
{
    char *proc_name = "add_1st_pass_routes";
    int fec_id;

    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ip_tunnel_basic_info.recycle_entry_fec_id);
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_ip_tunnel_basic_info.route_dip, cint_ip_tunnel_basic_info.route_dip_mask, cint_ip_tunnel_basic_info.access_vrf, fec_id), "");
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_tunnel_basic_info.route_dip6, cint_ip_tunnel_basic_info.route_dip6_mask, cint_ip_tunnel_basic_info.access_vrf, fec_id), "");
    return BCM_E_NONE;
}

int
add_2nd_pass_routes(int unit)
{
    char *proc_name = "add_2nd_pass_routes";
    BCM_IF_ERROR_RETURN_MSG(add_2nd_pass_ipv4_route(unit), "");
    BCM_IF_ERROR_RETURN_MSG(add_2nd_pass_ipv6_route(unit), "");
    return BCM_E_NONE;
}

int
add_2nd_pass_ipv4_route(int unit)
{
    char *proc_name = "add_2nd_pass_ipv4_route";
    int fec_id;

    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ip_tunnel_basic_info.arp_fec_id);
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_ip_tunnel_basic_info.route_dip, cint_ip_tunnel_basic_info.route_dip_mask, cint_ip_tunnel_basic_info.redirected_vrf, fec_id), "");
    return BCM_E_NONE;
}

int
add_2nd_pass_ipv6_route(int unit)
{
    char *proc_name = "add_2nd_pass_ipv6_route";
    int fec_id;

    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ip_tunnel_basic_info.arp_fec_id);
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_tunnel_basic_info.route_dip6, cint_ip_tunnel_basic_info.route_dip6_mask, cint_ip_tunnel_basic_info.redirected_vrf, fec_id), "");
    return BCM_E_NONE;
}

int
dnx_create_recycle_entry(int unit)
{
    char *proc_name = "dnx_ip_route_vrf_redirect_create_recycle_entry";
    recycle_entry_params_s recycle_entry_params;
    recycle_entry_params.recycle_app = bcmL2EgressRecycleAppRedirectVrf;
    recycle_entry_params.vrf_redirect_params.vrf = cint_ip_tunnel_basic_info.redirected_vrf;
    BCM_IF_ERROR_RETURN_MSG(create_vrf_redirect_recycle_entry(unit, &recycle_entry_params, &(cint_ip_tunnel_basic_info.recycle_entry_encap_id)), "");
    return BCM_E_NONE;
}

int
dnx_update_recycle_entry(int unit)
{
    char *proc_name = "update_recycle_entry";
    recycle_entry_params_s recycle_entry_params;
    recycle_entry_params.recycle_app = bcmL2EgressRecycleAppRedirectVrf;
    recycle_entry_params.vrf_redirect_params.vrf = cint_ip_tunnel_basic_info.redirected_vrf;
    BCM_IF_ERROR_RETURN_MSG(update_recycle_entry(unit, &recycle_entry_params, cint_ip_tunnel_basic_info.recycle_entry_encap_id), "");
    return BCM_E_NONE;
}

int
dnx_remove_recycle_entry(int unit)
{
    char *proc_name = "remove_recycle_entry";
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_egress_destroy(unit, cint_ip_tunnel_basic_info.recycle_entry_encap_id), "");
    return BCM_E_NONE;
}

int
dnx_get_control_lif(int is_uniform)
{
    if (is_uniform)
    {
        return cint_ip_tunnel_basic_info.control_lif_uniform;
    }
    else
    {
        return cint_ip_tunnel_basic_info.control_lif_short_pipe;
    }
}

int
dnx_update_recycle_entry_with_control_lif(int unit, int is_uniform)
{
    char *proc_name = "update_recycle_entry";
    recycle_entry_params_s recycle_entry_params;
    recycle_entry_params.recycle_app = bcmL2EgressRecycleAppRedirectVrf;
    recycle_entry_params.vrf_redirect_params.vrf = cint_ip_tunnel_basic_info.redirected_vrf;
    recycle_entry_params.control_lif_gport = dnx_get_control_lif(is_uniform);
    BCM_IF_ERROR_RETURN_MSG(update_recycle_entry(unit, &recycle_entry_params, cint_ip_tunnel_basic_info.recycle_entry_encap_id), "");
    return BCM_E_NONE;
}

int
dnx_create_control_lifs(int unit)
{
    char *proc_name = "dnx_create_control_lifs";
    BCM_IF_ERROR_RETURN_MSG(dnx_create_control_lif_short_pipe(unit), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_create_control_lif_uniform(unit), "");
    return BCM_E_NONE;
}

int
dnx_create_control_lif_short_pipe(int unit)
{
    char *proc_name = "dnx_create_control_lif_short_pipe";
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = (BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_RECYCLE);
    vlan_port.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
    vlan_port.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    vlan_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
    vlan_port.ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelInvalid;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    cint_ip_tunnel_basic_info.control_lif_short_pipe = vlan_port.vlan_port_id;
    return BCM_E_NONE;
}

int
dnx_create_control_lif_uniform(int unit)
{
    char *proc_name = "dnx_create_control_lif_uniform";
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = (BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_RECYCLE);
    vlan_port.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    vlan_port.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    vlan_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    vlan_port.ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelInvalid;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    cint_ip_tunnel_basic_info.control_lif_uniform = vlan_port.vlan_port_id;
    return BCM_E_NONE;
}


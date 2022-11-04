/*
 * cint_srv6_egress_tunnel_1_pass.c
 *
 *  Created on: Apr 3, 2020
 *      Author: sk889844
 */

struct cint_srv6_tunnel_info_s
{
    int eth_rif_intf_in;                /* in RIF */
    int eth_rif_intf_out;               /* out RIF */
    int eth_rif_intf_tunnel_disable_out; /* out RIF*/
    bcm_mac_t intf_in_mac_address;      /* mac for in RIF */
    bcm_mac_t intf_out_mac_address;     /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac;         /* mac for next hop */
    bcm_mac_t arp_next_hop_tunnel_disable_mac; /* mac for next hop when tunnel term is disabled*/
    bcm_mac_t mac_fwd_address;          /* fwd mac address */
    int vrf_in;                         /* VRF, part of the tunnel terminator lookup key*/
    int vrf_out;                        /* VRF, resolved after tunnel termination*/
    int tunnel_arp_id;                  /* Id for ARP entry */
    int tunnel_disable_arp_id;          /* Id for ARP entry */
    int tunnel_fec_id;                  /* FEC id */
    int tunnel_disable_fec_id;          /* FEC id */
    bcm_ip_t route_ipv4_dip;            /* IPv4 Route DIP */
    bcm_ip_t ipv4_mask;                 /* IPv4 Route DIP mask */
    bcm_ip6_t tunnel_ipv6_dip;          /* IPv6 Route DIP */
    bcm_ip6_t route_ipv6_dip;           /* IPv6 tunnel DIP */
    bcm_ip6_t ipv6_mask;                /* IPv6 mask */
    int srh_next_protocol;              /* Next protocol above SRv6, per IANA RFC */
    int srv6_term_tunnel_id;            /* Tunnel ID*/
    int l2_termination_vsi;             /* Tunnel VSI used when fwd is done on L2*/
};


cint_srv6_tunnel_info_s cint_srv6_tunnel_info =
{
        /*
         * eth_rif_intf_in | eth_rif_intf_out | eth_rif_intf_tunnel_disable_out
         */
         15, 100, 200,
        /*
         * intf_in_mac_address | intf_out_mac_address | arp_next_hop_mac | arp_next_hop_tunnel_disable_mac | mac_fwd_address
         */
         { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 }, { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a }, { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }, { 0xaa, 0xbb, 0x00, 0x00, 0xcd, 0x1d }, { 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c },
        /*
         * vrf_in | vrf_out
         */
         10, 14,
         /*
          * tunnel_arp_id | tunnel_disable_arp_id | tunnel_fec_id | tunnel_disable_fec_id
          */
         0x1384, 0x4500, 0, 0,
        /*
         * route_ipv4_dip | ipv4_mask
         */
        0x7fffff02 /* 127.255.255.02 */, 0xfffffff0,
        /*
         * tunnel_ipv6_dip
         */
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0xff, 0xff, 0xff, 0x13 },
        /*
         * route_ipv6_dip
         */
        { 0x76, 0x76, 0x87, 0x87, 0x54, 0x54, 0x45, 0x45, 0x12, 0x12, 0x32, 0x32, 0x16, 0x16, 0x61, 0x61 },
         /*
          * ipv6_mask
          */
         {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
         /*
          * srh_next_protocol | srv6_term_tunnel_id | l2_termination_vsi
          */
        4, 0, 15
};

struct cint_srv6_tunnel_l2vpn_additional_info_s
{
    int srv6_term_tunnel_id[4];         /* Tunnel ID for 0/1/2/x service tag cases*/
    bcm_ip6_t tunnel_ipv6_dip[4];          /* Tunnel DIP for 0/1/2/x service tag cases*/
    int outer_vid;
    int inner_vid;
    int access_port[3];                 /* out port for 0/1/2 vitag cases*/ 
};

cint_srv6_tunnel_l2vpn_additional_info_s cint_srv6_tunnel_l2vpn_info =
{
        /*
         * srv6_term_tunnel_id 
         */
        {0, 0, 0 ,0},

        /*
         * tunnel_ipv6_dip
         */
        {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0xff, 0xff, 0xff, 0x10 },
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0xff, 0xff, 0xff, 0x11 },
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0xff, 0xff, 0xff, 0x12 },
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0xff, 0xff, 0xff, 0x13 }},
       
         /*
          * outer_vid | inner_vid
          */
         200, 100,
         /*
          * out ports
          */
         {201, 202, 203}
};


int
srv6_tunnel_terminator_create(int unit)
{
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;

    /*
     * Configure the keys for the resulting LIF on termination (DIP, VRF, additional header qualifier)
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, cint_srv6_tunnel_info.tunnel_ipv6_dip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, cint_srv6_tunnel_info.ipv6_mask, 16);
    tunnel_term_set.vrf = cint_srv6_tunnel_info.vrf_in;
    tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
    /* If it's ESR USP, shortpipe and uniform are supported
     * Uniform: take QoS info from SRv6 tunnel
     * Shortpipe: take QoS info from fwd layer
     */
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;

    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if ((cint_srv6_tunnel_info.srh_next_protocol == 143) || (cint_srv6_tunnel_info.srh_next_protocol == 59))
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthIn6;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

    cint_srv6_tunnel_info.srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

    /*
     * Configure the termination resulting LIF's FODO to VRF
     *  - not needed if resulting LIF's FODO is to be VSI (when ETHoSRv6), because
     *    in this case it's set by 'bcm_tunnel_terminator_create' above with 'vlan' property
     */
    if ((cint_srv6_tunnel_info.srh_next_protocol != 143) && (cint_srv6_tunnel_info.srh_next_protocol != 59))
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = cint_srv6_tunnel_info.vrf_out;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    return BCM_E_NONE;
}
/**
 * The function disable routing for the tunnel per specific next layer protocol
 */
int
srv6_tunnel_terminator_routing_disable(
        int unit,
        int access_port)
{
    l3_ingress_intf ingress_rif;
    int fec_flags2 = 0;
    l3_ingress_intf_init(&ingress_rif);

    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, cint_srv6_tunnel_info.srv6_term_tunnel_id);
    if (cint_srv6_tunnel_info.srh_next_protocol == 143)
    {
        ingress_rif.flags = BCM_L3_INGRESS_TUNNEL_DISABLE_NATIVE_ETH;
    }
    else
    {
        ingress_rif.vrf = cint_srv6_tunnel_info.vrf_out;
        ingress_rif.flags = BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST;
    }
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");

    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_tunnel_disable_out, cint_srv6_tunnel_info.intf_out_mac_address),
        "intf_out_mac_address");
    /*
     * Create ARP entry and set its properties as different than the egress node destination
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &(cint_srv6_tunnel_info.tunnel_disable_arp_id),
                                                  cint_srv6_tunnel_info.arp_next_hop_tunnel_disable_mac,
                                                  cint_srv6_tunnel_info.eth_rif_intf_tunnel_disable_out), "");


    /*
     * Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF
     */
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &(cint_srv6_tunnel_info.tunnel_disable_fec_id)), "");
    cint_srv6_tunnel_info.tunnel_disable_fec_id += 300;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_disable_fec_id,
                                        cint_srv6_tunnel_info.eth_rif_intf_tunnel_disable_out,
                                        cint_srv6_tunnel_info.tunnel_disable_arp_id, access_port, 0, fec_flags2), "");

    /*
     * Add Route Ipv6 entry
     */
    if(*dnxc_data_get(unit, "tunnel", "feature", "tunnel_route_disable", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_srv6_tunnel_info.tunnel_ipv6_dip,
                              cint_srv6_tunnel_info.ipv6_mask,
                              cint_srv6_tunnel_info.vrf_out,
                              cint_srv6_tunnel_info.tunnel_disable_fec_id), "");
    }
    else
    { 
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_srv6_tunnel_info.tunnel_ipv6_dip,
                              cint_srv6_tunnel_info.ipv6_mask,
                              cint_srv6_tunnel_info.vrf_in,
                              cint_srv6_tunnel_info.tunnel_disable_fec_id), "");
    }
    return BCM_E_NONE;
}

/*
 * Main function for basic srv6 egress tunnel 1-pass termination
 * The function implements basic scenario of max SIDs termination possible
 * Inputs:
 * unit - relevant unit;
 * provider_port - incoming port;
 * access_port - outgoing port;
 */
int
srv6_egress_tunnel_basic(
    int unit,
    int provider_port,
    int access_port,
    int recycle_port)
{
    char error_msg[200]={0,};
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    int fec_flags2 = 0;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);


    /*
     * Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, provider_port, cint_srv6_tunnel_info.eth_rif_intf_in), "eth_rif_intf_in");

    /*
     * Set Out-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, access_port), "access_port");

    /** Set Out-Port default properties for the recycled packets*/
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, recycle_port), "recycle_port");

    if (!ibch1_supported)
    {
        /** Configure RCH port of Extended Termination type */
        snprintf(error_msg, sizeof(error_msg), "for rch_port %d", recycle_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm), error_msg);
    }

    /** Set port class in order for PMF to crop RCH ad IRPP */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,recycle_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");

    /*
     * Create routing interface for the tunnel's IP.
     * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure), to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in, cint_srv6_tunnel_info.intf_in_mac_address),
        "intf_in_mac_address");

    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out, cint_srv6_tunnel_info.intf_out_mac_address),
        "intf_out_mac_address");

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in;
    ingr_itf.vrf     = cint_srv6_tunnel_info.vrf_in;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "");

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    BCM_IF_ERROR_RETURN_MSG(srv6_tunnel_terminator_create(unit), "");

    /*
     * Create ARP entry and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &(cint_srv6_tunnel_info.tunnel_arp_id),
                                                  cint_srv6_tunnel_info.arp_next_hop_mac,
                                                  cint_srv6_tunnel_info.eth_rif_intf_out), "");


    /*
     * Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF
     */
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &(cint_srv6_tunnel_info.tunnel_fec_id)), "");
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id,
                                        cint_srv6_tunnel_info.eth_rif_intf_out,
                                        cint_srv6_tunnel_info.tunnel_arp_id, access_port, 0, fec_flags2), "");

    /*
     * Add Route Ipv6 entry
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_srv6_tunnel_info.route_ipv6_dip,
                              cint_srv6_tunnel_info.ipv6_mask,
                              cint_srv6_tunnel_info.vrf_out,
                              cint_srv6_tunnel_info.tunnel_fec_id), "");

    /*
     * Add Route Ipv4 entry
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.ipv4_mask,
                              cint_srv6_tunnel_info.vrf_out, cint_srv6_tunnel_info.tunnel_fec_id), "");

    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, cint_srv6_tunnel_info.mac_fwd_address, cint_srv6_tunnel_info.l2_termination_vsi);
    l2_addr.flags = BCM_L2_STATIC;      /* static entry */
    l2_addr.port = access_port;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;
    BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_create_all_sids_rcy_entries_and_extensions(
            unit, nof_2pass_recycle_entries, max_nof_terminated_usp_sids_1pass, recycle_port),"");

    return BCM_E_NONE;
}


/*
 * Main function for basic srv6 egress l2vpn tunnel 2-pass termination
 * The function implements basic scenario of max SIDs termination with and
 * without priority extended_p2p forwarding entries.
 * Inputs:
 * unit - relevant unit;
 * provider_port - incoming port;
 * access_port1 - outgoing port for extended_p2p 0 service tag case;
 * access_port2 - outgoing port for extended_p2p 1/x service tag case;
 * access_port3 - outgoing port for extended_p2p 2/x service tag case;
 */
int
srv6_egress_tunnel_end_dx2v_extended_p2p_main(
    int unit,
    int provider_port,
    int recycle_port,
    int access_port1,
    int access_port2,
    int access_port3)
{
    char error_msg[200]={0,};
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    int fec_flags2 = 0;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);

    /** Next protocol is ETH*/
    cint_srv6_tunnel_info.srh_next_protocol = 143;
    cint_srv6_tunnel_l2vpn_info.access_port[0] = access_port1;
    cint_srv6_tunnel_l2vpn_info.access_port[1] = access_port2;
    cint_srv6_tunnel_l2vpn_info.access_port[2] = access_port3;

    /*
     * Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, provider_port, cint_srv6_tunnel_info.eth_rif_intf_in), "eth_rif_intf_in");

    /*
     * Set Out-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, access_port1), "access_port1");
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, access_port2), "access_port2");
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, access_port3), "access_port3");

    /** Set Out-Port default properties for the recycled packets*/
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, recycle_port), "recycle_port");

    if (!ibch1_supported)
    {
        /** Configure RCH port of Extended Termination type */
        snprintf(error_msg, sizeof(error_msg), "for rch_port %d", recycle_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm), error_msg);
    }

    /** Set port class in order for PMF to crop RCH ad IRPP */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,recycle_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");

    /*
     * Create routing interface for the tunnel's IP.
     * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure), to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in, cint_srv6_tunnel_info.intf_in_mac_address),
        "intf_in_mac_address");

    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out, cint_srv6_tunnel_info.intf_out_mac_address),
        "intf_out_mac_address");

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in;
    ingr_itf.vrf     = cint_srv6_tunnel_info.vrf_in;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "");

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    bcm_tunnel_terminator_t tunnel_term_set;
    int nof_terminators = 4; /** 0 for 0-tag; 1 for 1-tag; 2 for 2-tag; 3 for x-tag.*/
    int nof_match_vlan;
    bcm_gport_t srv6_terminator_id[nof_terminators] = { 0 };

    /*
     * Configure the keys for the resulting LIF on termination (DIP, VRF, additional header qualifier)
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeEthIn6;
    tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
    
    sal_memcpy(tunnel_term_set.dip6_mask, cint_srv6_tunnel_info.ipv6_mask, 16);
    tunnel_term_set.vrf = cint_srv6_tunnel_info.vrf_in;
    tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_CROSS_CONNECT;

    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;

    for (nof_match_vlan = 0; nof_match_vlan < nof_terminators; nof_match_vlan++)
    {
        tunnel_term_set.nof_service_tags = (nof_match_vlan == 3 ? 0 : nof_match_vlan);
        tunnel_term_set.flags |= (nof_match_vlan == 3 ? BCM_TUNNEL_TERM_SERVICE_TAGGED_BASED_HEADER_ONLY : 0);
        tunnel_term_set.flags |= (nof_match_vlan > 0 ? BCM_TUNNEL_TERM_SERVICE_TAGGED : 0);

        sal_memcpy(tunnel_term_set.dip6, cint_srv6_tunnel_l2vpn_info.tunnel_ipv6_dip[nof_match_vlan], 16);

        BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");
        cint_srv6_tunnel_l2vpn_info.srv6_term_tunnel_id[nof_match_vlan] = tunnel_term_set.tunnel_id;

        printf("Created Srv6 tunnel terminator: 0x%08X! \n", tunnel_term_set.tunnel_id);
    }

    /*
     * Add EXtended P2P entry for each terminator
     */
    bcm_vswitch_flexible_connect_match_t port_match; 
    bcm_vswitch_flexible_connect_fwd_t port_fwd;

    bcm_vswitch_flexible_connect_match_t_init(&port_match);
    bcm_vswitch_flexible_connect_fwd_t_init(&port_fwd);

    /**Create the extended P2P entries for 0/1/2 service tag cases. HP cases. */
    for (nof_match_vlan = 0; nof_match_vlan < nof_terminators - 1; nof_match_vlan++)
    {
        port_match.match_port = cint_srv6_tunnel_l2vpn_info.srv6_term_tunnel_id[nof_match_vlan];
        port_match.match = BCM_PORT_MATCH_PORT;
        if (nof_match_vlan > 0) {
            port_match.match_vlan = cint_srv6_tunnel_l2vpn_info.outer_vid;
            port_match.match = BCM_PORT_MATCH_PORT_VLAN;
        }
        if (nof_match_vlan > 1) {
            port_match.match_inner_vlan = cint_srv6_tunnel_l2vpn_info.inner_vid;
            port_match.match = BCM_PORT_MATCH_PORT_VLAN_STACKED;
        }

        port_fwd.forward_port = cint_srv6_tunnel_l2vpn_info.access_port[nof_match_vlan];

        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_flexible_connect_add(unit, 0, &port_match, &port_fwd), "");
    }

    /**Create the extended P2P entries for 1/2 service tag cases. LP cases. */
    for (nof_match_vlan = 0; nof_match_vlan < nof_terminators - 1 - 1; nof_match_vlan++)
    {
        port_match.match_port = cint_srv6_tunnel_l2vpn_info.srv6_term_tunnel_id[nof_match_vlan + 1];
        port_match.match = BCM_PORT_MATCH_PORT;
        if (nof_match_vlan > 0) {
            port_match.match_vlan = cint_srv6_tunnel_l2vpn_info.outer_vid;
            port_match.match = BCM_PORT_MATCH_PORT_VLAN;
        }
        if (nof_match_vlan > 1) {
            port_match.match_inner_vlan = cint_srv6_tunnel_l2vpn_info.inner_vid;
            port_match.match = BCM_PORT_MATCH_PORT_VLAN_STACKED;
        }

        port_fwd.forward_port = cint_srv6_tunnel_l2vpn_info.access_port[nof_match_vlan];

        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_flexible_connect_add(unit, 0, &port_match, &port_fwd), "");
    }

    /**Create the extended P2P entries for X service tag cases. HP/LP cases*/
    for (nof_match_vlan = 0; nof_match_vlan < nof_terminators - 1; nof_match_vlan++)
    {
        port_match.match_port = cint_srv6_tunnel_l2vpn_info.srv6_term_tunnel_id[3];
        port_match.match = BCM_PORT_MATCH_PORT;
        if (nof_match_vlan > 0) {
            port_match.match_vlan = cint_srv6_tunnel_l2vpn_info.outer_vid;
            port_match.match = BCM_PORT_MATCH_PORT_VLAN;
        }
        if (nof_match_vlan > 1) {
            port_match.match_inner_vlan = cint_srv6_tunnel_l2vpn_info.inner_vid;
            port_match.match = BCM_PORT_MATCH_PORT_VLAN_STACKED;
        }

        /** Re-use the outport for 2 service tag case.*/
        port_fwd.forward_port = cint_srv6_tunnel_l2vpn_info.access_port[nof_match_vlan];

        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_flexible_connect_add(unit, 0, &port_match, &port_fwd), "");
    }


    /** define the structure which holds the mapping between nof_sids to RCH port and encap_id */
    bcm_srv6_extension_terminator_mapping_t extension_terminator_info;
    int sid_idx;

    extension_terminator_info.flags = 0;
    /** Convert Port number to GPORT */
    BCM_GPORT_LOCAL_SET(extension_terminator_info.port, recycle_port);

    /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;

    for (sid_idx = 0; sid_idx < nof_2pass_recycle_entries; sid_idx++)
    {
        /*
        * Create RCH encapsulation per each NOF SIDs, to take into account relevant:
        * -recycle_header_extension_length (bytes to remove on 2nd pass at RCH termination)
        * -additional_egress_termination_size (additional bytes to remove on 1st pass egress - additional to parsing start offset value)
        */
        int recycle_entry_encap_id;
        /** Create entry, which will be built the recycle header for the copy going on the 2nd pass*/
        BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_recycle_entry(unit, sid_idx, &recycle_entry_encap_id), "");

        /** Convert from L3_ITF to GPORT */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(extension_terminator_info.encap_id , recycle_entry_encap_id);


        extension_terminator_info.nof_sids = sid_idx + max_nof_terminated_usp_sids_1pass + 1;

        /** call the nof_sids API to map to RCH port and encapsulation */
        snprintf(error_msg, sizeof(error_msg), "for nof_sids:%d", sid_idx);
        BCM_IF_ERROR_RETURN_MSG(bcm_srv6_extension_terminator_add(unit, &extension_terminator_info), error_msg);


    } /** of for sid_idx*/


    /*
     * By default the packet with sevice tags in pwe_tagged_mode will be discarded if no match in 
     * native classification. It's done by a P2P default LIF.
     * We need to abondan the discard or lower it's priority for Extended P2P. 
     */
    int gport, trap_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(unit, 0, bcmRxTrapDfltDroppedPacket, &trap_id), "");

    BCM_GPORT_TRAP_SET(gport, 0, 0, trap_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_set (unit, bcmVlanNativeVplsServiceTaggedMiss, gport),
        "bcmVlanNativeVplsServiceTaggedMiss");

    return BCM_E_NONE;
}

/*
 * Remove the extended P2P match entries.
 * nof_match_vlan - 0/1/2.
 */
int
srv6_egress_tunnel_end_dx2v_extended_p2p_rm_hp_entry(
    int unit,
    int nof_match_vlan)
{
    bcm_vswitch_flexible_connect_match_t port_match; 

    bcm_vswitch_flexible_connect_match_t_init(&port_match);

    port_match.match = BCM_PORT_MATCH_PORT;
    if (nof_match_vlan > 0) {
        port_match.match_vlan = cint_srv6_tunnel_l2vpn_info.outer_vid;
        port_match.match = BCM_PORT_MATCH_PORT_VLAN;
    }
    if (nof_match_vlan > 1) {
        port_match.match_inner_vlan = cint_srv6_tunnel_l2vpn_info.inner_vid;
        port_match.match = BCM_PORT_MATCH_PORT_VLAN_STACKED;
    }

    /** Remove the extended p2p match entries for 0/1/2 service tag cases. HP cases. */
    port_match.match_port = cint_srv6_tunnel_l2vpn_info.srv6_term_tunnel_id[nof_match_vlan];
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_flexible_connect_delete(unit, &port_match), "");

    if (nof_match_vlan <= 1)
    {
        /** Remove the extended p2p match entries for 0/1/2 service tag cases. LP cases. */
        port_match.match_port = cint_srv6_tunnel_l2vpn_info.srv6_term_tunnel_id[nof_match_vlan + 1];
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_flexible_connect_delete(unit, &port_match), "");
    }

    /** Remove the extended p2p match entries for X service tag cases. HP/LP cases. */
    port_match.match_port = cint_srv6_tunnel_l2vpn_info.srv6_term_tunnel_id[3];
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_flexible_connect_delete(unit, &port_match), "");

    return BCM_E_NONE;
}



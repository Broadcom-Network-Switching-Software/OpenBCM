/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
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
    bcm_mac_t intf_in_mac_address;      /* mac for in RIF */
    bcm_mac_t intf_out_mac_address;     /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac;         /* mac for next hop */
    bcm_mac_t mac_fwd_address;          /* fwd mac address */
    int vrf_in;                         /* VRF, part of the tunnel terminator lookup key*/
    int vrf_out;                        /* VRF, resolved after tunnel termination*/
    int tunnel_arp_id;                  /* Id for ARP entry */
    int tunnel_fec_id;                  /* FEC id */
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
         * eth_rif_intf_in | eth_rif_intf_out
         */
         15, 100,
        /*
         * intf_in_mac_address | intf_out_mac_address | arp_next_hop_mac | mac_fwd_address
         */
         { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 }, { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a }, { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }, { 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c },
        /*
         * vrf_in | vrf_out
         */
         10, 14,
         /*
          * tunnel_arp_id | tunnel_fec_id
          */
         0x1384, 0,
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

int
srv6_tunnel_terminator_create(int unit)
{
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv = BCM_E_NONE;

    /*
     * Configure the keys for the resulting LIF on termination (DIP, VRF, additional header qualifier)
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeSR6;
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
    if (cint_srv6_tunnel_info.srh_next_protocol == 143)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthSR6;
    }

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }

    cint_srv6_tunnel_info.srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

    /*
     * Configure the termination resulting LIF's FODO to VRF
     *  - not needed if resulting LIF's FODO is to be VSI (when ETHoSRv6), because
     *    in this case it's set by 'bcm_tunnel_terminator_create' above with 'vlan' property
     */
    if (cint_srv6_tunnel_info.srh_next_protocol != 143)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = cint_srv6_tunnel_info.vrf_out;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        rv = intf_ingress_rif_set(unit, &ingress_rif);
        if (rv != BCM_E_NONE)
        {
            printf("Error intf_ingress_rif_set. rv = %d \n", rv);
            return rv;
        }
    }

    return rv;
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
    int rv = BCM_E_NONE;
    char *proc_name;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    proc_name = "srv6_egress_tunnel_1pass_basic";

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, provider_port, cint_srv6_tunnel_info.eth_rif_intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set, eth_rif_intf_in\n", proc_name);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set, access_port\n", proc_name);
        return rv;
    }

    /** Set Out-Port default properties for the recycled packets*/
    rv = out_port_set(unit, recycle_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set recycle_port \n", proc_name);
        return rv;
    }

    /** Configure RCH port of Extended Termination type */
    rv = bcm_port_control_set(unit, recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set for rch_port %d\n",proc_name, recycle_port);
        return rv;
    }

    /*
     * Create routing interface for the tunnel's IP.
     * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure), to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in, cint_srv6_tunnel_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create, intf_in_mac_address\n", proc_name);
        return rv;
    }

    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out, cint_srv6_tunnel_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create, intf_out_mac_address\n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in;
    ingr_itf.vrf     = cint_srv6_tunnel_info.vrf_in;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = srv6_tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, srv6_tunnel_terminator_create\n", proc_name);
        return rv;
    }

    /*
     * Create ARP entry and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_srv6_tunnel_info.tunnel_arp_id),
                                                  cint_srv6_tunnel_info.arp_next_hop_mac,
                                                  cint_srv6_tunnel_info.eth_rif_intf_out);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_encap__create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF
     */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &(cint_srv6_tunnel_info.tunnel_fec_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id,
                                        cint_srv6_tunnel_info.eth_rif_intf_out,
                                        cint_srv6_tunnel_info.tunnel_arp_id, access_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error,l3__egress_only_fec__create\n", proc_name);
        return rv;
    }

    /*
     * Add Route Ipv6 entry
     */
    rv = add_route_ipv6(unit, cint_srv6_tunnel_info.route_ipv6_dip,
                              cint_srv6_tunnel_info.ipv6_mask,
                              cint_srv6_tunnel_info.vrf_out,
                              cint_srv6_tunnel_info.tunnel_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_route_ipv6\n", proc_name);
        return rv;
    }

    /*
     * Add Route Ipv4 entry
     */
    rv = add_route_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.ipv4_mask,
                              cint_srv6_tunnel_info.vrf_out, cint_srv6_tunnel_info.tunnel_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_route_ipv4\n", proc_name);
        return rv;
    }

    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, cint_srv6_tunnel_info.mac_fwd_address, cint_srv6_tunnel_info.l2_termination_vsi);
    l2_addr.flags = BCM_L2_STATIC;      /* static entry */
    l2_addr.port = access_port;
    rv = bcm_l2_addr_add(unit, &l2_addr);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_l2_addr_add\n", proc_name);
        return rv;
    }
    /** define the structure which holds the mapping between nof_sids to RCH port and encap_id */
    bcm_srv6_extension_terminator_mapping_t extension_terminator_info;
    int sid_idx;

    extension_terminator_info.flags = 0;
    /** Convert Port number to GPORT */
    BCM_GPORT_LOCAL_SET(extension_terminator_info.port, recycle_port);

    /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp_1pass", NULL));
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
        rv = srv6_create_extended_termination_recycle_entry(unit, sid_idx, &recycle_entry_encap_id);
        if (rv != BCM_E_NONE)
        {
           printf("Error, srv6_create_extended_termination_recycle_entry \n");
           return rv;
        }

        /** Convert from L3_ITF to GPORT */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(extension_terminator_info.encap_id , recycle_entry_encap_id);


        extension_terminator_info.nof_sids = sid_idx + max_nof_terminated_usp_sids_1pass + 1;

        /** call the nof_sids API to map to RCH port and encapsulation */
        rv = bcm_srv6_extension_terminator_add(unit, &extension_terminator_info);

        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_srv6_extension_terminator_add for nof_sids:%d\n", sid_idx);
            return rv;
        }

    } /** of for sid_idx*/
    return rv;
}

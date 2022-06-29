/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

/*
 * Required Cints:
 * cint ../../../../src/examples/dnx/utility/cint_dnx_util_srv6.c
 * cint ../../../../src/examples/dnx/field/cint_field_epmf_cs_outlif_profile.c
 */


/** \brief List of SRv6 used instances */
enum srv6_instances_e
{
    INGRESS,
    END_POINT_1,
    END_POINT_2,
    EGRESS,
    RCH
};

 int NOF_INSTANCES = RCH+1;

/** \brief List of SRv6 used FEC ids */
enum srv6_fec_id_e
{
    INGRESS_SRH_FEC_ID,
    INGRESS_SRH_L2VPN_FEC_ID,
    INGRESS_FIRST_SID_FEC_ID,
    INGRESS_FIRST_SID_L2VPN_FEC_ID,
    END_POINT_1_FEC_ID,
    END_POINT_2_FEC_ID,
    EGRESS_FEC_ID,
    ARP_AC_FEC_ID
};

int NOF_FEC_ID = ARP_AC_FEC_ID+1;

/** \brief List of SRv6 used Global LIFs */
enum srv6_global_lifs_e
{
    INGRESS_SRH_GLOBAL_LIF,
    INGRESS_IP_TUNNEL_GLOBAL_LIF,
    INGRESS_IP_TUNNEL_L2VPN_GLOBAL_LIF
};

int NOF_GLOBAL_LIFS=INGRESS_IP_TUNNEL_L2VPN_GLOBAL_LIF+1;

/** \brief List of SRv6 used Terminated SIDs */
enum srv6_terminated_sids_e
{
    SID4,
    SID3,
    SID2,
    SID1,
    SID0
};

int NOF_SIDS = SID0+1;

/** \brief List of Unified-SID of 32b inside a Classic SID of 128b */
enum srv6_unified_sids_e
{
    USID4,
    USID1,
    USID2,
    USID3
};

int NOF_UNIFIED_SIDS = USID3+1;

/**
 * \brief - SRv6 factor between Classical (128b) and Unified (32b) SID
 */
int SRV6_UNIFIED_CLASSICAL_SID_FACTOR  =  4;
int host_entry_add =1;
struct cint_srv6_tunnel_info_s
{
    int eth_rif_intf_in[NOF_INSTANCES];                                           /* in RIF */
    int eth_rif_intf_out[NOF_INSTANCES-1];                                        /* out RIF */
    bcm_mac_t intf_in_mac_address[NOF_INSTANCES];                                 /* mac for in RIF */
    bcm_mac_t intf_out_mac_address[NOF_INSTANCES];                                /* mac for in RIF */
    bcm_mac_t arp_next_hop_mac[NOF_INSTANCES];                                    /* mac for next hop */
    int vrf_in[NOF_INSTANCES];                                                    /* VRF, resolved as Tunnel-InLif property*/
    int tunnel_arp_id[NOF_INSTANCES-1];                                           /* Id for ARP entry */
    int tunnel_fec_id[NOF_FEC_ID];                                                /* FEC id */
    int tunnel_global_lif[NOF_GLOBAL_LIFS];                                       /* Global LIFs */
    int tunnel_vid;                                                               /* VID */
    bcm_ip_t route_ipv4_dip;                                                      /* IPv4 Route DIP */
    bcm_ip_t route_ipv4_mask;                                                     /* IPv4 Route mask */
    bcm_ip6_t route_ipv6_dip;                                                     /* IPv6 Route DIP */
    bcm_ip6_t route_ipv6_mask;                                                    /* IPv6 DIP mask for routing entry */
    bcm_mac_t l2_fwd_mac;                                                         /* mac for bridging into tunnel */
    int l2_fwd_vsi;                                                               /* vsi for L2 fwd */
    bcm_ip6_t tunnel_ip6_dip[NOF_SIDS];                                           /* IPv6 Terminated DIPs */
    bcm_ip6_t tunnel_classic_unified_sid_arr[NOF_SIDS][NOF_UNIFIED_SIDS];         /* 2D Array of Unified SIDs in Classical SID  */
    bcm_ip6_t tunnel_ip6_sip;                                                     /* IPv6 Tunnel SIP */
    bcm_ip6_t tunnel_ipv6_dip_mask;                                               /* tunnel DIP mask*/
    int recycle_port;                                                             /* ESR USP RCH Port */
    int srh_next_protocol;                                                        /* Next protocol above SRv6, per IANA RFC */
    int l2_termination_vsi;                                                       /* When ETHoSRv6, in Extended Termiantion case, tunnel terminated LIF FODO (VSI) */
    bcm_mac_t l2_termination_mact_fwd_address;                                    /* When ETHoSRv6, MACT host entry for Bridging FWD lookup */
};


cint_srv6_tunnel_info_s cint_srv6_tunnel_info =
{
        /*
         * eth_rif_intf_in
         */
         {15, 16, 17, 18, 19},
        /*
         * eth_rif_intf_out
         */
         {100, 101, 102, 103},
        /*
         * intf_in_mac_address
         */
         {{ 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 },  { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }, { 0x00, 0x00, 0x00, 0x01, 0xcd, 0x1d },
          { 0x00, 0x00, 0x00, 0x02, 0xcd, 0x1d },  { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x11 }},
        /*
         * intf_out_mac_address
         */
         {{ 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a }, { 0x00, 0x21, 0x34, 0x56, 0x78, 0x9a }, { 0x00, 0x22, 0x34, 0x56, 0x78, 0x9a },
          { 0x00, 0x30, 0x34, 0x56, 0x78, 0x9a }, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
        /*
         * arp_next_hop_mac
         */
         {{ 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }, { 0x00, 0x00, 0x00, 0x01, 0xcd, 0x1d }, { 0x00, 0x00, 0x00, 0x02, 0xcd, 0x1d },
          { 0x00, 0x00, 0x00, 0x03, 0xcd, 0x1d }, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
        /*
         * In VRF
         */
         {1, 11, 12, 14, 14},
         /*
          * tunnel_arp_id
          */
         {0x1384, 0x1385, 0x1386, 0x1387},
        /*
         * tunnel_fec_id (update on run time)
         */
         {0, 0, 0, 0, 0, 0, 0, 0},
         /*
          * tunnel_global_lifs - SRH Base, 1st SID respectively
          */
         {0x4230, 0x6231, 0x6241},
         /*
          * tunnel_vid
          */
         100,
        /*
         * route_ipv4_dip
         */
        0x7fffff02 /* 127.255.255.02 */,
        /*
         * route_ipv4_mask
         */
        0xffffffff,
        /*
         * route_ipv6_dip
         */
        {0x11, 0x11, 0x22, 0x22, 0x11, 0x11, 0x22, 0x22, 0x12, 0x34, 0xf0, 0x00, 0xff, 0x00, 0xff, 0x13 },
        /*
         * route_ipv6_mask
         */
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        /*
          * mac for bridging into tunnel
          */
         {0x00, 0x01, 0x02, 0x03, 0x04, 0x05},
        /*
          * vsi for L2 fwd
          */
         2,
        /*
         * terminated_ip6_dips
         */
        {{ 0x76, 0x76, 0x87, 0x87, 0x54, 0x54, 0x45, 0x45, 0x12, 0x12, 0x32, 0x32, 0x16, 0x16, 0x61, 0x61 },
         { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x5a, 0x5a },
         { 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x77, 0x88, 0x88 },
         { 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0xff, 0xff, 0x11, 0x11, 0x22, 0x22 },
         { 0xab, 0xcd, 0xdb, 0xca, 0x12, 0x34, 0x43, 0x21, 0x10, 0x10, 0x98, 0x98, 0x45, 0x67, 0x9a, 0xbc }},
         /*
          * tunnel_classic_unified_sid_arr
          */
         {
             /** Classic SID4 cases per Unified-SID number in the classic SID */
           {{ 0xee, 0xee, 0xbb, 0x00, 0xee, 0xee, 0xbb, 0x01, 0xee, 0xee, 0xbb, 0x02, 0xee, 0xee, 0xbb, 0x03 },
            { 0xee, 0xee, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0xee, 0xee, 0xbb, 0x00, 0xee, 0xee, 0xbb, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0xee, 0xee, 0xbb, 0x00, 0xee, 0xee, 0xbb, 0x01, 0xee, 0xee, 0xbb, 0x02, 0x00, 0x00, 0x00, 0x00 }},


             /** Classic SID3 cases per Unified-SID number in the classic SID */
           {{ 0xdd, 0xdd, 0xbb, 0x00, 0xdd, 0xdd, 0xbb, 0x01, 0xdd, 0xdd, 0xbb, 0x02, 0xdd, 0xdd, 0xbb, 0x03 },
            { 0xdd, 0xdd, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0xdd, 0xdd, 0xbb, 0x00, 0xdd, 0xdd, 0xbb, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0xdd, 0xdd, 0xbb, 0x00, 0xdd, 0xdd, 0xbb, 0x01, 0xdd, 0xdd, 0xbb, 0x02, 0x00, 0x00, 0x00, 0x00 }},


            /** Classic SID2 cases per Unified-SID number in the classic SID */
           {{ 0xcc, 0xcc, 0xbb, 0x00, 0xcc, 0xcc, 0xbb, 0x01, 0xcc, 0xcc, 0xbb, 0x02, 0xcc, 0xcc, 0xbb, 0x03 },
            { 0xcc, 0xcc, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0xcc, 0xcc, 0xbb, 0x00, 0xcc, 0xcc, 0xbb, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0xcc, 0xcc, 0xbb, 0x00, 0xcc, 0xcc, 0xbb, 0x01, 0xcc, 0xcc, 0xbb, 0x02, 0x00, 0x00, 0x00, 0x00 }},

            /** Classic SID1 cases per Unified-SID number in the classic SID */
            {{ 0xbb, 0xbb, 0xbb, 0x00, 0xbb, 0xbb, 0xbb, 0x01, 0xbb, 0xbb, 0xbb, 0x02, 0xbb, 0xbb, 0xbb, 0x03 },
             { 0xbb, 0xbb, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
             { 0xbb, 0xbb, 0xbb, 0x00, 0xbb, 0xbb, 0xbb, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
             { 0xbb, 0xbb, 0xbb, 0x00, 0xbb, 0xbb, 0xbb, 0x01, 0xbb, 0xbb, 0xbb, 0x02, 0x00, 0x00, 0x00, 0x00 }},


            /** Classic SID0 cases per Unified-SID number in the classic SID */
            {{ 0xaa, 0xaa, 0xbb, 0x00, 0xaa, 0xaa, 0xbb, 0x01, 0xaa, 0xaa, 0xbb, 0x02, 0xaa, 0xaa, 0xbb, 0x03 },
             { 0xaa, 0xaa, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
             { 0xaa, 0xaa, 0xbb, 0x00, 0xaa, 0xaa, 0xbb, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
             { 0xaa, 0xaa, 0xbb, 0x00, 0xaa, 0xaa, 0xbb, 0x01, 0xaa, 0xaa, 0xbb, 0x02, 0x00, 0x00, 0x00, 0x00 }}

         },
         /*
          * tunnel_ip6_sip
          */
        { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xee, 0xff, 0xff, 0xee, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21 },
        /*
         * tunnel DIP mask
         */
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        /*
         * esr usp rch port
         */
        40,
       /*
        * next protocol above SRv6 which is IPv4
        */
        4,
         /*
          * l2_termination_vsi - when ETHoSRv6 tunnel terminated LIF FODO (VSI)
          */
         2,
         /*
          * when ETHoSRv6, MACT host entry for Bridging FWD lookup
          */
         { 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c }
};

int srv6_term_tunnel_id;
int srv6_control_lif_id;
/**
 *  SBFD reflector ipv6 work with SRV6
 */
int sbfd_reflector_ipv6_with_srv6 = 0;
int sbfd_reflector_ipv6_with_srv6_usp=0;
int ingress_tunnel_with_esem=0;

int add_trap_as_destination = 1;
int test_tunnel = 0;

uint32 sal_ceil_func(uint32 numerators , uint32 denominator)
{
    uint32  result;
    if (denominator == 0) {
        return 0xFFFFFFFF;
    }
    result = numerators / denominator;
    if (numerators % denominator != 0) {
        result++;
    }
    return result;
}

int
srv6_endpoint_tunnel_type_ipany(int unit, int is_cross_connect, int out_port, int fec)
{
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv;

    /*
     * Configure the keys for the resulting LIF on termination (SIP, DIP, VRF)
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);

    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;

    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, cint_srv6_tunnel_info.tunnel_ipv6_dip_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;

    if (is_cross_connect)
    {
        tunnel_term_set.flags = BCM_TUNNEL_TERM_CROSS_CONNECT;
    }
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create \n");
        return rv;
    }

    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

    if (is_cross_connect)
    {
        bcm_vswitch_cross_connect_t gports;
        bcm_vswitch_cross_connect_t_init(&gports);
        gports.port1 = tunnel_term_set.tunnel_id;

        if (fec && out_port)
        {
            printf("Error - Either FEC 0x%x or OutPort 0x%x must be 0 (Invalid) \n", fec, out_port);
            return BCM_E_PARAM;
        }
        else if (fec)
        {
            BCM_GPORT_FORWARD_PORT_SET(gports.port2, fec);
        }
        else if (out_port)
        {
            BCM_GPORT_LOCAL_SET(gports.port2, out_port);
        }
        else
        {
            printf("Error - Either FEC 0x%x or OutPort 0x%x must be non 0 (Valid) \n", fec, out_port);
            return BCM_E_PARAM;
        }

        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        rv = bcm_vswitch_cross_connect_add(unit, &gports);
        if(rv != BCM_E_NONE)
        {
            printf("Error bcm_vswitch_cross_connect_add. rv = %d \n", rv);
            return rv;
        }
    }

    /*
     * Configure the termination resulting LIF's FODO to VRF
     */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = 1;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error intf_ingress_rif_set. rv = %d \n", rv);
        return rv;
    }

    return rv;
}

int
srv6_endpoint_tunnel(int unit, uint8 next_protocol_is_l2, uint8 is_termination, uint8 lif_cs_profile, int is_p2p_tunnel)
{
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv;

    /*
     * Configure the keys for the resulting LIF on termination (SIP, DIP, VRF)
     */
    if(sbfd_reflector_ipv6_with_srv6_usp)
    {
        ip6_dip[12] = 0;
        ip6_dip[13] = 0;
    }
    bcm_tunnel_terminator_t_init(&tunnel_term_set);

    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;

    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, cint_srv6_tunnel_info.tunnel_ipv6_dip_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    if(is_termination)
    {
        tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_STAT_ENABLE;
        /* If it's ESR USP, shortpipe and uniform are supported
         * Uniform: take QoS info from SRv6 tunnel
         * Shortpipe: take QoS info from fwd layer
         */
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    }
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthIn6;
    }

    /*
     * Check if the user wants to create P2P tunnel termination
     */
    if(is_p2p_tunnel)
    {
        tunnel_term_set.tunnel_class = 7;
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
    }
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }

    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

    if(is_p2p_tunnel)
    {
        bcm_tunnel_terminator_config_key_t p2p_tunnel_terminator_config_key;
        bcm_tunnel_terminator_config_action_t p2p_tunnel_terminator_config_action;

        p2p_tunnel_terminator_config_key.dip6 = tunnel_term_set.dip6;
        p2p_tunnel_terminator_config_key.dip6_mask = tunnel_term_set.dip6_mask;
        p2p_tunnel_terminator_config_key.flags = tunnel_term_set.flags;
        p2p_tunnel_terminator_config_key.type = tunnel_term_set.type;
        p2p_tunnel_terminator_config_key.vrf = tunnel_term_set.vrf;

        p2p_tunnel_terminator_config_action.tunnel_class = 7;

        rv = bcm_tunnel_terminator_config_add(unit, 0, p2p_tunnel_terminator_config_key, p2p_tunnel_terminator_config_action);
        if(rv != BCM_E_NONE)
        {
            printf("Error bcm_tunnel_terminator_config_add. rv = %d \n", rv);
            return rv;
        }
    }

    if (add_trap_as_destination && (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL)))
    {
        /** Configure a host entry, which sets the destination to predifined Trap.
         * The trap is used later on in t he PMF1 stage to identify USP packets eligible for context selection */
        int trap_code;
        bcm_gport_t trap_code_gport;
        rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapSrv6Usp, &trap_code);
        if (rv != BCM_E_NONE)
        {
            printf("trap type get failed: %x \n", rv);
            return rv;
        }

        BCM_GPORT_TRAP_SET(trap_code_gport, trap_code, 7, 0);

        bcm_l3_host_t host;
        bcm_l3_host_t_init(host);

        host.l3a_ip6_addr = ip6_dip;
        host.l3a_vrf = 1;
        host.l3a_port_tgid = trap_code_gport;       /* Trap as destination */
        host.l3a_flags = BCM_L3_IP6;
        rv = bcm_l3_host_add(unit, host);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_l3_host_add failed: %x \n", rv);
            return rv;
        }
        bcm_ip6_t ip6_dip_2 = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xEE,0xEE,0xEE,0x13};
        host.l3a_ip6_addr = ip6_dip_2;
        rv = bcm_l3_host_add(unit, host);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_l3_host_add failed: %x \n", rv);
            return rv;
        }
    }
    else if (add_trap_as_destination && !(*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL)))
    {
        int trap_code;
        bcm_gport_t trap_code_gport;
        rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapSrv6Usp, &trap_code);
        if (rv != BCM_E_NONE)
        {
            printf("trap type get failed: %x \n", rv);
            return rv;
        }
        printf("Trap Code: %x \n", trap_code);
        BCM_GPORT_TRAP_SET(trap_code_gport, trap_code, 7, 0);
        bcm_l3_route_t l3rt;
        bcm_ip6_t mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
        bcm_l3_route_t_init(l3rt);

        l3rt.l3a_ip6_net = ip6_dip;
        l3rt.l3a_ip6_mask = mask;
        l3rt.l3a_vrf = 1;
        l3rt.l3a_port_tgid = trap_code_gport;
        l3rt.l3a_flags = BCM_L3_IP6;
        l3rt.l3a_flags2 = BCM_L3_FLAGS2_FWD_ONLY;

        print l3rt;
        printf("Entering bcm_l3_route_add!!!!!! \n", rv);
        rv = bcm_l3_route_add(unit, l3rt);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_l3_route_add failed: %x \n", rv);
        }

        bcm_ip6_t ip6_dip_2 = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xEE,0xEE,0xEE,0x13};
        l3rt.l3a_ip6_net = ip6_dip_2;
        rv = bcm_l3_route_add(unit, l3rt);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_l3_route_add failed: %x \n", rv);
        }
    }
    /*
     * Set In-LIF profile for iPMF - if the parameter is not '0'
     * -by configuring in the In-LIF profile 2b inlif_profile for IPMF
     */

    if (lif_cs_profile)
    {
        printf("Set In-LIF profile\n");
        rv = bcm_port_class_set(unit, tunnel_term_set.tunnel_id, bcmPortClassFieldIngressVport,
                                lif_cs_profile);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_class_set for IPv6 Tunnel Termination LIF CS profile set \n");
            return rv;
        }
    }

    /*
     * Configure the termination resulting LIF's FODO to VRF
     *  - not needed if resulting LIF's FODO is to be VSI (when ETHoSRv6), because
     *    in this case it's set by 'bcm_tunnel_terminator_create' above with 'vlan' property
     */
    if (!next_protocol_is_l2)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = 1;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        rv = intf_ingress_rif_set(unit, &ingress_rif);
        if (rv != BCM_E_NONE)
        {
            printf("Error intf_ingress_rif_set. rv = %d \n", rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}

int
srv6_endpoint_tunnel_cascaded(int unit, uint8 next_protocol_is_l2, uint8 is_termination, uint8 lif_cs_profile, int is_p2p_tunnel)
{
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_mask_96 = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0,0,0,0};
    bcm_ip6_t ip6_mask_96_func = {0,0,0,0,0,0,0,0,0,0,0,0,0xFF,0xFF,0,0};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv;

    /*
     * Configure the keys for the resulting LIF on termination (SIP, DIP, VRF)
     */
    if(sbfd_reflector_ipv6_with_srv6_usp)
    {
        ip6_dip[12] = 0;
        ip6_dip[13] = 0;
    }
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_96, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    tunnel_term_set.flags = BCM_TUNNEL_TERM_UP_TO_96_LOCATOR_SEGMENT_ID;
    if(is_termination)
    {
        tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_STAT_ENABLE | BCM_TUNNEL_TERM_UP_TO_96_LOCATOR_SEGMENT_ID;
        /* If it's ESR USP, shortpipe and uniform are supported
         * Uniform: take QoS info from SRv6 tunnel
         * Shortpipe: take QoS info from fwd layer
         */
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    }
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthIn6;
    }

    /*
     * Check if the user wants to create P2P tunnel termination
     */
    if(is_p2p_tunnel)
    {
        tunnel_term_set.tunnel_class = 7;
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
    }
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }

    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_96_func, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.default_tunnel_id = srv6_term_tunnel_id;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    if(is_termination)
    {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_STAT_ENABLE;
        /* If it's ESR USP, shortpipe and uniform are supported
         * Uniform: take QoS info from SRv6 tunnel
         * Shortpipe: take QoS info from fwd layer
         */
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    }
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthCascadedFunct;
    }

    /*
     * Check if the user wants to create P2P tunnel termination
     */
    if(is_p2p_tunnel)
    {
        tunnel_term_set.tunnel_class = 7;
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
    }
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }
    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;


    if(is_p2p_tunnel)
    {
        bcm_tunnel_terminator_config_key_t p2p_tunnel_terminator_config_key;
        bcm_tunnel_terminator_config_action_t p2p_tunnel_terminator_config_action;

        p2p_tunnel_terminator_config_key.dip6 = tunnel_term_set.dip6;
        p2p_tunnel_terminator_config_key.dip6_mask = tunnel_term_set.dip6_mask;
        p2p_tunnel_terminator_config_key.flags = tunnel_term_set.flags;
        p2p_tunnel_terminator_config_key.type = tunnel_term_set.type;
        p2p_tunnel_terminator_config_key.vrf = tunnel_term_set.vrf;

        p2p_tunnel_terminator_config_action.tunnel_class = 7;

        rv = bcm_tunnel_terminator_config_add(unit, 0, p2p_tunnel_terminator_config_key, p2p_tunnel_terminator_config_action);
        if(rv != BCM_E_NONE)
        {
            printf("Error bcm_tunnel_terminator_config_add. rv = %d \n", rv);
            return rv;
        }
    }

    /** Configure a host entry, which sets the destination to predifined Trap.
     * The trap is used later on in t he PMF1 stage to identify USP packets eligible for context selection */
    int trap_code;
    bcm_gport_t trap_code_gport;
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapSrv6Usp, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("trap type get failed: %x \n", rv);
    }

    BCM_GPORT_TRAP_SET(trap_code_gport, trap_code, 7, 0);

    bcm_l3_host_t host;
    bcm_l3_host_t_init(host);

    host.l3a_ip6_addr = ip6_dip;
    host.l3a_vrf = 1;
    host.l3a_port_tgid = trap_code_gport;       /* Trap as destination */
    host.l3a_flags = BCM_L3_IP6;
    rv = bcm_l3_host_add(unit, host);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_l3_host_add failed: %x \n", rc);
    }
    bcm_ip6_t ip6_dip_2 = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xEE,0xEE,0xEE,0x13};
    host.l3a_ip6_addr = ip6_dip_2;
    rv = bcm_l3_host_add(unit, host);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_l3_host_add failed: %x \n", rc);
    }

    /*
     * Set In-LIF profile for iPMF - if the parameter is not '0'
     * -by configuring in the In-LIF profile 2b inlif_profile for IPMF
     */

    if (lif_cs_profile)
    {
        printf("Set In-LIF profile\n");
        rv = bcm_port_class_set(unit, tunnel_term_set.tunnel_id, bcmPortClassFieldIngressVport,
                                lif_cs_profile);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_class_set for IPv6 Tunnel Termination LIF CS profile set \n");
            return rv;
        }
    }

    /*
     * Configure the termination resulting LIF's FODO to VRF
     *  - not needed if resulting LIF's FODO is to be VSI (when ETHoSRv6), because
     *    in this case it's set by 'bcm_tunnel_terminator_create' above with 'vlan' property
     */
    if (!next_protocol_is_l2)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = 1;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        rv = intf_ingress_rif_set(unit, &ingress_rif);
        if (rv != BCM_E_NONE)
        {
            printf("Error intf_ingress_rif_set. rv = %d \n", rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}

int
srv6_endpoint_udp_tunnel(int unit, uint8 next_protocol_is_l2, uint8 is_termination)
{
    int vrf = 1;
    uint32 ipv4_dip = 0x01020304;
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_dip_2 = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xEE,0xEE,0xEE,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv;

    /*
     * Configure the keys for the resulting LIF on termination (SIP, DIP, VRF)
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeUdp6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_2, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
       printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
       return rv;
    }

    /*
     * Configure the termination resulting LIF's FODO to VRF
     *  - not needed if resulting LIF's FODO is to be VSI (when ETHoSRv6), because
     *    in this case it's set by 'bcm_tunnel_terminator_create' above with 'vlan' property
     */
    if (!next_protocol_is_l2)
    {
       l3_ingress_intf_init(&ingress_rif);
       ingress_rif.vrf = 1;
       BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
       rv = intf_ingress_rif_set(unit, &ingress_rif);
       if (rv != BCM_E_NONE)
       {
           printf("Error intf_ingress_rif_set. rv = %d \n", rv);
           return rv;
       }
    }

    /*
     * Configure the keys for the resulting LIF on termination (SIP, DIP, VRF)
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_2, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    if(is_termination)
    {
        tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
        /* If it's ESR USP, shortpipe and uniform are supported
         * Uniform: take QoS info from SRv6 tunnel
         * Shortpipe: take QoS info from fwd layer
         */
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    }
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthIn6;
    }

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }

    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

    /*
     * Configure the termination resulting LIF's FODO to VRF
     *  - not needed if resulting LIF's FODO is to be VSI (when ETHoSRv6), because
     *    in this case it's set by 'bcm_tunnel_terminator_create' above with 'vlan' property
     */
    if (!next_protocol_is_l2)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = 1;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        rv = intf_ingress_rif_set(unit, &ingress_rif);
        if (rv != BCM_E_NONE)
        {
            printf("Error intf_ingress_rif_set. rv = %d \n", rv);
            return rv;
        }
    }

    if (host_entry_add)
    {
        /*
         * Add host entry for UDHoSRv6oIPv6oETH -> UDHoIPv6oETH where the packet does two passes. USP + original IPv6 is the FWD layer
         */
        if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
        {
            bcm_ip6_t full_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

            bcm_l3_route_t l3rt;
            bcm_l3_route_t_init(l3rt);

            l3rt.l3a_ip6_net = ip6_dip;
            l3rt.l3a_ip6_mask = full_mask;
            l3rt.l3a_vrf = vrf;
            l3rt.l3a_intf = ip_route_fec;       /* FEC-ID */
            l3rt.l3a_flags = BCM_L3_IP6;
            l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;

            rv = bcm_l3_route_add(unit, l3rt);
            if (rv != BCM_E_NONE)
            {
                printf("bcm_l3_route_add failed: %x \n", rv);
            }
        }
        else
        {
            rv = add_host_ipv6(unit, ip6_dip, vrf, ip_route_fec);
            if (rv != BCM_E_NONE)
            {
                printf("Error add_host_ipv6: vrf = %d, fec = %d, err_id = %d\n", vrf, ip_route_fec, rv);
                return rv;
            }
        }
    }

    /*
     * Add host entry for IPv4UDHoSRv6oIPv6oETH -> IPv4oETH - where the packet does two passes: USP+IPv6UDP TT+IPv4 FWD
     */
    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        bcm_l3_route_t l3rt;
        bcm_l3_route_t_init(l3rt);

        l3rt.l3a_vrf = vrf;
        l3rt.l3a_intf = ip_route_fec;       /* FEC-ID */
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
        l3rt.l3a_subnet = ipv4_dip;
        l3rt.l3a_ip_mask = 0xffffffff;
        l3rt.l3a_vrf = vrf;

        rv = bcm_l3_route_add(unit, l3rt);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_l3_route_add failed: %x \n", rv);
            return rv;
        }
    }
    else
    {
        rv = add_host_ipv4(unit, ipv4_dip, vrf, ip_route_fec, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error add_host_ipv4: vrf= %d, fec = %d, err_id = %d\n", vrf, ip_route_fec, rv);
            return rv;
        }
    }
    return rv;
}

int
srv6_endpoint_tunnel_prefix_function_classic_sid(int unit)
{
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_mask_96_loc = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_mask_96_func = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00};
    bcm_ip6_t ip6_mask_64_loc = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_mask_64_func = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_dip_96 = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_dip_64 = {0x12,0x34,0,0,0xFF,0xFE,0xFF,0x15,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_ip6_t ip6_dip_next_endpoint = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0,0,0xFF,0x13};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv;
    bcm_gport_t default_tunnel;
    int fec;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    fec += 0x500;

    /*
     * 1. Create 96 locator tunnel VRF is 10
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.flags = BCM_TUNNEL_TERM_UP_TO_96_LOCATOR_SEGMENT_ID;
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_96, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_96_loc, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }
    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = 10;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error intf_ingress_rif_set. rv = %d \n", rv);
        return rv;
    }
    rv = add_route_ipv6(unit, ip6_dip_next_endpoint, ip6_mask, 10, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv6: vrf = 10");
        return rv;
    }
    default_tunnel = tunnel_term_set.tunnel_id;
    /*
     * 2. Create 96 func tunnel VRF is 20
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
    tunnel_term_set.default_tunnel_id = default_tunnel;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_96, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_96_func, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }
    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = 20;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error intf_ingress_rif_set. rv = %d \n", rv);
        return rv;
    }
    rv = add_route_ipv6(unit, ip6_dip_next_endpoint, ip6_mask, 20, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv6: vrf = 20");
        return rv;
    }

    /*
     * 3. Create 64 locator tunnel VRF is 30
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.flags = BCM_TUNNEL_TERM_UP_TO_64_LOCATOR_SEGMENT_ID;
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_64, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_64_loc, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }
    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = 30;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error intf_ingress_rif_set. rv = %d \n", rv);
        return rv;
    }
    rv = add_route_ipv6(unit, ip6_dip_next_endpoint, ip6_mask, 30, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv6: vrf = 30");
        return rv;
    }
    default_tunnel = tunnel_term_set.tunnel_id;
    /*
     * 2. Create 96 func tunnel VRF is 40
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
    tunnel_term_set.default_tunnel_id = default_tunnel;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_64, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_64_func, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }
    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = 40;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error intf_ingress_rif_set. rv = %d \n", rv);
        return rv;
    }
    rv = add_route_ipv6(unit, ip6_dip_next_endpoint, ip6_mask, 40, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv6: vrf = 40");
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * This function creates an SRv6 Endpoint, with P2P information, using the BCM_TUNNEL_TERM_CROSS_CONNECT flag
 * The forwarding information is set with the bcm_vswitch_cross_connect_add API.
 * Forwarding information must be a FEC
 */
int
srv6_endpoint_tunnel_cross_connect(int unit, int out_port, int fec, uint8 next_protocol_is_l2)
{
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv;

    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.flags = BCM_TUNNEL_TERM_CROSS_CONNECT;

    tunnel_term_set.vrf = 1;
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = 2;
    }

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }

    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = tunnel_term_set.tunnel_id;


    if (fec && out_port)
    {
        printf("Error - Either FEC 0x%x or OutPort 0x%x must be 0 (Invalid) \n", fec, out_port);
        return BCM_E_PARAM;
    }
    else if (fec)
    {
        BCM_GPORT_FORWARD_PORT_SET(gports.port2, fec);
    }
    else if (out_port)
    {
        BCM_GPORT_LOCAL_SET(gports.port2, out_port);
    }
    else
    {
        printf("Error - Either FEC 0x%x or OutPort 0x%x must be non 0 (Valid) \n", fec, out_port);
        return BCM_E_PARAM;
    }


    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_vswitch_cross_connect_add. rv = %d \n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * This function creates an SRv6 uSID Endpoint, with P2P information, using the BCM_TUNNEL_TERM_CROSS_CONNECT flag
 * The forwarding information is set with the bcm_vswitch_cross_connect_add API.
 * Forwarding information must be a FEC
 */
int
srv6_usid_endpoint_tunnel_cross_connect(int unit, int out_port, int fec, uint8 next_protocol_is_l2, uint8 is_default)
{

    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_usid_prefix_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_mask_func = {0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_dip  = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x00, 0xEE, 0x01, 0xEE, 0x02, 0xEE, 0x03, 0xEE, 0x04, 0xEE, 0x05};
    bcm_ip6_t ip6_sip  = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xee, 0xff, 0xff, 0xee, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21};
    bcm_ip6_t ip6_usid_48b_prefix_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_mask_48b_prefix_func = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00};
    int is_prefix_48b_enabled = *dnxc_data_get(unit, "srv6", "general", "srv6_usid_prefix_48b_enable", NULL);

    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv, flags = 0;

    bcm_tunnel_type_t tunnel_type;

    tunnel_type = bcmTunnelTypeIpAnyIn6;
    if (is_default)
    {
        flags = BCM_TUNNEL_TERM_CASCADED_MISS_DISABLE_TERM;
    }
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = tunnel_type;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, (is_prefix_48b_enabled ? ip6_usid_48b_prefix_mask : ip6_usid_prefix_mask), 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.flags = BCM_TUNNEL_TERM_MICRO_SEGMENT_ID | BCM_TUNNEL_TERM_CROSS_CONNECT | flags;

    tunnel_term_set.vrf = 1;
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = 2;
    }

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }

    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;



    if (is_default)
    {

       /*
        * 2. Create 96 func tunnel VRF is 20
        */
       bcm_tunnel_terminator_t_init(&tunnel_term_set);
       tunnel_term_set.flags = BCM_TUNNEL_TERM_CROSS_CONNECT;
       tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
       tunnel_term_set.default_tunnel_id = srv6_term_tunnel_id;
       sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
       sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
       sal_memcpy(tunnel_term_set.dip6_mask, (is_prefix_48b_enabled ? ip6_mask_48b_prefix_func : ip6_mask_func), 16);
       sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
       tunnel_term_set.vrf = 5;
       tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
       tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
       tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;

       /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
       if (next_protocol_is_l2)
       {
           tunnel_term_set.vlan = 2;
           tunnel_term_set.type = bcmTunnelTypeEthCascadedFunct;
       }
       rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
       if(rv != BCM_E_NONE)
       {
           printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
           return rv;
       }
       srv6_term_tunnel_id = tunnel_term_set.tunnel_id;
    }

    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = tunnel_term_set.tunnel_id;


    if (fec && out_port)
    {
        printf("Error - Either FEC 0x%x or OutPort 0x%x must be 0 (Invalid) \n", fec, out_port);
        return BCM_E_PARAM;
    }
    else if (fec)
    {
        BCM_GPORT_FORWARD_PORT_SET(gports.port2, fec);
    }
    else if (out_port)
    {
        BCM_GPORT_LOCAL_SET(gports.port2, out_port);
    }
    else
    {
        printf("Error - Either FEC 0x%x or OutPort 0x%x must be non 0 (Valid) \n", fec, out_port);
        return BCM_E_PARAM;
    }


    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_vswitch_cross_connect_add. rv = %d \n", rv);
        return rv;
    }
    return BCM_E_NONE;
}


/*
 * This function creates an SRv6 GSID Endpoint, with P2P information, using the BCM_TUNNEL_TERM_CROSS_CONNECT flag
 * The forwarding information is set with the bcm_vswitch_cross_connect_add API.
 * Forwarding information must be a FEC
 */
int
srv6_gsid_endpoint_tunnel_cross_connect(int unit, int out_port, int fec, uint8 next_protocol_is_l2, int is_prefix_64b)
{

    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    /** GSID locator mask is Prefix 48b + GSIDs MSB 16b = 64b */
    bcm_ip6_t ip6_gsid_prefix_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_gsid_prefix_64b_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00};

    bcm_ip6_t ip6_dip  = {0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0x33, 0x33, 0xBB, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bcm_ip6_t ip6_dip_64b  = {0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0XDD, 0x33, 0x33, 0xBB, 0x03, 0x00, 0x00, 0x00, 0x00};
    bcm_ip6_t ip6_sip  = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xee, 0xff, 0xff, 0xee, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21};

    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv;

    bcm_tunnel_type_t tunnel_type;

    tunnel_type = bcmTunnelTypeIpAnyIn6;

    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = tunnel_type;
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    if (is_prefix_64b)
    {
        sal_memcpy(tunnel_term_set.dip6, ip6_dip_64b, 16);
        sal_memcpy(tunnel_term_set.dip6_mask, ip6_gsid_prefix_64b_mask, 16);
    }
    else
    {
        sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
        sal_memcpy(tunnel_term_set.dip6_mask, ip6_gsid_prefix_mask, 16);
    }
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.flags = BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID | BCM_TUNNEL_TERM_CROSS_CONNECT;

    tunnel_term_set.vrf = 1;
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = 2;
    }

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }

    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = tunnel_term_set.tunnel_id;


    if (fec && out_port)
    {
        printf("Error - Either FEC 0x%x or OutPort 0x%x must be 0 (Invalid) \n", fec, out_port);
        return BCM_E_PARAM;
    }
    else if (fec)
    {
        BCM_GPORT_FORWARD_PORT_SET(gports.port2, fec);
    }
    else if (out_port)
    {
        BCM_GPORT_LOCAL_SET(gports.port2, out_port);
    }
    else
    {
        printf("Error - Either FEC 0x%x or OutPort 0x%x must be non 0 (Valid) \n", fec, out_port);
        return BCM_E_PARAM;
    }


    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_vswitch_cross_connect_add. rv = %d \n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    Update the FECs IDs
 */
int
srv6_update_fecs_values(int unit)
{

    int rv;
    int first_fec_in_hier;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    /** Hierarchy 1 FECs */
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID] = first_fec_in_hier + 0x700;
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_L2VPN_FEC_ID] = first_fec_in_hier + 0x701;
    cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID] = first_fec_in_hier + 0x1;
    cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID] = first_fec_in_hier + 0x2;
    cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID] = first_fec_in_hier + 0x3;

    /** Hierarchy 2 FECs */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_FEC_ID] = first_fec_in_hier + 0x700;
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_L2VPN_FEC_ID] = first_fec_in_hier + 0x701;

    /** Hierarchy 3 FECs */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 2, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    cint_srv6_tunnel_info.tunnel_fec_id[ARP_AC_FEC_ID] = first_fec_in_hier + 0x702;

    return rv;
}

/*
 *    Configuring ISR (Ingress SRv6 Tunnel)
 *    This will create encapsulation of SRH,SIDs and IPv4 onto the packet
 *
 *    Specifically:
 *    - creating EEDB entries for SRH Base, SIDs[0..2], IPv6 Tunnel (SRv6 Type)
 *    - creation of x1 local-out-LIFs to return following LL entries from EEDB:
 *       1. SRH Base -> SID0 -> SID1 -> SID2 -> IPv6 Tunnel (LL)
 */
int
srv6_ingress_tunnel_config(
                            int unit,
                            int in_port,             /** Incoming port of SRV6 Ingress Tunnel */
                            int in_port_eth,         /** Incoming ETH port of SRV6 Ingress Tunnel */
                            int out_port,            /** Outgoing port of SRV6 Ingress Tunnel */
                            int nof_sids,
                            int secondary_srh_flags,  /** additional SRH Encap flags such as BCM_SRV6_SRH_BASE_INITIATOR_UNIFIED */
                            int secondary_sids_flags, /** additional SIDs Encap flags such as BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED */
                            int is_t_insert_encap,
                            int last_sid_is_dip,      /** if set, last SID (VPN) is given by IPv6 tunnel encapsulation */
                            int place_holder_arg_ignore)
{
    int rv;
    int first_fec_in_hier;
    char *proc_name = "srv6_ingress_tunnel_config";
    int vpn_from_sid = 1;
    if((last_sid_is_dip == 1))
    {
        vpn_from_sid = 0;
    }

    /*
     * Update FEC IDS
     */
    rv = srv6_update_fecs_values(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, srv6_get_fec_values\n");
        return rv;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */

    /** SRV6 Ingress Tunnel Eth RIF based on Port only */
    rv = in_port_intf_set(unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    if(in_port_eth != 0)
    {
        /** SRV6 Ingress Tunnel Eth RIF based on Port only */
        rv = in_port_intf_set(unit, in_port_eth, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
            return rv;
        }
    }

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */

    /** In SRV6 Ingress and End-Point-1 use the same out_port, (End-Point-1 and End-Point-2 receive from same Port, with different VLAN) */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port of SRV6 Ingress Tunnel %d\n",proc_name,out_port);
        return rv;
    }


    /*
     * 3. Create ETH-RIF and set its properties
     */

    /** SRV6 Ingress Tunnel My-MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], cint_srv6_tunnel_info.intf_in_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
        return rv;
    }

    /** SRV6 Ingress Tunnel out-port SA  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.tunnel_vid, cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }


    /*
     * 4. Set Incoming ETH-RIF properties
     */

    l3_ingress_intf ingress_rif;

    /** SRV6 Ingress Tunnel VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[INGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /*
     * Configure IPv6 tunnel for the SRv6 endpoint into srv6 tunnel case
     */
    bcm_ip6_t ip6_my_dip = {0x88,0x88,0x77,0x77,0x66,0x66,0x55,0x55,0x44,0x44,0x33,0x33,0x22,0x22,0x11,0x11};
    bcm_ip6_t ip6_my_dip_mask = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif_endpoint;
    /*
     * Configure the keys for the resulting LIF on termination (SIP, DIP, VRF)
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_my_dip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_my_dip_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }


    l3_ingress_intf_init(&ingress_rif_endpoint);
    ingress_rif_endpoint.vrf = 171 /* VRF of endpoint tunnel*/;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif_endpoint.intf_id, tunnel_term_set.tunnel_id);
    rv = intf_ingress_rif_set(unit, &ingress_rif_endpoint);
    if (rv != BCM_E_NONE)
    {
        printf("Error intf_ingress_rif_set for ingress_rif_endpoint. rv = %d \n", rv);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */

    uint32 flags2 = 0;

    flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    /** SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1*/
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /** indication that SID is UNIFIED */
    int is_unified_sid = secondary_srh_flags & BCM_SRV6_SRH_BASE_INITIATOR_UNIFIED;

    /*
     * 6. SRV6 FEC Entries
     */

    bcm_gport_t gport;
    bcm_gport_t gport_l2vpn;
    int fec_flags2 = 0;

    int srv6_basic_lif_encode;
    int encoded_fec1;
    int encoded_fec1_l2vpn;
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);

    if(is_t_insert_encap)
    {
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[ARP_AC_FEC_ID]);
    }
    else
    {
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_FEC_ID]);
        BCM_GPORT_FORWARD_PORT_SET(gport_l2vpn, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_L2VPN_FEC_ID]);
    }

    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], 0 , srv6_basic_lif_encode, gport, 0,
                                           fec_flags2, 0, 0,&encoded_fec1);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_1 only\n",proc_name);
        return rv;
    }
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_L2VPN_FEC_ID], 0 , srv6_basic_lif_encode, gport_l2vpn, 0,
                                           fec_flags2, 0, 0,&encoded_fec1_l2vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_1_L2VPN only\n",proc_name);
        return rv;
    }

    /** in T.Insert encap, IPv6 entry is not needed, the Tunnel is already IPv6 */
    if (!is_t_insert_encap)
    {
        int srv6_first_lif_encode;
        int encoded_fec2;
        int encoded_fec2_l2vpn;
        BCM_L3_ITF_SET(srv6_first_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[ARP_AC_FEC_ID]);
        rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_FEC_ID], 0 , srv6_first_lif_encode, gport, BCM_L3_2ND_HIERARCHY,
                fec_flags2, 0, 0,&encoded_fec2);

        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC_2 only, fec\n");
            return rv;
        }
        BCM_L3_ITF_SET(srv6_first_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_L2VPN_GLOBAL_LIF]);
        rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_L2VPN_FEC_ID], 0 , srv6_first_lif_encode, gport, BCM_L3_2ND_HIERARCHY,
                fec_flags2,0, 0,&encoded_fec2_l2vpn);

        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC_2_L2VPN only, fec\n");
            return rv;
        }

    }

    /** Define ARP+AC FEC Entry */
    BCM_GPORT_LOCAL_SET(gport, out_port);
    int encoded_fec3_arp_ac;

    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[ARP_AC_FEC_ID], 0, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], gport, BCM_L3_3RD_HIERARCHY,
                                           fec_flags2, 0, 0, &encoded_fec3_arp_ac);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC of ARP+AC only\n", proc_name);
        return rv;
    }


    /*
     * 7. Add route entry - we work with specific DIP addresses, so we don't need to add best match route entries.
     */

    rv = add_route_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.route_ipv4_mask, cint_srv6_tunnel_info.vrf_in[INGRESS], cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
    {
        printf("%s(): Error, in function add_route_ipv4() VRF:%d\n",proc_name, cint_srv6_tunnel_info.vrf_in[INGRESS]);
    }
    rv = add_route_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.route_ipv4_mask, 171 /* VRF of endpoint tunnel*/, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_route_ipv4() VRF:%d\n",proc_name, 171);
    }
    rv = add_route_ipv6(unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_srv6_tunnel_info.route_ipv6_mask, cint_srv6_tunnel_info.vrf_in[INGRESS], cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_route_ipv6() VRF:%d\n",proc_name, cint_srv6_tunnel_info.vrf_in[INGRESS]);
    }
    rv = add_route_ipv6(unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_srv6_tunnel_info.route_ipv6_mask, 171 /* VRF of endpoint tunnel*/, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_route_ipv6() VRF:%d\n",proc_name, 171);
    }



    /*
     * 8. Add host/mac entry
     */

    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL) && add_ipv6_host_entry)
    {
        /** SRV6 Tunnel Ingress Forwarding to FEC Hierarchy to bring out IPV6, SRH, SIDs*/
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);

        rv = add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, 0, gport);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv4() for SRV6 Ingress Tunnel\n",proc_name);
            return rv;
        }
        rv = add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, 171 /* VRF of endpoint tunnel*/, 0, 0, gport);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv4() for SRV6 Ingress Tunnel\n",proc_name);
            return rv;
        }
        rv = add_host_ipv6_encap_dest(unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, 0, gport);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv6() for SRV6 Ingress Tunnel\n",proc_name);
            return rv;
        }
        rv = add_host_ipv6_encap_dest(unit, cint_srv6_tunnel_info.route_ipv6_dip, 171 /* VRF of endpoint tunnel*/, 0, 0, gport);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv6() for SRV6 Ingress Tunnel\n",proc_name);
            return rv;
        }
    }

    /** Add MACT entry for bridging into SRv6 tunnel */
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_L2VPN_FEC_ID]);
    cint_srv6_tunnel_info.l2_fwd_vsi = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, cint_srv6_tunnel_info.l2_fwd_mac, cint_srv6_tunnel_info.l2_fwd_vsi);
    l2addr.port = gport;
    l2addr.encap_id = 0;
    l2addr.flags = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    /*
     * 9. Configuring ISR SRv6 Tunnel
     *
     *    - creating EEDB entries for SRH Base, SIDs[0..2], IPv6 Tunnel (SRv6 Type)
     *    - creation of x2 local-out-LIFs to return following entries from EEDB:
     *       1. SRH Base
     *       2. SID0 -> SID1 -> SID2 -> IPv6 Tunnel (LL)
     */


    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    bcm_gport_t next_sid_tunnel_id = 0;
    
    /** in T.Insert encap, IPv6 entry is not needed, the Tunnel is already IPv6 */
    if (!is_t_insert_encap)
    {
       bcm_tunnel_initiator_t_init(&tunnel_init_set);
       bcm_l3_intf_t_init(&intf);
       tunnel_init_set.type = bcmTunnelTypeSR6;
       /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
       sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);

       /** check if last SID is brought by IPv6 Tunnel */
       if (last_sid_is_dip)
       {
           sal_memcpy(tunnel_init_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0], 16);
       }

       /** we will pass the global-out-LIF id for 1st SID */
       tunnel_init_set.flags = BCM_TUNNEL_WITH_ID | BCM_TUNNEL_INIT_STAT_ENABLE;
       /** convert 1st SID global-LIF id to GPORT */
       BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);

       tunnel_init_set.ttl = 128;
       tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;

       rv = bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set);

       if (rv != BCM_E_NONE)
       {
          printf("Error, bcm_tunnel_initiator_create for IPv6 Tunnel \n");
          return rv;
       }
       bcm_tunnel_initiator_t_init(&tunnel_init_set);
       bcm_l3_intf_t_init(&intf);
       tunnel_init_set.type = bcmTunnelTypeEthSR6;
       /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
       sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);

       /** check if last SID is brought by IPv6 Tunnel */
       if (last_sid_is_dip)
       {
           sal_memcpy(tunnel_init_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0], 16);
       }

       /** we will pass the global-out-LIF id for 1st SID */
       tunnel_init_set.flags = BCM_TUNNEL_WITH_ID | BCM_TUNNEL_INIT_STAT_ENABLE;
       /** convert 1st SID global-LIF id to GPORT */
       BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_L2VPN_GLOBAL_LIF]);

       tunnel_init_set.ttl = 128;
       tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;

       rv = bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set);

       if (rv != BCM_E_NONE)
       {
          printf("Error, bcm_tunnel_initiator_create for IPv6 Tunnel \n");
          return rv;
       }
       /** to store next SID GPORT and init for SID0 to IPv6 Tunnel */
       next_sid_tunnel_id = tunnel_init_set.tunnel_id;
    }


    /*
     * define the SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t  sid_info;

    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */

    int sid_idx;
    int sid_idx_start = 0; /** from which SID to start encap - SID0 (last one) may be given by IPv6 Tunnel */

    if (vpn_from_sid == 0)
    {
        sid_idx_start = 1;
    }

    int classic_nof_sids = nof_sids;

    /** if case if UNIFIED-SID, then nof_sids is given on 32b sizes, calculate classical_nof_sids */
    if (is_unified_sid)
    {
        /** round up by dividing nof UNIFIED SIDs by 4 */
        classic_nof_sids = sal_ceil_func(nof_sids, SRV6_UNIFIED_CLASSICAL_SID_FACTOR);
    }

    for (sid_idx = sid_idx_start; sid_idx < classic_nof_sids; sid_idx++)
    {

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (sid_idx != sid_idx_start)
        {
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /* if this is the final SID to add, this SID is the first SID in list and has the global-LIF to the SID-LL
         * -don't also allow additional flags such as BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED - it must have Global-LIF
         */

        /** don't use any special flags but external additional flags as BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED */
        sid_info.flags = secondary_sids_flags;

        /** must set the tunnel id to 0 if not using WITH_ID flag */
        sid_info.tunnel_id = 0;

        /** Copy as SID value either classical SID or Unified-SID */
        if (is_unified_sid)
        {
            int unified_idx_in_sid = nof_sids % SRV6_UNIFIED_CLASSICAL_SID_FACTOR;
            sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_classic_unified_sid_arr[SID0 - sid_idx][unified_idx_in_sid], 16);

        }
        else
        {
            sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);
        }

        if (classic_nof_sids == 5 && (vpn_from_sid == 1))
        {
            if (sid_idx == 0)
            {
                /** set eedb phase to ARP, outermost SID/ VPN SID */
                sid_info.encap_access = bcmEncapAccessArp;
            }
            else
            {
                /** set to EEDB entry of SID0 */
                sid_info.encap_access = bcmEncapAccessTunnel4 - (sid_idx - sid_idx_start - 1);
            }
        }
        else
        {
            /** set to EEDB entry of SID0 */
            sid_info.encap_access = bcmEncapAccessTunnel4 - (sid_idx - sid_idx_start);
        }

        /** in case of T.Insert, SID0 should not point to IPv6 tunnel, but to nothing */
        if (is_t_insert_encap && (sid_idx == sid_idx_start))
        {
            sid_info.next_encap_id = 0;
        }
        else 
        {
            /** next_encap_id set to l3 interface - convert it from GPORT to l3_int */
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);
        }

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        rv = bcm_srv6_sid_initiator_create(unit, &sid_info);

        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d \n", sid_idx);
           return rv;
        }
    }

    /*
     * define the SRH Base EEDB entry
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
    /** we will pass the global-out-LIF id + additional SRH flags from test */
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID | secondary_srh_flags;

    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);


    /** in case of last SID is part of IPv6 DIP, nof_sids in SRH is substructed */
    int nof_sids_on_srh_entry = nof_sids;
    int srh_entry_has_ll = 1;
    if (last_sid_is_dip == 1)
    {
        nof_sids_on_srh_entry--;
    }

    if((nof_sids_on_srh_entry == 0))
    {
        srh_entry_has_ll = 0;
    }
    /** if no SIDs present, don't point to garbage but NULL */
    if (srh_entry_has_ll == 0)
    {
        srh_base_info.next_encap_id = 0;
    }
    else
    {
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);
    }

    /** set number of SIDs - true for both UNIFIED-SID and classical case */
    srh_base_info.nof_sids = nof_sids_on_srh_entry;


    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);

    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_dnx_srv6_srh_base_initiator_create \n");
       return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/*
 *  Configuring End.B6 insert
 *    This will create insertion of SRH, SIDs onto the packet
 *    End.B6 is a local SID behaviour, there's no need to look up next SID, so cross connect is used to point to FEC
 *      which points to global out lif for SRH
 *    In egress the EEDB encapsulation is:
 *        SRH Base -> SID3 -> SID2 -> SID1 -> SID0 -> ARP+AC
 */
int
srv6_endpoint_b6_insert(
                            int unit,
                            int in_port,
                            int out_port,
                            int nof_sids)
{
    int rv;
    int first_fec_in_hier;
    char *proc_name = "srv6_endpoint_b6_insert";

    /*
     * Update FEC IDS
     */
    rv = srv6_update_fecs_values(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, srv6_get_fec_values\n");
        return rv;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */

    /** SRV6 Ingress Tunnel Eth RIF based on Port only */
    rv = in_port_intf_set(unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port of SRV6 Ingress Tunnel %d\n",proc_name,out_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */

    /** SRV6 Ingress Tunnel My-MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], cint_srv6_tunnel_info.intf_in_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
        return rv;
    }

    /** SRV6 Ingress Tunnel out-port SA  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.tunnel_vid, cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */

    l3_ingress_intf ingress_rif;

    /** SRV6 Ingress Tunnel VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[INGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */

    uint32 flags2 = 0;
    int arp_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];

    flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    /** SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1*/
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /*
     * 6. SRV6 FEC Entries
     */

    bcm_gport_t gport;
    bcm_gport_t gport_l2vpn;
    int fec_flags2 = 0;

    int srv6_basic_lif_encode;
    int encoded_fec1;
    int encoded_fec1_l2vpn;
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);


    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }


    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], 0 , srv6_basic_lif_encode, gport, 0,
                                           fec_flags2, 0, 0,&encoded_fec1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_1 only\n",proc_name);
        return rv;
    }


    /*
     * 7. Add cross connect entry for End.B6
     *  End.B6 is a local SID behaviour, no need to look up next SID
     */
    rv = srv6_endpoint_tunnel_cross_connect(unit, 0, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in srv6_endpoint_tunnel_cross_connec for End.B6\n", proc_name);
        return rv;
    }

    /*
     * 8. Configuring SIDs
     *
     */

    /*
     * define the x3 SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;

    /** to store next SID GPORT and init for last SID (sid_idx = 0 in below config) to IPv6 Tunnel */
    bcm_gport_t next_sid_tunnel_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS]);

    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */

    int sid_idx;

    for (sid_idx = 0; sid_idx < nof_sids; sid_idx++)
    {

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (sid_idx != 0)
        {
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** don't use any special flags */
        sid_info.flags = 0;

        /** must set the tunnel id to 0 if not using WITH_ID flag */
        sid_info.tunnel_id = 0;

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx;

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (next_sid_tunnel_id != 0)
        {
            /** next_encap_id set to IPv6 interface - convert it from GPORT to l3_int */
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);
        }
        else
        {
            sid_info.next_encap_id = 0;
        }
        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        rv = bcm_srv6_sid_initiator_create(unit, &sid_info);

        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d \n", sid_idx);
           return rv;
        }
    }

    /*
     * define the SRH Base EEDB entry
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;
    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);

    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids;

    /** if no SIDs present, set next pointer to ARP */
    if (nof_sids == 0)
    {
        srh_base_info.next_encap_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];
    }
    else
    {
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);
    }

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s(): Error, bcm_dnx_srv6_srh_base_initiator_create \n", proc_name);
       return rv;
    }

    srv6_srh_base_tunnel_id = srh_base_info.tunnel_id;

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}


int
dnx_basic_example_ipvx_for_srv6_inner(
    int unit,
    int srh_next_protocol,
    int rch_port,
    int out_port,
    int fec)
{
    int diff_const = 10;        /* this constant is to add to below values to enable IPV4 example run with IPV6 in SRV6 example */
    int rv;
    int intf_in = 15 + diff_const;           /* Incoming packet ETH-RIF */
    int intf_out = 100 + diff_const;         /* Outgoing packet ETH-RIF */
    int vrf = 1;                             /* RCH VRF 2nd Pass is equal here to 1st Pass VRF */
    int vsi = 2;                             /* VSI that's used with L2 o SRv6 case for 2nd Pass MACT Lookup */
    int encap_id = 0x1384 + diff_const;         /* ARP-Link-layer (needs to be higher than 8192 for Jer Plus) */
    bcm_mac_t intf_in_mac_address = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x11 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 host = 0x7fffff02;
    bcm_ip6_t route_ipv6_dip = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = 0;
    int vlan = 100 + diff_const;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);
    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;

    char *err_proc_name;
    char *proc_name;

    int tunnel_global_lif = 0x4230 + diff_const; /* Create a SRV6 tunnel with #SID=0 */

    /*
     * Bridge configuration for L2 o SRv6 case
     */
    proc_name = "dnx_basic_example_ipvx_for_srv6_inner";
    printf("%s(): Enter. rch_port %d out_port %d  fec 0x%08X\r\n",proc_name, rch_port, out_port, fec);

    /*
     * encap id for jr2 must be > 2k
     */
    encap_id = 0x1384 + diff_const;

    /*
     * Jr2 myMac assignment is more flexible than in Jer1
     */
    intf_out_mac_address[0] = 0x00;
    intf_out_mac_address[1] = 0x12;
    intf_out_mac_address[2] = 0x34;
    intf_out_mac_address[3] = 0x56;
    intf_out_mac_address[4] = 0x78;
    intf_out_mac_address[5] = 0x9a;

    if(sbfd_reflector_ipv6_with_srv6)
    {
        intf_out_mac_address[0] = 0x00;
        intf_out_mac_address[1] = 0x00;
        intf_out_mac_address[2] = 0x00;
        intf_out_mac_address[3] = 0x00;
        intf_out_mac_address[4] = 0xcd;
        intf_out_mac_address[5] = 0x1d;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */

    /* We don't need to add this configuration for RCH port, this is handled with RCH Context */

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,intf_in);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties
     */

    printf("%s(): Going to call l3__egress_only_encap__create_inner() with encap_id 0x%08X, flags2 0x%08X\n",
                proc_name, encap_id, flags2);
    rv = l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, vlan, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }

if (test_tunnel)
{
    printf("%s(): Going to call bcm_srv6_srh_base_initiator_create() with tunnel_lif 0x%08X, encap_id 0x%08X, flags2 0x%08X\n",
           proc_name, tunnel_global_lif, encap_id, flags2);
    /* Config a SRV6 tunnel with #SID = 0 */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, tunnel_global_lif);
    int next_sid_tunnel_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, encap_id);
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, next_sid_tunnel_id);
    srh_base_info.nof_sids = 0;
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_dnx_srv6_srh_base_initiator_create \n");
       return rv;
    }
    BCM_L3_ITF_SET(encap_id, BCM_L3_ITF_TYPE_LIF, tunnel_global_lif);
}

    /*
     * 6. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */
    int fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    printf("%s(): Create main FEC and set its properties.\r\n",proc_name);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create_inner(unit, fec, 0 , encap_id, gport, 0, fec_flags2,
                                           0, 0,&encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }
    printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);

    /*
     * 8. Add host entry
     */
    int _l3_itf;
    BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, fec);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    if(srh_next_protocol == 4)
    {
        /** Skip host in L3 for APP_V2 */
        if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL) && add_ipv6_host_entry)
        {

            rv = add_host_ipv4(unit, host, vrf, _l3_itf, intf_out, 0);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
                return rv;
            }
        }
        else
        {
            /** L3 Routing */
            rv = add_route_ipv4(unit, host, cint_srv6_tunnel_info.route_ipv4_mask, vrf, fec);
            {
                printf("%s(): Error, in function add_route_ipv4() VRF:%d\n",proc_name, vrf);
            }
        }
    }
    else if(srh_next_protocol == 41)
    {
        bcm_ip6_t route_ipv6_dip = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };

        /** Skip host in L3 for APP_V2 */
        if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL) && add_ipv6_host_entry)
        {

            rv = add_host_ipv6_encap_dest(unit, route_ipv6_dip, vrf, _l3_itf, intf_out, 0);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
                return rv;
            }
        }
        else
        {

            /** Skip add route for IPv6, becuase this cint is usually called with basic_example_ipv6() which includes this,
             *  calling it twice would cause segmentation fault
            rv = add_route_ipv6(unit, route_ipv6_dip, cint_srv6_tunnel_info.route_ipv6_mask, vrf, fec);

            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, in function add_route_ipv6() VRF:%d\n",proc_name, vrf);
            }
            */

        }
    }
    else if((srh_next_protocol == 143) || (srh_next_protocol == 59)) /* L2 o SRv6 case, configure MAC-T address */
    {
        bcm_l2_addr_t l2_addr;

        bcm_l2_addr_t_init(&l2_addr, cint_srv6_tunnel_info.l2_termination_mact_fwd_address, vsi);
        l2_addr.flags = BCM_L2_STATIC;      /* static entry */
        l2_addr.port = out_port;
        rv = bcm_l2_addr_add(unit, &l2_addr);

        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l2_addr_add\n");
            return rv;
        }

    }
    else
    {
        printf("%s(): Error, invalid next protocol value (%d), \n",proc_name, srh_next_protocol);
        return -1;
    }

    /*
     * 9. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */



    /** Set Out-Port default properties for the recycled packets*/
    rv = out_port_set(unit, rch_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set recycle_port \n");
        return rv;
    }

    if (!ibch1_supported)
    {
        rv = bcm_port_control_set(unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, bcm_port_control_set for rch_port %d\n",proc_name, rch_port);
            return rv;
        }
    }

    /** Set port class in order for PMF to crop RCH ad IRPP */
    rv = bcm_port_class_set(unit,rch_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_class_set \n");
        return rv;
    }

    /** define the structure which holds the mapping between nof_sids to RCH port and encap_id */
    bcm_srv6_extension_terminator_mapping_t extension_terminator_info;
    int sid_idx;

    extension_terminator_info.flags = 0;
    /** Convert Port number to GPORT */
    BCM_GPORT_LOCAL_SET(extension_terminator_info.port, rch_port);

    /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
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

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}


int
dnx_basic_example_ipvx_for_srv6(
    int unit,
    int srh_next_protocol,
    int rch_port,
    int out_port)
{
    int rv;
    char *proc_name;

    int fec;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec);
    fec += 30;
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    proc_name = "dnx_basic_example_ipv4_for_srv6";
    printf("%s(): Enter. rch_port %d out_port %d fec 0x%x\r\n",proc_name, rch_port, out_port, fec);
    rv = dnx_basic_example_ipvx_for_srv6_inner(unit, srh_next_protocol,  rch_port, out_port, fec);
    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

int dnx_basic_example_ipvx_cross_connect_for_srv6(
    int unit,
    int rch_port,
    int out_port,
    int next_protocol)
{
    bcm_vswitch_cross_connect_t cc_gports;
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv;
    int fec;
    bcm_gport_t gport_fec;

    /*
     * Configure the keys for the resulting LIF on termination (SIP, DIP, VRF)
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, cint_srv6_tunnel_info.tunnel_ipv6_dip_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
    tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_CROSS_CONNECT;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }

    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = 1;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error intf_ingress_rif_set. rv = %d \n", rv);
        return rv;
    }
    dnx_basic_example_ipvx_for_srv6(unit, next_protocol, rch_port, out_port);

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec);
    fec += 30;
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    BCM_GPORT_FORWARD_PORT_SET(gport_fec, fec);

    bcm_vswitch_cross_connect_t_init(&cc_gports);
    cc_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    cc_gports.port1 = tunnel_term_set.tunnel_id;
    cc_gports.port2 = gport_fec;
    rv = bcm_vswitch_cross_connect_add(unit, &cc_gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        return rv;
    }
    return 0;
}

int dnx_basic_example_ipv4_cross_connect_for_srv6(
    int unit,
    int rch_port,
    int out_port)
{
    int rv;

    rv = dnx_basic_example_ipvx_cross_connect_for_srv6(unit, rch_port, out_port, 4);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }
    return 0;
}

int dnx_basic_example_ipv6_cross_connect_for_srv6(
    int unit,
    int rch_port,
    int out_port)
{
    int rv;

    rv = dnx_basic_example_ipvx_cross_connect_for_srv6(unit, rch_port, out_port, 41);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }
    return 0;
}

int
dnx_basic_example_eth_is_fwd_cross_connect_for_srv6(
    int unit,
    int out_port)
{
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv;

    /*
     * Configure the keys for the resulting LIF on termination (SIP, DIP, VRF)
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeEthIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, cint_srv6_tunnel_info.tunnel_ipv6_dip_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_CROSS_CONNECT;
    tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }

    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;
    printf("termination tunnel id is: %d\n", tunnel_term_set.tunnel_id);

    /*
     * CrossConnect tunnel to Port
     */
    bcm_vswitch_cross_connect_t gports;
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = tunnel_term_set.tunnel_id;
    gports.port2 = out_port;
    gports.encap2 = BCM_FORWARD_ENCAP_ID_INVALID;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        return rv;
    }

    return rv;
}


/** \brief For PSP extended termination, the configurations that needs to be applied are:
 *
 * - Map the packet to RCY port with RCH outlif.
 * - The mapping is done in PMF2, before FEC resolution, so that the packet was forwarded correctly.
 * - Reference application function "appl_ref_srv6_endpoint_usp_extended_cut" defines "SRv6_EGRS_ext_usp_pmf1"
 * - As the SRv6 Endpoint (IPv6 Tunnel Termination) is a LIF with a Global LIF, need to do In_LIF[0]=In_LIF[1] (to get correct ETH-RIF at second pass)
 * - The function inputs: IPMF2/3 context "field_ctx_ipmf2" & "field_ctx_ipmf3" need to be set to PMF2 and PMF3 contexts
 *           to be attached to, in case of following possible flows:
 *           -Single Pass USP: #(8, 8)   / ("SRv6_EGRS_usp_pmf1", "SRv6_EGRS_usp_pmf3")
 *           -Extended USP:    #(37, 37) / ("SRv6_EGRS_ext_usp_pmf1", "SRv6_EGRS_ext_usp_pmf3")
 *           -Extended PSP:    #(38, 38) / ("SRv6_ext_psp_pmf1", "SRv6_EP_classic_ext_psp_pmf3")
 */
int srv6_endpoint_psp_extended_configuration(int unit, int rch_port, int field_ctx_ipmf2, int field_ctx_ipmf3, int use_rch_qos)
{
    /* For PSP extended termination, the configurations that needs to be applied are:
     * 1. Map the packet to RCY port with RCH outlif.
     *    - The mapping is done in PMF2, before FEC resolution, as the packet was forwarded correctly.
     *    - As the IPv6 Endpoint is a LIF with a Global LIF, need to do In_LIF[0]=In_LIF[1] (to get correct ETH-RIF at second pass)
     */
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    int rv;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);
    uint32 egress_filters_enable_per_recycle_port_supported = *dnxc_data_get(unit, "port_pp", "filters", "egress_filters_enable_per_recycle_port_supported", NULL);
    /** Set incoming port header type of RCY port to BCM_SWITCH_PORT_HEADER_TYPE_RCH_SRV6_USP_PSP */
    if (!ibch1_supported)
    {
        key.type = bcmSwitchPortHeaderType;
        key.index = 1;
        value.value = BCM_SWITCH_PORT_HEADER_TYPE_RCH_SRV6_USP_PSP;
        rv = bcm_switch_control_indexed_port_set(unit, rch_port, key, value);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_indexed_port_set (port %d)\n", rch_port);
            return rv;
        }
    }

    /** Create CTRL-LIF LIF */
     bcm_vlan_port_t vlan_port;
     bcm_vlan_port_t_init(&vlan_port);
     vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
     vlan_port.vsi = 0;
     vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_RECYCLE;
     if (use_rch_qos)
     {
         vlan_port.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
         vlan_port.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
         vlan_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
     }
     else
     {
         vlan_port.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
         vlan_port.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
         vlan_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
     }

     rv = bcm_vlan_port_create(unit, &vlan_port);
     if (rv != BCM_E_NONE)
     {
         printf("Error, bcm_vlan_port_create\n");
         return rv;
     }
     srv6_control_lif_id = vlan_port.vlan_port_id;

     /** Create recycle encap */
     bcm_l2_egress_t recycle_entry;
     bcm_l2_egress_t_init(&recycle_entry);
     recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
     if (ibch1_supported)
     {
         recycle_entry.recycle_app = bcmL2EgressRecycleAppExtendedTerm;
     }
     else
     {
         recycle_entry.recycle_app = bcmL2EgressRecycleAppSrv6UspPsp;
     }
     recycle_entry.vlan_port_id = vlan_port.vlan_port_id;
     rv = bcm_l2_egress_create(unit, &recycle_entry);
     if (rv != BCM_E_NONE)
     {
         printf("Error, bcm_l2_egress_create \n");
         return rv;
     }

    /** when ibch1 support the recycleApp per port is default, and need to disable egress filters explicitly for ExtTerm and Drop&Cont, base on the RCH out-lif */
    if (egress_filters_enable_per_recycle_port_supported == 0 && recycle_entry.recycle_app == bcmL2EgressRecycleAppExtendedTerm)
    {
        int cs_profile_id = 5;
        rv = cint_field_epmf_cs_outlif_profile_disable_egress_filters_main(unit, recycle_entry.encap_id, cs_profile_id);
        if (rv != BCM_E_NONE)
        {
             printf("Error, cint_field_epmf_cs_outlif_profile_disable_egress_filters_main\n");
             return rv;
        }
    }
    /** PMF */
    /**destination qualifier */
    bcm_field_qualify_t qual_info_dest;
    bcm_field_qualifier_info_create_t qual_info_dest_info;
    bcm_field_qualify_t qual_info_outlif;
    bcm_field_qualifier_info_create_t qual_info_outlif_info;

    bcm_field_qualifier_info_create_t_init(&qual_info_dest_info);
    qual_info_dest_info.size = 32;
    qual_info_dest_info.name[0] = 'T';
    qual_info_dest_info.name[1] = '0';
    qual_info_dest_info.name[2] = field_ctx_ipmf2;
    qual_info_dest_info.name[3] = '\0';

    rv = bcm_field_qualifier_create(unit, 0, &qual_info_dest_info, &qual_info_dest);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create qual_info_dest_info\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info_outlif_info);
    qual_info_outlif_info.size = 22;
    qual_info_outlif_info.name[0] = 'T';
    qual_info_outlif_info.name[1] = '1';
    qual_info_outlif_info.name[2] = field_ctx_ipmf2;
    qual_info_outlif_info.name[3] = '\0';

    rv = bcm_field_qualifier_create(unit, 0, &qual_info_outlif_info, &qual_info_outlif);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create qual_info_outlif_info\n", rv);
        return rv;
    }

    bcm_field_group_info_t fg_info;
    int fg_id;
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.name[0] = 'T';
    fg_info.name[1] = '2';
    fg_info.name[2] = field_ctx_ipmf2;
    fg_info.name[3] = '\0';

    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = qual_info_dest;
    fg_info.qual_types[1] = qual_info_outlif;

    /*
     * Set actions - Width of the actions write to,
     *               dictate how many bits to take from the Qualifiers
     */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionForward;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionOutVport0;
    fg_info.action_with_valid_bit[1] = FALSE;
    rv = bcm_field_group_add(unit, 0, &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add Egress PSP ext cut ipmf3_fg\n", rv);
        return rv;
    }

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    int ii;
    uint32 hw_value[4] = {0};
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    uint32 destination[4] = {0};
    *destination = rch_port | 0xc0000;
    rv = bcm_field_action_value_map(unit, bcmFieldStageIngressPMF2, bcmFieldActionForward, destination, hw_value);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_dnx_field_action_value_map\n", rv);
        return rv;
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[0].input_arg = *hw_value;

    /** Set the value of fwd_layer_idx */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[1].input_arg = recycle_entry.encap_id & 0x3FFFFF;

     /** context id is defined in appl_ref_init */
    rv = bcm_field_group_context_attach(unit, 0, fg_id, field_ctx_ipmf2, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach ipmf2\n", rv);
        return rv;
    }

    return rv;
}

int
srv6_psp_usp_update_fwd_destination(int unit, int destination)
{
    bcm_field_context_info_t context_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get_size;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    int fg_id, rv = 0;
    void *dest_char;
    int id,id2;

    rv = bcm_field_action_info_get(unit,bcmFieldActionForward,bcmFieldStageIngressPMF1,&action_info_get_size);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_info_get cint_const_action\n", rv);
        return rv;
    }

    /*
     * Create a user define action with size 0, value is fixed
     */
    bcm_field_action_t  cint_const_action_id, cint_const_action_id_2;
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionForward;
    action_info.prefix_size = action_info_get_size.size;
    action_info.prefix_value = destination;
    action_info.size = 0;
    action_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "srv6_usp_psp_destination", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_const_action_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create cint_const_action\n", rv);
        return rv;
    }
    id = cint_const_action_id;
    printf("Action (%s) created ID: %d \n", "srv6_usp_psp_destination",id);


    /*
     * Create and attach TCAM group in iPMF1
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Constant FG : number of qualifiers in key must be 0 (empty key) */
    fg_info.nof_quals = 0;

    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = cint_const_action_id;
    fg_info.action_with_valid_bit[0] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "srv6_usp_psp_destination_fg", sizeof(fg_info.name));
    bcm_field_group_t   cint_const_fg_id = 0;
    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_const_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    id = cint_const_fg_id;
    printf("FG (%s) created ID: %d \n", "srv6_usp_psp_destination_fg",id);

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    printf("Attaching field group\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_const_fg_id, 9, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    return rv;
}

int srv6_icmp_trap_overwrite (int unit)
{
    bcm_field_context_info_t context_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get_size;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    int fg_id, rv = 0;
    void *dest_char;
    int id,id2, trap_code;

    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapDefault, &trap_code);
    rv = bcm_field_action_info_get(unit,bcmFieldActionTrap,bcmFieldStageIngressPMF1,&action_info_get_size);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_info_get cint_const_action\n", rv);
        return rv;
    }

    /*
     * Create a user define action with size 0, value is fixed
     */
    bcm_field_action_t  cint_const_action_id;
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionTrap;
    action_info.prefix_size = action_info_get_size.size;
    action_info.prefix_value =  (15 <<9 |trap_code);
    action_info.size = 0;
    action_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "srv6_usp_psp_icmp_destination", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_const_action_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create cint_const_action\n", rv);
        return rv;
    }
    id = cint_const_action_id;
    printf("Action (%s) created ID: %d \n", "srv6_usp_psp_icmp_destination",id);


    /*
     * Create and attach TCAM group in iPMF1
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Constant FG : number of qualifiers in key must be 0 (empty key) */
    fg_info.nof_quals = 0;

    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = cint_const_action_id;
    fg_info.action_with_valid_bit[0] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "srv6_usp_psp_icmp_destination_fg", sizeof(fg_info.name));
    bcm_field_group_t   cint_const_fg_id = 0;
    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_const_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    id = cint_const_fg_id;
    printf("FG (%s) created ID: %d \n", "srv6_usp_psp_icmp_destination_fg",id);

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    printf("Attaching field group\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_const_fg_id, 8, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_attach(unit, 0, cint_const_fg_id, 37, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    return rv;
}

int srv6_psp_copy_inlif1_to_inlif0 (int unit, int field_ctx_ipmf3)
{
    int rv = BCM_E_NONE, ii;
    /** PMF 3 */
    bcm_field_group_info_t fg_info3;
    int fg_id3;
    bcm_field_group_info_t_init(&fg_info3);
    fg_info3.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info3.stage = bcmFieldStageIngressPMF3;
    fg_info3.name[0] = 'T';
    fg_info3.name[1] = '3';
    fg_info3.name[2] = field_ctx_ipmf3;
    fg_info3.name[3] = '\0';
    fg_info3.nof_quals = 1;
    fg_info3.qual_types[0] = bcmFieldQualifyInVPort1;
    fg_info3.nof_actions = 1;
    fg_info3.action_types[0] = bcmFieldActionInVport0;
    fg_info3.action_with_valid_bit[0] = FALSE;
    rv = bcm_field_group_add(unit, 0, &fg_info3, &fg_id3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add Egress PSP ext cut ipmf3_fg\n", rv);
        return rv;
    }
    bcm_field_group_attach_info_t attach_info3;
    bcm_field_group_attach_info_t_init(&attach_info3);

    attach_info3.key_info.nof_quals = fg_info3.nof_quals;
    attach_info3.payload_info.nof_actions = fg_info3.nof_actions;

    for (ii = 0; ii < fg_info3.nof_quals; ii++)
    {
        attach_info3.key_info.qual_types[ii] = fg_info3.qual_types[ii];
    }
    for (ii = 0; ii < fg_info3.nof_actions; ii++)
    {
        attach_info3.payload_info.action_types[ii] = fg_info3.action_types[ii];
    }
    attach_info3.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    /** context id is defined in appl_ref_init */
    rv = bcm_field_group_context_attach(unit, 0, fg_id3, field_ctx_ipmf3, &attach_info3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach ipmf3\n", rv);
        return rv;
    }

    return rv;
}

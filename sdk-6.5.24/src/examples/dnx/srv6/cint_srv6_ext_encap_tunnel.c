/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

/*
 * An example for 8 SIDs encapsulation
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/field/cint_field_utils.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../src/./examples/dnx/srv6/cint_srv6_ext_encap_tunnel.c
 * cint
 * srv6_ingress_tunnel_ext_encap_config(0,201,203,40,5,3,0);
 * exit;
 *
 *  IPv4 FWD packet into extended SRv6 tunnel
 * tx 1 psrc=201 data=0x000c00020000000007000100080045000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be:
 *  Source port: 0, Destination port: 0
 *  Data: 0x00000000cd1d00123456789a8100006486dd6000000000bd2b80123456789abceeffffeecba987654321112233445566778811223344556644440410000707000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff1111222211112222333344445555666677778888112233445566778811223344556600001122334455667788112233445566111111223344556677881122334455662222112233445566778811223344556633331122334455667788112233445566444445000035000000007f00fb45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  IPv6 FWD packet into extended SRv6 tunnel
 * tx 1 psrc=201 data=0x000c0002000000000700010086dd60000000002106800000000000000000111122223333444411112222111122221234f000ff00ff13000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be:
 *  Source port: 0, Destination port: 0
 *  Data: 0x00000000cd1d00123456789a8100006486dd6003b53100d12b80123456789abceeffffeecba987654321112233445566778811223344556644442910000707000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff11112222111122223333444455556666777788881122334455667788112233445566000011223344556677881122334455661111112233445566778811223344556622221122334455667788112233445566333311223344556677881122334455664444600000000021067f0000000000000000111122223333444411112222111122221234f000ff00ff13000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Bridge packet into extended SRv6 tunnel
 * tx 1 psrc=201 data=0x0001020304050000000a0b0c8100000f080045000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be:
 *  Source port: 0, Destination port: 0
 *  Data: 0x00000000cd1d00123456789a8100006486dd60026c0d00cf2b80123456789abceeffffeecba987654321112233445566778811223344556644443b10000707000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff111122221111222233334444555566667777888811223344556677881122334455660000112233445566778811223344556611111122334455667788112233445566222211223344556677881122334455663333112233445566778811223344556644440001020304050000000a0b0c8100000f080045000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
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
    INGRESS_SRH_FIRST_PASS_FEC_ID,
    INGRESS_SRH_SECOND_PASS_FEC_ID,
    INGRESS_SRH_THIRD_PASS_FEC_ID,
    INGRESS_RCH_1_2_PASS_FEC_ID,
    INGRESS_RCH_2_3_PASS_FEC_ID,
    INGRESS_IP_TUNNEL_FEC_ID,
    INGRESS_IP_TUNNEL_L2VPN_FEC_ID,
    END_POINT_1_FEC_ID,
    END_POINT_2_FEC_ID,
    EGRESS_FEC_ID
};

int NOF_FEC_ID = EGRESS_FEC_ID+1;

/** \brief List of SRv6 used Global LIFs */
enum srv6_global_lifs_e
{
    INGRESS_SRH_GLOBAL_LIF,
    INGRESS_IP_TUNNEL_GLOBAL_LIF,
    INGRESS_IP_TUNNEL_L2VPN_GLOBAL_LIF,
    INGRESS_SRH_2ND_PASS_GLOBAL_LIF,
    INGRESS_SRH_3RD_PASS_GLOBAL_LIF,
    INGRESS_SRH_MAIN_GLOBAL_LIF
};

int NOF_GLOBAL_LIFS=INGRESS_SRH_MAIN_GLOBAL_LIF+1;

/** \brief List of SRv6 used Terminated SIDs */
enum srv6_terminated_sids_e
{
    SID12,
    SID11,
    SID10,
    SID9,
    SID8,
    SID7,
    SID6,
    SID5,
    SID4,
    SID3,
    SID2,
    SID1,
    SID0
};

int NOF_SIDS = SID0+1;

struct cint_srv6_tunnel_info_s
{
    int vlan_port_id[NOF_INSTANCES];                                              /* vlan_port_id */
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
    bcm_ip6_t route_ipv6_dip;                                                     /* IPv6 Route DIP */
    bcm_mac_t l2_fwd_mac;                                                         /* mac for bridging into tunnel */
    bcm_mac_t l2_fwd_mac_l2vpn;                                                   /* mac for bridging into tunnel */
    int l2_fwd_vsi;                                                               /* vsi for L2 fwd */
    bcm_ip6_t tunnel_ip6_dip[NOF_SIDS];                                           /* IPv6 Terminated DIPs */
    bcm_ip6_t tunnel_ip6_vpn_dip;                                                 /* IPv6 Terminated DIPs */
    bcm_ip6_t tunnel_ip6_sip;                                                     /* IPv6 Tunnel SIP */
    int recycle_port;                                                             /* ESR USP RCH Port */
    int srh_next_protocol;                                                        /* Next protocol above SRv6, per IANA RFC */
    int l2_termination_vsi;                                                       /* When ETHoSRv6, in Extended Termiantion case, tunnel terminated LIF FODO (VSI) */
    bcm_mac_t l2_termination_mact_fwd_address;                                    /* When ETHoSRv6, MACT host entry for Bridging FWD lookup */
};


cint_srv6_tunnel_info_s cint_srv6_tunnel_info =
{
        /*
         * vlan_port_id
         */
         {-1, -1, -1, -1, -1},
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
         {0x2384, 0x5385, 0x8386, 0xb387},
        /*
         * tunnel_fec_id (update on run time)
         */
         {0, 0, 0, 0, 0, 0},
         /*
          * tunnel_global_lifs - SRH Base, 1st SID respectively
          */
         {0x9230, 0x4231, 0x4241, 0x7232, 0xa233, 0xd234},
         /*
          * tunnel_vid
          */
         100,
        /*
         * route_ipv4_dip
         */
        0x7fffff02 /* 127.255.255.02 */,
        /*
         * route_ipv6_dip
         */
        {0x11, 0x11, 0x22, 0x22, 0x11, 0x11, 0x22, 0x22, 0x12, 0x34, 0xf0, 0x00, 0xff, 0x00, 0xff, 0x13 },
        /*
          * mac for bridging into tunnel
          */
         {0x00, 0x01, 0x02, 0x03, 0x04, 0x05},
         /*
           * mac for bridging into l2vpn tunnel
           */
          {0x00, 0x01, 0x02, 0x03, 0x04, 0x06},
        /*
          * vsi for L2 fwd
          */
         2,
        /*
         * tunnel_ip6_dip
         */
        {{ 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x0c, 0x0c },
         { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x0b, 0x0b },
         { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x0a, 0x0a },
         { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x09, 0x09 },
         { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x08, 0x08 },
         { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x07, 0x07 },
         { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x06, 0x06 },
         { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x05, 0x05 },
         { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x04, 0x04 },
         { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x03, 0x03 },
         { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x02, 0x02 },
         { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x01, 0x01 },
         { 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x00, 0x00 }},
         /*
          * tunnel_ip6_vpn_dip
          */
         { 0x76, 0x76, 0x87, 0x87, 0x54, 0x54, 0x45, 0x45, 0x12, 0x12, 0x32, 0x32, 0x16, 0x16, 0x61, 0x61 },
         /*
          * tunnel_ip6_sip
          */
        { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xee, 0xff, 0xff, 0xee, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21 },
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

int
srv6_endpoint_tunnel(int unit, uint8 next_protocol_is_l2, uint8 is_termination)
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
    tunnel_term_set.type = bcmTunnelTypeSR6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    if(is_termination)
    {
        tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
    }
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
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

    return BCM_E_NONE;
}

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
    tunnel_term_set.type = bcmTunnelTypeSR6;
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
        printf("Error bcm_dnx_vswitch_cross_connect_add. rv = %d \n", rv);
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
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range 0\n");
        return rv;
    }
    cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID] = first_fec_in_hier + 0x1;
    cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID] = first_fec_in_hier + 0x2;
    cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]      = first_fec_in_hier + 0x3;
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_FEC_ID]    = first_fec_in_hier + 0x6;
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_L2VPN_FEC_ID]    = first_fec_in_hier + 0x7;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range 1\n");
        return rv;
    }
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FIRST_PASS_FEC_ID] = first_fec_in_hier + 0x700;
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID] = first_fec_in_hier + 0x701;
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_THIRD_PASS_FEC_ID] = first_fec_in_hier + 0x702;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 2, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range 2\n");
        return rv;
    }
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_1_2_PASS_FEC_ID] = first_fec_in_hier + 0x700;
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_2_3_PASS_FEC_ID] = first_fec_in_hier + 0x701;

    return rv;
}

/*
 *    Configuring Extended ISR (Ingress SRv6 extednded encapsulation Tunnel)
 *    This will create encapsulation of SRH,SIDs and IPv4 onto the packet
 *
 *    Specifically:
 *    - creating EEDB entries for SRH Base, SIDs[0..3], IPv6 Tunnel (SRv6 Type)
 *    - creation of x1 local-out-LIFs to return following LL entries from EEDB:
 *       1. SRH Base -> SID0 -> SID1 -> SID2 -> IPv6 Tunnel (LL) -> ARP
 *    - Creating the first pass SIDs encap, including RCH entry
 *      RCH entry contain the out port and second pass outlif
 */
int
srv6_ingress_tunnel_ext_encap_config(
        int unit,
        int in_port,
        int out_port,
        int rch_port,
        int nof_sids_first_pass,
        int nof_sids_second_pass,
        int nof_sids_third_pass,
        int is_t_insert)
{
    return srv6_ingress_tunnel_ext_encap_config_with_qos(unit,in_port,out_port,rch_port,nof_sids_first_pass,nof_sids_second_pass,nof_sids_third_pass,is_t_insert, 0);
}
int
srv6_ingress_tunnel_ext_encap_config_with_qos(
        int unit,
        int in_port,
        int out_port,
        int rch_port,
        int nof_sids_first_pass,
        int nof_sids_second_pass,
        int nof_sids_third_pass,
        int is_t_insert,
        int is_uniform)
{
    int rv;
    /* IVE parameters */
    int ive_edit_profile = 5;
    int ive_outer_vid = 10;
    int ive_inner_vid = 9;
    int ive_tag_format = 8; /* default 0x8100 */
    /* IVE parameters */
    int eve_edit_profile = 6;
    int eve_outer_vid = 100;
    int eve_inner_vid = 90;
    int eve_tag_format = 16; /* 0x9100/0x8100, after IVE, it's dtag*/
    bcm_gport_t egress_native_ac;
    int first_fec_in_hier;
    char *proc_name = "srv6_ingress_tunnel_ext_encap_config";
    bcm_gport_t gport;
    int reduced_mode = 0;
    uint32 estimate_encap_size_required =
        *dnxc_data_get(unit, "lif", "out_lif", "global_lif_allocation_estimated_encap_size_enable", NULL);

    rv = bcm_switch_control_get(unit, bcmSwitchSRV6ReducedModeEnable, &reduced_mode);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_get bcmSwitchSRV6ReducedModeEnable\n");
        return rv;
    }
    if((nof_sids_second_pass > 4) || (nof_sids_third_pass > 4) || (nof_sids_first_pass > 4))
    {
        printf("Error, nof_sids in the RCYed pass is limited to 4, first=%d, second=%d, third = %d\n", nof_sids_first_pass, nof_sids_second_pass, nof_sids_third_pass);
        return -1;
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

    rv = bcm_port_control_set(unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedEncap);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
        return rv;
    }
    /** Set port class in order for PMF Keep RCH to Egress, and set parsing start type to RCY */
    rv = bcm_port_class_set(unit,rch_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,5);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_class_set \n");
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

    uint32 flags = BCM_L3_WITH_ID;
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    int arp_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];

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

    int global_lif_encoded;
    int encoded_fec_ip_tunnel;
    int encoded_fec_srh;
    int encoded_fec_rch;

    /*
     * Create IP-Tunnel FEC
     */
    if(!is_t_insert)
    {
        BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FIRST_PASS_FEC_ID]);

        rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_FEC_ID], 0 , global_lif_encoded, gport, 0, 0,
                                               0, 0,&encoded_fec_ip_tunnel);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create egress object FEC_1 only\n",proc_name);
            return rv;
        }

        BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_L2VPN_GLOBAL_LIF]);
        rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_L2VPN_FEC_ID], 0 , global_lif_encoded, gport, 0, 0,
                                               0, 0,&encoded_fec_ip_tunnel);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create egress object FEC_1 only\n",proc_name);
            return rv;
        }

    }
    /*
     * Create SRH first pass FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_1_2_PASS_FEC_ID]);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FIRST_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_2ND_HIERARCHY, 0,
                                           0, 0,&encoded_fec_srh);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_2 only\n",proc_name);
        return rv;
    }

    bcm_l2_egress_t recycle_entry_1_to_2;
    bcm_l2_egress_t_init(&recycle_entry_1_to_2);
    recycle_entry_1_to_2.flags = BCM_L2_EGRESS_RECYCLE_HEADER | BCM_L2_EGRESS_DEST_PORT;
    if(!is_t_insert)
    {
        recycle_entry_1_to_2.flags |= BCM_L2_EGRESS_EXTENDED_COPY_DEST_ENCAP;
    }
    recycle_entry_1_to_2.dest_encap_id = 0;
    BCM_GPORT_FORWARD_PORT_SET(recycle_entry_1_to_2.dest_port, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID]);
    recycle_entry_1_to_2.recycle_app = bcmL2EgressRecycleAppExtendedEncap;
    rv = bcm_l2_egress_create(unit, &recycle_entry_1_to_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create \n");
        return rv;
    }
    /*
     * Create RCH_1_2 pass FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, recycle_entry_1_to_2.encap_id);
    BCM_GPORT_LOCAL_SET(gport, rch_port);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_1_2_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_3RD_HIERARCHY, 0,
                                           0, 0,&encoded_fec_rch);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_3 only\n",proc_name);
        return rv;
    }

   /*
    * Create SRH second pass FEC
    */
   BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_2ND_PASS_GLOBAL_LIF]);
   if(nof_sids_third_pass == 0)
   {
       BCM_GPORT_LOCAL_SET(gport, out_port);
   }
   else
   {
       BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_2_3_PASS_FEC_ID]);
   }
   rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_2ND_HIERARCHY, 0,
                                          0, 0,&encoded_fec_srh);
   if (rv != BCM_E_NONE)
   {
       printf("%s(): Error, create egress object FEC_2 only\n",proc_name);
       return rv;
   }

   bcm_l2_egress_t recycle_entry_2_to_3;
   bcm_l2_egress_t_init(&recycle_entry_2_to_3);
   recycle_entry_2_to_3.flags = BCM_L2_EGRESS_RECYCLE_HEADER | BCM_L2_EGRESS_DEST_PORT;
   recycle_entry_2_to_3.dest_encap_id = 0;
   if(!is_t_insert)
   {
       recycle_entry_2_to_3.flags |= BCM_L2_EGRESS_EXTENDED_COPY_DEST_ENCAP;
   }
   BCM_GPORT_FORWARD_PORT_SET(recycle_entry_2_to_3.dest_port, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_THIRD_PASS_FEC_ID]);
   recycle_entry_2_to_3.recycle_app = bcmL2EgressRecycleAppExtendedEncap;
   rv = bcm_l2_egress_create(unit, &recycle_entry_2_to_3);
   if (rv != BCM_E_NONE)
   {
       printf("Error, bcm_l2_egress_create \n");
       return rv;
   }
   /*
    * Create RCH_2_3 pass FEC
    */
   BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, recycle_entry_2_to_3.encap_id);
   BCM_GPORT_LOCAL_SET(gport, rch_port);
   rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_2_3_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_3RD_HIERARCHY, 0,
                                          0, 0,&encoded_fec_rch);
   if (rv != BCM_E_NONE)
   {
       printf("%s(): Error, create egress object FEC_3 only\n",proc_name);
       return rv;
   }

   /*
    * Create SRH third pass FEC
    */
   BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_3RD_PASS_GLOBAL_LIF]);
   BCM_GPORT_LOCAL_SET(gport, out_port);
   rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_THIRD_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_2ND_HIERARCHY, 0,
                                          0, 0,&encoded_fec_srh);
   if (rv != BCM_E_NONE)
   {
       printf("%s(): Error, create egress object FEC_2 only\n",proc_name);
       return rv;
   }

    /*
     * 8. Add host/mac entry
     */
    /** SRV6 Tunnel Ingress Forwarding to FEC Hierarchy to bring out IPV6, SRH, SIDs*/
    if(!is_t_insert)
    {
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_FEC_ID]);
    }
    else
    {
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FIRST_PASS_FEC_ID]);
    }

    rv = add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, 0, gport);
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

    bcm_l2_addr_t l2addr;

    /** Add MACT entry for bridging into SRv6 tunnel */
    cint_srv6_tunnel_info.l2_fwd_vsi = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    bcm_l2_addr_t_init(&l2addr, cint_srv6_tunnel_info.l2_fwd_mac, cint_srv6_tunnel_info.l2_fwd_vsi);
    l2addr.port = gport;
    l2addr.encap_id = 0;
    l2addr.flags = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    /** SRV6 Tunnel Ingress Forwarding to FEC Hierarchy to bring out IPV6, SRH, SIDs*/
    if(!is_t_insert)
    {
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_L2VPN_FEC_ID]);
    }
    /** Add MACT entry for bridging into SRv6 L2VPN tunnel */
    cint_srv6_tunnel_info.l2_fwd_vsi = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    bcm_l2_addr_t_init(&l2addr, cint_srv6_tunnel_info.l2_fwd_mac_l2vpn, cint_srv6_tunnel_info.l2_fwd_vsi);
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

    if(!is_t_insert)
    {
        bcm_tunnel_initiator_t_init(&tunnel_init_set);
        bcm_l3_intf_t_init(&intf);
        tunnel_init_set.type = bcmTunnelTypeSR6;
        /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
        sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
        sal_memcpy(tunnel_init_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_vpn_dip, 16);
        tunnel_init_set.flags = BCM_TUNNEL_WITH_ID;
        if (is_uniform)
        {
            tunnel_init_set.ttl = 0;
            tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
        }
        else
        {
            tunnel_init_set.ttl = 128;
            tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        }
        BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
        if(estimate_encap_size_required)
        {
            /* Estimation is 0, all estimations are taken into account in the SRH outlif */
            tunnel_init_set.estimated_encap_size = 0;
        }
        rv = bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set);

        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_tunnel_initiator_create for IPv6 Tunnel \n");
           return rv;
        }

        /** Add the L2VPN tunnel */
        bcm_tunnel_initiator_t_init(&tunnel_init_set);
        bcm_l3_intf_t_init(&intf);
        tunnel_init_set.type = bcmTunnelTypeEthSR6;
        /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
        sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
        sal_memcpy(tunnel_init_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_vpn_dip, 16);
        tunnel_init_set.flags = BCM_TUNNEL_WITH_ID;
        if (is_uniform)
        {
            tunnel_init_set.ttl = 0;
            tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
        }
        else
        {
            tunnel_init_set.ttl = 128;
            tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        }
        BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_L2VPN_GLOBAL_LIF]);
        if(estimate_encap_size_required)
        {
            /* Estimation is 0, all estimations are taken into account in the SRH outlif */
            tunnel_init_set.estimated_encap_size = 0;
        }
        rv = bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set);

        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_tunnel_initiator_create for IPv6 Tunnel \n");
           return rv;
        }

        bcm_vlan_port_t vlan_port;
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vlan_port.vsi = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
        vlan_port.port = tunnel_init_set.tunnel_id;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_vlan_port_create\n", rv);
            return rv;
        }

        /* Save native ac for EVE */
        egress_native_ac = vlan_port.vlan_port_id;

    }
    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t  sid_info;
    bcm_srv6_sid_initiator_info_t_init(&sid_info);
    int next_sid_tunnel_id;
    int arp_as_ift;
    int sid_idx;
    int sid_location;
    int sid_start_idx_1st_pass = SID0 - nof_sids_third_pass - nof_sids_second_pass;
    int sid_start_idx_2nd_pass = SID0 - nof_sids_third_pass;
    int sid_start_idx_3rd_pass = SID0;
    int nof_total_sids = nof_sids_first_pass + nof_sids_second_pass + nof_sids_third_pass;

    BCM_L3_ITF_SET(arp_as_ift, BCM_L3_ITF_TYPE_LIF, arp_id);
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, arp_as_ift);

    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */
    if(nof_sids_third_pass > 0)
    {
        printf("Creating 3rd pass SRH tunnel start sid idx = %d, nof sid = %d\n", sid_start_idx_3rd_pass, nof_sids_third_pass);
        int sid_count = 0;
        for (sid_count = 0; sid_count < nof_sids_third_pass; sid_count++)
        {
            sid_idx = sid_start_idx_3rd_pass - sid_count;
            bcm_srv6_sid_initiator_info_t_init(&sid_info);

            /** set SID address to be last SID0 */
            sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[sid_idx], 16);
            /** set to EEDB entry of SID0 */
            sid_info.encap_access = bcmEncapAccessTunnel4 - sid_count;

            /** next_encap_id set to l3 interface - convert it from GPORT to l3_int */
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

            /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
            rv = bcm_srv6_sid_initiator_create(unit, &sid_info);
            if (rv != BCM_E_NONE)
            {
               printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d 3rd pass \n", sid_count);
               return rv;
            }
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
        bcm_srv6_srh_base_initiator_info_t srh_base_info;
        bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

        /** we will pass the global-out-LIF id*/
        srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

        /** convert SRH global-LIF id to GPORT */
        BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_3RD_PASS_GLOBAL_LIF]);

        /** set number of SIDs*/
        srh_base_info.nof_sids = nof_total_sids;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, next_sid_tunnel_id);

        /** set TTL and QOS modes */
        srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
        srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
        srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
        if(estimate_encap_size_required)
        {
            if(!is_t_insert)
            {
                /*
                 * This is the last pass, estimation is:
                 * nof_sids*16 + 8(SRH) + 40(IP header) - 16 - 30 (minus the BTR sizes (Inner SID + RCH))
                 */
                int estimation = (nof_sids_third_pass*16 + 8) - 16 - 30 + 40;
                srh_base_info.estimated_encap_size = estimation > 0 ? estimation : 0;
            }
            else
            {
                /*
                 * This is the last pass, estimation is:
                 * nof_sids*16 + 8(SRH) + 40(IP header) - 16 - 30 (minus the BTR sizes (Inner SID + RCH))
                 */
                int estimation = (nof_sids_third_pass*16) - 30;
                srh_base_info.estimated_encap_size = estimation > 0 ? estimation : 0;
            }
        }

        /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
        rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);

        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_srh_base_initiator_create 3rd pass \n");
           return rv;
        }
        next_sid_tunnel_id = 0;
    }

    if(nof_sids_second_pass > 0)
    {
        printf("Creating 2nd pass SRH tunnel start sid idx = %d, nof sid = %d\n", sid_start_idx_2nd_pass, nof_sids_second_pass);
        int sid_count = 0;
        for (sid_count = 0; sid_count < nof_sids_second_pass; sid_count++)
        {
            sid_idx = sid_start_idx_2nd_pass - sid_count;
            bcm_srv6_sid_initiator_info_t_init(&sid_info);

            /** set SID address to be last SID0 */
            sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[sid_idx], 16);

            /** set to EEDB entry of SID0 */
            sid_info.encap_access = bcmEncapAccessTunnel4 - sid_count;

            if(next_sid_tunnel_id != 0)
            {
                /** next_encap_id set to l3 interface - convert it from GPORT to l3_int */
                BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);
            }
            /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
            rv = bcm_srv6_sid_initiator_create(unit, &sid_info);
            if (rv != BCM_E_NONE)
            {
               printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d 2nd pass \n", sid_count);
               return rv;
            }
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
        bcm_srv6_srh_base_initiator_info_t srh_base_info;
        bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

        /** we will pass the global-out-LIF id*/
        srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

        /** convert SRH global-LIF id to GPORT */
        BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_2ND_PASS_GLOBAL_LIF]);

        /** set number of SIDs*/
        srh_base_info.nof_sids = nof_total_sids;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, next_sid_tunnel_id);

        /** set TTL and QOS modes */
        srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
        srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
        srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
        if(estimate_encap_size_required)
        {
            if(nof_sids_third_pass > 0)
            {
                if(!is_t_insert)
                {
                    /*
                     * This is the middle pass, estimation is:
                     * nof_sids*16 + 30 (RCH) + 16 (Inner SID) - 30 (minus the BTR sizes (Inner SID + RCH))
                     */
                    int estimation = nof_sids_second_pass*16 + 30 - 16 - 30 + 16;
                    srh_base_info.estimated_encap_size = estimation > 0 ? estimation : 0;
                }
                else
                {
                    /*
                     * This is the middle pass, estimation is:
                     * nof_sids*16 + 30 (RCH) + 16 (Inner SID) - 30 (minus the BTR sizes (Inner SID + RCH))
                     */
                    int estimation = nof_sids_second_pass*16;
                    srh_base_info.estimated_encap_size = estimation > 0 ? estimation : 0;
                }
            }
            else
            {
                if(!is_t_insert)
                {
                    /*
                     * This is the last pass, estimation is:
                     * nof_sids*16 + 8(SRH) + 40(IP header) - 16 - 30 (minus the BTR sizes (Inner SID + RCH))
                     */
                    int estimation = (nof_sids_second_pass*16 + 8) + 40 - 16 - 30;
                    srh_base_info.estimated_encap_size = estimation > 0 ? estimation : 0;
                }
                else
                {
                    /*
                     * This is the last pass, estimation is:
                     * nof_sids*16 + 8(SRH) + 40(IP header) - 16 - 30 (minus the BTR sizes (Inner SID + RCH))
                     */
                    int estimation = (nof_sids_second_pass*16) - 30;
                    srh_base_info.estimated_encap_size = estimation > 0 ? estimation : 0;
                }
            }
        }

        /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
        rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);

        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_srh_base_initiator_create 2nd pass\n");
           return rv;
        }
        next_sid_tunnel_id = 0;
    }

    if(nof_sids_first_pass > 0)
    {
        printf("Creating 1st pass SRH tunnel start sid idx = %d, nof sid = %d\n", sid_start_idx_1st_pass, nof_sids_first_pass);
        int sid_count = 0;
        for (sid_count = 0; sid_count < nof_sids_first_pass; sid_count++)
        {
            sid_idx = sid_start_idx_1st_pass - sid_count;
            bcm_srv6_sid_initiator_info_t_init(&sid_info);

            /** set SID address to be last SID0 */
            sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[sid_idx], 16);
            sid_info.encap_access = bcmEncapAccessTunnel4 - sid_count;

            if(next_sid_tunnel_id != 0)
            {
                /** next_encap_id set to l3 interface - convert it from GPORT to l3_int */
                BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);
            }

            /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
            rv = bcm_srv6_sid_initiator_create(unit, &sid_info);
            if (rv != BCM_E_NONE)
            {
               printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d 1st pass \n", sid_count);
               return rv;
            }
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
        bcm_srv6_srh_base_initiator_info_t srh_base_info;
        bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

        /** we will pass the global-out-LIF id*/
        srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

        /** convert SRH global-LIF id to GPORT */
        BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);

        /** set number of SIDs*/
        srh_base_info.nof_sids = nof_total_sids;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, next_sid_tunnel_id);

        /** set TTL and QOS modes */
        srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
        srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
        srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
        if(estimate_encap_size_required)
        {
            if(!is_t_insert)
            {
                /*
                 * This is the first pass, estimation is:
                 * nof_sids*16 + 30 (RCH) + 16 (Inner SID) - 16 for reduce mode
                 */
                srh_base_info.estimated_encap_size = nof_sids_first_pass*16 + 30 + 16 -16*reduced_mode;
            }
            else
            {
                /*
                 * This is the first pass, estimation is:
                 * nof_sids*16 + 30 (RCH) + 8 - 16 for reduce mode
                 */
                srh_base_info.estimated_encap_size = nof_sids_first_pass*16 + 30 + 8 -16*reduced_mode;
            }
        }

        /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
        rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);

        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_srh_base_initiator_create 1st pass\n");
           return rv;
        }
    }

    if(!is_t_insert)
    {
        /*
         * Set IVE on AC, outgoing packet is double tagged
         */
        rv = vlan_translate_ive_eve_translation_set(unit, in_port_intf[0],
                                                          0x9100,
                                                          0x8100,
                                                          bcmVlanActionReplace,
                                                          bcmVlanActionAdd,
                                                          ive_outer_vid,
                                                          ive_inner_vid,
                                                          ive_edit_profile,
                                                          ive_tag_format, 1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in vlan_translate_ive_eve_translation_set\n");
            return rv;
        }

        /*
         * Native EVE, to replace tags
         */
        rv = vlan_translate_ive_eve_translation_set(unit, egress_native_ac,
                                                          0x8100,
                                                          0x9100,
                                                          bcmVlanActionReplace,
                                                          bcmVlanActionReplace,
                                                          eve_outer_vid,
                                                          eve_inner_vid,
                                                          eve_edit_profile,
                                                          eve_tag_format, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in vlan_translate_ive_eve_translation_set\n");
            return rv;
        }
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/*
 *    Configuring SRv6 extended encap
 *    Specifically:
 *      1st pass:
 *        L2 MC -> replication index
 *        PPMC > (IP-tunnel outlif > DIP outlif) + RCH EE-uncollapse FEC
 *      2nd pass:
 *        RCH EE-uncollapse FEC(1st hierarchy) -> (SRH -> SID list) + ECMP(2nd hierarchy)
 *        ECMP -> FEC for ARP
 *     Native EVE is also covered
 */
int
srv6_ingress_l2vpn_mc_ext_encap_1st_and_2nd_pass(
        int unit,
        int in_port,
        int ce_port,
        int pe_port,
        int rch_port,
        int nof_sids)
{
    int rv;
    char *proc_name = "srv6_ingress_l2vpn_mc_ext_encap_1st_and_2nd_pass";
    int first_fec_in_hier;
    bcm_gport_t gport;
    bcm_gport_t ce_ac_encap_id;
    int reduced_mode = 0;
    int i;
    int ecmp_id = 2150;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_if_t intf_array[2];
    bcm_gport_t egress_native_ac;

    uint32 estimate_encap_size_required =
        *dnxc_data_get(unit, "lif", "out_lif", "global_lif_allocation_estimated_encap_size_enable", NULL);

    rv = bcm_switch_control_get(unit, bcmSwitchSRV6ReducedModeEnable, &reduced_mode);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_get bcmSwitchSRV6ReducedModeEnable\n");
        return rv;
    }

    if(nof_sids > 4)
    {
        printf("Error, nof_sids in the RCYed pass is limited to 4, %d\n",nof_sids);
        return -1;
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
    /** SRv6 ingress local CE port AC*/
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_vlan_port_create\n");
        return rv;
    }

    ce_ac_encap_id = vlan_port.encap_id;

    /** SRv6 ingress remote PE port*/
    rv = out_port_set(unit, pe_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port of SRV6 Ingress Tunnel %d\n",proc_name,pe_port);
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

    /** SRV6 Ingress Tunnel PE port SA  */
    /* Create 2 for ECMP */
    for (i = 0; i < 2; i++) {
        rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
            return rv;
        }
        cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS]++;
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
     * 5. Configure recycle port
     */
    rv = bcm_port_control_set(unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedEncapUncollapse);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set rch_port %d\n",proc_name,rch_port);
        return rv;
    }

    /** Set port class in order for PMF to crop RCH ad IRPP */
    rv = bcm_port_class_set(unit,rch_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_class_set \n",proc_name);
        return rv;
    }

    /*
     * 6. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */

    uint32 flags = BCM_L3_WITH_ID;
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    int arp_id;
    int encoded_fec;
    int arp_fec_id;
    cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS]=100;

    /* Update FEC ids */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range 0\n");
        return rv;
    }

    BCM_GPORT_LOCAL_SET(gport, pe_port);
    /* Create 2 FECs and each FEC pointing to differnet ARP */
    for (i = 0; i < 2; i++) {
        arp_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS] + i;

        /* ARP with different next_hop_mac and vid */
        rv = l3__egress_only_encap__create_inner(unit, 0, &arp_id, cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], 0, flags2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create egress object ARP only, in SRV6 Ingress Tunnel\n",proc_name);
            return rv;
        }

        arp_fec_id = first_fec_in_hier + 0x100 + i;
        /* FEC */
        rv = l3__egress_only_fec__create_inner(unit, arp_fec_id, 0 , arp_id, gport, BCM_L3_2ND_HIERARCHY, 0, 0, 0,&encoded_fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object only, fec\n");
            return rv;
        }
        /* Save FEC ids */
        intf_array[i] = encoded_fec;
        cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS][5]++;
        cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS]++;
    }

    /* Create an ECMP group */
    rv = l3__ecmp_create(unit, ecmp_id, 2, intf_array, BCM_L3_WITH_ID | BCM_L3_2ND_HIERARCHY, BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT, &ecmp, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3__ecmp_create create ECMP group\n");
        return rv;
    }

    /*
     * 7. SRV6 FEC Entries
     */

    int global_lif_encoded;
    int encoded_fec_ip_tunnel;
    int encoded_fec_srh;
    int encoded_fec_rch;

    /* Update FEC ids */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range 0\n");
        return rv;
    }

    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID] = first_fec_in_hier + 0x1;

    /*
     * Create SRH second pass FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_2ND_PASS_GLOBAL_LIF]);
    BCM_GPORT_FORWARD_PORT_SET(gport, ecmp_id);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID], 0 , global_lif_encoded, gport, 0, 0,
                                           0, 0,&encoded_fec_srh);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_2 only\n",proc_name);
        return rv;
    }

    /* Create 1st_to_2nd pass RCH EE(extended encap), destination is 2nd pass IP tunnel FEC */
    bcm_l2_egress_t recycle_entry_1_to_2;
    bcm_l2_egress_t_init(&recycle_entry_1_to_2);
    recycle_entry_1_to_2.flags = BCM_L2_EGRESS_RECYCLE_HEADER | BCM_L2_EGRESS_DEST_PORT;
    recycle_entry_1_to_2.dest_encap_id = 0;
    BCM_GPORT_FORWARD_PORT_SET(recycle_entry_1_to_2.dest_port, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID]);
    recycle_entry_1_to_2.recycle_app = bcmL2EgressRecycleAppExtendedEncap;
    rv = bcm_l2_egress_create(unit, &recycle_entry_1_to_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create \n");
        return rv;
    }

    /*
     * 8. Configuring ISR SRv6 Tunnel
     *
     *    - creating EEDB entries for SRH Base, SIDs[0..2], IPv6 Tunnel (SRv6 Type)
     *    - creation of x2 local-out-LIFs to return following entries from EEDB:
     *       1. SRH Base -> SID(4) ..-> SID(0)
     *       2. IPv6 Tunnel -> ARP+AC
     */

    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    /** Add the L2VPN tunnel */
    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeEthSR6;
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    /** Set VPN SID to be SID0(last SID) */
    sal_memcpy(tunnel_init_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0], 16);
    tunnel_init_set.flags = BCM_TUNNEL_WITH_ID;
    tunnel_init_set.ttl = 128;
    tunnel_init_set.encap_access = bcmEncapAccessTunnel4;
    tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_L2VPN_GLOBAL_LIF]);
    if(estimate_encap_size_required)
    {
        /*
         * This is the 1st pass, estimation is:40(IP header)
         */
        tunnel_init_set.estimated_encap_size = 40;
    }
    rv = bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set);
    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_tunnel_initiator_create for IPv6 Tunnel \n");
       return rv;
    }

    /* Egress native AC for EVE */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.vsi = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    vlan_port.port = tunnel_init_set.tunnel_id;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }

    /* Save native ac for EVE */
    egress_native_ac = vlan_port.vlan_port_id;

    /* Native EVE, replace and add VLAN */
    g_vlan_translate_with_eve.tag_format = 8;
    g_vlan_translate_with_eve.outer_action = bcmVlanActionReplace;
    g_vlan_translate_with_eve.inner_action = bcmVlanActionAdd;
    rv = vlan_translate_eve_translation_set(unit,egress_native_ac,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_eve_translation_set\n");
        return rv;
    }

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;
    bcm_srv6_sid_initiator_info_t_init(&sid_info);
    int next_sid_tunnel_id;
    int sid_idx;
    int sid_location;
    int sid_start_idx = SID1; /* SID0 is brought by IPv6 DIP */

    printf("Creating 2nd pass SRH tunnel, nof sid = %d\n", nof_sids);
    int sid_count = 0;
    for (sid_count = 0; sid_count < nof_sids - 1; sid_count++)
    {
        sid_idx = sid_start_idx - sid_count;
        bcm_srv6_sid_initiator_info_t_init(&sid_info);

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[sid_idx], 16);
        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_count - 1;/* last SID is IPv6 DIP */

        if(next_sid_tunnel_id != 0)
        {
            /** next_encap_id set to l3 interface - convert it from GPORT to l3_int */
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);
        }

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        rv = bcm_srv6_sid_initiator_create(unit, &sid_info);
        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d 2nd pass \n", sid_count);
           return rv;
        }
        next_sid_tunnel_id = sid_info.tunnel_id;
    }

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

    /** we will pass the global-out-LIF id*/
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_2ND_PASS_GLOBAL_LIF]);

    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids - 1;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, next_sid_tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
    if(estimate_encap_size_required)
    {
        /*
         * This is the last pass, estimation is:
         * nof_sids*16 + 8(SRH) + ARP(at least 14B)
         */
        srh_base_info.estimated_encap_size = (nof_sids - 1)*16 + 8 + 14 - 16*reduced_mode;;
    }

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_dnx_srv6_srh_base_initiator_create 2nd pass \n");
        return rv;
    }

    /*
     * 9. Add multicast entry
     */

    int mc_id = cint_srv6_tunnel_info.l2_fwd_vsi;

    /* Create multicast group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID, mc_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    bcm_multicast_replication_t rep_array;

    /* 1st copy to local CE */
    rep_array.port = ce_port;
    rep_array.encap1 = ce_ac_encap_id;

    rv = bcm_multicast_add(unit, mc_id, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add in Port= %d\n", ce_port);
        return rv;
    }

    /* 2nd copy to RCH EE, and finally to remote PE */
    bcm_if_t rep_idx = 0x300100;

    rep_array.port = rch_port;
    rep_array.encap1 = rep_idx;

    rv = bcm_multicast_add(unit, mc_id, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add in Port= %d\n", out_port_1);
        return rv;
    }

    /* Add PPMC entry */
    int cuds[2];
    BCM_L3_ITF_SET(cuds[0], BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_L2VPN_GLOBAL_LIF]);
    BCM_L3_ITF_SET(cuds[1], BCM_L3_ITF_TYPE_LIF, recycle_entry_1_to_2.encap_id);
    rv = bcm_multicast_encap_extension_create(unit, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID, &rep_idx, 2, cuds);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_multicast_encap_extension_create\n");
        return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/*
 *    Configuring SRv6 extended encap
 *    Specifically:
 *      1st pass:
 *        L2 MC -> replication index
 *        PPMC > Native AC + RCH EE-uncollapse FEC
 *      2nd pass:
 *        RCH EE-uncollapse FEC -> IP tunnel FEC(1st hierarchy) + SRH FEC(2nd hierarchy) + ECMP(3rd hierarchy)
 *          IP tunnel FEC -> (tunnel outlif > DIP outlif)
 *          SRH FEC -> (SRH -> SID list)
 *          ECMP -> FECs for ARPs
 *     Native EVE is also covered
 */
int
srv6_ingress_l2vpn_mc_ext_encap_2nd_pass_only(
        int unit,
        int in_port,
        int ce_port,
        int pe_port,
        int rch_port,
        int nof_sids_second_pass)
{
    int rv;
    char *proc_name = "srv6_ingress_l2vpn_mc_ext_encap_2nd_pass_only";
    int first_fec_in_hier;
    bcm_gport_t gport;
    bcm_gport_t ce_ac_encap_id;
    int reduced_mode = 0;
    bcm_if_t intf_array[2];
    int i;
    int ecmp_id = 2150;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_gport_t egress_native_ac;

    uint32 estimate_encap_size_required =
        *dnxc_data_get(unit, "lif", "out_lif", "global_lif_allocation_estimated_encap_size_enable", NULL);

    rv = bcm_switch_control_get(unit, bcmSwitchSRV6ReducedModeEnable, &reduced_mode);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_get bcmSwitchSRV6ReducedModeEnable\n");
        return rv;
    }

    if(nof_sids_second_pass > 4)
    {
        printf("Error, nof_sids in the RCYed pass is limited to 4, %d\n",nof_sids_second_pass);
        return -1;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    /** SRV6 Ingress Tunnel Eth RIF based on Port + cvlan */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.match_vlan = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    vlan_port.vsi = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_port_create\n", proc_name);
        return rv;
    }
    cint_srv6_tunnel_info.vlan_port_id[INGRESS] = vlan_port.vlan_port_id;
    printf("%s(): port = %d, in_lif = 0x%08X\n", proc_name, vlan_port.port, vlan_port.vlan_port_id);

    rv = bcm_vlan_gport_add(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_gport_add\n",proc_name);
        return rv;
    }


    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    /** SRv6 ingress local CE port AC*/
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_vlan_port_create\n");
        return rv;
    }

    ce_ac_encap_id = vlan_port.encap_id;

    /** SRv6 ingress remote PE port*/
    rv = out_port_set(unit, pe_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port of SRV6 Ingress Tunnel %d\n",proc_name,pe_port);
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

    /** SRV6 Ingress Tunnel PE port SA  */
    /* Create 2 for ECMP */
    for (i = 0; i < 2; i++) {
        rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
            return rv;
        }
        cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS]++;
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
     * 5. Configure recycle port
     */
    rv = bcm_port_control_set(unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedEncapUncollapse);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set rch_port %d\n",proc_name,rch_port);
        return rv;
    }

    /** Set port class in order for PMF to crop RCH ad IRPP */
    rv = bcm_port_class_set(unit,rch_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_class_set \n",proc_name);
        return rv;
    }

    /*
     * 6. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */

    uint32 flags = BCM_L3_WITH_ID;
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    int arp_id;
    int encoded_fec;
    int arp_fec_id;
    cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS]=100;

    /* Update FEC ids */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 2, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range 0\n");
        return rv;
    }

    BCM_GPORT_LOCAL_SET(gport, pe_port);
    /* Create 2 FECs and each FEC pointing to differnet ARP */
    for (i = 0; i < 2; i++) {
        arp_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS] + i;

        /* ARP with different next_hop_mac and vid */
        rv = l3__egress_only_encap__create_inner(unit, 0, &arp_id, cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], 0, flags2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create egress object ARP only, in SRV6 Ingress Tunnel\n",proc_name);
            return rv;
        }

        arp_fec_id = first_fec_in_hier + 0x100 + i;
        /* FEC */
        rv = l3__egress_only_fec__create_inner(unit, arp_fec_id, 0 , arp_id, gport, BCM_L3_3RD_HIERARCHY, 0, 0, 0,&encoded_fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object only, fec\n");
            return rv;
        }
        /* Save FEC ids */
        intf_array[i] = encoded_fec;
        cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS][5]++;
        cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS]++;
    }

    /* Create an ECMP group */
    rv = l3__ecmp_create(unit, ecmp_id, 2, intf_array, BCM_L3_WITH_ID | BCM_L3_3RD_HIERARCHY, BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT, &ecmp,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3__ecmp_create create ECMP group\n");
        return rv;
    }


    /*
     * 7. SRV6 FEC Entries
     */

    int global_lif_encoded;
    int encoded_fec_ip_tunnel;
    int encoded_fec_srh;
    int encoded_fec_rch;

    /* Update FEC ids */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range 0\n");
        return rv;
    }

    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID] = first_fec_in_hier + 0x100;
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_1_2_PASS_FEC_ID] = first_fec_in_hier + 0x200;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range 0\n");
        return rv;
    }

    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_L2VPN_FEC_ID] = first_fec_in_hier + 0x1;

    /*
     * Create SRH second pass FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_2ND_PASS_GLOBAL_LIF]);
    BCM_GPORT_FORWARD_PORT_SET(gport, ecmp_id);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_2ND_HIERARCHY, 0,
                                           0, 0,&encoded_fec_srh);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_2 only\n",proc_name);
        return rv;
    }

    /*
     * Create IP-Tunnel FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_L2VPN_GLOBAL_LIF]);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID]);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_L2VPN_FEC_ID], 0 , global_lif_encoded, gport, 0, 0,
                                           0, 0,&encoded_fec_ip_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_1 only\n",proc_name);
        return rv;
    }

    /* Create 1st_to_2nd pass RCH EE(extended encap), destination is 2nd pass IP tunnel FEC */
    bcm_l2_egress_t recycle_entry_1_to_2;
    bcm_l2_egress_t_init(&recycle_entry_1_to_2);
    recycle_entry_1_to_2.flags = BCM_L2_EGRESS_RECYCLE_HEADER | BCM_L2_EGRESS_DEST_PORT;
    recycle_entry_1_to_2.dest_encap_id = 0;
    BCM_GPORT_FORWARD_PORT_SET(recycle_entry_1_to_2.dest_port, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_L2VPN_FEC_ID]);
    recycle_entry_1_to_2.recycle_app = bcmL2EgressRecycleAppExtendedEncap;
    rv = bcm_l2_egress_create(unit, &recycle_entry_1_to_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create \n");
        return rv;
    }

    /*
     * 8. Configuring ISR SRv6 Tunnel
     *
     *    - creating EEDB entries for SRH Base, SIDs[0..2], IPv6 Tunnel (SRv6 Type)
     *    - creation of x2 local-out-LIFs to return following entries from EEDB:
     *       1. IPv6 Tunnel
     *       2. SRH Base -> SID(4) ..-> SID(0)
     *       3. ARP+AC
     */

    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    /** Add the L2VPN tunnel */
    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeEthSR6;
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    /** Set VPN SID to be SID0(last SID) */
    sal_memcpy(tunnel_init_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0], 16);
    tunnel_init_set.flags = BCM_TUNNEL_WITH_ID;
    tunnel_init_set.ttl = 128;
    tunnel_init_set.encap_access = bcmEncapAccessTunnel4;
    tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_L2VPN_GLOBAL_LIF]);
    if(estimate_encap_size_required)
    {
        /* Estimation is 0, all estimations are taken into account in the SRH outlif */
        tunnel_init_set.estimated_encap_size = 0;
    }
    rv = bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set);
    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_tunnel_initiator_create for IPv6 Tunnel \n");
       return rv;
    }

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;
    bcm_srv6_sid_initiator_info_t_init(&sid_info);
    int next_sid_tunnel_id;
    int sid_idx;
    int sid_location;
    int sid_start_idx = SID1; /* SID0 is brought by IPv6 DIP */

    printf("Creating 2nd pass SRH tunnel, nof sid = %d\n", nof_sids_second_pass);
    int sid_count = 0;
    for (sid_count = 0; sid_count < nof_sids_second_pass - 1; sid_count++)
    {
        sid_idx = sid_start_idx - sid_count;
        bcm_srv6_sid_initiator_info_t_init(&sid_info);

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[sid_idx], 16);
        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_count - 1;/* last SID is IPv6 DIP */

        if(next_sid_tunnel_id != 0)
        {
            /** next_encap_id set to l3 interface - convert it from GPORT to l3_int */
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);
        }

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        rv = bcm_srv6_sid_initiator_create(unit, &sid_info);
        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d 2nd pass \n", sid_count);
           return rv;
        }
        next_sid_tunnel_id = sid_info.tunnel_id;
    }

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

    /** we will pass the global-out-LIF id*/
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_2ND_PASS_GLOBAL_LIF]);

    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids_second_pass - 1;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, next_sid_tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
    if(estimate_encap_size_required)
    {
        /*
         * This is the last pass, estimation is:
         * nof_sids*16 + 8(SRH) + 40(IP header) - 16 - 30 (minus the BTR sizes (Inner SID + RCH))
         */
        srh_base_info.estimated_encap_size = (nof_sids_second_pass*16 + 8) + 40 - 16*reduced_mode;
    }

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_dnx_srv6_srh_base_initiator_create 2nd pass \n");
        return rv;
    }

    /* Egress native AC for EVE */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_NATIVE;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_vlan_port_create\n");
        return rv;
    }

    /* Save native ac for EVE */
    egress_native_ac = vlan_port.vlan_port_id;

    /* Native EVE, replace and add VLAN */
    g_vlan_translate_with_eve.tag_format = 8;
    g_vlan_translate_with_eve.outer_action = bcmVlanActionReplace;
    g_vlan_translate_with_eve.inner_action = bcmVlanActionAdd;
    rv = vlan_translate_eve_translation_set(unit,egress_native_ac,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_eve_translation_set\n");
        return rv;
    }

    /*
     * 9. Add multicast entry
     */

    int mc_id = cint_srv6_tunnel_info.l2_fwd_vsi;

    /* Create multicast group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID, mc_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    bcm_multicast_replication_t rep_array;

    /* 1st copy to local CE */
    rep_array.port = ce_port;
    rep_array.encap1 = ce_ac_encap_id;

    rv = bcm_multicast_add(unit, mc_id, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add in Port= %d\n", ce_port);
        return rv;
    }

    /* 2nd copy to RCH EE, and finally to remote PE */
    bcm_if_t rep_idx = 0x300100;

    rep_array.port = rch_port;
    rep_array.encap1 = rep_idx;

    rv = bcm_multicast_add(unit, mc_id, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add in Port= %d\n", out_port_1);
        return rv;
    }

    /* Add PPMC entry */
    int cuds[2];
    BCM_L3_ITF_SET(cuds[0], BCM_L3_ITF_TYPE_LIF, vlan_port.encap_id);
    BCM_L3_ITF_SET(cuds[1], BCM_L3_ITF_TYPE_LIF, recycle_entry_1_to_2.encap_id);
    rv = bcm_multicast_encap_extension_create(unit, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID, &rep_idx, 2, cuds);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_multicast_encap_extension_create\n");
        return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}


/*
 *    Configuring SRv6 extended encap of multiple SRHs,meaning instead of using a single long SRH list, it uses several SRHs
 *    Specifically:
 *      1st pass: IP host -> IP tunnel FEC (1st hierarchy) + 1st pass SRH FEC (2nd hierarchy) + 2nd pass RCH EE-uncollapse FEC (3rd hierarchy)
 *                           1st pass SRH FEC -> (SRH -> SID list) (up to 4 SIDs)
 *      2nd pass: 2nd pass RCH EE-uncollapse FEC -> 2nd pass SRH FEC (2nd hierarchy) + 3rd pass RCH EE-uncollapse FEC (3rd hierarchy)
 *                2nd pass SRH FEC -> (SRH -> SID list) (up to 4 SIDs)
 *      3rd pass: 3rd pass RCH EE-uncollapse FEC -> 3rd pass SRH FEC
 *                3rd pass SRH FEC -> (SRH -> SID list -> ARP)
 *    Note: for T.encap if nof_sids > 7, it use 3 SRHs, otherwise use 2 SRHs.
 *          for T.insert if nof_sids > 6, it use 3 SRHs, otherwise use 2 SRHs.
 */
int
srv6_ingress_tunnel_ext_encap_of_multiple_SRHs(
        int unit,
        int in_port,
        int out_port,
        int rch_port,
        int nof_sids,
        int is_t_insert,
        int last_sid_is_dip)
{
    int rv;
    char *proc_name = "srv6_ingress_tunnel_ext_encap_of_multiple_SRHs";
    int first_fec_in_hier;
    bcm_gport_t gport;
    int reduced_mode = 0;
    /* On JR2, 1st pass can only encap 2 SIDs(SL=2) for non_reduced mode, otherwise will breach BTC size */
    int is_inner_srh_max_2_sid = (*dnxc_data_get(unit, "srv6", "encapsulation", "max_nof_encap_sids_main_pass", NULL) >= 5) ? 0:1;

    rv = bcm_switch_control_get(unit, bcmSwitchSRV6ReducedModeEnable, &reduced_mode);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_get bcmSwitchSRV6ReducedModeEnable\n");
        return rv;
    }

    if (is_t_insert)
    {
        if((nof_sids < 4) || nof_sids > 9)
        {
            printf("Error, nof_sids should be between 4 to 9 in T.insert mode %d\n", nof_sids);
            return -1;
        }
    } else
    {
        if((nof_sids < 5) || nof_sids > 10)
        {
            printf("Error, nof_sids should be between 5 to 10 in T.encap %d\n", nof_sids);
            return -1;
        }
    }

    int nof_sids_third_pass;
    int nof_sids_second_pass;
    int nof_sids_first_pass;

    nof_sids_second_pass = 3;
    if ((is_t_insert && nof_sids > 6) || (!is_t_insert && nof_sids > 7))
    {
        /* 3 pass required if sid numer > 6 for T.insert or sid numer > 7 for T.encap */
        nof_sids_third_pass = 3;
    }

    if (is_inner_srh_max_2_sid && !reduced_mode)
    {
        if ((is_t_insert && nof_sids > 5) || (!is_t_insert && nof_sids > 6))
        {
            nof_sids_third_pass = 3;
            nof_sids_second_pass = 2;
        }
    }

    nof_sids_first_pass = nof_sids - nof_sids_third_pass - nof_sids_second_pass;

    uint32 estimate_encap_size_required =
        *dnxc_data_get(unit, "lif", "out_lif", "global_lif_allocation_estimated_encap_size_enable", NULL);

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
     * 5. Configure recycle port
     */
    rv = bcm_port_control_set(unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedEncapUncollapse);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set rch_port %d\n",proc_name,rch_port);
        return rv;
    }

    /** Set port class in order for PMF to crop RCH at IRPP */
    rv = bcm_port_class_set(unit,rch_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_class_set \n",proc_name);
        return rv;
    }

    /*
     * 6. Configure ARP entry
     */
    uint32 flags = BCM_L3_WITH_ID;
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    int arp_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];

    /** SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1*/
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3__egress_only_encap__create_inner for overlay ARP\n");
        return rv;
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
     * 7. SRV6 FEC Entries
     */

    int global_lif_encoded;
    int encoded_fec_ip_tunnel;
    int encoded_fec_srh;
    int encoded_fec_rch;

    /*
     * Create SRH third pass FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_3RD_PASS_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_THIRD_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_2ND_HIERARCHY, 0,
                                           0, 0,&encoded_fec_srh);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create SRH 3rd pass FEC\n",proc_name);
        return rv;
    }

    /* Create 2nd_to_3rd pass RCH EE(extended encap), destination is 3rd pass SRH FEC */
    bcm_l2_egress_t recycle_entry_2_to_3;
    bcm_l2_egress_t_init(&recycle_entry_2_to_3);
    recycle_entry_2_to_3.flags = BCM_L2_EGRESS_RECYCLE_HEADER | BCM_L2_EGRESS_DEST_PORT;
    recycle_entry_2_to_3.dest_encap_id = 0;
    BCM_GPORT_FORWARD_PORT_SET(recycle_entry_2_to_3.dest_port, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_THIRD_PASS_FEC_ID]);
    recycle_entry_2_to_3.recycle_app = bcmL2EgressRecycleAppExtendedEncap;
    rv = bcm_l2_egress_create(unit, &recycle_entry_2_to_3);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create RCH EE from 2nd to 3rd pass \n");
        return rv;
    }

    /*
     * Create RCH_2_3 pass FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, recycle_entry_2_to_3.encap_id);
    BCM_GPORT_LOCAL_SET(gport, rch_port);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_2_3_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_3RD_HIERARCHY, 0,
                                           0, 0,&encoded_fec_rch);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create RCH 2nd to 3rd pass FEC\n",proc_name);
        return rv;
    }

    /*
     * Create SRH second pass FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_2ND_PASS_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, out_port);
   if(nof_sids_third_pass == 0)
   {
       BCM_GPORT_LOCAL_SET(gport, out_port);
   }
   else
   {
       BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_2_3_PASS_FEC_ID]);
   }
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_2ND_HIERARCHY, 0,
                                           0, 0,&encoded_fec_srh);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create SRH 2nd pass FEC\n",proc_name);
        return rv;
    }

    /* Create 1st_to_2nd pass RCH EE(extended encap), destination is 2nd pass SRH FEC */
    bcm_l2_egress_t recycle_entry_1_to_2;
    bcm_l2_egress_t_init(&recycle_entry_1_to_2);
    recycle_entry_1_to_2.flags = BCM_L2_EGRESS_RECYCLE_HEADER | BCM_L2_EGRESS_DEST_PORT;
    recycle_entry_1_to_2.dest_encap_id = 0;
    BCM_GPORT_FORWARD_PORT_SET(recycle_entry_1_to_2.dest_port, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID]);
    recycle_entry_1_to_2.recycle_app = bcmL2EgressRecycleAppExtendedEncap;
    rv = bcm_l2_egress_create(unit, &recycle_entry_1_to_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create RCH EE from 1st to 2nd pass \n");
        return rv;
    }

    /*
     * Create RCH_1_2 pass FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, recycle_entry_1_to_2.encap_id);
    BCM_GPORT_LOCAL_SET(gport, rch_port);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_1_2_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_3RD_HIERARCHY, 0,
                                           0, 0,&encoded_fec_rch);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create RCH 1st to 2nd pass FEC\n",proc_name);
        return rv;
    }

    /*
     * Create SRH first pass FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_1_2_PASS_FEC_ID]);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FIRST_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_2ND_HIERARCHY, 0,
                                           0, 0,&encoded_fec_srh);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create SRH 1st pass FEC\n",proc_name);
        return rv;
    }

    /*
     * Create IP tunnel FEC
     */
    if(!is_t_insert)
    {
        BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FIRST_PASS_FEC_ID]);
        rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_FEC_ID], 0 , global_lif_encoded, gport, 0, 0,
                                               0, 0,&encoded_fec_ip_tunnel);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create IP tunnel FEC\n",proc_name);
            return rv;
        }
    }

    /*
     * 8. Configuring ISR SRv6 Tunnel
     */

    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    /** Add SRv6 tunnel */
    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeSR6;
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    /** check if last SID is brought by IPv6 Tunnel */
    if (last_sid_is_dip)
    {
        sal_memcpy(tunnel_init_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0], 16);
        tunnel_init_set.encap_access = bcmEncapAccessTunnel4;
    }
    tunnel_init_set.flags = BCM_TUNNEL_WITH_ID;
    tunnel_init_set.ttl = 128;
    tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    if(estimate_encap_size_required)
    {
        /* Estimation is 0, all estimations are taken into account in the SRH outlif */
        tunnel_init_set.estimated_encap_size = 0;
    }
    rv = bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set);
    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_tunnel_initiator_create for IPv6 Tunnel \n");
       return rv;
    }


    /* 9. Build multiple SRHs */
    /*
     *  for example, totally we have 10 SIDs, T.encap, non-reduced mode, it will split to 3 SRHs
     *    1st pass: build SID0~SID3, SL=3
     *    2nd pass T.insert mode: configure SID4~6, SL=2, it will build SID3~6, SL=3
     *    3rd pass T.insert mode: configure SID7~9, SL=2, it will build SID6~9, SL=3
 */
    bcm_srv6_sid_initiator_info_t  sid_info;
    bcm_srv6_sid_initiator_info_t_init(&sid_info);
    int next_sid_tunnel_id;
    int arp_as_ift;
    int sid_idx;
    int sid_location;
    int sid_start_idx_1st_pass = SID0;
    int sid_start_idx_2nd_pass = SID0 - nof_sids_first_pass;
    int sid_start_idx_3rd_pass = SID0 - nof_sids_first_pass - nof_sids_second_pass;
    if (last_sid_is_dip)
    {
        sid_start_idx_1st_pass = SID1; /* SID0 (last one) is given by IPv6 Tunnel */
        nof_sids_first_pass = nof_sids_first_pass - 1;
    }

    BCM_L3_ITF_SET(arp_as_ift, BCM_L3_ITF_TYPE_LIF, arp_id);
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, arp_as_ift);

    int sid_count = 0;
    if(nof_sids_third_pass > 0)
    {
        printf("Creating 3rd pass SRH tunnel start sid idx = %d, nof sid = %d\n", sid_start_idx_3rd_pass, nof_sids_third_pass);
        for (sid_count = 0; sid_count < nof_sids_third_pass; sid_count++)
        {
            sid_idx = sid_start_idx_3rd_pass - sid_count;
            bcm_srv6_sid_initiator_info_t_init(&sid_info);

            /** set SID address to be last SID0 */
            sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[sid_idx], 16);
            /** set to EEDB entry of SID0 */
            sid_info.encap_access = bcmEncapAccessTunnel4 - sid_count;

            /** next_encap_id set to l3 interface - convert it from GPORT to l3_int */
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

            /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
            rv = bcm_srv6_sid_initiator_create(unit, &sid_info);
            if (rv != BCM_E_NONE)
            {
               printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d 3rd pass \n", sid_count);
               return rv;
            }
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
        bcm_srv6_srh_base_initiator_info_t srh_base_info;
        bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

        /** we will pass the global-out-LIF id*/
        srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

        /** convert SRH global-LIF id to GPORT */
        BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_3RD_PASS_GLOBAL_LIF]);

        /** set number of SIDs*/
        srh_base_info.nof_sids = nof_sids_third_pass;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, next_sid_tunnel_id);

        /** set TTL and QOS modes */
        srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
        srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
        srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
        if(estimate_encap_size_required)
        {
            /*
             * This is the last pass, it's always T.insert
             */
            srh_base_info.estimated_encap_size = nof_sids_third_pass*16 + 8 - 16*reduced_mode;
        }

        /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
        rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);

        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_srh_base_initiator_create 3rd pass \n");
           return rv;
        }
        next_sid_tunnel_id = 0;
    }

    printf("Creating 2nd pass SRH tunnel start sid idx = %d, nof sid = %d\n", sid_start_idx_2nd_pass, nof_sids_second_pass);
    for (sid_count = 0; sid_count < nof_sids_second_pass; sid_count++)
    {
        sid_idx = sid_start_idx_2nd_pass - sid_count;
        bcm_srv6_sid_initiator_info_t_init(&sid_info);

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_count;

        if(next_sid_tunnel_id != 0)
        {
            /** next_encap_id set to l3 interface - convert it from GPORT to l3_int */
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);
        }

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        rv = bcm_srv6_sid_initiator_create(unit, &sid_info);
        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d 2nd pass \n", sid_count);
           return rv;
        }
        next_sid_tunnel_id = sid_info.tunnel_id;
    }

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

    /** we will pass the global-out-LIF id*/
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_2ND_PASS_GLOBAL_LIF]);

    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids_second_pass;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, next_sid_tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
    if(estimate_encap_size_required)
    {
        {
            /*
             * This is the 2nd pass, it's always T.insert
             */
            srh_base_info.estimated_encap_size = nof_sids_second_pass*16 + 8 - 16*reduced_mode;
        }
    }

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_dnx_srv6_srh_base_initiator_create 2nd pass\n");
        return rv;
    }
    next_sid_tunnel_id = 0;

    printf("Creating 1st pass SRH tunnel start sid idx = %d, nof sid = %d\n", sid_start_idx_1st_pass, nof_sids_first_pass);
    for (sid_count = 0; sid_count < nof_sids_first_pass; sid_count++)
    {
        sid_idx = sid_start_idx_1st_pass - sid_count;
        bcm_srv6_sid_initiator_info_t_init(&sid_info);

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[sid_idx], 16);
        if (last_sid_is_dip)
        {
            sid_info.encap_access = bcmEncapAccessTunnel4 - sid_count - 1;
        } else
        {
            sid_info.encap_access = bcmEncapAccessTunnel4 - sid_count;
        }

        if(next_sid_tunnel_id != 0)
        {
            /** next_encap_id set to l3 interface - convert it from GPORT to l3_int */
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);
        }

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        rv = bcm_srv6_sid_initiator_create(unit, &sid_info);
        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d 1st pass \n", sid_count);
           return rv;
        }
        next_sid_tunnel_id = sid_info.tunnel_id;
    }

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

    /** we will pass the global-out-LIF id*/
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);

    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids_first_pass;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, next_sid_tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
    if(estimate_encap_size_required)
    {
        /* 1st pass will differ for T.insert and T.encap */
         if(!is_t_insert)
        {
            /*
             * This is the first pass, estimation is:
             * nof_sids*16 + 8(SRH) + 40(IP header)
             */
            srh_base_info.estimated_encap_size = nof_sids_first_pass*16 + 8 + 40;
        } else {
            srh_base_info.estimated_encap_size = nof_sids_first_pass*16 + 8;
        }
    }

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_dnx_srv6_srh_base_initiator_create 1st pass\n");
       return rv;
    }

    /*
     * 10. Add host entries
     */
    /** SRV6 Tunnel Ingress Forwarding to FEC Hierarchy to bring out IPV6, SRH, SIDs*/
    if(!is_t_insert)
    {
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_FEC_ID]);
    }
    else
    {
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FIRST_PASS_FEC_ID]);
    }

    /* IPv4 host */
    rv = add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, 0, gport);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4 for SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /* IPv6 host */
    rv = add_host_ipv6_encap_dest(unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, 0, gport);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6() for SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/*
 *    Configuring SRv6 endpoint and into T.insert using 2nd pass
 *    Specifically:
 *      1st pass: IP host -> 2nd pass RCH EE-uncollapse FEC
 *      2nd pass: 2nd pass RCH EE-uncollapse FEC -> (2nd pass RCH EE -> 2nd pass SRH FEC)
 *                2nd pass SRH FEC -> (SRH -> SID list -> ARP)
 */
int
srv6_ingress_tunnel_2nd_pass_insert(
        int unit,
        int in_port,
        int out_port,
        int rch_port,
        int nof_sids)
{
    int rv;
    char *proc_name = "srv6_ingress_tunnel_2nd_pass_insert";
    int first_fec_in_hier;
    bcm_gport_t gport;
    int reduced_mode = 0;

    rv = bcm_switch_control_get(unit, bcmSwitchSRV6ReducedModeEnable, &reduced_mode);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_get bcmSwitchSRV6ReducedModeEnable\n");
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
     * 5. Configure recycle port
     */
    rv = bcm_port_control_set(unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedEncapUncollapse);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set rch_port %d\n",proc_name,rch_port);
        return rv;
    }

    /** Set port class in order for PMF to crop RCH at IRPP */
    /*
    rv = bcm_port_class_set(unit,rch_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_class_set \n",proc_name);
        return rv;
    } */

    /*
     * 6. Configure ARP entry
     */
    uint32 flags = BCM_L3_WITH_ID;
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    int arp_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];

    /** SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1*/
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3__egress_only_encap__create_inner for overlay ARP\n");
        return rv;
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
     * 7. SRV6 FEC Entries
     */

    int global_lif_encoded;
    int encoded_fec_ip_tunnel;
    int encoded_fec_srh;
    int encoded_fec_rch;

    /*
     * Create SRH second pass FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_2ND_PASS_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_2ND_HIERARCHY, 0,
                                           0, 0,&encoded_fec_srh);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create SRH 2nd pass FEC\n",proc_name);
        return rv;
    }

    /* Create 1st_to_2nd pass RCH EE(extended encap), destination is 2nd pass SRH FEC */
    bcm_l2_egress_t recycle_entry_1_to_2;
    bcm_l2_egress_t_init(&recycle_entry_1_to_2);
    recycle_entry_1_to_2.flags = BCM_L2_EGRESS_RECYCLE_HEADER | BCM_L2_EGRESS_DEST_PORT;
    recycle_entry_1_to_2.dest_encap_id = 0;
    BCM_GPORT_FORWARD_PORT_SET(recycle_entry_1_to_2.dest_port, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID]);
    recycle_entry_1_to_2.recycle_app = bcmL2EgressRecycleAppExtendedEncap;
    rv = bcm_l2_egress_create(unit, &recycle_entry_1_to_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create RCH EE from 1st to 2nd pass \n");
        return rv;
    }

    /*
     * Create RCH_1_2 pass FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, recycle_entry_1_to_2.encap_id);
    BCM_GPORT_LOCAL_SET(gport, rch_port);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_1_2_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_3RD_HIERARCHY, 0,
                                           0, 0,&encoded_fec_rch);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create RCH 1st to 2nd pass FEC\n",proc_name);
        return rv;
    }

    /*
     * 8. Configuring ISR SRv6 Tunnel
     */

    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    /** Add SRv6 tunnel */
    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeSR6;
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    tunnel_init_set.flags = BCM_TUNNEL_WITH_ID;
    tunnel_init_set.ttl = 128;
    tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);

    rv = bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set);
    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_tunnel_initiator_create for IPv6 Tunnel \n");
       return rv;
    }

    /* 9. Build multiple SRHs */
    bcm_srv6_sid_initiator_info_t  sid_info;
    bcm_srv6_sid_initiator_info_t_init(&sid_info);
    int next_sid_tunnel_id;
    int arp_as_ift;
    int sid_idx;
    int sid_location;
    int sid_start_idx_2nd_pass = SID0;

    BCM_L3_ITF_SET(arp_as_ift, BCM_L3_ITF_TYPE_LIF, arp_id);
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, arp_as_ift);

    int sid_count = 0;

    printf("Creating 2nd pass SRH tunnel start sid idx = %d, nof sid = %d\n", sid_start_idx_2nd_pass, nof_sids);
    for (sid_count = 0; sid_count < nof_sids; sid_count++)
    {
        sid_idx = sid_start_idx_2nd_pass - sid_count;
        bcm_srv6_sid_initiator_info_t_init(&sid_info);

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_count;

        if(next_sid_tunnel_id != 0)
        {
            /** next_encap_id set to l3 interface - convert it from GPORT to l3_int */
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);
        }

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        rv = bcm_srv6_sid_initiator_create(unit, &sid_info);
        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d 2nd pass \n", sid_count);
           return rv;
        }
        next_sid_tunnel_id = sid_info.tunnel_id;
    }

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

    /** we will pass the global-out-LIF id*/
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_2ND_PASS_GLOBAL_LIF]);

    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, next_sid_tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_dnx_srv6_srh_base_initiator_create 2nd pass\n");
        return rv;
    }
    next_sid_tunnel_id = 0;

    /*
     * 10. Add host entries
     */
    /** SRV6 Tunnel Ingress Forwarding to FEC Hierarchy to bring out IPV6, SRH, SIDs*/
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_1_2_PASS_FEC_ID]);

    /* IPv6 host */
    rv = add_host_ipv6_encap_dest(unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, 0, gport);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6() for SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /*
     * 11. Configure endpoint
     */
    rv = srv6_endpoint_tunnel(unit, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in srv6_endpoint_tunnel\n",proc_name);
        return rv;
    }

    /*
     * 12. Configure PMF to sets FAI
     */
    rv = srv6_ingress_tunnel_2nd_pass_insert_set_field(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in srv6_ingress_tunnel_2nd_pass_insert_set_field\n",proc_name);
        return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}


bcm_field_group_t insert_2nd_pass_fg_id;

/*
 * This function sets FAI to 4(not to update TTL)
 */
int srv6_ingress_tunnel_2nd_pass_insert_set_field(int unit) {

    int rv = BCM_E_NONE;
    int i;
    void *dest_char;
    bcm_field_entry_t ent_id;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    int gport;
    bcm_field_context_t context_id = 0; /* Use default context */

    /** Create field group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /* Qualifiers:
     *    In port: rcy port
     *    Out lif: SRH global lif
     */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    fg_info.qual_types[1] = bcmFieldQualifyOutVPort0;

    /* Actions:
     *    FAI[2] = 1, do not decrement TTL
     */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionForwardingAdditionalInfo;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "2nd_pass_insert", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, 0, &fg_info, &insert_2nd_pass_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_field_group_add\n");
        return rv;
    }
    printf("insert_2nd_pass_fg_id:%d\n", insert_2nd_pass_fg_id);

    /* Attaching the FG */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    for (i = 0; i < fg_info.nof_quals; i++)
    {
        attach_info.key_info.qual_types[i] = fg_info.qual_types[i];
    }

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, insert_2nd_pass_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_field_group_context_attach\n");
        return rv;
    }

    /* Add field entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = 40; /* rcy port */
    ent_info.entry_qual[0].mask[0] = 0xff;

    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    /* 2nd pass out lif */
    BCM_GPORT_TUNNEL_ID_SET(gport, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_2ND_PASS_GLOBAL_LIF]);
    ent_info.entry_qual[1].value[0] = gport;
    ent_info.entry_qual[1].mask[0] = 0x3fffff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 4; /* Do not decrement the TTL indication */

    rv = bcm_field_entry_add(unit, 0, insert_2nd_pass_fg_id, &ent_info, &ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_field_entry_add\n");
        return rv;
    }

    return rv;
}

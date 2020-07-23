/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * An example for 8 SIDs encapsulation
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/field/cint_field_utils.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../src/./examples/dnx/srv6/cint_srv6_ext_encap_tunnel.c
 * cint
 * srv6_ingress_tunnel_ext_encap_config(0,201,203,40,5,3);
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
    INGRESS_SRH_FEC_ID,
    INGRESS_FIRST_SID_FEC_ID,
    INGRESS_SRH_EXT_PASS_FEC_ID,
    INGRESS_FIRST_SID_EXT_PASS_FEC_ID,
    END_POINT_1_FEC_ID,
    END_POINT_2_FEC_ID,
    EGRESS_FEC_ID
};

int NOF_FEC_ID = EGRESS_FEC_ID+1;

/** \brief List of SRv6 used Global LIFs */
enum srv6_global_lifs_e
{
    INGRESS_SRH_GLOBAL_LIF,
    INGRESS_FIRST_SID_GLOBAL_LIF,
    INGRESS_SRH_EXT_PASS_GLOBAL_LIF,
    INGRESS_FIRST_SID_EXT_PASS_GLOBAL_LIF,
    INGRESS_SRH_MAIN_GLOBAL_LIF
};

int NOF_GLOBAL_LIFS=INGRESS_SRH_MAIN_GLOBAL_LIF+1;

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

/** \brief List of SRv6 used Terminated SIDs */
enum srv6_terminated_sids2_e
{
    SID2_5,
    SID2_4,
    SID2_3,
    SID2_2,
    SID2_1,
    SID2_0
};
int NOF_SIDS2 = SID2_0+1;

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
    bcm_ip6_t route_ipv6_dip;                                                     /* IPv6 Route DIP */
    bcm_mac_t l2_fwd_mac;                                                         /* mac for bridging into tunnel */
    int l2_fwd_vsi;                                                               /* vsi for L2 fwd */
    bcm_ip6_t tunnel_ip6_dip[NOF_SIDS];                                           /* IPv6 Terminated DIPs */
    bcm_ip6_t tunnel_ip6_dip_ext[NOF_SIDS2];                                      /* IPv6 Terminated DIPs */
    bcm_ip6_t tunnel_ip6_sip;                                                     /* IPv6 Tunnel SIP */
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
         {0x1230, 0x1231, 0x1232, 0x1233, 0x1234},
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
          * terminated_ip6_dip_ext
          */
         {{ 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x55, 0x55 },
          { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x44, 0x44 },
          { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x33, 0x33 },
          { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x22, 0x22 },
          { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x11, 0x11 },
          { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x00, 0x00 }},
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

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID] = first_fec_in_hier + 0x700;
    cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID] = first_fec_in_hier + 0x1;
    cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID] = first_fec_in_hier + 0x2;
    cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID] = first_fec_in_hier + 0x3;
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_EXT_PASS_FEC_ID] = first_fec_in_hier + 0x4;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_FEC_ID] = first_fec_in_hier + 0x700;
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_EXT_PASS_FEC_ID] = first_fec_in_hier + 0x1;

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
        int nof_sids_second_pass)
{

    int rv;
    int first_fec_in_hier;
    char *proc_name = "srv6_ingress_tunnel_ext_encap_config";

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

    bcm_gport_t gport;
    int srv6_basic_lif_encode;
    int encoded_fec1;
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_EXT_PASS_GLOBAL_LIF]);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_EXT_PASS_FEC_ID]);

    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_EXT_PASS_FEC_ID], 0 , srv6_basic_lif_encode, gport, 0,
                                           0, 0,&encoded_fec1);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_1 only\n",proc_name);
        return rv;
    }

    int srv6_first_lif_encode;
    int encoded_fec2;
    BCM_L3_ITF_SET(srv6_first_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_FIRST_SID_EXT_PASS_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, rch_port);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_EXT_PASS_GLOBAL_LIF], 0 , srv6_first_lif_encode, gport, BCM_L3_2ND_HIERARCHY,
                                           0, 0,&encoded_fec2);

    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC_2 only, fec\n");
        return rv;
    }

    bcm_l2_egress_t recycle_entry;
    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER | BCM_L2_EGRESS_DEST_ENCAP_ID | BCM_L2_EGRESS_DEST_PORT;
    recycle_entry.dest_encap_id =  cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_MAIN_GLOBAL_LIF];
    recycle_entry.dest_port = out_port;
    recycle_entry.recycle_app = bcmL2EgressRecycleAppExtendedEncap;
    rv = bcm_l2_egress_create(unit, &recycle_entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create \n");
        return rv;
    }

    /*
     * 7. Add route entry - we work with specific DIP addresses, so we don't need to add best match route entries.
     */

    /*
     * 8. Add host/mac entry
     */

    /** SRV6 Tunnel Ingress Forwarding to FEC Hierarchy to bring out IPV6, SRH, SIDs*/
    if(nof_sids_first_pass !=0 )
    {
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_EXT_PASS_FEC_ID]);
    }
    else
    {
        BCM_GPORT_LOCAL_SET(gport, rch_port);
    }
    add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, recycle_entry.encap_id, gport);
    add_host_ipv6_encap_dest(unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, recycle_entry.encap_id, gport);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4() for SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /** Add MACT entry for bridging into SRv6 tunnel */
    cint_srv6_tunnel_info.l2_fwd_vsi = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, cint_srv6_tunnel_info.l2_fwd_mac, cint_srv6_tunnel_info.l2_fwd_vsi);
    l2addr.port = gport;
    l2addr.encap_id = BCM_ENCAP_ID_GET(recycle_entry.encap_id);
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


    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeSR6;
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);

    tunnel_init_set.ttl = 128;
    tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    BCM_L3_ITF_SET(tunnel_init_set.l3_intf_id, BCM_L3_ITF_TYPE_LIF, arp_id);

    rv = bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set);

    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_tunnel_initiator_create for IPv6 Tunnel \n");
       return rv;
    }

    /*
     * define the x3 SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t  sid_info;
    bcm_srv6_sid_initiator_info_t_init(&sid_info);

    /** to store next SID GPORT and init for SID0 to IPv6 Tunnel */
    bcm_gport_t next_sid_tunnel_id = tunnel_init_set.tunnel_id;

    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */

    int sid_idx;
    int sid_0_main_pass;
    for (sid_idx = 0; sid_idx < nof_sids_second_pass; sid_idx++)
    {
        bcm_srv6_sid_initiator_info_t_init(&sid_info);

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);

        if ((SID0 - sid_idx) == SID4)
        {
            /** set to EEDB entry of SID0 */
            sid_info.encap_access = bcmEncapAccessNativeArp;
        }
        else
        {
            /** set to EEDB entry of SID0 */
            sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx;
        }

        /** next_encap_id set to l3 interface - convert it from GPORT to l3_int */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        rv = bcm_srv6_sid_initiator_create(unit, &sid_info);

        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d \n", sid_idx);
           return rv;
        }
        if (sid_idx == (nof_sids_second_pass - 1))
        {
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_0_main_pass, sid_info.tunnel_id);
        }
        next_sid_tunnel_id = sid_info.tunnel_id;
    }

    /*
     * define the SRH Base EEDB entry for Main PASS
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

    /** we will pass the global-out-LIF id*/
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_MAIN_GLOBAL_LIF]);

    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids_second_pass;
    srh_base_info.nof_additional_sids_extended_encap = nof_sids_first_pass;

    srh_base_info.next_encap_id = sid_0_main_pass;

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


    /*
     * Extended (first) tunneling configurations
     */
    /*
     * define the SRH Base EEDB entry for Extended PASS
     */
    if(nof_sids_first_pass != 0)
    {
        printf("Starting Ext termination configurations \n");

        /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
        bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

        /** we will pass the global-out-LIF id*/
        srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID | BCM_SRV6_SRH_BASE_INITIATOR_EXTENDED_ENCAP;

        /** convert SRH global-LIF id to GPORT */
        BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_EXT_PASS_GLOBAL_LIF]);

        /** set number of SIDs*/
        srh_base_info.nof_sids = nof_sids_first_pass;

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

        printf("Ext termination SIDs configurations \n");
        /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
        next_sid_tunnel_id = 0;
    }
    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */

    for (sid_idx = 0; sid_idx < nof_sids_first_pass; sid_idx++)
    {
        bcm_srv6_sid_initiator_info_t_init(&sid_info);

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (sid_idx != 0)
        {
            /** next_encap_id set to l3 interface - convert it from GPORT to l3_int */
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);
        }

        if (sid_idx == (nof_sids_first_pass - 1))
        {
            BCM_GPORT_TUNNEL_ID_SET(sid_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_FIRST_SID_EXT_PASS_GLOBAL_LIF]);
            sid_info.flags = BCM_SRV6_SID_INITIATOR_WITH_ID;
        }

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip_ext[SID2_0 - sid_idx], 16);

        if ((SID2_0 - sid_idx) == SID2_5)
        {
            /** set to EEDB entry of SID0 */
            sid_info.encap_access = bcmEncapAccessNativeArp;
        }
        else if ((SID2_0 - sid_idx) == SID2_0)
        {
            /** set to EEDB entry of SID0 */
            sid_info.encap_access = bcmEncapAccessArp;
        }
        else
        {
            /** set to EEDB entry of SID0 */
            sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx + 1;
        }


        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        rv = bcm_srv6_sid_initiator_create(unit, &sid_info);

        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_sid_initiator_create for SID2_%d \n", sid_idx);
           return rv;
        }
        next_sid_tunnel_id = sid_info.tunnel_id;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/*
 * packet will be routed from in_port to out-port
 *
 * Route:
 * packet to send:
 *  - in port = in_port
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff00-0x7fffff0f except 0x7fffff03
 * expected:
 *  - out port = out_port
 *  - vlan = 100.
 *  - DA = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 */

/*
 * protection info options:
 * failover_id - FEC protection pointer (0 means disable protection)
 * failover_out_port - primary protection port (relevant when failover_id != 0)
 *
 * If 'do_raw' is non-zero then 'fec' is used as is (raw) within 'add_route_ipv4()'
 */
/*
 * example:
 int unit = 0;
 int in_port = 200;
 int out_port = 201;
 int in_qos_map_id = 7;
 */
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
    int encap_id = 8193 + diff_const;         /* ARP-Link-layer (needs to be higher than 8192 for Jer Plus) */
    bcm_mac_t intf_in_mac_address = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x11 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 host = 0x7fffff02;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = 0;
    int vlan = 100 + diff_const;
    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;

    char *err_proc_name;
    char *proc_name;

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

    /*
     * 6. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */

    printf("%s(): Create main FEC and set its properties.\r\n",proc_name);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create_inner(unit, fec, 0 , encap_id, gport, 0,
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
        rv = add_host_ipv4(unit, host, vrf, _l3_itf, intf_out, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
            return rv;
        }
    }
    else if(srh_next_protocol == 41)
    {
        bcm_ip6_t route_ipv6_dip = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
        rv = add_host_ipv6_encap_dest(unit, route_ipv6_dip, vrf, _l3_itf, intf_out, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
            return rv;
        }
    }
    else if(srh_next_protocol == 143) /* L2 o SRv6 case, configure MAC-T address */
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


    /** define the structure which holds the mapping between nof_sids to RCH port and encap_id */
    bcm_srv6_extension_terminator_mapping_t extension_terminator_info;
    int sid_idx;

    extension_terminator_info.flags = 0;
    /** Convert Port number to GPORT */
    BCM_GPORT_LOCAL_SET(extension_terminator_info.port, rch_port);

    /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp_1pass", NULL));
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

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
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
    tunnel_term_set.type = bcmTunnelTypeSR6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
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

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
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
    tunnel_term_set.type = bcmTunnelTypeSR6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
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
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        return rv;
    }

    return rv;
}

/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_dnx_srv6_evpn_basic.c
 * Purpose: Example of SRv6 EVPN basic configuration.
 *
 * Description:
 * -The cint is an example of traversing a MultiCast packet ETHo(SRv6 optional)oIPv6oETH EVPN packet through an SRv6 USP Egress node.
 *  It demonstrates how an EVPN packet, whether with SRH or without it, is filtered per Port per its ESI label.
 *  ESI label is located as 16b LSB of the IPv6 DIP.
 * -------------------------------------
 *
 *
 *
 *********************************
 *       Device  Demo Run        *
 *********************************
 *
 * Add/modify following SOC Properties to "config-jr2.bcm":
 * -----------------------------
 * tm_port_header_type_in_200=ETH
 * tm_port_header_type_in_40=RCH_0
 * tm_port_header_type_out_40=ETH
 * ucode_port_40=RCY.0:core_0.40
 * appl_param_psp_reduced_mode=0
 *
 * Shell Commands:
 * ---------------
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../../../src/examples/dnx/field/cint_field_srv6_evpn_esi.c
 * cint ../../../../src/examples/dnx/srv6/cint_srv6_basic.c
 * cint ../../../../src/examples/dnx/srv6/cint_dnx_srv6_evpn_basic.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_util_srv6.c
 *
 *
 *
 *
 * cint;
 * int rv;
 * rv = basic_example_ipv6(0,200,201,0,0,0);
 * print rv;
 * rv = srv6_evpn_main(0,200,201,202,203);
 * print rv;
 * rv = srv6_evpn_tunnel_termination(0,20,0,1);
 * print rv;
 * rv = srv6_evpn_tunnel_termination(0,20,1,1);
 * print rv;
 * rv = srv6_evpn_tunnel_termination(0,20,2,1);
 * print rv;
 * rv = srv6_evpn_tunnel_termination(0,20,4,0);
 * print rv;
 * rv = srv6_evpn_tunnel_termination(0,20,5,0);
 * print rv;
 * rv = srv6_evpn_tunnel_termination(0,20,6,0);
 * print rv;
 * dnx_basic_example_ipvx_for_srv6(0,143,40,201);
 * print rv;
 * exit;
 *
 * debug rx +
 *
 * 1) Multicast ETHoIPv6oETH Packet (no SRH) with ESI = 400 (allowed on all ports) - expected 4 packets out
 *
 * PacKeT TX raw port=200 DATA=000c0002000000000700010086dd6000000000338f80000000000000000000000000000000aa000000000000000012340000aaaa019000000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 *
 * rx packet 1.1:
 * data = 00000000011700000000011281000008FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 * rx packet 1.2:
 * data = 00000000011700000000011281000009FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 * rx packet 1.3:
 * data = 0000000001170000000001128100000AFFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 * rx packet 1.4:
 * data = 0000000001170000000001128100000BFFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 *
 * 2) Multicast ETHoIPv6oETH Packet (no SRH) with ESI = 300 (allowed on 2 ports) - expected 2 packets out
 *
 * PacKeT TX raw port=200 DATA=000c0002000000000700010086dd6000000000338f80000000000000000000000000000000aa000000000000000012340000aaaa012c00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 *
 * rx packet 2.1:
 * data = 00000000011700000000011281000008FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 * rx packet 2.2:
 * data = 00000000011700000000011281000009FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 *
 * 3) Multicast ETHoIPv6oETH Packet (no SRH) with ESI = 200 (allowed on 2 ports) - expected 2 packets out
 *
 * PacKeT TX raw port=200 DATA=000c0002000000000700010086dd6000000000338f80000000000000000000000000000000aa000000000000000012340000aaaa00c800000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 *
 * rx packet 3.1:
 * data = 0000000001170000000001128100000AFFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 * rx packet 3.2:
 * data = 0000000001170000000001128100000BFFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 *
 * 4) Multicast ETHoSRv6oIPv6oETH Packet with ESI = 400 (allowed on all ports) - expected 4 packets out
 *
 * PacKeT TX raw port=200 DATA=000c0002000000000700010086dd60000000006b2b80000000000000000000000000000000aa000000000000000012340000ffff01908f0600000200000001001600350070000000db070000000001001600350070000000db0700000001000000000000000012340000ffff019000000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 *
 * rx packet 1.1:
 * data = 00000000011700000000011281000008FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 * rx packet 1.2:
 * data = 00000000011700000000011281000009FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 * rx packet 1.3:
 * data = 0000000001170000000001128100000AFFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 * rx packet 1.4:
 * data = 0000000001170000000001128100000BFFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 *
 * 5) Multicast ETHoSRv6oIPv6oETH Packet with ESI = 300 (allowed on 2 ports) - expected 2 packets out
 *
 * PacKeT TX raw port=200 DATA=000c0002000000000700010086dd60000000006b2b80000000000000000000000000000000aa000000000000000012340000ffff012c8f0600000200000001001600350070000000db070000000001001600350070000000db0700000001000000000000000012340000ffff012c00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 *
 * rx packet 2.1:
 * data = 00000000011700000000011281000008FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 * rx packet 2.2:
 * data = 00000000011700000000011281000009FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 *
 * 6) Multicast ETHoSRv6oIPv6oETH Packet with ESI = 200 (allowed on 2 ports) - expected 2 packets out
 *
 * PacKeT TX raw port=200 DATA=000c0002000000000700010086dd60000000006b2b80000000000000000000000000000000aa000000000000000012340000ffff00c88f0600000200000001001600350070000000db070000000001001600350070000000db0700000001000000000000000012340000ffff00c800000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 *
 * rx packet 3.1:
 * data = 0000000001170000000001128100000AFFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 *
 * rx packet 3.2:
 * data = 0000000001170000000001128100000BFFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
 */

enum srv6_evpn_esi_idx_e
{
    SRV6_EVPN_ESI_200_MC,
    SRV6_EVPN_ESI_300_MC,
    SRV6_EVPN_ESI_400_MC,
    SRV6_EVPN_ESI_400_UC,
    SRV6_EVPN_ESI_200_MC_NO_SRH,
    SRV6_EVPN_ESI_300_MC_NO_SRH,
    SRV6_EVPN_ESI_400_MC_NO_SRH,
    SRV6_EVPN_ESI_400_UC_NO_SRH
};

int NOF_ESI_LABELS = SRV6_EVPN_ESI_400_UC_NO_SRH+1;

struct srv6_evpn_info_s
{
    bcm_vlan_port_t ce1_vlan_port;
    bcm_vlan_port_t ce3_vlan_port;
    bcm_vlan_port_t p1_vlan_port;
    bcm_vlan_port_t p2_vlan_port;
    int in_lif_acl_profile_esi;
};

struct cint_srv6_evpn_info_s {
    int ce1_port; /* ce1 port (access)*/
    int ce3_port; /* ce3 port (access) */
    int p1_port;  /* p1 port (provider) */
    int p2_port;  /* p2 port (provider) */
    int ce1_vlan; /* ce1's vlan */
    int ce3_vlan; /* ce3's vlan */
    int p1_vlan;  /* p1's vlan */
    int p2_vlan;  /* p2's vlan */
    int ce1_eve_vlan; /* ce1's eve translated vlan when packet goes out from ce-1 */
    int ce3_eve_vlan; /* ce3's eve translated vlan when packet goes out from ce-3 */
    int p1_eve_vlan;  /* p1's eve translated vlan when packet goes out from p-1 */
    int p2_eve_vlan;  /* p2's eve translated vlan when packet goes out from p-2 */
    int esi_200_ce1_ce3;  /* ESI value for ce1 and ce3 */
    int esi_300_p1_p2;    /* ESI value for p1 and p2 */
    int esi_400_no_ports; /* ESI value that's configured in the system but non blocking any port  */
    int tpid;     /* tpid value for all vlan tags in the application*/
    int vsi; /* vsi representing the service in this application */
    int ipv6_tunnel_term_lif_id[NOF_ESI_LABELS]; /* 1st Pass IPv6 termination LIF id - used for reclassification in 2nd Pass */
    bcm_ip6_t ip6_dip_with_esi[NOF_ESI_LABELS]; /** IPv6 DIPs that contain ESI labels as 16b LSB */

};


cint_srv6_evpn_info_s cint_srv6_evpn_info =
/* ports :
   ce1_port | ce3_port | p1_port | p2_port  */
    {200,      201,        202 ,     203,
/* vlans:
   ce1_vlan |  ce3_vlan | p1_vlan | p2_vlan */
    20,          20,        20,      20,
/* eve translated vlans:
   ce1_eve_vlan |  ce3_eve_vlan | p1_eve_vlan | p2_eve_vlan */
        0x8,           0x9,           0xa,          0xb,
/* ESI values:
   esi_200_ce1_ce3 |  esi_300_p1_p2 | esi_400_no_ports */
        200,           300,                  400,
/* tpid */
   0x8100,
/* vsi */
    20,
/* IPv6 Tunnel In_LIF Ids after MyMac termination:
   With SRH: esi = 200 mc |  esi = 300 mc | esi = 400 mc | esi = 400 uc*/
               { 0x5002,        0x5003,        0x5004,       0x5005,
/* Wo SRH:   esi = 200 mc |  esi = 300 mc | esi = 400 mc | esi = 400 uc*/
                 0x5012,        0x5013,        0x5014,       0x5015
   },

/* IPv6 DIPs that contain ESI as 16b as LSB: */
   {
                    /* With SRH */
                   /* esi = 200 mc */
     {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0x00,0xC8},
                   /* esi = 300 mc */
     {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0x01,0x2C},
                   /* esi = 400 mc */
     {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0x01,0x90},
                   /* esi = 400 uc */
     {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xEE,0xEE,0x01,0x90},

                    /* Wo SRH */
                  /* esi = 200 mc */
    {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xAA,0xAA,0x00,0xC8},
                  /* esi = 300 mc */
    {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xAA,0xAA,0x01,0x2C},
                  /* esi = 400 mc */
    {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xAA,0xAA,0x01,0x90},
                  /* esi = 400 uc */
    {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xBB,0xBB,0x01,0x90}

   }

};


srv6_evpn_info_s srv6_evpn_global_info;


/*
 * Select EVPN misc profiles
 */
int
srv6_evpn_profiles_set(
    int unit)
{
    /*
     * By default, FP in-lif profile is 2 bits. The value for these bits is managed by the application, in
     * consideration of other ACL application that may use these bits.
     * Need to select a value between 0-3. 0 should be reserved for non-ESI profiles.
     * Selected 1 arbitrarily.
     */
    srv6_evpn_global_info.in_lif_acl_profile_esi = 1;

    return BCM_E_NONE;
}



/*
 * Configures MyDip of an IPv6 address in an SRv6 tunnel
 */
int
srv6_evpn_tunnel_termination(
                    int unit,
                    int vsi,
                    int esi_idx,
                    int srh_present)
{

    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip;
    bcm_ip6_t ip6_sip  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};


    sal_memcpy(ip6_sip, ip6_sip, 16);
    sal_memcpy(ip6_dip, cint_srv6_evpn_info.ip6_dip_with_esi[esi_idx], 16);

    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv;

    /*
     * Configure the keys for the resulting LIF on termination (SIP, DIP, VRF)
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    if (srh_present)
    {
        tunnel_term_set.type = bcmTunnelTypeEthSR6;
    }
    else
    {
        tunnel_term_set.type = bcmTunnelTypeEthIn6;
    }

    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID | BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_term_set.tunnel_id, cint_srv6_evpn_info.ipv6_tunnel_term_lif_id[esi_idx]);

    /* If it's ESR USP, shortpipe and uniform are supported
     * Uniform: take QoS info from SRv6 tunnel
     * Shortpipe: take QoS info from fwd layer
     */
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;


    tunnel_term_set.vlan = vsi;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d, end-point-%d\n", rv, end_point_id);
        return rv;
    }

    /*
     * Set In-LIF profile for iPMF selection of ESI filter application
     * -by configuring in the In-LIF profile 2b inlif_profile for IPMF
     */

    printf("Set In-LIF profile to select ESI FP ingress application\n");
    rv = bcm_port_class_set(unit, tunnel_term_set.tunnel_id, bcmPortClassFieldIngressVport,
                            srv6_evpn_global_info.in_lif_acl_profile_esi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_port_class_set for IPv6 Tunnel Termination LIF \n");
        return rv;
    }


    return BCM_E_NONE;
}


/*
 * Main function to configure the scenario described above
 * The function performs the following activities:
 * - Setup the VSI
 * - Create ETH segments - in this example - AC links
 * - Setup EVPN services for L2 MC
 */
int
srv6_evpn_main(
    int unit,
    int ce1_port,
    int ce3_port,
    int p1_port,
    int p2_port)
{
    int rv = BCM_E_NONE;
    int vswitch_ports[4];

    printf("~~ SRv6 EVPN Basic Example ~~\n");

    printf("Load application profiles and misc configs\n");
    rv = srv6_evpn_profiles_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in srv6_evpn_profiles_set\n");
        return rv;
    }

    printf("Create VPN (%d)\n", cint_srv6_evpn_info.vsi);
    rv = srv6_evpn_vswitch_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in srv6_evpn_vswitch_create\n");
        return rv;
    }

    vswitch_ports[0] = ce1_port;
    vswitch_ports[1] = ce3_port;
    vswitch_ports[2] = p1_port;
    vswitch_ports[3] = p2_port;
    rv = srv6_evpn_vswitch_add_ports(unit, 4, vswitch_ports);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, srv6_evpn_vswitch_add_ports");
        return rv;
    }


    printf("Init ESI filter FP application\n");
    rv = srv6_evpn_esi_filter_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, srv6_evpn_esi_filter_init");
        return rv;
    }

    printf("Create ETH segments\n");
    rv = srv6_evpn_eth_segments_create(unit, ce1_port, ce3_port, p1_port, p2_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_eth_segment_create\n");
        return rv;
    }



    

    /* rv = srv6_evpn_tunnel_termination(unit, cint_srv6_evpn_info.vsi, SRV6_EVPN_ESI_200);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in srv6_evpn_tunnel_termination, esi_idx:%d\n", SRV6_EVPN_ESI_200);
        return rv;
    }
    rv = srv6_evpn_tunnel_termination(unit, cint_srv6_evpn_info.vsi, SRV6_EVPN_ESI_300);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in srv6_evpn_tunnel_termination, esi_idx:%d\n", SRV6_EVPN_ESI_300);
        return rv;

    }

    rv = srv6_evpn_tunnel_termination(unit, cint_srv6_evpn_info.vsi, SRV6_EVPN_ESI_400);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in srv6_evpn_tunnel_termination, esi_idx:%d\n", SRV6_EVPN_ESI_400);
        return rv;
    }
    printf("Done -  IPv6 Tunnel Termination in Extended Mode\n");

    */

    return rv;
}

/*
 * Create L2 VPN (VSI and MC group)
 */
int
srv6_evpn_vswitch_create(
    int unit)
{
    int rv = BCM_E_NONE;

    printf("- Create VSI\n");
    rv = bcm_vswitch_create_with_id(unit, cint_srv6_evpn_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_dnx_vswitch_create_with_id\n");
        return rv;
    }

    printf("- Create MC group\n");
    rv = multicast__open_mc_group(unit, &cint_srv6_evpn_info.vsi, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__open_mc_group\n");
        return rv;
    }

    return rv;
}

/*
 * Add ports to VPN
 */
int
srv6_evpn_vswitch_add_ports(
    int unit,
    int nof_ports,
    int *ports)
{
    int rv = BCM_E_NONE;

    while (nof_ports--)
    {
        rv = bcm_vlan_gport_add(unit, cint_srv6_evpn_info.vsi, ports[nof_ports], 0);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR, bcm_vlan_gport_add port=%d\n", ports[nof_ports]);
            return rv;
        }

    }

    return rv;
}

/*
 * Setup Field Processor based ESI filter.
 */
int
srv6_evpn_esi_filter_init(
    int unit)
{
    int rv = BCM_E_NONE;

    rv = cint_field_srv6_evpn_ingress(unit, srv6_evpn_global_info.in_lif_acl_profile_esi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, cint_field_srv6_evpn_ingress\n");
        return rv;
    }

    rv = cint_field_srv6_evpn_egress(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, cint_field_srv6_evpn_egress\n");
        return rv;
    }

    return rv;
}

/*
 * Configure in/out port
 */
int
srv6_evpn_configure_port(
    int unit,
    int port)
{
    int rv = BCM_E_NONE;

    /*
     * set class
     */
    rv = bcm_port_class_set(unit, port, bcmPortClassId, port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_port_class_set, port=%d, \n", port);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: out_port_set, port=%d\n", port);
        return rv;
    }


    return rv;
}

/*
 * Information for ES creation
 */
struct srv6_evpn_es_info_s
{
    int port;                       /* Local port */
    int vid;                        /* VLAN tag VID*/
    int eve_vid;                    /* Translation to EVE VLAN tag VID, if 0 don't do translation */
    int vsi;                        /* VSI */
    int label;                      /* ES identifier. May be set on an ESI label. Used for filtering */
    uint32 extra_flags;             /* Additional flags for bcm_vlan_port_create */
    bcm_vlan_port_t * vlan_port;    /* Pointer to allocated vlan_port object */
    int add_to_flooding;            /* Add to the global flooding domain */
};

/*
 * Create AC and (optionally) add to flooding domain of the VSI
 * - AC with VLAN EVE translation with a single tag.
 */
int
srv6_evpn_es_add(
    int unit,
    srv6_evpn_es_info_s * esi)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_t esi_port_handle;
    bcm_field_entry_t esi_label_handle;
    bcm_vlan_port_t * vport = esi->vlan_port;

    rv = srv6_evpn_configure_port(unit, esi->port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in srv6_evpn_configure_port\n");
        return rv;
    }


    bcm_vlan_port_t_init(vport);
    vport->criteria = BCM_VLAN_PORT_MATCH_NONE; /** create AC in EEDB not ESEM as LIF */
    vport->flags |= esi->extra_flags | BCM_VLAN_PORT_CREATE_EGRESS_ONLY;

    rv = bcm_vlan_port_create(unit, vport);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_port_create\n");
        return rv;
    }
    printf("Created Vport with Encap_Id %d and Gport %d\n", vport->encap_id, vport->vlan_port_id);

    rv = bcm_vlan_gport_add(unit, esi->vsi, esi->port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_gport_add\n");
        return rv;
    }


    /* Add EVE VLAN Translation to Egress AC LIF if eve_vid != 0 */
    if (esi->eve_vid)
    {

        printf("Adding VLAN Translation in AC LIF %d (Gport %x) to VID %d \n",
                vport->encap_id, vport->vlan_port_id, esi->eve_vid);

        rv = vlan_translate_ive_eve_translation_set(unit, vport->vlan_port_id,
                                                    0x8100,                           0,
                                                    bcmVlanActionReplace,             bcmVlanActionNone,
                                                    esi->eve_vid,                     0,
                                                    3, 8, 0);
                                                    /* vlan tag-format as per enum in  appl_ref_vlan_init.c
                                                     * dnx_pp_vlan_tag_format_default_t struct:
                                                     * DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG = 8 */

        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in vlan_translate_ive_eve_translation_set\n");
            return rv;
        }

    }

    printf("Add ESI filter entry to FP ESI DB\n");
    rv = cint_field_srv6_evpn_egress_esi_port_entry_add(unit, egress_context_id, egress_fg_id,
                                                   &esi_port_handle, esi->label, esi->port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in cint_field_srv6_evpn_egress_esi_port_entry_add\n");
        return rv;
    }
    rv = cint_field_srv6_evpn_egress_esi_entry_add(unit, egress_context_id, egress_fg_id,
                                                 &esi_label_handle, esi->label);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in cint_field_srv6_evpn_egress_esi_entry_add\n");
        return rv;
    }

    /* Add to flooding domain */
    if (esi->add_to_flooding)
    {
        bcm_gport_t gport;

        BCM_GPORT_MODPORT_SET(gport, unit, esi->port);


        printf("Add Port %d to Multicast Group with VSI %d with AC LIF %d\n",
                                       gport, esi->vsi, vport->encap_id);

        rv = multicast__add_multicast_entry(unit, esi->vsi, &gport,
                                            &vport->encap_id, 1, egress_mc);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in multicast__add_multicast_entry\n");
            return rv;
        }
    }


    return rv;
}

/*
 * Create ETH segments connected to the PE using AC.
 * See cint_evpn.c for specific of ESI 200 and ESI 300.
 */
int
srv6_evpn_eth_segments_create(
    int unit,
    int ce1_port,
    int ce3_port,
    int p1_port,
    int p2_port)
{
    int rv = BCM_E_NONE;
    srv6_evpn_es_info_s ce1_esi_200;
    srv6_evpn_es_info_s ce3_esi_200;
    srv6_evpn_es_info_s p1_esi_300;
    srv6_evpn_es_info_s p2_esi_300;
    bcm_field_entry_t esi_label_handle;

    printf("- Allowing ESI=400 to allow packets \n");
    rv = cint_field_srv6_evpn_egress_esi_entry_add(unit, egress_context_id, egress_fg_id,
                                                 &esi_label_handle, cint_srv6_evpn_info.esi_400_no_ports);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in cint_field_srv6_evpn_egress_esi_entry_add\n");
        return rv;
    }

    printf("- Create CE1 ES (ESI 200)\n");
    ce1_esi_200.add_to_flooding = 1;
    ce1_esi_200.port = ce1_port;
    ce1_esi_200.vid = cint_srv6_evpn_info.ce1_vlan;
    ce1_esi_200.eve_vid = cint_srv6_evpn_info.ce1_eve_vlan;
    ce1_esi_200.vsi = cint_srv6_evpn_info.vsi;
    ce1_esi_200.label = cint_srv6_evpn_info.esi_200_ce1_ce3;
    ce1_esi_200.vlan_port = &srv6_evpn_global_info.ce1_vlan_port;
    rv = srv6_evpn_es_add(unit, &ce1_esi_200);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_esi_create ce1_vlan\n");
        return rv;
    }

    printf("- Create CE3 ES (ESI 200)\n");
    ce3_esi_200.add_to_flooding = 1;
    ce3_esi_200.port = ce3_port;
    ce3_esi_200.vid = cint_srv6_evpn_info.ce3_vlan;
    ce3_esi_200.eve_vid = cint_srv6_evpn_info.ce3_eve_vlan;
    ce3_esi_200.vsi = cint_srv6_evpn_info.vsi;
    ce3_esi_200.label = cint_srv6_evpn_info.esi_200_ce1_ce3;
    ce3_esi_200.vlan_port = &srv6_evpn_global_info.ce3_vlan_port;
    rv = srv6_evpn_es_add(unit, &ce3_esi_200);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_esi_create ce3_vlan\n");
        return rv;
    }

    printf("- Create P1 ES (ESI 300)\n");
    p1_esi_300.add_to_flooding = 1;
    p1_esi_300.port = p1_port;
    p1_esi_300.vid = cint_srv6_evpn_info.p1_vlan;
    p1_esi_300.eve_vid = cint_srv6_evpn_info.p1_eve_vlan;
    p1_esi_300.vsi = cint_srv6_evpn_info.vsi;
    p1_esi_300.label = cint_srv6_evpn_info.esi_300_p1_p2;
    p1_esi_300.vlan_port = &srv6_evpn_global_info.p1_vlan_port;
    rv = srv6_evpn_es_add(unit, &p1_esi_300);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_esi_create p1_vlan\n");
        return rv;
    }

    printf("- Create P2 ES (ESI 300)\n");
    p2_esi_300.add_to_flooding = 1;
    p2_esi_300.port = p2_port;
    p2_esi_300.vid = cint_srv6_evpn_info.p2_vlan;
    p2_esi_300.eve_vid = cint_srv6_evpn_info.p2_eve_vlan;
    p2_esi_300.vsi = cint_srv6_evpn_info.vsi;
    p2_esi_300.label = cint_srv6_evpn_info.esi_300_p1_p2;
    p2_esi_300.vlan_port = &srv6_evpn_global_info.p2_vlan_port;
    rv = srv6_evpn_es_add(unit, &p2_esi_300);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_esi_create p2_vlan\n");
        return rv;
    }


    return rv;
}















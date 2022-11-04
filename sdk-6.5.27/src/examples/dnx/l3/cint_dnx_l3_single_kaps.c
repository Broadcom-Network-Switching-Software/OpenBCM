/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
/*
 * Example of a simple-router scenario
 * Test Scenario
 *
 * Test Scenario - start
 * #configure KAPS2 capacity to be 0
  config add mdb_profile=balanced
  tr 141
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
  cint ../../../../src/examples/sand/cint_ip_route_basic.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_l3_single_kaps.c
  cint
  dnx_l3_single_kaps_fwd_example(0,200,202,0xCCCE);
  exit;
 *
 * ETH-RIF packet
  tx 1 psrc=200 data=0x000c00020000000007000100080045000035000000008000fa45c08001017f020203000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data:
 * 0x00000000cd1d00123456789a81000064080045000035000000007f00fb45c08001017f020203000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Test Scenario - end
 */


/**
 * A procedure that will configure IPv4 and IPv6 UC forwarding entries in LEM and LPM with VRF = 0 when capacity for KAPS2 is 0.
 */
int
dnx_l3_single_kaps_fwd_example(
    int unit,
    int in_port,
    int out_port,
    int fec)
{
    char error_msg[200]={0,};
    int intf_in = 15;          /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int vrf = 0;
    int encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;  /* ARP-Link-layer */
    int fec_flags2 = 0;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC   */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* dest-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 route = 0x7F020203;
    uint32 host = 0x7F020202;
    uint32 mask = 0xFFFFFFFF;
    bcm_ip6_t ipv6_host = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x11 };
    bcm_ip6_t ipv6_route ={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x12 };
    bcm_ip6_t ipv6_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    int encoded_fec;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), "intf_in");
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", intf_in);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_in, intf_in_mac_address), error_msg);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, intf_out_mac_address), "intf_out");
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_out");
    /*
     * 5. Create ARP and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, intf_out, 0, 0),
        "create egress object ARP only");
    /*
     * 6. Create FEC and set its properties
     */

    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    printf("Create main FEC and set its properties.\r\n");
    BCM_GPORT_LOCAL_SET(gport, out_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec, encap_id, 0, gport, 0,fec_flags2, 0, 0, &encoded_fec),
        "create egress object FEC only");
    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, host, vrf, encoded_fec, 0, 0), "");
    }
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, route, mask, vrf, encoded_fec), "");
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec);
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_route_basic_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, fec), error_msg);

    /*
     * 7. Add host entry
     */
    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec);
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, vrf, fec), error_msg);
    }
    return BCM_E_NONE;
}

/**
 * A procedure that will configure IPv4 and IPv6 UC entries in LEM and LPM with VRF = 0
 * that will be used for RPF check when capacity for KAPS2 is 0.
 *
 * Packets received on in_port_loose will go through a loose RPF check.
 * Packets received on in_port_strict will go through a strict RPF check.
 */
int
dnx_l3_single_kaps_rpf_example(
    int unit,
    int in_port_loose,
    int in_port_strict,
    int out_port,
    int fec)
{
    int rv;
    int intf_in_loose = 15;          /* Incoming packet ETH-RIF */
    int intf_in_strict = 16;          /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int vrf = 0;
    int encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;  /* ARP-Link-layer */
    int nof_entries = 3;
    int idx;
    int fec_flags2_fwd = 0;
    int fec_flags2_rpf = 0;
    int fec_rpf_loose;
    int fec_rpf_strict;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC   */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* dest-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 route = 0x7F020203; /* 127.2.2.3 */
    uint32 host = 0x7F020202; /*  127.2.2.2 */
    uint32 sip_loose_lem = 0xC0800101; /* 192.128.1.1 */
    uint32 sip_strict_lem = 0xC0800102; /* 192.128.1.2 */
    uint32 sip_loose_kaps = 0xC0800103; /* 192.128.1.3 */
    uint32 sip_strict_kaps = 0xC0800104; /* 192.128.1.4 */
    uint32 mask = 0xFFFFFFFF;
    bcm_ip6_t ipv6_host = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x11 };
    bcm_ip6_t ipv6_route = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x12 };
    bcm_ip6_t ipv6_sip_loose_lem = { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };
    bcm_ip6_t ipv6_sip_strict_lem = { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03 };
    bcm_ip6_t ipv6_sip_loose_kaps = { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04 };
    bcm_ip6_t ipv6_sip_strict_kaps = { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05 };
    bcm_ip6_t ipv6_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    char error_msg[200]={0,};

    if (*dnxc_data_get(unit, "l3", "fec", "fer_hw_version", NULL) == 2)
    {
         BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit,0,1,&fec_rpf_loose), "");
         fec_rpf_strict = fec_rpf_loose + 1;
    } else {
        fec_rpf_loose = fec + 1;
        fec_rpf_strict = fec + 2;
    }


    int encoded_fec;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);

    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2_fwd |= BCM_L3_FLAGS2_FWD_ONLY;
        fec_flags2_rpf |= BCM_L3_FLAGS2_RPF_ONLY;
    }

    /** Config traps  */
    rv = l3_dnx_ip_rpf_config_traps(&unit, 1);
    if (rv == BCM_E_PARAM) {
        rv = BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN_MSG(rv, "");
    /*
     * 1. Set In-Port to In ETh-RIF and RIF properties
     */
    snprintf(error_msg, sizeof(error_msg), "in_port = %d intf_in = %d", in_port_strict, intf_in_strict);
    BCM_IF_ERROR_RETURN_MSG(cint_ip_rpf_port_intf_set(unit, in_port_strict, intf_in_strict, vrf, bcmL3IngressUrpfStrict), error_msg);
    snprintf(error_msg, sizeof(error_msg), "in_port = %d intf_in = %d", in_port_loose, intf_in_loose);
    BCM_IF_ERROR_RETURN_MSG(cint_ip_rpf_port_intf_set(unit, in_port_loose, intf_in_loose, vrf, bcmL3IngressUrpfLoose), error_msg);

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "ntf_out out_port %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, intf_out_mac_address), "intf_out");

    BCM_GPORT_LOCAL_SET(gport, out_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec_rpf_loose, intf_in_loose, 0, gport, 0,fec_flags2_rpf),
        "create egress object FEC only, fec_rpf_loose");

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec_rpf_strict, intf_in_strict, 0, gport, 0,fec_flags2_rpf),
        "create egress object FEC only, intf_in_strict");

    /*
     * 3. Create ARP and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, intf_out, 0, 0),
        "create egress object ARP only");
    /*
     * 4. Create FEC and set its properties
     */
    printf("Create main FEC and set its properties.\r\n");
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec, encap_id, 0, gport, 0,fec_flags2_fwd, 0, 0, &encoded_fec),
        "create egress object FEC only");

    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, host, vrf, encoded_fec, 0, 0), "");
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, sip_loose_lem, vrf, fec_rpf_loose, 0, 0), "");
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, sip_strict_lem, vrf, fec_rpf_strict, 0, 0), "");
    }

    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, route, mask, vrf, encoded_fec), "");
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_rpf(unit, sip_loose_kaps, mask, vrf, fec_rpf_loose), "");
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_rpf(unit, sip_strict_kaps, mask, vrf, fec_rpf_strict), "");

    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec);
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, ipv6_route, ipv6_mask, vrf, fec), error_msg);
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec_rpf_loose);
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6_rpf(unit, ipv6_sip_loose_kaps, ipv6_mask, vrf, fec_rpf_loose), error_msg);
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec_rpf_strict);
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6_rpf(unit, ipv6_sip_strict_kaps, ipv6_mask, vrf, fec_rpf_strict), error_msg);

    /*
     * 5. Add host entry
     */
    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec);
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, ipv6_host, vrf, fec), error_msg);
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec_rpf_loose);
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, ipv6_sip_loose_lem, vrf, fec_rpf_loose), error_msg);
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec_rpf_strict);
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, ipv6_sip_strict_lem, vrf, fec_rpf_strict), error_msg);
    }
    return BCM_E_NONE;
}

/**
 * A procedure that will configure IPv4 and IPv6 MC forwarding entries in LEM and LPM with VRF = 0 when capacity for KAPS2 is 0.
 * A multicast group will be created that will replicate packets to out_port1 and out_port2
 */
int
dnx_l3_single_kaps_mc_fwd_example(
    int unit,
    int in_port,
    int out_port1,
    int out_port2)
{
    char error_msg[200]={0,};
    int intf_in = 15;          /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int vrf = 0;
    int mc_group = 0x500;
    int out_ports[2] = {out_port1, out_port2};
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC   */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* dest-MAC */
    bcm_gport_t gport;
    uint32 route = 0xE0020203;
    uint32 host = 0xE0020202;
    uint32 tcam_route = 0xE0020204;
    uint32 mask = 0xFFFFFFFF;
    uint32 sip = 0xC0800101;
    bcm_ip6_t ipv6_host = { 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x11 };
    bcm_ip6_t ipv6_route = { 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x12 };
    bcm_ip6_t ipv6_tcam = { 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
    bcm_ip6_t ipv6_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    bcm_ip6_t ipv6_sip = { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };
    bcm_ip6_t no_addr = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    int encoded_fec;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), "intf_in");
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port %d", out_port1);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port1), error_msg);
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port %d", out_port2);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port2), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", intf_in);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_in, intf_in_mac_address), error_msg);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, intf_out_mac_address), "intf_out");
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_out");
    BCM_IF_ERROR_RETURN_MSG(create_multicast_group_and_replications(unit, mc_group, out_ports), "");

    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, host, mask, 0, 0, intf_in, vrf, mc_group, 0, 0), "to LEM table");
    }

    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, route, mask, sip, mask, 0, vrf, mc_group, 0, 0), "to LPM table");

    if (*dnxc_data_get(unit, "l3", "fwd", "tcam_entry_support", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, tcam_route, mask, sip, mask, intf_in, vrf, mc_group, 0, BCM_IPMC_TCAM),
            "to TCAM table");
    }

    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, ipv6_host, ipv6_mask, no_addr, no_addr, intf_in, vrf, 0, 0, mc_group, 0, 0, 0),
            "to LEM table");
    }
    /** When using VRF_Group compression, LEM and TCAM tables are used and config entry is mandatory */
    if (*dnxc_data_get(unit, "l3", "fwd", "ipmc_config_cmprs_vrf_group", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, ipv6_route, ipv6_mask, no_addr, no_addr, 0, vrf, 0x123, 0xFFFF, mc_group, 0, 0, 0),
            "to LPM table");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, ipv6_route, ipv6_mask, no_addr, no_addr, 0, vrf, 0, 0, mc_group, 0, 0, 0),
            "to LPM table");
    }


    if (*dnxc_data_get(unit, "l3", "fwd", "tcam_entry_support", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, ipv6_tcam, ipv6_mask, ipv6_sip, ipv6_mask, intf_in, vrf, 0, 0, mc_group, 0, BCM_IPMC_TCAM, 0),
            "to TCAM table");
    }
    return BCM_E_NONE;
}

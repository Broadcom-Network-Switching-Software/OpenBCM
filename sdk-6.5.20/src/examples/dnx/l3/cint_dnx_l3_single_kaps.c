/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Example of a simple-router scenario
 * Test Scenario
 *
 * ./bcm.user
 * cint src/examples/sand/utility/cint_sand_utils_global.c
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/dnx/l3/cint_dnx_l3_single_kaps.c
 * cint
 * dnx_l3_single_kaps_fwd_example(0,200,202,0xCCCE);
 * exit;
 *
 * ETH-RIF packet
 * tx 1 psrc = 200 data = 0x000c00020000000007000100080045000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data:
 * 0x00000000cd1d000c0002000181000064080045000035000000007f00fb45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
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
    int rv;
    int intf_in = 15;          /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int vrf = 0;
    int encap_id = 0x1384;         /* ARP-Link-layer */
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
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out out_port %d\n",out_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in %d\n", intf_in);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
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
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }
    /*
     * 6. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, intf_out, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }
    /*
     * 5. Create FEC and set its properties
     */
    printf("Create main FEC and set its properties.\r\n");
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create_inner(unit, fec, encap_id, 0, gport, 0, 0, 0, &encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    rv = add_host_ipv4(unit, host, vrf, encoded_fec, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4(), \n");
        return rv;
    }
    rv = add_route_ipv4(unit, route, mask, vrf, encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv4(), \n");
        return rv;
    }
    rv = add_route_ipv6(unit, cint_ip_route_basic_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv6: vrf = %d, fec = %d, err_id = %d \n", vrf, fec, rv);
        return rv;
    }

    /*
     * 7. Add host entry
     */
    rv = add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv6: vrf = %d, fec = %d, err_id = %d\n", vrf, fec, rv);
        return rv;
    }
    return rv;
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
    int encap_id = 0x1384;         /* ARP-Link-layer */
    int nof_entries = 3;
    int idx;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC   */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* dest-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 route = 0x7F020203;
    uint32 host = 0x7F020202;
    uint32 sip_loose_lem = 0xC0800101;
    uint32 sip_strict_lem = 0xC0800102;
    uint32 sip_loose_kaps = 0xC0800103;
    uint32 sip_strict_kaps = 0xC0800104;
    uint32 mask = 0xFFFFFFFF;
    bcm_ip6_t ipv6_host = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x11 };
    bcm_ip6_t ipv6_route = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x12 };
    bcm_ip6_t ipv6_sip_loose_lem = { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };
    bcm_ip6_t ipv6_sip_strict_lem = { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03 };
    bcm_ip6_t ipv6_sip_loose_kaps = { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04 };
    bcm_ip6_t ipv6_sip_strict_kaps = { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05 };
    bcm_ip6_t ipv6_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    int fec_rpf_loose = fec + 1;
    int fec_rpf_strict = fec + 2;
    int encoded_fec;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);

    /** Config traps  */
    rv = l3_dnx_ip_rpf_config_traps(&unit, 1);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, l3_dnx_ip_rpf_config_traps\n");
        return rv;
    }
    /*
     * 1. Set In-Port to In ETh-RIF and RIF properties
     */
    rv = cint_ip_rpf_port_intf_set(unit, in_port_strict, intf_in_strict, vrf, bcmL3IngressUrpfStrict);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_ip_rpf_port_intf_set in_port = %d intf_in = %d\n", in_port_strict, intf_in_strict);
        return rv;
    }
    rv = cint_ip_rpf_port_intf_set(unit, in_port_loose, intf_in_loose, vrf, bcmL3IngressUrpfLoose);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_ip_rpf_port_intf_set in_port = %d intf_in = %d\n", in_port_loose, intf_in_loose);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out out_port %d\n",out_port);
        return rv;
    }

    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create(unit, fec_rpf_loose, intf_in_loose, 0, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, fec_rpf_loose\n");
        return rv;
    }

    rv = l3__egress_only_fec__create(unit, fec_rpf_strict, intf_in_strict, 0, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, intf_in_strict\n");
        return rv;
    }

    /*
     * 6. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, intf_out, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }
    /*
     * 5. Create FEC and set its properties
     */
    printf("Create main FEC and set its properties.\r\n");
    rv = l3__egress_only_fec__create_inner(unit, fec, encap_id, 0, gport, 0, 0, 0, &encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    rv = add_host_ipv4(unit, host, vrf, encoded_fec, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4(), \n");
        return rv;
    }
    rv = add_host_ipv4(unit, sip_loose_lem, vrf, fec_rpf_loose, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4(), \n");
        return rv;
    }
    rv = add_host_ipv4(unit, sip_strict_lem, vrf, fec_rpf_strict, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4(), \n");
        return rv;
    }

    rv = add_route_ipv4(unit, route, mask, vrf, encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv4(), \n");
        return rv;
    }
    rv = add_route_ipv4(unit, sip_loose_kaps, mask, vrf, fec_rpf_loose);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv4(), \n");
        return rv;
    }
    rv = add_route_ipv4(unit, sip_strict_kaps, mask, vrf, fec_rpf_strict);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv4(), \n");
        return rv;
    }

    rv = add_route_ipv6(unit, ipv6_route, ipv6_mask, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv6: vrf = %d, fec = %d, err_id = %d \n", vrf, fec, rv);
        return rv;
    }
    rv = add_route_ipv6(unit, ipv6_sip_loose_kaps, ipv6_mask, vrf, fec_rpf_loose);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv6: vrf = %d, fec = %d, err_id = %d \n", vrf, fec_rpf_loose, rv);
        return rv;
    }
    rv = add_route_ipv6(unit, ipv6_sip_strict_kaps, ipv6_mask, vrf, fec_rpf_strict);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv6: vrf = %d, fec = %d, err_id = %d \n", vrf, fec_rpf_strict, rv);
        return rv;
    }

    /*
     * 7. Add host entry
     */
    rv = add_host_ipv6(unit, ipv6_host, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv6: vrf = %d, fec = %d, err_id = %d\n", vrf, fec, rv);
        return rv;
    }
    rv = add_host_ipv6(unit, ipv6_sip_loose_lem, vrf, fec_rpf_loose);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv6: vrf = %d, fec = %d, err_id = %d\n", vrf, fec_rpf_loose, rv);
        return rv;
    }
    rv = add_host_ipv6(unit, ipv6_sip_strict_lem, vrf, fec_rpf_strict);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv6: vrf = %d, fec = %d, err_id = %d\n", vrf, fec_rpf_strict, rv);
        return rv;
    }
    return rv;
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
    int rv;
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
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out out_port %d\n", out_port1);
        return rv;
    }
    rv = out_port_set(unit, out_port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out out_port %d\n", out_port2);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in %d\n", intf_in);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
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
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }
    rv = create_multicast_group_and_replications(unit, mc_group, out_ports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create_multicast_group_and_replications\n");
        return rv;
    }

    rv = add_ipv4_ipmc(unit, host, mask, 0, 0, intf_in, vrf, mc_group, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv4_ipmc to LEM table, \n");
        return rv;
    }

    rv = add_ipv4_ipmc(unit, route, mask, sip, mask, 0, vrf, mc_group, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function %s to LPM table \n");
        return rv;
    }

    rv = add_ipv4_ipmc(unit, tcam_route, mask, sip, mask, intf_in, vrf, mc_group, 0, BCM_IPMC_TCAM);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv4_ipmc to TCAM table, \n");
        return rv;
    }

    rv = add_ipv6_ipmc(unit, ipv6_host, ipv6_mask, no_addr, no_addr, intf_in, vrf, 0, 0, mc_group, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv6_ipmc to LEM table, \n");
        return rv;
    }
    rv = add_ipv6_ipmc(unit, ipv6_route, ipv6_mask, no_addr, no_addr, 0, vrf, 0, 0, mc_group, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv6_ipmc to LPM table, \n");
        return rv;
    }
    rv = add_ipv6_ipmc(unit, ipv6_tcam, ipv6_mask, ipv6_sip, ipv6_mask, intf_in, vrf, 0, 0, mc_group, 0, BCM_IPMC_TCAM);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv6_ipmc to LPM table, \n");
        return rv;
    }
    return rv;
}

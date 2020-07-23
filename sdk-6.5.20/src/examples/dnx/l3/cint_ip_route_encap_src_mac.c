/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */
/*
 * Example of a simple-router scenario to encapsulate the source MAC
 * Test Scenario 
 *     For MY-MAC MSB is not configurable which not meet the requirement to configure the custom source MAC for some scenario,
 *     So Jericho2 add the capability to support the custom source MAC.
 *
 * ./bcm.user
 * cint src/examples/sand/utility/cint_sand_utils_global.c
 * cint src/examples/sand/utility/cint_sand_utils_l3.c
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/dnx/l3/cint_ip_route_encap_src_mac.c
 * cint
 * basic_example(0,200,202);
 * exit;
 *
 * ETH-RIF packet 
 * tx 1 psrc = 200 data = 0x000c00020000000007000100080045000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be: 
 * Data:
 * 0x00000000cd1d00112233445581000064080045000035000000007f00fb45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 */

/******* Run example ******/

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
 *  - SA = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}
 *  TTL decremented
 */

/*
 * protection info options:
 * failover_id - FEC protection pointer (0 means disable proteection)
 * failover_out_port - primary protection port (relevant when failover_id != 0)
 */
/*
 * example: 
 int unit = 0;
 int in_port = 200;
 int out_port = 201;
 */
int
basic_example_encap_src_mac(
    int unit,
    int in_port,
    int out_port,
    int host_format,
    int is_custom_sa,
    bcm_failover_t failover_id,
    int failover_out_port)
{
    int rv;
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int fec;
    int vrf = 1;
    int encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;         /* ARP-Link-layer */
    int host_encap_id = 0;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_mac_t custom_sa_mac = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };        /* src-MAC */
    bcm_gport_t gport;
    uint32 route = 0x7fffff00;
    uint32 host = 0x7fffff02;
    uint32 mask = 0xfffffff0;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = 0;
    int vlan = 100;
    int failover_is_primary = 0;    
    sand_utils_l3_arp_s arp_structure;
    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
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
        printf("Error, out_port_intf_set intf_out\n");
        return rv;
    }

    if (failover_id != 0)
    {
        rv = out_port_set(unit, failover_out_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, out_port_intf_set intf_out\n");
            return rv;
        }
    }

    /*
     * 3. Create ETH-RIF and set its properties 
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
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
     * 5. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */
    if (host_format != 2)
    {
        BCM_GPORT_LOCAL_SET(gport, out_port);
        rv = l3__egress_only_fec__create_inner(unit, fec, ((host_format == 1) || (host_format == 3) ) ? 0 : intf_out, encap_id, gport, 0,
                                               failover_id, failover_is_primary,&encoded_fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rv;
        }

        /** if protection is enabled , also create another fec(primary) */
        if (failover_id != 0)
        {
            fec--;
            BCM_GPORT_LOCAL_SET(gport, failover_out_port);
            failover_is_primary = 1;
            rv = l3__egress_only_fec__create_inner(unit, fec, (host_format == 1) ? 0 : intf_out, encap_id, gport, 0,
                                                   failover_id, failover_is_primary,&encoded_fec);
            if (rv != BCM_E_NONE)
            {
                printf("Error, create egress object FEC only\n");
                return rv;
            }
        }
    }

    /*
     * 6. Create ARP and set its properties
     */
    if (is_custom_sa)
    {
        flags2 |= BCM_L3_FLAGS2_SRC_MAC;
    }
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, encap_id, 0, flags2, arp_next_hop_mac, vlan);
    arp_structure.src_mac_addr = custom_sa_mac;
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }

    /*
     * 7. Add Route entry
     */
    rv = add_route_ipv4(unit, route, mask, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function internal_ip_route, \n");
        return rv;
    }

    /*
     * 8. Add host entry
     */
    BCM_GPORT_LOCAL_SET(gport, out_port);
    if (host_format == 1)
    {
        rv = add_host_ipv4(unit, host, vrf, encoded_fec, intf_out, 0);
    }
    else if (host_format == 2)
    {
        rv = add_host_ipv4(unit, host, vrf, intf_out, encap_id /* arp id */ , gport);
    }
    else
    {
        rv = add_host_ipv4(unit, host, vrf, encoded_fec, 0, 0);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function internal_ip_host, \n");
        return rv;
    }

    return rv;
}


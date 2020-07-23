/*
 * $Id: cint_ip_unknown_destination.c
 * Exp $
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Example of a simple mc router scenario to flood packet to a chosen MC group.

 *
 * ./bcm.user
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/sand/cint_ipmc_route_basic.c
 * cint src/examples/dnx/l3/cint_ip_unknown_destination.c
 * cint
 * cint_ip_unknown_multicast_destination(0,200,201,202);
 * exit;
 *
 * ETH-RIF packet
 * tx 1 psrc = 200 data = 0x01005e020202000007000100810000640800450000350000000080009743c0800101e0020202000102030405060708090a
 * Received packets on unit 0 should be:
 * Data:
 * 0x01005e020202000007000100810000640800450000350000000080009743c0800101e0020202000102030405060708090a
 * 0x01005e020202000007000100810000640800450000350000000080009743c0800101e0020202000102030405060708090a
 */

/******* Run example ******/

/*
 * packets will be routed from in_port to out-port1 and out-port2 after MC group is set to flooding destination
 *
 * Route:
 * packet to send:
 *  - in port = in_port
 *  - Vlan ID = 100
 *  - DA = Any
 *  - DIP = Any
 * expected:
 *  - out port = out_port
 *  - vlan = 100.
 *  - DA = input SA
 *  - SA = input SA
 *  TTL decremented
 */

int
cint_ip_unknown_multicast_destination(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int is_bridge_fallback)
{
    int rv;
    int intf_in = 150;
    int intf_out = 50;
    int encap_id = 0x1234;
    int vsi_da_not_found_destination = 10;
    int unknown_da_profile = 7;

    bcm_ip_t mask = 0xFFFFFFFF;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_vlan_control_vlan_t control;
    bcm_vlan_control_vlan_t_init(&control);
    bcm_port_flood_group_t flood_groups;
    bcm_gport_t flood_destination;
    int idx;
    int is_jericho2 = 0;
    int flags;
    int ipv6_mc_da_is_uc = 0;
    bcm_vlan_port_t vlan_port;
    int out_lifs[2];
    bcm_vlan_port_t_init(&vlan_port);
    bcm_multicast_replication_t replications[cint_ipmc_info.nof_replications];

    cint_ipmc_info.out_ports[0] = out_port1;
    cint_ipmc_info.out_ports[1] = out_port2;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    cint_ipmc_info.intf_in = intf_in;

    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE) {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }
    if (is_jericho2)
    {
        ipv6_mc_da_is_uc = *(dnxc_data_get(unit, "l3", "feature", "ipv6_mc_compatible_dmac", NULL));
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
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.match_vlan = intf_out;
    vlan_port.vsi = 0;
    for (idx = 0; idx < 2; idx++)
    {
        vlan_port.port = cint_ipmc_info.out_ports[idx];
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
            return rv;
        }
        out_lifs[idx] = vlan_port.vlan_port_id;
        printf("port=%d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vlan_port.port, vlan_port.vlan_port_id,
               vlan_port.encap_id);
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = bcm_vlan_create(unit, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan create intf_in\n");
        return rv;
    }
	
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    ingress_rif.intf_id = intf_in;
    ingress_rif.vrf = intf_in;
    if (is_bridge_fallback == 1)
    {
        ingress_rif.flags = BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
    }
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * 4. Create multicast replications
     */
    set_multicast_replication(&replications[0], cint_ipmc_info.out_ports[0], 0x3fffff & out_lifs[0]);
    set_multicast_replication(&replications[1], cint_ipmc_info.out_ports[1], 0x3fffff & out_lifs[1]);

    bcm_port_flood_group_t_init(&flood_groups);
    BCM_GPORT_MCAST_SET(flood_destination, unknown_da_profile);
    flood_groups.unknown_multicast_group = flood_destination;
    if (*dnxc_data_get(unit, "l2", "feature", "bc_same_as_unknown_mc", NULL)) {
        flood_groups.broadcast_group = flood_groups.unknown_multicast_group;
    }

    /*
     * The IPv6 MC packet ETH header is parsed as a UC one.
     * Therefore we need to update the unknown_unicast_group to be able to hit the default flooding destination.
     */
    if (ipv6_mc_da_is_uc)
    {
        flood_groups.unknown_unicast_group = flood_destination;
    }
    rv = bcm_port_flood_group_set(unit, in_port, 0, &flood_groups);

    control.flags2 = 0;
    control.broadcast_group = vsi_da_not_found_destination;
    control.unknown_multicast_group = vsi_da_not_found_destination;
    control.unknown_unicast_group = vsi_da_not_found_destination;
    control.forwarding_vlan = intf_in;
    rv = bcm_vlan_control_vlan_set(unit, intf_in, control);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_control_vlan_set in vsi= %d\n", intf_in);
        return rv;
    }

    /*
     * 5. Create a multicast group
     */
    cint_ipmc_info.mc_group = vsi_da_not_found_destination + unknown_da_profile;
    flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
    rv = bcm_multicast_create(unit, flags, &cint_ipmc_info.mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    rv = bcm_multicast_add(unit, cint_ipmc_info.mc_group, BCM_MULTICAST_INGRESS_GROUP, 2, replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    return rv;
}


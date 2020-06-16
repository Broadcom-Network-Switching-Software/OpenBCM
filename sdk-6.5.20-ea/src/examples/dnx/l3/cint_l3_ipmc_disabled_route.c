/*
 * $Id: cint_l3_ipmc_disabled_route.c
 * Exp $
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 *
 * ./bcm.user
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/sand/cint_ipmc_route_basic.c
 * cint src/examples/dnx/l3/cint_l3_ipmc_disabled_route.c
 * cint
 * cint_ipmc_disabled_switch_l3mcastl2(0,200,201,202,203);
 * exit;
 *
 * ETH-RIF packet
 * tx 1 psrc = 200 data = 0x01005e020202000007000100810000960800450000350000000080009743c0800101e0020202000102030405060708090a0b0c0d0e0f
 * Received packets on unit 0 should be:
 * Data:
 * 0x01005e020202000007000100810000960800450000350000000080009743c0800101e0020202000102030405060708090a0b0c0d0e0f
 * 0x01005e020202000007000100810000960800450000350000000080009743c0800101e0020202000102030405060708090a0b0c0d0e0f
 */
int l2_mc_group;
/******* Run example ******/

/*
 * packets will be routed from in_port to out-port1 and out-port2 after L2 MC group is hit
 *
 * Route:
 * packet to send:
 *  - in port = in_port
 *  - Vlan ID = 150
 *  - DA = 01:00:5E:02:02:02
 *  - DIP = 0xE0020202
 * expected:
 *  - out port = out_port1, outport2
 *  - vlan = 150
 *  - DA = input DA
 *  - SA = input SA
 *  TTL decremented
 */
int
cint_ipmc_disabled_switch_l3mcastl2(
    int unit,
    int in_port,
    int in_port2,
    int out_port1,
    int out_port2)
{
    int rv;
    int intf_in = 150;
    int out_rifs[4] = {50, 51, 52, 53};

    bcm_ip_t mask = 0xFFFFFFFF;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    int idx;
    int flags;
    bcm_vlan_port_t vlan_port;
    int out_lifs[2];
    
    bcm_vlan_port_t_init(&vlan_port);
    bcm_multicast_replication_t replications[cint_ipmc_info.nof_replications];

    cint_ipmc_info.out_ports[0] = out_port1;
    cint_ipmc_info.out_ports[1] = out_port2;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    cint_ipmc_info.intf_in = intf_in;
    l2_mc_group = 0x1111;
    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    rv = in_port_intf_set(unit, in_port2, intf_in+1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchL3McastL2, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }

    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    ingress_rif.intf_id = intf_in;
    ingress_rif.vrf = intf_in;
    ingress_rif.flags = BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST | BCM_L3_INGRESS_L3_MCAST_L2;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }
    for (idx = 0; idx < 4; idx++)
    {
        rv = intf_eth_rif_create(unit, out_rifs[idx], intf_out_mac_address);
        if (rv != BCM_E_NONE)
        {
            printf("Error, intf_eth_rif_create intf_in\n");
            return rv;
        }
        intf_out_mac_address[5] ++;
    }

    /*
     * Create multicast replications
     */
    set_multicast_replication(&replications[0], cint_ipmc_info.out_ports[0], -1);
    set_multicast_replication(&replications[1], cint_ipmc_info.out_ports[1], -1);

    /*
     * Create a multicast group
     */
    flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
    rv = bcm_multicast_create(unit, flags, &l2_mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    rv = bcm_multicast_add(unit, l2_mc_group, BCM_MULTICAST_INGRESS_GROUP, 2, replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    rv = add_ipv4_ipmc(unit, cint_ipmc_info.mc_host_dip, mask, 0, 0, intf_in, intf_in, l2_mc_group, 0, BCM_IPMC_L2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv4_ipmc, \n");
        return rv;
    }
    
    l2_mc_group++;
    /*
     * Create multicast replications
     */
    set_multicast_replication(&replications[0], cint_ipmc_info.out_ports[0], -1);
    set_multicast_replication(&replications[1], cint_ipmc_info.out_ports[1], -1);

    /*
     * Create a multicast group
     */
    flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
    rv = bcm_multicast_create(unit, flags, &l2_mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    rv = bcm_multicast_add(unit, l2_mc_group, BCM_MULTICAST_INGRESS_GROUP, 2, replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }
    
    bcm_l2_addr_t l2_addr;
    bcm_mac_t mc_mac = {0x01, 0, 0x5E, 0x2, 0x2, 0x2};
    bcm_l2_addr_t_init(&l2_addr, mc_mac, intf_in + 1);
    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    l2_addr.l2mc_group = l2_mc_group;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("bcm_l2_addr_add failed \n");
        return rv;
    }

    return rv;
}

/**
 * Helper function that flips the switch bcmSwitchL3McastL2 to 0 and creates a new RIF with ID 151.
 */
int cint_ipmc_disabled_switch_additional(
    int unit)
{
    int rv;
    int intf_in = 151;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    
    rv = bcm_switch_control_set(unit, bcmSwitchL3McastL2, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }
    
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    ingress_rif.intf_id = intf_in;
    ingress_rif.vrf = intf_in;
    ingress_rif.flags = BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }


    return rv;
}

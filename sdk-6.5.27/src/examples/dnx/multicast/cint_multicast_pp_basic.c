/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

int failover_id = 100;
int pwe_mpls_tunnel_id1;
int pwe_mpls_tunnel_id2;
/*
 * Configuration:
 * 
 * Test Scenario - start
  cint ../../../../src/examples/dnx/multicast/cint_multicast_pp_basic.c
  cint
  int unit = 0;
  int rv = 0;
  int in_port = 200;
  int out_port1 = 201;
  int out_port2 = 202;
  int out_port3 = 203;
  int nof_replications = 2;
  rv = multicast_pp_basic(unit,in_port,out_port1,out_port2,out_port3,nof_replications);
  print rv;
 * Test Scenario - end
 * 
 */

int multicast_pp_basic(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int nof_replications)
{
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 0x3000FF; /* Outgoing packet ETH-RIF */
    int vrf = 1;
    int idx;
    int replication_list[3] = {0x1500, 0x1501, 0x1502};
    bcm_mac_t intf_in_mac_address = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a }; /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x01, 0x00, 0x5E, 0x02, 0x02, 0x02 };
    uint32 host = 0xE0020202;
    uint32 sip = 0xc0800101;
    uint32 mask = 0xffffffff;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags = BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID;
    int mc_group = 5500;
    int encap_id = 0x1500;
    int nof_cuds;
    int label = 0x246;
    int out_ports[3] = {out_port1, out_port2, out_port3};
    int max_nof_outlifs = 3;
    bcm_multicast_replication_t replications[nof_replications];

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), "intf_in");

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    for (idx = 0; idx < max_nof_outlifs; idx++)
    {
        BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_ports[idx]), "intf_out");
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_create(unit, intf_in, intf_in_mac_address, vrf, 0), "");

    /*
     * 4. Create out interface which will be used for the MPLS ARP.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, intf_out_mac_address), "");
    /*
     * 5. Create an ARP which is going to be used for the MPLS tunnel initiator.
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &(cint_mpls_encapsulation_basic_info.arp_id_push), arp_next_hop_mac,
            intf_out), "create egress object ARP only, arp_id");
    intf_out++;

    /*
     * 6. Create the MPLS tunnel and assign the tunnel ID to be the first in the replication array.
     */
    cint_mpls_encapsulation_basic_info.pushed_or_swapping_label = label;
    BCM_IF_ERROR_RETURN_MSG(mpls_encapsulation_basic_create_push_or_swap_tunnel(unit, 64), "");
    printf("TUNNEL ID = %d\n\n", cint_mpls_encapsulation_basic_info.push_tunnel_id);
    replication_list[0] = cint_mpls_encapsulation_basic_info.push_tunnel_id;

    /*
     * 7. Create multicast replications
     */
    for (idx = 0; idx < nof_replications; idx++)
    {
        /*
         * 7.1 Create an L3 interface
         */
        /*
         * in case intf_out == eth_rif_id it should be already created
         */
        if (intf_out != intf_in)
        {
            BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, intf_out_mac_address), "");
        }
        /*
         * 7.2 Create an ARP for this intf_out that will be used in the list of replications of the PPMC entry
         */
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &replication_list[1], arp_next_hop_mac, intf_out),
            "create egress object ARP only, arp_id");
        /*
         * 7.3 Set the replication.
         */
        set_multicast_replication(&replications[idx], out_ports[idx], intf_out);

        /*
         * 7.4 Create the PPMC entry for this replication
         */
        /** The number of outLIFs for an entry is determined as 2 for the first replication and 3 for all others */
        nof_cuds = 2;
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_encap_extension_create(unit, flags, intf_out, nof_cuds, replication_list), "");

        /*
         * 7.5 Set some changes for the next replication.
         */
        intf_out++;
        replication_list[1]++;
        arp_next_hop_mac[5]++;
        intf_out_mac_address[5]++;
    }

    /*
     * 8. Create a multicast group
     */
    BCM_IF_ERROR_RETURN_MSG(create_multicast(unit, replications, nof_replications, mc_group), "");

    /*
     * 9. Create an IPMC host entry which results to the created MC group.
     */
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, host, mask, sip, mask, intf_in, vrf, mc_group, 0, 0), "to LEM table");

    return BCM_E_NONE;
}

/*
 * The cint configuration prepares the needed entries to successfully forward over ETH using a MC group.
 * The MC group is deliberately left empty in order to validate different order and type of MC replications.
 *
 * Configuration:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/multicast/cint_multicast_pp_basic.c
  cint
  int unit = 0;
  int rv = 0;
  int in_port = 200;
  int out_port1 = 201;
  int out_port2 = 202;
  int out_port3 = 203;
  int mc_id = 1000;
  rv = cint_multicast_ingress_pruning_same_intf_l2_fwd(unit,in_port,out_port1,out_port2,out_port3,mc_id);
  print rv;
 * Test Scenario - end
 * 
 */
int cint_multicast_ingress_pruning_same_intf_l2_fwd(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int mc_id)
{
    bcm_mac_t my_mac_address = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};     /* my-MAC */
    int vid_in = 15;
    uint32 pruning_flags = BCM_MULTICAST_REP_PRUNING | BCM_MULTICAST_REP_SAME_INTERFACE_EN;
    uint32 set_flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
    BCM_IF_ERROR_RETURN_MSG(cint_multicast_pruning_protection_test_internal(unit, in_port, out_port1, out_port2, out_port3, mc_id), "");

    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(l2_addr, my_mac_address, vid_in);
    l2_addr.flags = BCM_L2_MCAST | BCM_L2_STATIC;
    l2_addr.l2mc_group = mc_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr), "");
    return BCM_E_NONE;
}

/*
 * Create VSI ETH-RIF and set its properties:
 * - eth_rif: ETH-RIF ID
 * - my_mac_address - My-MAC address
 * - vrf - Virtual routing interface
 * - flags - Ingress property flags
 */
int intf_ingress_rif_create(
    int unit,
    int eth_rif,
    bcm_mac_t my_mac_address,
    int vrf,
    uint32 flags)
{
    l3_ingress_intf ingress_rif;
    /*
     * Set Incoming ETH-RIF properties
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, eth_rif, my_mac_address), "intf_in");

    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = eth_rif;
    ingress_rif.flags = flags;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "ingress_rif");
    return BCM_E_NONE;
}

/*
 * The cint configuration prepares the needed entries to successfully forward over ETH when having an IPv4oETH compatible-MC packet.
 * The MC group is deliberately left empty in order to validate different order and type of MC replications.
 *
 * Configuration:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/multicast/cint_multicast_pp_basic.c
  cint
  int unit = 0;
  int rv = 0;
  int in_port = 200;
  int out_port1 = 201;
  int out_port2 = 202;
  int out_port3 = 203;
  int mc_id = 1000;
  rv = cint_multicast_ingress_pruning_same_intf_ipmc_l2(unit,in_port,out_port1,out_port2,out_port3,mc_id);
  print rv;
 * Test Scenario - end
 * 
 */
int cint_multicast_ingress_pruning_same_intf_ipmc_l2(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int mc_id)
{
    uint32 route = 0xE0020202;
    uint32 sip = 0xc0800101;
    uint32 mask = 0xffffffff;
    bcm_ip6_t dip_v6_addr = {0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x2, 0x2, 0x2};
    bcm_ip6_t v6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t sip_v6_addr = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    bcm_mac_t my_mac_address = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};     /* my-MAC */
    bcm_mac_t ipv4mc_l2_address = {0x01, 0x00, 0x5E, 0x02, 0x02, 0x02};
    bcm_mac_t ipv6mc_l2_address = {0x33, 0x33, 0x00, 0x02, 0x02, 0x02};
    int vrf = 1;
    int vid_in1 = 15;
    int vid_in2 = 16;
    

    uint32 pruning_flags = BCM_MULTICAST_REP_PRUNING | BCM_MULTICAST_REP_SAME_INTERFACE_EN;
    uint32 set_flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;

    BCM_IF_ERROR_RETURN_MSG(cint_multicast_pruning_protection_test_internal(unit, in_port, out_port1, out_port2, out_port3, mc_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vid_in2), "intf_out");
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, out_port1, vid_in2), "intf_in");

    /** Create ETH-RIF and set its properties */
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_create(unit, vid_in1, my_mac_address, vrf, BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST | BCM_L3_INGRESS_L3_MCAST_L2 | BCM_L3_INGRESS_IP6_L3_MCAST_L2), "");
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_create(unit, vid_in2, my_mac_address, vrf, BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST), "");

    /** Add V4Mc and v6MC fowarding entries. */
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc_bridge(unit, route, mask, sip, mask, vid_in1, 0, mc_id, 0, 0), "");
    
    BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc_bridge(unit, dip_v6_addr, v6_mask, sip_v6_addr, v6_mask, 0, vid_in1, 0x0CEE, mc_id, 0, 0), "");

    /** Add L2 MC forwarding entries. */
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(l2_addr, ipv4mc_l2_address, vid_in2);
    l2_addr.flags = BCM_L2_MCAST | BCM_L2_STATIC;
    l2_addr.l2mc_group = mc_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr), "");

    bcm_l2_addr_t_init(l2_addr, ipv6mc_l2_address, vid_in2);
    l2_addr.flags = BCM_L2_MCAST | BCM_L2_STATIC;
    l2_addr.l2mc_group = mc_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr), "");

    return BCM_E_NONE;
}


/**
 * An internal function to add the common configuration
 * that is needed to validate Ingress Mc pruning when forwarding over L2 and IPMC for split horizon.
 */
int multicast_ingress_pruning_split_horizon_common(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int mc_id)
{
    char error_msg[200]={0,};
    int idx;
    int vid_in = 20;
    int vid_out = 0xFF;        /* Outgoing packet ETH-RIF */
    int ingr_orientation = 2;
    int egr_orientation = 3;
    int nof_out_ports = 3;
    int out_ports[3] = {out_port1, out_port2, out_port3};
    uint32 create_flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC | BCM_MULTICAST_WITH_ID;
    /*
     * 1. Set In-Port to In ETh-RIF
     */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = vid_in;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.ingress_network_group_id = ingr_orientation;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vid_in, in_port, 0), "");

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    for (idx = 0; idx < nof_out_ports; idx++)
    {
        snprintf(error_msg, sizeof(error_msg), "out_port[%d] = %d", idx, out_ports[idx]);
        BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_ports[idx]), error_msg);
    }

    uint32 config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
    bcm_switch_network_group_config_t config_set;
    bcm_switch_network_group_config_t_init(&config_set);
    config_set.dest_network_group_id = egr_orientation;
    config_set.config_flags = config_flags;
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit, ingr_orientation, &config_set), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vid_in), "intf_out");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vid_out), "intf_out");
    /** 4. Configure a MC group with 1 replication that has same interface filtering enabled.*/
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, create_flags, mc_id), "intf_out");
    return BCM_E_NONE;
}

/*
 * The cint configuration prepares the needed entries to successfully forward over ETH using a MC group.
 * The MC group is deliberately left empty in order to validate different order and type of MC replications.
 * The confgiuration includes preparation for split horizon filtering when ingress orientation is 2 and egress is 3.
 *
 * Configuration:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/multicast/cint_multicast_pp_basic.c
  cint
  int unit = 0;
  int rv = 0;
  int in_port = 200;
  int out_port1 = 201;
  int out_port2 = 202;
  int out_port3 = 203;
  int mc_id = 1000;
  rv = cint_multicast_ingress_pruning_ipmc_l2(unit,in_port,out_port1,out_port2,out_port3,mc_id);
  print rv;
 * Test Scenario - end
 * 
 */
int cint_multicast_ingress_pruning_split_horizon_l2_fwd(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int mc_id)
{
    int vid_in = 15;
    bcm_mac_t my_mac_address = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};     /* my-MAC */

    BCM_IF_ERROR_RETURN_MSG(cint_multicast_pruning_protection_test_internal(unit, in_port, out_port1, out_port2, out_port3, mc_id), "");

    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(l2_addr, my_mac_address, vid_in);
    l2_addr.flags = BCM_L2_MCAST | BCM_L2_STATIC;
    l2_addr.l2mc_group = mc_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr), "");
    return BCM_E_NONE;
}

/*
 * The cint configuration prepares the needed entries to successfully forward over IP MC in case of IPoETH using a MC group.
 * The MC group is deliberately left empty in order to validate different order and type of MC replications.
 * The confgiuration includes preparation for split horizon filtering when ingress orientation is 2 and egress is 3.
 *
 * Configuration:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/multicast/cint_multicast_pp_basic.c
  cint
  int unit = 0;
  int rv = 0;
  int in_port = 200;
  int out_port1 = 201;
  int out_port2 = 202;
  int out_port3 = 203;
  int mc_id = 1000;
  rv = cint_multicast_ingress_pruning_split_horizon_ipmc_fwd(unit,in_port,out_port1,out_port2,out_port3,mc_id);
  print rv;
 * Test Scenario - end
 * 
 */
int cint_multicast_ingress_pruning_split_horizon_ipmc_fwd(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int mc_id)
{
    int vid_in1 = 15;
    int vid_in2 = 16;
    uint32 route = 0xE0020202;
    uint32 sip = 0xc0800101;
    uint32 mask = 0xffffffff;
    bcm_ip6_t dip_v6_addr = {0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x2, 0x2, 0x2};
    bcm_ip6_t v6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t sip_v6_addr = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    bcm_mac_t my_mac_address = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};     /* my-MAC */
    bcm_mac_t ipv4mc_l2_address = {0x01, 0x00, 0x5E, 0x02, 0x02, 0x02};
    bcm_mac_t ipv6mc_l2_address = {0x33, 0x33, 0x00, 0x02, 0x02, 0x02};
    int vrf = 1;

    int ingr_orientation = 2;
    int egr_orientation = 3;
    BCM_IF_ERROR_RETURN_MSG(cint_multicast_pruning_protection_test_internal(unit, in_port, out_port1, out_port2, out_port3, mc_id), "");
    uint32 config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
    bcm_switch_network_group_config_t config_set;
    bcm_switch_network_group_config_t_init(&config_set);
    config_set.dest_network_group_id = egr_orientation;
    config_set.config_flags = config_flags;
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit, ingr_orientation, &config_set), "");
    /*
     * 1. Set In-Port to In ETh-RIF
     */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = out_port1;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = vid_in2;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.ingress_network_group_id = ingr_orientation;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vid_in2, out_port1, 0), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vid_in2), "intf_out");

    /*
     * Set Incoming ETH-RIF properties
     * VID1 will indicate v4MC and v6MC forwarding type.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_create(unit, vid_in1, my_mac_address, vrf, BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST | BCM_L3_INGRESS_L3_MCAST_L2 | BCM_L3_INGRESS_IP6_L3_MCAST_L2), "");

    /** VID2 will indicate v4Bridge and v6Bridge forwarding type */
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_create(unit, vid_in2, my_mac_address, vrf, BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST), "");

    /** Add V4Mc and v6MC fowarding entries. */
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc_bridge(unit, route, mask, sip, mask, vid_in1, 0, mc_id, 0, 0), "");
    
    BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc_bridge(unit, dip_v6_addr, v6_mask, sip_v6_addr, v6_mask, 0, vid_in1, 0x0CEE, mc_id, 0, 0), "");

    /** Add L2 MC forwarding entries. */
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(l2_addr, ipv4mc_l2_address, vid_in2);
    l2_addr.flags = BCM_L2_MCAST | BCM_L2_STATIC;
    l2_addr.l2mc_group = mc_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr), "");

    bcm_l2_addr_t_init(l2_addr, ipv6mc_l2_address, vid_in2);
    l2_addr.flags = BCM_L2_MCAST | BCM_L2_STATIC;
    l2_addr.l2mc_group = mc_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr), "");

    return BCM_E_NONE;
}

/*
 * The cint configuration prepares the needed entries to successfully forward over ETH using a MC group.
 * The MC group is deliberately left empty in order to validate different order and type of MC replications.
 *
 * Configuration:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/multicast/cint_multicast_pp_basic.c
  cint
  int unit = 0;
  int rv = 0;
  int in_port = 200;
  int out_port1 = 201;
  int out_port2 = 202;
  int out_port3 = 203;
  int mc_id = 1000;
  rv = int cint_multicast_ingress_protection_switch_l2_fwd(unit,in_port,out_port1,out_port2,out_port3,mc_id);
  print rv;
 * Test Scenario - end
 * 
 */
int cint_multicast_ingress_protection_switch_l2_fwd(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int mc_id)
{
    bcm_mac_t my_mac_address = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};     /* my-MAC */
    int vid_in = 15;
    uint32 pruning_flags = BCM_MULTICAST_REP_PRUNING | BCM_MULTICAST_REP_SAME_INTERFACE_EN;
    uint32 set_flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
    BCM_IF_ERROR_RETURN_MSG(cint_multicast_pruning_protection_test_internal(unit, in_port, out_port1, out_port2, out_port3, mc_id), "");

    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(l2_addr, my_mac_address, vid_in);
    l2_addr.flags = BCM_L2_MCAST | BCM_L2_STATIC;
    l2_addr.l2mc_group = mc_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr), "");
    return BCM_E_NONE;
}


/*
 * The cint configuration prepares the needed entries to successfully forward over ETH using a MC group.
 * The MC group is deliberately left empty in order to validate different order and type of MC replications.
 * Forwarding configuration for Bridge IPMC - v4MC/v6MC and v4Bridge/v6Bridge and route IPMC is created in order to validate the protection switching in all cases.
 *
 * Configuration:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/multicast/cint_multicast_pp_basic.c
  cint
  int unit = 0;
  int rv = 0;
  int in_port = 200;
  int out_port1 = 201;
  int out_port2 = 202;
  int out_port3 = 203;
  int mc_id = 1000;
  rv = cint_multicast_ingress_protection_switch_ipmc_fwd(unit,in_port,out_port1,out_port2,out_port3,mc_id);
  print rv;
 * Test Scenario - end
 * 
 */
int cint_multicast_ingress_protection_switch_ipmc_fwd(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int mc_id)
{
    int vid_in1 = 15;
    int vid_in2 = 16;
    int vid_in3 = 17;
    int vid_out1 = 0xFF;
    int vid_out2 = 0xFE;
    uint32 route = 0xE0020202;
    uint32 sip = 0xc0800101;
    uint32 mask = 0xffffffff;
    bcm_ip6_t dip_v6_addr = {0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x2, 0x2, 0x2};
    bcm_ip6_t v6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t sip_v6_addr = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    bcm_mac_t my_mac_address = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};     /* ing my-MAC */
    bcm_mac_t egr_my_mac_address1 = {0x00, 0x0c, 0x00, 0x00, 0xab, 0xcd};     /* egr my-MAC */
    bcm_mac_t egr_my_mac_address2 = {0x00, 0x0c, 0x00, 0x00, 0xab, 0xce};     /* egr my-MAC */
    bcm_mac_t next_hop_mac = {0x00, 0x00, 0x00, 0x00, 0x1d, 0xcd};     /* next hop MAC addr */
    bcm_mac_t ipv4mc_l2_address = {0x01, 0x00, 0x5E, 0x02, 0x02, 0x02};
    bcm_mac_t ipv6mc_l2_address = {0x33, 0x33, 0x00, 0x02, 0x02, 0x02};

    int vrf = 1;
    int vrf2 = 2;

    BCM_IF_ERROR_RETURN_MSG(cint_multicast_pruning_protection_test_internal(unit, in_port, out_port1, out_port2, out_port3, mc_id), "");
    /*
     * Set In-Port to In ETh-RIF
     */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = out_port1;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = vid_in2;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vid_in2, out_port1, 0), "");
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, out_port2, vid_in3), "intf_in");

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vid_in2), "intf_out");

    /** Create ETH-RIF and set its properties */
    /*
     * Set Incoming ETH-RIF properties
     * VID1 will indicate v4MC and v6MC forwarding type.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_create(unit, vid_in1, my_mac_address, vrf, BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST | BCM_L3_INGRESS_L3_MCAST_L2 | BCM_L3_INGRESS_IP6_L3_MCAST_L2), "");
    /** VID2 will indicate v4Bridge and v6Bridge forwarding type */
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_create(unit, vid_in2, my_mac_address, vrf, BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST), "");
    /** VID3 will indicate L3 forwarding. */
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_create(unit, vid_in3, my_mac_address, vrf2, 0), "");

    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, vid_out1, egr_my_mac_address1), "intf_in");
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, vid_out2, egr_my_mac_address2), "intf_in");

    /** Add V4Mc and v6MC fowarding entries. */
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc_bridge(unit, route, mask, sip, mask, vid_in1, 0, mc_id, 0, 0), "");
    
    BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc_bridge(unit, dip_v6_addr, v6_mask, sip_v6_addr, v6_mask, 0, vid_in1, 0x0CEE, mc_id, 0, 0), "");

    /** Add L2 MC forwarding entries. */
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(l2_addr, ipv4mc_l2_address, vid_in2);
    l2_addr.flags = BCM_L2_MCAST | BCM_L2_STATIC;
    l2_addr.l2mc_group = mc_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr), "");

    bcm_l2_addr_t_init(l2_addr, ipv6mc_l2_address, vid_in2);
    l2_addr.flags = BCM_L2_MCAST | BCM_L2_STATIC;
    l2_addr.l2mc_group = mc_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr), "");

    /** Add IPMC fowarding entries. */
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, route, mask, sip, mask, vid_in3, vrf2, mc_id, 0, 0), "");
    
    BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, dip_v6_addr, v6_mask, sip_v6_addr, v6_mask, vid_in3, vrf2, 0x0CEE, 0xFFFF, mc_id, 0, 0, 0), "");
    return BCM_E_NONE;
}

/**
 * An internal function to add the common configuration
 * that is needed to validate Ingress Mc pruning when forwarding over L2 and IPMC
 */
int cint_multicast_pruning_protection_test_internal(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int mc_id)
{
    char error_msg[200]={0,};
    int idx;
    int vid_in = 15;           /* Incoming packet ETH-RIF */
    int vid_out = 0xFF;        /* Outgoing packet ETH-RIF */
    int nof_out_ports = 3;
    int out_ports[3] = {out_port1, out_port2, out_port3};
    int ingr_orientation = 1;
    int egr_orientation = 3;

    uint32 create_flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC | BCM_MULTICAST_WITH_ID;

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = vid_in;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.ingress_network_group_id = ingr_orientation;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vid_in, in_port, 0), "");

    uint32 config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
    bcm_switch_network_group_config_t config_set;
    bcm_switch_network_group_config_t_init(&config_set);
    config_set.dest_network_group_id = egr_orientation;
    config_set.config_flags = config_flags;
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit, ingr_orientation, &config_set), "");
    bcm_switch_network_group_config_t_init(&config_set);
    config_set.dest_network_group_id = 1;
    config_set.config_flags = config_flags;
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit, ingr_orientation, &config_set), "");

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    for (idx = 0; idx < nof_out_ports; idx++)
    {
        snprintf(error_msg, sizeof(error_msg), "out_port[%d] = %d", idx, out_ports[idx]);
        BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_ports[idx]), error_msg);
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vid_in), "intf_out");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vid_out), "intf_out");
    /** 4. Configure a MC group without replications.*/
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, create_flags, mc_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_failover_create(unit, BCM_FAILOVER_INGRESS_MC | BCM_FAILOVER_WITH_ID, &failover_id), "");

    return BCM_E_NONE;
}

/*
 * The cint configuration prepares the needed entries to successfully forward over ETH using a MC group with pruning and protection.
 * The MC group is deliberately left empty in order to validate different order and type of MC replications.
 *
 * Configuration:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/multicast/cint_multicast_pp_basic.c
  cint
  int unit = 0;
  int rv = 0;
  int in_port = 200;
  int out_port1 = 201;
  int out_port2 = 202;
  int out_port3 = 203;
  int mc_id = 1000;
  rv = int cint_multicast_ingress_protection_switch_l2_fwd(unit,in_port,out_port1,out_port2,out_port3,mc_id);
  print rv;
 * Test Scenario - end
 * 
 */
int cint_multicast_ingress_pruning_protection_mix_l2_fwd(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int mc_id)
{
    bcm_mac_t my_mac_address = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};     /* my-MAC */
    int vid_in = 15;
    uint32 pruning_flags_same_itf = BCM_MULTICAST_REP_PRUNING | BCM_MULTICAST_REP_SAME_INTERFACE_EN;
    uint32 set_flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
    uint32 protection_flags = BCM_MULTICAST_REP_EITHER;
    uint32 split_horizon_flags = BCM_MULTICAST_REP_PRUNING | BCM_MULTICAST_REP_NETWORK_GROUP_EN;
    int egr_orientation = 3;
    BCM_IF_ERROR_RETURN_MSG(cint_multicast_pruning_protection_test_internal(unit, in_port, out_port1, out_port2, out_port3, mc_id), "");
    /** Add entries to the MC group */
    /** Replication with protection */
    bcm_multicast_replication_t replications[3];
    bcm_multicast_replication_t_init(&replications[0]);
    replications[0].flags |= protection_flags;
    replications[0].port = out_port1;
    replications[0].port2 = out_port2;
    replications[0].failover_id = failover_id;

    /** Replication with same interface filter enabled */
    bcm_multicast_replication_t_init(&replications[1]);
    replications[1].same_if_port = in_port;
    replications[1].same_if_encap_id = 0;
    replications[1].flags |= pruning_flags_same_itf;
    replications[1].port = out_port3;
    replications[1].encap1 = 0;

    /** Replication with split horizon and protection */
    bcm_multicast_replication_t_init(&replications[2]);
    replications[2].network_group_id = egr_orientation;
    replications[2].flags |= split_horizon_flags | protection_flags;
    replications[2].port = out_port1;
    replications[2].port2 = out_port3;
    replications[2].failover_id = failover_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, mc_id, set_flags, 3, replications), "");

    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(l2_addr, my_mac_address, vid_in);
    l2_addr.flags = BCM_L2_MCAST | BCM_L2_STATIC;
    l2_addr.l2mc_group = mc_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr), "");
    return BCM_E_NONE;
}


/*
 * The cint configuration prepares the needed entries to successfully terminate and encapsulate MPLS when using a MC group.
 * The MC group is deliberately left empty in order to validate different order and type of MC replications.
 * The configuration includes preparation for split horizon filtering when ingress orientation is 2 and egress is 3.
 *
 * Configuration:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_l3.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan_translate.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utility_mpls.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utility_vpls_advanced.c
  cint ../../../../src/examples/dnx/multicast/cint_multicast_pp_basic.c
 * 
  cint
  int unit = 0;
  int rv = 0;
  int in_port = 200;
  int out_port1 = 201;
  int out_port2 = 202;
  int out_port3 = 203;
  int mc_id = 1000;
  rv = cint_multicast_ingress_pruning_mpls(unit,in_port,out_port1,out_port2,out_port3,mc_id);
  print rv;
 * Test Scenario - end
 * 
 */
int cint_multicast_ingress_pruning_mpls(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int mc_id)
{
    bcm_mac_t my_mac_address = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};     /* my-MAC */
    int vid_in = 15;
    char error_msg[200]={0,};
    int rv = BCM_E_NONE;
    int ac_outer_vlan_1 = 100;
    int ac_inner_vlan_1 = 200;
    int vpn = vid_in;
    int ac_vid = 200;
    int pwe_vid = 30;
    bcm_gport_t gport_dest;
    int egr_orientation = 3;
    int ingr_orientation = 1;
    mpls_util_mpls_tunnel_initiator_create_s mpls_encap_tunnel1[1];
    mpls_util_mpls_tunnel_initiator_create_s mpls_encap_tunnel2[1];

    {
        bcm_vlan_port_t vlan_port;
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.port = in_port;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
        vlan_port.vsi = pwe_vid;
        vlan_port.flags = 0;
        vlan_port.ingress_network_group_id = ingr_orientation;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, pwe_vid, in_port, 0), "");

        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.port = out_port1;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
        vlan_port.vsi = ac_vid;
        vlan_port.flags = 0;
        vlan_port.ingress_network_group_id = ingr_orientation;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, ac_vid, out_port1, 0), "");
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, 5, out_port1, 0), "");
        uint32 config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
        bcm_switch_network_group_config_t config_set;
        bcm_switch_network_group_config_t_init(&config_set);
        config_set.dest_network_group_id = egr_orientation;
        config_set.config_flags = config_flags;
        BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit, ingr_orientation, &config_set), "");
        /** 4. Configure a MC group without replications.*/
        uint32 create_flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC | BCM_MULTICAST_WITH_ID;
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, create_flags, mc_id), "");
        BCM_IF_ERROR_RETURN_MSG(bcm_failover_create(unit, BCM_FAILOVER_INGRESS_MC | BCM_FAILOVER_WITH_ID, &failover_id), "");
    }
    /** VPLS configuration */
    {
        int i;
        int vpls_idx;
        bcm_gport_t vlan_default;
        bcm_vlan_control_vlan_t vlan_control;
        BCM_IF_ERROR_RETURN_MSG(init_default_vpls_util_advanced_params(unit, in_port, out_port1), "");

        /*
        * create vlan based on the vsi (vpn)
        */
        snprintf(error_msg, sizeof(error_msg), "Failed to create VLAN %d", vpn);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vpn), error_msg);

        for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
        {
            for(vpls_idx = 0; vpls_idx < VPLS_UTIL_ADVANCED_MAX_NOF_VPLS; vpls_idx++)
            {
                if(mpls_is_valid_lable(vpls_util_entity[i].vpls_ingress[vpls_idx].match_label))
                {
                    rv = bcm_vlan_control_vlan_get(unit, *vpls_util_entity[i].vpls_ingress[vpls_idx].forwarding_domain, &vlan_control);
                    if (rv == BCM_E_NONE)
                    {
                        continue;
                    }
                    /* create vlan based on the vsi (vpn) */
                    rv = bcm_vlan_create(unit, *vpls_util_entity[i].vpls_ingress[vpls_idx].forwarding_domain);
                    if (rv == BCM_E_EXISTS) {
                        rv = BCM_E_NONE;
                    }
                    snprintf(error_msg, sizeof(error_msg), "Failed to create VLAN %d",
                        *vpls_util_entity[i].vpls_ingress[vpls_idx].forwarding_domain);
                    BCM_IF_ERROR_RETURN_MSG(rv, error_msg);
                }
            }
        }

        /** mpls_util_main */
        {
            BCM_IF_ERROR_RETURN_MSG(init_default_mpls_params(unit, in_port, out_port1), "");
            BCM_IF_ERROR_RETURN_MSG(mpls_util_default_vlan(unit, &vlan_default), "");

            for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
            {
                /** Configure ILM POP entries */
                BCM_IF_ERROR_RETURN_MSG(mpls_create_switch_tunnels(unit, mpls_util_entity[i].mpls_switch_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS, 1), "POP");
            }
            /** Configure L3 interfaces */
            for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
            {
                BCM_IF_ERROR_RETURN_MSG(mpls_create_l3_interfaces(unit, mpls_util_entity[i].rifs, MPLS_UTIL_MAX_NOF_RIFS), "");
            }

            /** Configure ARP entries */
            for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
            {
                mpls_util_arp_s arp_struct = mpls_util_entity[i].arps[0];
                dnx_utils_l3_arp_s arp_entry;
                dnx_utils_l3_arp_s_common_init(unit, arp_struct.allocation_flags, &arp_entry, arp_struct.arp, arp_struct.flags, arp_struct.flags2, arp_struct.next_hop, *arp_struct.intf);
                arp_entry.flags2 |= BCM_L3_FLAGS2_SRC_MAC;
                arp_entry.src_mac_addr = my_mac_address;

                snprintf(error_msg, sizeof(error_msg), "for index %d", i);
                BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(unit, arp_entry), error_msg);
                mpls_util_entity[i].arps[0].arp = arp_entry.l3eg_arp_id;
            }
            /** Configure push or php entries */
            mpls_util_entity[1].mpls_encap_tunnel[0].label[0] = 4444;
            mpls_util_entity[1].mpls_encap_tunnel[0].num_labels = 1;
            mpls_util_entity[1].mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            mpls_util_entity[1].mpls_encap_tunnel[0].l3_intf_id = &mpls_util_entity[1].arps[0].arp;
            for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
            {
                BCM_IF_ERROR_RETURN_MSG(mpls_create_initiator_tunnels(unit, mpls_util_entity[i].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS), "");
            }
            pwe_mpls_tunnel_id1 = mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;
            pwe_mpls_tunnel_id2 = mpls_util_entity[1].mpls_encap_tunnel[0].tunnel_id;

            /** Configure ILM PUSH entries */
            for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
            {
                BCM_IF_ERROR_RETURN_MSG(mpls_create_switch_tunnels(unit, mpls_util_entity[i].mpls_switch_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS, 0), "PUSH");
            }
        }

        vpls_util_entity[0].vpls_ingress[0].pwe_nwk_group_id = ingr_orientation;
        vpls_util_entity[1].vpls_ingress[0].pwe_nwk_group_id = ingr_orientation;
        vpls_util_entity[0].vpls_ingress[0].nwk_group_valid = 1;
        vpls_util_entity[1].vpls_ingress[0].nwk_group_valid = 1;
        /** Configure egress VPLS entries */
        for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
        {
            BCM_IF_ERROR_RETURN_MSG(vpls_util_advanced_mpls_port_add_encapsulation(unit, vpls_util_entity[i].vpls_egress, VPLS_UTIL_ADVANCED_MAX_NOF_VPLS), "");
        }

        /** Configure ingress VPLS entries */
        for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
        {
            BCM_IF_ERROR_RETURN_MSG(vpls_util_advanced_mpls_port_add_termination(unit, vpls_util_entity[i].vpls_ingress, VPLS_UTIL_ADVANCED_MAX_NOF_VPLS), "");
        }

        /** Configure static MACs entries */
        BCM_GPORT_MCAST_SET(gport_dest, mc_id);
        vpls_util_entity[1].l2_macs[0].vid = 5;
        vpls_util_entity[0].l2_macs[0].vid = ac_vid;
        for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
        {
            bcm_l2_addr_t l2addr;

            if(vpls_util_entity[i].l2_macs[0].valid)
            {
                bcm_l2_addr_t_init(&l2addr, vpls_util_entity[i].l2_macs[0].mac_addr, vpls_util_entity[i].l2_macs[0].vid);
                l2addr.flags = vpls_util_entity[i].l2_macs[0].flags | BCM_L2_MCAST;
                l2addr.l2mc_group = mc_id;
                BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2addr), "");
            }
        }

        /** Configure TPID entries */
        for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
        {
            BCM_IF_ERROR_RETURN_MSG(vpls_util_advanced_tag_formats_set(unit, vpls_util_entity[i].tpids, VPLS_UTIL_ADVANCED_MAX_NOF_TPIDS), "");
        }

        /** Configure vlan editing entries */
        for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
        {
            BCM_IF_ERROR_RETURN_MSG(vpls_util_advanced_vlan_editing_config(unit, vpls_util_entity[i].vlan_edits, VPLS_UTIL_ADVANCED_MAX_NOF_VLAN_EDITS), "");
        }
    }
    return BCM_E_NONE;
}

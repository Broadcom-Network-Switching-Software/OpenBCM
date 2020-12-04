/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_dnx_evpn_etree.c
 * Purpose: Example of EVPN E-Tree configuration.
 */
/*
 * This cint demonstrate configuration of a PE in a EVPN network for E-Tree application.
 * It is similar to evpn basic application, but some special cnfigurations for E-Tree.
 * The configurations are based on the following topology.
 *
 *  EVPN core diagram:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                            .  .  . . .      . . .   . . .                                         |
 *  |                                          .            .  .       .       .                                        |
 *  |   Access                               .                                  .                            Access     |                       
 *  |                                      .                MPLS Core            . .                                    |
 *  |                                    .   /+-----+  -   -   -   -   -   -+-----+  .                                  |             
 *  |   +--------+                     .      | P1  | \                 /   | P3  | \                        +--------+ | 
 *  |  / Root   /|                    .   /   |     |                       |     | \  .                    / Root   /| | 
 *  | +--------+ |                   .        +-----+   \             /     +-----+   \  .                 +--------+ | |
 *  | |  CE1   | |\                 .  /       / |                             |      \   .               /|  CE2   | | |
 *  | |        |/  \    +--------+ .                      \         /                   \   .+--------+  / |        |/  |
 *  | +--------+\ esi  /        /| /        /    |                             |       \    /        /| esi+--------+   |
 *  |             200\+--------+ |                          \     /                       \+--------+ |/100/            |
 *  |             \   |  PE1   | |      /        |                             |        \  |   PE3  | |   /             |
 *  |             DF  |        |/ \                          \ /                           |        |/   DF             |
 *  |               \ +--------+     /           |                             |         \/+--------+\  /               |
 *  |                 /             \                        / \                                      \/                |
 *  |                             /              |                             |        / \           /\                |
 *  |               /  \+--------+    \                    /     \                         +--------+/  \               |
 *  |             DF   /        /|\              |                             |      /   /        /|    DF             |
 *  |             /   +--------+ |      \                /         \                     +--------+ |\    \             |
 *  |   +--------+ esi|  PE2   | |. \            |                             |    /    |   PE4  | | \ esi\ +--------+ |
 *  |  / leaf   /| 300|        |/  .      \            /             \                   |        |/   \400 / Leaf   /| |
 *  | +--------+ |  / +--------+    . \          |                             |  /     /+--------+     \  +--------+ | |
 *  | |  CE3   | | /                 .      \+-----+  /                 \   +-----+       .              \ |  CE4   | | |
 *  | |        |/                      .\    | P2  |                        | P4  |   / .                 \|        |/  |
 *  | +--------+ /                      . \  |     |/                     \ |     |    .                   +--------+   |
 *  |                                   .   \+-----+  -   -   -   -   -  -  +-----+ / .                                 |
 *  |                                     .                                   . .  .                                    |
 *  |                                      .               . .       .      .                                           |
 *  |                                        .  .   .  .  .   .  .  . . . .                                             |
 *  |                                                                                                                   |
 *  |                                                                                                                   |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * Test Scenario
 *
 * cint src/examples/sand/utility/cint_sand_utils_global.c
 * cint src/examples/sand/utility/cint_sand_utils_global.c
 * cint src/examples/sand/utility/cint_sand_utils_l3.c
 * cint src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint src/examples/dnx/field/cint_field_evpn_esi.c
 * cint src/examples/dnx/evpn/cint_dnx_evpn_basic.c
 * cint src/examples/dnx/evpn/cint_dnx_evpn_etree.c
 * cint
 * evpn_etree_main(0,200,201,202,203);
 * exit;
 *
 *
 * Traffic for the tests:
 * - UC traffic from CE1 to CE2 via PE4, passed with EVPN encapsulation. SA learned without Leaf indication.
 * - UC traffic from CE2 to CE1 via PE2, passed with EVPN termination.
 * - UC traffic from CE3 to CE4 via PE3, droped due to Leaf filter(PMF). SA learned with Leaf indication.
 * - PE1 BUM traffic from CE1, 4 copies:
 *   -- One copy to CE3 (ETH), droped due to orientation filter;
 *   -- one copy to PE2 (IML with ESI)
 *   -- two copies to PE3/PE4 (IML)
 * - PE1 BUM traffic from P2 (without Leaf/ESI label), 4 copies
 *   -- One copy to CE1, passed
 *   -- One copy to CE3, passed
 *   -- Two copies back to core dropped due to orientation.
 * - PE1 BUM traffic from P2 (with ESI label), 4 copies
 *   -- One copy to CE1, droped due to ESI filter.
 *   -- One copy to CE3, passed.
 *   -- Two copies back to core dropped due to orientation.
 * - PE1 BUM traffic from CE3 (ETH), 4 copies
 *   -- One copy to CE1 (ETH), passed.
 *      (The copy should not exist actually since PE1 is not the DF of CE1. We set it for observing Leaf filter.)
 *   -- Three copies to PE2/PE3/PE4 (IML with Leaf).
 * - PE1 BUM traffic from P2 (with Leaf label), 4 copies
 *   -- One copy to CE1 (ETH), Passed.
 *      (The copy should not exist actually since PE1 is not the DF of CE1. We set it for observing Leaf filter.)
 *   -- One copy to CE3 (ETH), droped due to Leaf filter(orientation).
 *   -- Two copies back to core dropped due to orientation.
 */
struct evpn_etree_additional_info_s
{
    int etree_flush_group;   /** Make sense for etree only. Suppose the MSB is used.*/
    bcm_mac_t ce4_mac;      /* ce4's mac address*/
    int pe1_pe2_ce3_leaf_label; /* The Leaf label of pe1, pe2, ce3 */
    int pe1_pe3_ce3_leaf_label; /* The Leaf label of pe1, pe3, ce3 */
    int pe1_pe4_ce3_leaf_label; /* The Leaf label of pe1, pe4, ce3 */
};

evpn_etree_additional_info_s evpn_etree_info = 
{
    /** etree_flush_group  | ce4_mac  */
    1 << 3,                 {0x00, 0x00, 0x00, 0x00, 0x04, 0x13},
    /** pe1_pe2_ce3_leaf | pe1_pe3_ce3_leaf | pe1_pe4_ce3_leaf  */
    301,                   302,               303,
};

/*
 * Main function to configure the scenario described above and in cint_dnx_evpn_basic.c
 */
int
evpn_etree_main(
    int unit,
    int ce1_port,
    int ce3_port,
    int p1_port,
    int p2_port)
{
    int rv = BCM_E_NONE;
    int vswitch_ports[4];

    printf("~~ EVPN E-Tree Basic Example ~~\n");
    is_evpn_etree = TRUE;

    printf("Load application profiles and misc configs\n");
    rv = evpn_profiles_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_profiles_set\n");
        return rv;
    }

    printf("Create VPN (%d)\n", cint_evpn_info.vsi);
    rv = evpn_vswitch_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_vpn_create\n");
        return rv;
    }

    vswitch_ports[0] = ce1_port;
    vswitch_ports[1] = ce3_port;
    vswitch_ports[2] = p1_port;
    vswitch_ports[3] = p2_port;
    rv = evpn_vswitch_add_ports(unit, 4, vswitch_ports);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_vswitch_add_ports");
        return rv;
    }

    printf("Init ESI filter FP application\n");
    rv = evpn_esi_filter_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_esi_filter_init");
        return rv;
    }

    printf("Init Etree filter FP application\n");
    rv = evpn_etree_filter_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_etree_filter_init");
        return rv;
    }

    printf("Create ETH segments\n");
    rv = evpn_eth_segments_create(unit, ce1_port, ce3_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_eth_segment_create\n");
        return rv;
    }

    printf("Setup MPLS L3 network\n");
    rv = evpn_mpls_core_setup(unit, p1_port, p2_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_core_setup\n");
        return rv;
    }

    printf("Setup EVPN Services\n");
    rv = evpn_service_egress_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_egress_tunnels_config\n");
        return rv;
    }
    printf("Setup EVPN Etree leaf encap\n");
    rv = evpn_etree_leaf_encap_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_etree_leaf_encap_config\n");
        return rv;
    }
    rv = evpn_service_ingress_config(unit, cint_evpn_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_ingress_tunnels_config\n");
        return rv;
    }

    printf("Connect ES to egress in UC flow\n");
    rv = evpn_egress_uc_l2_address_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_egress_uc_l2_address_add\n");
        return rv;
    }

    return rv;
}

/*
 * Setup Field Processor based Etree filter.
 * in UC, the filter for leaf -> leaf is done in ingress PE with iPMF.
 *     Suggested preselectors: in_lif_profile, fwd_layer_index, etc.
 *     Suggested qualifiers:
 *        inAC.leaf_indication, UDQ based on bcmFieldQualifyAcInLifWideData
 *        MACT.leaf_indication, UDQ based on bcmFieldQualifyDstClassL2
 *     actions: drop if both are set.
 * in BUM, the filter for leaf -> leaf is done in egress PE with iPMF and ePMF.
 *   The recommended solution is as below:
 *     iPMF (Leaf DB) --
 *     Suggested preselectors: in_lif_profile, fwd_layer_index, fwd_layer_type, etc.
 *     Suggested qualifiers: MPLS.LABEL (the label after IML).
 *     actions: drop(if no match), bcmFieldActionInVportClass0(update in_lif_profile0 with nwk_group_id).
 *     Thus, the traffic is filtered by split-horizon.
 *   User can also use other solution if according to his applications, such as use the same
 *   Filter meachnism and DBs as ESI as below:
 *     iPMF -- Same as ESI.
 *     ePMF -- Leaf entries should be added as below:
 *        Leaf_lable + out_port -> pass for port to root site.       
 *
 *   Here to simplify the example we use the latter solution.
 */
int
evpn_etree_filter_init(
    int unit)
{
    int rv = BCM_E_NONE;

    /** We use the same in-lif-profile as ESI since the fwd_layer_index is different*/
    rv = cint_field_evpn_etree_ingress(unit, evpn_global_info.in_lif_acl_profile_esi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, cint_field_evpn_etree_ingress\n");
        return rv;
    }

    /** Add leaf -> root entries for traffic*/
    rv = cint_field_evpn_etree_egress_leaf_port_entry_add(unit, evpn_etree_info.pe1_pe2_ce3_leaf_label, cint_evpn_info.ce1_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, cint_field_evpn_etree_egress_leaf_port_entry_add\n");
        return rv;
    }

    rv = cint_field_evpn_etree_egress_leaf_port_entry_add(unit, evpn_etree_info.pe1_pe3_ce3_leaf_label, cint_evpn_info.ce1_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, cint_field_evpn_etree_egress_leaf_port_entry_add\n");
        return rv;
    }

    rv = cint_field_evpn_etree_egress_leaf_port_entry_add(unit, evpn_etree_info.pe1_pe4_ce3_leaf_label, cint_evpn_info.ce1_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, cint_field_evpn_etree_egress_leaf_port_entry_add\n");
        return rv;
    }

    return rv;
}

/*
 * Configure EVPN Etree Leaf encapsulation for traffic from PE1 to other PEs
 */
int
evpn_etree_leaf_encap_config(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_esi_info_t pe1_pe2_esi_200_encap;
    bcm_mpls_esi_info_t pe1_pe3_esi_400_encap;
    bcm_mpls_esi_info_t pe1_pe4_esi_100_encap;


    printf("Add ESI encapsulation for IML traffic from ESI-300 to PE2\n");
    bcm_mpls_esi_info_t_init(&pe1_pe2_esi_200_encap);
    pe1_pe2_esi_200_encap.esi_label = evpn_etree_info.pe1_pe2_ce3_leaf_label;
    pe1_pe2_esi_200_encap.out_class_id = evpn_global_info.dual_homed_peer_profile;
    pe1_pe2_esi_200_encap.src_port = evpn_global_info.ce3_vlan_port.port;
    rv = bcm_mpls_esi_encap_add(unit, &pe1_pe2_esi_200_encap);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_esi_encap_add esi-300\n");
        return rv;
    }

    printf("Add ESI encapsulation for IML traffic from ESI-300 to PE3\n");
    bcm_mpls_esi_info_t_init(&pe1_pe3_esi_400_encap);
    pe1_pe3_esi_400_encap.esi_label = evpn_etree_info.pe1_pe3_ce3_leaf_label;
    pe1_pe3_esi_400_encap.out_class_id = 0;
    pe1_pe3_esi_400_encap.src_port = evpn_global_info.ce3_vlan_port.port;
    rv = bcm_mpls_esi_encap_add(unit, &pe1_pe3_esi_400_encap);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_esi_encap_add esi-400\n");
        return rv;
    }

    printf("Add ESI encapsulation for IML traffic from ESI-300 to PE4\n");
    bcm_mpls_esi_info_t_init(&pe1_pe4_esi_100_encap);
    pe1_pe4_esi_100_encap.esi_label = evpn_etree_info.pe1_pe4_ce3_leaf_label;
    pe1_pe4_esi_100_encap.out_class_id = evpn_global_info.dual_homed_peer_profile - 1;
    pe1_pe4_esi_100_encap.src_port = evpn_global_info.ce3_vlan_port.port;
    rv = bcm_mpls_esi_encap_add(unit, &pe1_pe4_esi_100_encap);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_esi_encap_add esi-100\n");
        return rv;
    }

    return rv;
}



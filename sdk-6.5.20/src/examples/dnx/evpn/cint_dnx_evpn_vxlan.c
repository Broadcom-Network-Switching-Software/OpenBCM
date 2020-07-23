/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_dnx_evpn_vxlan.c
 * Purpose: Example of EVPN basic configuration with VXLAN links as ETH segments.
 */
/*
 * This cint demonstrate configuration of a PE in a EVPN network, where the CEs
 * are connected to the EVPN core using VXLAN.
 * The following configuration is simulated:
 * +--------------------------------------------+--------------------------------------------------------+
 * |                                            |                                                        |
 * |                             VXLAN Network  |  EVPN Core                                             |
 * |                                            |                                                        |
 * |     VXLAN TOR1                             |                           EVPN Peer1                   |
 * |     RIF 10                                 |                           RIF 30                       |
 * |     MAC: 00::11                            |                           MAC: 00::01:33               |
 * |     IP: 171.0.0.1                          |                           EVPN: 0x510                  |
 * |     VNI: 0x50                  +-----------+------------+              IML: 0x610, Peer-ID: 0x30    |
 * |     VSI: 0x20                  |                        |              LSP: 0x5000                  |
 * |     NWK Group: 1               |                        |              NWK Group: 2                 |
 * |     <-------------------------->                        <--------------------------->               |
 * |     ESI: 0x111                 |                        |                                           |
 * |                                |                        |                                           |
 * |                                |      Provider Edge     |                                           |
 * |                                |         (PE1)          |                                           |
 * |                                |                        |                                           |
 * |     ESI: 0x222                 |                        |                                           |
 * |     <-------------------------->                        <--------------------------->               |
 * |     VXLAN TOR2                 |                        |              EVPN Peer2                   |
 * |     RIF 20                     |                        |              RIF 40                       |
 * |     MAC: 00::22                +------------+-----------+              MAC: 00::01:44               |
 * |     IP: 171.0.0.2                           |                          EVPN: 0x520                  |
 * |     VNI: 0x50                               |                          IML: 0x620, Peer-ID: 0x40    |
 * |     VSI: 0x20                               |                          LSP: 0x6000                  |
 * |     NWK Group: 1                            |                          NWK Group: 2                 |
 * |                                             |                                                       |
 * +---------------------------------------------+-------------------------------------------------------+
 *
 * VSI's MACT will include:
 * 00:01:00:00:01:11 -> EVPN P1
 * 00:01:00:00:02:22 -> VXLAN TOR2
 *
 * EVPN Peer 1 is dual-homed to ESI 0x111
 * EVPN Peer 2 is dual-homed to ESI 0x222
 *
 * Split horizon is configured to filter traffic for NWK group back to itself, both for NWK group 1 and 2.
 *
 * Test Scenario
 *
 * cint src/examples/sand/utility/cint_sand_utils_global.c
 * cint src/examples/sand/utility/cint_sand_utils_global.c
 * cint src/examples/sand/utility/cint_sand_utils_l3.c
 * cint src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint src/examples/sand/utility/cint_sand_utils_vxlan.c
 * cint src/examples/dnx/utility/cint_dnx_utils_evpn.c
 * cint src/examples/dnx/field/cint_field_evpn_esi.c
 * cint src/examples/dnx/evpn/cint_dnx_evpn_vxlan.c
 * cint
 * evpn_vxlan_main(0,200,201,202,203);
 * exit;
 *
 * UC EVPN -> VXLAN
 * tx 1 psrc=202 data=0x0000000001330000000002228100001e8847050000400051014000010000012200000000025581000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x000000000556000000000022810040140800450a00570000000040112388ab000002ab0001025000555500430000080000000000500000010000012200000000025581000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 * EVPN -> VXLAN flooding
 * tx 1 psrc=202 data=0x0000000001330000000002228100001e8847050000400061014000010203040500000000025581000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000000005550000000000118100400a0800450a0057000000004011238aab000001ab0001015000555500430000080000000000500000010203040500000000025581000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Data: 0x000000000556000000000022810040140800450a00570000000040112388ab000002ab0001025000555500430000080000000000500000010203040500000000025581000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 * EVPN -> VXLAN flooding with ESI filter
 * tx 1 psrc=203 data=0x00000000014400000000022381000028884706000040006200400022214000010203040500000000022381000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000000005550000000000118100400a0800450a0057000000004011238aab000001ab0001015000555500430000080000000000500000010203040500000000022381000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 * UC EVPN -> VXLAN
 * tx 1 psrc=200 data=0x0000000000110000000005558100000a080045000057000000008011e393ab000101ab000001500012b500430000080000000000500000010000011100000000055581000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x000000000222000000000133810000208847050000fe005101fe00010000011100000000055581000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 * VXLAN -> EVPN Flooding
 * tx 1 psrc=200 data=0x0000000000110000000005558100000a080045000057000000008011e393ab000101ab000001500012b500430000080000000000500000000000025500000000055581000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x000000000222000000000133810000208847050000fe006100fe0011110100000000025500000000055581000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Data: 0x000000000223000000000144810000208847060000fe006201fe00000000025500000000055581000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */

struct evpn_vxlan_tor_info_s
{
    /***
     * Configuration
 */
    int local_port;
    int rif;
    int vrf;
    bcm_mac_t my_mac;
    bcm_mac_t tor_mac;
    uint32 my_ip;
    int tunnel_fec_id;
    /***
     * Created objects
 */
    bcm_tunnel_terminator_t tunnel_term;
    bcm_l3_egress_t arp;
    int arp_id;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t tunnel_init_intf;
    vxlan_port_add_s vxlan_port;
};

/*
 * Structure to hold core mpls interfaces for carrying traffic to a peer
 */
struct evpn_vxlan_core_intf_s
{
    int rif;
    bcm_mac_t next_hop_mac;
    bcm_mac_t rif_mac;
    bcm_mpls_label_t lsp_label;
};

/*
 * General structure to hold the objects generated to implement
 * the scenario described in cint_evpn.c.
 */
struct evpn_vxlan_info_s
{
    int vpn;
    int vni;
    evpn_vxlan_tor_info_s tor1;
    evpn_vxlan_tor_info_s tor2;
    evpn_util_peer_info_s peer1;
    evpn_vxlan_core_intf_s peer1_core_ifs;
    evpn_util_peer_info_s peer2;
    evpn_vxlan_core_intf_s peer2_core_ifs;

    /** Static addresses added to the MACT */
    bcm_mac_t static_mac1;
    bcm_mac_t static_mac2;

    /** User defined in-lif profile for ESI FP filter. */
    int in_lif_acl_profile_esi;

    /** EVPN Network group ID for split horizon */
    int evpn_nwk_grp_id;
    /** When network group is taken from ESEM, need default native AC with the correct value */
    bcm_vlan_port_t evpn_default_native_ac;

    /** VXLAN Network Group ID for split horizon */
    int vxlan_nwk_grp_id;
    /** When network group is taken from ESEM, need default native AC with the correct value */
    bcm_vlan_port_t vxlan_default_native_ac;

    /** IML label range properties */
    bcm_mpls_label_t iml_label_range_low;
    bcm_mpls_label_t iml_label_range_high;
};

evpn_vxlan_info_s evpn_vxlan_info;

int
evpn_vxlan_params_init(
    int unit)
{
    int rv = BCM_E_NONE;
    evpn_util_peer_info_s * p_info;
    evpn_vxlan_core_intf_s * p_ifs;
    evpn_vxlan_tor_info_s * p_tor;
    bcm_mpls_egress_label_t * eg_label;
    int fec_id;

    bcm_mac_t p1_rif_mac = {0x00, 0x00, 0x00, 0x00, 0x01, 0x33};
    bcm_mac_t p1_next_hop = {0x00, 0x00, 0x00, 0x00, 0x02, 0x22};
    bcm_mac_t p2_rif_mac = {0x00, 0x00, 0x00, 0x00, 0x01, 0x44};
    bcm_mac_t p2_next_hop = {0x00, 0x00, 0x00, 0x00, 0x02, 0x23};

    bcm_mac_t vxlan_rif_mac1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
    bcm_mac_t tor1_mac = {0x00, 0x00, 0x00, 0x00, 0x05, 0x55};
    bcm_mac_t vxlan_rif_mac2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
    bcm_mac_t tor2_mac = {0x00, 0x00, 0x00, 0x00, 0x05, 0x56};

    bcm_mac_t static_mac1 = {0x00, 0x01, 0x00, 0x00, 0x01, 0x11};
    bcm_mac_t static_mac2 = {0x00, 0x01, 0x00, 0x00, 0x01, 0x22};

    printf("Load global configuration\n");
    /*
     * By default, FP in-lif profile is 2 bits. The value for these bits is managed by the application, in
     * consideration of other ACL application that may use these bits.
     * Need to select a value between 0-3. 0 should be reserved for non-ESI profiles.
     * Selected 1 arbitrarily.
     */
    evpn_vxlan_info.in_lif_acl_profile_esi = 1;
    /*
     * Select network groups for EVPN and VXLAN, to configure split horizon to filter
     * traffic going back (for example through traffic) to the same network (EVPN to
     * EVPN, or VXLAN to VXLAN).
     */
    evpn_vxlan_info.vxlan_nwk_grp_id = 1;
    evpn_vxlan_info.evpn_nwk_grp_id = 2;

    evpn_vxlan_info.vpn = 0x20;
    evpn_vxlan_info.vni = 0x50;

    evpn_vxlan_info.iml_label_range_low = 0x600;
    evpn_vxlan_info.iml_label_range_high = 0x6ff;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    printf("Load TOR1 configuration\n");
    p_tor = &evpn_vxlan_info.tor1;
    p_tor->rif = 10;
    p_tor->vrf = 100;
    p_tor->my_mac = vxlan_rif_mac1;
    p_tor->tor_mac = tor1_mac;
    p_tor->my_ip = 0xAB000001;
    p_tor->tunnel_fec_id = fec_id;

    printf("Load TOR2 configuration\n");
    p_tor = &evpn_vxlan_info.tor2;
    p_tor->rif = 20;
    p_tor->vrf = 200;
    p_tor->my_mac = vxlan_rif_mac2;
    p_tor->tor_mac = tor2_mac;
    p_tor->my_ip = 0xAB000002;
    p_tor->tunnel_fec_id = fec_id + 2;

    printf("Load Peer1 configuration\n");
    p_info = &evpn_vxlan_info.peer1;
    p_info->vpn = evpn_vxlan_info.vpn;
    p_info->evpn_label = 0x510;
    p_info->iml_label = 0x610;
    p_info->iml_esi_acl_lif_profile = evpn_vxlan_info.in_lif_acl_profile_esi;
    p_info->iml_dual_homed_peer_profile = 0x30;
    p_info->nof_dual_homed_ces = 1;
    p_info->dual_homed_ce_info[0].esi_label = 0x111;
    p_info->dual_homed_ce_info[0].src_port = &evpn_vxlan_info.tor1.local_port;
    p_info->network_group_id = evpn_vxlan_info.evpn_nwk_grp_id;
    p_info->default_native_ac_for_split_horizon = &evpn_vxlan_info.evpn_default_native_ac;
    p_ifs = &evpn_vxlan_info.peer1_core_ifs;
    p_ifs->rif = 30;
    p_ifs->rif_mac = p1_rif_mac;
    p_ifs->next_hop_mac = p1_next_hop;
    p_ifs->lsp_label = 0x5000;


    printf("Load Peer2 configuration\n");
    p_info = &evpn_vxlan_info.peer2;
    p_info->vpn = evpn_vxlan_info.vpn;
    p_info->evpn_label = 0x520;
    p_info->iml_label = 0x620;
    p_info->iml_esi_acl_lif_profile = evpn_vxlan_info.in_lif_acl_profile_esi;
    p_info->iml_dual_homed_peer_profile = 0x40;
    p_info->nof_dual_homed_ces = 1;
    p_info->dual_homed_ce_info[0].esi_label = 0x222;
    p_info->dual_homed_ce_info[0].src_port = &evpn_vxlan_info.tor2.local_port;
    p_info->network_group_id = evpn_vxlan_info.evpn_nwk_grp_id;
    p_info->default_native_ac_for_split_horizon = &evpn_vxlan_info.evpn_default_native_ac;
    p_ifs = &evpn_vxlan_info.peer2_core_ifs;
    p_ifs->rif = 40;
    p_ifs->rif_mac = p2_rif_mac;
    p_ifs->next_hop_mac = p2_next_hop;
    p_ifs->lsp_label = 0x6000;

    printf("Load static addresses for MACT\n");
    evpn_vxlan_info.static_mac1 = static_mac1;
    evpn_vxlan_info.static_mac2 = static_mac2;

    return rv;
}


int
init_evpn_mpls_util_params(
    int unit)
{
    int rv = BCM_E_NONE;
    mpls_util_entity_s * entity;
    evpn_vxlan_core_intf_s * p_ifs;

    printf("Configuring Entity 0 - MPLS tunnels from PE1 to Peer1\n");
    p_ifs = &evpn_vxlan_info.peer1_core_ifs;
    entity = &mpls_util_entity[0];
    entity->arps[0].next_hop = p_ifs->next_hop_mac;
    entity->arps[0].flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
    entity->rifs[0].mac_addr = p_ifs->rif_mac;
    entity->rifs[0].intf = p_ifs->rif;
    printf("- Configure MPLS LSP encapsulation 0\n");
    entity->mpls_encap_tunnel[0].num_labels = 1;
    entity->mpls_encap_tunnel[0].label[0] = p_ifs->lsp_label;
    entity->mpls_encap_tunnel[0].encap_access = bcmEncapAccessTunnel2;
    entity->mpls_encap_tunnel[0].l3_intf_id = &entity->arps[0].arp;
    printf("  - Reconnect entity 0 to it's own out port\n");
    entity->fecs[0].port = &entity->ports[0].port;
    printf("- Configure MPLS LSP termination\n");
    entity->mpls_switch_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    entity->mpls_switch_tunnel[0].label = p_ifs->lsp_label;
    entity->mpls_switch_tunnel[0].vrf = 1;
    /* When JR2 works under IOP mode, use FEC format:dest+EEI */
    if ((soc_property_get(unit , "system_headers_mode",1) == 0)) {
        entity->fecs[0].flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }

    printf("Configure Entity 1 - MPLS tunnel from PE1 to PE2 through P2\n");
    p_ifs = &evpn_vxlan_info.peer2_core_ifs;
    entity = &mpls_util_entity[1];
    entity->arps[0].next_hop = p_ifs->next_hop_mac;
    entity->arps[0].flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
    entity->rifs[0].mac_addr = p_ifs->rif_mac;
    entity->rifs[0].intf = p_ifs->rif;
    printf("- Configure MPLS LSP encapsulation 0\n");
    entity->mpls_encap_tunnel[0].num_labels = 1;
    entity->mpls_encap_tunnel[0].label[0] = p_ifs->lsp_label;
    entity->mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    entity->mpls_encap_tunnel[0].encap_access = bcmEncapAccessTunnel2;
    entity->mpls_encap_tunnel[0].l3_intf_id = &entity->arps[0].arp;
    printf("  - Reconnect entity 1 to it's own out port\n");
    entity->fecs[0].port = &entity->ports[0].port;
    printf("- Reset entity 1 fec 0");
    entity->fecs[0].tunnel_gport = &entity->mpls_encap_tunnel[0].tunnel_id;
    entity->fecs[0].tunnel_gport2 = &MPLS_UTIL_INVALID_GPORT;
    printf("- Configure MPLS LSP termination\n");
    entity->mpls_switch_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    entity->mpls_switch_tunnel[0].label = p_ifs->lsp_label;
    entity->mpls_switch_tunnel[0].vrf = 2;

    return rv;
}

/*
 * Main function to configure the scenario described above and in cint_evpn.c
 * The function performs the following activities:
 * - Setup the VSI
 * - Create ETH segments - in this example - VXLAN links
 * - Configure a L3 MPLS core connecting the device to neighboring MPLS routers
 * - Setup EVPN services for L2 MC and UC over the MPLS core
 * - Add static entries in MACT to emulate BGP triggered address advertising.
 */
int
evpn_vxlan_main(
    int unit,
    int tor1_port,
    int tor2_port,
    int peer1_port,
    int peer2_port)
{
    int rv = BCM_E_NONE;

    printf("~~ EVPN Basic Example ~~\n");

    printf("Load application profiles and misc configs\n");
    rv = evpn_vxlan_params_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in init_evpn_vxlan_params\n");
        return rv;
    }

    printf("Create VPN (%d)\n", evpn_vxlan_info.vpn);
    rv = sand_utils_vxlan_open_vpn(unit, evpn_vxlan_info.vpn, 0, evpn_vxlan_info.vni);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in sand_utils_vxlan_open_vpn\n");
        return rv;
    }

    printf("Init ESI filter FP application\n");
    rv = evpn_util_esi_filter_init(unit, evpn_vxlan_info.in_lif_acl_profile_esi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_esi_filter_init");
        return rv;
    }

    printf("Init Split Horizon filter\n");
    rv = evpn_vxlan_split_horizon_filter_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_vxlan_split_horizon_filter_config\n");
        return rv;
    }

    printf("Create ETH segments\n");
    rv = evpn_vxlan_eth_segments_create(unit, tor1_port, tor2_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_eth_segment_create\n");
        return rv;
    }

    printf("Setup MPLS L3 network\n");
    rv = evpn_mpls_core_setup(unit, peer1_port, peer2_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_core_setup\n");
        return rv;
    }

    printf("Setup EVPN Services\n");
    rv = evpn_service_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_egress_tunnels_config\n");
        return rv;
    }

    printf("Create forwarding connections (MACT)\n");
    rv = evpn_vxlan_forwarding_connect(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_vxlan_forwarding_connect\n");
        return rv;
    }

    /* Configure ESI filter for IOP mode */
    if ((soc_property_get(unit , "system_headers_mode",1) == 0)) {
        /* Init forbidden port */
        field_evpn_info_init(unit, 0, tor2_port, peer2_port, 0, tor2_port, 0);

        cint_field_evpn_info.esi1_label[0] = evpn_vxlan_info.peer2.dual_homed_ce_info->esi_label >> 16;
        cint_field_evpn_info.esi1_label[1] = evpn_vxlan_info.peer2.dual_homed_ce_info->esi_label >> 8 & 0xFF;
        cint_field_evpn_info.esi1_label[2] = evpn_vxlan_info.peer2.dual_homed_ce_info->esi_label & 0xFF;

        /* Set IPMF to set forbidden port to learn_extension */
        rv = field_jr2_comp_evpn_esi_db(unit, jr2_ing_presel_id-1/*presel_id*/);
        if (BCM_E_NONE != rv) {
            printf("Error,unit: %d, in esi_db\n", unit);
            return rv;
        }

        /* Configure EPMF to drop when dst_port matches learn_extension */
        rv = field_jr2_comp_evpn_egress_filter_db(unit, jr2_egr_presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error,unit: %d,  in evpn_egress_filter_db\n", unit);
            return rv;
        }
    }

    return rv;
}

/*
 * Configure split horizon for EVPN to EVPN filtering
 * Configure split horizon for VXLAN to VXLAN filtering
 */
int
evpn_vxlan_split_horizon_filter_config(
    int unit)
{
    int rv = BCM_E_NONE;

    bcm_switch_network_group_config_t network_group_config;
    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id = evpn_vxlan_info.evpn_nwk_grp_id;
    network_group_config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
    rv = bcm_switch_network_group_config_set(unit, evpn_vxlan_info.evpn_nwk_grp_id, &network_group_config);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_switch_network_group_config_set EVPN group ID\n");
        return rv;
    }

    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id = evpn_vxlan_info.vxlan_nwk_grp_id;
    network_group_config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
    rv = bcm_switch_network_group_config_set(unit, evpn_vxlan_info.vxlan_nwk_grp_id, &network_group_config);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_switch_network_group_config_set VXLAN group ID\n");
        return rv;
    }

    return rv;
}

/*
 * Create TOR1 and TOR2 vxlan services encapsulation and termination.
 */
int
evpn_vxlan_eth_segments_create(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    int add_to_flooding_domain;

    printf("Create ES for TOR1\n");
    evpn_vxlan_info.tor1.local_port = port1;
    add_to_flooding_domain = 1;
    rv = evpn_vxlan_es_create(unit, &evpn_vxlan_info.tor1, add_to_flooding_domain);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_vxlan_es_create tor1\n");
        return rv;
    }
    rv = evpn_util_esi_filter_entry_add(unit, evpn_vxlan_info.peer1.dual_homed_ce_info[0].esi_label, &port1, 1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_util_esi_filter_entry_add tor1\n");
        return rv;
    }

    printf("Create ES for TOR2\n");
    evpn_vxlan_info.tor2.local_port = port2;
    add_to_flooding_domain = 1;
    rv = evpn_vxlan_es_create(unit, &evpn_vxlan_info.tor2, add_to_flooding_domain);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_vxlan_es_create tor2\n");
        return rv;
    }
    rv = evpn_util_esi_filter_entry_add(unit, evpn_vxlan_info.peer2.dual_homed_ce_info[0].esi_label, &port2, 1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_util_esi_filter_entry_add tor2\n");
        return rv;
    }

    return rv;
}

/*
 * Create a single VXLAN o UDP o IPv4-Tunnel according to the supplied info.
 */
int
evpn_vxlan_es_create(
    int unit,
    evpn_vxlan_tor_info_s * info,
    int add_to_flooding)
{
    int rv = BCM_E_NONE;
    l3_ingress_intf ingr_itf;

    printf("Set port default vlan domain\n");
    rv = bcm_port_class_set(unit, info->local_port, bcmPortClassId, info->local_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_port_class_set\n");
        return rv;
    }

    printf("Set Out-Port default properties\n");
    rv = out_port_set(unit, info->local_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in out_port_set\n");
        return rv;
    }

    printf("Set In-Port to In ETh-RIF, for incoming port, configure default VSI.\n");
    sand_utils_l3_port_s l3_port;
    sand_utils_l3_port_s_common_init(unit, 0, &l3_port, info->local_port, info->rif);
    rv = sand_utils_l3_port_set(unit, l3_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in in_port_intf_set\n");
        return rv;
    }

    /*** build tunnel terminations and router interfaces ***/
    printf("Setup RIF\n");
    sand_utils_l3_eth_rif_s l3_eth_rif;
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &l3_eth_rif, info->rif, 0, 0, info->my_mac, info->vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &l3_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in sand_utils_l3_eth_rif_create\n");
        return rv;
    }

    printf("Set Incoming ETH-RIF properties for the tunnel's DIP.\n");
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = info->rif;
    ingr_itf.vrf = info->vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in intf_ingress_rif_set\n");
        return rv;
    }

    printf("Create IP tunnel terminator for SIP,DIP, VRF lookup\n");
    bcm_tunnel_terminator_t_init(&info->tunnel_term);
    info->tunnel_term.dip      = info->my_ip;
    info->tunnel_term.dip_mask = 0xffffffff;
    info->tunnel_term.sip      = info->my_ip + 0x100;
    info->tunnel_term.sip_mask = 0xffffffff;
    info->tunnel_term.type     = bcmTunnelTypeVxlan;
    info->tunnel_term.vrf      = info->vrf;
    rv = bcm_tunnel_terminator_create(unit,&info->tunnel_term);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_tunnel_terminator_create\n");
        return rv;
    }

    printf("Create ARP\n");
    rv = l3__egress_only_encap__create(unit, BCM_L3_EGRESS_ONLY, &(info->arp_id), info->tor_mac, info->rif);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in l3__egress_only_encap__create\n");
        return rv;
    }

    printf("Create IP tunnel initiator\n");
    bcm_l3_intf_t_init(&info->tunnel_init_intf);
    bcm_tunnel_initiator_t_init(&info->tunnel_init);
    info->tunnel_init.dip = info->my_ip + 0x100;
    info->tunnel_init.sip = info->my_ip;
    info->tunnel_init.flags = 0;
    info->tunnel_init.dscp = 10;
    info->tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    info->tunnel_init.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    info->tunnel_init.type = bcmTunnelTypeVxlan;
    info->tunnel_init.l3_intf_id = info->arp_id;
    info->tunnel_init.ttl = 64;
    info->tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
    rv = bcm_tunnel_initiator_create(unit,&info->tunnel_init_intf, &info->tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_tunnel_initiator_create\n");
        return rv;
    }

    printf("Create FEC\n");
    rv = l3__egress_only_fec__create(unit, info->tunnel_fec_id,
                                     info->tunnel_init_intf.l3a_intf_id, 0, info->local_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in l3__egress_only_fec__create\n");
        return rv;
    }

    printf("Create VXLAN port\n");
    vxlan_port_s_clear(&info->vxlan_port);
    info->vxlan_port.vpn = evpn_vxlan_info.vpn;
    info->vxlan_port.in_port = info->local_port;
    info->vxlan_port.in_tunnel = info->tunnel_term.tunnel_id;
    info->vxlan_port.out_tunnel = info->tunnel_init.tunnel_id;
    BCM_L3_ITF_SET(info->vxlan_port.egress_if, BCM_L3_ITF_TYPE_FEC, info->tunnel_fec_id);
    info->vxlan_port.network_group_id = evpn_vxlan_info.vxlan_nwk_grp_id;
    info->vxlan_port.add_to_mc_group = add_to_flooding;
    rv = vxlan__vxlan_port_add(unit, &info->vxlan_port);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in vxlan__vxlan_port_add\n");
        return rv;
    }

    /**
    * JR2: outlif profile used for split horizon filter is taken from (L2-FODO) rather than IP-Tunnel. 
    * L2-FODO entry was created in vxlan VPN creation.
    */
    int nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);
    if (nwk_grp_from_esem) {
        bcm_vlan_port_t vlan_port;
        bcm_vlan_port_t_init(&vlan_port); 
        vlan_port.vsi = evpn_vxlan_info.vpn;   /* vsi, part of the key in ESEM lookup */
        vlan_port.match_class_id = 0;          /* network domain, part of the key in ESEM lookup */
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI; 
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION; 
        vlan_port.egress_network_group_id = evpn_vxlan_info.vxlan_nwk_grp_id; 
        printf("bcm_vlan_port_create with orientation: %d \n", vlan_port.egress_network_group_id);
        rv = bcm_vlan_port_create(unit, vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_create \n");
            return rv;
        }
    }

    return rv;
}

/*
 * Create EVPN L3 network to route packets from PE1 to P1, P2
 * (see cint_evpn.c for details)
 */
int
evpn_mpls_core_setup(
    int unit,
    int p1_port,
    int p2_port)
{
    int rv = BCM_E_NONE;

    printf("Initializing MPLS utility.\n");
    rv = init_default_mpls_params(unit, p1_port, p2_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in init_default_mpls_params\n");
        return rv;
    }

    printf("Configure example specific parameters to MPLS utility.\n");
    rv = init_evpn_mpls_util_params(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_utility_re_init\n");
        return rv;
    }

    printf("Run MPLS utility\n");
    rv = mpls_util_main(unit, 0/*ignored*/, 0/*ignored*/);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in mpls_util_main\n");
        return rv;
    }

    printf("Add vlan translation to ARP+VLAN translation entries\n");
    rv = evpn_mpls_arp_plus_ac_configure(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_arp_plus_ac_configure\n");
        return rv;
    }

    return rv;
}

/*
 * Set vlan translation in ARP+AC entries, used for MPLS tunnels
 * with ESI DB lookup trigger.
 */
int
evpn_mpls_arp_plus_ac_configure(
    int unit)
{
    int rv = BCM_E_NONE;

    int encap_id;
    int arp_outlif;
    bcm_gport_t arp_gport;

    encap_id = mpls_util_entity[0].arps[0].arp;
    arp_outlif = BCM_L3_ITF_VAL_GET(encap_id);
    BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(arp_gport, arp_outlif);
    BCM_GPORT_VLAN_PORT_ID_SET(arp_gport, arp_gport);
    rv = vlan_translate_ive_eve_translation_set(unit, arp_gport,
                                                0x8100,                           0,
                                                bcmVlanActionArpVlanTranslateAdd, bcmVlanActionNone,
                                                evpn_vxlan_info.vpn,              0,
                                                3, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in vlan_translate_ive_eve_translation_set\n");
        return rv;
    }

    encap_id = mpls_util_entity[1].arps[0].arp;
    arp_outlif = BCM_L3_ITF_VAL_GET(encap_id);
    BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(arp_gport, arp_outlif);
    BCM_GPORT_VLAN_PORT_ID_SET(arp_gport, arp_gport);
    rv = vlan_translate_ive_eve_translation_set(unit, arp_gport,
                                                0x8100,                           0,
                                                bcmVlanActionArpVlanTranslateAdd, bcmVlanActionNone,
                                                evpn_vxlan_info.vpn,              0,
                                                4, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in vlan_translate_ive_eve_translation_set\n");
        return rv;
    }

    return rv;
}

/*
 * Configure EVPN services
 */
int
evpn_service_config(
    int unit)
{
    int rv = BCM_E_NONE;

    bcm_mpls_esi_info_t esi_encap;
    bcm_mpls_range_action_t action;

    int nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);
    if (nwk_grp_from_esem)
    {
        bcm_vlan_port_t * default_native_ac = &evpn_vxlan_info.evpn_default_native_ac;
        /*
         * Default egress native AC with network group for the IMLs (used for
         * setting up split horizon)
         */
        printf("Create egress default native AC with EVPN network group ID for split horizon\n");
        bcm_vlan_port_t_init(default_native_ac);
        default_native_ac->criteria = BCM_VLAN_PORT_MATCH_NONE;
        default_native_ac->flags = BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_NATIVE
                | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        default_native_ac->egress_network_group_id = evpn_vxlan_info.evpn_nwk_grp_id;
        rv = bcm_vlan_port_create(unit, default_native_ac);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_vlan_port_create default_native_ac\n");
            return rv;
        }
    }

    printf("Set IML range\n");
    action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
    rv = bcm_mpls_range_action_add(unit, evpn_vxlan_info.iml_label_range_low,
                                   evpn_vxlan_info.iml_label_range_high, &action);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_mpls_range_action_add\n");
        return rv;
    }

    printf("Creating EVPN services for Peer1\n");
    rv = evpn_util_peer_services_create(unit, &evpn_vxlan_info.peer1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_util_peer_services_create peer1\n");
        return rv;
    }

    printf("Creating EVPN services for Peer2\n");
    rv = evpn_util_peer_services_create(unit, &evpn_vxlan_info.peer2);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_util_peer_services_create peer1\n");
        return rv;
    }

    return rv;
}

/*
 * Connect forwarding entries
 * 00:01:00:00:01:11 -> EVPN P1
 * 00:01:00:00:02:22 -> VXLAN TOR2
 */
int
evpn_vxlan_forwarding_connect(
    int unit)
{
    int rv = BCM_E_NONE;
    int encap_id;
    bcm_gport_t fec_gport;
    bcm_l2_addr_t l2addr1, l2addr2;
    int mc_id = evpn_vxlan_info.vpn;
    int cuds[2];
    int mc_ext_id;
    int port;

    printf("Connect static address 1 to EVPN\n");
    encap_id = BCM_L3_ITF_VAL_GET(evpn_vxlan_info.peer1.evpn_egress_label.tunnel_id);
    BCM_GPORT_FORWARD_PORT_SET(fec_gport, mpls_util_entity[0].fecs[0].fec_id);
    bcm_l2_addr_t_init(&l2addr1, evpn_vxlan_info.static_mac1, evpn_vxlan_info.vpn);
    l2addr1.flags |= BCM_L2_STATIC;
    l2addr1.encap_id = encap_id;
    l2addr1.port = fec_gport;
    rv = bcm_l2_addr_add(unit, &l2addr1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_l2_addr_add\n");
        return rv;
    }

    printf("Connect static address 2 to VXLAN\n");
    bcm_l2_addr_t_init(&l2addr2, evpn_vxlan_info.static_mac2, evpn_vxlan_info.vpn);
    l2addr2.flags |= BCM_L2_STATIC;
    l2addr2.port = evpn_vxlan_info.tor2.vxlan_port.vxlan_port_id;
    rv = bcm_l2_addr_add(unit, &l2addr2);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_l2_addr_add\n");
        return rv;
    }

    printf("Adding IML egress labels to the VPN MC group\n");
    /*
     * VXLAN encaps are added through the vxlan_add utility.
     */
    /** Peer1 IML */
    mc_ext_id = 0;
    cuds[0] = evpn_vxlan_info.peer1.iml_egress_label.tunnel_id;
    cuds[1] = *(mpls_util_entity[0].fecs[0].tunnel_gport); /** not really a gport */
    rv = bcm_multicast_encap_extension_create(unit, 0, &mc_ext_id, 2, cuds);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_multicast_encap_extension_create\n");
        return rv;
    }
    printf(" - Created PPMC entry with ID = 0x%08x\n", mc_ext_id);
    port = *(mpls_util_entity[0].fecs[0].port);
    rv = multicast__add_multicast_entry(unit, mc_id, &port, &mc_ext_id, 1, egress_mc);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: multicast__add_multicast_entry\n");
        return rv;
    }
    printf(" - <port=%d, rep_idx=0x%08x> added to MC group %d\n", port, mc_ext_id, mc_id);

    /** Peer2 IML */
    mc_ext_id = 0;
    cuds[0] = evpn_vxlan_info.peer2.iml_egress_label.tunnel_id;
    cuds[1] = *(mpls_util_entity[1].fecs[0].tunnel_gport); /** not really a gport */
    rv = bcm_multicast_encap_extension_create(unit, 0, &mc_ext_id, 2, cuds);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_multicast_encap_extension_create\n");
        return rv;
    }
    printf(" - Created PPMC entry with ID = 0x%08x\n", mc_ext_id);
    port = *(mpls_util_entity[1].fecs[0].port);
    rv = multicast__add_multicast_entry(unit, mc_id, &port, &mc_ext_id, 1, egress_mc);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: multicast__add_multicast_entry\n");
        return rv;
    }
    printf(" - <port=%d, rep_idx=0x%08x> added to MC group %d\n", port, mc_ext_id, mc_id);

    return rv;
}

/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_dnx_evpn_trunk.c
 * Purpose: Example of EVPN basic configuration with TRUNK links as ETH segments.
 */
/*
 * This cint demonstrate configuration of a PE in a EVPN network, where the CEs
 * are connected to the EVPN core using multiple links (LAG).
 * The following configuration is simulated:
 * +--------------------------------------------+--------------------------------------------------------+
 * |                                            |                                                        |
 * |                            Access Network  |  EVPN Core                                             |
 * |                                            |                                                        |
 * |                                            |                           EVPN Peer1                   |
 * |                                            |                           RIF 30                       |
 * |                                            |                           MAC: 00::01:33               |
 * |                                            |                           EVPN: 0x510                  |
 * |     Trunk 1                    +-----------+------------+              IML: 0x610, Peer-ID: 0x30    |
 * |     Members: 200, 201          |                        |              LSP: 0x5000                  |
 * |     VLAN: 64                   |                        |              NWK Group: 2                 |
 * |     <-------------------------->                        <--------------------------->               |
 * |     ESI: 0x111                 |                        |                                           |
 * |                                |                        |                                           |
 * |                                |      Provider Edge     |                                           |
 * |                                |         (PE1)          |                                           |
 * |                                |                        |                                           |
 * |     ESI: 0x222                 |                        |                                           |
 * |     <-------------------------->                        <--------------------------->               |
 * |     Trunk 2                    |                        |              EVPN Peer2                   |
 * |     Members: 13, 14            |                        |              RIF 40                       |
 * |     VLAN: 64                   +------------+-----------+              MAC: 00::01:44               |
 * |                                             |                          EVPN: 0x520                  |
 * |                                             |                          IML: 0x620, Peer-ID: 0x40    |
 * |                                             |                          LSP: 0x6000                  |
 * |                                             |                          NWK Group: 2                 |
 * |                                             |                                                       |
 * +---------------------------------------------+-------------------------------------------------------+
 *
 * VSI's MACT will include:
 * 00:01:00:00:01:11 -> EVPN P1
 *
 * EVPN Peer 1 is dual-homed to ESI 0x111
 * EVPN Peer 2 is dual-homed to ESI 0x222
 *
 * Usage:
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../../../src/examples/dnx/cint_dnx_utility_mpls.c
 * cint ../../../../src/examples/dnx/cint_dnx_utils_evpn.c
 * cint ../../../../src/examples/dnx/field/cint_field_evpn_esi.c
 * cint ../../../../src/examples/dnx/cint_dnx_utility_trunk.c
 * cint ../../../../src/examples/dnx/cint_dnx_evpn_trunk.c
 * cint
 * print evpn_trunk_main(unit, t1_port1, t1_port2, evpn1_port, evpn2_port);
 */


/*
 * Structure to hold information for creation of VLAN port over an ETH segment link.
 */
struct evpn_trunk_eth_segment_s
{
    cint_trunk_utils_s * trunk;
    bcm_vlan_t vid;
    bcm_vpn_t vsi;
    int esi;
    int add_to_flooding;

    bcm_vlan_port_t vlan_port;
};

/*
 * Structure to hold core mpls interfaces for carrying traffic to a peer
 */
struct evpn_trunk_core_intf_s
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
struct evpn_trunk_info_s
{
    int vpn;

    evpn_util_peer_info_s peer1;
    evpn_trunk_core_intf_s peer1_core_ifs;
    evpn_util_peer_info_s peer2;
    evpn_trunk_core_intf_s peer2_core_ifs;

    /** Static addresses added to the MACT */
    bcm_mac_t static_mac1;

    /** User defined in-lif profile for ESI FP filter. */
    int in_lif_acl_profile_esi;

    /** EVPN Network group ID for split horizon */
    int evpn_nwk_grp_id;
    /** When network group is taken from ESEM, need default native AC with the correct value */
    bcm_vlan_port_t evpn_default_native_ac;

    /** Access Network Group ID for split horizon */
    int access_nwk_grp_id;

    /** IML label range properties */
    bcm_mpls_label_t iml_label_range_low;
    bcm_mpls_label_t iml_label_range_high;

    /** CE links information */
    cint_trunk_utils_s trunk1;
    evpn_trunk_eth_segment_s es111;

    cint_trunk_utils_s trunk2;
    evpn_trunk_eth_segment_s es222;
};

evpn_trunk_info_s evpn_trunk_info;

int
evpn_trunk_params_init(
    int unit)
{
    int rv = BCM_E_NONE;
    evpn_util_peer_info_s * p_info;
    evpn_trunk_core_intf_s * p_ifs;
    bcm_mpls_egress_label_t * eg_label;

    bcm_mac_t p1_rif_mac = {0x00, 0x00, 0x00, 0x00, 0x01, 0x33};
    bcm_mac_t p1_next_hop = {0x00, 0x00, 0x00, 0x00, 0x02, 0x22};
    bcm_mac_t p2_rif_mac = {0x00, 0x00, 0x00, 0x00, 0x01, 0x44};
    bcm_mac_t p2_next_hop = {0x00, 0x00, 0x00, 0x00, 0x02, 0x23};

    bcm_mac_t static_mac1 = {0x00, 0x01, 0x00, 0x00, 0x01, 0x11};

    printf("Load global configuration\n");
    /*
     * By default, FP in-lif profile is 2 bits. The value for these bits is managed by the application, in
     * consideration of other ACL application that may use these bits.
     * Need to select a value between 0-3. 0 should be reserved for non-ESI profiles.
     * Selected 1 arbitrarily.
     */
    evpn_trunk_info.in_lif_acl_profile_esi = 1;
    /*
     * Select network groups for EVPN and Access, to configure split horizon to filter
     * traffic going back (for example through traffic) to the same network (EVPN to
     * EVPN).
     */
    evpn_trunk_info.access_nwk_grp_id = 0;
    evpn_trunk_info.evpn_nwk_grp_id = 2;

    evpn_trunk_info.vpn = 0x20;

    evpn_trunk_info.iml_label_range_low = 0x600;
    evpn_trunk_info.iml_label_range_high = 0x6ff;

    printf("Load CE1 link configuration\n");
    cint_trunk_utils_s * t1 = &evpn_trunk_info.trunk1;
    t1->group = 10;
    t1->vlan_domain = 128;
    t1->vlan = evpn_trunk_info.vpn;
    evpn_trunk_eth_segment_s * es1 = &evpn_trunk_info.es111;
    es1->esi = 0x111;
    es1->trunk = t1;
    es1->vid = evpn_trunk_info.vpn;
    es1->vsi = evpn_trunk_info.vpn;

    printf("Load CE2 link configuration\n");
    cint_trunk_utils_s * t2 = &evpn_trunk_info.trunk2;
    t2->group = 20;
    t2->vlan_domain = 96;
    t2->vlan = evpn_trunk_info.vpn;
    evpn_trunk_eth_segment_s * es2 = &evpn_trunk_info.es222;
    es2->esi = 0x222;
    es2->trunk = t2;
    es2->vid = evpn_trunk_info.vpn;
    es2->vsi = evpn_trunk_info.vpn;


    printf("Load Peer1 configuration\n");
    p_info = &evpn_trunk_info.peer1;
    p_info->vpn = evpn_trunk_info.vpn;
    p_info->evpn_label = 0x510;
    p_info->iml_label = 0x610;
    p_info->iml_esi_acl_lif_profile = evpn_trunk_info.in_lif_acl_profile_esi;
    p_info->iml_dual_homed_peer_profile = 0x30;
    p_info->nof_dual_homed_ces = 1;
    p_info->dual_homed_ce_info[0].esi_label = 0x111;
    p_info->dual_homed_ce_info[0].src_port = &evpn_trunk_info.trunk1.gport;
    p_info->network_group_id = evpn_trunk_info.evpn_nwk_grp_id;
    p_info->default_native_ac_for_split_horizon = &evpn_trunk_info.evpn_default_native_ac;
    p_ifs = &evpn_trunk_info.peer1_core_ifs;
    p_ifs->rif = 30;
    p_ifs->rif_mac = p1_rif_mac;
    p_ifs->next_hop_mac = p1_next_hop;
    p_ifs->lsp_label = 0x5000;


    printf("Load Peer2 configuration\n");
    p_info = &evpn_trunk_info.peer2;
    p_info->vpn = evpn_trunk_info.vpn;
    p_info->evpn_label = 0x520;
    p_info->iml_label = 0x620;
    p_info->iml_esi_acl_lif_profile = evpn_trunk_info.in_lif_acl_profile_esi;
    p_info->iml_dual_homed_peer_profile = 0x40;
    p_info->nof_dual_homed_ces = 1;
    p_info->dual_homed_ce_info[0].esi_label = 0x222;
    p_info->dual_homed_ce_info[0].src_port = &evpn_trunk_info.trunk2.gport;
    p_info->network_group_id = evpn_trunk_info.evpn_nwk_grp_id;
    p_info->default_native_ac_for_split_horizon = &evpn_trunk_info.evpn_default_native_ac;
    p_ifs = &evpn_trunk_info.peer2_core_ifs;
    p_ifs->rif = 40;
    p_ifs->rif_mac = p2_rif_mac;
    p_ifs->next_hop_mac = p2_next_hop;
    p_ifs->lsp_label = 0x6000;

    printf("Load static addresses for MACT\n");
    evpn_trunk_info.static_mac1 = static_mac1;

    return rv;
}


int
init_evpn_mpls_util_params(
    int unit)
{
    int rv = BCM_E_NONE;
    mpls_util_entity_s * entity;
    evpn_trunk_core_intf_s * p_ifs;

    printf("Configuring Entity 0 - MPLS tunnels from PE1 to Peer1\n");
    p_ifs = &evpn_trunk_info.peer1_core_ifs;
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

    printf("Configure Entity 1 - MPLS tunnel from PE1 to PE2 through P2\n");
    p_ifs = &evpn_trunk_info.peer2_core_ifs;
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

    /* When JR2 works under IOP mode, use FEC format:dest+EEI */
    if ((soc_property_get(unit , "system_headers_mode",1) == 0)) {
        mpls_util_entity[0].fecs[0].flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }

    return rv;
}

/*
 * Main function to configure the scenario described above and in cint_evpn.c
 * The function performs the following activities:
 * - Setup the VSI
 * - Create ETH segments - in this example - Trunks links
 * - Configure a L3 MPLS core connecting the device to neighboring MPLS routers
 * - Setup EVPN services for L2 MC and UC over the MPLS core
 * - Add static entries in MACT to emulate BGP triggered address advertising.
 */
int
evpn_trunk_main(
    int unit,
    int t1_port1,
    int t1_port2,
    int peer1_port,
    int peer2_port)
{
    int rv = BCM_E_NONE;

    printf("~~ EVPN Basic Example ~~\n");

    /* If it's IOP mode, use egress MC due to multiple outlifs in MC entries */
    if ((soc_property_get(unit , "system_headers_mode",1) == 0)) {
        egress_mc = 1;
    }

    printf("Load application profiles and misc configs\n");
    rv = evpn_trunk_params_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in init_evpn_trunk_params\n");
        return rv;
    }

    printf("Create VPN (%d)\n", evpn_trunk_info.vpn);
    rv = evpn_trunk_vswitch_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_trunk_vswitch_create\n");
        return rv;
    }

    printf("Init ESI filter FP application\n");
    rv = evpn_util_esi_filter_init(unit, evpn_trunk_info.in_lif_acl_profile_esi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_esi_filter_init\n");
        return rv;
    }

    printf("Init Split Horizon filter\n");
    rv = evpn_trunk_split_horizon_filter_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_trunk_split_horizon_filter_config\n");
        return rv;
    }

    printf("Create ETH segments\n");
    rv = evpn_trunk_eth_segments_create(unit, t1_port1, t1_port2, 13, 14);
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
    rv = evpn_trunk_forwarding_connect(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_trunk_forwarding_connect\n");
        return rv;
    }

    /* Configure ESI filter for IOP mode */
    if ((soc_property_get(unit , "system_headers_mode",1) == 0)) {
        /* Init forbidden port */
        field_evpn_info_init(unit, 0, evpn_trunk_info.trunk1.gport, peer1_port, 0, evpn_trunk_info.trunk1.gport, 0);

        cint_field_evpn_info.esi1_label[1] = 1;
        cint_field_evpn_info.esi1_label[2] = 17;
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

int
evpn_trunk_vswitch_create(
    int unit)
{
    int rv = BCM_E_NONE;

    printf("- Create VSI\n");
    rv = bcm_vswitch_create_with_id(unit, evpn_trunk_info.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_dnx_vswitch_create_with_id\n");
        return rv;
    }

    printf("- Create MC group\n");
    rv = multicast__open_mc_group(unit, &evpn_trunk_info.vpn, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__open_mc_group\n");
        return rv;
    }

    return rv;
}

/*
 * Configure split horizon for EVPN to EVPN filtering
 */
int
evpn_trunk_split_horizon_filter_config(
    int unit)
{
    int rv = BCM_E_NONE;

    bcm_switch_network_group_config_t network_group_config;
    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id = evpn_trunk_info.evpn_nwk_grp_id;
    network_group_config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
    rv = bcm_switch_network_group_config_set(unit, evpn_trunk_info.evpn_nwk_grp_id, &network_group_config);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_switch_network_group_config_set EVPN group ID\n");
        return rv;
    }

    return rv;
}




/*
 * Create a VLAN for the globally used VPN on an ETH segment port.
 */
int
evpn_trunk_es_create(
    int unit,
    evpn_trunk_eth_segment_s * es)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t * vport = &es->vlan_port;
    bcm_port_tpid_class_t tpid_class;

    bcm_port_tpid_class_t_init(&tpid_class);
    tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.tag_format_class_id = 4;
    tpid_class.tpid1 = 0x8100;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    tpid_class.port = es->trunk->gport;
    rv = bcm_port_tpid_class_set(unit, &tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_port_tpid_class_set\n");
        return rv;
    }

    bcm_vlan_port_t_init(vport);
    vport->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vport->port = es->trunk->gport;
    vport->match_vlan = es->vid;
    rv = bcm_vlan_port_create(unit, vport);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_port_create\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, es->vid, es->trunk->gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_gport_add\n");
        return rv;
    }

    printf("Add VLAN Port to VSI and flooding group\n");
    rv = bcm_vswitch_port_add(unit, es->vsi, vport->vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vswitch_port_add\n");
        return rv;
    }

    if (es->add_to_flooding)
    {
        rv = multicast__add_multicast_entry(unit, es->vsi, es->trunk->gport, &vport->encap_id, 1, egress_mc);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in multicast__add_multicast_entry\n");
            return rv;
        }
    }

    printf("Add ESI filter entry to FP ESI DB\n");
    rv = evpn_util_esi_filter_entry_add(unit, es->esi, &es->trunk->gport, 1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_util_esi_filter_entry_add\n");
        return rv;
    }

    return rv;
}
/*
 * Create Trunk T1 with ports t1_port1 and t1_port2,
 * Create Trunk T2 with ports t2_port1 and t2_port2.
 */
int
evpn_trunk_eth_segments_create(
    int unit,
    int t1_port1,
    int t1_port2,
    int t2_port1,
    int t2_port2)
{
    int rv = BCM_E_NONE;
    cint_trunk_utils_s * t1 = &evpn_trunk_info.trunk1;
    cint_trunk_utils_s * t2 = &evpn_trunk_info.trunk2;

    printf("Create Trunk T1\n");
    t1->ports[0] = t1_port1;
    t1->ports[1] = t1_port2;
    t1->ports_nof = 2;
    t1->headers_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    t1->headers_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    rv = cint_trunk_utils_create(unit, t1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in cint_trunk_utils_create\n");
        return rv;
    }
    printf("Create ES 0x111 over T1\n");
    evpn_trunk_info.es111.add_to_flooding = 1;
    rv = evpn_trunk_es_create(unit, &evpn_trunk_info.es111);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_trunk_es_create\n");
        return rv;
    }

    printf("Create Trunk T2\n");
    t2->ports[0] = t2_port1;
    t2->ports[1] = t2_port2;
    t2->ports_nof = 2;
    t2->headers_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    t2->headers_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    rv = cint_trunk_utils_create(unit, t2);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in cint_trunk_utils_create\n");
        return rv;
    }
    printf("Create ES 0x222 over T2\n");
    evpn_trunk_info.es222.add_to_flooding = 1;
    rv = evpn_trunk_es_create(unit, &evpn_trunk_info.es222);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_trunk_es_create\n");
        return rv;
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
                                                evpn_trunk_info.vpn,              0,
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
                                                evpn_trunk_info.vpn,              0,
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
        bcm_vlan_port_t * default_native_ac = &evpn_trunk_info.evpn_default_native_ac;
        /*
         * Default egress native AC with network group for the IMLs (used for
         * setting up split horizon)
         */
        printf("Create egress default native AC with EVPN network group ID for split horizon\n");
        bcm_vlan_port_t_init(default_native_ac);
        default_native_ac->criteria = BCM_VLAN_PORT_MATCH_NONE;
        default_native_ac->flags = BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_NATIVE
                | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        default_native_ac->egress_network_group_id = evpn_trunk_info.evpn_nwk_grp_id;
        rv = bcm_vlan_port_create(unit, default_native_ac);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_vlan_port_create default_native_ac\n");
            return rv;
        }
    }

    printf("Set IML range\n");
    action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
    rv = bcm_mpls_range_action_add(unit, evpn_trunk_info.iml_label_range_low,
                                   evpn_trunk_info.iml_label_range_high, &action);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_mpls_range_action_add\n");
        return rv;
    }

    printf("Creating EVPN services for Peer1\n");
    rv = evpn_util_peer_services_create(unit, &evpn_trunk_info.peer1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_util_peer_services_create peer1\n");
        return rv;
    }

    printf("Creating EVPN services for Peer2\n");
    rv = evpn_util_peer_services_create(unit, &evpn_trunk_info.peer2);
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
 *
 * Add IML encaps to the VSI's flooding group
 */
int
evpn_trunk_forwarding_connect(
    int unit)
{
    int rv = BCM_E_NONE;

    int encap_id;
    bcm_gport_t fec_gport;
    bcm_l2_addr_t l2addr1, l2addr2;
    int mc_id = evpn_trunk_info.vpn;
    int cuds[2];
    int mc_ext_id;
    int port;
    int system_headers_mode;

    printf("Connect static address 1 to EVPN\n");
    encap_id = BCM_L3_ITF_VAL_GET(evpn_trunk_info.peer1.evpn_egress_label.tunnel_id);
    BCM_GPORT_FORWARD_PORT_SET(fec_gport, mpls_util_entity[0].fecs[0].fec_id);
    bcm_l2_addr_t_init(&l2addr1, evpn_trunk_info.static_mac1, evpn_trunk_info.vpn);
    l2addr1.flags |= BCM_L2_STATIC;
    l2addr1.encap_id = encap_id;
    l2addr1.port = fec_gport;
    rv = bcm_l2_addr_add(unit, &l2addr1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_l2_addr_add\n");
        return rv;
    }

    /** JR2: Create fabric MC for each core in case of egress Multicast. */
    system_headers_mode = soc_property_get(unit, "system_headers_mode", 1);
    if (egress_mc && (system_headers_mode == 0) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)))
    {
        int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        bcm_module_t mreps[2];
        mreps[0] = 0;
        mreps[1] = 1; /* Ignore it in single core devices*/
        rv = bcm_fabric_multicast_set(unit, mc_id, 0, nof_cores, mreps);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_fabric_multicast_set \n");
            return rv;
        }
    }

    printf("Adding IML egress labels to the VPN MC group\n");
    /** Peer1 IML */
    mc_ext_id = 0;
    cuds[0] = evpn_trunk_info.peer1.iml_egress_label.tunnel_id;
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
    cuds[0] = evpn_trunk_info.peer2.iml_egress_label.tunnel_id;
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

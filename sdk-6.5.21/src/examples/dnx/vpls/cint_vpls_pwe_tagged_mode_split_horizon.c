/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * File: cint_vpls_pwe_tagged_mode_split_horizon.c
 * Purpose: Example of VPN flooding domain setup with AC ports, PWE ports and PWE tagged modes (indexed/unindexed).
 *
 * This example creates a VPN, a list of logical AC ports, a list of logical PWE ports and adds all the logical ports to the VPN.
 * pwe_tagged_mode_split_horizon_params_init shows an example of creating 2 AC ports and 2 PWE ports, and connecting
 * them to VPN 100. See figure below:
 * +-----------------------------------------+-------------------------------------------------------------+
 * |                                         :                                                             |
 * |                                  NWK 0  :  NWK 1                                                      |
 * |                                         :                                                             |
 * |                                         :                        Native ETH VID: 100                  |
 * |                                         :                        VC label: 3456                       |
 * |                                         :                        MPLS label: 3333                     |
 * |                                         :                        VID: 30                              |
 * |                                +--------+-------+                RIF MAC:  00:0C:00:02:00:01          |
 * |                                |                |                Next Hop: 00:00:00:00:CD:1D          |
 * |   nwk_group_id: 0              |                |                nwk_group_id: 2                      |
 * |   AC 1    <-------------------->                <-------------------->   PWE 1                        |
 * |                                |                |                                                     |
 * |      VID: 100                  |    VSwitch     |                                                     |
 * |                                |    VPN: 100    |                                                     |
 * |   AC 2    <-------------------->                <-------------------->   PWE 2                        |
 * |   nwk_group_id: 1              |                |                 Native ETH VID 100                  |
 * |                                +--------+-------+                 VC label: 3457                      |
 * |                                         :                         MPLS label: 3334                    |
 * |                                         :                         MPLS label: 3335                    |
 * |                                         :                         VID: 31                             |
 * |                                         :                         RIF MAC:  00:0C:00:02:00:02         |
 * |                                         :                         Next Hop: 00:00:00:00:CD:1E         |
 * |                                         :                         nwk_group_id: 3                     |
 * |                                         :                                                             |
 * |                                         :                                                             |
 * +-----------------------------------------+-------------------------------------------------------------+
 *
 * PWE_1 is connected to unindexed native AC.
 * PWE_2 is connected to indexed native AC.
 *
 * Usage:
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_tpid.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../../../src/examples/sand/cint_qos_l2_phb.c
 * cint ../../../../src/examples/sand/cint_sand_mpls_qos.c
 * cint ../../../../src/examples/sand/cint_sand_vswitch_vpls.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../../../src/examples/dnx/vpls/cint_vpls_pwe_tagged_mode_split_horizon.c
 * cint
 * print pwe_tagged_mode_split_horizon_main(0,200,201,202,203);
 */

/*
 * Global settings
 */

int pwe_tm_sh_ac_1_nwk_group_id = 0;
int pwe_tm_sh_ac_2_nwk_group_id = 1;
int pwe_tm_sh_pwe_1_nwk_group_id = 2;
int pwe_tm_sh_pwe_2_nwk_group_id = 3;

int pwe_tm_sh_illegal_qos_id = 0xffff;
int pwe_tm_sh_vpls_ingress_qos_id = 3;
int pwe_tm_sh_eth_egress_qos_id = 0;
int pwe_tm_sh_ing_opcode_id = 5;
int pwe_tm_sh_egr_opcode_id = 5;
int pwe_tm_sh_eth_ingress_qos_id = 4;


struct pwe_tagged_mode_split_horizon_native_ac_s
{
    int is_indexed;                             /* indication whether the Native AC is used for indexed or unindexed PWE tagged mode */
    bcm_gport_t ingress_vlan_port_id;           /* vlan port id of the inner ac_port, used for ingress */
    bcm_gport_t egress_vlan_port_id;            /* vlan port id of the inner ac_port, used for egress */
    bcm_if_t ac_encap_id;                       /* The global lif of the indexed AC entry. */
    bcm_gport_t* ingress_port;
    bcm_gport_t* egress_port;
    bcm_vlan_t outer_vlan;
    bcm_vlan_t inner_vlan;
    int ingress_network_group_id;
    int egress_network_group_id;
};

struct pwe_tagged_mode_split_horizon_vlan_params_s
{
    uint16 outer_tpid;
    uint16 inner_tpid;
    bcm_port_tag_format_class_t single_tag_format;
    bcm_port_tag_format_class_t double_tagged_format;
    bcm_gport_t* double_tagged_port;
    uint32 egress_vlan_edit_profile;
    uint32 ingress_vlan_edit_profile;
    bcm_vlan_t egress_canonical_outer_vlan;
    bcm_vlan_t egress_canonical_inner_vlan;
    uint32 class_id;                            /* used for PWE's vlan domain (network layer domain) */
};

struct pwe_tagged_mode_split_horizon_add_mc_s
{
    int add_ac;
    int add_pwe;
};

pwe_tagged_mode_split_horizon_vlan_params_s vlan_params[2];
pwe_tagged_mode_split_horizon_native_ac_s native_ac[2];

/* Used for debugging - indicates whether to include AC (add_ac) and PWE (add_pwe) ports to multicast group */
pwe_tagged_mode_split_horizon_add_mc_s add_mc_dev[2];

/*
 * Setup for example application. See file header for description
 */
void
pwe_tagged_mode_split_horizon_params_init(
    int unit)
{
    int nwk_grp_from_esem;
    int isj2c_q2a = 0;
    int rv;

    isj2c_q2a = is_device_or_above(unit, JERICHO2C) && !is_device_or_above(unit, JERICHO2P);
    nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);

    /*
    * unindexed
    */
    /* pwe ingress tagged mode */
    vswitch_vpls_params.pwe_params[0].pwe_term.nof_service_tags = 2;
    vswitch_vpls_params.pwe_params[0].pwe_term.flags |= BCM_MPLS_PORT_SERVICE_TAGGED;
    /* pwe qos */
    vswitch_vpls_params.pwe_params[0].pwe_term.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    vswitch_vpls_params.pwe_params[0].pwe_term.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    vswitch_vpls_params.pwe_params[0].pwe_term.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    vswitch_vpls_params.pwe_params[0].pwe_term.qos_map_id = pwe_tm_sh_eth_ingress_qos_id;
    /* pwe egress tagged mode */
    vswitch_vpls_params.pwe_params[0].pwe_encap.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    vswitch_vpls_params.pwe_params[0].mpls_encap_tunnel.encap_access = bcmEncapAccessTunnel3;

    if(vswitch_vpls_params.test_split_horizon)
    {
        vswitch_vpls_params.ac_ports[0].nwk_group_valid = 1;
        vswitch_vpls_params.ac_ports[0].ac_nwk_group_id = pwe_tm_sh_ac_1_nwk_group_id;
        vswitch_vpls_params.pwe_params[0].pwe_encap.nwk_group_valid = 1;
        vswitch_vpls_params.pwe_params[0].pwe_encap.pwe_nwk_group_id = pwe_tm_sh_pwe_1_nwk_group_id;
        vswitch_vpls_params.pwe_params[0].pwe_term.nwk_group_valid = 1;
        vswitch_vpls_params.pwe_params[0].pwe_term.pwe_nwk_group_id = pwe_tm_sh_pwe_1_nwk_group_id;
    }

    native_ac[0].is_indexed = 0;
    native_ac[0].ingress_port = &vswitch_vpls_params.pwe_params[0].pwe_term.mpls_port_id;
    native_ac[0].egress_port = &vswitch_vpls_params.pwe_params[0].pwe_encap.mpls_port_id;;
    native_ac[0].outer_vlan = 1000;
    native_ac[0].inner_vlan = 2000;
    native_ac[0].ingress_network_group_id = vswitch_vpls_params.ac_ports[0].ac_nwk_group_id;
    native_ac[0].egress_network_group_id = vswitch_vpls_params.pwe_params[0].pwe_encap.pwe_nwk_group_id;

    vlan_params[0].outer_tpid = 0x9200;
    vlan_params[0].inner_tpid = 0x8200;
    vlan_params[0].single_tag_format = 8;
    vlan_params[0].double_tagged_format = 16;
    vlan_params[0].double_tagged_port = &vswitch_vpls_params.pwe_params[0].pwe_term.mpls_port_id;
    vlan_params[0].egress_vlan_edit_profile = 4;
    vlan_params[0].ingress_vlan_edit_profile = 5;
    vlan_params[0].egress_canonical_outer_vlan = native_ac[0].outer_vlan;
    vlan_params[0].egress_canonical_inner_vlan = native_ac[0].inner_vlan;
    vlan_params[0].class_id = isj2c_q2a ? 2019 : 507;

    add_mc_dev[0].add_ac = 1;
    add_mc_dev[0].add_pwe = 1;

    /*
    * indexed
    */
    /* pwe ingress tagged mode */
    vswitch_vpls_params.pwe_params[1].pwe_term.nof_service_tags = 2;
    vswitch_vpls_params.pwe_params[1].pwe_term.flags |= BCM_MPLS_PORT_SERVICE_TAGGED;
    
    /*vswitch_vpls_params.pwe_params[1].pwe_term.flags2 |= BCM_MPLS_PORT2_SERVICE_TAGGED_INDEXED;*/
    /* pwe qos */
    vswitch_vpls_params.pwe_params[1].pwe_term.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    vswitch_vpls_params.pwe_params[1].pwe_term.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    vswitch_vpls_params.pwe_params[1].pwe_term.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    vswitch_vpls_params.pwe_params[1].pwe_term.qos_map_id = pwe_tm_sh_eth_ingress_qos_id;

    /* pwe egress tagged mode */
    vswitch_vpls_params.pwe_params[1].pwe_encap.encap_access = bcmEncapAccessTunnel2;
    vswitch_vpls_params.pwe_params[1].mpls_encap_tunnel.encap_access = bcmEncapAccessTunnel3;
    /* pwe qos */
    vswitch_vpls_params.pwe_params[1].pwe_encap.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;

    if(vswitch_vpls_params.test_split_horizon)
    {
        vswitch_vpls_params.ac_ports[1].nwk_group_valid = 1;
        
        vswitch_vpls_params.ac_ports[1].ac_nwk_group_id = pwe_tm_sh_ac_2_nwk_group_id;
        vswitch_vpls_params.pwe_params[1].pwe_encap.nwk_group_valid = 1;
        vswitch_vpls_params.pwe_params[1].pwe_encap.pwe_nwk_group_id = pwe_tm_sh_pwe_2_nwk_group_id;
        vswitch_vpls_params.pwe_params[1].pwe_term.nwk_group_valid = 1;
        vswitch_vpls_params.pwe_params[1].pwe_term.pwe_nwk_group_id = pwe_tm_sh_pwe_2_nwk_group_id;
    }

    native_ac[1].is_indexed = 1;
    native_ac[1].ingress_port = &vswitch_vpls_params.pwe_params[1].pwe_term.mpls_port_id;
    native_ac[1].egress_port = &vswitch_vpls_params.pwe_params[1].pwe_encap.mpls_port_id;;
    native_ac[1].outer_vlan = 1000;
    native_ac[1].inner_vlan = 2000;
    native_ac[1].ingress_network_group_id = vswitch_vpls_params.ac_ports[1].ac_nwk_group_id;
    native_ac[1].egress_network_group_id = vswitch_vpls_params.pwe_params[1].pwe_encap.pwe_nwk_group_id;

    vlan_params[1].outer_tpid = 0x9300;
    vlan_params[1].inner_tpid = 0x8300;
    vlan_params[1].single_tag_format = 8;
    vlan_params[1].double_tagged_format = 16;
    vlan_params[1].double_tagged_port = &vswitch_vpls_params.pwe_params[1].pwe_term.mpls_port_id;
    vlan_params[1].egress_vlan_edit_profile = 6;
    vlan_params[1].ingress_vlan_edit_profile = 7;
    vlan_params[1].egress_canonical_outer_vlan = native_ac[1].outer_vlan;
    vlan_params[1].egress_canonical_inner_vlan = native_ac[1].inner_vlan;
    vlan_params[1].class_id = isj2c_q2a ? 2020 : 508;

    add_mc_dev[1].add_ac = 1;
    add_mc_dev[1].add_pwe = 1;
}

/*
 * For Ingress:
 * Define AC for inner ETH validation on packets going from core to customer.
 * The resulting vlan_port_id is saved in cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id.
 */
int
pwe_tm_sh_add_network_native_ac(
    int unit,
    bcm_vpn_t vsi,
    pwe_tagged_mode_split_horizon_native_ac_s* native_ac)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    /*
     * add port.
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    if (native_ac->is_indexed)
    {
        vlan_port.flags |= BCM_VLAN_PORT_ALLOC_SYMMETRIC;
        if (vswitch_vpls_params.test_split_horizon)
        {
            vlan_port.ingress_network_group_id = native_ac->ingress_network_group_id;
        }
    }
    else
    {
        vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
    }
    vlan_port.vsi = vsi;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }
    /*
     * Save the created vlan_port_id
     */
    native_ac->ingress_vlan_port_id = vlan_port.vlan_port_id;
    printf("    ingress native ac = 0x%08x\n", vlan_port.vlan_port_id);

    /*
     * Add Match
     */
    bcm_port_match_info_t match_info;
    bcm_port_match_info_t_init(&match_info);
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY | BCM_PORT_MATCH_NATIVE;
    match_info.port = *native_ac->ingress_port;
    match_info.match = BCM_PORT_MATCH_PORT_VLAN_STACKED;        /* Match 2 tags */
    match_info.match_vlan = native_ac->outer_vlan;
    match_info.match_inner_vlan = native_ac->inner_vlan;
    rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_port_match_add\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Define AC for inner ETH validation on packets going from customer to core.
 * This creates a SW DB entry that is later used by VLAN translation APIs for setting inner ETH editting.
 * The resulting vlan_port_id is saved in vlan_port_id.
 */
int
pwe_tm_sh_add_core_native_ac(
    int unit,
    bcm_vpn_t vsi,
    pwe_tagged_mode_split_horizon_native_ac_s* native_ac)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    int nwk_grp_from_esem;
    /*
     * add port.
     */

    nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    if (native_ac->is_indexed)
    {
        vlan_port.vsi = 0;
        vlan_port.tunnel_id = *native_ac->egress_port;
        vlan_port.flags |= BCM_VLAN_PORT_WITH_ID;
        vlan_port.vlan_port_id = native_ac->egress_vlan_port_id;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
        
        if(vswitch_vpls_params.test_split_horizon)
        {
            vlan_port.egress_network_group_id = native_ac->egress_network_group_id;
        }
    }
    else
    {
        vlan_port.vsi = vsi;
        vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS;
        vlan_port.port = *native_ac->egress_port;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;

        if(vswitch_vpls_params.test_split_horizon && nwk_grp_from_esem)
        {
            vlan_port.egress_network_group_id = native_ac->egress_network_group_id;
        }
    }    

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }
    /*
     * Save the created vlan_port_id
     */
    native_ac->egress_vlan_port_id = vlan_port.vlan_port_id;
    native_ac->ac_encap_id = vlan_port.encap_id;

    printf("    egress native ac vlan_port_id = 0x%08x\n", vlan_port.vlan_port_id);
    printf("    egress native ac encap_id = 0x%08x\n", vlan_port.encap_id);
    

    return rv;
}

/* Set tag formats for un tagged and double tagged packets*/
int
pwe_tm_sh_tag_formats_set(
    int unit,
    pwe_tagged_mode_split_horizon_vlan_params_s* vlan_params)
{
    int rv = BCM_E_NONE;
    bcm_port_tpid_class_t port_tpid_class;

    /*
     * set tag format for double tagged packets 
     */
    /* add TPID to global array */
    rv = tpid__tpid_add(unit, vlan_params->outer_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add for 0x$x, rv = %d\n", vlan_params->outer_tpid, rv);
        return rv;
    }

    /* add TPID to global array */
    rv = tpid__tpid_add(unit, vlan_params->inner_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add for 0x%x, rv = %d\n", vlan_params->inner_tpid, rv);
        return rv;
    }

    printf("    double tagged: port= 0x%x, tag format= %d, tpid1 = 0x%x, tpid2 = 0x%x\n", *vlan_params->double_tagged_port, vlan_params->double_tagged_format, vlan_params->outer_tpid, vlan_params->inner_tpid);
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = *vlan_params->double_tagged_port;
    port_tpid_class.tpid1 = vlan_params->outer_tpid;
    port_tpid_class.tpid2 = vlan_params->inner_tpid;
    port_tpid_class.tag_format_class_id = vlan_params->double_tagged_format;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set, double_tagged_port=%d, \n", *vlan_params->double_tagged_port);
        return rv;
    }

    return rv;
}

int
pwe_tm_sh_configure_nwk_groups(
    int unit,
    int nwk_group_ac_1,
    int nwk_group_ac_2,
    int nwk_group_pwe_1,
    int nwk_group_pwe_2)
{
    int rv = BCM_E_NONE;
    bcm_switch_network_group_config_t config;
    int src_idx, dst_idx;

    printf("Set all network groups to DROP:\n");
    for (src_idx = 0; src_idx < 4; src_idx++)
    {
        for (dst_idx = 0; dst_idx < 4; dst_idx++)
        {
            printf("    source_network_group_id(%d) -> dest_network_group_id(%d) DROP\n", src_idx, dst_idx);
            config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
            config.dest_network_group_id = dst_idx;
            rv = bcm_switch_network_group_config_set(unit, src_idx, &config);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_switch_network_group_config_set src=%d -> dst=%d\n", src_idx, dst_idx);
                return rv;
            }
        }
    }

    printf("Open required directions:\n");

    printf("    source_network_group_id(%d) -> dest_network_group_id(%d) PASS\n", nwk_group_ac_1, nwk_group_pwe_1);
    config.config_flags = 0;
    config.dest_network_group_id = nwk_group_pwe_1;
    rv = bcm_switch_network_group_config_set(unit, nwk_group_ac_1, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_network_group_config_set src=%d -> dst=%d\n", nwk_group_ac_1, nwk_group_pwe_1);
        return rv;
    }

    printf("    source_network_group_id(%d) -> dest_network_group_id(%d) PASS\n", nwk_group_ac_1, nwk_group_ac_2);
    config.config_flags = 0;
    config.dest_network_group_id = nwk_group_ac_2;
    rv = bcm_switch_network_group_config_set(unit, nwk_group_ac_1, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_network_group_config_set src=%d -> dst=%d\n", nwk_group_ac_1, nwk_group_ac_2);
        return rv;
    }

    printf("    source_network_group_id(%d) -> dest_network_group_id(%d) PASS\n", nwk_group_ac_1, nwk_group_ac_1);
    config.config_flags = 0;
    config.dest_network_group_id = nwk_group_ac_1;
    rv = bcm_switch_network_group_config_set(unit, nwk_group_ac_1, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_network_group_config_set src=%d -> dst=%d\n", nwk_group_ac_1, nwk_group_ac_1);
        return rv;
    }

    printf("    source_network_group_id(%d) -> dest_network_group_id(%d) PASS\n", nwk_group_ac_2, nwk_group_ac_2);
    config.config_flags = 0;
    config.dest_network_group_id = nwk_group_ac_2;
    rv = bcm_switch_network_group_config_set(unit, nwk_group_ac_2, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_network_group_config_set src=%d -> dst=%d\n", nwk_group_ac_2, nwk_group_ac_2);
        return rv;
    }

    printf("    source_network_group_id(%d) -> dest_network_group_id(%d) PASS\n", nwk_group_ac_2, nwk_group_pwe_2);
    config.config_flags = 0;
    config.dest_network_group_id = nwk_group_pwe_2;
    rv = bcm_switch_network_group_config_set(unit, nwk_group_ac_2, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_network_group_config_set src=%d -> dst=%d\n", nwk_group_ac_2, nwk_group_pwe_2);
        return rv;
    }

    printf("    source_network_group_id(%d) -> dest_network_group_id(%d) PASS\n", nwk_group_pwe_1, nwk_group_pwe_1);
    config.config_flags = 0;
    config.dest_network_group_id = nwk_group_pwe_1;
    rv = bcm_switch_network_group_config_set(unit, nwk_group_pwe_1, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_network_group_config_set src=%d -> dst=%d\n", nwk_group_pwe_1, nwk_group_pwe_1);
        return rv;
    }

    printf("    source_network_group_id(%d) -> dest_network_group_id(%d) PASS\n", nwk_group_pwe_1, nwk_group_ac_1);
    config.config_flags = 0;
    config.dest_network_group_id = nwk_group_ac_1;
    rv = bcm_switch_network_group_config_set(unit, nwk_group_pwe_1, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_network_group_config_set src=%d -> dst=%d\n", nwk_group_pwe_1, nwk_group_ac_1);
        return rv;
    }

    printf("    source_network_group_id(%d) -> dest_network_group_id(%d) PASS\n", nwk_group_pwe_2, nwk_group_pwe_2);
    config.config_flags = 0;
    config.dest_network_group_id = nwk_group_pwe_2;
    rv = bcm_switch_network_group_config_set(unit, nwk_group_pwe_2, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_network_group_config_set src=%d -> dst=%d\n", nwk_group_pwe_2, nwk_group_pwe_2);
        return rv;
    }

    printf("    source_network_group_id(%d) -> dest_network_group_id(%d) PASS\n", nwk_group_pwe_2, nwk_group_ac_2);
    config.config_flags = 0;
    config.dest_network_group_id = nwk_group_ac_2;
    rv = bcm_switch_network_group_config_set(unit, nwk_group_pwe_2, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_network_group_config_set src=%d -> dst=%d\n", nwk_group_pwe_2, nwk_group_ac_2);
        return rv;
    }
    
    return rv;
}

int
pwe_tm_sh_configure_eve_ive(
    int unit,
    bcm_vpn_t vsi,
    pwe_tagged_mode_split_horizon_vlan_params_s* vlan_params,
    pwe_tagged_mode_split_horizon_native_ac_s* native_ac)
{
    int rv = BCM_E_NONE;
    bcm_gport_t native_vlan_gport;

    /* Set PWE's vlan domain (network layer domain). */
    rv = bcm_port_class_set(unit, *native_ac->ingress_port, bcmPortClassIngress, vlan_params->class_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=0x%x, \n", *native_ac->ingress_port);
        return rv;
    }

    /* Set tag formats for un tagged and double tagged packets */
    printf("Configure tag formats\n");
    rv = pwe_tm_sh_tag_formats_set(unit, vlan_params);
    if (rv != BCM_E_NONE)
    {
        printf("Error, pwe_tm_sh_tag_formats_set failed\n");
        return rv;
    }

    /* Add AC for inner ETH IVE (removing/replacing service tags) - for Ingress flow */
    printf("Inner AC configuration\n");
    rv = pwe_tm_sh_add_network_native_ac(unit, vsi, native_ac);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_tm_sh_add_network_native_ac\n", rv);
        return rv;
    }

    if(native_ac->is_indexed)
    {
        native_vlan_gport = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(native_ac->ingress_vlan_port_id);
        BCM_GPORT_SUB_TYPE_LIF_SET(native_ac->egress_vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, native_vlan_gport);
        BCM_GPORT_VLAN_PORT_ID_SET(native_ac->egress_vlan_port_id, native_ac->egress_vlan_port_id);
    }

    /* Configure IVE for core to access traffic: delete two service tags */
    rv = vlan_translate_ive_eve_translation_set(unit, native_ac->ingress_vlan_port_id,
                                           0, 0, bcmVlanActionDelete, bcmVlanActionDelete, 0, 0,
                                           vlan_params->ingress_vlan_edit_profile, vlan_params->double_tagged_format, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=0x%08x, \n", vlan_params->ingress_native_ac_vlan_port_id);
        return rv;
    }

    /* Add AC for inner ETH EVE (adding service tags) - for Egress flow */
    rv = pwe_tm_sh_add_core_native_ac(unit, vsi, native_ac);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_tm_sh_add_core_native_ac\n", rv);
        return rv;
    }

    /* Add EVE for the AC created in the previous step */
    rv = vlan_translate_ive_eve_translation_set(unit, native_ac->egress_vlan_port_id,
                                           vlan_params->outer_tpid, vlan_params->inner_tpid, bcmVlanActionReplace, bcmVlanActionAdd,
                                           vlan_params->egress_canonical_outer_vlan, vlan_params->egress_canonical_inner_vlan,
                                           vlan_params->egress_vlan_edit_profile, vlan_params->single_tag_format, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=0x%08x, \n", native_ac->egress_vlan_port_id);
        return rv;
    }

    return rv;
}

/*
 * Add PWE to VPN
 */
int
pwe_tm_sh_vpn_pwe_add(
    int unit,
    bcm_vpn_t vpn_id,
    vswitch_vpls_pwe_info_s * pwe_param,
    int is_indexed,
    bcm_if_t ac_encap_id)
{

    int rv = BCM_E_NONE;
    int mc_id = vpn_id;
    bcm_multicast_replication_t rep;
    int cuds[2];

    bcm_multicast_replication_t_init(&rep);
    rep.port = pwe_param->pwe_port.port;
    rep.encap1 = pwe_param->pwe_mc_rep_idx;

    rv = bcm_multicast_add(unit, mc_id, egress_mc?BCM_MULTICAST_EGRESS_GROUP:BCM_MULTICAST_INGRESS_GROUP, 1, &rep);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    /*
     * Add mpls encapsulation as outlif2 to PPMC table
     * Key: mc_rep_idx
     * Result: PWE encap_id, MPLS encap_id
     */
    if(is_indexed)
    {
        BCM_L3_ITF_SET(cuds[0], BCM_L3_ITF_TYPE_LIF, ac_encap_id);
    }
    else
    {
        BCM_L3_ITF_SET(cuds[0], BCM_L3_ITF_TYPE_LIF, pwe_param->pwe_encap.encap_id);
    }
    cuds[1] = pwe_param->mpls_encap_tunnel[0].tunnel_id;
    rv = bcm_multicast_encap_extension_create(unit, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID, &(pwe_param->pwe_mc_rep_idx),
            2, cuds);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_encap_extension_create\n");
        return rv;
    }

    return BCM_E_NONE;
}

int
pwe_tm_sh_create_vpls(
    int unit,
    int vsi,
    l2_port_properties_s *ac_port,
    vswitch_vpls_pwe_info_s *pwe_param,
    pwe_tagged_mode_split_horizon_vlan_params_s vlan_params,
    pwe_tagged_mode_split_horizon_native_ac_s native_ac,
    pwe_tagged_mode_split_horizon_add_mc_s add_mc_dev)
{
    int rv = BCM_E_NONE;

    /* Init AC port */
    rv = vswitch_vpls_ac_create(unit, ac_port, vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_configure_port_properties\n", rv);
        return rv;
    }

    /* create PWE */
    rv = vswitch_vpls_pwe_create(unit, pwe_param);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpls_pwe_create\n", rv);
        return rv;
    }
    printf("Configure eve ive using:\n");
    printf("    ingress mpls_port_id = 0x%x\n", pwe_param->pwe_term.mpls_port_id);
    printf("    egress mpls_port_id = 0x%x\n", pwe_param->pwe_encap.mpls_port_id);
    rv = pwe_tm_sh_configure_eve_ive(unit, vsi, &vlan_params, &native_ac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, pwe_tm_sh_configure_eve_ive\n");
        return rv;
    }

    /* Connect GPORT (VLAN-Port) to its QOS-profile */
    rv = pwe_tm_sh_qos_map_gport(unit, pwe_tm_sh_eth_ingress_qos_id, native_ac.ingress_vlan_port_id, pwe_tm_sh_eth_egress_qos_id, native_ac.egress_vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, pwe_tm_sh_qos_map_gport vlan_port_ing = 0x%x, vlan_port_eg = 0x%x\n", native_ac.ingress_vlan_port_id, native_ac.egress_vlan_port_id);
        return rv;
    }

    rv = pwe_tm_sh_qos_map_gport(unit, pwe_tm_sh_eth_ingress_qos_id,/*pwe_param->pwe_term.mpls_port_id*/ac_port->vlan_port_id, pwe_tm_sh_eth_egress_qos_id, pwe_param->pwe_encap.mpls_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, pwe_tm_sh_qos_map_gport egress mpls_port_id_ing = 0x%x, mpls_port_id_eg = 0x%x\n", pwe_param->pwe_term.mpls_port_id, pwe_param->pwe_encap.mpls_port_id);
        return rv;
    }

    printf("Connect AC and PWE to multicast\n\n");
    /* Add AC to multicast VPN */
    if(add_mc_dev.add_ac)
    {
        rv = vswitch_vpls_vpn_vlan_port_add(unit, vsi, ac_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vswitch_vpls_vpn_vlan_port_add\n", rv);
            return rv;
        }
    }
    /* Add pwe to multicast VPN */
    if(add_mc_dev.add_pwe)
    {
        rv = pwe_tm_sh_vpn_pwe_add(unit, vsi, pwe_param, native_ac.is_indexed, native_ac.ac_encap_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vswitch_vpls_vpn_pwe_add\n", rv);
            return rv;
        }
    }

    return rv;
}

/*
 * Connect GPORT (VLAN-Port) to its QOS-profile
 */
int
pwe_tm_sh_qos_map_gport(
    int unit,
    int ingress_qos_profile,
    bcm_gport_t vlan_port_id,
    int egress_qos_profile,
    bcm_gport_t mpls_port_id)
{
    int rv = BCM_E_NONE;
    int ingress_qos_map_id;
    int egress_qos_map_id;

    rv = bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK,
                                       ingress_qos_profile, &ingress_qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_qos_map_id_get_by_profile\n");
        return rv;
    }

    rv = bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK,
                                       egress_qos_profile, &egress_qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_qos_map_id_get_by_profile\n");
        return rv;
    }

    rv = bcm_qos_port_map_set(unit, vlan_port_id, ingress_qos_map_id, -1);
    if (rv != BCM_E_NONE)
    {
        printf("error bcm_qos_port_map_set setting up ingress gport(0x%08x) to map\n", vlan_port_id);
        return rv;
    }

    /** set remark profile to pwe*/
    rv = bcm_qos_port_map_set(unit, mpls_port_id, -1, egress_qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("error bcm_qos_port_map_set setting up egress gport(0x%08x) to map\n", mpls_port_id);
        return rv;
    }

    return rv;
}
    
/*
 * Main flow. See file header for description.
 */
int
pwe_tagged_mode_split_horizon_main(
    int unit,
    int ac_port_1,
    int ac_port_2,
    int pwe_port_1,
    int pwe_port_2)
{
    int rv = BCM_E_NONE;
    int idx;

    vswitch_vpls_params.test_split_horizon = 1;
    vswitch_vpls_params_init(unit, &vswitch_vpls_params, ac_port_1, ac_port_2, pwe_port_1, pwe_port_2);

    pwe_tagged_mode_split_horizon_params_init(unit);

    /* Eth QOS */
    rv = qos_map_l2_example(unit, pwe_tm_sh_eth_ingress_qos_id, pwe_tm_sh_eth_egress_qos_id, pwe_tm_sh_ing_opcode_id, pwe_tm_sh_egr_opcode_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, qos_map_l2_example\n");
        return rv;
    }

    /* MPLS QOS */
    rv = dnx_qos_map_mpls(unit,1,1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, dnx_qos_map_mpls\n");
        return rv;
    }

    /* create vlan based on the vsi (vpn) */
    rv = bcm_vlan_create(unit, vswitch_vpls_params.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, vswitch_vpls_params.vpn);
        return rv;
    }

    /*
     * Delete all global tpids 
     * Note: This will delete all the default settings (see appl_dnx_vlan_init). 
     */
    rv = tpid__tpids_clear_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpids_clear_all, rv = %d\n", rv);
        return rv;
    }

    /* Create VPN */
    rv = vswitch_vpls_vpn_create(unit, vswitch_vpls_params.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpls_vpn_create\n");
        return rv;
    }

    if(vswitch_vpls_params.test_split_horizon)
    {
        printf("Configure network groups for split horyzon filtering\n");
        rv = pwe_tm_sh_configure_nwk_groups(unit, vswitch_vpls_params.ac_ports[0].ac_nwk_group_id, vswitch_vpls_params.ac_ports[1].ac_nwk_group_id, 
                                    vswitch_vpls_params.pwe_params[0].pwe_encap.pwe_nwk_group_id, vswitch_vpls_params.pwe_params[1].pwe_encap.pwe_nwk_group_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, pwe_tm_sh_configure_nwk_groups\n");
            return rv;
        }
    }

    for(idx = 0; idx < vswitch_vpls_params.pwe_ports_nof; idx++)
    {
        rv = pwe_tm_sh_create_vpls(unit, vswitch_vpls_params.vpn, &(vswitch_vpls_params.ac_ports[idx]), &(vswitch_vpls_params.pwe_params[idx]), 
                                    vlan_params[idx], native_ac[idx], add_mc_dev[idx]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, pwe_tm_sh_create_vpls failed for idx = %d\n", idx);
            return rv;
        }
    }

    return rv;
}



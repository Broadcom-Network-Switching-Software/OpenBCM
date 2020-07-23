/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_stat_pp_egress_metadata.c Purpose: Statistics examples for SDK egress statistics and metadata APIs using an L2 application.
 */

/*
 * File: cint_stat_pp_egress_metadata.c
 * Purpose: Example of multiple egress PP statistics
 *
 * Count packets on specific metadata offset inside the counted object counter set (in relevant offset)
 *
 * Usage:
 * ------
 *
 * Test Scenario:
 *--------------
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dnx/l3/cint_ip_route_encap_src_mac.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_egress_metadata.c
 * cint
 * basic_example_egress_metadata_fwd_stat_pp(0,1);
 * or
 * basic_example_egress_metadata_enc_stat_pp(0,1,2,3);
 * exit;
 */

struct cint_stat_pp_egress_metadata_config_info_s
{
    bcm_stat_pp_metadata_header_type_t header_type;
    uint32 egress_metadata_val;
};

cint_stat_pp_egress_metadata_config_info_s cint_stat_pp_egress_metadata_enc_info_ipv4 =
{
        bcmStatPpMetadataHeaderTypeIpv4,
        1,
};

cint_stat_pp_egress_metadata_config_info_s cint_stat_pp_egress_metadata_enc_info_ipv6 =
{
        bcmStatPpMetadataHeaderTypeIpv6,
        2,
};

cint_stat_pp_egress_metadata_config_info_s cint_stat_pp_egress_metadata_enc_info_mpls =
{
        bcmStatPpMetadataHeaderTypeMpls,
        3,
};

cint_stat_pp_egress_metadata_config_info_s cint_stat_pp_egress_metadata_fwd_info_eth =
{
        bcmStatPpMetadataHeaderTypeEth,
        1,
};

cint_stat_pp_config_info_s egress_metadata_statistics_config =
{
        /**database_id */
        1,
        /**crps_engine_id */
        1,
        /**pp_profile */
        1,
        /**counter_command */
        1,
        /**crps_start */
        0,
        /**crps_end */
        100,
        /**type_id*/
        0,
        /**stat_id*/
        1,
        /**expansion_flag*/
        EXPANSION_TYPE_META_DATA_FULL_4_BITS
};

/**Example for etpp enc metadata - mpls lif statistics
 * Counting is on mpls lif with next layer == ipv4*/
int
basic_example_egress_metadata_enc_stat_pp(
    int unit,
    int port1,
    int port2,
    int mpls_label)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;
    bcm_stat_pp_info_t stat_info;
    bcm_stat_pp_profile_info_t pp_profile;
    bcm_stat_pp_metadata_info_t metadata_info;
    int pp_profile_id;
    uint32 entry_handle_id;
    int out_label=4000;

    init_default_mpls_params(unit, port1, port2);

    /** fec pointer is following the mpls first encap tunnel, which will be created later */
    mpls_util_entity[0].fecs[0].tunnel_gport = &mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;

    /** Configure mpls tunnel */
    mpls_util_entity[0].mpls_encap_tunnel[0].label[0] = mpls_label;
    mpls_util_entity[0].mpls_encap_tunnel[0].encap_access = bcmEncapAccessTunnel1;
    mpls_util_entity[0].mpls_encap_tunnel[0].l3_intf_id = &mpls_util_entity[1].arps[0].arp;
    mpls_util_entity[0].mpls_encap_tunnel[0].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[0].flags |= (BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_STAT_ENABLE);
    mpls_util_entity[0].mpls_encap_tunnel[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    mpls_util_entity[0].mpls_encap_tunnel[0].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    /** MPLS LSR into tunnel */
    mpls_util_entity[0].mpls_switch_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_SWAP;
    mpls_util_entity[0].mpls_switch_tunnel[0].egress_label = out_label;

    /** Create the mpls tunnels */
    rv = mpls_util_main(unit, port1, port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in mpls_util_main\n", rv);
        return rv;
    }

    /** Configure statistics for the mpls lif */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id);
    rv = cint_stat_pp_config_main(unit, egress_metadata_statistics_config, bcmStatCounterInterfaceEgressTransmitPp, gport, BCM_CORE_ALL, TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("egress_metadata_statistics_config failed for etps_1_statistics_config\n");
        return rv;
    }

    /* Layertype: IPv4 */
    metadata_info.flags = BCM_STAT_PP_METADATA_EGRESS;
    metadata_info.flags |= BCM_STAT_PP_METADATA_TUNNEL;
    metadata_info.header_type = cint_stat_pp_egress_metadata_enc_info_ipv4.header_type;
    metadata_info.metadata = cint_stat_pp_egress_metadata_enc_info_ipv4.egress_metadata_val;

    /**Configure metadata mapping*/
    rv = bcm_stat_pp_metadata_set(unit, &metadata_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_stat_pp_metadata_set \n");
    }

    /* Layertype: IPv6 */
    metadata_info.flags = BCM_STAT_PP_METADATA_EGRESS;
    metadata_info.flags |= BCM_STAT_PP_METADATA_TUNNEL;
    metadata_info.header_type = cint_stat_pp_egress_metadata_enc_info_ipv6.header_type;
    metadata_info.metadata = cint_stat_pp_egress_metadata_enc_info_ipv6.egress_metadata_val;

    /**Configure metadata mapping*/
    rv = bcm_stat_pp_metadata_set(unit, &metadata_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_stat_pp_metadata_set \n");
    }

    /* Layertype: MPLS */
    metadata_info.flags = BCM_STAT_PP_METADATA_EGRESS;
    metadata_info.flags |= BCM_STAT_PP_METADATA_TUNNEL;
    metadata_info.header_type = cint_stat_pp_egress_metadata_enc_info_mpls.header_type;
    metadata_info.metadata = cint_stat_pp_egress_metadata_enc_info_mpls.egress_metadata_val;

    /**Configure metadata mapping*/
    rv = bcm_stat_pp_metadata_set(unit, &metadata_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_stat_pp_metadata_set \n");
    }

    return rv;
}

/**Example for etpp fwd metadata - vlan statistics
 * Counting is on egress vsi with eth forwarding*/
int
basic_example_egress_metadata_fwd_stat_pp(
    int unit,
    int vid)
{
    int rv = BCM_E_NONE;
    bcm_vlan_t vlan;
    bcm_vlan_control_vlan_t vsi_control;
    bcm_stat_pp_metadata_info_t metadata_info;
    uint32 entry_handle_id;

    /** Configure statistics for the egress vsi*/
    egress_metadata_statistics_config.stat_id = vid;
    /**Configure crps and reserve metadata buffer space*/
    rv = cint_stat_pp_config_main(unit, egress_metadata_statistics_config, bcmStatCounterInterfaceEgressTransmitPp, 0, BCM_CORE_ALL, FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("egress_metadata_statistics_config failed for etps_1_statistics_config\n");
        return rv;
    }

    /**Create vlan with statistics*/
    vlan = vid;
    vsi_control.forwarding_vlan = vid;

    vsi_control.egress_stat_pp_profile = egress_metadata_statistics_config.pp_profile;
    vsi_control.egress_stat_id = egress_metadata_statistics_config.stat_id;
    rv = bcm_vlan_control_vlan_set(unit, vlan, vsi_control);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_vlan_control_vlan_set \n");
    }

    metadata_info.flags = BCM_STAT_PP_METADATA_EGRESS;
    metadata_info.flags |= BCM_STAT_PP_METADATA_FORWARD;
    metadata_info.address_type_flags = BCM_STAT_PP_METADATA_ADDRESS_TYPE_UCAST;
    metadata_info.header_type = cint_stat_pp_egress_metadata_fwd_info_eth.header_type;
    metadata_info.metadata = cint_stat_pp_egress_metadata_fwd_info_eth.egress_metadata_val;

    /**Configure metadata mapping*/
    rv = bcm_stat_pp_metadata_set(unit, &metadata_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_stat_pp_metadata_set \n");
    }

    return rv;
}


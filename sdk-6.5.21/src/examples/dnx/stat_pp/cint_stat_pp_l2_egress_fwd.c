/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_stat_pp_l2_egress_fwd.c Purpose: Statistics examples for SDK egress APIs using an mpls encapsulation.
 */

/*
 * File: cint_stat_pp_l2_egress_fwd.c
 * Purpose: Example of egress vsi fwd statistics collection
 *
 * Configure statistics on egress vsi
 *
 * The cint includes:
 * - Main function to configure egress vsi with statistics enabled along the way
 * - Structs with configurations used to configure the statistics profiles (crps and pp)
 * - A struct with mpls encapsulation required configuration
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
 * cint ../../src/examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_erpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint ../../src/examples/src/examples/dnx/stat_pp/cint_stat_pp_l2_egress_fwd.c
 * cint
 * cint_stat_pp_l2_egress_fwd_example(0,10);
 * exit;
 *
 *
 * Sent packet:
 *tx 1 psrc=201 data=0000000012340000000000018100000508004500003500000000800026C40A0000050A000001000102030405060708090A0B
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0000000012340000000000018100000508004500003500000000800026C40A0000050A000001000102030405060708090A0B
 * CRPS verify: 1 packet received on egress vsi
 */

cint_stat_pp_config_info_s ingress_vsi_statistics_config =
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
        1,
        /**tunnel_metadata_size*/
        3,
        /**tunnel_metadata_shift*/
        0
};

cint_stat_pp_config_info_s egress_vsi_statistics_config =
{
        /**database_id */
        2,
        /**crps_engine_id */
        2,
        /**pp_profile */
        2,
        /**counter_command */
        2,
        /**crps_start */
        0,
        /**crps_end */
        100,
        /**type_id*/
        0,
        /**stat_id*/
        2,
        /**expansion_flag*/
        1
};

int
cint_stat_pp_l2_egress_fwd_example(
    int unit,
    int vid)
{
    int rv = BCM_E_NONE;
    int is_lif = 0;
    bcm_vlan_t vlan;
    bcm_vlan_control_vlan_t vsi_control;
    bcm_vlan_t ingress_vlan;
    bcm_vlan_control_vlan_t ingress_vsi_control;


    /** Counfigure statistics for the egress vsi */
    is_lif = 0;
    egress_vsi_statistics_config.stat_id = vid;
    rv = cint_stat_pp_config_main(unit, egress_vsi_statistics_config, bcmStatCounterInterfaceEgressTransmitPp, 0, BCM_CORE_ALL, is_lif);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for egress_vsi_statistics_config\n");
    }

    /** Counfigure statistics for the ingress vsi */
    rv = cint_stat_pp_config_main(unit, ingress_vsi_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, is_lif);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for ingress_vsi_statistics_config\n");
    }

    vlan = vid;
    vsi_control.forwarding_vlan = vid;

    vsi_control.egress_stat_pp_profile = egress_vsi_statistics_config.pp_profile;
    vsi_control.egress_stat_id = egress_vsi_statistics_config.stat_id;
    vsi_control.ingress_stat_pp_profile = ingress_vsi_statistics_config.pp_profile;
    vsi_control.ingress_stat_id = ingress_vsi_statistics_config.stat_id;
    rv = bcm_vlan_control_vlan_set(unit, vlan, vsi_control);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_vlan_control_vlan_set \n");
    }


    return rv;
}

/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_stat_pp_port_ingress.c Purpose: Statistics examples for SDK ingress APIs using PP port object.
 */

/*
 * File: cint_stat_pp_port_ingress.c
 * Purpose: Example of ingress PP port statistics
 *
 * Configure statistics on In-PP-Port
 *
 * The cint includes:
 * - Main function to configure ip route with statistics enabled along the way
 * - Structs with configurations used to configure the statistics profiles (crps and pp)
 * - Configure pp port statistics command generation
 *
 * Usage:
 * ------
 *
 * Test Scenario:
 *--------------
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/examples/dnx/crps/cint_crps_irpp_config.c
 * cint
 * cint_stat_pp_port_ingress_example(0,0,201);
 * exit;
 *
 * Sent packet:
 * tx 1 psrc=201
 *
 * CRPS verify: 1 packet received
 */

/*
 * Statistics configurations (see src/examples/dnx/stat_pp/cint_stat_pp_config.c)
 */
cint_stat_pp_config_info_s crps_statistics_config =
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
        800,
        /**type_id*/
        0,
        /**stat_id*/
        50,
        /**expansion_flag*/
        5
};

/*
 * main function.
 * Calls:
 * cint_stat_pp_config_crps()
 * cint_stat_pp_create_pp_profile()
 * Configure pp port statistics - bcm_gport_stat_set
 */
int
cint_stat_pp_port_ingress_example(
    int unit,
    int in_core,
    int in_port)
{
    bcm_gport_t local_gport = 0;
    bcm_stat_pp_profile_info_t pp_profile;
    bcm_stat_pp_info_t stat_info; 
    int pp_profile_id;
    int rv = 0;

    if (crps_statistics_config.pp_profile == 0)
    {
        printf("Invalid stat pp profile, not configuring...\n");
        return rv;
    }

    /*
     * Configure crps database
     */
    rv = cint_stat_pp_config_crps(unit, crps_statistics_config, bcmStatCounterInterfaceIngressReceivePp, in_core);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_config_crps\n");
        return rv;
    }

    /*
     * Configure pp profile
     */
    rv = cint_stat_pp_create_pp_profile(unit, crps_statistics_config, bcmStatCounterInterfaceIngressReceivePp, &pp_profile, &pp_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_create_pp_profile\n");
        return rv;
    }

    /*
     * Configure statistics command for the pp port
     */
    stat_info.stat_pp_profile = pp_profile_id;
    stat_info.stat_id = crps_statistics_config.stat_id;
    if (BCM_GPORT_IS_LOCAL(in_port))
    {
        BCM_GPORT_LOCAL_SET(local_gport, in_port);
        rv = bcm_gport_stat_set(unit, local_gport, in_core, bcmStatCounterInterfaceIngressReceivePp, stat_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_gport_stat_set\n");
            return rv;
        }
    } else {
        rv = bcm_gport_stat_set(unit, in_port, in_core, bcmStatCounterInterfaceIngressReceivePp, stat_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_gport_stat_set\n");
            return rv;
        }
    }
   
    return rv;
}

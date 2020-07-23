/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_stat_pp_port_ingress_meter_crps.c Purpose: Statistics examples for SDK ingress APIs using PP port object.
 */

/*
 * File: cint_stat_pp_port_ingress_meter_crps.c
 * Purpose: Example of ingress PP port statistics
 *
 * Configure statistics on In-PP-Port
 *
 * The cint includes:
 * - Structs with configurations used to configure the statistics profiles (crps, mrps and pp)
 * - Configure pp port statistics command generation - create two profiles and map two profiles, use crps and meter databases
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
 * cint ../../src/examples/dnx/policer/cint_policer_generic_meter.c
 * cint
 * cint_stat_pp_port_ingress_meter_crps_example(0,1,201,202);
 * exit;
 *
 * Sent packet:
 * tx 1 psrc=201
 * tx 1 psrc=202
 *
 * CRPS verify: 1 packet received, via signals - correct object stat id
 * Meter verify: via signals - correct object stat id
 */

struct cint_stat_pp_config_meter_s
{
    int database_id;        /* meter Id for the database that will be used when reading a counter*/
    int meter_engine_id;     /* meter engine id*/
    int pp_profile;         /* profile of the counted object*/
    int is_ingress;         /* is ingress or egress database*/
    int base_pointer;         /* start of meter range */
    int single_bucket_mode;   /* select the mode - single_bucket_mode*/
    int expansion_enable;     /* enable meter expansion*/
    int total_nof_meters;     /* total number of meters*/
    int configure_meters;     /* configure automatically the meters or manually*/
    int stat_id;                      /* stat id*/
};

cint_stat_pp_config_meter_s meter_statistics_config =
{
    /** database_id */
    0,
    /** meter engine id */
    10,
    /** pp_profile */
    2,
    /** is ingress */
    1,
    /** base_pointer */
    0,
    /** single/dual bucket mode */
    0,
    /** expansion */
    0,
    /** total nof meters */
    1024,
    /** configure meters */
    1,
    /** stat id */
    10
};


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
        5,
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
 * configure meter database
 * Calls:
 * policer_generic_meter_database_example - create meter database
 */
int
cint_stat_pp_config_meter(
    int unit,
    int core_id,
    cint_stat_pp_config_meter_s cint_stat_pp_config_meter_info)
{
    int rv = 0;
    /*
     * Configure meter database
     */
    rv = policer_generic_meter_database_example(unit,
                                                                                      core_id,
                                                                                      cint_stat_pp_config_meter_info.is_ingress,
                                                                                      cint_stat_pp_config_meter_info.database_id,
                                                                                      cint_stat_pp_config_meter_info.base_pointer,
                                                                                      cint_stat_pp_config_meter_info.single_bucket_mode,
                                                                                      cint_stat_pp_config_meter_info.expansion_enable,
                                                                                      cint_stat_pp_config_meter_info.total_nof_meters,
                                                                                      cint_stat_pp_config_meter_info.configure_meters,
                                                                                      cint_stat_pp_config_meter_info.meter_engine_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_config_meter\n");
        return rv;
    }

    return rv;
}

/*
 * Configure pp statistic profile for meter database - pp meter enable
 */
int
cint_stat_pp_create_pp_profile_meter(
    int unit,
    cint_stat_pp_config_meter_s cint_stat_pp_config_meter_info,
    bcm_stat_pp_profile_info_t *pp_profile,
    int *pp_profile_id)
{
    int rv;
    int flags = BCM_STAT_PP_PROFILE_WITH_ID;

    /** Init struct */
    bcm_stat_pp_profile_info_t_init(pp_profile);

    pp_profile->counter_command_id = cint_stat_pp_config_meter_info.database_id;
    pp_profile->is_meter_enable = 1;
    *pp_profile_id = cint_stat_pp_config_meter_info.pp_profile;

    rv = bcm_stat_pp_profile_create(unit, flags, bcmStatCounterInterfaceIngressReceivePp, *pp_profile_id, pp_profile);
    return rv;
}

/*
 * main function - create two databases - crps and meter; create two profiles mapped to two ports
 * Calls:
 * cint_stat_pp_config_crps()
 * cint_stat_pp_create_pp_profile()
 *
 * cint_stat_pp_config_meter()
 * cint_stat_pp_create_pp_profile_meter()
 * Configure pp port statistics - bcm_gport_stat_set
 */
int
cint_stat_pp_port_ingress_meter_crps_example(
    int unit,
    int core,
    int port_crps,
    int port_meter)
{
    bcm_gport_t local_gport = 0;
    bcm_stat_pp_profile_info_t pp_profile_crps, pp_profile_meter;
    bcm_stat_pp_info_t stat_info_crps, stat_info_meter;
    int pp_profile_id_crps, pp_profile_id_meter;
    int rv = 0;

    if (crps_statistics_config.pp_profile == 0)
    {
        printf("Invalid stat pp profile, not configuring...\n");
        return rv;
    }

    if (meter_statistics_config.pp_profile == 0)
    {
        printf("Invalid stat pp profile, not configuring...\n");
        return rv;
    }

    printf("Configure CRPS database and PP profile\n");
    /*
     * Configure crps database
     */
    rv = cint_stat_pp_config_crps(unit, crps_statistics_config, bcmStatCounterInterfaceIngressReceivePp, core);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_config_crps\n");
        return rv;
    }

    /*
     * Configure pp profile
     */
    rv = cint_stat_pp_create_pp_profile(unit, crps_statistics_config, bcmStatCounterInterfaceIngressReceivePp, &pp_profile_crps, &pp_profile_id_crps);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_create_pp_profile\n");
        return rv;
    }

    /*
     * Configure statistics command for the pp port
     */
    stat_info_crps.stat_pp_profile = pp_profile_id_crps;
    stat_info_crps.stat_id = crps_statistics_config.stat_id;
    BCM_GPORT_LOCAL_SET(local_gport, port_crps);
    rv = bcm_gport_stat_set(unit, local_gport, core, bcmStatCounterInterfaceIngressReceivePp, stat_info_crps);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_gport_stat_set\n");
        return rv;
    }

    printf("Configure meter database and PP profile\n");
    /*
     * Configure meter engine id to attach to the meter database
     */
    meter_statistics_config.meter_engine_id = *dnxc_data_get(unit, "meter", "mem_mgmt", "base_big_engine_for_meter", NULL);
    /*
     * Configure meter database
     */
    rv = cint_stat_pp_config_meter(unit, core, meter_statistics_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_config_meter\n");
        return rv;
    }

    /*
     * Configure pp profile
     */
    rv = cint_stat_pp_create_pp_profile_meter(unit, meter_statistics_config, &pp_profile_meter, &pp_profile_id_meter);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_create_pp_profile_meter\n");
        return rv;
    }

    /*
     * Configure statistics command for the pp port
     */
    stat_info_meter.stat_pp_profile = pp_profile_id_meter;
    stat_info_meter.stat_id = meter_statistics_config.stat_id;
    BCM_GPORT_LOCAL_SET(local_gport, port_meter);
    rv = bcm_gport_stat_set(unit, local_gport, core, bcmStatCounterInterfaceIngressReceivePp, stat_info_meter);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_gport_stat_set\n");
        return rv;
    }

    return rv;
}


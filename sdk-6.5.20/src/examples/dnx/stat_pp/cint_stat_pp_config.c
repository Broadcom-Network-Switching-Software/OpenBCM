/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_stat_pp_config.c
 * Purpose:     utility functions to configure statistics on fwd, lif APIs
 *
 *
 * The cint includes:
 *  - Main function to configure statistics on fwd\lif API.
 *  - The main function configures the crps, stat_pp and gport in case of lif
 *  - A struct with default configuration
 *
 * Usage:
 * ------
 * cint examples/sand/utility/cint_sand_utils_global.c
 * cint examples/sand/cint_ip_route_basic.c
 * cint examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint examples/sand/utility/cint_sand_utils_pwe.c
 * cint examples/sand/utility/cint_sand_utils_global.c
 * cint examples/dnx/crps/cint_crps_db_utils.c
 * cint examples/dnx/crps/cint_crps_irpp_config.c
 * cint examples/dnx/crps/cint_crps_erpp_config.c
 * cint examples/dnx/crps/cint_crps_etpp_config.c
 * rv = cint_stat_pp_config_main(int unit, cint_stat_pp_config_info_s cint_stat_pp_config_info, bcm_stat_counter_interface_type_t engine_source, bcm_gport_t gport, int core_id, int is_lif);
 *
 * Calling sequence:
 * 1. cint_stat_pp_config_crps() - configure crps profile
 * 2. cint_stat_pp_create_pp_profile() - Configure stat pp profile
 * 3. In case of lif statistics - call bcm_gport_stat_set()
 *
 * Important note:
 * ---------------
 * In case of lif statistics, the call to bcm_gport_stat_set() should be done with the encoded gport.
 * Therefore, instead of calling to cint_stat_pp_config_main() directly, one can simply call each API in the correct order:
 * 1. cint_stat_pp_config_crps()
 * 2. cint_stat_pp_create_pp_profile()
 * 3. Create and encode the lif (for example: bcm_dnx_mpls_port_add())
 * 4. call bcm_gport_stat_set()
 */

struct cint_stat_pp_config_info_s
{
    int database_id;        /* crps Id for the database that will be used when reading a counter*/
    int crps_engine_id;     /* crps engine id*/
    int pp_profile;         /* profile of the counted object*/
    int counter_command;    /* counter command id*/
    int crps_start;         /* crps start range array per type*/
    int crps_end;           /* crps end range array per type_id*/
    int type_id;            /* Used in crps to distinguish between different segments on the same engine*/
    int stat_id;            /* Counted object id, counter pointer sent to the crps*/
    int expansion_flag;     /* crps, type of data stored in engine*/
    int tunnel_metadata_size;     /* crps , size of tunnel metadata*/
    int tunnel_metadata_shift;     /* crps, tunnel metada shift in bits in IRPP metadata*/
    int is_protection_fec;         /*Indicate that this profile might serve a protection fec*/
};

cint_stat_pp_config_info_s cint_stat_pp_config_info_default =
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
    1,
    /**expansion_flag*/
    5,
    /**is_protection_fec*/
    0
};

/*
 * Configure the counting engine profile
 * Call the relevant crps wrapper based on the engine_source
 */
int
cint_stat_pp_config_crps(
   int unit,
   cint_stat_pp_config_info_s cint_stat_pp_config_info,
   bcm_stat_counter_interface_type_t engine_source,
   int core_id)
{
    int rv;

    switch (engine_source)
    {
    case bcmStatCounterInterfaceIngressReceivePp:
        rv = crps_irpp_data_base_create_wrapper(unit,
                                                core_id,
                                                cint_stat_pp_config_info.database_id,
                                                cint_stat_pp_config_info.counter_command,
                                                cint_stat_pp_config_info.type_id,
                                                cint_stat_pp_config_info.crps_start,
                                                cint_stat_pp_config_info.crps_end,
                                                cint_stat_pp_config_info.crps_engine_id,
                                                cint_stat_pp_config_info.expansion_flag);
        break;
    case bcmStatCounterInterfaceEgressReceivePp:
        
        printf("Unsupported engine - ERPP\n");
        /*rv = crps_erpp_data_base_create_wrapper(unit,
                                                core_id,
                                                cint_stat_pp_config_info.database_id,
                                                cint_stat_pp_config_info.pp_profile,
                                                cint_stat_pp_config_info.type_id,
                                                cint_stat_pp_config_info.crps_start,
                                                cint_stat_pp_config_info.crps_end,
                                                cint_stat_pp_config_info.crps_engine_id,
                                                cint_stat_pp_config_info.expansion_flag);*/
        break;
    case bcmStatCounterInterfaceEgressTransmitPp:
        rv = crps_etpp_data_base_create_wrapper(unit,
                                                core_id,
                                                cint_stat_pp_config_info.database_id,
                                                cint_stat_pp_config_info.counter_command,
                                                cint_stat_pp_config_info.type_id,
                                                cint_stat_pp_config_info.crps_start,
                                                cint_stat_pp_config_info.crps_end,
                                                cint_stat_pp_config_info.crps_engine_id,
                                                cint_stat_pp_config_info.expansion_flag);
        break;
    default:
        printf("engine source not found\n");
        return 1;
        break;
    }
    if (rv != BCM_E_NONE)
    {
        printf("crps wrapper failed\n");
    }
    return rv;
}

/*
 * Configure pp statistic profile
 */
int
cint_stat_pp_create_pp_profile(
    int unit,
    cint_stat_pp_config_info_s cint_stat_pp_config_info,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_profile_info_t *pp_profile,
    int *pp_profile_id)
{
    int rv;
    int flags = BCM_STAT_PP_PROFILE_WITH_ID;
    
    /** Init struct */
    bcm_stat_pp_profile_info_t_init(pp_profile);

    pp_profile->counter_command_id = cint_stat_pp_config_info.counter_command;

    if (engine_source == bcmStatCounterInterfaceIngressReceivePp)
    {
        pp_profile->ingress_tunnel_metadata_size = cint_stat_pp_config_info.tunnel_metadata_size;
        pp_profile->ingress_tunnel_metadata_start_bit = cint_stat_pp_config_info.tunnel_metadata_shift;
    }

    *pp_profile_id = cint_stat_pp_config_info.pp_profile;

    pp_profile->is_protection_fec = cint_stat_pp_config_info.is_protection_fec;
    rv = bcm_stat_pp_profile_create(unit, flags, engine_source, *pp_profile_id, pp_profile);

    return rv;
}

/*
 * main function.
 * Calls:
 * cint_stat_pp_config_crps()
 * cint_stat_pp_create_pp_profile()
 * bcm_gport_stat_set()
 */
int
cint_stat_pp_config_main(
   int unit,
   cint_stat_pp_config_info_s cint_stat_pp_config_info,
   bcm_stat_counter_interface_type_t engine_source,
   bcm_gport_t gport,
   int core_id,
   int is_lif)
{
    bcm_stat_pp_profile_info_t pp_profile;
    bcm_stat_pp_info_t stat_info; 
    int pp_profile_id;
    int rv = 0;

    if (cint_stat_pp_config_info.pp_profile == 0)
    {
        printf("Invalid stat pp profile, not configuring...\n");
        return rv;
    }

    /*
     * Configure crps entry
     */
    rv = cint_stat_pp_config_crps(unit, cint_stat_pp_config_info, engine_source, core_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_config_crps\n");
        return rv;
    }

    /*
     * Configure pp profile
     */
    rv = cint_stat_pp_create_pp_profile(unit, cint_stat_pp_config_info, engine_source, &pp_profile, &pp_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_create_pp_profile\n");
        return rv;
    }

    /*
     * if the object is lif - call gport_stat_set()
     */
    if (is_lif)
    {
        stat_info.stat_pp_profile = pp_profile_id;
        stat_info.stat_id = cint_stat_pp_config_info.stat_id;
        rv = bcm_gport_stat_set(unit, gport, core_id, engine_source, stat_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error in cint_stat_pp_create_gport_stat_object\n");
            return rv;
        }
    }
    return rv;
}

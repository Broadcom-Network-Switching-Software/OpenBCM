/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_vpls_statistics.c Purpose: Statistic configuration for vpls (pwe port, pwe lif, ac).
 */

/*
 * File: cint_vpls_statistics.c
 * Purpose: Example of vpls statistics pp count configuration
 *
 * Configure statistics on pwe port, pwe lif, ac
 *
 * The cint includes:
 * - Main function to configure statistics on pwe lif, pwe port and ac physical port
 * - A struct with configurations used to configure the statistics profiles (crps and pp)
 *
 * Usage:
 * ------
 *
 * Test Scenario:
 *--------------
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_pwe.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/examples/dnx/vpls/cint_vpls_statistics.c
 * cint ../../src/examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_erpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint
 * cint_vpls_statistics_main(0,200,201);
 * exit;
 *
 * 1: pwe -> ac
 * tx 1 psrc=201 data=0x000c0002000100000000cd1d8100001e884700d0504000d8014000110000011200010203040581000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x00110000011200010203040581000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on pwe lif statistic interface, 1 packet received on pwe port statistic interface
 *
 * tx 1 psrc=200 data=0x000c0002000000110000011281000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x00000000cd1d000c000200018100001e884700d050fe00d801fe000c0002000000110000011281000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * CRPS verify: 1 packet received on ac port statistic interface
 *
 * This cint is used in order to configure statistics counting on pwe lif, pwe port and ac port.
 * The cint calls vpls_main() cint API in order to configure the ports and lifs.
 * It calls cint_stat_pp_config_main() with the relevant configuration params for the fwd APIs (ac, pwe ports).
 * For pwe lif - it calls cint_stat_pp_config.c APIs directly, since the call to bcm_gport_stat_set(), which is part of
 * the cint_stat_pp_config_main() function, should be done after creating and encoding the pwe lif, while the creation of the lif
 * is going through crps validation and therefore should be done after configuring the counting engine.
 */

struct vpls_statistics_properties_s
{
    int stat_id;
    int pp_profile_id;
    int counter_command;
    int engine_id;
    int database_id;
};

vpls_statistics_properties_s vpls_pwe_lif_statistics_properties =
{
    /**stat_id */
    1,
    /**pp_profile_id*/
    1,
    /**counter_command*/
    1,
	/**engine_id*/
	1,
	/**database_id*/
	1
};

vpls_statistics_properties_s vpls_ac_statistics_properties =
{
    /**stat_id */
    4,
    /**pp_profile_id*/
    8,
    /**counter_command*/
    8,
	/**engine_id*/
	4,
	/**database_id*/
	4
};

vpls_statistics_properties_s vpls_pwe_statistics_properties =
{
    /**stat_id */
    7,
    /**pp_profile_id*/
    6,
    /**counter_command*/
    6,
	/**engine_id*/
	7,
	/**database_id*/
	7
};

/*
 * main function.
 * Calls:
 * init_default_vpls_params()
 * cint_stat_pp_config_main()
 * vpls_main()
 * cint_stat_pp_config_crps()
 * cint_stat_pp_create_pp_profile()
 * bcm_gport_stat_set()
 */
int
cint_vpls_statistics_main(
   int unit,
   int ac_p,
   int pwe_p)
{
    int rv;
    bcm_stat_pp_profile_info_t pp_profile;
    bcm_stat_pp_info_t stat_info;
    int pp_profile_id;

    /* When JR2 works under IOP mode, use FEC format:dest+EEI */
    if ((soc_property_get(unit , "system_headers_mode",1) == 0)) {
        mpls_tunnel_in_eei = 1;
    }

    /** init vpls params for cint */
    rv = init_default_vpls_params(unit);
    if (rv != BCM_E_NONE)
    {
        printf("init_default_vpls_params failed\n");
        return rv;
    }

    /** add statistics flag to enable counting */
    pwe_term.flags2 |= BCM_MPLS_PORT2_STAT_ENABLE;

    /** Update ac_port, pwe_port stat id and pp_profile for cint_dnx_utils_vpls.c cint */
    ac_port.stat_id = vpls_ac_statistics_properties.stat_id;
    ac_port.stat_pp_profile = vpls_ac_statistics_properties.pp_profile_id;
    pwe_port.stat_id = vpls_pwe_statistics_properties.stat_id;
    pwe_port.stat_pp_profile = vpls_pwe_statistics_properties.pp_profile_id;

    /** ac port statistics configuration */
    cint_stat_pp_config_info_default.pp_profile = vpls_ac_statistics_properties.pp_profile_id;
    cint_stat_pp_config_info_default.counter_command = vpls_ac_statistics_properties.counter_command;
    cint_stat_pp_config_info_default.crps_engine_id = vpls_ac_statistics_properties.engine_id;
    cint_stat_pp_config_info_default.database_id = vpls_ac_statistics_properties.database_id;
    cint_stat_pp_config_info_default.stat_id = vpls_ac_statistics_properties.stat_id;
    rv = cint_stat_pp_config_main(unit, cint_stat_pp_config_info_default, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed\n");
        return rv;
    }

    /** pwe port statistics configuration */
    cint_stat_pp_config_info_default.pp_profile = vpls_pwe_statistics_properties.pp_profile_id;
    cint_stat_pp_config_info_default.counter_command = vpls_pwe_statistics_properties.counter_command;
    cint_stat_pp_config_info_default.crps_engine_id = vpls_pwe_statistics_properties.engine_id;
    cint_stat_pp_config_info_default.database_id = vpls_pwe_statistics_properties.database_id;
    cint_stat_pp_config_info_default.stat_id = vpls_pwe_statistics_properties.stat_id;
    rv = cint_stat_pp_config_main(unit, cint_stat_pp_config_info_default, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed\n");
        return rv;
    }

    /** pwe lif statistics configuration */
    cint_stat_pp_config_info_default.pp_profile = vpls_pwe_lif_statistics_properties.pp_profile_id;
    cint_stat_pp_config_info_default.counter_command = vpls_pwe_lif_statistics_properties.counter_command;
    cint_stat_pp_config_info_default.crps_engine_id = vpls_pwe_lif_statistics_properties.engine_id;
    cint_stat_pp_config_info_default.database_id = vpls_pwe_lif_statistics_properties.database_id;
    cint_stat_pp_config_info_default.stat_id = vpls_pwe_lif_statistics_properties.stat_id;

    /** For pwe lif, since the lif id is known only after the lif's creation, we first need to configure the crps with the pwe lif statistic IDs, then we need to create the lif and finally attach it to the pp profile */
    /*
     * Configure crps entry
     */
    rv = cint_stat_pp_config_crps(unit, cint_stat_pp_config_info_default, bcmStatCounterInterfaceIngressReceivePp, BCM_CORE_ALL);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_config_crps\n");
        return rv;
    }

    /** Call vpls cint to create vpls, configure ports and lifs */
    rv = vpls_main(unit, ac_p, pwe_p);
    if (rv != BCM_E_NONE)
    {
        printf("vpls_main failed\n");
        return rv;
    }

    /** Configure pp profile */
    rv = cint_stat_pp_create_pp_profile(unit, cint_stat_pp_config_info_default, bcmStatCounterInterfaceIngressReceivePp, &pp_profile, &pp_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_create_pp_profile\n");
        return rv;
    }

    /** configure statistics on the pwe lif, now that the gport is encoded */
    stat_info.stat_pp_profile = pp_profile_id;
    stat_info.stat_id = vpls_pwe_lif_statistics_properties.stat_id;
    rv = bcm_gport_stat_set(unit, pwe_term.mpls_port_id, BCM_CORE_ALL, bcmStatCounterInterfaceIngressReceivePp, stat_info);
    if (rv != BCM_E_NONE)
    {
         printf("Error in cint_stat_pp_create_gport_stat_object\n");
    }

    return rv;
}

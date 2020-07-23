/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_stat_pp_ipv6_ipmc.c Purpose: Statistics examples for SDK ingress APIs using an ipv6 ipmc application.
 */

/*
 * File: cint_stat_pp_ipv6_ipmc.c
 * Purpose: Example of multiple ingress PP statistics
 *
 * Add two ipv6 ipmc entries: one is in LEM lookup, another is TCAM lookup.
 *
 * The cint includes:
 * - Main function to configure ipv6 ipmc with statistics enabled along the way
 * - Structs with configurations used to configure the statistics profiles (crps and pp)
 * - A struct with ipv6 ipmc required configuration
 *
 * Usage:
 * ------
 *
 * Test Scenario:
 *--------------
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ipmc_route_basic.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_ipv6_ipmc.c
 * cint
 * cint_stat_pp_ipv6_ipmc_example(0,0xCC,200,201,202);
 * exit;
 *
 * Sent packet:
 * tx 1 psrc=200 data=0x80c8333300001111000007000100810000cc86dd600000000049068000000011000000000000333322221111ffff0000000000000000000000001111000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 *
 * Received packets on unit 0 should be:
 * Source port: 200, Destination port: 201
 * Data: 0x33330000111100123456789a8100010086dd600000000049067f00000011000000000000333322221111ffff0000000000000000000000001111000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Source port: 200, Destination port: 202
 * Data: 0x33330000111100123456789b8100010186dd600000000049067f00000011000000000000333322221111ffff0000000000000000000000001111000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on ipv6 ipmc entry
 */

/*
 * Statistics configurations (see src/examples/dnx/stat_pp/cint_stat_pp_config.c)
 */
cint_stat_pp_config_info_s ipmc_1_statistics_config =
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
        1
};

cint_stat_pp_config_info_s ipmc_2_statistics_config =
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

/*
 * main function.
 * Calls:
 * cint_stat_pp_config_crps()
 * cint_stat_pp_create_pp_profile()
 * Configures ipv6 ipmc
 */
int
cint_stat_pp_ipv6_ipmc_example(
    int unit,
    int vid_in,
    int in_port,
    int out_port,
    int out_port2)
{
    int rv;
    bcm_ipmc_addr_t data;
    bcm_ip6_t full_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t part_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0};
    cint_ipmc_info.out_ports[0] = out_port;
    cint_ipmc_info.out_ports[1] = out_port2;

    if (!(soc_property_get(unit, "ext_ipv4_fwd_enable", 0) || soc_property_get(unit, "ext_ipv6_fwd_enable", 0)))
    {
        /** Configure statistics for LEM */
        rv = cint_stat_pp_config_main(unit, ipmc_1_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
        if (rv != BCM_E_NONE)
        {
            printf("cint_stat_pp_config_main failed for ipmc_1_statistics_config\n");
            return rv;
        }
    }

    /** Configure statistics for TCAM */
    rv = cint_stat_pp_config_main(unit, ipmc_2_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for ipmc_2_statistics_config\n");
        return rv;
    }

    /** Set up ipv6 ipmc service */
    rv = basic_ipmc_v6_example(unit, vid_in, cint_ipmc_info.in_port, cint_ipmc_info.out_ports[0], cint_ipmc_info.out_ports[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, basic_ipmc_v6_example\n");
        return rv;
    }

    if (!(soc_property_get(unit, "ext_ipv4_fwd_enable", 0) || soc_property_get(unit, "ext_ipv6_fwd_enable", 0)))
    {
        /** Replace ipv6 IPMC entry based on LEM */
        bcm_ipmc_addr_t_init(&data);
        cint_ipmc_info.ipv6_dip_addr[15] --;
        cint_ipmc_info.ipv6_dip_addr[15] --;
        data.mc_ip6_addr = cint_ipmc_info.ipv6_dip_addr;
        data.mc_ip6_mask = full_mask;
        data.vid        = cint_ipmc_info.intf_in;
        data.vrf        = cint_ipmc_info.vrf;
        data.group      = cint_ipmc_info.mc_group;
        data.stat_id    = ipmc_1_statistics_config.stat_id;
        data.stat_pp_profile = ipmc_1_statistics_config.pp_profile;
        data.flags      = BCM_IPMC_REPLACE | BCM_IPMC_IP6;
        rv = bcm_ipmc_add(unit, &data);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function bcm_ipmc_add to LEM table, \n");
            return rv;
        }
    }
    else
    {
        cint_ipmc_info.ipv6_dip_addr[15] --;
        cint_ipmc_info.ipv6_dip_addr[15] --;
    }

    if (soc_property_get(unit, "ext_ipv6_fwd_enable", 0))
    {
        /** Replace ipv6 IPMC entry based on TCAM */
        cint_ipmc_info.ipv6_dip_addr[15] ++;
        cint_ipmc_info.ipv6_dip_addr[15] ++;
        bcm_ipmc_addr_t_init(&data);
        data.mc_ip6_addr = cint_ipmc_info.ipv6_dip_addr;
        data.mc_ip6_mask = full_mask;
        data.s_ip6_addr  = cint_ipmc_info.ipv6_sip_addr;
        data.s_ip6_mask  = part_mask;
        data.vid        = cint_ipmc_info.intf_in;
        data.vrf        = cint_ipmc_info.vrf;
        data.group      = cint_ipmc_info.mc_group;
        data.stat_id    = ipmc_2_statistics_config.stat_id;
        data.stat_pp_profile = ipmc_2_statistics_config.pp_profile;
        data.flags      = BCM_IPMC_IP6;
	    if (kbp_fwd_enable_jr2)
	    {
	        /* JR2 requires flag BCM_IPMC_TCAM if external forwarding enabled */
	        data.flags |= BCM_IPMC_TCAM;
	    }
        rv = bcm_ipmc_remove(unit, &data);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function bcm_ipmc_remove to TCAM table, \n");
            return rv;
        }
        rv = bcm_ipmc_add(unit, &data);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function bcm_ipmc_add to TCAM table, \n");
            return rv;
        }
    }

    return rv;
}


/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_stat_pp_ipv4_ipmc.c Purpose: Statistics examples for SDK ingress APIs using an ipv4 ipmc application.
 */

/*
 * File: cint_stat_pp_ipv4_ipmc.c
 * Purpose: Example of multiple ingress PP statistics
 *
 * Add two ipv4 ipmc entries: one is in LEM lookup, another is TCAM lookup.
 *
 * The cint includes:
 * - Main function to configure ipv4 ipmc with statistics enabled along the way
 * - Structs with configurations used to configure the statistics profiles (crps and pp)
 * - A struct with ipv4 ipmc required configuration
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
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_ipv4_ipmc.c
 * cint
 * cint_stat_pp_ipv4_ipmc_example(0,0xCC,200,201,202);
 * exit;
 *
 * Sent packet:
 * tx 1 psrc=200 data=01005e020202000007000100810000cc0800450000350000000080009743c0800101e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 * Received packets on unit 0 should be:
 * Source port: 200, Destination port: 201
 * Data: 0x01005e02020200123456789a81000100080045000035000000007f009843c0800101e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c
 * Source port: 200, Destination port: 202
 * Data: 0x01005e02020200123456789b81000101080045000035000000007f009843c0800101e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c 
 * CRPS verify: 1 packet received on ipv4 ipmc entry
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
 * Configures ipv4 ipmc
 */
int
cint_stat_pp_kbp_ipv4_ipmc_example(
    int unit,
    int vid_in,
    int in_port,
    int out_port,
    int out_port2,
    int use_kbp)
{
    int rv;
    bcm_ipmc_addr_t data;
    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_part_mask = 0xFFFF0000;
    cint_ipmc_info.out_ports[0] = out_port;
    cint_ipmc_info.out_ports[1] = out_port2;

    /** Counfigure statistics for LEM */
    rv = cint_stat_pp_config_main(unit, ipmc_1_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for ipmc_1_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for TCAM */
    rv = cint_stat_pp_config_main(unit, ipmc_2_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for ipmc_2_statistics_config\n");
        return rv;
    }

    /** Set up ipv4 ipmc service */
    rv = basic_ipmc_example(unit, vid_in, cint_ipmc_info.in_port, cint_ipmc_info.out_ports[0], cint_ipmc_info.out_ports[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, basic_ipmc_example\n");
        return rv;
    }

    if (!use_kbp)
    {
        /** Replace IPv4 IPMC entry based on LEM */
        bcm_ipmc_addr_t_init(&data);
        data.mc_ip_addr = cint_ipmc_info.mc_host_dip;
        data.mc_ip_mask = dip_mask;
        data.vid        = cint_ipmc_info.intf_in;
        data.vrf        = cint_ipmc_info.vrf;
        data.group      = cint_ipmc_info.mc_group;
        data.stat_id    = ipmc_1_statistics_config.stat_id;
        data.stat_pp_profile = ipmc_1_statistics_config.pp_profile;
        data.flags      = BCM_IPMC_REPLACE;
        rv = bcm_ipmc_add(unit, &data);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function bcm_ipmc_add to LEM table, \n");
            return rv;
        }

        /** Replace IPv4 IPMC entry based on TCAM */
        bcm_ipmc_addr_t_init(&data);
        data.mc_ip_addr = cint_ipmc_info.mc_host_dip + 2;
        data.mc_ip_mask = dip_mask;
        data.s_ip_addr  = cint_ipmc_info.host_sip;
        data.s_ip_mask  = sip_part_mask;
        data.vid        = cint_ipmc_info.intf_in;
        data.vrf        = cint_ipmc_info.vrf;
        data.group      = cint_ipmc_info.mc_group;
        data.stat_id    = ipmc_2_statistics_config.stat_id;
        data.stat_pp_profile = ipmc_2_statistics_config.pp_profile;
        data.flags      = BCM_IPMC_REPLACE | BCM_IPMC_TCAM;
        rv = bcm_ipmc_add(unit, &data);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function bcm_ipmc_add to TCAM table, \n");
            return rv;
        }
    }
    else
    {
        /*
         * Replace IPv4 IPMC entry based on external TCAM.
         * The entry should be deleted first, because update will fail due to moving the payload from one AD DB to another.
         */
        bcm_ipmc_addr_t_init(&data);
        data.mc_ip_addr = cint_ipmc_info.mc_host_dip + 2;
        data.mc_ip_mask = dip_mask;
        data.s_ip_addr  = cint_ipmc_info.host_sip;
        data.s_ip_mask  = sip_part_mask;
        data.vid        = cint_ipmc_info.intf_in;
        data.vrf        = cint_ipmc_info.vrf;
        data.flags      = BCM_IPMC_TCAM;
        rv = bcm_ipmc_remove(unit, &data);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function bcm_ipmc_remove from external TCAM table, \n");
            return rv;
        }

        bcm_ipmc_addr_t_init(&data);
        data.mc_ip_addr = cint_ipmc_info.mc_host_dip + 2;
        data.mc_ip_mask = dip_mask;
        data.s_ip_addr  = cint_ipmc_info.host_sip;
        data.s_ip_mask  = sip_part_mask;
        data.vid        = cint_ipmc_info.intf_in;
        data.vrf        = cint_ipmc_info.vrf;
        data.group      = cint_ipmc_info.mc_group;
        data.stat_id    = ipmc_2_statistics_config.stat_id;
        data.stat_pp_profile = ipmc_2_statistics_config.pp_profile;
        data.flags      = BCM_IPMC_TCAM;
        rv = bcm_ipmc_add(unit, &data);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function bcm_ipmc_add to external TCAM table, \n");
            return rv;
        }
    }

    return rv;
}

/*
 * main function.
 * Calls:
 * cint_stat_pp_config_crps()
 * cint_stat_pp_create_pp_profile()
 * Configures ipv4 ipmc
 */
int
cint_stat_pp_ipv4_ipmc_example(
    int unit,
    int vid_in,
    int in_port,
    int out_port,
    int out_port2)
{
    int rv;

    rv = cint_stat_pp_kbp_ipv4_ipmc_example(unit, vid_in, in_port, out_port, out_port2, FLASE)

    return rv;
}

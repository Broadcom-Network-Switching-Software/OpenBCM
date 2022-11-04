/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_stat_pp_ipv6_ipmc.c Purpose: Statistics examples for SDK ingress APIs using an ipv6 ipmc application.
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
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_l3.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
  cint ../../../../src/examples/dnx/crps/cint_crps_db_utils.c
  cint ../../../../src/examples/dnx/crps/cint_crps_irpp_config.c
  cint ../../../../src/examples/dnx/stat_pp/cint_stat_pp_ipv6_ipmc.c
  cint
  cint_stat_pp_ipv6_ipmc_example(0,0xCC,200,201,202,0);
  exit;
 *
 * Sent packet:
  tx 1 psrc=200 data=0x80c8333300001111000007000100810000cc86dd600000000049068000000011000000000000333322221111ffff0000000000000000000000001111000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 *
 * Received packets on unit 0 should be:
 * Source port: 200, Destination port: 201
 * Data: 0x33330000111100123456789a8100010086dd600000000049067f00000011000000000000333322221111ffff0000000000000000000000001111000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Source port: 200, Destination port: 202
 * Data: 0x33330000111100123456789b8100010186dd600000000049067f00000011000000000000333322221111ffff0000000000000000000000001111000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on ipv6 ipmc entry
 * Test Scenario - end
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
    int out_port2,
    int use_kbp)
{
    bcm_ipmc_addr_t data;
    bcm_ip6_t full_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t part_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0};
    cint_ipmc_info.out_ports[0] = out_port;
    cint_ipmc_info.out_ports[1] = out_port2;
    int ext_ipv4_fwd_enable = (*dnxc_data_get(unit, "elk", "application", "ipv4", NULL));
    int ext_ipv6_fwd_enable = (*dnxc_data_get(unit, "elk", "application", "ipv6", NULL));

    if (!(ext_ipv4_fwd_enable || ext_ipv6_fwd_enable))
    {
        /** Configure statistics for LEM */
        BCM_IF_ERROR_RETURN_MSG(cint_stat_pp_config_main(unit, ipmc_1_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0),
            "failed for ipmc_1_statistics_config");
    }

    /** Configure statistics for TCAM */
    BCM_IF_ERROR_RETURN_MSG(cint_stat_pp_config_main(unit, ipmc_2_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0),
        "failed for ipmc_2_statistics_config");

    /** Set up ipv6 ipmc service */
    BCM_IF_ERROR_RETURN_MSG(basic_ipmc_v6_example(unit, vid_in, cint_ipmc_info.in_port, cint_ipmc_info.out_ports[0], cint_ipmc_info.out_ports[1]), "");

    if (!(ext_ipv4_fwd_enable || ext_ipv6_fwd_enable))
    {
        /** Replace ipv6 IPMC entry based on LEM */
        bcm_ipmc_addr_t_init(&data);
        cint_ipmc_info.ipv6_dip_addr[15] --;
        cint_ipmc_info.ipv6_dip_addr[15] --;
        if (*(dnxc_data_get(unit, "l3", "fwd", "ipmc_config_cmprs_vrf_group", NULL)))
        {
            data.s_ip6_addr = cint_ipmc_info.ipv6_sip_addr;
            data.s_ip6_mask = part_mask;
            data.mc_ip_addr = cint_ipmc_info.compressed_sip;
            data.mc_ip_mask = 0xFFFF;
        } else {
            data.mc_ip6_addr = cint_ipmc_info.ipv6_dip_addr;
            data.mc_ip6_mask = full_mask;
            data.vrf        = cint_ipmc_info.vrf;
        }
        if (*(dnxc_data_get(unit, "l3", "fwd", "ipmc_rif_key_participation", NULL)))
        {
            data.vid        = cint_ipmc_info.intf_in;
        }
        data.group      = cint_ipmc_info.mc_group;
        data.stat_id    = ipmc_1_statistics_config.stat_id;
        data.stat_pp_profile = ipmc_1_statistics_config.pp_profile;
        data.flags      = BCM_IPMC_REPLACE | BCM_IPMC_IP6;
        BCM_IF_ERROR_RETURN_MSG(bcm_ipmc_add(unit, &data), "to LEM table");
    }
    else
    {
        cint_ipmc_info.ipv6_dip_addr[15] --;
        cint_ipmc_info.ipv6_dip_addr[15] --;
    }

    if (ext_ipv6_fwd_enable)
    {
        /** Replace ipv6 IPMC entry based on TCAM */
        cint_ipmc_info.ipv6_dip_addr[15] ++;
        cint_ipmc_info.ipv6_dip_addr[15] ++;
        bcm_ipmc_addr_t_init(&data);
        if (*(dnxc_data_get(unit, "l3", "fwd", "ipmc_config_cmprs_vrf_group", NULL)))
        {
            data.s_ip6_addr = cint_ipmc_info.ipv6_sip_addr;;
            data.s_ip6_mask = part_mask;
            data.mc_ip_addr = cint_ipmc_info.compressed_sip;
            data.mc_ip_mask = 0xFFFF;
        } else {
            data.mc_ip6_addr = cint_ipmc_info.ipv6_dip_addr;
            data.mc_ip6_mask = full_mask;
            data.s_ip6_addr  = cint_ipmc_info.ipv6_sip_addr;
            data.s_ip6_mask  = part_mask;
            data.vrf         = cint_ipmc_info.vrf;
        }
        if (*(dnxc_data_get(unit, "l3", "fwd", "ipmc_rif_key_participation", NULL)))
        {
            data.vid        = cint_ipmc_info.intf_in;
        }
        data.group      = cint_ipmc_info.mc_group;
        data.stat_id    = ipmc_2_statistics_config.stat_id;
        data.stat_pp_profile = ipmc_2_statistics_config.pp_profile;
        data.flags      = BCM_IPMC_IP6;
        if (use_kbp)
        {
            /* JR2 requires flag BCM_IPMC_TCAM if external forwarding enabled */
            data.flags |= BCM_IPMC_TCAM;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_ipmc_remove(unit, &data), "to TCAM table");
        BCM_IF_ERROR_RETURN_MSG(bcm_ipmc_add(unit, &data), "to TCAM table");
    }

    return BCM_E_NONE;
}


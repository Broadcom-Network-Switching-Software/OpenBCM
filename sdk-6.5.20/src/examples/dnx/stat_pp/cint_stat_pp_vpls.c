/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_stat_pp_vpls.c Purpose: Statistic configuration for vpls (pwe in/out lif, mpls in/out lif).
 */

/*
 * File: cint_stat_pp_vpls.c
 * Purpose: Example of vpls statistics pp count configuration
 *
 * Configure statistics on pwe in/out lif, mpls in/out lif
 *
 * The cint includes:
 * - Main function to configure statistics on pwe in/out lif, mpls in/out lif
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
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_vpls.c
 * cint ../../src/examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_erpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint
 * 1: pwe term
 * Send pkt:
 * cint_stat_pp_vpls_example_term(0,200,201);
 * tx 1 psrc=201 data=0x000c0002000100000000cd1d8100001e884700d0504000d8014000110000011200010203040581000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 201, Destination port: 200
 * Data: 0x00110000011200010203040581000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on pwe inlif statistic interface,  1 packet received on mpls inlif statistic interface
 *
 * 2: pwe encap
 * cint_stat_pp_vpls_example_encap(0,200,201);
 * tx 1 psrc=200 data=0x000c0002000000110000011281000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 200, Destination port: 201
 * Data: 0x00000000cd1d000c000200018100001e884700d050fe00d801fe000c0002000000110000011281000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * CRPS verify: 1 packet received on pwe outlif statistic interface, 1 packet received on mpls outlif statistic interface
 *
 * This cint is used in order to configure statistics counting on pwe in/out lif, mpls in/out lif
 * The cint calls vpls_main() cint API in order to configure the ports and lifs.
 * It calls cint_stat_pp_config_main() with the relevant configuration params for the fwd APIs (ac, pwe ports).
 * For pwe/mpls lif - it calls cint_stat_pp_config.c APIs directly, since the call to bcm_gport_stat_set(), which is part of
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

cint_stat_pp_config_info_s vpls_pwe_inlif_statistics_properties =
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

cint_stat_pp_config_info_s vpls_mpls_inlif_statistics_properties =
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
        1,
        /**tunnel_metadata_size*/
        3,
        /**tunnel_metadata_shift*/
        3
};

cint_stat_pp_config_info_s vpls_pwe_outlif_statistics_properties =
{
        /**database_id */
        3,
        /**crps_engine_id */
        3,
        /**pp_profile */
        3,
        /**counter_command */
        3,
        /**crps_start */
        0,
        /**crps_end */
        100,
        /**type_id*/
        0,
        /**stat_id*/
        3,
        /**expansion_flag*/
        1
};

cint_stat_pp_config_info_s vpls_mpls_outlif_statistics_properties =
{
        /**database_id */
        4,
        /**crps_engine_id */
        4,
        /**pp_profile */
        2,
        /**counter_command */
        4,
        /**crps_start */
        0,
        /**crps_end */
        100,
        /**type_id*/
        0,
        /**stat_id*/
        4,
        /**expansion_flag*/
        1
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
cint_stat_pp_vpls_example_term(
   int unit,
   int ac_p,
   int pwe_p)
{
    int rv, idx;
    bcm_stat_pp_profile_info_t pp_profile;
    bcm_stat_pp_info_t stat_info;
    int pp_profile_id[2] = {1, 2};
    int stat_id[2] = {1, 2};
    int ports[2];

    /** init vpls params for cint */
    rv = init_default_vpls_params(unit);
    if (rv != BCM_E_NONE)
    {
        printf("init_default_vpls_params failed\n");
        return rv;
    }

    /** add statistics flag to enable counting */
    pwe_term.flags2 |= BCM_MPLS_PORT2_STAT_ENABLE;
    mpls_term_tunnel[0].flags2 != BCM_MPLS_SWITCH2_STAT_ENABLE;

    /** pwe inlif statistics configuration */
    rv = cint_stat_pp_config_main(unit, vpls_pwe_inlif_statistics_properties, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for vpls_pwe_inlif_statistics_properties\n");
        return rv;
    }

    /** mpls inlif statistics configuration */
    rv = cint_stat_pp_config_main(unit, vpls_mpls_inlif_statistics_properties, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for vpls_mpls_inlif_statistics_properties\n");
        return rv;
    }

    /** Call vpls cint to create vpls, configure ports and lifs */
    rv = vpls_main(unit, ac_p, pwe_p);
    if (rv != BCM_E_NONE)
    {
        printf("vpls_main failed\n");
        return rv;
    }

    ports[0] = pwe_term.mpls_port_id;
    ports[1] = mpls_term_tunnel[0].tunnel_id;
    /** configure statistics on the pwe lif, now that the gport is encoded */
    for(idx = 0; idx < 2; idx++) {
        stat_info.stat_pp_profile = pp_profile_id[idx];
        stat_info.stat_id = stat_id[idx];
        print stat_info;
        rv = bcm_gport_stat_set(unit, ports[idx], BCM_CORE_ALL, bcmStatCounterInterfaceIngressReceivePp, stat_info);
        if (rv != BCM_E_NONE)
        {
             printf("Error in bcm_gport_stat_set in IRPP\n");
        }
    }

    return rv;
}

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
cint_stat_pp_vpls_example_encap(
   int unit,
   int ac_p,
   int pwe_p)
{
    int rv, idx, gport;
    bcm_stat_pp_profile_info_t pp_profile;
    bcm_stat_pp_info_t stat_info;
    int pp_profile_id[2] = {0x80000003, 0x80000002};
    int stat_id[2] = {3, 4};
    int ports[2];

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
    pwe_encap.label_flags |= BCM_MPLS_EGRESS_LABEL_STAT_ENABLE;
    mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_STAT_ENABLE;

    /** pwe outlif statistics configuration */
    rv = cint_stat_pp_config_main(unit, vpls_pwe_outlif_statistics_properties, bcmStatCounterInterfaceEgressTransmitPp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for vpls_pwe_outlif_statistics_properties\n");
        return rv;
    }

    /** mpls outlif statistics configuration */
    rv = cint_stat_pp_config_main(unit, vpls_mpls_outlif_statistics_properties, bcmStatCounterInterfaceEgressTransmitPp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for vpls_mpls_outlif_statistics_properties\n");
        return rv;
    }

    /** Call vpls cint to create vpls, configure ports and lifs */
    rv = vpls_main(unit, ac_p, pwe_p);
    if (rv != BCM_E_NONE)
    {
        printf("vpls_main failed\n");
        return rv;
    }

    ports[0] = pwe_encap.mpls_port_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport,mpls_encap_tunnel[0].tunnel_id);
    ports[1] = gport;
    /** configure statistics on the pwe lif, now that the gport is encoded */
    for(idx = 0; idx < 2; idx++) {
        stat_info.stat_pp_profile = pp_profile_id[idx];
        stat_info.stat_id = stat_id[idx];
        print stat_info;
        print ports[idx];
        rv = bcm_gport_stat_set(unit, ports[idx], BCM_CORE_ALL, bcmStatCounterInterfaceEgressTransmitPp, stat_info);
        if (rv != BCM_E_NONE)
        {
             printf("Error in bcm_gport_stat_set in ETPP\n");
        }
    }

    return rv;
}

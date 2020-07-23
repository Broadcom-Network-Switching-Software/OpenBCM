/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_stat_pp_ip_tunnel.c Purpose: Statistics examples for SDK ingress APIs using ip tunnel.
 */

/*
 * File: cint_stat_pp_ip_tunnel.c
 * Purpose: Example of ipv4 tunnel term statistics collection
 *          Example of ipv4 tunnel encap statistics collection
 *          Example of ipv6 tunnel term statistics collection
 *          Example of ipv6 tunnel encap statistics collection
 *
 * Configure statistics on ipv4/6 term/encap object
 *
 * The cint includes:
 * - Main function to configure ipv4/6 term/encap with statistics
 * - Structs with configurations used to configure the statistics profiles (crps and pp)
 *
 * Usage:
 * ------
 *
 * Test Scenario:
 *--------------
 * ./bcm.user-opt
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_pwe.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_ip_tunnel.c
 * cint ../../src/examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_erpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint ../../src/examples/dnx/tunnel/cint_dnx_ipv6_tunnel_termination_basic.c
 * cint ../../src/examples/dnx/tunnel/cint_dnx_ipv6_tunnel_encapsulation_basic.c
 * cint ../../src/examples/sand/cint_ip_tunnel_term_basic.c
 * cint ../../src/examples/sand/cint_ip_tunnel_encap_basic.c
 * cint
 * 1. IPV4 tunnel term
 * cint_stat_pp_ipv4_tunnel_example_term(0,202,203);
 *
 * Sent packet:
 * tx 1 psrc=202 data=0x000c0002000000000700010008004500004d00000000802fe37fac000001ab00000100000800450000350000000040007704c0800101a0a1a1a2000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2
 *
 * Received packets on unit 0 should be:
 * Source port: 202, Destination port: 203
 * Data: 0x00000000cd1d000c0002000181000064080045000035000000003f007804c0800101a0a1a1a2000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on ipv4 tunnel term entry
 *
 * 2. IPV4 tunnel encap
 * cint_stat_pp_ipv4_tunnel_example_encap(0,202,203);
 *
 * Sent packet:
 * tx 1 psrc=202 data=0x000c00020056005000001234080045000035000000008000b7c0010203047fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2
 *
 * Received packets on unit 0 should be:
 * Source port: 202, Destination port: 203
 * Data: 0x00020000cd1d000c000200568100a0640800450a004d00000000402f3956a0000011a10000110000080045000035000000007f00b8c0010203047fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on ipv4 tunnel encap entry
 *
 * 3. IPV6 tunnel term
 * cint_stat_pp_ipv6_tunnel_example_term(0,200,203);
 *
 * Sent packet:
 * tx 1 psrc=200 data=0x000c0002000000000700010086dd6000000000612f80ecefeeedecebeae9e8e7e6e5e4e3e2e120010db800000000021122fffe33445500000800450000350000000040007704c0800101a0a1a1a2000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2
 *
 * Received packets on unit 0 should be:
 * Source port: 200, Destination port: 203
 * Data: 0x00000000cd1d000c0002000181000064080045000035000000003f007804c0800101a0a1a1a2000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on ipv6 tunnel term entry
 *
 * 4. IPV6 tunnel encap
 * cint_stat_pp_ipv6_tunnel_example_encap(0,200,202);
 *
 * Sent packet:
 * tx 1 psrc=200 data=0x000c00020056005000001234080045000035000000008000a7b80a0a0a047fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2
 *
 * Received packets on unit 0 should be:
 * Source port: 200, Destination port: 202
 * Data: 0x00020000cd1d000c000200568100a06486dd60a3c28300392f40ecefeeedecebeae9e8e7e6e5e4e3e2e120010db800000000021122fffe3344550000080045000035000000007f00a8b80a0a0a047fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on IPV6 tunnel encap entry
 */

cint_stat_pp_config_info_s ipv4_tunnel_outlif_statistics_properties =
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

cint_stat_pp_config_info_s ipv4_tunnel_inlif_statistics_properties =
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
        0
};

cint_stat_pp_config_info_s ipv6_tunnel_outlif_statistics_properties =
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

cint_stat_pp_config_info_s ipv6_tunnel_inlif_statistics_properties =
{
        /**database_id */
        4,
        /**crps_engine_id */
        4,
        /**pp_profile */
        4,
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
        1,
        /**tunnel_metadata_size*/
        3,
        /**tunnel_metadata_shift*/
        0
};

int cint_stat_pp_ipv4_tunnel_example_term(
   int unit,
   int inP,
   int outP)
{
    int rv, idx;
    bcm_stat_pp_info_t stat_info;
    int gport;

    /** ipv4 tunnel inlif statistics configuration */
    rv = cint_stat_pp_config_main(unit, ipv4_tunnel_inlif_statistics_properties, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for ipv4_tunnel_inlif_statistics_properties\n");
        return rv;
    }

    /** A GRE4oIPv4 tunnel header is being terminated */
    rv = ip_tunnel_term_basic(unit, inP, outP);
    if (rv != BCM_E_NONE)
    {
        printf("ip_tunnel_term_basic failed\n");
        return rv;
    }

    /** configure statistics on the ipv4 tunnel inlif, now that the gport is encoded */
    stat_info.stat_pp_profile = ipv4_tunnel_inlif_statistics_properties.pp_profile;
    stat_info.stat_id = ipv4_tunnel_inlif_statistics_properties.stat_id;
    print stat_info;
    rv = bcm_gport_stat_set(unit, cint_ip_tunnel_basic_info.tunnel_id, BCM_CORE_ALL, bcmStatCounterInterfaceIngressReceivePp, stat_info);
    if (rv != BCM_E_NONE)
    {
         printf("Error in bcm_gport_stat_set in IRPP\n");
    }

    return rv;
}

int cint_stat_pp_ipv4_tunnel_example_encap(
   int unit,
   int inP,
   int outP)
{
    int rv, idx;
    bcm_stat_pp_info_t stat_info;
    int gport;

    /** ipv4 tunnel outlif statistics configuration */
    rv = cint_stat_pp_config_main(unit, ipv4_tunnel_outlif_statistics_properties, bcmStatCounterInterfaceEgressTransmitPp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for ipv4_tunnel_outlif_statistics_properties\n");
        return rv;
    }

    /** A GRE4oIPv4 tunnel header is being encapsulated */
    rv = ip_tunnel_encap_basic(unit, inP, outP);
    if (rv != BCM_E_NONE)
    {
        printf("ip_tunnel_encap_basic failed\n");
        return rv;
    }

    /** configure statistics on the ipv4 tunnel outlif, now that the gport is encoded */
    BCM_GPORT_TUNNEL_ID_SET(gport, g_tunnel_id);
    stat_info.stat_pp_profile = 0x80000000 + ipv4_tunnel_outlif_statistics_properties.pp_profile;
    stat_info.stat_id = ipv4_tunnel_outlif_statistics_properties.stat_id;
    print stat_info;
    rv = bcm_gport_stat_set(unit, gport, BCM_CORE_ALL, bcmStatCounterInterfaceEgressTransmitPp, stat_info);
    if (rv != BCM_E_NONE)
    {
         printf("Error in bcm_gport_stat_set in IRPP\n");
    }

    return rv;
}

int cint_stat_pp_ipv6_tunnel_example_term(
   int unit,
   int inP,
   int outP)
{
    int rv, idx;
    bcm_stat_pp_info_t stat_info;
    int gport;

    /** ipv6 tunnel inlif statistics configuration */
    rv = cint_stat_pp_config_main(unit, ipv6_tunnel_inlif_statistics_properties, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for ipv6_tunnel_inlif_statistics_properties\n");
        return rv;
    }

    /** A GRE4oIPv6 tunnel header is being terminated */
    rv = ipv6_tunnel_term_basic(unit, inP, outP);
    if (rv != BCM_E_NONE)
    {
        printf("ipv6_tunnel_term_basic failed\n");
        return rv;
    }

    BCM_GPORT_TUNNEL_ID_SET(gport, 50000);
    /** configure statistics on the ipv6 tunnel inlif, now that the gport is encoded */
    stat_info.stat_pp_profile = ipv6_tunnel_inlif_statistics_properties.pp_profile;
    stat_info.stat_id = ipv6_tunnel_inlif_statistics_properties.stat_id;
    print stat_info;
    rv = bcm_gport_stat_set(unit, gport, BCM_CORE_ALL, bcmStatCounterInterfaceIngressReceivePp, stat_info);
    if (rv != BCM_E_NONE)
    {
         printf("Error in bcm_gport_stat_set in IRPP\n");
    }

    return rv;
}

int cint_stat_pp_ipv6_tunnel_example_encap(
   int unit,
   int inP,
   int outP)
{
    int rv, idx;
    bcm_stat_pp_info_t stat_info;
    int gport;

    /** ipv6 tunnel outlif statistics configuration */
    rv = cint_stat_pp_config_main(unit, ipv6_tunnel_outlif_statistics_properties, bcmStatCounterInterfaceEgressTransmitPp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for ipv6_tunnel_outlif_statistics_properties\n");
        return rv;
    }

    /** A GRE4oIPv6 tunnel header is being encapsulated */
    rv = ipv6_basic_tunnel_encapsulation(unit, inP, outP, 1);
    if (rv != BCM_E_NONE)
    {
        printf("ipv6_basic_tunnel_encapsulation failed\n");
        return rv;
    }

    /** configure statistics on the ipv6 tunnel outlif, now that the gport is encoded */
    BCM_GPORT_TUNNEL_ID_SET(gport, cint_ipv6_tunnel_basic_info.ipv6_tunnel_id);
    stat_info.stat_pp_profile = 0x80000000 + ipv6_tunnel_outlif_statistics_properties.pp_profile;
    stat_info.stat_id = ipv6_tunnel_outlif_statistics_properties.stat_id;
    print stat_info;
    rv = bcm_gport_stat_set(unit, gport, BCM_CORE_ALL, bcmStatCounterInterfaceEgressTransmitPp, stat_info);
    if (rv != BCM_E_NONE)
    {
         printf("Error in bcm_gport_stat_set in IRPP\n");
    }

    return rv;
}

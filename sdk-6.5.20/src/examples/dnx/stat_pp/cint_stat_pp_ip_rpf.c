/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_stat_pp_ip_rpf.c Purpose: RPF Statistics examples for SDK ingress APIs using l3 UC rif.
 */

/*
 * File: cint_stat_pp_ip_rpf.c
 * Purpose: Example of ipv4/6 loose  (match rpf miss rif)  statistics collection: bcmL3IngressUrpfLoose
 *          Example of ipv4/6 loose  (match rpf match rif) statistics collection: bcmL3IngressUrpfLoose
 *          Example of ipv4/6 strict (match rpf match rif) statistics collection: bcmL3IngressUrpfStrict
 *
 * Configure statistics on ipv4/6 RPF object
 *
 * The cint includes:
 * - Main function to configure ipv4/6 UC RPF with statistics
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
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_rpf_basic.c
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_erpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_ip_rpf.c
 * cint
 * IPV4
 * dnx_example_ipv4_rpf_stat_pp(0,200,201,202,15,20,200,100,0);
 *
 * 1. IPV4 UC loose RPF (match rpf miss rif)
 * Sent packet:
 * tx 1 psrc=200 data=0x000c00020000000007000100080045000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2
 * Received packets on unit 0 should be:
 * Source port: 200, Destination port: 202
 * Data: 0x00000000cd1d00123456789a81000064080045000035000000007f00fb45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on IPV4 loose stat (match rpf miss rif)

 * 2. IPV4 UC loose RPF (match rpf match rif)
 * Sent packet:
 * tx 1 psrc=200 data=0x000c00020000000007000100080045000035000000008000fa44c08001027fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2
 * Received packets on unit 0 should be:
 * Source port: 200, Destination port: 202
 * Data: 0x00000000cd1d00123456789a81000064080045000035000000007f00fb44c08001027fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on IPV4 loose stat (match rpf match rif)

 * 3. IPV4 UC strict RPF (match rpf match rif)
 * Sent packet:
 * tx 1 psrc=201 data=0x000c00020000000007000100080045000035000000008000fa44c08001027fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2
 * Received packets on unit 0 should be:
 * Source port: 201, Destination port: 202
 * Data: 0x00000000cd1d00123456789a81000064080045000035000000007f00fb44c08001027fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on IPV4 strict stat (match rpf match rif)

 * IPV6
 * dnx_example_ipv6_rpf_stat_pp(0,200,202,203,15,20,200,100,0);
 *
 * 1. IPV6 UC loose RPF (match rpf miss rif)
 * Sent packet:
 * tx 1 psrc=200 data=0x000c0002000000000700010086dd60000000004906800000000000000000000000000000bbaa000000000000000012340000ff00ff13000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2
 * Received packets on unit 0 should be:
 * Source port: 200, Destination port: 203
 * Data: 0x00000000cd1d00123456789a8100006486dd600000000049067f0000000000000000000000000000bbaa000000000000000012340000ff00ff13000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on IPV6 loose stat (match rpf miss rif)

 * 2. IPV6 UC loose RPF (match rpf match rif)
 * Sent packet:
 * tx 1 psrc=200 data=0x000c0002000000000700010086dd6000000000490680000000000000cddd0000000000005678000000000000000012340000ff00ff13000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2
 * Received packets on unit 0 should be:
 * Source port: 200, Destination port: 203
 * Data: 0x00000000cd1d00123456789a8100006486dd600000000049067f000000000000cddd0000000000005678000000000000000012340000ff00ff13000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on IPV6 loose stat (match rpf match rif)

 * 3. IPV6 UC strict RPF (match rpf match rif)
 * Sent packet:
 * tx 1 psrc=202 data=0x000c0002000000000700010086dd6000000000490680000000000000cddd0000000000005678000000000000000012340000ff00ff13000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2
 * Received packets on unit 0 should be:
 * Source port: 202, Destination port: 203
 * Data: 0x00000000cd1d00123456789a8100006486dd600000000049067f000000000000cddd0000000000005678000000000000000012340000ff00ff13000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on IPV6 strict stat (match rpf match rif)
 */

cint_stat_pp_config_info_s loose_hit_rpf_miss_rif_statistics_config =
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
        5
};

cint_stat_pp_config_info_s loose_hit_rpf_match_rif_statistics_config =
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
        800,
        /**type_id*/
        0,
        /**stat_id*/
        2,
        /**expansion_flag*/
        5
};

cint_stat_pp_config_info_s strict_hit_rpf_match_rif_statistics_config =
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
        800,
        /**type_id*/
        0,
        /**stat_id*/
        4,
        /**expansion_flag*/
        5
};

int
dnx_example_ipv4_rpf_stat_pp(
    int unit,
    int in_port_loose,
    int in_port_strict,
    int out_port,
    int vsi_loose,
    int vsi_strict,
    int vrf_loose,
    int vrf_strict,
    int vpbr_enabled)
{
    int rv;
    bcm_l3_host_t host;
    int fec = cint_rpf_route_info.fec_id;
    int fec_rpf_loose = fec + 2;
    int fec_rpf_strict = fec + 4;

    /** Counfigure statistics for Loose:  Hit RPF miss RIF, stat cmd is valid and can be countered correctly */
    rv = cint_stat_pp_config_main(unit, loose_hit_rpf_miss_rif_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for loose_hit_rpf_miss_rif_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for Loose:  Hit RPF matched RIF, stat cmd is valid and can be countered correctly */
    rv = cint_stat_pp_config_main(unit, loose_hit_rpf_match_rif_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for loose_hit_rpf_match_rif_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for strict: Hit RPF matched RIF, stat cmd is valid and can be countered correctly */
    rv = cint_stat_pp_config_main(unit, strict_hit_rpf_match_rif_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for strict_hit_rpf_match_rif_statistics_config\n");
        return rv;
    }

    /** Example of a router with RPF scenario */
    rv = dnx_example_ipv4_rpf(unit, in_port_loose, in_port_strict, out_port, vsi_loose, vsi_strict, vrf_loose, vrf_strict, vpbr_enabled);
    if (rv != BCM_E_NONE)
    {
        printf("dnx_example_ipv4_rpf failed\n");
        return rv;
    }

    /** Update stat pp information of host entry for Loose:  Hit RPF not matched RIF*/
    bcm_l3_host_t_init(host);
    host.l3a_ip_addr = cint_rpf_route_info.host_sip;
    host.l3a_flags = BCM_L3_REPLACE;
    host.l3a_vrf = vrf_loose;
    host.l3a_intf = fec_rpf_loose;
    host.stat_id = loose_hit_rpf_miss_rif_statistics_config.stat_id;
    host.stat_pp_profile = loose_hit_rpf_miss_rif_statistics_config.pp_profile;
    rv = bcm_l3_host_add(unit, host);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_l3_host_add for loose_hit_rpf_miss_rif_statistics_config\n");
    }

    /** Update stat pp information of host entry for Loose:  Hit RPF matched RIF*/
    bcm_l3_host_t_init(host);
    host.l3a_ip_addr = cint_rpf_route_info.host_sip + 1;
    host.l3a_flags = BCM_L3_REPLACE;
    host.l3a_vrf = vrf_loose;
    host.l3a_intf = fec_rpf_loose;
    host.stat_id = loose_hit_rpf_match_rif_statistics_config.stat_id;
    host.stat_pp_profile = loose_hit_rpf_match_rif_statistics_config.pp_profile;
    rv = bcm_l3_host_add(unit, host);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_l3_host_add for loose_hit_rpf_match_rif_statistics_config\n");
    }

    /** Update stat pp information of host entry for strict: Hit RPF matched RIF*/
    bcm_l3_host_t_init(host);
    host.l3a_ip_addr = cint_rpf_route_info.host_sip + 1;
    host.l3a_flags = BCM_L3_REPLACE;
    host.l3a_vrf = vrf_strict;
    host.l3a_intf = fec_rpf_strict;
    host.stat_id = strict_hit_rpf_match_rif_statistics_config.stat_id;
    host.stat_pp_profile = strict_hit_rpf_match_rif_statistics_config.pp_profile;
    rv = bcm_l3_host_add(unit, host);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_l3_host_add for strict_hit_rpf_match_rif_statistics_config\n");
    }

    return rv;
}

int
dnx_example_ipv6_rpf_stat_pp(
    int unit,
    int in_port_loose,
    int in_port_strict,
    int out_port,
    int vsi_loose,
    int vsi_strict,
    int vrf_loose,
    int vrf_strict,
    int vpbr_enabled)
{
    int rv;
    bcm_l3_host_t host;
    int fec = cint_rpf_route_info.fec_id;
    int fec_rpf_loose = fec + 2;
    int fec_rpf_strict = fec + 4;
    bcm_ip6_t host_sip_1 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbb, 0xaa }; /*Source IP address 1*/
    bcm_ip6_t host_sip_2 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcd, 0xdd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x78 }; /*Source IP address 2*/


    /** Counfigure statistics for Loose:  Hit RPF miss RIF, stat cmd is valid and can be countered correctly */
    rv = cint_stat_pp_config_main(unit, loose_hit_rpf_miss_rif_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for loose_hit_rpf_miss_rif_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for Loose:  Hit RPF matched RIF, stat cmd is valid and can be countered correctly */
    rv = cint_stat_pp_config_main(unit, loose_hit_rpf_match_rif_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for loose_hit_rpf_match_rif_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for strict: Hit RPF matched RIF, stat cmd is valid and can be countered correctly */
    rv = cint_stat_pp_config_main(unit, strict_hit_rpf_match_rif_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for strict_hit_rpf_match_rif_statistics_config\n");
        return rv;
    }

    /** Example of a router with RPF scenario */
    rv = dnx_example_ipv6_rpf(unit, in_port_loose, in_port_strict, out_port, vsi_loose, vsi_strict, vrf_loose, vrf_strict, vpbr_enabled);
    if (rv != BCM_E_NONE)
    {
        printf("dnx_example_ipv6_rpf failed\n");
        return rv;
    }


    /** Update stat pp information of host entry for Loose:  Hit RPF miss RIF*/
    bcm_l3_host_t_init(host);
    host.l3a_ip6_addr = host_sip_1;
    host.l3a_flags = BCM_L3_REPLACE | BCM_L3_IP6;
    host.l3a_vrf = vrf_loose;
    host.l3a_intf = fec_rpf_loose;
    host.stat_id = loose_hit_rpf_miss_rif_statistics_config.stat_id;
    host.stat_pp_profile = loose_hit_rpf_miss_rif_statistics_config.pp_profile;
    rv = bcm_l3_host_add(unit, host);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_l3_host_add for loose_hit_rpf_miss_rif_statistics_config\n");
    }

    /** Update stat pp information of host entry for Loose:  Hit RPF matched RIF*/
    bcm_l3_host_t_init(host);
    host.l3a_ip6_addr = host_sip_2;
    host.l3a_flags = BCM_L3_REPLACE | BCM_L3_IP6;
    host.l3a_vrf = vrf_loose;
    host.l3a_intf = fec_rpf_loose;
    host.stat_id = loose_hit_rpf_match_rif_statistics_config.stat_id;
    host.stat_pp_profile = loose_hit_rpf_match_rif_statistics_config.pp_profile;
    rv = bcm_l3_host_add(unit, host);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_l3_host_add for loose_hit_rpf_match_rif_statistics_config\n");
    }

    /** Update stat pp information of host entry for strict: Hit RPF matched RIF*/
    bcm_l3_host_t_init(host);
    host.l3a_ip6_addr = host_sip_2;
    host.l3a_flags = BCM_L3_REPLACE | BCM_L3_IP6;
    host.l3a_vrf = vrf_strict;
    host.l3a_intf = fec_rpf_strict;
    host.stat_id = strict_hit_rpf_match_rif_statistics_config.stat_id;
    host.stat_pp_profile = strict_hit_rpf_match_rif_statistics_config.pp_profile;
    rv = bcm_l3_host_add(unit, host);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_l3_host_add for strict_hit_rpf_match_rif_statistics_config\n");
    }

    return rv;
}

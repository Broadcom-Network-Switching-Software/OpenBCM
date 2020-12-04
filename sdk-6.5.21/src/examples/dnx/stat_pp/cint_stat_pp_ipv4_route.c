/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_stat_pp_ipv4_route.c Purpose: Statistics examples for SDK ingress APIs using an IPV4 application.
 */

/*
 * File: cint_stat_pp_ipv4_route.c
 * Purpose: Example of multiple ingress PP statistics
 *
 * Configure statistics on 3 hierarchies of fecs, src and dst lookups in fwd
 *
 * The cint includes:
 * - Main function to configure ip route with statistics enabled along the way
 * - Structs with configurations used to configure the statistics profiles (crps and pp)
 * - A struct with ip route required configuration
 *
 * Usage:
 * ------
 *
 * Test Scenario:
 *--------------
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_hierarchical_fec.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_erpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint ../../src/examples/src/examples/dnx/stat_pp/cint_stat_pp_ipv4_route.c
 * cint
 * cint_stat_pp_ingress_example(0,200,201);
 * exit;
 *
 * Sent packet:
 * tx 1 psrc=201 data=0x00000000CD1D00123456789A9100003781000038080045000035000000007F00F45C08001017FFFFF02000102030405060708090A0B0C0D0E0F10111213141516118191A1B1C1D1E1F20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x00000000CD1D00123456789A9100003781000038080045000035000000007F00FB45C08001017FFFFF02000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20
 * CRPS verify: 1 packet received on fec 1, fec 2, fec 3, src lookup, dst lookup
 */

struct cint_stat_pp_ipv4_route_info_s
{
    int in_port;                                        /** incoming port */
    int out_port;                                       /** outgoing port */
    int intf_in;                                        /** Incoming packet ETH-RIF */
    int intf_out;                                       /** Outgoing packet ETH-RIF */
    bcm_mac_t intf_in_mac_address;                      /** mac for in RIF */
    bcm_mac_t intf_out_mac_address;                     /** mac for out RIF */
    bcm_mac_t arp_next_hop_mac;                         /** mac for out RIF */
    int encap_id1;                                      /** encap ID 1 */
    int encap_id2;                                      /** encap ID 2 */
    bcm_ip_t host;                                      /** host */
    bcm_ip_t host_sip_uc;                               /** UC SIP host */
    int vrf;                                            /** VRF */
    bcm_if_t fec_id1;                                   /** FEC id 1*/
    bcm_if_t fec_id2;                                   /** FEC id 2*/
    bcm_if_t fec_id3;                                   /** FEC id 3*/
    bcm_if_t ecmp_id1;                                  /** ECMP id 1*/
    bcm_if_t ecmp_id2;                                  /** ECMP id 2*/
    bcm_if_t ecmp_id3;                                  /** ECMP id 3*/
    int new_outer_vlan;                                 /** Outer VLAN ID */
    int new_inner_vlan;                                 /** Inner VLAN ID */
    int outer_tpid;                                     /** Outer TPID */
    int inner_tpid;                                     /** Inner TPID */
    bcm_if_t ecmp_id;                                   /** ECMP interface id 1 */
};

cint_stat_pp_ipv4_route_info_s cint_stat_pp_ipv4_route_info =
{
    /*
     * ports : in_port | out_port
     */
    200, 201,
    /*
     * intf_in | intf_out
     */
    15, 100,
    /*
     * intf_in_mac_address | intf_out_mac_address | arp_next_hop_mac
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x12, 0x34, 0x56, 0x78, 0x9a}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * encap_id1
     */
    0x1384,
    /*
     * encap_id2
     */
    0x1386,
    /*
     * host
     */
    0x7fffff02 /** 127.255.255.2 */,
    /*
     * host_sip_uc
     */
    0xc0800101 /** 192.128.1.1 */,
    /*
     * vrf
     */
    1,
    /*
     * fec_id1 (set on run time)
     */
    0,
    /*
     * fec_id2(set on run time)
     */
    0,
    /*
     * fec_id3(set on run time)
     */
    0,
    /*
     * ecmp_id1
     */
    50,
    /*
     * ecmp_id2
     */
    2150,
    /*
     * ecmp_id3
     */
    4097,
    /*
     * new_outer_vlan
     */
    55,
    /*
     * new_inner_vlan
     */
    56,
    /*
     * outer_tpid
     */
    0x9100,
    /*
     * inner_tpid
     */
    0x8100,
    /*
     * ecmp_id
     */
    50,
};

/*
 * Statistics configurations (see src/examples/dnx/stat_pp/cint_stat_pp_config.c)
 */
cint_stat_pp_config_info_s fec_1_statistics_config =
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
        10,
        /**type_id*/
        0,
        /**stat_id*/
        7,
        /**expansion_flag*/
        5
};

cint_stat_pp_config_info_s fec_2_statistics_config =
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
        20,
        /**crps_end */
        30,
        /**type_id*/
        0,
        /**stat_id*/
        22,
        /**expansion_flag*/
        5
};

cint_stat_pp_config_info_s fec_3_statistics_config =
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
        40,
        /**crps_end */
        50,
        /**type_id*/
        0,
        /**stat_id*/
        43,
        /**expansion_flag*/
        5
};

cint_stat_pp_config_info_s ecmp_1_statistics_config =
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
        50,
        /**expansion_flag*/
        5
};

cint_stat_pp_config_info_s ecmp_2_statistics_config =
{
        /**database_id */
        5,
        /**crps_engine_id */
        5,
        /**pp_profile */
        5,
        /**counter_command */
        5,
        /**crps_start */
        2140,
        /**crps_end */
        2180,
        /**type_id*/
        0,
        /**stat_id*/
        2150,
        /**expansion_flag*/
        5
};

cint_stat_pp_config_info_s ecmp_3_statistics_config =
{
        /**database_id */
        6,
        /**crps_engine_id */
        6,
        /**pp_profile */
        6,
        /**counter_command */
        6,
        /**crps_start */
        4050,
        /**crps_end */
        4100,
        /**type_id*/
        0,
        /**stat_id*/
        4097,
        /**expansion_flag*/
        5
};

cint_stat_pp_config_info_s l3_host_statistics_config =
{
        /**database_id */
        7,
        /**crps_engine_id */
        7,
        /**pp_profile */
        7,
        /**counter_command */
        7,
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

cint_stat_pp_config_info_s l3_host_sip_uc_statistics_config =
{
        /**database_id */
        8,
        /**crps_engine_id */
        8,
        /**pp_profile */
        8,
        /**counter_command */
        8,
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

/*
 * create ecmp group, pointing to 1 fec
 */
int
cint_stat_pp_ipv4_route_create_ecmp(
    int unit,
    bcm_if_t fec_id,
    bcm_if_t ecmp_id,
    uint32 flags,
    cint_stat_pp_config_info_s cint_stat_pp_config_info,
    bcm_l3_egress_ecmp_t *ecmp)
{
    int rv;
    bcm_if_t fec;

    bcm_l3_egress_ecmp_t_init(*ecmp);
    fec = fec_id;
    ecmp->flags = flags;
    ecmp->ecmp_intf = ecmp_id;
    ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
    ecmp->stat_id = cint_stat_pp_config_info.stat_id;
    ecmp->stat_pp_profile = cint_stat_pp_config_info.pp_profile;
    rv = bcm_l3_egress_ecmp_create(unit, ecmp, 1, &fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_egress_ecmp_create\n");
    }
    return rv;
}

/*
 * Update Fec values
 */
int
cint_stat_pp_update_fec_ids_values(
    int unit)
{
    int rv;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_stat_pp_ipv4_route_info.fec_id1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range hierarchy 1\n");
        return rv;
    }

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &cint_stat_pp_ipv4_route_info.fec_id2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range hierarchy 2\n");
        return rv;
    }

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 2, &cint_stat_pp_ipv4_route_info.fec_id3);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range hierarchy 3\n");
        return rv;
    }

    printf("cint_stat_pp_ipv4_route_info.fec_id1 %d\n",cint_stat_pp_ipv4_route_info.fec_id1 );
    printf("cint_stat_pp_ipv4_route_info.fec_id2 %d\n",cint_stat_pp_ipv4_route_info.fec_id2 );
    printf("cint_stat_pp_ipv4_route_info.fec_id3 %d\n",cint_stat_pp_ipv4_route_info.fec_id3 );

    return rv;
}


/*
 * main function.
 * Calls:
 * cint_stat_pp_config_crps()
 * cint_stat_pp_create_pp_profile()
 * Configures ipv4 route
 */
int
cint_stat_pp_ipv4_route_example(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_gport_t lif_as_gport = 0;
    bcm_gport_t gport = 0;
    l3_ingress_intf ingr_itf;
    sand_utils_l3_fec_s fec_structure;
    bcm_if_t fec;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_l3_host_t host;


    cint_stat_pp_ipv4_route_info.encap_id1 = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;
    cint_stat_pp_ipv4_route_info.encap_id2 = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x386;
    cint_stat_pp_ipv4_route_info.in_port = in_port;
    cint_stat_pp_ipv4_route_info.out_port = out_port;

    rv = cint_stat_pp_update_fec_ids_values(unit);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_update_fec_ids_values failed\n");
        return rv;
    }

    /** Counfigure statistics for first fec */
    rv = cint_stat_pp_config_main(unit, fec_1_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for fec_1_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for second fec */
    rv = cint_stat_pp_config_main(unit, fec_2_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for fec_2_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for third fec */
    rv = cint_stat_pp_config_main(unit, fec_3_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for fec_3_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for the first ecmp */
    rv = cint_stat_pp_config_main(unit, ecmp_1_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for ecmp_1_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for the second ecmp */
    rv = cint_stat_pp_config_main(unit, ecmp_2_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for ecmp_2_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for the third ecmp */
    rv = cint_stat_pp_config_main(unit, ecmp_3_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for ecmp_3_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for l3 host */
    rv = cint_stat_pp_config_main(unit, l3_host_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for l3_host_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for l3 host */
    rv = cint_stat_pp_config_main(unit, l3_host_sip_uc_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for l3_host_sip_uc_statistics_config\n");
        return rv;
    }

    l3_ingress_intf_init(&ingr_itf);

    /** Set In-Port to In ETh-RIF */
    rv = in_port_intf_set(unit, cint_stat_pp_ipv4_route_info.in_port, cint_stat_pp_ipv4_route_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /** Set Out-Port default properties */
    rv = out_port_set(unit, cint_stat_pp_ipv4_route_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out\n");
        return rv;
    }

    /** Create ETH-RIF and set its properties */
    rv = intf_eth_rif_create(unit, cint_stat_pp_ipv4_route_info.intf_in, cint_stat_pp_ipv4_route_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    rv = intf_eth_rif_create(unit, cint_stat_pp_ipv4_route_info.intf_out, cint_stat_pp_ipv4_route_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /** Set Incoming ETH-RIF properties, Indicate RPF lookup for interface */
    ingr_itf.intf_id = cint_stat_pp_ipv4_route_info.intf_in;
    ingr_itf.vrf = cint_stat_pp_ipv4_route_info.vrf;
    rv = intf_ingress_rif_rpf_set(unit, cint_stat_pp_ipv4_route_info.intf_in, cint_stat_pp_ipv4_route_info.vrf, bcmL3IngressUrpfLoose);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set ingr_itf\n");
        return rv;
    }

    /** Create ARP for second FEC and set its properties */
    rv = l3__egress_only_encap__create(unit, 0, &cint_stat_pp_ipv4_route_info.encap_id1, cint_stat_pp_ipv4_route_info.arp_next_hop_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    /** Create AC-Ptr for 3rd FEC and set its properties */
    rv = out_vlan_port_intf_set(unit, cint_stat_pp_ipv4_route_info.out_port, cint_stat_pp_ipv4_route_info.encap_id2, &lif_as_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_vlan_port_intf_set out_port\n");
        return rv;
    }

    BCM_GPORT_SUB_TYPE_LIF_SET(cint_stat_pp_ipv4_route_info.encap_id2, 0, cint_stat_pp_ipv4_route_info.encap_id2);
    BCM_GPORT_VLAN_PORT_ID_SET(cint_stat_pp_ipv4_route_info.encap_id2, cint_stat_pp_ipv4_route_info.encap_id2);

    /** Add VLAN editing for encap stage (using UNTAG tag_struct - 0) */
    rv = vlan_translate_ive_eve_translation_set(unit, lif_as_gport, cint_stat_pp_ipv4_route_info.outer_tpid,
                                                cint_stat_pp_ipv4_route_info.inner_tpid, bcmVlanActionAdd, bcmVlanActionAdd,
                                                cint_stat_pp_ipv4_route_info.new_outer_vlan, cint_stat_pp_ipv4_route_info.new_inner_vlan, 1, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_ive_eve_translation_set\n");
        return rv;
    }

    /** Create 3rd hierarchy FEC with encap_id_2 and gport pointing to the out_port */
    BCM_GPORT_LOCAL_SET(gport, cint_stat_pp_ipv4_route_info.out_port);
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination = gport;
    fec_structure.tunnel_gport = cint_stat_pp_ipv4_route_info.encap_id2;
    fec_structure.fec_id = cint_stat_pp_ipv4_route_info.fec_id3;
    fec_structure.flags = BCM_L3_3RD_HIERARCHY;
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    fec_structure.flags2 = 0;
    fec_structure.stat_id = fec_3_statistics_config.stat_id;
    fec_structure.stat_pp_profile = fec_3_statistics_config.pp_profile;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    /** Create ECMP group in 3rd hieararchy pointing to the last fec */
    rv = cint_stat_pp_ipv4_route_create_ecmp(unit,
                                             cint_stat_pp_ipv4_route_info.fec_id3,
                                             cint_stat_pp_ipv4_route_info.ecmp_id3,
                                             BCM_L3_WITH_ID | BCM_L3_3RD_HIERARCHY,
                                             ecmp_3_statistics_config,
                                             &ecmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_stat_pp_ipv4_route_create_ecmp\n");
        return rv;
    }

    /** Create second hierarchy FEC pointing to the third hierarchy ecmp */
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport, ecmp.ecmp_intf);
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination = gport;
    fec_structure.tunnel_gport = cint_stat_pp_ipv4_route_info.encap_id1;
    fec_structure.fec_id = cint_stat_pp_ipv4_route_info.fec_id2;
    fec_structure.flags = BCM_L3_2ND_HIERARCHY;
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    fec_structure.stat_id = fec_2_statistics_config.stat_id;
    fec_structure.stat_pp_profile = fec_2_statistics_config.pp_profile;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    /** Create ECMP group in 2nd hieararchy pointing to the second fec */
    rv = cint_stat_pp_ipv4_route_create_ecmp(unit,
                                             cint_stat_pp_ipv4_route_info.fec_id2,
                                             cint_stat_pp_ipv4_route_info.ecmp_id2,
                                             BCM_L3_WITH_ID | BCM_L3_2ND_HIERARCHY,
                                             ecmp_2_statistics_config,
                                             &ecmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_stat_pp_ipv4_route_create_ecmp\n");
        return rv;
    }

    /** Create first level FEC that point to a second level FEC */
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport, ecmp.ecmp_intf);
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination = gport;
    fec_structure.tunnel_gport = cint_stat_pp_ipv4_route_info.intf_out;
    fec_structure.fec_id = cint_stat_pp_ipv4_route_info.fec_id1;
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    fec_structure.stat_id = fec_1_statistics_config.stat_id;
    fec_structure.stat_pp_profile = fec_1_statistics_config.pp_profile;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    /** Create first hierarchy ECMP group which points to the first fec */
    rv = cint_stat_pp_ipv4_route_create_ecmp(unit,
                                             cint_stat_pp_ipv4_route_info.fec_id1,
                                             cint_stat_pp_ipv4_route_info.ecmp_id1,
                                             BCM_L3_WITH_ID,
                                             ecmp_1_statistics_config,
                                             &ecmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_stat_pp_ipv4_route_create_ecmp\n");
        return rv;
    }

    /** Add host entry - dst lookup */
    bcm_l3_host_t_init(host);
    host.l3a_ip_addr = cint_stat_pp_ipv4_route_info.host;
    host.l3a_vrf = cint_stat_pp_ipv4_route_info.vrf;
    host.l3a_intf = ecmp.ecmp_intf;
    host.stat_id = l3_host_statistics_config.stat_id;
    host.stat_pp_profile = l3_host_statistics_config.pp_profile;
    rv = bcm_l3_host_add(unit, host);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_l3_host_add\n");
    }

    /** Add host entry - src lookup */
    bcm_l3_host_t_init(host);
    host.l3a_ip_addr = cint_stat_pp_ipv4_route_info.host_sip_uc;
    host.l3a_vrf = cint_stat_pp_ipv4_route_info.vrf;
    host.l3a_intf = ecmp.ecmp_intf;
    host.stat_id = l3_host_sip_uc_statistics_config.stat_id;
    host.stat_pp_profile = l3_host_sip_uc_statistics_config.pp_profile;
    rv = bcm_l3_host_add(unit, host);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_l3_host_add\n");
    }

    return rv;
}

/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_stat_pp_mpls_egress.c Purpose: Statistics examples for SDK egress APIs using an mpls encapsulation.
 */

/*
 * File: cint_stat_pp_mpls_egress.c
 * Purpose: Example of etpp lif statistics collection
 *          Example of etpp term statistics collection
 *
 * Configure statistics on 4 mpls etps's
 * Configure statistics on mpls php object
 *
 * The cint includes:
 * - Main function to configure mpls encapsulation with statistics enabled along the way
 * - Main function to configure mpls php with statistics
 * - Structs with configurations used to configure the statistics profiles (crps and pp)
 * - A struct with mpls encapsulation required configuration
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
 * cint ../../src/examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_erpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint ../../src/examples/src/examples/dnx/stat_pp/cint_stat_pp_mpls_egress.c
 * cint
 * cint_stat_pp_mpls_encap_example(0,200,201);
 * exit;
 *
 * cint_stat_pp_mpls_encap_example:
 * Sent packet:
 *tx 1 psrc=201 data=0x45000035000000008000b640a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x00110000011200000000cd1d81000028884700bb807f00bb907f00bba07f00bbb7f45000035000000007f00b740a0a1a1a2a0a1a1a3000102030405060708090a00c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on etps 1, 2, 3, 4
 *
 * cint_stat_pp_mpls_php_example:
 * Sent packet:
 * tx 1 psrc=201 data=0x45000035000000008000b640a0a1a1a2a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x00110000011200000000CD1D81000028080045000035000000003F00F740A0A1A1A2A0A1A1A3000102030405060708090A0B
 * CRPS verify: 1 packet received on egress term stage
 */

struct cint_stat_pp_mpls_egress_info_s
{
    bcm_mpls_label_t label1;                            /** first mpls label */
    bcm_mpls_label_t label2;                            /** second mpls label */
    bcm_mpls_label_t label3;                            /** third mpls label */
    bcm_mpls_label_t label4;                            /** fourth mpls label */
};

cint_stat_pp_mpls_egress_info_s cint_stat_pp_mpls_egress_info =
{
    /*
     * label1 | label2 | label3 | label4
     */
    3000, 3001, 3002, 3003,
};

/*
 * Statistics configurations (see src/examples/dnx/stat_pp/cint_stat_pp_config.c)
 */
cint_stat_pp_config_info_s etps_1_statistics_config =
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

cint_stat_pp_config_info_s etps_2_statistics_config =
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

cint_stat_pp_config_info_s etps_3_statistics_config =
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

cint_stat_pp_config_info_s etps_4_statistics_config =
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
        1
};

cint_stat_pp_config_info_s mpls_php_statistics_config =
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

int
cint_stat_pp_mpls_encap_example(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;
    int is_lif = TRUE;
    bcm_stat_pp_info_t stat_info;
    bcm_stat_pp_profile_info_t pp_profile;
    int pp_profile_id;

    init_default_mpls_params(unit, port1, port2);

    /** fec pointer is following the mpls first encap tunnel, which will be created later */
    mpls_util_entity[0].fecs[0].tunnel_gport = &mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;

    /** Configure 4th mpls tunnel */
    mpls_util_entity[0].mpls_encap_tunnel[3].label[0] = cint_stat_pp_mpls_egress_info.label4;
    mpls_util_entity[0].mpls_encap_tunnel[3].encap_access = bcmEncapAccessTunnel4;
    mpls_util_entity[0].mpls_encap_tunnel[3].l3_intf_id = &mpls_util_entity[1].arps[0].arp;
    mpls_util_entity[0].mpls_encap_tunnel[3].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[3].flags |= (BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_STAT_ENABLE);
    mpls_util_entity[0].mpls_encap_tunnel[3].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    mpls_util_entity[0].mpls_encap_tunnel[3].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;

    /** Configure 3rd mpls tunnel */
    mpls_util_entity[0].mpls_encap_tunnel[2].label[0] = cint_stat_pp_mpls_egress_info.label3;
    mpls_util_entity[0].mpls_encap_tunnel[2].encap_access = bcmEncapAccessTunnel3;
    mpls_util_entity[0].mpls_encap_tunnel[2].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[3].tunnel_id;
    mpls_util_entity[0].mpls_encap_tunnel[2].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[2].flags |= (BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_STAT_ENABLE);
    mpls_util_entity[0].mpls_encap_tunnel[2].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    mpls_util_entity[0].mpls_encap_tunnel[2].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;

    /** Configure 2nd mpls tunnel */
    mpls_util_entity[0].mpls_encap_tunnel[1].label[0] = cint_stat_pp_mpls_egress_info.label2;
    mpls_util_entity[0].mpls_encap_tunnel[1].encap_access = bcmEncapAccessTunnel2;
    mpls_util_entity[0].mpls_encap_tunnel[1].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[2].tunnel_id;
    mpls_util_entity[0].mpls_encap_tunnel[1].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[1].flags |= (BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_STAT_ENABLE);
    mpls_util_entity[0].mpls_encap_tunnel[1].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    mpls_util_entity[0].mpls_encap_tunnel[1].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;

    /** Configure 1st mpls tunnel */
    mpls_util_entity[0].mpls_encap_tunnel[0].label[0] = cint_stat_pp_mpls_egress_info.label1;
    mpls_util_entity[0].mpls_encap_tunnel[0].encap_access = bcmEncapAccessTunnel1;
    mpls_util_entity[0].mpls_encap_tunnel[0].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[1].tunnel_id;
    mpls_util_entity[0].mpls_encap_tunnel[0].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[0].flags |= (BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_STAT_ENABLE);
    mpls_util_entity[0].mpls_encap_tunnel[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    mpls_util_entity[0].mpls_encap_tunnel[0].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;

    /** Create the mpls tunnels */
    rv = mpls_util_main(unit, port1, port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in mpls_util_main\n", rv);
        return rv;
    }

    /** Counfigure statistics for first etps */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id);
    rv = cint_stat_pp_config_main(unit, etps_1_statistics_config, bcmStatCounterInterfaceEgressTransmitPp, gport, BCM_CORE_ALL, is_lif);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for etps_1_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for second etps */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, mpls_util_entity[0].mpls_encap_tunnel[1].tunnel_id);
    rv = cint_stat_pp_config_main(unit, etps_2_statistics_config, bcmStatCounterInterfaceEgressTransmitPp, gport, BCM_CORE_ALL, is_lif);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for etps_2_statistics_config\n");
        return rv;
    }

    /** Counfigure statistics for third etps */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, mpls_util_entity[0].mpls_encap_tunnel[2].tunnel_id);
    rv = cint_stat_pp_config_main(unit, etps_3_statistics_config, bcmStatCounterInterfaceEgressTransmitPp, gport, BCM_CORE_ALL, is_lif);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for etps_3_statistics_config\n");
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2_B0))
    {
        /** Counfigure statistics for fourth etps */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, mpls_util_entity[0].mpls_encap_tunnel[3].tunnel_id);
        rv = cint_stat_pp_config_main(unit, etps_4_statistics_config, bcmStatCounterInterfaceEgressTransmitPp, gport, BCM_CORE_ALL, is_lif);
        if (rv != BCM_E_NONE)
        {
            printf("cint_stat_pp_config_main failed for etps_4_statistics_config\n");
            return rv;
        }
    }

    return rv;
}

/*
 * main php statistics function.
 * Calls:
 * mpls_util_main()
 * cint_stat_pp_config_main()
 */
int
cint_stat_pp_mpls_php_example(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;
    int is_lif = TRUE;

    init_default_mpls_params(unit, port1, port2);

    /** Configure mpls tunnel and php */
    mpls_util_entity[0].mpls_encap_tunnel[0].label[0] = BCM_MPLS_LABEL_INVALID;
    mpls_util_entity[0].mpls_encap_tunnel[0].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[0].action = BCM_MPLS_EGRESS_ACTION_PHP;
    mpls_util_entity[0].mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_ACTION_VALID;

    mpls_util_entity[0].mpls_switch_tunnel[0].label = cint_stat_pp_mpls_egress_info.label1;
    mpls_util_entity[0].mpls_switch_tunnel[0].egress_if = &mpls_util_entity[0].fecs[0].fec_id;
    mpls_util_entity[0].mpls_switch_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_NOP;
    mpls_util_entity[0].mpls_switch_tunnel[0].flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    /** Create the mpls tunnels */
    rv = mpls_util_main(unit, port1, port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in mpls_util_main\n", rv);
        return rv;
    }

    /** Counfigure statistics on mpls php object */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id);
    rv = cint_stat_pp_config_main(unit, mpls_php_statistics_config, bcmStatCounterInterfaceEgressTransmitPp, gport, BCM_CORE_ALL, is_lif);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for etps_1_statistics_config\n");
        return rv;
    }

    return rv;
}

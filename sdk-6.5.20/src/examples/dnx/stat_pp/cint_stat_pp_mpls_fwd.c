/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_stat_pp_mpls_fwd.c Purpose: Statistics examples for SDK ingress APIs using mpls fwd.
 */

/*
 * File: cint_stat_pp_mpls_fwd.c
 * Purpose: Example of mpls fwd php  statistics collection
 *          Example of mpls fwd swap statistics collection
 *          Example of mpls fwd nop  statistics collection
 *
 * Configure statistics on mpls php/swap/nop object
 *
 * The cint includes:
 * - Main function to configure mpls php/swap/nop with statistics
 * - Structs with configurations used to configure the statistics profiles (crps and pp)
 *
 * Usage:
 * ------
 *
 * Test Scenario:
 *--------------
 * ./bcm.user-opt
 * cd ../../../../regress/bcm
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_sand_mpls_qos.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/sand/cint_sand_multi_device_ipv4_uc.c
 * cint ../../src/examples/sand/cint_sand_mpls_lsr.c
 * cint ../../src/examples/sand/cint_mpls_tunnel_initiator.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_mpls_fwd.c
 * cint
 * 1. mpls fwd php entry
 * cint_stat_pp_mpls_fwd_php_example(0,0,200,201);
 *
 * Sent packet:
 * tx 1 psrc=200 data=0x00000000001100005c6d68d1810000648847003e932f45000035000000008000b1c10a0000057fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2
 *
 * Received packets on unit 0 should be:
 * Source port: 200, Destination port: 201
 * Data: 0x000000000022000000000011810000c8080045000035000000008000b1c10a0000057fffff03003e932f45000035000000008000b1c10a0000057fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * CRPS verify: 1 packet received on mpls fwd php entry
 *
 * 2. mpls fwd swap entry
 * cint_stat_pp_mpls_fwd_swap_example(0,0,200,201);
 *
 * Sent packet:
 * tx 1 psrc=200 data=0x0000000000110000d444394481000064884701388904000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200
 *
 * Received packets on unit 0 should be:
 * Source port: 200, Destination port: 201
 * Data: 0x000000000022000000000011810040c8884701f40903000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 * CRPS verify: 1 packet received on mpls fwd swap entry
 *
 * 3. mpls fwd nop entry
 * fec_point_to_tunnel=1;
 * cint_stat_pp_mpls_fwd_nop_example(0,0,200,201,0);
 *
 * Sent packet:
 * tx 1 psrc=200 data=0x0000000000330000d166d5148100012c8847017eb140000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2
 *
 * Received packets on unit 0 should be:
 * Source port: 200, Destination port: 201
 * Data: 0x000000000044000000000033810001f4884701b5813f000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 * CRPS verify: 1 packet received on mpls fwd nop entry
 */

/*
 * Statistics configurations (see src/examples/dnx/stat_pp/cint_stat_pp_config.c)
 */
cint_stat_pp_config_info_s mpls_fwd_php_statistics_config =
{
        /* database_id  */
        1,
        /* crps_engine_id */
        1,
        /* pp_profile */
        1,
        /* counter_command */
        1,
        /* crps_start */
        0,
        /* crps_end */
        100,
        /* type_id */
        0,
        /* stat_id */
        1,
        /* expansion_flag */
        5
};

cint_stat_pp_config_info_s mpls_fwd_swap_statistics_config =
{
        /* database_id */
        2,
        /* crps_engine_id */
        2,
        /* pp_profile */
        2,
        /* counter_command */
        2,
        /* crps_start */
        0,
        /* crps_end */
        100,
        /* type_id */
        0,
        /* stat_id */
        2,
        /* expansion_flag */
        5
};

cint_stat_pp_config_info_s mpls_fwd_nop_statistics_config =
{
        /* database_id */
        4,
        /* crps_engine_id */
        4,
        /* pp_profile */
        4,
        /* counter_command */
        4,
        /* crps_start */
        0,
        /* crps_end */
        100,
        /* type_id */
        0,
        /* stat_id */
        4,
        /* expansion_flag */
        5
};

/*
 * MPLS ILM PHP Stat PP information
 */
int mpls_lsr_ilm_php_entry_replace_with_stat_pp(
    int unit,
    int out_sys_port)
{
      int rv;
    bcm_mpls_tunnel_switch_t entry;
    bcm_mpls_tunnel_switch_t_init(&entry);

    /* incoming label */
    entry.label = cint_mpls_lsr_info.incoming_label[1];
    entry.action = BCM_MPLS_SWITCH_ACTION_PHP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /*
     *TTL/EXP as uniform mode
     *JR2 uses ingress_qos_model to replace of flags for qos propagation
     */
    if (is_device_or_above(unit, JERICHO2)) {
        entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    } else {
        entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;

    }
    if ((!soc_property_get(unit, "system_headers_mode", 1) && is_device_or_above(unit, JERICHO2))
        ||!is_device_or_above(unit, JERICHO2) )
    {
        entry.flags |= BCM_MPLS_SWITCH_NEXT_HEADER_IPV4;
    }
    entry.egress_if = cint_mpls_lsr_info.fec_id_for_arp;
    entry.stat_id = mpls_fwd_php_statistics_config.stat_id;
    entry.stat_pp_profile = mpls_fwd_php_statistics_config.pp_profile;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    return rv;
}

/*
 * MPLS ILM SWAP Stat PP information
 */
int mpls_lsr_ilm_swap_entry_replace_with_stat_pp(
    int unit,
    int out_sys_port,
    int rif)
{
    int rv, mpls_termination_label_index_enable;
    bcm_mpls_tunnel_switch_t entry;
    mpls_termination_label_index_enable = soc_property_get(unit, "mpls_termination_label_index_enable", 0);

    /*
     * swap over MPLS
     */
    bcm_mpls_tunnel_switch_t_init(&entry);
    /* incoming label */
    if (mpls_termination_label_index_enable) {
        BCM_MPLS_INDEXED_LABEL_SET(entry.label,cint_mpls_lsr_info.incoming_label[0],1);
    } else {
        entry.label = cint_mpls_lsr_info.incoming_label[0];
    }
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    entry.egress_label.label = cint_mpls_lsr_info.outgoing_label[0];
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    if(rif) {
       entry.ingress_if = rif;
       entry.flags |= BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF;
    }
    /* FEC entry pointing to fec for arp */
    entry.egress_if = cint_mpls_lsr_info.fec_id_for_arp;
    entry.stat_id = mpls_fwd_swap_statistics_config.stat_id;
    entry.stat_pp_profile = mpls_fwd_swap_statistics_config.pp_profile;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    /*
    * Set Incoming ETH-RIF properties
    */
    if (is_device_or_above(unit, JERICHO2))
    {
        bcm_l3_ingress_t l3_ing_if;
        bcm_l3_ingress_t_init(&l3_ing_if);
        bcm_l3_ingress_get(unit, rif, l3_ing_if);
        if (rv != BCM_E_NONE)
        {
             printf("Error (%d), bcm_l3_ingress_get \n", rv);
             return rv;
        }
        l3_ing_if.flags = l3_ing_if.flags |BCM_L3_INGRESS_WITH_ID |BCM_L3_INGRESS_MPLS_INTF_NAMESPACE;
        rv = bcm_l3_ingress_create(unit,l3_ing_if, rif);
        if (rv != BCM_E_NONE)
        {
             printf("Error (%d), bcm_l3_ingress_create \n", rv);
             return rv;
        }
    }
    return rv;
}

/*
 *  MPLS LSR PHP Stat PP
 */
int cint_stat_pp_mpls_fwd_php_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;
    int is_lif = FALSE;

    /*
     * step 1.port and rif configuration
     */
    rv = mpls_lsr_l2_l3_configuration(ingress_unit,egress_unit, in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, mpls_lsr_l2_l3_configuration\n");
        return rv;
    }

    /*
     * step 2. ingress FWD configuration
     */
    rv = mpls_lsr_ilm_php_entry_create(ingress_unit,out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, mpls_lsr_ilm_php_entry_create\n");
        return rv;
    }

    /*
     * step 3. Counfigure statistics for mpls fwd php
     */
    rv = cint_stat_pp_config_main(ingress_unit, mpls_fwd_php_statistics_config, bcmStatCounterInterfaceIngressReceivePp, gport, BCM_CORE_ALL, is_lif);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for mpls_fwd_php_statistics_config\n");
        return rv;
    }

    /*
     * step 4. update the mpls php entry with stat pp information
     */
    rv = mpls_lsr_ilm_php_entry_replace_with_stat_pp(ingress_unit, out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("mpls_lsr_ilm_php_entry_replace_with_stat_pp failed\n");
        return rv;
    }

    return rv;
}

/*
 *  MPLS LSR SWAP Stat PP
 */
int cint_stat_pp_mpls_fwd_swap_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;
    int is_lif = FALSE;

    /*
     * step 1.MPLS LSR SWAP
     */
    rv = multi_dev_mpls_lsr_swap_with_context_example(ingress_unit,egress_unit, in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_mpls_lsr_swap_with_context_example\n");
        return rv;
    }

    /*
     * step 2. Counfigure statistics for mpls fwd swap
     */
    rv = cint_stat_pp_config_main(ingress_unit, mpls_fwd_swap_statistics_config, bcmStatCounterInterfaceIngressReceivePp, gport, BCM_CORE_ALL, is_lif);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for mpls_fwd_swap_statistics_config\n");
        return rv;
    }

    /*
     * step 3. update the mpls swap entry with stat pp information
     */
    rv = mpls_lsr_ilm_swap_entry_replace_with_stat_pp(ingress_unit, out_sys_port, cint_ipv4_route_info.in_rif);
    if (rv != BCM_E_NONE)
    {
        printf("mpls_lsr_ilm_swap_entry_replace_with_stat_pp failed\n");
        return rv;
    }

    return rv;
}

/*
 *  MPLS LSR NOP Stat PP
 */
int cint_stat_pp_mpls_fwd_nop_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port,
    int is_ingress_push)
{
    int rv = BCM_E_NONE, eedb_index = 0x3000;
    bcm_gport_t gport;
    int is_lif = FALSE;
    int fec;

    /*
     * step 1. Counfigure statistics for mpls fwd NOP
     */
    rv = cint_stat_pp_config_main(ingress_unit, mpls_fwd_nop_statistics_config, bcmStatCounterInterfaceIngressReceivePp, gport, BCM_CORE_ALL, is_lif);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for mpls_fwd_nop_statistics_config\n");
        return rv;
    }

    /*
     * step 2.MPLS LSR NOP
     */
    rv = mpls_tunnel_switch_push_example(ingress_unit, egress_unit, in_sys_port, out_sys_port, is_ingress_push, eedb_index);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_tunnel_switch_push_example\n");
        return rv;
    }

    return rv;
}

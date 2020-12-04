/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_srv6_alternate_marking.c.
 */

/*
 * This cint is an example of Alternate Marking (RFC8321) of Ingress Node on SRv6.
 * It contains IPv4 forwarding into Srv6 tunnel with Alternate Marking encapsulation.
 *
 * The Alternate Marking setting are on the flow label in IPv6 header.
 * - flow_label[15:0] is the flow id
 * - flow_label[16] is D bit
 * - flow_label[17] is L bit
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/field/cint_field_utils.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../src/./examples/dnx/srv6/cint_srv6_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/./examples/dnx/srv6/cint_srv6_alternate_marking.c
 * cint ../../src/./examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/./examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/./examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/./examples/dnx/field/cint_field_srv6_alternate_marking.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint
 * srv6_alternate_marking_ingress_node_example(0,200,202,201,4,0,0,0,0);
 * exit;
 *
 * cint
 * cint_field_srv6_alternate_marking_period_change(0,0,0);
 * exit;
 *
 * 1. SRv6 Ingress Tunnel packet with L=0, D=1
 *     tx 1 psrc=200 data=0x000c0002000000000700010081000000080045000049000000008006fa2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Should recieve 2 packets: forward copy + snoop copy. ToD timestamp in the snooped copy.
 *     Fwd copy:
 *         Data: 0x03d80000000800000268000000000000000009a62a9608000800000000000007fa000200000005b50000000000cd1d00123456789a8100006486dd6001123400912b80123456789abceeffffeecba9876543211234567887654321a5a5a5a55a5a5a5a0408040303000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff11112222111122223333444455556666777788881234567887654321a5a5a5a55a5a5a5a45000049000000007f06fb2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f10111213
 *     snoop copy:
 *         Data: 0x00000000cd1d00123456789a8100006486dd6001123400912b80123456789abceeffffeecba9876543211234567887654321a5a5a5a55a5a5a5a0408040303000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff11112222111122223333444455556666777788881234567887654321a5a5a5a55a5a5a5a45000049000000007f06fb2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 2. SRv6 Ingress Tunnel packet with L=0, D=0
 *     tx 1 psrc=200 data=0x000c0002000000000700010081000000080045000049000000008006fa2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *     Fwd copy:
 *         Data: 0x00000000cd1d00123456789a8100006486dd6000123400912b80123456789abceeffffeecba9876543211234567887654321a5a5a5a55a5a5a5a0408040303000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff11112222111122223333444455556666777788881234567887654321a5a5a5a55a5a5a5a45000049000000007f06fb2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * cint
 * cint_field_srv6_alternate_marking_period_change(0,0,1);
 * exit;
 *
 * 3. SRv6 Ingress Tunnel packet with L=1, D=1
 *     tx 1 psrc=200 data=0x000c0002000000000700010081000000080045000049000000008006fa2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Should recieve 2 packets: forward copy + snoop copy. ToD timestamp in the snooped copy.
 *     Fwd copy:
 *         Data: 0x03d8000000080000026800000000000000000969d46fce000800000000000007fa000200000005b50000000000cd1d00123456789a8100006486dd6003123400912b80123456789abceeffffeecba9876543211234567887654321a5a5a5a55a5a5a5a0408040303000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff11112222111122223333444455556666777788881234567887654321a5a5a5a55a5a5a5a45000049000000007f06fb2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f10111213
 *     snoop copy:
 *         Data: 0x00000000cd1d00123456789a8100006486dd6003123400912b80123456789abceeffffeecba9876543211234567887654321a5a5a5a55a5a5a5a0408040303000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff11112222111122223333444455556666777788881234567887654321a5a5a5a55a5a5a5a45000049000000007f06fb2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 4. SRv6 Ingress Tunnel packet with L=1, D=0
 *     tx 1 psrc=200 data=0x000c0002000000000700010081000000080045000049000000008006fa2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *     Fwd copy:
 *         Data: 0x00000000cd1d00123456789a8100006486dd6002123400912b80123456789abceeffffeecba9876543211234567887654321a5a5a5a55a5a5a5a0408040303000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff11112222111122223333444455556666777788881234567887654321a5a5a5a55a5a5a5a45000049000000007f06fb2bc08001017fffff021f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */

struct cint_srv6_am_params_s{
    int stat_id;
    int loss_KeyGenVar_period;
    int no_loss_KeyGenVar_period;
    int crps_database_id;
    int core_id;
};

/** Used for coordinating between setting the counter
 *  processor and getting the counters. */
struct cint_srv6_am_crps_params_s{
    uint32 total_nof_counters;
    int core;
    int engine;
};

/**
 * \brief IPT profile IDs
 */
int CINT_INSTRU_IPT_TAIL_EDIT_PROFILE              = 1;

/*
 * Default values used for setting back defaults
 */
bcm_instru_ipt_t default_tail_edit_config;
int default_switch_id;


cint_srv6_am_params_s cint_srv6_alternate_marking_params = {
     /*
      * stat_id
      */
     1,
     /*
      * loss_KeyGenVar_period,   no_loss_KeyGenVar_period
      */
     1,                            0, /** In this example using the first bit of KeyGenVar, i.e. profile 1, so using values 0 and 1 */
     0,0 /* Database-id, core ID to be filled by mpls_deep_stack_alternate_marking_crps()*/
};

cint_srv6_am_crps_params_s cint_srv6_alternate_marking_crps_params = {
    0, /* filled after creating the counters*/
    0,
    3, /* use engine 3*/
};
int cint_srv6_am_flow_id = 0x1234;
/*
 * 0 - fwd on ipv4; 1 - fwd on ipv6
 */
int cint_srv6_am_fwd_type = 0;
bcm_field_entry_t ipmf2_entry_handle;
bcm_field_entry_t ipmf3_entry_handle_L_bit_1;
bcm_field_entry_t ipmf3_entry_handle_L_bit_0;

int
cint_instru_ipt_main(
   int unit,
   int switch_id,
   int shim_type)
{
    int rv;
    uint32 ipt_profile;
    bcm_instru_ipt_t config;
    uint32 supported_flags_per_device = 0;

    /* Following flags shared between all devices:*/
    supported_flags_per_device = BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_TOD | BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_SYS_PORT |
            BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_TOD | BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_SYS_PORT |
            BCM_INSTRU_IPT_METADATA_FLAG_SWITCH_ID | BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO;

    if (*dnxc_data_get (unit,"instru","ipt","advanced_ipt",NULL))
    {
        /* Advanced IPT flags*/
        supported_flags_per_device |= BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_SIZE_FG | BCM_INSTRU_IPT_METADATA_FLAG_LATENCY| BCM_INSTRU_IPT_METADATA_FLAG_PORT_RATE
            | BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_SIZE_CG;

    } else {
        /* Basic IPT flags*/
        supported_flags_per_device |= BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_DEV_ID |
                BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_RESERVED |  BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_DEV_ID;
    }


    /** set tail-edit profile */
    {
        ipt_profile = CINT_INSTRU_IPT_TAIL_EDIT_PROFILE;

        /** get default */
        rv  = bcm_instru_ipt_profile_get(unit, 0, ipt_profile, &default_tail_edit_config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_get failed for Tail Edit\n");
            return rv;
        }

        bcm_instru_ipt_t_init(&config);

        config.node_type = bcmInstruIptNodeTypeTail;
        config.metadata_flags = supported_flags_per_device;

        rv  = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, &config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_set failed for Tail Edit\n");
            return rv;
        }
    }

    if (0==*dnxc_data_get (unit,"instru","ipt","advanced_ipt",NULL))
    {
        /* Following configurations also use "ingress resereved" flags (Jer2 devices only)*/
        supported_flags_per_device |= BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_RESERVED;
    }



    /** get default switch id */
    rv = bcm_instru_control_get(unit, 0, bcmInstruControlIptSwitchId, &default_switch_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_control_get failed for setting switch id\n");
        return rv;
    }

    /** set switch ID */
    rv = bcm_instru_control_set(unit, 0, bcmInstruControlIptSwitchId, switch_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_control_set failed for setting switch id\n");
        return rv;
    }

    return rv;
}


/* Add CRPS configuration */
int
srv6_alternate_marking_crps(
    int unit,
    int in_port)
{
    int rv = BCM_E_NONE;
    bcm_stat_pp_profile_info_t pp_profile;
    int core_id,  tm_port;
    int counter_base = 0;
    int database_id = 0;
    bcm_stat_engine_t engine;

    rv = get_core_and_tm_port_from_port(unit, in_port, &core_id, &tm_port);
    if (rv != BCM_E_NONE) {
       printf("Error, in get_core_and_tm_port_from_port\n");
       return rv;
    }
    cint_srv6_alternate_marking_crps_params.core = core_id;

    rv = crps_oam_database_set(unit, core_id, bcmStatCounterInterfaceIngressOam, g_cint_field_srv6_am_ing_ent.command_id , 1,
                               &(cint_srv6_alternate_marking_crps_params.engine), counter_base, database_id,
                               &cint_srv6_alternate_marking_crps_params.total_nof_counters);
    if (rv != BCM_E_NONE)
    {
        printf("Error, crps_oam_database_set \n");
        return rv;
    }

    /* For testing purposes, use stat-id 8*/
    cint_srv6_alternate_marking_params.stat_id = 8;
    cint_srv6_alternate_marking_params.core_id = core_id;
    cint_srv6_alternate_marking_params.crps_database_id = database_id;

    bcm_stat_engine_t_init(&engine);
    /* Set the eviction rate to 0.25 milliseconds per counter.
       8K counters in this engine, so this comes out to ~2 seconds to evict the whole engine. */
    engine.engine_id = cint_srv6_alternate_marking_crps_params.engine;
    engine.core_id = core_id;
    rv = bcm_stat_counter_engine_control_set(unit,0,engine,bcmStatCounterSequentialSamplingInterval,250);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_stat_counter_engine_control_set \n");
        return rv;
    }

    return rv;
}



/**
 * Snooping configuration for alternate marking ingress node:
 * 1. Set snoop destination
 * 2. Set tail edit profile which appends the tail edit header
 *
 *
 * @param unit
 * @param snoop_cmnd
 *
 * @return int
 */
int srv6_alternate_marking_snoop(
    int unit,
    int snoop_cmnd)
{
    bcm_mirror_header_info_t mirror_header_info;
    bcm_mirror_destination_t mirror_dest;
    bcm_port_config_t port_config;
    int cpu_port;
    bcm_instru_ipt_t config;


    /** First, get the CPU port */
    bcm_mirror_destination_t_init(&mirror_dest);
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_config));
    BCM_PBMP_ITER(port_config.cpu, cpu_port)
    {
        mirror_dest.gport = cpu_port;
        break;
    }

    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP  | BCM_MIRROR_DEST_WITH_ID |BCM_MIRROR_DEST_REPLACE ;
    mirror_dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;
    BCM_GPORT_MIRROR_SET(mirror_dest.mirror_dest_id, snoop_cmnd);
    BCM_IF_ERROR_RETURN(bcm_mirror_destination_create(unit, &mirror_dest));

    /** Now set tail-edit-profile on the system headers */
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.pp.tail_edit_profile = CINT_INSTRU_IPT_TAIL_EDIT_PROFILE;
    mirror_header_info.tm.ase_ext.ase_type = bcmPktDnxAseTypeInt;
    mirror_header_info.tm.ase_ext.valid = 1;
    BCM_IF_ERROR_RETURN(bcm_mirror_header_info_set(unit,BCM_MIRROR_DEST_IS_SNOOP,mirror_dest.mirror_dest_id,&mirror_header_info));

    return 0;
}

/*
 * Add ingress node PMF configuration per 5-tuple defining flow ID.
 *
 * Ingress PMF:
 * - Per flow (5-tuple), adding one PMF entries in iPMF2, assign a flow ID.
 * - Per flow ID, adding two PMF entries in iPMF3, each action assigns a different counter.
 *
 * Egress PMF:
 * - ASE check and generate snoop copy
 */
int
srv6_alternate_marking_ingress_node_field(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_t entry_handle;

    /*
    * Configure cint params. First the entry for L bit =1.
    */
    g_cint_field_srv6_am_ing_ent.SrcIp = 0xc0800101;
    g_cint_field_srv6_am_ing_ent.DstIp = 0x7fffff02;
    g_cint_field_srv6_am_ing_ent.ProtocolType = 6; /*TCP*/
    g_cint_field_srv6_am_ing_ent.L4Dst = 9000;
    g_cint_field_srv6_am_ing_ent.L4Src = 8000;

    rv = cint_field_srv6_alternate_marking_ingress_node_iPMF2(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_srv6_alternate_marking_ingress_node_iPMF2 \n", rv);
        return rv;
    }

    rv = cint_field_srv6_alternate_marking_ingress_node_iPMF2_entry_add(unit, &ipmf2_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_srv6_alternate_marking_ingress_node_iPMF2_entry_add \n", rv);
        return rv;
    }

    printf("Ingress node iPMF2 configuration Done! \n\n");

    rv = cint_field_srv6_alternate_marking_ingress_node_iPMF3(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_srv6_alternate_marking_ingress_node_iPMF3 \n", rv);
        return rv;
    }

    g_cint_field_srv6_am_ing_ent.KeyGenVar_period = cint_srv6_alternate_marking_params.loss_KeyGenVar_period;
    g_cint_field_srv6_am_ing_ent.statId0 = 1; /* Counter 1*/
    g_cint_field_srv6_am_ing_ent.l_bit_set = 1;

    rv = cint_field_srv6_alternate_marking_ingress_node_iPMF3_entry_add(unit, &ipmf3_entry_handle_L_bit_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_srv6_alternate_marking_ingress_node_iPMF3_entry_add \n", rv);
        return rv;
    }


    /* Alternate between the two counters per flow - Now configure the entry with L bit =0.*/
    /*For eviction purposes in the CRPS, its best to take counters as far away from each other.
    In this example we assume
    L=1 range: 1- total_nof_counters/2
    L=0 range: total_nof_counters/2 + 1 - total_nof_counters */
    g_cint_field_srv6_am_ing_ent.statId0  = cint_srv6_alternate_marking_crps_params.total_nof_counters /2 + 1;
    g_cint_field_srv6_am_ing_ent.KeyGenVar_period = cint_srv6_alternate_marking_params.no_loss_KeyGenVar_period;
    g_cint_field_srv6_am_ing_ent.l_bit_set = 0;

    rv = cint_field_srv6_alternate_marking_ingress_node_iPMF3_entry_add(unit, &ipmf3_entry_handle_L_bit_0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_srv6_alternate_marking_ingress_node_iPMF3_entry_add \n", rv);
        return rv;
    }

    printf("Ingress node iPMF3 configuration Done! \n\n");

    if (g_cint_field_srv6_am_ing_ent.is_ext_encap == 1) {
        rv = cint_field_srv6_ext_alternate_marking_ingress_node_ePMF(unit);
    }
    else {
        rv = cint_field_srv6_alternate_marking_ingress_node_ePMF(unit);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_srv6_alternate_marking_ingress_node_ePMF \n", rv);
        return rv;
    }
    printf("Ingress node ePMF configuration Done! \n\n");

    return rv;
}

/*
 * An example of Alternate Marking (RFC8321) of Ingress Node on SRv6
 * This example is only for SRv6 packet with 1-pass solution
 *
 * @param unit
 * @param in_port                 - Incoming port of SRV6 Ingress Tunnel
 * @param in_port_eth             - Incoming ETH port of SRV6 Ingress Tunnel
 * @param out_port                - Outgoing port of SRV6 Ingress Tunnel 
 * @param nof_sids
 * @param secondary_srh_flags     - additional SRH Encap flags such as BCM_SRV6_SRH_BASE_INITIATOR_UNIFIED
 * @param secondary_sids_flags    - additional SIDs Encap flags such as BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED
 * @param is_t_insert_encap
 * @param last_sid_is_dip         - if set, last SID (VPN) is given by IPv6 tunnel encapsulation
 *
 * @return int
 */
int
srv6_alternate_marking_ingress_node_example (
    int unit,
    int in_port,
    int in_port_eth,
    int out_port,
    int nof_sids,
    int secondary_srh_flags,
    int secondary_sids_flags,
    int is_t_insert_encap,
    int last_sid_is_dip)
{
    int rv = BCM_E_NONE;

    rv = srv6_ingress_tunnel_config(unit, in_port, in_port_eth, out_port, nof_sids, secondary_srh_flags, secondary_sids_flags, is_t_insert_encap, last_sid_is_dip);
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_ingress_tunnel_config\n");
        return rv;
    }

    /*
     * Add CRPS configuration
     */
    rv = srv6_alternate_marking_crps(unit, in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_alternate_marking_crps\n");
        return rv;
    }

    /*
     * Configure PMF rules
     */
    rv = srv6_alternate_marking_ingress_node_field(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_alternate_marking_ingress_node_field\n");
        return rv;
    }

    rv = srv6_alternate_marking_snoop(unit, cint_field_srv6_am_egress_snoop_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_alternate_marking_snoop\n");
        return rv;
    }

    /*
    * Timestamp information when D bit is set is gotten through meta data.
    * Switch id parameter is the node ID field.
    */
    BCM_IF_ERROR_RETURN(cint_instru_ipt_main(unit, 0xab, 0));

    return rv;
}

/*
 * An example of Alternate Marking (RFC8321) of Ingress Node on SRv6
 * This example is only for SRv6 packet with extended encaupsulation solution
 *
 * @param unit
 * @param in_port                 - Incoming port of SRV6 Ingress Tunnel
 * @param out_port                - Outgoing port of SRV6 Ingress Tunnel 
 * @param rch_port                - Recycle Port
 * @param nof_sids_first_pass     - Number of SIDs encapsulated in first pass
 * @param nof_sids_second_pass    - Number of SIDs encapsulated in second pass
 * @param nof_sids_third_pass     - Number of SIDs encapsulated in third pass
 *
 * @return int
 */
int
srv6_ext_encap_alternate_marking_ingress_node_example (
    int unit,
    int in_port,
    int out_port,
    int rch_port,
    int nof_sids_first_pass,
    int nof_sids_second_pass,
    int nof_sids_third_pass)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport_out;

    BCM_GPORT_LOCAL_SET(gport_out, out_port);

    g_cint_field_srv6_am_ing_ent.is_ext_encap = 1;
    g_cint_field_srv6_am_ing_ent.nof_sid_1st_pass = nof_sids_first_pass;

    /*
     * Configuring Extended ISR (Ingress SRv6 extednded encapsulation Tunnel)
     * This will create encapsulation of SRH,SIDs and IPv4 onto the packet
     */
    rv = srv6_ingress_tunnel_ext_encap_config(unit, in_port, out_port, rch_port, nof_sids_first_pass, nof_sids_second_pass, nof_sids_third_pass);   
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_ingress_tunnel_ext_encap_config\n");
        return rv;
    }

    /*
     * Add CRPS configuration
     */
    rv = srv6_alternate_marking_crps(unit, in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_alternate_marking_crps\n");
        return rv;
    }

    /*
     * Set pmf data for each out port of Srv6 packet.
     * This pmf data is used to distinguish between recycle port and out port of Srv6 packet by epmf presel
     * See cint_field_srv6_ext_alternate_marking_ingress_node_ePMF
     */
    g_cint_field_srv6_am_ing_ent.egr_port_pmf_data = 0x3;
    rv = bcm_port_class_set(unit, out_port, bcmPortClassFieldEgressPacketProcessingPortCs, g_cint_field_srv6_am_ing_ent.egr_port_pmf_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_alternate_marking_snoop\n");
        return rv;
    }

    /*
     * Configure PMF rules
     */
    rv = srv6_alternate_marking_ingress_node_field(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_alternate_marking_ingress_node_field\n");
        return rv;
    }

    /*
     * Configure Snooping for alternate marking ingress node
     */
    rv = srv6_alternate_marking_snoop(unit, cint_field_srv6_am_egress_snoop_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_alternate_marking_snoop\n");
        return rv;
    }

    /*
    * Timestamp information when D bit is set is gotten through meta data.
    * Switch id parameter is the node ID field.
    */
    BCM_IF_ERROR_RETURN(cint_instru_ipt_main(unit, 0xab, 0));

    return rv;
}

/**
 * Get the counter value for the session configured in this
 * cint.
 * Two separate counters may be gotten - with the L bit set and
 * without.
 * Note that getting the counter while traffic is running will
 * result in inconsistent behavior in the counter processor!
 * When L bit is set through
 * cint_field_alternate_marking_period_change(), call this API
 * with L bit 0 and the other way around.
 *
 *
 * @param unit
 * @param l_bit_set
 * @param counter
 * @param ingress_node
 *
 * @return int
 */
int srv6_alternate_marking_get_counter(int unit, int l_bit_set, int * counter) {
    bcm_stat_counter_input_data_t input_data;
    bcm_stat_counter_output_data_t output_data;
    int stat_array[64];
    const uint32 *max_counter_set_size = dnxc_data_get(unit, "crps", "engine", "max_counter_set_size", NULL);
    bcm_stat_counter_value_t counter_value[(*max_counter_set_size) * 2];
    int source_id;
    uint64 counter_value_64b;

    bcm_stat_counter_input_data_t_init(&input_data);
    bcm_stat_counter_output_data_t_init(&output_data);

    /* Counters used for the entry set in this cint: 1 with L bit set, total_nof_counters/2 without*/
    source_id = l_bit_set ? 1 : cint_srv6_alternate_marking_crps_params.total_nof_counters/2 + 1 ;

    stat_array[0] = BCM_STAT_COUNTER_STAT_SET(0,0);
    input_data.core_id = cint_srv6_alternate_marking_crps_params.core;
    input_data.database_id = 0;
    input_data.type_id = 0;
    input_data.counter_source_id = source_id;
    input_data.nstat = 1;
    input_data.stat_arr = stat_array;

    output_data.value_arr = counter_value;

    BCM_IF_ERROR_RETURN(bcm_stat_counter_get(unit, BCM_STAT_COUNTER_CLEAR_ON_READ, &input_data, &output_data));

    counter_value_64b = output_data.value_arr[0].value;

    COMPILER_64_TO_32_LO(*counter,counter_value_64b);

    return 0;

}


/**
 * Function should be called immediatly after
 * cint_field_alternate_marking_period_change(), before reading
 * the counter.
 *
 * This enables reading the inactive counter i.e. if
 * transmitting with the L bit set enable reading the counters
 * with L bit unset.
 *
 * Counters should be read after a sufficient period of time has
 * passed after this function was called to allow eviction.
 *
 * @param unit
 * @param new_L_bit - set to 1 when switching from packets with
 *                  L=0 to L=1. Set to 0 when going the other
 *                  way.
 *
 * @return int
 */
int switch_counter_procedure_before_reading(int unit, int active_L_bit){
    bcm_stat_counter_database_t database;
    bcm_stat_eviction_boundaries_t boundaries;
    bcm_stat_counter_enable_t enable_config;

    bcm_stat_eviction_boundaries_t_init(&boundaries);
    bcm_stat_counter_database_t_init(&database);
    bcm_stat_counter_enable_t_init(&enable_config);

    database.database_id = cint_srv6_alternate_marking_params.crps_database_id;
    database.core_id = cint_srv6_alternate_marking_params.core_id;

    /* Step 1: Set the eviction boundry on the active counter. */
    if (active_L_bit)
    {
        /* Going from L=0 to L=1.
           Need to enable eviction on the L=0 range, disable on L=1 range.
           This means evict on the range
           total_nof_counters /2  to total_nof_counters.
           */
        boundaries.start = cint_srv6_alternate_marking_crps_params.total_nof_counters / 2;
        boundaries.end = cint_srv6_alternate_marking_crps_params.total_nof_counters - 1;
    } else
    {
        /* Other way around.
           0 to total_nof_counters/2*/
        boundaries.start = 0;
        boundaries.end = cint_srv6_alternate_marking_crps_params.total_nof_counters / 2 - 1;
    }
    BCM_IF_ERROR_RETURN(bcm_stat_database_eviction_boundaries_set(unit,0,&database,0,&boundaries)); /* tpye ID is 0 for OAM counters*/

    /* Step 2: Enable eviction on the engine.
       Becasue of step 1, eviction will only take place in the inactive range.*/

    enable_config.enable_counting = TRUE; /* Set to true in any case*/
    enable_config.enable_eviction = TRUE;
    BCM_IF_ERROR_RETURN(bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config));



    return 0;
}

/**
 * Procedure for reading the inactive counter. 3 steps:
 * 1. Set eviction boundaries to disable eviction on active
 * counter range.
 *
 * 2. Enable eviction on the engine. Due to step 1 eviction will
 * only be enabled on the inactive range
 *
 * 3. Wait. This is required for eviction to occur (SW counters
 * get the HW counters)
 *
 * 4. Read the inactive counter
 *
 * 5. Disable eviction globally
 *
 * @param unit
 * @param active_L_bit - Read the opposite counter
 * @param counter
 *
 * @return int
 */
int read_counter_and_handle_eviction(
    int unit,
    int active_L_bit,
    int * counter)
{
    bcm_stat_counter_enable_t enable_config;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_database_t_init(&database);
    bcm_stat_counter_enable_t_init(&enable_config);

    BCM_IF_ERROR_RETURN(switch_counter_procedure_before_reading(unit, active_L_bit));

    printf("Succesfully set eviction boundaries and enabled eviction. Going to sleep to allow reading the counters.");
    sal_sleep(2); /* Wait for eviction to take place*/

    BCM_IF_ERROR_RETURN(srv6_alternate_marking_get_counter(unit, !active_L_bit, counter));
    printf("Read the counter, value is %d. Now disabling eviction back\n",*counter);


    /* Now disable eviction back on all the range.
       Otherwise, when we switch back the L bit counters will not be stamped on ASE and packet flow will be incorrect.*/
    database.database_id = cint_srv6_alternate_marking_params.crps_database_id;
    database.core_id = cint_srv6_alternate_marking_params.core_id;
    enable_config.enable_counting = TRUE; /* Set to true in any case*/
    enable_config.enable_eviction = FALSE;
    BCM_IF_ERROR_RETURN(bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config));

    return 0;
}


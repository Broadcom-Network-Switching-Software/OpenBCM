/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_ipv6_alternate_marking.c.
 */

/*
 * This cint is an example of Alternate Marking (RFC8321) of Ingress Node on IPv6.
 * It contains IPv4 forwarding into Srv6 tunnel with Alternate Marking encapsulation.
 *
 * The Alternate Marking setting are on the flow label in IPv6 header.
 * - flow_label[15:0] is the flow id
 * - flow_label[16] is D bit, for measuring end-to-end latency between ingress and egress node.
 *       If D=1, then a copy of the packet is generated with timestamp.
 * - flow_label[17] is L bit, for color information.
 *
 * Test Scenario
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/field/cint_field_utils.c
  cint ../../../../src/examples/sand/cint_ip_route_basic.c
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
  cint ../../../../src/examples/dnx/tunnel/cint_dnx_ipv6_tunnel_encapsulation.c
  cint ../../../../src/examples/dnx/l3/cint_ipv6_alternate_marking.c
  cint ../../../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
  cint ../../../../src/examples/dnx/crps/cint_crps_db_utils.c
  cint ../../../../src/examples/dnx/crps/cint_crps_irpp_config.c
  cint ../../../../src/examples/dnx/field/cint_field_ipv6_alternate_marking.c
  cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
  cint
  ipv6_tunnel_alternate_marking_ingress_node_example(0,201,202,6,-1,0);
  exit;
 *
  cint
  cint_field_ipv6_alternate_marking_period_change(0,0,0);
  exit;
 *
 * 1. IPv4 routing into IPv6 tunnel packet with L=0, D=1
      tx 1 psrc=201 data=0x000c00020056005000001234080045000049000000004006e79e0a0a0a047fffff031f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Should recieve 2 packets: forward copy + snoop copy. ToD timestamp in the snooped copy.
 *     Fwd copy:
 *         Data: 0x00020000cd1d000c000200568100a06486dd60a1123400490440ecefeeedecebeae9e8e7e6e5e4e3e2e120010db800000000021122fffe33445545000049000000003f06e89e0a0a0a047fffff031f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *     snoop copy:
 *         Data: 0x02b80000000800000268000000000000000009a17e5108000800000000000007fa000200000005b50000020000cd1d000c000200568100a06486dd60a1123400490440ecefeeedecebeae9e8e7e6e5e4e3e2e120010db800000000021122fffe33445545000049000000003f06e89e0a0a0a047fffff031f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f10111213
 *
 * 2. IPv4 routing into IPv6 tunnel packet with L=0, D=0
      tx 1 psrc=201 data=0x000c00020056005000001234080045000049000000004006e79e0a0a0a047fffff031f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *     Fwd copy:
 *         Data: 0x00020000cd1d000c000200568100a06486dd60a0123400490440ecefeeedecebeae9e8e7e6e5e4e3e2e120010db800000000021122fffe33445545000049000000003f06e89e0a0a0a047fffff031f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
  cint
  cint_field_ipv6_alternate_marking_period_change(0,0,1);
  exit;
 *
 * 3. IPv4 routing into IPv6 tunnel packet with L=1, D=1
      tx 1 psrc=201 data=0x000c00020056005000001234080045000049000000004006e79e0a0a0a047fffff031f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Should recieve 2 packets: forward copy + snoop copy. ToD timestamp in the snooped copy.
 *     Fwd copy:
 *         Data: 0x00020000cd1d000c000200568100a06486dd60a3123400490440ecefeeedecebeae9e8e7e6e5e4e3e2e120010db800000000021122fffe33445545000049000000003f06e89e0a0a0a047fffff031f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *     snoop copy:
 *         Data: 0x02b80000000800000268000000000000000009d07dcfae000800000000000007fa000200000005b50000020000cd1d000c000200568100a06486dd60a3123400490440ecefeeedecebeae9e8e7e6e5e4e3e2e120010db800000000021122fffe33445545000049000000003f06e89e0a0a0a047fffff031f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f10111213
 *
 * 4. Pv4 routing into IPv6 tunnel packet with L=1, D=0
      tx 1 psrc=201 data=0x000c00020056005000001234080045000049000000004006e79e0a0a0a047fffff031f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *     Fwd copy:
 *         Data: 0x00020000cd1d000c000200568100a06486dd60a2123400490440ecefeeedecebeae9e8e7e6e5e4e3e2e120010db800000000021122fffe33445545000049000000003f06e89e0a0a0a047fffff031f40232800000000000000000010001000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Test Scenario - end
 *
 */

struct cint_ipv6_am_params_s{
    int stat_id;
    int loss_KeyGenVar_period;
    int no_loss_KeyGenVar_period;
    int crps_database_id;
    int core_id;
};

/** Used for coordinating between setting the counter
 *  processor and getting the counters. */
struct cint_ipv6_am_crps_params_s{
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


cint_ipv6_am_params_s cint_ipv6_alternate_marking_params = {
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

cint_ipv6_am_crps_params_s cint_ipv6_alternate_marking_crps_params = {
    0, /* filled after creating the counters*/
    0,
    3, /* use engine 3*/
};
int cint_ipv6_am_egress_counter_base = 100;
int cint_ipv6_am_flow_id = 0x1234;
int cint_ipv6_am_D_bit_in_flow_label = 16;
int cint_ipv6_am_L_bit_in_flow_label = 17;

/*
 * 0 - fwd on ipv4; 1 - fwd on ipv6
 *
 */
int cint_ipv6_am_fwd_type = 0;
bcm_field_entry_t ipmf2_entry_handle;
bcm_field_entry_t ipmf3_entry_handle_L_bit_1;
bcm_field_entry_t ipmf3_entry_handle_L_bit_0;

int
cint_instru_ipt_main(
   int unit,
   int switch_id,
   int shim_type)
{
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
    if (!(*dnxc_data_get (unit,"instru","ipt","ipt_mapping_v2",NULL)))
    {
        ipt_profile = CINT_INSTRU_IPT_TAIL_EDIT_PROFILE;

        /** get default */
        BCM_IF_ERROR_RETURN_MSG(bcm_instru_ipt_profile_get(unit, 0, ipt_profile, &default_tail_edit_config), "for Tail Edit");

        bcm_instru_ipt_t_init(&config);

        config.node_type = bcmInstruIptNodeTypeTail;
        config.metadata_flags = supported_flags_per_device;

        BCM_IF_ERROR_RETURN_MSG(bcm_instru_ipt_profile_set(unit, 0, ipt_profile, &config), "for Tail Edit");
    }

    if (0==*dnxc_data_get (unit,"instru","ipt","advanced_ipt",NULL))
    {
        /* Following configurations also use "ingress resereved" flags (Jer2 devices only)*/
        supported_flags_per_device |= BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_RESERVED;
    }



    /** get default switch id */
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_control_get(unit, 0, bcmInstruControlIptSwitchId, &default_switch_id),
        "failed for setting switch id");

    /** set switch ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_control_set(unit, 0, bcmInstruControlIptSwitchId, switch_id),
        "failed for setting switch id");

    return BCM_E_NONE;
}


/* Add CRPS configuration */
int
ipv6_alternate_marking_crps(
    int unit,
    int in_port)
{
    bcm_stat_pp_profile_info_t pp_profile;
    int core_id,  tm_port;
    int counter_base = 0;
    int database_id = 0;
    bcm_stat_engine_t engine;
    bcm_stat_counter_interface_type_t intf_type;
    int L_bit_mapping = *(dnxc_data_get(unit, "instru", "alternate_marking", "L_bit_profile_mapping", NULL));

    BCM_IF_ERROR_RETURN_MSG(get_core_and_tm_port_from_port(unit, in_port, &core_id, &tm_port), "");
    cint_ipv6_alternate_marking_crps_params.core = core_id;

    if(L_bit_mapping)
    {
        intf_type =  bcmStatCounterInterfaceEgressOam;
        /**command must be 1*/
        g_cint_field_ipv6_am_ing_ent.command_id = 1;
    }
    else
    {
        intf_type= bcmStatCounterInterfaceIngressOam;
    }

    BCM_IF_ERROR_RETURN_MSG(crps_oam_database_set(unit, core_id, intf_type, g_cint_field_ipv6_am_ing_ent.command_id , 1,
                               &(cint_ipv6_alternate_marking_crps_params.engine), counter_base, database_id,
                               &cint_ipv6_alternate_marking_crps_params.total_nof_counters), "");

    /* For testing purposes, use stat-id 8*/
    cint_ipv6_alternate_marking_params.stat_id = 8;
    cint_ipv6_alternate_marking_params.core_id = core_id;
    cint_ipv6_alternate_marking_params.crps_database_id = database_id;

    bcm_stat_engine_t_init(&engine);
    /* Set the eviction rate to 0.25 milliseconds per counter.
       8K counters in this engine, so this comes out to ~2 seconds to evict the whole engine. */
    engine.engine_id = cint_ipv6_alternate_marking_crps_params.engine;
    engine.core_id = core_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_stat_counter_engine_control_set(unit,0,engine,bcmStatCounterSequentialSamplingInterval,250), "");

    return BCM_E_NONE;
}



/**
 * Snooping configuration for alternate marking ingress node:
 * 1. Set snoop destination
 * 2. Set tail edit profile which appends the tail edit header
 *
 * @param unit
 * @param snoop_cmnd
 *
 * @return int
 */
int ipv6_alternate_marking_snoop(
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
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &port_config), "");
    BCM_PBMP_ITER(port_config.cpu, cpu_port)
    {
        mirror_dest.gport = cpu_port;
        break;
    }

    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP  | BCM_MIRROR_DEST_WITH_ID;
    mirror_dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;
    BCM_GPORT_MIRROR_SET(mirror_dest.mirror_dest_id, snoop_cmnd);
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_create(unit, &mirror_dest), "");

    /** Now set tail-edit-profile on the system headers */
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.pp.tail_edit_profile = CINT_INSTRU_IPT_TAIL_EDIT_PROFILE;
    mirror_header_info.tm.ase_ext.ase_type = bcmPktDnxAseTypeInt;
    mirror_header_info.tm.ase_ext.valid = 1;
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_header_info_set(unit,BCM_MIRROR_DEST_IS_SNOOP,mirror_dest.mirror_dest_id,&mirror_header_info), "");

    return BCM_E_NONE;
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
 *
 */
int
ipv6_alternate_marking_ingress_node_field(
    int unit)
{
    bcm_field_entry_t entry_handle;
    int L_bit_mapping = *(dnxc_data_get(unit, "instru", "alternate_marking", "L_bit_profile_mapping", NULL));

    /*
    * Configure cint params. First the entry for L bit =1.
    */
    g_cint_field_ipv6_am_ing_ent.SrcIp = 0x0a0a0a04;
    g_cint_field_ipv6_am_ing_ent.DstIp = 0x7fffff03;

    g_cint_field_ipv6_am_ing_ent.SrcIp6Low[1] = 0x1b08b366;
    g_cint_field_ipv6_am_ing_ent.SrcIp6Low[0] = 0x4d4141fc;
    g_cint_field_ipv6_am_ing_ent.DstIp6Low[1] = 0x6957bf33;
    g_cint_field_ipv6_am_ing_ent.DstIp6Low[0] = 0x1944a2c0;

    g_cint_field_ipv6_am_ing_ent.ProtocolType = 6; /*TCP*/
    g_cint_field_ipv6_am_ing_ent.L4Dst = 9000;
    g_cint_field_ipv6_am_ing_ent.L4Src = 8000;

    if (!L_bit_mapping)
    {
        BCM_IF_ERROR_RETURN_MSG(cint_field_ipv6_alternate_marking_ingress_node_iPMF2(unit), "");
    
        BCM_IF_ERROR_RETURN_MSG(cint_field_ipv6_alternate_marking_ingress_node_iPMF2_entry_add(unit, &ipmf2_entry_handle), "");
    
        printf("Ingress node iPMF2 configuration Done! \n\n");
    
        BCM_IF_ERROR_RETURN_MSG(cint_field_ipv6_alternate_marking_ingress_node_iPMF3(unit), "");
    
        g_cint_field_ipv6_am_ing_ent.KeyGenVar_period = cint_ipv6_alternate_marking_params.loss_KeyGenVar_period;
        g_cint_field_ipv6_am_ing_ent.statId0 = 1; /* Counter 1*/
        g_cint_field_ipv6_am_ing_ent.l_bit_set = 1;
    
        BCM_IF_ERROR_RETURN_MSG(cint_field_ipv6_alternate_marking_ingress_node_iPMF3_entry_add(unit, &ipmf3_entry_handle_L_bit_1), "");
    
    
        /* Alternate between the two counters per flow - Now configure the entry with L bit =0.*/
        /*For eviction purposes in the CRPS, its best to take counters as far away from each other.
        In this example we assume
        L=1 range: 1- total_nof_counters/2
        L=0 range: total_nof_counters/2 + 1 - total_nof_counters */
        g_cint_field_ipv6_am_ing_ent.statId0  = cint_ipv6_alternate_marking_crps_params.total_nof_counters /2 + 1;
        g_cint_field_ipv6_am_ing_ent.KeyGenVar_period = cint_ipv6_alternate_marking_params.no_loss_KeyGenVar_period;
        g_cint_field_ipv6_am_ing_ent.l_bit_set = 0;
    
        BCM_IF_ERROR_RETURN_MSG(cint_field_ipv6_alternate_marking_ingress_node_iPMF3_entry_add(unit, &ipmf3_entry_handle_L_bit_0), "");
    
        printf("Ingress node iPMF3 configuration Done! \n\n");
    }

    BCM_IF_ERROR_RETURN_MSG(cint_field_ipv6_alternate_marking_ingress_node_ePMF(unit), "");
    printf("Ingress node ePMF configuration Done! \n\n");

    return BCM_E_NONE;
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
 * @param unit
 * @param l_bit_set
 * @param counter
 * @param ingress_node
 *
 * @return int
 */
int ipv6_alternate_marking_get_counter(int unit, int l_bit_set, int * counter) {
    bcm_stat_counter_input_data_t input_data;
    bcm_stat_counter_output_data_t output_data;
    int stat_array[64];
    const uint32 *max_counter_set_size = dnxc_data_get(unit, "crps", "engine", "max_counter_set_size", NULL);
    bcm_stat_counter_value_t counter_value[(*max_counter_set_size) * 2];
    int source_id;
    uint64 counter_value_64b;
    int L_bit_mapping = *(dnxc_data_get(unit, "instru", "alternate_marking", "L_bit_profile_mapping", NULL));

    bcm_stat_counter_input_data_t_init(&input_data);
    bcm_stat_counter_output_data_t_init(&output_data);

    if (L_bit_mapping)
    {
        /** cint_ipv6_am_egress_counter_base, is set by ePMF in cint_field_alternate_marking.c*/
        source_id = cint_ipv6_am_egress_counter_base + l_bit_set;
    }
    else
    {
        /* Counters used for the entry set in this cint: 1 with L bit set, total_nof_counters/2 without*/
        source_id = l_bit_set ? 1 : cint_ipv6_alternate_marking_crps_params.total_nof_counters/2 + 1 ;
    }

    stat_array[0] = BCM_STAT_COUNTER_STAT_SET(0,0);
    input_data.core_id = cint_ipv6_alternate_marking_crps_params.core;
    input_data.database_id = 0;
    input_data.type_id = 0;
    input_data.counter_source_id = source_id;
    input_data.nstat = 1;
    input_data.stat_arr = stat_array;

    output_data.value_arr = counter_value;

    BCM_IF_ERROR_RETURN_MSG(bcm_stat_counter_get(unit, BCM_STAT_COUNTER_CLEAR_ON_READ, &input_data, &output_data), "");

    counter_value_64b = output_data.value_arr[0].value;

    COMPILER_64_TO_32_LO(*counter,counter_value_64b);

    return BCM_E_NONE;

}

/*
 * An example of Alternate Marking (RFC8321) of Ingress Node on IPv6 Tunnel
 *
 * @param unit
 * @param access_port             - access port
 * @param provider_port           - Outgoing port of IPv6 Ingress Tunnel
 * @param tunnel_type             - can be bcmTunnelTypeIpAnyIn6 or bcmTunnelTypeGreAnyIn6
 * @param gre_type                - -1-No Gre, 0-Gre4, 1-Gre8
 * @param forward_type            - 0-IPv4, 1-IPv6
 *
 * @return int
 */
int
ipv6_tunnel_alternate_marking_ingress_node_example (
    int unit,
    int access_port,
    int provider_port,
    bcm_tunnel_type_t tunnel_type,
    int gre_type /* -1-No Gre, 0-Gre4, 1-Gre8 */,
    int forward_type /* 0-Ipv4, 1-Ipv6*/)
{
    int L_bit_mapping = *(dnxc_data_get(unit, "instru", "alternate_marking", "L_bit_profile_mapping", NULL));

    if (tunnel_type != bcmTunnelTypeIpAnyIn6 && tunnel_type != bcmTunnelTypeGreAnyIn6) {
        printf("tunnel_type(%d) is not supported\n", tunnel_type);
        return BCM_E_PARAM;
    }

    if (forward_type !=0 && forward_type != 1) {
        printf("forward_type(%d) is not supported\n", forward_type);
        return BCM_E_PARAM;
    }
    cint_ipv6_am_fwd_type = forward_type;

    if (L_bit_mapping)
    {
        CINT_INSTRU_IPT_TAIL_EDIT_PROFILE = 2;
    }

    /*
     * Setup IPv4/IPv6 UC servie, routing into the IPv6 tunnel
     */
    cint_ipv6_tunnel_info.tunnel_type = tunnel_type;
    BCM_IF_ERROR_RETURN_MSG(ipv6_tunnel_encapsulation(unit, access_port, provider_port, forward_type, gre_type), "");

    /*
     * Add CRPS configuration
     */
    BCM_IF_ERROR_RETURN_MSG(ipv6_alternate_marking_crps(unit, access_port), "");

    /*
     * Configure PMF rules
     */
    BCM_IF_ERROR_RETURN_MSG(ipv6_alternate_marking_ingress_node_field(unit), "");

    BCM_IF_ERROR_RETURN_MSG(ipv6_alternate_marking_snoop(unit, cint_field_ipv6_am_egress_snoop_id), "");

    /*
    * Timestamp information when D bit is set is gotten through meta data.
    * Switch id parameter is the node ID field.
    */
    BCM_IF_ERROR_RETURN_MSG(cint_instru_ipt_main(unit, 0xab, 0), "");

    return BCM_E_NONE;
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

    database.database_id = cint_ipv6_alternate_marking_params.crps_database_id;
    database.core_id = cint_ipv6_alternate_marking_params.core_id;

    /* Step 1: Set the eviction boundry on the active counter. */
    if (active_L_bit)
    {
        /* Going from L=0 to L=1.
           Need to enable eviction on the L=0 range, disable on L=1 range.
           This means evict on the range
           total_nof_counters /2  to total_nof_counters.
           */
        boundaries.start = cint_ipv6_alternate_marking_crps_params.total_nof_counters / 2;
        boundaries.end = cint_ipv6_alternate_marking_crps_params.total_nof_counters - 1;
    } else
    {
        /* Other way around.
           0 to total_nof_counters/2*/
        boundaries.start = 0;
        boundaries.end = cint_ipv6_alternate_marking_crps_params.total_nof_counters / 2 - 1;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_stat_database_eviction_boundaries_set(unit,0,&database,0,&boundaries), ""); /* tpye ID is 0 for OAM counters*/

    /* Step 2: Enable eviction on the engine.
       Becasue of step 1, eviction will only take place in the inactive range.*/

    enable_config.enable_counting = TRUE; /* Set to true in any case*/
    enable_config.enable_eviction = TRUE;
    BCM_IF_ERROR_RETURN_MSG(bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config), "");



    return BCM_E_NONE;
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

    BCM_IF_ERROR_RETURN_MSG(switch_counter_procedure_before_reading(unit, active_L_bit), "");

    printf("Succesfully set eviction boundaries and enabled eviction. Going to sleep to allow reading the counters.");
    sal_sleep(2); /* Wait for eviction to take place*/

    BCM_IF_ERROR_RETURN_MSG(ipv6_alternate_marking_get_counter(unit, !active_L_bit, counter), "");
    printf("Read the counter, value is %d. Now disabling eviction back\n",*counter);


    /* Now disable eviction back on all the range.
       Otherwise, when we switch back the L bit counters will not be stamped on ASE and packet flow will be incorrect.*/
    database.database_id = cint_ipv6_alternate_marking_params.crps_database_id;
    database.core_id = cint_ipv6_alternate_marking_params.core_id;
    enable_config.enable_counting = TRUE; /* Set to true in any case*/
    enable_config.enable_eviction = FALSE;
    BCM_IF_ERROR_RETURN_MSG(bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config), "");

    return BCM_E_NONE;
}

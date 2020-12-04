/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_oam_performance_measurement.c Purpose: basic examples for OAM loss and delay measurement.
 */

/* Short group indication */
int is_short_maid = 1;
int is_jumbo_dm = 0;
bcm_oam_endpoint_t oam_perf_up_mep_id = 4096;
bcm_oam_endpoint_t oam_perf_down_mep_id = 4100;
bcm_mac_t src_mac_up = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
bcm_mac_t src_mac_down = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x01};
int nof_mep = 0;
int jumbo_with_Id = 1;

/** LM/DM reports */
uint32 last_delta_FCB = -1;
uint32 last_delta_FCf = -1;
uint32 acc_delta_FCB = 0;
uint32 acc_delta_FCf = 0;
uint32 delay_report_count = 0;

/**
 * Jumbo DM tlv feature is used to support the DM length to 9600 bytes. User needs to know below things about this feature
 *     Only 1K endpoints can be supported, the MEP entry is allocated by steps 64 with ID. 
 *     Mep memory allocation is SelfContained, short maid and ICC maid support 512 entries respectively.
 *     256 MEPs can support DMM/DMR with TLV length up to 9600.
 *     TLV length was configurable, and must be 12*N (N>=0)
 *     Data pattern is 4Bytes repeatable pattern
 *     Data TLV of DMR is from local DMM configuration
 *
 *  This example demonstrate how to create 1024 mep and 256 Jumbo dm entry
 *
 * @param unit
 * @param port1 - 
 * @param port2 - 
 *
 * @return int
 */
int dnx_oam_jumbo_dm_run_range(
    int unit,
    bcm_port_t port1,
    bcm_port_t port2)
{
    bcm_error_t rv;
    bcm_gport_t gport1;
    bcm_gport_t gport2;
    bcm_oam_group_info_t group_info_short;
    bcm_oam_group_info_t group_info_long;
    bcm_mac_t my_cfm_mac = {0x00, 0x00, 0x00, 0xff, 0xff, 0xff};
    uint32 flags = 0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile = 1;
    bcm_oam_profile_t acc_egress_profile = 1;
    int lm_counter_base_id_0;
    int lm_counter_base_id_1;
    int counter_if_0 = 0;
    int counter_if_1 = 1;
    int vid = 5;
    int index = 0;
    int mepid_steps = 0;

    /*
     * Create OAM group
     */
    bcm_oam_group_info_t_init(group_info_short);
    group_info_short.flags = BCM_OAM_GROUP_WITH_ID;
    group_info_short.id = 7;
    bcm_oam_group_info_t_init(group_info_long);
    group_info_long.flags = BCM_OAM_GROUP_WITH_ID;
    group_info_long.id = 9;
    /* Short MAID */
    sal_memcpy(group_info_short.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, group_info_short);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_group_create.\n", bcm_errmsg(rv));
        return rv;
    }
    /* Long MAID */
    sal_memcpy(group_info_long.name, long_name, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, group_info_long);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_group_create.\n", bcm_errmsg(rv));
        return rv;
    }
    
    /*
     * Setting My-CFM-MAC
     */
    rv = oam__my_cfm_mac_set(unit, port2, my_cfm_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in oam__my_cfm_mac_set.\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * Allocate counter engines and get counters
     */
    rv = set_counter_resource(unit, port2, counter_if_0, 1, &lm_counter_base_id_0);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in set_counter_resource.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = set_counter_resource(unit, port2, counter_if_1, 1, &lm_counter_base_id_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in set_counter_resource.\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * Get default profile encoded values.
     * Default profile is 1 and is configured
     * on init(application) stage.
     */
    rv = bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_profile_id_get_by_type.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_profile_id_get_by_type.\n",  bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_oam_profile_id_get_by_type(unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_profile_id_get_by_type.\n",  bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_oam_profile_id_get_by_type(unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &acc_egress_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_profile_id_get_by_type.\n",  bcm_errmsg(rv));
        return rv;
    }
	
    /**1, test create 1k MEP entry*/
    for (index = 0; index < 1024; index ++ )
    {

        /*
         * Create cross-connected VLAN ports
         */
        rv = dnx_oam_cross_connected_vlan_ports_create(unit, port1, port2, vid+nof_mep, vid+nof_mep, &gport1, &gport2);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in dnx_oam_cross_connected_vlan_ports_create.\n",  bcm_errmsg(rv));
            return rv;
        }
    
        rv = bcm_oam_lif_profile_set(unit, 0, gport2, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in bcm_oam_lif_profile_set.\n",  bcm_errmsg(rv));
            return rv;
        }

        mepid_steps = 64 * index;
        nof_mep ++;
        jumbo_with_Id = 1;
        printf("+++ create %d mep +++ \n", nof_mep);
        /*
         * Create accelerated Down-MEP
         */
        rv = dnx_oam_accelerated_endpoint_create(unit, 0 + mepid_steps, gport2, /*group_info_short.id*/(index >127) ? group_info_short.id : group_info_long.id,
                                                 counter_if_1, lm_counter_base_id_1, acc_ingress_profile, port2, 0);

        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in dnx_oam_accelerated_endpoint_create.\n",  bcm_errmsg(rv));
            return rv;
        }
    }
    
    for (index = 0; index < 1024; index ++ )
    {
        rv = bcm_oam_endpoint_destroy(unit, 64 * index);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in bcm_oam_endpoint_destroy.\n",  bcm_errmsg(rv));
            return rv;
        }
    }
    printf(" free 1024 entries done.\n");


    /** 2: test create 256 DM entry*/
    for (index = 0; index < 256; index ++ )
    {
        /*
         * Create cross-connected VLAN ports
         */
        rv = dnx_oam_cross_connected_vlan_ports_create(unit, port1, port2, vid+nof_mep, vid+nof_mep, &gport1, &gport2);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in dnx_oam_cross_connected_vlan_ports_create.\n",  bcm_errmsg(rv));
            return rv;
        }
    
        rv = bcm_oam_lif_profile_set(unit, 0, gport2, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in bcm_oam_lif_profile_set.\n",  bcm_errmsg(rv));
            return rv;
        }

        mepid_steps = 64 * index;
        nof_mep ++;
        jumbo_with_Id = 1;
        printf("+++ create %d mep +++ \n", index);
        /*
         * Create accelerated Down-MEP
         */
        rv = dnx_oam_accelerated_endpoint_create(unit, 0 + mepid_steps, gport2, /*group_info_short.id*/(index >=128) ? group_info_short.id : group_info_long.id,
                                                 counter_if_1, lm_counter_base_id_1, acc_ingress_profile, port2, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in dnx_oam_accelerated_endpoint_create.\n",  bcm_errmsg(rv));
            return rv;
        }
        rv = dnx_oam_delay_add(unit, mepid_steps, 16388 + mepid_steps, BCM_OAM_ENDPOINT_CCM_PERIOD_100MS);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in dnx_oam_loss_add.\n",  bcm_errmsg(rv));
            return rv;
        }
    }
    
    for (index = 0; index < 256; index ++ )
    {
        bcm_oam_endpoint_destroy(unit, 64 * index);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in bcm_oam_endpoint_destroy.\n",  bcm_errmsg(rv));
            return rv;
        }
    }
    printf("  Again : free 256 entries.\n");
    
    return rv;
}

/**
 * Create an accelerated Up-MEP with ID 4096 and level 5
 * and an accelerated Down-MEP with ID 4100 and level 2.
 * Up-MEP will have a loss (with extended statistics)
 * and a delay entry.
 * Down-MEP 4100 will have a piggy-back loss entry.
 *
 *
 * @param unit
 * @param port1
 * @param port2
 *
 * @return int
 */
int dnx_oam_lm_dm_run_with_defaults(
    int unit,
    bcm_port_t port1,
    bcm_port_t port2)
{
    bcm_error_t rv;
    bcm_gport_t gport1;
    bcm_gport_t gport2;
    bcm_oam_group_info_t group_info;
    bcm_mac_t my_cfm_mac = {0x00, 0x00, 0x00, 0xff, 0xff, 0xff};
    uint32 flags = 0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile;
    bcm_oam_profile_t acc_egress_profile = 1;
    int lm_counter_base_id_0;
    int lm_counter_base_id_1;
    int counter_if_0 = 0;
    int counter_if_1 = 1;
    int vid = 5;

    /*
     * Create cross-connected VLAN ports
     */
    rv = dnx_oam_cross_connected_vlan_ports_create(unit, port1, port2, vid, vid, &gport1, &gport2);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in dnx_oam_cross_connected_vlan_ports_create.\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * Setting My-CFM-MAC
     */
    rv = oam__my_cfm_mac_set(unit, port2, my_cfm_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in oam__my_cfm_mac_set.\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * Allocate counter engines and get counters
     */
    rv = set_counter_resource(unit, port2, counter_if_0, 1, &lm_counter_base_id_0);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in set_counter_resource.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = set_counter_resource(unit, port2, counter_if_1, 1, &lm_counter_base_id_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in set_counter_resource.\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * Get default profile encoded values.
     * Default profile is 1 and is configured
     * on init(application) stage.
     */
    rv = bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_profile_id_get_by_type.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_profile_id_get_by_type.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = oam_set_of_action_profiles_create(unit);
    if (rv != BCM_E_NONE) {
        printf("oam_set_of_action_profiles_create fail \n");
        return rv;
    }

    acc_ingress_profile = oam_acc_lif_profiles.oam_acc_profile_eth_dual_ended;
    if(is_jumbo_dm)
    {
        acc_ingress_profile=1;
    }
    rv = bcm_oam_profile_id_get_by_type(unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_profile_id_get_by_type.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_oam_profile_id_get_by_type(unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &acc_egress_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_profile_id_get_by_type.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_oam_lif_profile_set(unit, 0, gport2, ingress_profile, egress_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_lif_profile_set.\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * Create OAM group
     */
    bcm_oam_group_info_t_init(group_info);
    group_info.flags = BCM_OAM_GROUP_WITH_ID;
    group_info.id = 7;
    
    if (use_48_maid){
        sal_memcpy(group_info.name, str_48b_name, BCM_OAM_GROUP_NAME_LENGTH);
        group_info.flags = BCM_OAM_GROUP_WITH_ID | BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE;
        group_info.group_name_index = 0xA014; /* Should be multiple of 4 */
    }
    else if (is_short_maid)
    {
        /* Short MAID */
        sal_memcpy(group_info.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    }
    else
    {   /* Long MAID */
        sal_memcpy(group_info.name, long_name, BCM_OAM_GROUP_NAME_LENGTH);
    }
    rv = bcm_oam_group_create(unit, group_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_group_create.\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * Create accelerated Up-MEP
     */
    rv = dnx_oam_accelerated_endpoint_create(unit, oam_perf_up_mep_id, gport2, group_info.id,
                                             counter_if_0, lm_counter_base_id_0, acc_egress_profile, 0, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in dnx_oam_accelerated_endpoint_create (Up MEP).\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * Create accelerated Down-MEP
     */
    rv = dnx_oam_accelerated_endpoint_create(unit, oam_perf_down_mep_id, gport2, group_info.id,
                                             counter_if_1, lm_counter_base_id_1, acc_ingress_profile, port2, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in dnx_oam_accelerated_endpoint_create (Down MEP).\n", bcm_errmsg(rv));
        return rv;
    }
    if(is_jumbo_dm)
    {
        /*
         * Add delay on Up-MEP
         */
        rv = dnx_oam_delay_add(unit, oam_perf_up_mep_id, 65552, BCM_OAM_ENDPOINT_CCM_PERIOD_100MS);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in dnx_oam_delay_add.\n", bcm_errmsg(rv));
            return rv;
        }
        /*
         * Add delay on Down-MEP
         */
        rv = dnx_oam_delay_add(unit, oam_perf_down_mep_id, 65556, BCM_OAM_ENDPOINT_CCM_PERIOD_100MS);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in dnx_oam_loss_add.\n", bcm_errmsg(rv));
            return rv;
        }
    }
    else 
    {
        /*
         * Add loss on Up-MEP
         */
        rv = dnx_oam_loss_add(unit, oam_perf_up_mep_id, 65552, BCM_OAM_ENDPOINT_CCM_PERIOD_10MS, 0, 1);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in dnx_oam_loss_add.\n", bcm_errmsg(rv));
            return rv;
        }

        /*
         * Add delay on Up-MEP
         */
        rv = dnx_oam_delay_add(unit, oam_perf_up_mep_id, 0, BCM_OAM_ENDPOINT_CCM_PERIOD_10MS);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in dnx_oam_loss_add.\n", bcm_errmsg(rv));
            return rv;
        }

        /*
         * Add CCM piggy-back loss on Down-MEP
         */
        rv = dnx_oam_loss_add(unit, oam_perf_down_mep_id, 65556, BCM_OAM_ENDPOINT_CCM_PERIOD_10MS, 1, 1);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %s, in dnx_oam_loss_add.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

/**
 * Create accelerated OAM endpoint
 *
 *
 * @param unit
 * @param mep_id - OAM endpoint ID
 * @param gport - VLAN port ID
 * @param group_id - OAM group ID
 * @param lm_counter_if - LM counter interface
 * @param lm_counter_base_id - LM counter pointer
 * @param acc_profile - accelerated profile ID
 * @param tx_port - relevant for Down-MEP only
 * @param is_up - Up-MEP indication
 *
 * @return int
 */
int dnx_oam_accelerated_endpoint_create(
    int unit,
    bcm_oam_endpoint_t mep_id,
    bcm_gport_t gport,
    bcm_oam_group_t group_id,
    int lm_counter_if,
    int lm_counter_base_id,
    int acc_profile,
    bcm_port_t tx_port,
    uint8 is_up)
{
    bcm_error_t rv;
    bcm_oam_endpoint_info_t acc_endpoint;
    bcm_oam_endpoint_info_t remote_endpoint;

    /*
     * Adding accelerated endpoint
     */
    bcm_oam_endpoint_info_t_init(&acc_endpoint);
    acc_endpoint.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;
    acc_endpoint.type = bcmOAMEndpointTypeEthernet;
    acc_endpoint.flags = jumbo_with_Id ? BCM_OAM_ENDPOINT_WITH_ID : 0;
    if (is_up)
    {
        acc_endpoint.flags |= BCM_OAM_ENDPOINT_UP_FACING;
    }
    acc_endpoint.id = mep_id;
    acc_endpoint.gport = gport;
    acc_endpoint.group = group_id;
    acc_endpoint.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    acc_endpoint.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    acc_endpoint.acc_profile_id = acc_profile;
    acc_endpoint.vlan = 5;
    acc_endpoint.pkt_pri = 0 + (2<<1);  /* dei(1bit) + (pcp(3bit) << 1) */
    acc_endpoint.outer_tpid = 0x8100;
    acc_endpoint.lm_counter_base_id = lm_counter_base_id;
    acc_endpoint.lm_counter_if = lm_counter_if;
    if (is_up)
    {
        acc_endpoint.level = 5;
        acc_endpoint.name = 124;
        sal_memcpy(acc_endpoint.src_mac_address, src_mac_up, 6);
    }
    else
    {
        acc_endpoint.level = 2;
        acc_endpoint.name = 127;
        BCM_GPORT_SYSTEM_PORT_ID_SET(acc_endpoint.tx_gport, tx_port);
        sal_memcpy(acc_endpoint.src_mac_address, src_mac_down, 6);
    }
    rv = bcm_oam_endpoint_create(unit, acc_endpoint);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_endpoint_create.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Created Local MEP: ID %d\n", acc_endpoint.id);

    if (is_up)
    {
        oam_perf_up_mep_id = acc_endpoint.id;
    }
    else
    {
        oam_perf_down_mep_id = acc_endpoint.id;
    }

    /*
     * Adding Remote MEP
     */
    bcm_oam_endpoint_info_t_init(remote_endpoint);
    remote_endpoint.flags = BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    remote_endpoint.id = acc_endpoint.id;
    remote_endpoint.local_id = acc_endpoint.id;
    remote_endpoint.name = 0xff;
    remote_endpoint.ccm_period = 0;
    remote_endpoint.loc_clear_threshold = 1;

    rv = bcm_oam_endpoint_create(unit, remote_endpoint);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_endpoint_create.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Created RMEP: ID %d\n", remote_endpoint.id);

    return rv;
}

/**
 * Add loss measurement entries
 *
 *
 * @param unit
 * @param mep_id - OAM endpoint ID
 * @param loss_id - LM entry ID
 * @param period - loss period
 * @param is_dual_ended_lm - CCM piggy-back indication
 *
 * @return int
 */
int dnx_oam_loss_add(
    int unit,
    bcm_oam_endpoint_t mep_id,
    int loss_id,
    int period,
    uint8 is_dual_ended_lm,
    uint8 use_extended_statistics)
{
    bcm_error_t rv;
    bcm_oam_loss_t loss;

    /*
     * Create LM entries
     */
    bcm_oam_loss_t_init(&loss);
    if (loss_id)
    {
        loss.flags = BCM_OAM_LOSS_WITH_ID;
    }

    if (use_extended_statistics)
    {
        loss.flags |= BCM_OAM_LOSS_STATISTICS_EXTENDED;
    }

    if (!is_dual_ended_lm)
    {
        loss.flags |= BCM_OAM_LOSS_SINGLE_ENDED;
        loss.peer_da_mac_address[1] = 0xab;
        loss.peer_da_mac_address[3] = 0xef;
    }

    loss.id = mep_id;
    loss.loss_id = loss_id;
    loss.period = period;
    rv = bcm_oam_loss_add(unit, &loss);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_loss_add.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/**
 * Add delay measurement entries
 *
 *
 * @param unit
 * @param mep_id - OAM endpoint ID
 * @param delay_id - DM entry ID
 * @param period - delay period
 *
 * @return int
 */
int dnx_oam_delay_add(
    int unit,
    bcm_oam_endpoint_t mep_id,
    int delay_id,
    int period)
{
    bcm_error_t rv;
    bcm_oam_delay_t delay;

    /*
     * Create DM entries
     */
    bcm_oam_delay_t_init(&delay);
    if (delay_id)
    {
        delay.flags = BCM_OAM_LOSS_WITH_ID;
    }
    delay.id = mep_id;
    delay.peer_da_mac_address[1] = 0xab;
    delay.peer_da_mac_address[3] = 0xef;
    delay.delay_id = delay_id;
    delay.period = period;
    delay.timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
    if(is_jumbo_dm)
    {
        delay.tlvs[0].tlv_type = bcmOamTlvTypeData;
        delay.tlvs[0].tlv_length= 9600;
        delay.tlvs[0].four_byte_repeatable_pattern= 0xabcdeeff;
    }
    rv = bcm_oam_delay_add(unit, &delay);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_delay_add.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* This is an example of using bcm_oam_performance_event_register api.
 * A simple callback is created for lm/dm report events.
 * Triggered upon LM/DM report rx.
 */
int cb_stat(int unit,
            bcm_oam_event_type_t event_type,
            bcm_oam_group_t group,
            bcm_oam_endpoint_t endpoint,
            bcm_oam_performance_event_data_t *event_data,
            void *user_data) {

   int et = event_type, gp = group, ep = endpoint;
   printf("UNIT: %d, EVENT: %d, GROUP: %d, MEP: 0x%0x\n Event Data: \n",unit,et,gp,ep);
   print *event_data;

    /* Update globals */
    switch (event_type) {
    case bcmOAMEventEndpointDmStatistics:
        delay_report_count++;
        break;
    case bcmOAMEventEndpointLmStatistics:
        last_delta_FCB = event_data->delta_FCB;
        acc_delta_FCB+= last_delta_FCB;
        last_delta_FCf = event_data->delta_FCf;
        acc_delta_FCf+= last_delta_FCf;
        break;
    }
}

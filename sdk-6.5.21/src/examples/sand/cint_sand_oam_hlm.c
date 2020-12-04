/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_sand_oam_hlm.c Purpose: Example of using OAM Hierarchical loss measurement.
 */
/*
 *
 * Usage:
 *
 *  JER1:
 *   cd ../../../../src/examples/dpp/
 *   cint ../sand/utility/cint_sand_utils_global.c
 *   cint utility/cint_utils_multicast.c
 *   cint utility/cint_utils_mpls_port.c
 *   cint utility/cint_utils_oam.c
 *   cint cint_port_tpid.c
 *   cint cint_advanced_vlan_translation_mode.c
 *   cint cint_l2_mact.c
 *   cint cint_vswitch_metro_mp.c
 *   cint utility/cint_multi_device_utils.c
 *   cint cint_queue_tests.c
 *   cint cint_oam.c
 *   cint ../sand/cint_sand_oam_hlm.c
 *   cint
 *   int unit = 0;
 *   int port_1 = 13, port_2 = 14;
 *   print dnx_oam_hierarchical_lm_example(unit, port_1, port_2);
 *
 *  JR2 and above:
 *   cd ../../../../src/examples/sand/
 *   cint utility/cint_sand_utils_oam.c
 *   cint cint_sand_oam.c
 *   cint cint_sand_oam_hlm.c
 *   cint ../dnx/cint_oam_basic.c
 *   cint ../dnx/crps/cint_crps_oam_config.c
 *   cint
 *   int unit = 0;
 *   int port_1 = 13, port_2 = 14;
 *   print dnx_oam_hierarchical_lm_example(unit, port_1, port_2);
 *
 * This CINT creates two vlan ports and cross-connects them.
 * 2 Down-MEPs of level 1 and 3 will be configured on P1.
 * 2 Up-MEPs of level 4 and 6 will be configured on P2.
 *
 *    Down-MEPs                           Up-MEPs
 *       (3)    +----+            +----+    (6)
 *       (1)    | P1 |------------| P2 |    (4)
 *              +----+            +----+
 *
 * Two counter engines are configured. MEPs on the same LIF will have
 * counter pointers from a differenet counter engine.
 */

int hierarchical_lm_is_acc = 0;
int is_pcp = 0;
oam__ep_info_s meps_info[4];
int lm_counter_base_ids[4];
/** Jr2 ONLY */
int lm_counter_if[3] = {0, 1, 2};

/**
 * Cint that demonstrates hierarchical loss measurement by MD-Level on
 * Down-MEPs and Up-MEPs.
 *
 * @param unit
 * @param port1
 * @param port2
 *
 * @return int
 */
int oam_hierarchical_lm_example(
    int unit,
    int port1,
    int port2)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_vlan_port_t vp1, vp2;
    bcm_vswitch_cross_connect_t cross_connect;
    bcm_oam_endpoint_info_t mep_1, mep_3, mep_4, mep_6;
    bcm_oam_profile_t ingress_profile = 1, egress_profile = 1;
    bcm_oam_profile_t ingress_acc_profile = 1, egress_acc_profile = 1;
    bcm_oam_group_info_t group_info_short_ma;
    uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = { 1, 3, 2, 0xab, 0xcd };
    int vid1 = 0x10, vid2 = 0x20;
    int down_mdl_1 = 3, down_mdl_2 = 1;
    int up_mdl_1 = 6, up_mdl_2 = 4;
    uint32 flags = 0;
    uint64 arg;

    /**
     * Set port classification ID
     */
    rv = bcm_port_class_set(unit, port1, bcmPortClassId, port1);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_port_class_set.\n", rv);
        return rv;
    }

    /**
     * Set port classification ID
     */
    rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_port_class_set.\n", rv);
        return rv;
    }

    /**
     * This is meant for QAX and above only
     */
    rv = oam__device_type_get(unit, &device_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in oam__device_type_get.\n", rv);
        return rv;
    }

    if (device_type < device_type_qax) {
        return BCM_E_UNAVAIL;
    }

    /**
     * Allocate counter engines and get counters
     */
    if (device_type < device_type_jericho2)
    {
        rv = hierarchical_lm_counters_setup(unit, 2, 2, counter_engines, counter_base_ids, BCM_STAT_COUNTER_MULTIPLE_SOURCES_PER_ENGINE);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in hierarchical_lm_counters_setup.\n", rv);
            return rv;
        }

        printf("Allocated counters:\n");
        print counter_base_ids;
    }
    else
    {
        rv = set_counter_resource(unit, port1, lm_counter_if[0], 1, &lm_counter_base_ids[0]);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
        rv = set_counter_resource(unit, port1, lm_counter_if[1], 1, &lm_counter_base_ids[2]);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
        rv = set_counter_resource(unit, port2, lm_counter_if[0], 1, &lm_counter_base_ids[1]);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
        rv = set_counter_resource(unit, port2, lm_counter_if[1], 1, &lm_counter_base_ids[3]);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
    }

    /**
     * Create VLAN ports
     */
    bcm_vlan_port_t_init(&vp1);
    if (device_type < device_type_jericho2)
    {
        vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    }
    else
    {
        vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    }
    vp1.port = port1;
    vp1.match_vlan = vid1;
    vp1.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : vid1;
    rv = bcm_vlan_port_create(unit, &vp1);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_vlan_port_create.\n", rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vid1, port1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_vlan_gport_add.\n", rv);
        return rv;
    }

    bcm_vlan_port_t_init(&vp2);
    if (device_type < device_type_jericho2)
    {
        vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    }
    else
    {
        vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    }
    vp2.port = port2;
    vp2.match_vlan = vid2;
    vp2.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : vid2;
    rv = bcm_vlan_port_create(unit, &vp2);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_vlan_port_create.\n", rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vid2, port2, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_vlan_gport_add.\n", rv);
        return rv;
    }

    /**
     * Cross-connect the two VLAN ports
     */
    bcm_vswitch_cross_connect_t_init(&cross_connect);
    cross_connect.port1 = vp1.vlan_port_id;
    gport1 = vp1.vlan_port_id;
    cross_connect.port2 = vp2.vlan_port_id;
    gport2 = vp2.vlan_port_id;

    rv = bcm_vswitch_cross_connect_add(unit, &cross_connect);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_vswitch_cross_connect_add.\n", rv);
        return rv;
    }

    /**
     * Get default profile encoded values.
     * Default profile is 1 and it is
     * configured on init(application) stage.
     * Relevant for Jericho2 ONLY
     */
    if (device_type >= device_type_jericho2)
    {
        /** Get non-accelerated profiles */
        rv = bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
        if (rv != BCM_E_NONE) {
            printf("bcm_oam_profile_id_get_by_type(ingress) \n");
            return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
        if (rv != BCM_E_NONE) {
            printf("bcm_oam_profile_id_get_by_type(egress)\n");
            return rv;
        }

        /** Get accelerated profiles */
        rv = bcm_oam_profile_id_get_by_type(unit, ingress_acc_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &ingress_acc_profile);
        if (rv != BCM_E_NONE) {
            printf("bcm_oam_profile_id_get_by_type(ingress) \n");
            return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, egress_acc_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &egress_acc_profile);
        if (rv != BCM_E_NONE) {
            printf("bcm_oam_profile_id_get_by_type(egress)\n");
            return rv;
        }

        rv = bcm_oam_lif_profile_set(unit, 0, vp1.vlan_port_id, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
            printf("bcm_oam_lif_profile_set\n");
            return rv;
        }
        rv = bcm_oam_lif_profile_set(unit, 0, vp2.vlan_port_id, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
            printf("bcm_oam_lif_profile_set\n");
            return rv;
        }
    }

    /**
     * Create OAM group
     */
    bcm_oam_group_info_t_init(&group_info_short_ma);
    sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, &group_info_short_ma);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_group_create.\n", rv);
        return rv;
    }

    /**
     * Configure the counters to use PCP
     */
    if (is_pcp)
    {
        if (device_type < device_type_jericho2)
        {
            rv = oam_qos_map_create(unit, gport2, BCM_GPORT_INVALID, BCM_GPORT_INVALID);
            if (rv != BCM_E_NONE)
            {
                printf("Error no %d, in oam_qos_map_create.\n", rv);
                return rv;
            }
        }
        else
        {
            /**
             * Set Counter Range for PCP based counters.
             * As default all counter range defined as non-PCP.
             * To use PCP based counters, limited counter range
             * should be set.
             * To set all counters for PCP,
             * range_min = range_max = 0 should be used.
             */

           /**
            * Configure range to: Min=0, Max=0.
            * That allows the counters to be set
            */
           COMPILER_64_ZERO(arg);
           rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMin, arg);
           if (rv != BCM_E_NONE)
           {
               printf("Error no %d, in bcm_oam_control_set.\n", rv);
               return rv;
           }

           rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMax, arg);
           if (rv != BCM_E_NONE)
           {
               printf("Error no %d, in bcm_oam_control_set.\n", rv);
               return rv;
           }
        }
    }

    if (device_type >= device_type_jericho2)
    {
        /**
         * Set My-CFM-MAC (for Jericho2 Only)
         */
        rv = oam__my_cfm_mac_set(unit, port1, mac_mep_1);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }

        rv = oam__my_cfm_mac_set(unit, port2, mac_mep_2);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }
    }

    if (!hierarchical_lm_is_acc)
    {
        /**
         * Add non-accelerated Down-MEP
         */
        bcm_oam_endpoint_info_t_init(&mep_1);
        mep_1.type = bcmOAMEndpointTypeEthernet;
        mep_1.group = group_info_short_ma.id;
        mep_1.level = down_mdl_1;
        mep_1.lm_flags = BCM_OAM_LM_PCP * is_pcp;
        mep_1.gport = gport1;
        if (device_type < device_type_jericho2)
        {
            sal_memcpy(mep_1.dst_mac_address, mac_mep_1, 6);
            mep_1.timestamp_format = get_oam_timestamp_format(unit);
        }

        if (device_type >= device_type_jericho2)
        {
            mep_1.lm_counter_if = lm_counter_if[0];
            mep_1.lm_counter_base_id = lm_counter_base_ids[0];
        }
        else
        {
            mep_1.lm_counter_base_id = counter_base_ids[0];
        }
    } else {
        /**
         * Add accelerated Down-MEP
         */
        /* TX */
        bcm_oam_endpoint_info_t_init(&mep_1);
        mep_1.type = bcmOAMEndpointTypeEthernet;
        mep_1.group =  group_info_short_ma.id;
        mep_1.level = down_mdl_1;
        BCM_GPORT_SYSTEM_PORT_ID_SET(mep_1.tx_gport, port1);
        mep_1.name = 123;
        mep_1.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
        mep_1.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
        if (device_type >= device_type_jericho2)
        {
            mep_1.acc_profile_id = ingress_acc_profile;
            mep_1.id = 0x8000;
            mep_1.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        }
        mep_1.vlan = 0x10;
        if (device_type < device_type_jericho2)
        {
            mep_1.vpn = 0x20; /* Add VSI in order to support HLM Down-MEP injection counting */
        }
        mep_1.pkt_pri = mep_acc_info.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1) */
        mep_1.outer_tpid = 0x8100;
        mep_1.inner_vlan = 0;
        mep_1.inner_pkt_pri = 0;
        mep_1.inner_tpid = 0;
        if (device_type < device_type_jericho2)
        {
            mep_1.int_pri = 3 + (1<<2);
        }
        else
        {
            mep_1.int_pri = 2 + (1<<2);
        }
        mep_1.sampling_ratio = sampling_ratio;
        mep_1.lm_flags = BCM_OAM_LM_PCP * is_pcp; /* In case of PCP set the flag */
        mep_1.flags2 =  BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE; /* Take RDI only from scanner */
        if (device_type < device_type_jericho2)
        {
            mep_1.timestamp_format = get_oam_timestamp_format(unit);
        }
        /* The MAC address that the CCM packets are sent with */
        sal_memcpy(mep_1.src_mac_address, src_mac_mep_3, 6);

        /* RX */
        mep_1.gport = vp1.vlan_port_id;
        if (device_type < device_type_jericho2)
        {
            sal_memcpy(mep_1.dst_mac_address, mac_mep_1, 6);
        }

        if (device_type >= device_type_jericho2)
        {
            mep_1.lm_counter_if = lm_counter_if[0];
            mep_1.lm_counter_base_id = lm_counter_base_ids[0];
        }
        else
        {
            mep_1.lm_counter_base_id = counter_base_ids[0];
        }
    }

    printf("bcm_oam_endpoint_create mep_1\n");
    rv = bcm_oam_endpoint_create(unit, &mep_1);
    if (rv != BCM_E_NONE)
    {
       printf("Error no %d, in bcm_oam_endpoint_create.\n", rv);
       return rv;
    }
    printf("Created Down-MEP with id %d\n", mep_1.id);

    ep1.gport = mep_1.gport;
    ep1.id = mep_1.id;

    /**
     * Add another non-accelerated Down-MEP
     */
    bcm_oam_endpoint_info_t_init(&mep_3);
    mep_3.type = bcmOAMEndpointTypeEthernet;
    mep_3.group = group_info_short_ma.id;
    mep_3.level = down_mdl_2;
    mep_3.lm_flags = BCM_OAM_LM_PCP * is_pcp; /* In case of PCP set the flag */
    mep_3.gport = gport1;
    if (device_type < device_type_jericho2)
    {
        sal_memcpy(mep_3.dst_mac_address, mac_mep_1, 6);
        mep_3.timestamp_format = get_oam_timestamp_format(unit);
    }

    if (device_type >= device_type_jericho2)
    {
        mep_3.lm_counter_if = lm_counter_if[1];
        mep_3.lm_counter_base_id = lm_counter_base_ids[2];
    }
    else
    {
        mep_3.lm_counter_base_id = counter_base_ids[2];
    }

    printf("bcm_oam_endpoint_create mep_3\n");
    rv = bcm_oam_endpoint_create(unit, &mep_3);
    if (rv != BCM_E_NONE)
    {
       printf("Error no %d, in bcm_oam_endpoint_create.\n", rv);
       return rv;
    }
    printf("Created non-accelerated Down-MEP with id %d\n", mep_3.id);

    ep2.gport = mep_3.gport;
    ep2.id = mep_3.id;

    if (!hierarchical_lm_is_acc)
    {
        /**
         * Add non-accelerated Up-MEP
         */
        bcm_oam_endpoint_info_t_init(&mep_4);
        mep_4.type = bcmOAMEndpointTypeEthernet;
        mep_4.group = group_info_short_ma.id;
        mep_4.level = up_mdl_1;
        mep_4.tx_gport = BCM_GPORT_INVALID;
        mep_4.flags = BCM_OAM_ENDPOINT_UP_FACING;
        mep_4.gport = gport2;
        mep_4.lm_flags = BCM_OAM_LM_PCP * is_pcp; /* In case of PCP set the flag */
        if (device_type < device_type_jericho2)
        {
            sal_memcpy(mep_4.dst_mac_address, mac_mep_2, 6);
            mep_4.timestamp_format = get_oam_timestamp_format(unit);
        }

        if (device_type >= device_type_jericho2)
        {
            mep_4.lm_counter_if = lm_counter_if[0];
            mep_4.lm_counter_base_id = lm_counter_base_ids[1];
        }
        else
        {
            mep_4.lm_counter_base_id = counter_base_ids[1];
        }
    }
    else
    {
        /**
         * Add accelerated Up-MEP
         */
        /* TX */
        bcm_oam_endpoint_info_t_init(&mep_4);
        mep_4.type = bcmOAMEndpointTypeEthernet;
        mep_4.group =  group_info_short_ma.id;
        mep_4.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
        mep_4.level = up_mdl_1;
        mep_4.tx_gport = BCM_GPORT_INVALID; /* Up-MEP requires invalid gport */
        mep_4.name = 123;
        mep_4.flags |= BCM_OAM_ENDPOINT_UP_FACING;
        mep_4.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
        if (device_type >= device_type_jericho2)
        {
            mep_4.acc_profile_id = egress_acc_profile;
            mep_4.id = 0x8004;
            mep_4.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        }
        mep_4.vlan = 0x20;
        mep_4.pkt_pri = mep_acc_info.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1) */
        mep_4.outer_tpid = 0x8100;
        mep_4.inner_vlan = 0;
        mep_4.inner_pkt_pri = 0;
        mep_4.inner_tpid = 0;
        if (device_type < device_type_jericho2)
        {
            mep_4.int_pri = 3 + (1<<2);
        }
        else
        {
            mep_4.int_pri = 0;
        }
        mep_4.sampling_ratio = sampling_ratio;
        mep_4.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE; /* Take RDI only from scanner */
        if (device_type < device_type_jericho2)
        {
            mep_4.timestamp_format = get_oam_timestamp_format(unit);
        }
        /* The MAC address that the CCM packets are sent with */
        sal_memcpy(mep_4.src_mac_address, src_mac_mep_2, 6);

        /* RX */
        mep_4.gport = vp2.vlan_port_id;
        mep_4.lm_flags = BCM_OAM_LM_PCP * is_pcp; /* In case of PCP set the flag */
        if (device_type < device_type_jericho2)
        {
            sal_memcpy(mep_4.dst_mac_address, mac_mep_2, 6);
        }

        if (device_type >= device_type_jericho2)
        {
            mep_4.lm_counter_if = lm_counter_if[0];
            mep_4.lm_counter_base_id = lm_counter_base_ids[1];
        }
        else
        {
            mep_4.lm_counter_base_id = counter_base_ids[1];
        }
    }

    printf("bcm_oam_endpoint_create mep_4\n");
    rv = bcm_oam_endpoint_create(unit, &mep_4);
    if (rv != BCM_E_NONE)
    {
       printf("Error no %d, in bcm_oam_endpoint_create.\n", rv);
       return rv;
    }
    printf("Created Up-MEP with id %d\n", mep_4.id);

    ep3.gport = mep_4.gport;
    ep3.id = mep_4.id;

    /**
     * Add another non-accelerated Up-MEP
     */
    bcm_oam_endpoint_info_t_init(&mep_6);
    mep_6.type = bcmOAMEndpointTypeEthernet;
    mep_6.group = group_info_short_ma.id;
    mep_6.level = up_mdl_2;
    mep_6.tx_gport = BCM_GPORT_INVALID;
    mep_6.flags = BCM_OAM_ENDPOINT_UP_FACING;
    mep_6.gport = gport2;
    mep_6.lm_flags = BCM_OAM_LM_PCP * is_pcp; /* In case of PCP set the flag */
    if (device_type < device_type_jericho2)
    {
        sal_memcpy(mep_6.dst_mac_address, mac_mep_2, 6);
        mep_6.timestamp_format = get_oam_timestamp_format(unit);
    }

    if (device_type >= device_type_jericho2)
    {
        mep_6.lm_counter_if = lm_counter_if[1];
        mep_6.lm_counter_base_id = lm_counter_base_ids[3];
    }
    else
    {
        mep_6.lm_counter_base_id = counter_base_ids[3];
    }

    printf("bcm_oam_endpoint_create mep_6\n");
    rv = bcm_oam_endpoint_create(unit, &mep_6);
    if (rv != BCM_E_NONE)
    {
       printf("Error no %d, in bcm_oam_endpoint_create.\n", rv);
       return rv;
    }
    printf("Created MEP with id %d\n", mep_6.id);

    ep4.gport = mep_6.gport;
    ep4.id = mep_6.id;

    return rv;
}

/**
 * Cint that demonstrates hierarchical loss measurement by LIF
 * on Down-MEPs
 *
 * @param unit
 * @param port1
 * @param port2
 *
 * @return int
 */
int oam_hierarchical_lif_lm_example(
    int unit,
    int in_sysport,
    int out_sysport)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t gports[3];
    int term_labels[3] = {0x100, 0x200, 0x300};
    bcm_oam_endpoint_info_t def_mep;
    bcm_oam_group_info_t group_info_short_ma;
    bcm_oam_profile_t ingress_profile_id;
    bcm_oam_profile_t egress_profile_id;
    int raw_lif_action_profile_id;
    uint32 flags = 0;
    uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};
    int mdl = 4;
    int counters_to_use[3] = {0, 2, 1};
    int lm_counter_if[3] = {0, 1, 2};
    int i;
    uint64 arg;

    rv = oam__device_type_get(unit, &device_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in oam__device_type_get.\n", rv);
        return rv;
    }

    if (device_type < device_type_jericho2)
    {
        /* Creates MPLS switch with:
         * My-MAC: 0x11
         * Next-hop mac: 0x22
         * in-label: 5000
         * out-label: 8000
         * in-vlan: 100
         * out-vlan: 200
         */
        mpls_lsr_init(in_sysport, out_sysport, 0x11, 0x22, 5000, 8000, 100,200,0);
        rv = mpls_lsr_config(&unit, 1/*nof-units*/, 0/*extend example*/);
        if (rv != BCM_E_NONE)
        {
           printf("Error no %d, in mpls_lsr_config.\n", rv);
           return rv;
        }
    }
    else
    {
        rv = mpls_termination_basic_main(unit, in_sysport, out_sysport);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in mpls_termination_basic_main.\n", rv);
            return rv;
        }

        rv = mpls_termination_basic_tunnel_id_stack_get(unit, 3, gports);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in mpls_termination_basic_tunnel_id_stack_get.\n", rv);
            return rv;
        }
    }

    /**
     * Allocate counter engines and get counters
     */
    if (device_type < device_type_jericho2)
    {
        rv = hierarchical_lm_counters_setup(unit, 2, 2, counter_engines, counter_base_ids, BCM_STAT_COUNTER_MULTIPLE_SOURCES_PER_ENGINE);
        if (rv != BCM_E_NONE)
        {
           printf("Error no %d, in hierarchical_lm_counters_setup.\n", rv);
           return rv;
        }

        printf("Allocated counters:\n");
        print counter_base_ids;
        printf("Allocated engines:\n");
        print counter_engines;
    }
    else
    {
        rv = set_counter_resource(unit, in_sysport, lm_counter_if[0], 1, &lm_counter_base_ids[0]);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
        rv = set_counter_resource(unit, in_sysport, lm_counter_if[1], 1, &lm_counter_base_ids[1]);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
        rv = set_counter_resource(unit, in_sysport, lm_counter_if[2], 1, &lm_counter_base_ids[2]);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
    }

    /**
     * Create OAM group
     */
    bcm_oam_group_info_t_init(&group_info_short_ma);
    sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, &group_info_short_ma);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_group_create.\n", rv);
        return rv;
    }

    if (device_type >= device_type_jericho2)
    {
        /**
         * Create OAM profiles
         */
        rv = oam_set_of_action_profiles_create(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in oam_set_of_action_profiles_create.\n", rv);
            return rv;
        }

        raw_lif_action_profile_id = oam_lif_profiles.oam_profile_mpls_single_ended;
        rv = bcm_oam_profile_id_get_by_type(unit, raw_lif_action_profile_id, bcmOAMProfileIngressLIF, &flags, &ingress_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_oam_profile_id_get_by_type.\n", rv);
            return rv;
        }

        egress_profile_id = BCM_OAM_PROFILE_INVALID;

        flags = 0;
        rv = bcm_oam_lif_profile_set(unit, flags, gports[0], ingress_profile_id, egress_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
            return rv;
        }
        rv = bcm_oam_lif_profile_set(unit, flags, gports[1], ingress_profile_id, egress_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
            return rv;
        }
        rv = bcm_oam_lif_profile_set(unit, flags, gports[2], ingress_profile_id, egress_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
            return rv;
        }
    }

    /**
     * Configure the counters to use PCP
     */
    if (is_pcp)
    {
        if (device_type < device_type_jericho2)
        {
            rv = oam_qos_map_create(unit, BCM_GPORT_INVALID, BCM_GPORT_INVALID, BCM_GPORT_INVALID);
            if (rv != BCM_E_NONE)
            {
                printf("Error no %d, in oam_qos_map_create.\n", rv);
                return rv;
            }
        }
        else
        {
            /**
             * Set Counter Range for PCP based counters.
             * As default all counter range defined as non-PCP.
             * To use PCP based counters, limited counter range
             * should be set.
             * To set all counters for PCP,
             * range_min = range_max = 0 should be used.
             */

           /**
            * Configure range to: Min=0, Max=0.
            * That allows the counters to be set
            */
           COMPILER_64_ZERO(arg);
           rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMin, arg);
           if (rv != BCM_E_NONE)
           {
               printf("Error no %d, in bcm_oam_control_set.\n", rv);
               return rv;
           }

           rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMax, arg);
           if (rv != BCM_E_NONE)
           {
               printf("Error no %d, in bcm_oam_control_set.\n", rv);
               return rv;
           }
        }
    }

    /**
     * Add In-LIFs and MEPs
     */
    for (i = 0; i < 3; i++) {
        if (device_type < device_type_jericho2)
        {
            /** InLIF */
            rv = mpls_add_term_entry(unit, term_labels[i], 0);
            if (rv != BCM_E_NONE)
            {
               printf("Error no %d, in mpls_add_term_entry.\n", rv);
               return rv;
            }

            gports[i] = ingress_tunnel_id_indexed[i];
            printf("Added termination of label 0x%05x -- GPort: 0x%x\n", term_labels[i], gports[i]);
        }

        /* MEP */
        bcm_oam_endpoint_info_t_init(&meps[i]);
        meps[i].type = bcmOAMEndpointTypeBHHMPLS;
        meps[i].group = group_info_short_ma.id;
        meps[i].level = mdl;
        meps[i].gport = gports[i];
        if (device_type < device_type_jericho2)
        {
            meps[i].timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
            meps[i].lm_counter_base_id = counter_base_ids[counters_to_use[i]];
        }
        else
        {
            meps[i].lm_counter_base_id = lm_counter_base_ids[i];
            meps[i].lm_counter_if = lm_counter_if[i];
        }
        meps[i].lm_flags = BCM_OAM_LM_PCP * is_pcp;
        printf("bcm_oam_endpoint_create meps[%d]\n", i);
        rv = bcm_oam_endpoint_create(unit, &meps[i]);
        if (rv != BCM_E_NONE)
        {
           printf("Error no %d, in bcm_oam_endpoint_create.\n", rv);
           return rv;
        }

        printf("Created MEP with id 0x%04x\n\tcounter: 0x%04x\n", meps[i].id, meps[i].lm_counter_base_id);

        meps_info[i].gport = gports[i];
        meps_info[i].id = meps[i].id;
        meps_info[i].lm_counter_base_id = meps[i].lm_counter_base_id;
    }

    if (device_type < device_type_jericho2)
    {
        /**
         * Add default endpoint on 3rd LIF.
         * Used for trapping OAM on more than 2 LIFs hierarchy
         */
        bcm_oam_endpoint_info_t_init(&def_mep);
        def_mep.level = mdl;
        def_mep.gport = gports[2];
        def_mep.lm_counter_base_id = 0;
        def_mep.timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
        def_mep.id = BCM_OAM_ENDPOINT_DEFAULT_INGRESS0;
        def_mep.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        def_mep.lm_flags = BCM_OAM_LM_PCP * is_pcp;
        printf("bcm_oam_endpoint_create default_mep\n", i);
        rv = bcm_oam_endpoint_create(unit, &def_mep);
        if (rv != BCM_E_NONE)
        {
           printf("Error no %d, in bcm_oam_endpoint_create.\n", rv);
           return rv;
        }

        printf("Created MEP with id 0x%04x\n\tcounter: 0x%04x\n", def_mep.id, def_mep.lm_counter_base_id);

        meps_info[3].id = def_mep.id;

        /**
         * Add dummy terminated label.
         * Used to align labels lookup in ISEM tables
         */
        rv = mpls_add_term_entry(unit, 0xfff, 0);
        if (rv != BCM_E_NONE)
        {
           printf("Error no %d, in mpls_add_term_entry.\n", rv);
           return rv;
        }
    }

    return rv;
}

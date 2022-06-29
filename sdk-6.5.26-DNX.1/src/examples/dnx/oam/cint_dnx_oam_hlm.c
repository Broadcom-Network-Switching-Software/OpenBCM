/*
 * $ File: cint_dnx_oam_hlm.c Purpose: Example of using OAM Hierarchical loss measurement.
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
 *
 * Test Scenario - start
 *
 * ./bcm.user
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_oam.c
 * cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
 * cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../../../src/examples/dnx/oam/cint_dnx_oam.c
 * cint ../../../../src/examples/dnx/oam/cint_dnx_oam_hlm.c
 * cint
 * oam_hierarchical_lm_example(0,201,203);
 * exit;
 *
 * Send data packet to LIF 1, expect to be forwarded to LIF 2 and to increment all counters
 *   tx 1 psrc=201 data=0x0000000000100000fb32a37b81000010ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x0000000000100000fb32a37b81000010ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Sending LMM packet with MDL 1, expect to be trapped to the CPU
 *   tx 1 psrc=201 data=0x0000000102030000174b0c8b810000108902202b000c00000000000000000000000000
 *
 */

struct dnx_oam_hlm_config_s
{
    int hierarchical_lm_is_acc;
    int is_pcp;
    int lm_counter_base_ids[4];
    int lm_counter_if[3];
};

dnx_oam_hlm_config_s dnx_oam_hlm_info = {
    /*
     * hierarchical_lm_is_acc
     */
    0,
    /*
     * is_pcp
     */
    0,
    /*
     * lm_counter_base_ids
     */
    {0, 0, 0, 0},
    /*
     * lm_counter_if
     */
    {0, 1, 2}
};

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
     * Allocate counter engines and get counters
     */
    rv = set_counter_resource(unit, port1, dnx_oam_hlm_info.lm_counter_if[0], 1, &dnx_oam_hlm_info.lm_counter_base_ids[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in set_counter_resource.\n", rv);
        return rv;
    }
    rv = set_counter_resource(unit, port1, dnx_oam_hlm_info.lm_counter_if[1], 1, &dnx_oam_hlm_info.lm_counter_base_ids[2]);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in set_counter_resource.\n", rv);
        return rv;
    }
    rv = set_counter_resource(unit, port2, dnx_oam_hlm_info.lm_counter_if[0], 1, &dnx_oam_hlm_info.lm_counter_base_ids[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in set_counter_resource.\n", rv);
        return rv;
    }
    rv = set_counter_resource(unit, port2, dnx_oam_hlm_info.lm_counter_if[1], 1, &dnx_oam_hlm_info.lm_counter_base_ids[3]);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in set_counter_resource.\n", rv);
        return rv;
    }

    /**
     * Create VLAN ports
     */
    bcm_vlan_port_t_init(&vp1);
    vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vp1.port = port1;
    vp1.match_vlan = vid1;
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
    vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vp2.port = port2;
    vp2.match_vlan = vid2;
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
     */
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
    if (dnx_oam_hlm_info.is_pcp)
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

    /**
     * Set My-CFM-MAC
     */
    rv = oam__my_cfm_mac_set(unit, port1, dnx_oam_endpoint_info_glb.mac_mep_1);
    if (rv != BCM_E_NONE) {
        printf("oam__my_cfm_mac_set fail \n");
        return rv;
    }

    dnx_oam_results_glb.station_id[0] = gl_station_id;

    rv = oam__my_cfm_mac_set(unit, port2, dnx_oam_endpoint_info_glb.mac_mep_2);
    if (rv != BCM_E_NONE) {
        printf("oam__my_cfm_mac_set fail \n");
        return rv;
    }

    dnx_oam_results_glb.station_id[1] = gl_station_id;

    if (!dnx_oam_hlm_info.hierarchical_lm_is_acc)
    {
        /**
         * Add non-accelerated Down-MEP
         */
        bcm_oam_endpoint_info_t_init(&mep_1);
        mep_1.type = bcmOAMEndpointTypeEthernet;
        mep_1.group = group_info_short_ma.id;
        mep_1.level = down_mdl_1;
        mep_1.lm_flags = BCM_OAM_LM_PCP * dnx_oam_hlm_info.is_pcp;
        mep_1.gport = gport1;
        mep_1.lm_counter_if = dnx_oam_hlm_info.lm_counter_if[0];
        mep_1.lm_counter_base_id = dnx_oam_hlm_info.lm_counter_base_ids[0];
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
        mep_1.acc_profile_id = ingress_acc_profile;
        mep_1.id = 0x8000;
        mep_1.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        mep_1.vlan = 0x10;
        mep_1.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1) */
        mep_1.outer_tpid = 0x8100;
        mep_1.inner_vlan = 0;
        mep_1.inner_pkt_pri = 0;
        mep_1.inner_tpid = 0;
        mep_1.int_pri = 2 + (1<<2);
        mep_1.sampling_ratio = 0;
        mep_1.lm_flags = BCM_OAM_LM_PCP * dnx_oam_hlm_info.is_pcp; /* In case of PCP set the flag */
        mep_1.flags2 =  BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE; /* Take RDI only from scanner */

        /* The MAC address that the CCM packets are sent with */
        sal_memcpy(mep_1.src_mac_address, dnx_oam_endpoint_info_glb.src_mac_mep_3, 6);

        /* RX */
        mep_1.gport = vp1.vlan_port_id;
        mep_1.lm_counter_if = dnx_oam_hlm_info.lm_counter_if[0];
        mep_1.lm_counter_base_id = dnx_oam_hlm_info.lm_counter_base_ids[0];
    }

    printf("bcm_oam_endpoint_create mep_1\n");
    rv = bcm_oam_endpoint_create(unit, &mep_1);
    if (rv != BCM_E_NONE)
    {
       printf("Error no %d, in bcm_oam_endpoint_create.\n", rv);
       return rv;
    }
    printf("Created Down-MEP with id %d\n", mep_1.id);

    dnx_oam_results_glb.ep1.gport = mep_1.gport;
    dnx_oam_results_glb.ep1.id = mep_1.id;

    /**
     * Add another non-accelerated Down-MEP
     */
    bcm_oam_endpoint_info_t_init(&mep_3);
    mep_3.type = bcmOAMEndpointTypeEthernet;
    mep_3.group = group_info_short_ma.id;
    mep_3.level = down_mdl_2;
    mep_3.lm_flags = BCM_OAM_LM_PCP * dnx_oam_hlm_info.is_pcp; /* In case of PCP set the flag */
    mep_3.gport = gport1;
    mep_3.lm_counter_if = dnx_oam_hlm_info.lm_counter_if[1];
    mep_3.lm_counter_base_id = dnx_oam_hlm_info.lm_counter_base_ids[2];

    printf("bcm_oam_endpoint_create mep_3\n");
    rv = bcm_oam_endpoint_create(unit, &mep_3);
    if (rv != BCM_E_NONE)
    {
       printf("Error no %d, in bcm_oam_endpoint_create.\n", rv);
       return rv;
    }
    printf("Created non-accelerated Down-MEP with id %d\n", mep_3.id);

    dnx_oam_results_glb.ep2.gport = mep_3.gport;
    dnx_oam_results_glb.ep2.id = mep_3.id;

    if (!dnx_oam_hlm_info.hierarchical_lm_is_acc)
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
        mep_4.lm_flags = BCM_OAM_LM_PCP * dnx_oam_hlm_info.is_pcp; /* In case of PCP set the flag */
        mep_4.lm_counter_if = dnx_oam_hlm_info.lm_counter_if[0];
        mep_4.lm_counter_base_id = dnx_oam_hlm_info.lm_counter_base_ids[1];
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
        mep_4.acc_profile_id = egress_acc_profile;
        mep_4.id = 0x8004;
        mep_4.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        mep_4.vlan = 0x20;
        mep_4.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1) */
        mep_4.outer_tpid = 0x8100;
        mep_4.inner_vlan = 0;
        mep_4.inner_pkt_pri = 0;
        mep_4.inner_tpid = 0;
        mep_4.int_pri = 0;
        mep_4.sampling_ratio = 0;
        mep_4.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE; /* Take RDI only from scanner */

        /* The MAC address that the CCM packets are sent with */
        sal_memcpy(mep_4.src_mac_address, dnx_oam_endpoint_info_glb.src_mac_mep_2, 6);

        /* RX */
        mep_4.gport = vp2.vlan_port_id;
        mep_4.lm_flags = BCM_OAM_LM_PCP * dnx_oam_hlm_info.is_pcp; /* In case of PCP set the flag */
        mep_4.lm_counter_if = dnx_oam_hlm_info.lm_counter_if[0];
        mep_4.lm_counter_base_id = dnx_oam_hlm_info.lm_counter_base_ids[1];
    }

    printf("bcm_oam_endpoint_create mep_4\n");
    rv = bcm_oam_endpoint_create(unit, &mep_4);
    if (rv != BCM_E_NONE)
    {
       printf("Error no %d, in bcm_oam_endpoint_create.\n", rv);
       return rv;
    }
    printf("Created Up-MEP with id %d\n", mep_4.id);

    dnx_oam_results_glb.ep3.gport = mep_4.gport;
    dnx_oam_results_glb.ep3.id = mep_4.id;

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
    mep_6.lm_flags = BCM_OAM_LM_PCP * dnx_oam_hlm_info.is_pcp; /* In case of PCP set the flag */
    mep_6.lm_counter_if = dnx_oam_hlm_info.lm_counter_if[1];
    mep_6.lm_counter_base_id = dnx_oam_hlm_info.lm_counter_base_ids[3];

    printf("bcm_oam_endpoint_create mep_6\n");
    rv = bcm_oam_endpoint_create(unit, &mep_6);
    if (rv != BCM_E_NONE)
    {
       printf("Error no %d, in bcm_oam_endpoint_create.\n", rv);
       return rv;
    }
    printf("Created MEP with id %d\n", mep_6.id);

    dnx_oam_results_glb.ep4.gport = mep_6.gport;
    dnx_oam_results_glb.ep4.id = mep_6.id;

    return rv;
}

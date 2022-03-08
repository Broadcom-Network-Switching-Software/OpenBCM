/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_dnx_oam_eth_over_srv6.c Purpose: basic examples of upmep injection enters Srv6 tunnel.
 */

/* Short group indication */
int is_short_maid = 1;
int nof_mep = 0;

bcm_oam_endpoint_t oam_up_mep_id = 4096;
bcm_mac_t src_mac_up = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

/**
 * Example of upmep injection enters Srv6 tunnel
 * Up-MEP will have a loss and a delay entry.
 *
 * packet format of injected oam packet is OAMoEth
 * packet format of output packet is OAMoSRv6_2oIPv6oETH
 *
 * @param unit
 * @param port1
 * @param port2
 * @param nof_sids
 *
 * @return int
 */
int dnx_eth_oam_up_mep_over_srv6 (
    int unit,
    bcm_port_t port1,
    bcm_port_t port2, 
    int nof_sids)
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
    int core_id=0; 
    bcm_oam_control_key_t keyA;
    uint32 rcy_port = 60;
    uint64 arg;
    int up_mep_tod_recycle_enable = *(dnxc_data_get(unit,  "oam", "feature", "up_mep_tod_recycle_enable", NULL));

    int secondary_srh_flags = 0;  /** additional SRH Encap flags such as BCM_SRV6_SRH_BASE_INITIATOR_UNIFIED */
    int secondary_sids_flags = 0; /** additional SIDs Encap flags such as BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED */
    int is_t_insert_encap = 0;
    int last_sid_is_dip = 1;

    bcm_vswitch_cross_connect_t gports;

    /*
     * Create cross-connected VLAN ports
     */
    rv = dnx_oam_cross_connected_vlan_ports_create(unit, port1, port2, vid, vid, &gport1, &gport2);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in dnx_oam_cross_connected_vlan_ports_create.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("finished Create cross-connected VLAN ports\n");

    /*
     * Configuration Ingress SRv6 Tunnel
     */
    rv = srv6_ingress_tunnel_config(unit, port2, port2, port1, nof_sids, secondary_srh_flags, secondary_sids_flags, is_t_insert_encap, last_sid_is_dip);
    if (rv != BCM_E_NONE)
    {
        printf("Error in srv6_ingress_tunnel_config\n");
        return rv;
    }
    printf("finished Configuration Ingress SRv6 Tunnel\n");

    /*
     * Connect inlif to Srv6 tunnel
     */

    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = gport2;
    BCM_GPORT_FORWARD_PORT_SET(gports.port2, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_L2VPN_FEC_ID]);
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_vswitch_cross_connect_add to connect inlif to Srv6 tunnel\n");
        return rv;
    }
    printf("finished Connect inlif to Srv6 tunnel\n");

    /*
     * Setting My-CFM-MAC
     */
    rv = oam__my_cfm_mac_set(unit, port2, my_cfm_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in oam__my_cfm_mac_set.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("finished Setting my-cfm-mac\n");

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
    printf("finished Allocate counter engines\n");

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
    printf("finished oam profile\n");

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
    printf("finished oam group\n");

    /*
     * Create accelerated Up-MEP
     */
    rv = dnx_oam_accelerated_endpoint_create(unit, oam_up_mep_id, gport2, group_info.id,
                                             counter_if_0, lm_counter_base_id_0, acc_egress_profile, 0, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in dnx_oam_accelerated_endpoint_create (Up MEP).\n", bcm_errmsg(rv));
        return rv;
    }
    printf("finished create accelerated Up-MEP\n");

    /*
     * Add loss on Up-MEP
     */
    rv = dnx_oam_loss_add(unit, oam_up_mep_id, 65552, BCM_OAM_ENDPOINT_CCM_PERIOD_10MS, 0, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in dnx_oam_loss_add.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("finished add loss\n");

    /*
     * Add delay on Up-MEP
     */
    if(up_mep_tod_recycle_enable)
    {
        for(core_id=0; core_id<=1; core_id++)
        {
            keyA.type = bcmOamControlUpMepDmRecyclePort;
            keyA.index = core_id;
            COMPILER_64_SET(arg, 0, rcy_port+core_id);
            rv = bcm_oam_control_indexed_set(0,keyA,arg);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }
        }
    }

    rv = dnx_oam_delay_add(unit, oam_up_mep_id, 0, BCM_OAM_ENDPOINT_CCM_PERIOD_10MS);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in dnx_oam_loss_add.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("finished add delay\n");

    return rv;
}

/**
 * Create accelerated OAM endpoint
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
    acc_endpoint.flags = 0;
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

    rv = bcm_oam_endpoint_create(unit, acc_endpoint);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_endpoint_create.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Created Local MEP: ID %d\n", acc_endpoint.id);

    if (is_up)
    {
        oam_up_mep_id = acc_endpoint.id;
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
    rv = bcm_oam_delay_add(unit, &delay);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %s, in bcm_oam_delay_add.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

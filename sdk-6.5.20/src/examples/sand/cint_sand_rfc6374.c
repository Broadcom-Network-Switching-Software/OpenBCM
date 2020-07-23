/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_sand_rfc6374.c Purpose: basic examples for MplsLmDm.
 */
/*Section OAM indication */
int is_section_oam = 0;

/**
 * Indicate dm priority when adding RFC-6374 DM with priority
 * and for Jericho2 only
 */
int dm_priority = 0;

/**
 * Indicate mep id duplicated from the original mep db entry 
 * when adding RFC-6374 DM with priority
 * and for Jericho2 only
 */
int duplicated_mep_id = 9600;

int port_1 = 13;
int port_2 = 14;
int mpls_lm_dm_trap_dest_port = BCM_GPORT_INVALID;
int is_ntp = 0;
/* Enable/disable LM/DM report mode */
int oam_lm_dm_reports_enabled = 0;
/* Enable/disable LM/DM statistics mode */
int oam_lm_dm_statistics_enabled = 0;

uint32 delay_report_count = 0;
uint32 last_delta_FCB = 0;
uint32 last_delta_FCf = 0;
uint32 delay_seconds = 0;
uint32 delay_nanoseconds = 0;

/* Inferred LM indication */
int is_inferred_lm = 0;

int lm_counter_base_id;

/* Disable counting for certain opcode */
int count_disable = 0;
int is_1way_delay = 0;
int cpu_trap_code = 0;

bcm_oam_endpoint_info_t mpls_lm_dm_mep_info;

int mpls_lm_dm_profile_id;
int mpls_lm_dm_acc_profile_id;

/** Trap ID for MPLS-LM-DM */
int mpls_lm_dm_punt_trap;

/**
 * Triggered upon LM/DM report RX.
 */
int cb_mpls_lm_dm_stat(int unit,
            bcm_oam_event_type_t event_type,
            bcm_oam_group_t group,
            bcm_oam_endpoint_t endpoint,
            bcm_oam_performance_event_data_t *event_data,
            void *user_data) {

    print unit;
    print event_type;
    print endpoint;

    /* Update globals */
    switch (event_type) {
    case bcmOAMEventEndpointDmStatistics:
        delay_report_count++;
        delay_seconds = COMPILER_64_HI(event_data->last_delay);
        delay_nanoseconds = COMPILER_64_LO(event_data->last_delay);
        printf("Delay in seconds = %d, nanoseconds = %d\n", delay_seconds, delay_nanoseconds);
        break;
    case bcmOAMEventEndpointLmStatistics:
        last_delta_FCB = event_data->delta_FCB;
        last_delta_FCf = event_data->delta_FCf;
        printf("last_delta_FCB = %d, last_delta_FCf = %d\n", last_delta_FCB, last_delta_FCf);
        break;
    }
}

/**
 * sets trap for OAM MPLS LM DM error to specific port
 *
 * @param unit
 * @param dest_port_or_queue
 * @param is_queue 
 *
 * @return int
 */
int mpls_lm_dm_trap_set(int unit, int dest_port_or_queue, int is_queue) {
    bcm_rx_trap_config_t trap_config;
    int trap_code=0x488; /* trap code on FHEI  will be (trap_code & 0xff), in this case 0x88.*/
    /* valid trap codes for oamp traps are 0x401 - 0x4ff */
    int rv, is_dnx;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_device_member_get\n", rv);
        return rv;
    }

    if(is_dnx)
    {
        /* For the DNX family, get the trap code configured for this punt type */
        rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOampMplsLmDmErr, &trap_code);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_rx_trap_type_get\n", rv);
            return rv;
        }
        mpls_lm_dm_punt_trap = (trap_code & 0xFF);
    	trap_config.flags = BCM_RX_TRAP_TRAP | BCM_RX_TRAP_WITH_ID;
    }
    else
    {
        rv =  bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapOampMplsLmDmErr, &trap_code);
        if (rv != BCM_E_NONE) {
           printf("trap create: (%s) \n",bcm_errmsg(rv));
           return rv;
        }
    }

    if (is_queue) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(trap_config.dest_port, dest_port_or_queue);
    }
    else {
        BCM_GPORT_SYSTEM_PORT_ID_SET(trap_config.dest_port, dest_port_or_queue);
    }

    rv = bcm_rx_trap_set(unit, trap_code, trap_config);
    if (rv != BCM_E_NONE) {
        printf("trap set: (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/**
 * Set defaut action for response and query control codes
 *
 * @param unit
 * @param dest_port_or_queue
 * @param is_queue 
 *
 *
 * @return int
 */
int oam_mpls_LmDm_filter_config (int unit, int dest_port_or_queue, int is_queue) {

    int i, rv;
    uint8 action;
    bcm_oam_control_key_t keyA;
    uint64 arg;

    /** Create OAMP trap code for MplsLmDM trap */
    rv = mpls_lm_dm_trap_set(unit, dest_port_or_queue, is_queue);
    if (rv != BCM_E_NONE) {
       printf("trap create: (%s) \n",bcm_errmsg(rv));
       return rv;
    }

    /** Configure default action for response control codes
     *  Codes 0x0-0x1   - Success              - Pass
     *  Codes 0x0-0xF   - Error notifications  - Ignore
     *  0x10 - 0x1D     - Error                - Punt
     */
    keyA.type = bcmOamControlMplsLmDmOampActionPerResponseControlCode;
    for (i = 0; i <= 0x1D; ++i)
    {
        if (i <= 1)
        {
            action = 0; /** Pass */
        } 
        else if (i <= 0xf)
        {
            action = 1; /** Ignore */
        }
        else
        {
            action = 2; /** Punt */
        }

        keyA.index = i;
        COMPILER_64_SET(arg, 0, action);
        rv = bcm_oam_control_indexed_set(unit, keyA, arg);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        /** Reset the value */
        COMPILER_64_SET(arg, 0, 0);
        rv = bcm_oam_control_indexed_get(unit, keyA, &arg);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        if (COMPILER_64_LO(arg) != action)
        {
            printf("Expected value:%d, is different than actual:%d \n",COMPILER_64_LO(arg), action);
        }
    }

   
    /** Configure default action query control codes
     *  0x0: In-band Response Requested.        - Pass
     *  0x1: Out-of-band Response Requested.    - Pass
     *  0x2: No Response Requested.             - Ignore
     */
    keyA.index = 0x2;
    keyA.type = bcmOamControlMplsLmDmOampActionPerQueryControlCode;
    COMPILER_64_SET(arg, 0, 1 /** Ignore */);
       
    rv = bcm_oam_control_indexed_set(unit,keyA,arg);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    COMPILER_64_SET(arg, 0, 0);
   
    rv = bcm_oam_control_indexed_get(unit,keyA,&arg);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    if (COMPILER_64_LO(arg) != 0x1)
    {
        printf("Expected value: 0x1, is different than actual:%d \n",COMPILER_64_LO(arg));
    }

    return rv;
}

/**
 * Create and set action profiles for MplsLmDm.
 *
 * cint_oam_action.c is required for jericho2.
 *
 * @param unit
 * @param mep_type     - endpoint type.
 * @param is_ntp       - set "1" for NTP timestamp format, and "0" for IEEE1588 timestamp format
 * @param count_lm_dm  - set "1" to enable, and "0" to disable count of LM packets.
 * @param profile_id  - return lif profile id.
 * @param acc_profile_id  - return acc profile id.
 * @return int
 */
int mpls_lm_dm_action_profiles_config(int unit,int mep_type, int is_ntp, int count_lm_dm, bcm_oam_profile_t * profile_id, bcm_oam_profile_t * acc_profile_id)
{
    int rv;
    uint32 flags;
    bcm_oam_profile_type_t profile_type;

    flags = (mep_type == bcmOAMEndpointTypeMplsLmDmSection) ? BCM_OAM_PROFILE_TYPE_SECTION : 0;
    flags |= is_ntp ? BCM_OAM_PROFILE_DM_TYPE_NTP : 0;
    profile_type = bcmOAMProfileIngressLIF;
    rv = bcm_oam_profile_create(unit, flags, profile_type, profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_profile_create, bcmOAMProfileIngressLIF.\n", rv);
        return rv;
    }

    profile_type = bcmOAMProfileIngressAcceleratedEndpoint;
    rv = bcm_oam_profile_create(unit, flags, profile_type, acc_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_profile_create, bcmOAMProfileIngressAcceleratedEndpoint.\n", rv);
        return rv;
    }

    mpls_lm_dm_profile_id = *profile_id;
    mpls_lm_dm_acc_profile_id = *acc_profile_id;

    rv = dnx_mpls_lm_dm_actions_config(unit, 0 /** non-acc */, *profile_id, count_lm_dm);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in mpls_lm_dm_actions_config for non-acc.\n", rv);
        return rv;
    }

    rv = dnx_mpls_lm_dm_actions_config(unit, 1 /** acc */, *acc_profile_id, count_lm_dm);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in mpls_lm_dm_actions_config for acc profile.\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Set action to specific destination for provided profile.
 *
 * cint_oam_action.c is required for jericho2.
 *
 * @param unit
 * @param is_acc     - set to "1" for accelerated endpoints
 * @param profile_id - profile_id to be configured for MplsLmDm
 * @param count_lm_dm  - set to "1" for mpls_lm_dm packets to be counted
 * @return int
 */
int dnx_mpls_lm_dm_actions_config(int unit, int is_acc, bcm_oam_profile_t profile_id, int count_lm_dm)
{
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    oam_destinations_s oam_destinations;
    bcm_gport_t dest_port;
    int flags = 0;
    int rv= 0;

    rv = oam_get_destinations(unit, &oam_destinations);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in oam_get_destinations.\n", rv);
        return rv;
    }

    if (is_acc)
    {
        dest_port = oam_destinations.oamp_mpls_dest_gport;
    }
    else
    {
        dest_port = oam_destinations.active_down_dest_gport;
    }

    printf("dest port for MplsLmDm 0x%x\n",dest_port);

    /* Action for LM */
    action_key.flags |= BCM_OAM_ACTION_KEY_MPLS_LM_DM_OPCODE;
    action_key.inject = 0;
    action_key.endpoint_type = bcmOAMMatchTypeMEP;
    action_key.mpls_lm_dm_opcode = bcmOamMplsLmDmOpcodeTypeLm;
    action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;

    action_result.flags = 0;
    action_result.meter_enable = 0;
    action_result.destination = dest_port;
    action_result.counter_increment = count_lm_dm;

    rv = bcm_oam_profile_action_set(unit, flags, profile_id, &action_key, &action_result);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_oam_profile_action_set: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* Action for DM */
    action_key.flags |= BCM_OAM_ACTION_KEY_MPLS_LM_DM_OPCODE;
    action_key.inject = 0;
    action_key.endpoint_type = bcmOAMMatchTypeMEP;
    action_key.mpls_lm_dm_opcode = bcmOamMplsLmDmOpcodeTypeDm;
    action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;

    action_result.flags = 0;
    action_result.meter_enable = 0;
    action_result.destination = dest_port;
    action_result.counter_increment = count_lm_dm;

    rv = bcm_oam_profile_action_set(unit, flags, profile_id, &action_key, &action_result);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_oam_profile_action_set: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/**
 * Create non-accelerated MplsLmDm endpoint
 *
 * cint_sand_oam_y1731_over_tunnel.c is required for jericho2
 *
 * @param unit
 * @param type - any of the following types:
 *             - bcmOAMEndpointTypeMplsLmDmLsp
 *             - bcmOAMEndpointTypeMplsLmDmPw
 *             - bcmOAMEndpointTypeMplsLmDmSection
 * @param use_mpls_out_gport - used for TX counting
 * @param set_as_accelerated - set to "1" for accelerated and to "0" for non accelerated MEP
 * @param count_lm_dm  - set to "1" for mpls_lm_dm packets to be counted
 *
 * @return int
 */
int mpls_lm_dm_endpoint_create(int unit, bcm_oam_endpoint_type_t type, int use_mpls_out_gport, int set_as_accelerated, int count_lm_dm)
{
    bcm_error_t rv;
    bcm_gport_t oamp_port;
    bcm_gport_t out_gport = BCM_GPORT_INVALID;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    int encap_id;
    int gport;
    int intf_id;
    int label;
    int mpls_termination_label_index_enable;
    int tx_port;
    int opcode = bcmOamMplsLmDmOpcodeTypeLm;
    bcm_oam_event_types_t loss_event, delay_event;
    int counter_port;
    bcm_oam_profile_type_t profile_type;
    bcm_oam_profile_t profile_id, acc_profile_id;
    int flags;
    int mpls_outlif , mpls_label;
    bcm_gport_t mpls_out_gport;


    rv = oam__device_type_get(unit, &device_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error in oam__device_type_get: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    if (device_type <= device_type_arad_plus)
    {
        printf("Error: MplsLmDm is not supported for Arad, Arad+, Jericho2 and above.\n");
        return -1;
    }

    if (device_type >= device_type_jericho2)
    {
        if (type == bcmOAMEndpointTypeMplsLmDmLsp)
        {
            rv = mpls_oam_init(unit, bcmOAMEndpointTypeBHHMPLS, 0, &gport, &mpls_outlif, &mpls_label, &mpls_out_gport, &tunnel_switch, &mpls_termination_label_index_enable);
            if (rv != BCM_E_NONE) {
              printf("Error, in mpls_init\n");
              return rv;
            }
            label = mpls_label;
            intf_id = mpls_outlif; /* out lif: mpls_port->encap_id */
            tx_port = port_2;
        }
        else if (type == bcmOAMEndpointTypeMplsLmDmPw)
        {
            rv = mpls_oam_init(unit, bcmOAMEndpointTypeBHHPwe, 0, &gport, &mpls_outlif, &mpls_label, &mpls_out_gport, &tunnel_switch, &mpls_termination_label_index_enable);
            if (rv != BCM_E_NONE) {
              printf("Error, in mpls_init\n");
              return rv;
            }
            label = mpls_label;
            intf_id = mpls_outlif; /* out lif: mpls_port->encap_id */
            tx_port = port_2;
        }
        else if (type == bcmOAMEndpointTypeMplsLmDmSection)
        {
            rv = mpls_oam_init(unit, bcmOAMEndpointTypeBhhSection, 0, &gport, &mpls_outlif, &mpls_label, &mpls_out_gport, &tunnel_switch, &mpls_termination_label_index_enable);
            if (rv != BCM_E_NONE) {
              printf("Error, in mpls_init\n");
              return rv;
            }
            label = mpls_label;
            intf_id = mpls_outlif; /* out lif: mpls_port->encap_id */
            tx_port = port_2;
        }
        else
        {
            printf("Endpoint type is not supported.\n");
            return -1;
        }
    }
    else
    {
      if (type == bcmOAMEndpointTypeMplsLmDmLsp)
      {
        /* Use the mpls_lsr function */
        printf("Call mpls_lsr_tunnel_example\n");
        rv = mpls_lsr_tunnel_example(&unit, 1, port_1, port_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error in mpls_lsr_tunnel_example: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /* Read mpls index soc property */
        mpls_termination_label_index_enable = soc_property_get(unit, "mpls_termination_label_index_enable", 0);
        if (mpls_termination_label_index_enable)
        {
            gport = ingress_tunnel_id_indexed[0];
        }
        else
        {
            gport = ingress_tunnel_id;
        }

        label = 1234;
        BCM_GPORT_TUNNEL_ID_SET(out_gport, mpls_lsr_info_1.ingress_intf);
        intf_id = mpls_lsr_info_1.encap_id;
        tx_port = port_2;
      }
    else if (type == bcmOAMEndpointTypeMplsLmDmPw)
    {
        /* Use the pwe_init function */
        printf("Call pwe_init\n");
        pwe_cw = 1;
        rv = pwe_init(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error in pwe_init: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        label = 1234;
        gport = mpls_lsr_info_1.mpls_port_id;
        out_gport = mpls_lsr_info_1.mpls_port_id;
        intf_id = mpls_lsr_info_1.encap_id; /* out lif: mpls_port->encap_id */
        tx_port = port_2;
    }
    else if (type == bcmOAMEndpointTypeMplsLmDmSection)
    {
        /* Use the mpls_various_scenarios_main function */
        printf("Call mpls_various_scenarios_main\n");
        is_section_oam = 1;
        cint_mpls_various_scenarios_info.tunnel_label = 900;
        rv = mpls_various_scenarios_main(unit, port_1, port_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error in mpls_various_scenarios_main: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        label = 13;
        gport = cint_mpls_various_scenarios_info.vport_id1;
        intf_id = cint_mpls_various_scenarios_info.arp_encap_id1;
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(out_gport, cint_mpls_various_scenarios_info.arp_encap_id1);
        tx_port = port_1;
      }
      else
      {
        printf("Endpoint type is not supported.\n");
        return -1;
      }
    }

    counter_port = port_2;
    if (type == bcmOAMEndpointTypeMplsLmDmSection && device_type < device_type_jericho2)
    {
        counter_port = port_1;
    }

    if (device_type >= device_type_jericho2)
    {
        /* Allocate counter */
        rv = set_counter_resource(unit, counter_port, 0, 1, &lm_counter_base_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }

        rv = mpls_lm_dm_action_profiles_config(unit, type, is_ntp, count_lm_dm, &profile_id, &acc_profile_id);
        if (rv != BCM_E_NONE)
        {
           printf("Error in bcm_oam_lif_profile_set: (%s)\n", bcm_errmsg(rv));
           return rv;
        }

        /* set mp profile for MplsLmDm endpoint */
        flags = 0;
        rv = bcm_oam_lif_profile_set(unit,flags,gport,profile_id,-1);
        if (rv != BCM_E_NONE)
        {
           printf("Error in bcm_oam_lif_profile_set: (%s)\n", bcm_errmsg(rv));
           return rv;
        }
    }
    else
    {
        /* Allocate counter */
        rv = set_counter_source_and_engines(unit, &lm_counter_base_id, counter_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error in set_counter_source_and_engines: (%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Create MplsLmDm endpoint */
    bcm_oam_endpoint_info_t_init(&mpls_lm_dm_mep_info);

    mpls_lm_dm_mep_info.type = type;
    /** Endpoint Statistics require lower endpoint ID to configure all counters */
    mpls_lm_dm_mep_info.id = oam_lm_dm_statistics_enabled * 4100;
    mpls_lm_dm_mep_info.gport = gport; /* In-LIF */
    mpls_lm_dm_mep_info.mpls_out_gport = (use_mpls_out_gport ? out_gport : BCM_GPORT_INVALID); /* Out-LIF */
    mpls_lm_dm_mep_info.lm_counter_base_id = lm_counter_base_id;
    /** If it is required enable TX/RX oamp stat counting */
    mpls_lm_dm_mep_info.flags2 |= oam_lm_dm_statistics_enabled * (BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS | BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS | BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS);
    /** Endpoint Statistics require lower endpoint ID to configure all counters */
    mpls_lm_dm_mep_info.flags |= oam_lm_dm_statistics_enabled * BCM_OAM_ENDPOINT_WITH_ID;
    if (device_type >= device_type_jericho2)
    {
      mpls_lm_dm_mep_info.lm_counter_if = 0;
    }
    if (set_as_accelerated)
    {
        BCM_GPORT_SYSTEM_PORT_ID_SET(mpls_lm_dm_mep_info.tx_gport, port_2);
        mpls_lm_dm_mep_info.intf_id = intf_id; /* Out-LIF */
        mpls_lm_dm_mep_info.session_id = 234;
        mpls_lm_dm_mep_info.egress_label.label = label;
        mpls_lm_dm_mep_info.egress_label.ttl = 20;
        mpls_lm_dm_mep_info.egress_label.exp = 0;
        if (device_type >= device_type_jericho2)
        {
            /** 
            * Pointer to MplsLmDm additional entry
            * LM, LM STAT, DM will be placed to same entry next bank.
            */
            mpls_lm_dm_mep_info.extra_data_index = 0x9000;
            mpls_lm_dm_mep_info.acc_profile_id = acc_profile_id;
            mpls_lm_dm_mep_info.egress_label.ttl = 0xa;
            mpls_lm_dm_mep_info.egress_label.exp = 1;
        }
    }

    if (is_inferred_lm)
    {
        mpls_lm_dm_mep_info.flags2 |= BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM;
        opcode = bcmOamMplsLmDmOpcodeTypeIlm;
    }

    if (device_type < device_type_jericho2)
    {
        /**
         * For Jericho and Jericho_plus the default timestamp_format is bcmOAMTimestampFormatNTP,
         * set "oam_dm_ntp_enable" SOC to "0" to enable bcmOAMTimestampFormatIEEE1588v1
         */
        mpls_lm_dm_mep_info.timestamp_format = (soc_property_get(unit, "oam_dm_ntp_enable", 1) ? bcmOAMTimestampFormatNTP : bcmOAMTimestampFormatIEEE1588v1);
    }
    else
    {
        /**
         * For Jericho2 and above the default timestamp_format is bcmOAMTimestampFormatIEEE1588v1,
         * set "is_ntp" global variable to "1" to enable bcmOAMTimestampFormatNTP
         */
         mpls_lm_dm_mep_info.timestamp_format = is_ntp ? bcmOAMTimestampFormatNTP : bcmOAMTimestampFormatIEEE1588v1;

    }

    rv = bcm_oam_endpoint_create(unit, &mpls_lm_dm_mep_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_oam_endpoint_create: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Created MplsLmDm MEP with ID %d\n", mpls_lm_dm_mep_info.id);

    if (set_as_accelerated)
    {
        if (device_type < device_type_jericho2)
        {
            rv = oamp_gport_get(unit, &oamp_port);
            if (rv != BCM_E_NONE)
            {
                printf("Error in mpls_lm_dm_endpoint_action_set: (%s)\n", bcm_errmsg(rv));
                return rv;
            }

            /* Set action for LM trap to OAMP */
            rv = mpls_lm_dm_endpoint_action_set(unit, oamp_port, mpls_lm_dm_mep_info.id, bcmOAMActionFwd, opcode);
            if (rv != BCM_E_NONE)
            {
                printf("Error in mpls_lm_dm_endpoint_action_set: (%s)\n", bcm_errmsg(rv));
                return rv;
            }

            /* Set the action to count LM packets, required only for Jericho device */
            if ((device_type < device_type_jericho_plus) && is_inferred_lm)
            {
                rv = mpls_lm_dm_endpoint_action_set(unit, BCM_GPORT_INVALID, mpls_lm_dm_mep_info.id, bcmOAMActionCountEnable, opcode);
                if (rv != BCM_E_NONE)
                {
                    printf("Error in mpls_lm_dm_endpoint_action_set: (%s)\n", bcm_errmsg(rv));
                    return rv;
                }
            }
            /* Set action for DM trap to OAMP */
            rv = mpls_lm_dm_endpoint_action_set(unit, oamp_port, mpls_lm_dm_mep_info.id, bcmOAMActionFwd, bcmOamMplsLmDmOpcodeTypeDm);
            if (rv != BCM_E_NONE)
            {
                printf("Error in mpls_lm_dm_endpoint_action_set: (%s)\n", bcm_errmsg(rv));
                return rv;
            }
        }

        bcm_oam_loss_t loss_obj;
        bcm_oam_loss_t_init(&loss_obj);

        loss_obj.id = mpls_lm_dm_mep_info.id;
        loss_obj.period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
        loss_obj.flags = BCM_OAM_LOSS_SINGLE_ENDED;
        if (oam_lm_dm_reports_enabled)
        {
            loss_obj.flags |= BCM_OAM_LOSS_REPORT_MODE;
        }
        else
        {
            /* Use extended statistics*/
            loss_obj.flags |= BCM_OAM_LOSS_STATISTICS_EXTENDED; /* LMM based loss management*/
        }

        rv = bcm_oam_loss_add(unit,&loss_obj);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_oam_loss_add: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        bcm_oam_delay_t delay_obj;
        bcm_oam_delay_t_init(&delay_obj);
        delay_obj.id = mpls_lm_dm_mep_info.id;
        delay_obj.period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
        if (device_type < device_type_jericho2)
        {
            /**
             * For Jericho and Jericho_plus the default timestamp_format is bcmOAMTimestampFormatNTP,
             * set "oam_dm_ntp_enable" SOC to "0" to enable bcmOAMTimestampFormatIEEE1588v1
             */
            delay_obj.timestamp_format = (soc_property_get(unit, "oam_dm_ntp_enable", 1) ? bcmOAMTimestampFormatNTP : bcmOAMTimestampFormatIEEE1588v1);
        }
        else
        {
            /**
             * Setting RFC-6374 DM with priority on Jericho2
             */
            duplicated_mep_id = oam_lm_dm_statistics_enabled ? 4200 : 9600;
            delay_obj.pkt_pri_bitmap = 1<<dm_priority;
            delay_obj.pm_id = duplicated_mep_id;
            delay_obj.delay_id = 36880;
        }
        delay_obj.flags = (oam_lm_dm_reports_enabled ? BCM_OAM_DELAY_REPORT_MODE : 0) | (is_1way_delay ? BCM_OAM_DELAY_ONE_WAY : 0);
        rv = bcm_oam_delay_add(unit,&delay_obj);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_oam_delay_add: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        if (oam_lm_dm_reports_enabled)
        {
            BCM_OAM_EVENT_TYPE_SET(loss_event, bcmOAMEventEndpointLmStatistics);
            rv = bcm_oam_performance_event_register(unit, loss_event, cb_mpls_lm_dm_stat, (void*)1);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_oam_performance_event_register: (%s)\n", bcm_errmsg(rv));
                return rv;
            }

            BCM_OAM_EVENT_TYPE_SET(delay_event, bcmOAMEventEndpointDmStatistics);
            rv = bcm_oam_performance_event_register(unit, delay_event, cb_mpls_lm_dm_stat, (void*)2);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_oam_performance_event_register: (%s)\n", bcm_errmsg(rv));
                return rv;
            }
        }
    }

    return rv;
}

/**
 * Create non-accelerated MplsLmDm over LSP endpoint
 *
 *
 * @param unit
 * @param port1 - port on which the MEP resides
 * @param port2
 * @param use_mpls_out_gport - used for TX counting
 * @param set_as_accelerated - set to "1" for accelerated and to "0" for non accelerated MEP
 * @param count_lm_dm  - set to "1" for mpls_lm_dm packets to be counted
 *
 * @return int
 */
int mpls_lm_dm_run_with_defaults_lsp(int unit, int port1, int port2, int use_mpls_out_gport, int set_as_accelerated,int count_lmm) {
    bcm_error_t rv;
    port_1 = port1;
    port_2 = port2;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in mpls__mpls_pipe_mode_exp_set: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = mpls_lm_dm_endpoint_create(unit, bcmOAMEndpointTypeMplsLmDmLsp, use_mpls_out_gport, set_as_accelerated,count_lmm);
    if (rv != BCM_E_NONE)
    {
        printf("Error in mpls_lm_dm_endpoint_create: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    if (mpls_lm_dm_trap_dest_port != BCM_GPORT_INVALID)
    {
        rv = oam_mpls_LmDm_filter_config(unit, mpls_lm_dm_trap_dest_port, 0);
        if (rv != BCM_E_NONE) {
            printf("Error in mpls__mpls_pipe_mode_exp_set: (%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /** Allocate counters for 4k MEPs with rx/tx statistics enable, counter base is the first MEP.*/
    if (oam_lm_dm_statistics_enabled)
    {
        rv = cint_oam_oamp_statistics_main(unit, mpls_lm_dm_mep_info.id << 3 /** per opcode cnt enable */ , 4096, 2 /** RX/TX enabled */, 1 /** Enable eviction for stat cnt */);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

/**
 * Create non-accelerated MplsLmDm over PWE endpoint
 *
 *
 * @param unit
 * @param port1 - port on which the MEP resides
 * @param port2
 * @param use_mpls_out_gport - used for TX counting
 * @param set_as_accelerated - set to "1" for accelerated and to "0" for non accelerated MEP
 *
 * @return int
 */
int mpls_lm_dm_run_with_defaults_pwe(int unit, int port1, int port2, int use_mpls_out_gport, int set_as_accelerated) {
    bcm_error_t rv;
    port_1 = port1;
    port_2 = port2;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in mpls__mpls_pipe_mode_exp_set: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = mpls_lm_dm_endpoint_create(unit, bcmOAMEndpointTypeMplsLmDmPw, use_mpls_out_gport, set_as_accelerated,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error in mpls_lm_dm_endpoint_create: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    if (mpls_lm_dm_trap_dest_port != BCM_GPORT_INVALID)
    {
        rv = oam_mpls_LmDm_filter_config(unit, mpls_lm_dm_trap_dest_port, 0);
        if (rv != BCM_E_NONE) {
            printf("Error in mpls__mpls_pipe_mode_exp_set: (%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

/**
 * Create non-accelerated Section MplsLmDm endpoint
 *
 *
 * @param unit
 * @param port1 - port on which the MEP resides
 * @param port2
 * @param use_mpls_out_gport - used for TX counting
 * @param set_as_accelerated - set to "1" for accelerated and to "0" for non accelerated MEP
 *
 * @return int
 */
int mpls_lm_dm_run_with_defaults_section(int unit, int port1, int port2, int use_mpls_out_gport, int set_as_accelerated) {
    bcm_error_t rv;
    port_1 = port1;
    port_2 = port2;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in mpls__mpls_pipe_mode_exp_set: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = mpls_lm_dm_endpoint_create(unit, bcmOAMEndpointTypeMplsLmDmSection, use_mpls_out_gport, set_as_accelerated,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error in mpls_lm_dm_endpoint_create: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    if (mpls_lm_dm_trap_dest_port != BCM_GPORT_INVALID)
    {
        rv = oam_mpls_LmDm_filter_config(unit, mpls_lm_dm_trap_dest_port, 0);
        if (rv != BCM_E_NONE) {
            printf("Error in mpls__mpls_pipe_mode_exp_set: (%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

/**
 * MplsLmDm endpoint action set calling sequence example
 *
 *
 * @param unit
 * @param dest_port - Must be BCM_GPORT_INVALID for bcmOAMActionCountEnable.
 *                    Otherwise, it should be a trap.
 * @param endpoint_id - MplsLmDm endpoint ID
 * @param action_type - Action to be applied:
 *                  - bcmOAMActionCountEnable to enable counting.
 *                  - bcmOAMActionFwd to forward the packet to e specified destination.
 *                  - Set count_disable and do not pass any actions to disable counting.
 * @param opcode - MplsLmDm opcode upon which action will be applied:
 *                  - bcmOamMplsLmDmOpcodeTypeLm
 *                  - bcmOamMplsLmDmOpcodeTypeDm
 *                  - bcmOamMplsLmDmOpcodeTypeIlm
 *
 * @return int
 */
int mpls_lm_dm_endpoint_action_set(int unit, bcm_gport_t dest_port, bcm_oam_endpoint_t endpoint_id, bcm_oam_action_type_t action_type, bcm_oam_mpls_lm_dm_opcode_type_t opcode) {
    bcm_error_t rv;
    int trap_code;
    bcm_oam_endpoint_action_t action;
    bcm_rx_trap_t trap_type;
    bcm_rx_trap_config_t trap_config;

    bcm_oam_endpoint_action_t_init(&action);

    action.destination = dest_port;
    if ((dest_port != BCM_GPORT_INVALID) && !BCM_GPORT_IS_TRAP(dest_port)) {
        /**
         * action.destination can only receive a trap as destination.
         * Allocate a new trap.
         */
        trap_type = bcmRxTrapUserDefine;
        rv = bcm_rx_trap_type_create(unit, 0, trap_type, &trap_code);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_rx_trap_type_create: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
        rv = port_to_system_port(unit, dest_port, &trap_config.dest_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error in port_to_system_port: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        rv = bcm_rx_trap_set(unit, trap_code, trap_config);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_rx_trap_set: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        printf("Trap set, trap_code = %d\n", trap_code);
        BCM_GPORT_TRAP_SET(action.destination, trap_code, 7, 0);

        cpu_trap_code = trap_code;
    }

    BCM_OAM_OPCODE_SET(action, opcode);
    if (!count_disable)
    {
        BCM_OAM_ACTION_SET(action, action_type);
    }
    rv = bcm_oam_endpoint_action_set(unit, endpoint_id, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_oam_endpoint_action_set: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

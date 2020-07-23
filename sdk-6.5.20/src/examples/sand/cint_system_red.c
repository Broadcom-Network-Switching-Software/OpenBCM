/*~~~~~~~~~~~~~~~~~~~~~~~~~~ System RED ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_system_red.c
 * Purpose: Example System RED Validation
 * Reference: BCM88650 TM Device Driver (System )
 * 
 * Environment:
 *
 * The following application example assumes the following system configuration:
 *
 * IMPORTANT: this cint configure only the System RED, it does not configure the system application (port, voq, scheduling...) 
 *
 *   gp:0x24000080   cos:8                Unicast Queue Group    mod: 0   port:12 voq: 128)
 *   gp:0x24000088   cos:8                Unicast Queue Group    mod: 0   port:13 voq: 136)
 *   gp:0x24000090   cos:8                Unicast Queue Group    mod: 0   port:14 voq: 144)
 *
 *                                    in-ports: 12, 13            
 *                                    out-ports: 14             
 *                                           --------              
 *                                    -------|      |         
 *                              in_ports     |      | out_port
 *                                    -------|      |--------------
 *                                           |      |              
 *                                           --------    
 *
 *                          
 *  Application description:
 *    1. PP application, including 2 fap_ports, connected to Traffic Generator.
 *    2. UC traffic configuration: 2 streams on each port, each stream has a different DP.
 *         Port_1: X% green and (1-X)% yellow.
 *         Port_2: (1-X)% green and X% yellow.
 *    3. System RED configuration is set to prefer  green packets, over yellow.
 *    4. Qs: Each port is destined to a different Q, both Qs goes to the same destination Port.
 *    5. EGQ/SCH: are configured to set RR between the Qs (no strict priority).
 *    6. Set high rate class drop thresholds (make sure only system red mechanism  is dropping).
 *
 *
 * Test Run:
 *    1. Configure system according to instruction  above
 *    2. Run config_system_red_param().   
 *    3. inject packets according to the instruction  above
 *    2. Expected Output:
 *         a. Validate that only low p. packets are dropped, according to S_RED thresholds.
 *         b. On port_1 the traffic bandwidth  should be X%     (only green pass)
 *         c. On port_2 the traffic bandwidth  should be (1-X)% (only green pass)
 *
 * Output example A:
 *    1. 10Gb port
 *    2. X = 90%
 *    3. for out_port, tc_high: EGQ_PQP_UNICAST_BYTES_COUNTER = 9.000387 Gbps
 *    4. for out_port, tc_low: EGQ_PQP_UNICAST_BYTES_COUNTER = 1.027330 Gbps
 *
 * Output example B:
 *    1. 10Gb port
 *    2. X = 70%
 *    3. for out_port, tc_high: EGQ_PQP_UNICAST_BYTES_COUNTER = 6.975541 Gbps
 *    4. for out_port, tc_low: EGQ_PQP_UNICAST_BYTES_COUNTER = 2.985351 Gbps
 *
 *
 * All the configuration example is in config_system_red_param():
 * Relevant soc properties configuration:
 *   system_red_enable=1
 *
 * Cint running:
  cd ../../../../src/examples/dpp
  cint cint_system_red.c
  cint
  int out_port_voq = 144;
  config_system_red_param_app(unit, out_port_voq);
 *
 *
 * System RED API using example:
 *   1. Setting Dbuff, BDB thresholds:
 *        Usage example: 
           system_red_rsrc_thrs_example(unit, 144);
 *
 * Remarks:
 *   1. aging is supported only from Arad-B0.
 *   2. config_system_red_param() thresholds  adjusted to 10Gbps port.
 *   3. Dbuff, BDB thresholds are global, there for API does not use gport or cosq (need to insert valid values).
 *   4. In bcm_cosq_gport_threshold_set(), number of values (thresholds) is 4, and number of dps id 3 (4 bit Source-Q-size)
 *   5. formula for system RED drop probability:
 *     a. via bcm_cosq_gport_discard_set(), gport_size.size_max we are configuring DrpProb.
 *     b. drop probability = 1 - DrpProb / (64K - 1))
 *     c. DrpProb = 1 ==> drop-probability is not 0 but almost always drop.
 *     d. DrpProb = 64K - 1 ==> drop-probability = 1 (100%).
 *   6. Not validated for PB.
 */
 
 int config_system_red_param(int unit, int out_port_voq, int tc_high, int tc_low) {
  
    bcm_error_t rv = BCM_E_NONE ;
    int range_value[15]       ;
    int range_value_2[15]       ;
    
    int i;
    int is_dnx;
    uint32 flags = 0x0;
    uint32 core_clock_khz;
    uint32 max_ing_aging_period;
    int max_age_clocks;
    int max_age_nanosec;
    int max_age_milisec;
    
    bcm_gport_t                 gport, voq_gport, tc_high_gport, tc_low_gport;
    bcm_cos_queue_t             cosq, tc_high_cosq, tc_low_cosq;
    bcm_fabric_config_discard_t discard;
    bcm_color_t                 color;    
    bcm_cosq_gport_size_t       gport_size;
    bcm_cosq_gport_discard_t    dp_discard;
    
    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_device_member_get, $rv \n");
    }

    if (is_dnx)
    {
        /**
         * To calculate max age, take the max possible value in clocks from HW
         * and convert it to nicroseconds. The HW field is 14 bits, so the max 
         * value is 0x3FFF, or 16383. This HW value is in 16k clocks units.
         */
        core_clock_khz = *(dnxc_data_get(unit, "device", "general", "core_clock_khz", NULL));
        max_age_clocks = 16383*16*1024;
        max_age_nanosec = (max_age_clocks/core_clock_khz)*1000000;
        max_age_milisec = max_age_nanosec/1000000;
        /**
         * If the calculationg gives a larger value than the allowed maximum, set to max.
         */
        max_ing_aging_period = *(dnxc_data_get(unit, "system_red", "info", "max_ing_aging_period", NULL));  
        max_age_milisec = (max_age_milisec > max_ing_aging_period) ? max_ing_aging_period : max_age_milisec;
    }
    else
    {
        max_age_milisec = 268;
    }

    /* enable system RED */
    discard.flags = 0x0;
    discard.enable = 0x1;
    discard.age = max_age_milisec;
    discard.age_mode = bcmFabricAgeModeReset;  /* bcmFabricAgeModeDecrement; */
    rv = bcm_fabric_config_discard_set(unit, discard);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_fabric_config_discard_set(), rv=%d.\n", rv);
        return rv;
    }
    
    /* Ingress configurations */
    
    /* 1. set Q size levels */
    range_value[14] = 14000;
    range_value[13] = 13000;
    range_value[12] = 12000;
    range_value[11] = 11000;
    range_value[10] = 10000;
    range_value[9 ] = 9000 ;
    range_value[8 ] = 8000 ;
    range_value[7 ] = 7000 ;
    range_value[6 ] = 6000 ;
    range_value[5 ] = 5000 ;
    range_value[4 ] = 4000 ;
    range_value[3 ] = 3000 ;
    range_value[2 ] = 2000 ;
    range_value[1 ] = 1000 ;
    range_value[0 ] = 0000 ;

    flags = BCM_COSQ_GPORT_SIZE_COLOR_SYSTEM_RED;
    
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, out_port_voq);

    if (is_dnx)
    {
        bcm_switch_profile_mapping_t profile_mapping;

        /* get rate class for VOQ with cosq=tc_high */
        cosq = tc_high;
        profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
        bcm_cosq_profile_mapping_get(unit, voq_gport, cosq, 0, &profile_mapping);
        tc_high_gport = profile_mapping.mapped_profile;

        /* get rate class for VOQ with cosq=tc_low */
        cosq = tc_low;
        profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
        bcm_cosq_profile_mapping_get(unit, voq_gport, cosq, 0, &profile_mapping);
        tc_low_gport = profile_mapping.mapped_profile;

        tc_high_cosq = tc_low_cosq = 0;
    }
    else
    {
        tc_high_gport = tc_low_gport = voq_gport;
        tc_high_cosq = tc_high;
        tc_low_cosq = tc_low;
    }

    /* tc_high */
    for (i=0 ; i < 15 ; i++) {
        color = i; /* 0-14 */    
        gport_size.size_max = range_value[i];
        rv = bcm_cosq_gport_color_size_set(unit, tc_high_gport, tc_high_cosq, color, flags, &gport_size);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_color_size_set(), rv=%d.\n", rv);
            return rv;
        }
    }
    
    /* tc_low */
    for (i=0 ; i < 15 ; i++) {
        color = i; /* 0-15 */    
        gport_size.size_max = range_value[i];
        rv =  bcm_cosq_gport_color_size_set(unit, tc_low_gport, tc_low_cosq, color, flags, &gport_size);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_color_size_set(), rv=%d.\n", rv);
            return rv;
        }
    }
    
    /* 2. set SRED parameters */
    dp_discard.gain = 0;
        
    /* 
     * rate_class = 1; DP = 0 --> admit always
     * rate_class = 2; DP = 0 --> admit always
     */
    dp_discard.flags = BCM_COSQ_DISCARD_SYSTEM | BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_PROBABILITY1 | BCM_COSQ_DISCARD_COLOR_GREEN;
    dp_discard.drop_probability = 0xffff; 
    dp_discard.min_thresh = 0xd; /* adm */
    dp_discard.max_thresh = 0xd; /* prb */

    /* tc_high */
    rv = bcm_cosq_gport_discard_set(unit, tc_high_gport, tc_high_cosq, &dp_discard);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_discard_set(), rv=%d.\n", rv);
        return rv;
    }

    /* tc_low */
    rv = bcm_cosq_gport_discard_set(unit, tc_low_gport, tc_low_cosq, &dp_discard);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_discard_set(), rv=%d.\n", rv);
        return rv;
    }
    
    dp_discard.flags = BCM_COSQ_DISCARD_SYSTEM | BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_PROBABILITY2 | BCM_COSQ_DISCARD_COLOR_GREEN;
    dp_discard.drop_probability = 0xff00;
    dp_discard.min_thresh = 0xd; /* prb */
    dp_discard.max_thresh = 0xe; /* drp */

    /* tc_high */
    rv = bcm_cosq_gport_discard_set(unit, tc_high_gport, tc_high_cosq, &dp_discard);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_discard_set(), rv=%d.\n", rv);
        return rv;
    }
    
    /* tc_low */
    rv = bcm_cosq_gport_discard_set(unit, tc_low_gport, tc_low_cosq, &dp_discard);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_discard_set(), rv=%d.\n", rv);
        return rv;
    }
    
    /* 
     * rate_class = 1; DP = 1 --> drop  always
     * rate_class = 2; DP = 1 --> drop  always 
     */
    dp_discard.flags = BCM_COSQ_DISCARD_SYSTEM | BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_PROBABILITY1 | BCM_COSQ_DISCARD_COLOR_YELLOW;
    dp_discard.drop_probability = 0xff;
    dp_discard.min_thresh = 0x1; /* adm */
    dp_discard.max_thresh = 0x1; /* prb */

    /* tc_high */
    rv = bcm_cosq_gport_discard_set(unit, tc_high_gport, tc_high_cosq, &dp_discard);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_discard_set(), rv=%d.\n", rv);
        return rv;
    }

    /* tc_low */
    rv = bcm_cosq_gport_discard_set(unit, tc_low_gport, tc_low_cosq, &dp_discard);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_discard_set(), rv=%d.\n", rv);
        return rv;
    }
    
    dp_discard.flags = BCM_COSQ_DISCARD_SYSTEM | BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_PROBABILITY2 | BCM_COSQ_DISCARD_COLOR_YELLOW;
    dp_discard.drop_probability = 0x1;
    dp_discard.min_thresh = 0x1; /* prb */
    dp_discard.max_thresh = 0x1; /* drp */

    /* tc_high */
    rv = bcm_cosq_gport_discard_set(unit, tc_high_gport, tc_high_cosq, &dp_discard);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_discard_set(), rv=%d.\n", rv);
        return rv;
    }

    /* tc_low */;
    rv = bcm_cosq_gport_discard_set(unit, tc_low_gport, tc_low_cosq, &dp_discard);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_discard_set(), rv=%d.\n", rv);
        return rv;
    }
      
    return rv;
};
 

int sysport_to_voq_get(int unit, int out_port_sysport, int *out_port_voq)
{
    int rv = BCM_E_NONE;
    bcm_gport_t sysport_gport, voq_gport;

    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, out_port_sysport);

    rv = bcm_cosq_sysport_ingress_queue_map_get(unit, 0, sysport_gport, &voq_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_cosq_sysport_ingress_queue_map_get(), rv=%d.\n", rv);
        return rv;
    }
    *out_port_voq = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(voq_gport);

    return rv;
}

/*
 * Example for System RED application
 */
int config_system_red_param_app(int unit, int destination) {
    
    int rv = BCM_E_NONE, is_dnx, tc_low, tc_high, out_port_sysport, out_port_voq;

    tc_low = 0;
    tc_high = 7;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_device_member_get, $rv \n");
    }
    
    if (is_dnx)
    {
        out_port_sysport = destination;
        rv = sysport_to_voq_get(unit, out_port_sysport, &out_port_voq);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sysport_to_voq_get(), rv=%d.\n", rv);
            return rv;
        }
    }
    else
    {
        out_port_voq = (destination - 1) * 8 + 32;
    }

    rv = config_system_red_param(unit, out_port_voq, tc_high, tc_low);
    if (rv != BCM_E_NONE)
    {
        printf("Error, config_system_red_param(), rv=%d.\n", rv);
        return rv;
    }
    
    return rv;
}
    
/*
 * System RED API using example - Setting resource thresholds
 */
int system_red_rsrc_thrs_example(int unit, int destination)
{
    int 
        rv = BCM_E_NONE,
        i,
        val,
        is_dnx,
        out_port_voq,
        out_port_sysport,
        is_dram_present,
        nof_calls_to_api;
    bcm_gport_t          gport;
    bcm_cos_queue_t      cosq;
    bcm_cosq_threshold_t threshold;

    int is_qax;
    bcm_info_t info;
 
    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_device_member_get, $rv \n");
    }

    rv = bcm_info_get(unit, &info);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_info_get, rv=%d\n", rv);
        return rv;
    }

    is_qax = (info.device == 0x8270 || info.device == 0x8470);
    

    /* API does not use gport or cosq */
    if (is_dnx)
    {
        out_port_sysport = destination;
        rv = sysport_to_voq_get(unit, out_port_sysport, &out_port_voq);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sysport_to_voq_get(), rv=%d.\n", rv);
            return rv;
        }
        gport = 0;
        cosq = 0;
        is_dram_present = *(dnxc_data_get(unit, "dram", "general_info", "dram_info", "dram_bitmap"));
        /* when there is no DRAM, we dont configure the DRAM BDBs threshold, so we only call the API 8 times instead of 12*/
        nof_calls_to_api = is_dram_present ? 12 : 8;
    }
    else
    {
        out_port_voq = (destination - 1) * 8 + 32;
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(gport, out_port_voq);
        cosq = 2;
        nof_calls_to_api = 12;
    }
    
    for (i=0; i < nof_calls_to_api; i++) {
    
        val = i;
        
        if (i < 4) {
            threshold.type = (is_dnx || is_qax) ? bcmCosqThresholdOcbPacketDescriptorBuffers : bcmCosqThresholdDbuffs;
        } else if (i < 8) {
            threshold.type = (is_dnx || is_qax) ? bcmCosqThresholdOcbDbuffs : bcmCosqThresholdFullDbuffs;
        } else if (i < 12) {
            threshold.type = bcmCosqThresholdBufferDescriptorBuffers;
        } 
        
        if (i % 4 == 0x0) {
            threshold.flags = BCM_COSQ_THRESHOLD_COLOR_SYSTEM_RED | BCM_COSQ_THRESHOLD_RANGE_0;
        } else if (i % 4 == 0x1) {
            threshold.flags = BCM_COSQ_THRESHOLD_COLOR_SYSTEM_RED | BCM_COSQ_THRESHOLD_RANGE_1;
        } else if (i % 4 == 0x2) {
            threshold.flags = BCM_COSQ_THRESHOLD_COLOR_SYSTEM_RED | BCM_COSQ_THRESHOLD_RANGE_2;
        } else if (i % 4 == 0x3) {
            threshold.flags = BCM_COSQ_THRESHOLD_COLOR_SYSTEM_RED | BCM_COSQ_THRESHOLD_RANGE_3;
        }

        /* Get value Before */        
        threshold.value = 0;
        threshold.dp = 0;
        rv = bcm_cosq_gport_threshold_get(unit, gport, cosq, &threshold);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_threshold_get(), rv=%d.\n", rv);
            return rv;
        }
        print threshold;
        
        threshold.value = 1 << val;
        threshold.dp = val++;
        printf("i=%x, val=0x%x, threshold.flags=0x%x, threshold.value=0x%x\n", i, val, threshold.flags, threshold.value);    
        rv = bcm_cosq_gport_threshold_set(unit, gport, cosq, &threshold);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_threshold_get(), rv=%d.\n", rv);
            return rv;
        }
        
        /* Get value After */
        threshold.value = 0;
        threshold.dp = 0;
        rv = bcm_cosq_gport_threshold_get(unit, gport, cosq, &threshold);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_threshold_get(), rv=%d.\n", rv);
            return rv;
        }
        print threshold;
    }
    
    return rv;
}


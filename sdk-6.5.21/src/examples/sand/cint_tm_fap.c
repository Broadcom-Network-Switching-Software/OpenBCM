/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: E2E Scheduling~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_tm_fap.c
 * Purpose:     Example of simple E2E scheduler configuration where resource management is by SDK
 * 
 * The example creates four HR schedulers and four voq connector groups for four ports.
 *
 * The settings include:
 *  o    Example delay tolerance configuration.
 *  o    Set module ID and enable traffic.
 *  o    Set up the association between system ports and gports.
 *  o    Create Scheduling hierarchy shown in Figure 4a. The setup is by specifying BCM_COSQ_GPORT_WITH_ID flag.
 *  o    Set bandwidth limitation of 500 Mbps on scheduler. 
 *
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   | creates 4 HR scheduler and 4 voq connector group for 4 ports  |     
 *   |                                                               |
 *   |      E2E Hierarchy is as follows for sample port:             |
 *   |                                                               |
 *   |                  system port 10 maps to                       |
 *   |                             |                                 |
 *   |     EGRESS               port (1-4)                           |
 *   |                             |                                 | 
 *   |        HR scheduler for port 1 created during soc init        |
 *   |                             |                                 |
 *   |                        HR scheduler                           |
 *   |                             |                                 |
 *   |                       SP0  SP1  SP2  SP4    no WFQ            |
 *   |     INGRESS                                                   |
 *   |                       connector group                         |
 *   |                      |    |    |    |                         |
 *   |                     cos0 cos1  cos2 cos3                      |
 *   |                 voq  8    9    10   11                        |
 *   |                                                               |
 *   |         +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+          |
 *   |         |   Figure 4a: Simple Scheduling Scheme    |          |
 *   |         |        Resource allocation by SDK        |          |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
 
int main(void)
{
    int unit = 0;
    int my_modid = 0;
    int idx, port, system_port;
    bcm_gport_t dest_gport[13];
    bcm_gport_t sys_gport[13];
    int num_cos = 8;
    int flags = 0;
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t gport_ucast_voq_connector_group[13];
    bcm_gport_t gport_ucast_queue_group[13];
    bcm_gport_t gport_ucast_scheduler[13];
    bcm_gport_t e2e_parent_gport[13];
    int ports[13];
    int voq;
    int ARAD_CARD_NUM_PORTS = 6;
    bcm_cosq_gport_connection_t connection;
    int kbits_sec_max;
    bcm_gport_t e2e_gport;
    int max_burst, max_burst_factor;
    int cosq = 0;
    bcm_cosq_delay_tolerance_t delay_tolerance = {0};
    bcm_fabric_control_t control_type;
    bcm_cosq_voq_connector_gport_t config;
    bcm_port_config_t port_config;
    bcm_pbmp_t pbmp;
    bcm_info_t info;
    int is_dnx;

    rv = bcm_info_get(unit, &info);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_info_get, rv=%d\n", rv);
        return rv;
    }

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }

    max_burst_factor = (is_dnx ? 4 : 1);

    /*Get all NIF ports*/
    rv = bcm_port_config_get(unit, &port_config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_config_get \n");
        return rv;
    }
    BCM_PBMP_ASSIGN(pbmp, port_config.nif);
    idx = 0;
    BCM_PBMP_ITER(pbmp, port) {
        ports[idx] = port;
        ++idx;
    }
    ARAD_CARD_NUM_PORTS = idx;


    /* config tuned for 1024B credit worth */
    /* Normal delay tolerance */
    delay_tolerance.credit_request_hungry_multiplier                        =   2048/8;
    delay_tolerance.credit_request_hungry_normal_to_slow_thresh             =   1536; 
    delay_tolerance.credit_request_hungry_off_to_normal_thresh              =   3072;
    delay_tolerance.credit_request_hungry_off_to_slow_thresh                =  -1024;
    delay_tolerance.credit_request_hungry_slow_to_normal_thresh             =   3072;

    delay_tolerance.credit_request_satisfied_backlog_enter_thresh           =   0;
    delay_tolerance.credit_request_satisfied_backlog_exit_thresh            =   0;
    delay_tolerance.credit_request_satisfied_backoff_enter_thresh           =  16384;
    delay_tolerance.credit_request_satisfied_backoff_exit_thresh            =  16384;

    delay_tolerance.credit_request_satisfied_empty_queue_exceed_thresh      =      1;
    delay_tolerance.credit_request_satisfied_empty_queue_max_balance_thresh =   2048;
    delay_tolerance.credit_request_satisfied_empty_queue_thresh             =      0;

    delay_tolerance.credit_request_watchdog_status_msg_gen                  =    0;
    delay_tolerance.credit_request_watchdog_delete_queue_thresh             =    150;
    delay_tolerance.flags                                                   =    0;
   

    if (!is_dnx) {
        /* Example, set normal delay tolerance on for level 14 */
        rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_14,
                                                &delay_tolerance);

        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_delay_tolerance_level_set() failed $rv\n");
            return rv;
        }
    }
    else {
        int remote_credit_size;
        uint32 *data_val = dnxc_data_get(unit, "iqs", "credit", "worth", NULL);
        remote_credit_size = *data_val;
        rv = cint_credit_request_profile_backward_compatibilty_set(unit, remote_credit_size);
        if (rv != BCM_E_NONE) {
            printf("cint_credit_request_profile_backward_compatibilty_set() failed $rv\n");
            return rv;
        }

    }


    /* Slow delay tolerance */
    delay_tolerance.credit_request_hungry_multiplier                        =   2048/8;
    delay_tolerance.credit_request_hungry_normal_to_slow_thresh             =  -3000; /* disable */
    delay_tolerance.credit_request_watchdog_status_msg_gen                  =    0;
    delay_tolerance.credit_request_watchdog_delete_queue_thresh             =    150;
    delay_tolerance.credit_request_hungry_off_to_slow_thresh                =  -2000;
    delay_tolerance.credit_request_hungry_off_to_normal_thresh              =  -2000;
    delay_tolerance.credit_request_hungry_slow_to_normal_thresh             =  -3000; /* disable */


    delay_tolerance.credit_request_satisfied_backoff_enter_thresh           =  16000;
    delay_tolerance.credit_request_satisfied_backoff_exit_thresh            =  16000;
    delay_tolerance.credit_request_satisfied_backlog_enter_thresh           =   4000;
    delay_tolerance.credit_request_satisfied_backlog_exit_thresh            =   2000;
    delay_tolerance.credit_request_satisfied_empty_queue_thresh             =   4000;
    delay_tolerance.credit_request_satisfied_empty_queue_max_balance_thresh =   5000;
    delay_tolerance.credit_request_satisfied_empty_queue_exceed_thresh      =      1;
    delay_tolerance.flags                                                   =   0;

    if (!is_dnx) {
        /* Example, set slow delay tolerance on for level 15 */
        rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_13,
                                                &delay_tolerance);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_delay_tolerance_level_set() failed $rv\n");
            return rv;
        }
    }

    if (is_dnx)
    {
        int modid_count, actual_modid_count;
        
        rv = bcm_stk_modid_count(unit, &modid_count);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_stk_modid_count\n");
            return rv;
        }
        bcm_stk_modid_config_t modid_array[modid_count];

        rv = bcm_stk_modid_config_get_all(unit, modid_count, modid_array, &actual_modid_count);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_stk_modid_config_get_all\n");
            return rv;
        }
        my_modid = modid_array[0].modid;
    }
    else {
        rv = bcm_stk_modid_get(unit, &my_modid);
        if (rv != BCM_E_NONE) {
            printf("bcm_stk_my_modid_get failed $rv\n");
            return rv;
        }
    }

    if (!is_dnx) {

        control_type = bcmFabricMulticastQueueMin;
        rv = bcm_fabric_control_set(unit,control_type, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in queue range set, $control_type $rv \n");
            return rv;
        }
        control_type = bcmFabricMulticastQueueMax;
        rv = bcm_fabric_control_set(unit,control_type, 1023);
        if (rv != BCM_E_NONE) {
            printf("Error, in queue range set, $control_type $rv \n");
            return rv;
        }
        control_type = bcmFabricQueueMin;
        rv = bcm_fabric_control_set(unit,control_type, 1024);
        if (rv != BCM_E_NONE) {
            printf("Error, in queue range set, $control_type $rv \n");
            return rv;
        }
        control_type = bcmFabricQueueMax;
        if(info.device == 0x8270) { /*QUX check*/
            rv = bcm_fabric_control_set(unit,control_type, 16*1024 - 1);
        } else { 	
            rv = bcm_fabric_control_set(unit,control_type, 32*1024 - 1);
        }
        if (rv != BCM_E_NONE) {
            printf("Error, in queue range set, $control_type $rv \n");
            return rv;
        }
    }

    for (idx=0; idx<ARAD_CARD_NUM_PORTS; idx++) {

        port = system_port = ports[idx];

        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport[idx], system_port);
        BCM_GPORT_LOCAL_SET(dest_gport[idx], port);
    
        rv = bcm_stk_sysport_gport_set(unit, sys_gport[idx], dest_gport[idx]);

        if (rv != BCM_E_NONE) {
            printf("bcm_stk_sysport_gport_set failed $rv\n");
            return rv;
        }
    
        /* create scheduler */
        flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_SINGLE_WFQ;
    
        rv = bcm_cosq_gport_add(unit, dest_gport[idx], 1, 
                    flags, &gport_ucast_scheduler[idx]);
    
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_add scheduler failed $rv\n");
            return rv;
        }
    
        /* COS 0 */
        rv = bcm_cosq_gport_sched_set(unit,
                          gport_ucast_scheduler[idx],
                          0,
                          BCM_COSQ_SP1,
                          0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }
    
        BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, port);
    
        rv = bcm_cosq_gport_attach(unit, e2e_gport, 
                       gport_ucast_scheduler[idx], 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_attach failed $rv\n");
            return rv;
        }
    
        /* create voq connector */          
        BCM_GPORT_LOCAL_SET(config.port, port);
        config.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR;
        config.numq = num_cos;
        config.remote_modid = my_modid;
        config.nof_remote_cores = is_dnx ?  2 : 1;
        rv = bcm_cosq_voq_connector_gport_add(unit, &config, &gport_ucast_voq_connector_group[idx]);
    
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_add connector failed $rv\n");
            return rv;
        }
    
        
        /* COS 0 */
        rv = bcm_cosq_gport_sched_set(unit,
                          gport_ucast_voq_connector_group[idx],
                          0,
                          BCM_COSQ_SP0,
                          0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }
        
        /* COS 1 */
        rv = bcm_cosq_gport_sched_set(unit,
                          gport_ucast_voq_connector_group[idx],
                          1,
                          BCM_COSQ_SP1,
                          0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }
    
        /* COS 2 */
        rv = bcm_cosq_gport_sched_set(unit,
                          gport_ucast_voq_connector_group[idx],
                          2,
                          BCM_COSQ_SP2,
                          0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }
        
        /* COS 3 */
        rv = bcm_cosq_gport_sched_set(unit,
                          gport_ucast_voq_connector_group[idx],
                          3,
                          BCM_COSQ_SP4,
                          0);
    
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }
    
        for (cosq=0; cosq<4; cosq++) {
            rv = bcm_cosq_gport_attach(unit, gport_ucast_scheduler[idx], 
                           gport_ucast_voq_connector_group[idx], cosq);
            if (rv != BCM_E_NONE) {
                printf("bcm_cosq_gport_attach failed $rv\n");
                return rv;
            }
        }
        
        flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
    
        rv = bcm_cosq_gport_add(unit, sys_gport[idx], num_cos, 
                    flags, &gport_ucast_queue_group[idx]);
    
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_add queue failed $rv\n");
            return rv;
        }
        printf("ucast gport(0x%08x)\n", gport_ucast_queue_group[idx]);
    
    
        /* connect voq to flow */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
        connection.remote_modid = my_modid;
        connection.voq = gport_ucast_queue_group[idx];
        connection.voq_connector = gport_ucast_voq_connector_group[idx];
        
        rv = bcm_cosq_gport_connection_set(unit, &connection);
    
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_connection_set ingress failed $rv\n");
            return rv;
        }
    
        /* connect voq to flow */
        int core;

        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
        connection.voq = gport_ucast_queue_group[idx];
        connection.voq_connector = gport_ucast_voq_connector_group[idx];

        connection.remote_modid = my_modid;
        
        rv = bcm_cosq_gport_connection_set(unit, &connection);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_connection_set egress failed $rv\n");
            return rv;
        }
        
        /*
         * Set rate on the E2E Interface
         */
        kbits_sec_max = 1000000; /* 1Gbps */
    
        BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, port);
    
        rv = bcm_fabric_port_get(unit,
                     e2e_gport,
                     0,
                     &e2e_parent_gport[idx]);
        
    
        if (rv != BCM_E_NONE) {
            printf("bcm_fabric_port_get failed $rv\n");
            return rv;
        }
    
        rv = bcm_cosq_gport_bandwidth_set(unit,
                          e2e_parent_gport[idx],
                          0,
                          0,
                          kbits_sec_max,
                          0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_set if failed $rv\n");
            return rv;
        }
        
        
        /*
         * Set rate on the E2E port
         */
        kbits_sec_max = 500000; /* 500Mbps */
        
        BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, port);
    
        rv = bcm_cosq_gport_bandwidth_set(unit,
                          e2e_gport,
                          0,
                          0,
                          kbits_sec_max,
                          0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_set failed $rv\n");
            return rv;
        }
    
    
        /* Set rate on the Scheduler */
        kbits_sec_max = 500000; /* 500 Mbps */
        /* Scheduler */
        rv = bcm_cosq_gport_bandwidth_set(unit,
                          gport_ucast_scheduler[idx],
                          0,
                          0,
                          kbits_sec_max,
                          0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set sched failed $rv\n");
            return rv;
        }
        
        max_burst = 3000 * max_burst_factor;
        
        /* Set max burst on the Scheduler */
        rv = bcm_cosq_control_set(unit,
                      gport_ucast_scheduler[idx],
                      0,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst);
        
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set max burst sched failed $rv\n");
            return rv;
        }
        
        max_burst = 3000 * max_burst_factor;
        
        for (cosq=0; cosq<4; cosq++) {
            /* Set rate on the Connector flow */
            rv = bcm_cosq_gport_bandwidth_set(unit,
                              gport_ucast_voq_connector_group[idx],
                              cosq,
                              0,
                              kbits_sec_max,
                              0);
            if (rv != BCM_E_NONE) {
                printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
                return rv;
            }
    
            /* Set maxburst on the Connector flow */
            rv = bcm_cosq_control_set(unit,
                          gport_ucast_voq_connector_group[idx],
                          cosq,
                          bcmCosqControlBandwidthBurstMax,
                          max_burst);
            
            if (rv != BCM_E_NONE) {
                printf("bcm_cosq_control_set connector maxburst failed $rv\n");
                return rv;
            }
        }
        printf("port(%d) sysport(%d) ucast gport(0x%08x)\n", port, system_port, gport_ucast_queue_group[idx]);
    }
    printf("cint_tm_petra.c completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}

main();

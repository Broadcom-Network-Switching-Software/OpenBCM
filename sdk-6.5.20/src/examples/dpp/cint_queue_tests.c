/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_queue_tests.c
 * Purpose: Examples of queue tests
 */

/* Code to test Arad VOQ attributes */
/*int CHIP_REV_JERICHO  = 4;*/
/*int CHIP_REV_QAX      = 5;*/
/*int CHIP_REV_QUX      = 6;*/


int open_test_queues(int unit, int test_q_num_in_q_bundle, int test_first_queue, int test_last_queue)
{
    int rv = BCM_E_PARAM, i; 
        
    uint32 flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID;
    bcm_gport_t port, base_q;

    bcm_port_config_t port_config;
    int cpu_port = 0;

    rv = bcm_port_config_get(unit, &port_config);
    if (rv) {
      printf("failed to get cpu port for the test\n");
      return rv;
    }
    BCM_PBMP_ITER(port_config.cpu, cpu_port) {break;}

    BCM_GPORT_LOCAL_SET(port, cpu_port); /* user port 0 for all queues */

    for (i = test_first_queue; i <= test_last_queue; i += test_q_num_in_q_bundle) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, i);
        /* The created ports are not first associated sysports, so they can not be used for traffic, and are only for testing queue properties */
        if ((rv = bcm_cosq_gport_add(unit, port, test_q_num_in_q_bundle, flags, &base_q)) != BCM_E_NONE) {
            printf ("failed to create queue bundle 0x%x\n", i);
            return rv;
        }
    }
    return rv;
}

int open_test_ingress_queue_bundle_create(int unit, 
                                          int local_core_id, 
                                          bcm_cosq_queue_attributes_t* queue_atrributes_array,
                                          int queue_atrributes_array_size,
                                          int test_q_num_in_q_bundle, 
                                          int test_first_queue, 
                                          int test_last_queue)
{
    int rv = BCM_E_PARAM, i, cosq;
        
    uint32 flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID;
    bcm_gport_t port, base_q;
    bcm_cosq_ingress_queue_bundle_gport_config_t queue_config;
    bcm_port_config_t port_config;
    int cpu_port = 0;

    rv = bcm_port_config_get(unit, &port_config);
    if (rv) {
      printf("failed to get cpu port for the test\n");
      return rv;
    }
    BCM_PBMP_ITER(port_config.cpu, cpu_port) {break;}

    BCM_GPORT_LOCAL_SET(port, cpu_port); /* user port 0 for all queues */
    queue_config.flags = flags;
    queue_config.port = port;
    queue_config.numq = test_q_num_in_q_bundle;
    for (i = test_first_queue; i <= test_last_queue; i += test_q_num_in_q_bundle) {
        /*Set base queue value*/
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, i);
        /*Set random rate class, and credit reqiest profile*/
        for (cosq = 0; cosq < test_q_num_in_q_bundle; cosq++) {
            queue_config.queue_atrributes[cosq].rate_class = queue_atrributes_array[(sal_rand() % queue_atrributes_array_size)].rate_class;
            queue_config.queue_atrributes[cosq].delay_tolerance_level = queue_atrributes_array[(sal_rand() % queue_atrributes_array_size)].delay_tolerance_level;
        }
        /* Set core rate class, and credit reqiest profile*/
        queue_config.local_core_id = (local_core_id == -1) ? (sal_rand()%2) : local_core_id;        
        /* The created ports are not first associated sysports, so they can not be used for teaffic, and are only for testing queue properties */
        if ((rv = bcm_cosq_ingress_queue_bundle_gport_add(unit, &queue_config, &base_q)) != BCM_E_NONE) {
            printf ("failed to create queue bundle 0x%x\n", i);
            return rv;
        }
    }
    return rv;
}
int get_queue_attributes(int unit, 
                         int local_core_id, 
                         int test_q_num_in_q_bundle, 
                         int test_first_queue, 
                         int test_last_queue,
                         bcm_cosq_queue_attributes_t* queue_atrributes)
{
    int PREDEFIEND_OFFSET = 128;
    int rv = BCM_E_PARAM, cosq;
    int queue_id, index;
    bcm_gport_t base_q;
    bcm_switch_profile_mapping_t profile_mapping;
    int mode, weight, sched_mode;
    for (index = 0, queue_id = test_first_queue; queue_id <= test_last_queue; queue_id++, index++) {
        cosq = (queue_id % test_q_num_in_q_bundle);
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, queue_id - cosq);
        /*get rate class profile*/
        profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
        rv = bcm_cosq_profile_mapping_get(unit, base_q, cosq, 0, &profile_mapping);
        if (rv != BCM_E_NONE) {
            return rv;
        }
        queue_atrributes[index].rate_class = BCM_GPORT_PROFILE_GET(profile_mapping.mapped_profile);
        rv = bcm_cosq_gport_sched_get(unit, base_q, cosq, &mode, &weight);
        if (rv != BCM_E_NONE) {
            printf ("bcm_cosq_gport_sched_get failed to get queue bundles %d, cosq %d, credit request profile\n", base_q, cosq);
            return rv;
        }
        queue_atrributes[index].delay_tolerance_level = mode;
    }
    return rv;   
}
int delete_test_queues(int unit, int quiet, int test_q_num_in_q_bundle, int test_first_queue, int test_last_queue)
{
    int rv, i; 
    bcm_gport_t base_q;

    for (i = test_first_queue; i <= test_last_queue; i += test_q_num_in_q_bundle) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, i);
        if ((rv = bcm_cosq_gport_delete(unit, base_q)) != BCM_E_NONE && !quiet) {
            printf ("failed to delete queue bundle 0x%x\n", i);
            return rv;
        }
    }
    return BCM_E_NONE;
}

int is_close (int32 number, int32 found) {
    uint32 n,f;

    if (number < 0) {
       n = -number;
       f  = -found;
    } else {
       n = number;
       f  = found;
    }

    if (f < n && f * 2 <= n) {
        printf ("found number %d too small compared to expected %d\n", found, number);
        return -1;
    } else if (f > n && f >= 2 * n) {
        printf ("found number %d too big compared to expected %d\n", found, number);
        return 1;
    }
    return 0;
}

int is_array_close (int32* number_a, int32* found_a, char** description_a) {
    int rv;
    for (; *description_a; ++number_a, ++found_a, ++description_a) {
        if ((rv = is_close(*number_a, *found_a))) {
            printf("failed comparing %s\n", *description_a);
            return rv;
        }
    }
    return 0;
}

int test_create_sysport_and_queue_bundle_from_port(int unit, int nof_qs_in_bundle, bcm_gport_t dest_port, int sysport, bcm_gport_t *out_q_base, bcm_gport_t *orig_sysport_port_dest) {
    bcm_gport_t sysport_gport, sysport_gport_get;
    int rv;
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sysport);
    if ((rv = bcm_stk_sysport_gport_get(unit, sysport_gport, orig_sysport_port_dest)) != BCM_E_NONE) {
        printf ("failed getting the original modport of sysport %d\n", sysport);
    } else if ((rv = bcm_stk_gport_sysport_get(unit, *orig_sysport_port_dest, &sysport_gport_get)) != BCM_E_NONE) {
        printf ("failed getting the original sysport from original modport %d\n", sysport_gport_get);
    } else if (sysport_gport_get != sysport_gport) {
        /*If the values do not match the orig_sysport_port_dest is invalid*/
        *orig_sysport_port_dest = -1;
    }

    if ((rv = bcm_stk_sysport_gport_set(unit, sysport_gport, dest_port)) != BCM_E_NONE) {
        printf ("failed creating sysport %d from destination gport 0x%x\n", sysport, dest_port);
    } else if ((rv = bcm_cosq_gport_add(unit, sysport_gport, nof_qs_in_bundle, BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, out_q_base)) != BCM_E_NONE) {
        printf ("failed creating queue bundle using sysport %d from destination gport 0x%x\n", sysport, dest_port);
    }
    return rv;
}



/*
 Test the sysport, modport and VOQs mapping.
 Define a sysport modport and VOQ gports, map sysport to modport and VOQ.
 Then, set the mapping and checking if the get returns the correct arguments.
 */
int test_sysport_to_modport_to_queue_bundle_mapping(int unit, bcm_gport_t mod_port) {
    int sysport = 3000;
    int nof_qs_in_bundle = 4;
    int qid = 760;
    int rv, i;
    int nof_qs_in_bundle_get;
    uint32 flags;
    bcm_gport_t gport_test_get, base_queues, sysport_gport, out_q_base, out_q_base_get;
    
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sysport);
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(out_q_base, qid);

    bcm_gport_t ports[2] = {sysport_gport , mod_port};
    
    if ((rv = bcm_stk_sysport_gport_set(unit, sysport_gport, mod_port)) != BCM_E_NONE) {
        printf ("failed setting the original modport of sysport %d\n", sysport);
    }
    for (i = 0; i < 2; ++i) {
        if ((rv = bcm_stk_sysport_gport_get(unit, ports[i], &gport_test_get)) != BCM_E_NONE) {
            printf ("failed getting the original modport of sysport %d\n", sysport);
        } else if (mod_port != gport_test_get) {
            printf ("problem in result from bcm_stk_sysport_gport_get set mod_port %d, got mod_port %d\n", mod_port, gport_test_get);
            rv = BCM_E_CONFIG;
        } else if ((rv = bcm_cosq_gport_add(unit, ports[i], nof_qs_in_bundle, BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID, out_q_base)) != BCM_E_NONE) {
            printf ("failed creating queue bundle %d using sysport %d", out_q_base, sysport);
        } else if ((rv = bcm_cosq_gport_get(unit, out_q_base, &gport_test_get, &nof_qs_in_bundle_get, &flags)) != BCM_E_NONE) {
            printf ("failed getting the original voq of modport %d\n", sysport);
        } else if (!(BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID)) {
            printf ("problem in result of flags from bcm_cosq_gport_get\n");
            rv = BCM_E_CONFIG;
        } else if (nof_qs_in_bundle != nof_qs_in_bundle_get) {
            printf ("problem in result of nof_qs_in_bundle from bcm_cosq_gport_get\n");
            rv = BCM_E_CONFIG;
        } else if (gport_test_get != mod_port) {
            printf ("problem in result of port from bcm_cosq_gport_get\n");
            rv = BCM_E_CONFIG;
        } else if ((rv = bcm_cosq_gport_delete(unit, out_q_base)) != BCM_E_NONE) {
            printf ("Failed to delete VOQ %d\n", out_q_base);
        } 
    }
    
    return rv;
}



int test_delete_sysport_and_queue_bundle(int unit, bcm_gport_t q_base, int sysport, bcm_gport_t orig_sysport_port_dest) {
    bcm_gport_t sysport_gport;
    int rv;
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sysport);
    if ((rv = bcm_cosq_gport_delete(unit, q_base)) != BCM_E_NONE) {
        printf ("failed deleting queue bundle gport 0x%x\n", q_base);
    } 
    if (orig_sysport_port_dest != -1) {
        /*If orig_sysport_port_dest == -1 then there is no original destination to map too.*/
        if ((rv = bcm_stk_sysport_gport_set(unit, sysport_gport, orig_sysport_port_dest)) != BCM_E_NONE) {
            printf ("failed restoring sysport %d to original modport gport 0x%x\n", sysport, orig_sysport_port_dest);
        }
    }
    return rv;
}

int compare_struct(void *struct1, void *struct2, unsigned int struct_size) {
    unsigned char *a = struct1;
    unsigned char *b = struct2;
    unsigned int i;
    for (i = 0; i < struct_size; ++i) {
        if (a[i] != b[i]) return 1;
    }
    return 0;
}



int test_voq_attributes(int unit, int chip_rev) /* chip_rev == 0 means ARAD_A0, 3 is Arad+, 4 is jericho, 5 is QAX, 6 is QUX*/
{
    int rv;
    int test_q_num_in_q_bundle = 4;
    int test_first_queue = ((chip_rev == 6) ? 8 : ((chip_rev > 4) ? 16 : 64)) * 1024 - 256;
    int test_last_queue = test_first_queue + 127;

    /* delete the test queues if they existed before */
    if ((rv = delete_test_queues(unit, 1, test_q_num_in_q_bundle, test_first_queue, test_last_queue)) != BCM_E_NONE) {
        return rv;
    }
    /* create the test queues */
    if ((rv = open_test_queues(unit, test_q_num_in_q_bundle, test_first_queue, test_last_queue)) != BCM_E_NONE) {
        return rv;
    }

    if ((rv = test_voq_attributes_range(unit, chip_rev, test_q_num_in_q_bundle, test_first_queue, test_last_queue)) != BCM_E_NONE) {
        printf ("failed to test voq attributes in ragne %d-%d\n", test_first_queue, test_last_queue);
        return BCM_E_FAIL;
    }

    /* delete the test queues */
    if ((rv = delete_test_queues(unit, 0, test_q_num_in_q_bundle, test_first_queue, test_last_queue)) != BCM_E_NONE) {
        return rv;
    }
    return BCM_E_NONE;
}


int test_voq_attributes_ingress_queue_bundle(int unit, int chip_rev) /* chip_rev == 0 means ARAD_A0, 3 is Arad+, 4 is jericho, 5 is qax, 6 is QUX */
{
    int rv;
    int test_q_num_in_q_bundle = 4;
    int test_first_queue = ((chip_rev == 6) ? 8 : ((chip_rev > 4) ? 16 : 64)) * 1024 - 256;
    int test_last_queue = test_first_queue + 127;
    bcm_cosq_queue_attributes_t queue_atrributes_array;
    int queue_atrributes_array_size = 1; 

    queue_atrributes_array.rate_class = 0;
    queue_atrributes_array.delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED;

    /* delete the test queues if they existed before */
    if ((rv = delete_test_queues(unit, 1, test_q_num_in_q_bundle, test_first_queue, test_last_queue)) != BCM_E_NONE) {
        return rv;
    }
    /* create the test queues */
    if ((rv = open_test_ingress_queue_bundle_create(unit, 
                                                    BCM_CORE_ALL, 
                                                    &queue_atrributes_array,
                                                    queue_atrributes_array_size,
                                                    test_q_num_in_q_bundle, 
                                                    test_first_queue, 
                                                    test_last_queue)) != BCM_E_NONE) {
        return rv;
    }

    if ((rv = test_voq_attributes_range(unit, chip_rev, test_q_num_in_q_bundle, test_first_queue, test_last_queue)) != BCM_E_NONE) {
        printf ("failed to test voq attributes in ragne %d-%d\n", test_first_queue, test_last_queue);
        return BCM_E_FAIL;
    }

    /* delete the test queues */
    if ((rv = delete_test_queues(unit, 0, test_q_num_in_q_bundle, test_first_queue, test_last_queue)) != BCM_E_NONE) {
        return rv;
    }
    return BCM_E_NONE;
}

int test_voq_attributes_ingress_queue_bundle_2(int unit, int chip_rev) /* chip_rev == 0 means ARAD_A0, 3 is Arad+, 4 is jericho*/
{
    int rv;
    int test_q_num_in_q_bundle = 4;
    int test_first_queue = ((chip_rev == 6) ? 8 : ((chip_rev > 4) ? 16 : 64)) * 1024 - 256;
    int test_first_queue_end = test_first_queue + 7;
    int test_last_queue_start = test_first_queue_end + 1;
    int test_last_queue = test_last_queue_start + 119;
    bcm_cosq_queue_attributes_t queue_atrributes_array;
    bcm_cosq_queue_attributes_t queue_atrributes_array_get[128];
    int queue_atrributes_array_size = 1; 

    queue_atrributes_array.rate_class = 0;
    queue_atrributes_array.delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED;

    /* delete the test queues if they existed before */
    if ((rv = delete_test_queues(unit, 1, test_q_num_in_q_bundle, test_first_queue, test_last_queue)) != BCM_E_NONE) {
        return rv;
    }
    /* create the first test queues */
    if ((rv = open_test_ingress_queue_bundle_create(unit, 
                                                    BCM_CORE_ALL, 
                                                    &queue_atrributes_array,
                                                    queue_atrributes_array_size,
                                                    test_q_num_in_q_bundle, 
                                                    test_first_queue, 
                                                    test_first_queue_end)) != BCM_E_NONE) {
        return rv;
    }

    if ((rv = test_voq_attributes_range(unit, chip_rev, test_q_num_in_q_bundle, test_first_queue, test_first_queue_end)) != BCM_E_NONE) {
        printf ("failed to test voq attributes in ragne %d-%d\n", test_first_queue, test_last_queue);
        return BCM_E_FAIL;
    }
    /* Get configured queue attributes*/
    rv = get_queue_attributes(
            unit, 
            BCM_CORE_ALL, 
            test_q_num_in_q_bundle, 
            test_first_queue,
            test_first_queue_end,
            queue_atrributes_array_get);
    if (rv != BCM_E_NONE) {
        printf ("failed");
        return BCM_E_FAIL;
    }
    /* create the other test queues with the new attributes*/
    if ((rv = open_test_ingress_queue_bundle_create(
                unit, 
                BCM_CORE_ALL, 
                queue_atrributes_array_get,
                (test_first_queue_end - test_first_queue),
                test_q_num_in_q_bundle, 
                test_last_queue_start, 
                test_last_queue)) != BCM_E_NONE) {
        return rv;
    }
    /*Test the other queues*/
    if ((rv = test_voq_attributes_range(unit, chip_rev, test_q_num_in_q_bundle, test_last_queue_start, test_last_queue)) != BCM_E_NONE) {
        printf ("failed to test voq attributes in ragne %d-%d\n", test_first_queue, test_last_queue);
        return BCM_E_FAIL;
    }

    printf ("delete_test_queues");
    /* delete the test queues */
    if ((rv = delete_test_queues(unit, 0, test_q_num_in_q_bundle, test_first_queue, test_last_queue)) != BCM_E_NONE) {
        return rv;
    }
    return BCM_E_NONE;
}

/*
 Test the VOQ attributes.
 Works on queues it creates, using set and get operations.
 Since most of the attributes use a limited number of hardware profiles the test tries not to consume many profiles.
 The testing is done one queue per time, and usually returning it to its original configuration.
 */
int test_voq_attributes_range(int unit, int chip_rev, int test_q_num_in_q_bundle, int test_first_queue, int test_last_queue) /* chip_rev == 0 means ARAD_A0, 3 is Arad+ */
{
    int rv, i, j;
    bcm_cosq_delay_tolerance_t dt1 = {0}, dt2 = {0};
    bcm_gport_t base_q, gport;
    bcm_cos_queue_t cosq;

    bcm_port_config_t port_config;
    int a_port, a_tm_port;
    if ((rv = bcm_port_config_get(unit, &port_config)) != BCM_E_NONE) {
        printf("failed to get a port for the test\n");
        return rv;
    }
    BCM_PBMP_ITER(port_config.port, a_port) {break;}
    /* get tm_port from port */
    if ((rv = bcm_port_gport_get(unit, a_port, &gport)) != BCM_E_NONE) {
        printf("failed to get a gport from port\n");
        return rv;
    }
    a_tm_port = BCM_GPORT_MODPORT_PORT_GET(gport);

    /* 1. Configure Credit Request profile */
    dt1.credit_request_watchdog_status_msg_gen = (chip_rev >= 3) ? 0 : 125;
    dt1.credit_request_watchdog_delete_queue_thresh = 160;
    dt1.credit_request_hungry_off_to_slow_thresh = -2700;
    dt1.credit_request_hungry_off_to_normal_thresh = -2000;
    dt1.credit_request_hungry_slow_to_normal_thresh = 20000;
    dt1.credit_request_hungry_normal_to_slow_thresh = 10000;
    dt1.credit_request_hungry_multiplier = 4000;
    dt1.credit_request_satisfied_backoff_enter_thresh = 17000;
    dt1.credit_request_satisfied_backoff_exit_thresh = 16000;
    dt1.credit_request_satisfied_backlog_enter_thresh = 4099;
    dt1.credit_request_satisfied_backlog_exit_thresh = 2040;
    dt1.credit_request_satisfied_empty_queue_thresh = 2000;
    dt1.credit_request_satisfied_empty_queue_max_balance_thresh = 4000;
    dt1.credit_request_satisfied_empty_queue_exceed_thresh = 1;
    dt1.flags = 0;
    if ((rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_14, &dt1)) != BCM_E_NONE) {
        printf ("failed to set delay tolerance 14\n");
        return BCM_E_FAIL;
    }
    if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_14, &dt2)) != BCM_E_NONE) {
        printf ("failed to get delay tolerance 14\n");
        return BCM_E_FAIL;
    }

    int32 exp_a[15] = {
    dt1.credit_request_watchdog_status_msg_gen,
    dt1.credit_request_watchdog_delete_queue_thresh,
    dt1.credit_request_hungry_off_to_slow_thresh,
    dt1.credit_request_hungry_off_to_normal_thresh,
    dt1.credit_request_hungry_slow_to_normal_thresh,
    dt1.credit_request_hungry_normal_to_slow_thresh,
    dt1.credit_request_hungry_multiplier,
    dt1.credit_request_satisfied_backoff_enter_thresh,
    dt1.credit_request_satisfied_backoff_exit_thresh,
    dt1.credit_request_satisfied_backlog_enter_thresh,
    dt1.credit_request_satisfied_backlog_exit_thresh,
    dt1.credit_request_satisfied_empty_queue_thresh,
    dt1.credit_request_satisfied_empty_queue_max_balance_thresh,
    dt1.credit_request_satisfied_empty_queue_exceed_thresh,
    0 };
    int32 found_a[15] = {
    dt2.credit_request_watchdog_status_msg_gen,
    dt2.credit_request_watchdog_delete_queue_thresh,
    dt2.credit_request_hungry_off_to_slow_thresh,
    dt2.credit_request_hungry_off_to_normal_thresh,
    dt2.credit_request_hungry_slow_to_normal_thresh,
    dt2.credit_request_hungry_normal_to_slow_thresh,
    dt2.credit_request_hungry_multiplier,
    dt2.credit_request_satisfied_backoff_enter_thresh,
    dt2.credit_request_satisfied_backoff_exit_thresh,
    dt2.credit_request_satisfied_backlog_enter_thresh,
    dt2.credit_request_satisfied_backlog_exit_thresh,
    dt2.credit_request_satisfied_empty_queue_thresh,
    dt2.credit_request_satisfied_empty_queue_max_balance_thresh,
    dt2.credit_request_satisfied_empty_queue_exceed_thresh,
    0 };
    char* desc_a[15] = {
    "credit_request_watchdog_status_msg_gen",
    "credit_request_watchdog_delete_queue_thresh",
    "credit_request_hungry_off_to_slow_thresh",
    "credit_request_hungry_off_to_normal_thresh",
    "credit_request_hungry_slow_to_normal_thresh",
    "credit_request_hungry_normal_to_slow_thresh",
    "credit_request_hungry_multiplier",
    "credit_request_satisfied_backoff_enter_thresh",
    "credit_request_satisfied_backoff_exit_thresh",
    "credit_request_satisfied_backlog_enter_thresh",
    "credit_request_satisfied_backlog_exit_thresh",
    "credit_request_satisfied_empty_queue_thresh",
    "credit_request_satisfied_empty_queue_max_balance_thresh",
    "credit_request_satisfied_empty_queue_exceed_thresh",
    NULL };

    if (is_array_close (exp_a, found_a, desc_a)) {
        printf("found significant difference in delay tolerance retrieved after set\n");
        return BCM_E_FAIL;
    }

    printf("1. Associate Credit Request profiles with queues \n");

    int mode, weight;
    int cr_templates[3] = {BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED, BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY, BCM_COSQ_DELAY_TOLERANCE_15};
    for (i = test_first_queue, j = 0; i <= test_last_queue; i += test_q_num_in_q_bundle) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, i);
        for (cosq = 0; cosq < test_q_num_in_q_bundle; ++cosq) {
            j = (j + 1) % 3;
            if ((rv = bcm_cosq_gport_sched_get(unit, base_q, cosq, &mode, &weight)) != BCM_E_NONE) {
                printf ("failed to get default credit rate template queue 0x%x+%d\n", i, cosq);
                return rv;
            } else if (mode != BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED) {
                printf("queue base: 0x%x, cosq: %d was created with non default credit request (tolerance level) profile\n", i, cosq);
                return BCM_E_FAIL;
            } else if ((rv = bcm_cosq_gport_sched_set(unit, base_q, cosq, cr_templates[j], 0)) != BCM_E_NONE) {
                printf ("failed to set credit rate template queue 0x%x+%d\n", i, cosq);
                return rv;
            } else if ((rv = bcm_cosq_gport_sched_get(unit, base_q, cosq, &mode, &weight)) != BCM_E_NONE) {
                printf ("failed to get credit rate template queue 0x%x+%d\n", i, cosq);
                return rv;
            } else if (mode != cr_templates[j]) {
                printf("queue base: 0x%x, cosq: %d retrieved credit request (tolerance level) profile 0x%x is not the one that was set 0x%x\n", i, cosq, mode, cr_templates[j]);
                return BCM_E_FAIL;
            } else if ((rv = bcm_cosq_gport_sched_set(unit, base_q, cosq, BCM_COSQ_DELAY_TOLERANCE_NORMAL, 0)) != BCM_E_NONE) {
                printf ("failed to set default credit rate template queue 0x%x+%d\n", i, cosq);
                return rv;
            }
        }
    }


    printf("2. Configure max and guaranteed queue size per DP.\n");

    bcm_color_t color;
    bcm_color_t colors[4] = {bcmColorGreen, bcmColorYellow, bcmColorRed, bcmColorBlack};
    bcm_cosq_gport_size_t q_size, orig_size_bytes, orig_size_bds, q_size_bytes = {0, 20000}, exact_q_size_bytes, q_size_bds = {0, 25}, exact_q_size_bds;
    bcm_cosq_gport_size_t orig_size_words, orig_size_sram_words, orig_size_sram_pds;
    bcm_cosq_gport_size_t q_size_words = {100, 20000, 1, 10000}, exact_q_size_words, q_size_sram_words = {50, 10000, -1, 5000}, exact_q_size_sram_words, q_size_sram_pds = {50, 10000, 3, 5000}, exact_q_size_sram_pds;
    for (i = (((test_first_queue + 3 *test_last_queue + 3)/4) / test_q_num_in_q_bundle) * test_q_num_in_q_bundle, /* use a quarter of the queues to make the test faster */
         j = 0; i <= test_last_queue; i += test_q_num_in_q_bundle) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, i);
        if (chip_rev >= 5) {
            exp_a[0] = q_size_words.size_min; exp_a[1] = q_size_words.size_max; exp_a[2] = q_size_words.size_alpha_max; exp_a[3] = q_size_words.size_fadt_min;
            desc_a[0] = "guaranteed size in Words"; desc_a[1] = "FADT maximal threshold in Words"; desc_a[2] = "FADT alpha"; desc_a[3] = "FADT minimal threshold in Words"; desc_a[4] = NULL;
            exp_a[5] = q_size_sram_words.size_min; exp_a[6] = q_size_sram_words.size_max; exp_a[7] = q_size_sram_words.size_alpha_max; exp_a[8] = q_size_sram_words.size_fadt_min;
            desc_a[5] = "guaranteed size in SRAM-Words"; desc_a[6] = "FADT maximal threshold in SRAM-Words"; desc_a[7] = "FADT alpha"; desc_a[8] = "FADT minimal threshold in SRAM-Words"; desc_a[9] = NULL;
            exp_a[10] = q_size_sram_pds.size_min; exp_a[11] = q_size_sram_pds.size_max; exp_a[12] = q_size_sram_pds.size_alpha_max; exp_a[13] = q_size_sram_pds.size_fadt_min;
            desc_a[10] = "guaranteed size in SRAM-PDs"; desc_a[11] = "FADT maximal threshold in SRAM-PDs"; desc_a[12] = "FADT alpha"; desc_a[13] = "FADT minimal threshold in SRAM-PDs"; desc_a[14] = NULL;

            for (cosq = 0; cosq < test_q_num_in_q_bundle; ++cosq) {
                for (j = 0; j < 4; ++j) {
                    color = colors[j];  /* j == colors[j] , is the DP */
                    /* Get original values */
                    if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &orig_size_words)) != BCM_E_NONE) {
                        printf ("failed to get original queue size in Words 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BYTES, &orig_size_sram_words)) != BCM_E_NONE) {
                        printf ("failed to get original queue size in SRAM-Words 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC, &orig_size_sram_pds)) != BCM_E_NONE) {
                        printf ("failed to get original queue size in SRAM-PDs 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    } 
                    
                    /* Test Words */
                    if ((rv = bcm_cosq_gport_color_size_set(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &q_size_words)) != BCM_E_NONE) {
                        printf ("failed to set queue size in words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_words)) != BCM_E_NONE) {
                        printf ("failed to get queue size in words 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    }
                    found_a[0] = exact_q_size_words.size_min; found_a[1] = exact_q_size_words.size_max; found_a[2] = exact_q_size_words.size_alpha_max; found_a[3] = exact_q_size_words.size_fadt_min;
                    if (is_array_close (exp_a, found_a, desc_a)) {
                        printf ("found wrong queue size data at 0x%x+%d,%d\n", i, cosq, color);
                        print exact_q_size_words;
                        return BCM_E_FAIL;
                    } 
                    
                    /* Test SRAM-Words */
                    if ((rv = bcm_cosq_gport_color_size_set(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BYTES, &q_size_sram_words)) != BCM_E_NONE) {
                        printf ("failed to set queue size in sram-words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_sram_words)) != BCM_E_NONE) {
                        printf ("failed to get queue size in sram-words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    }
                    found_a[0] = exact_q_size_sram_words.size_min; found_a[1] = exact_q_size_sram_words.size_max; found_a[2] = exact_q_size_sram_words.size_alpha_max; found_a[3] = exact_q_size_sram_words.size_fadt_min;
                    if (is_array_close (exp_a + 5, found_a, desc_a + 5)) {
                        printf ("found wrong queue size data in sram-words at 0x%x+%d,%d\n", i, cosq, j);
                        print exact_q_size_sram_words;
                        return BCM_E_FAIL;
                    } 
                    
                    /* Test SRAM-PDs */
                    if ((rv = bcm_cosq_gport_color_size_set(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC, &q_size_sram_pds)) != BCM_E_NONE) {
                        printf ("failed to set queue size in sram-pds 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC, &exact_q_size_sram_pds)) != BCM_E_NONE) {
                        printf ("failed to get queue size in sram-pds 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    }
                    found_a[0] = exact_q_size_sram_pds.size_min; found_a[1] = exact_q_size_sram_pds.size_max; found_a[2] = exact_q_size_sram_pds.size_alpha_max; found_a[3] = exact_q_size_sram_pds.size_fadt_min;
                    if (is_array_close (exp_a + 10, found_a, desc_a + 10)) {
                        printf ("found wrong queue size data in sram-pds at 0x%x+%d,%d\n", i, cosq, j);
                        print exact_q_size_sram_pds;
                        return BCM_E_FAIL;
                    } 
                    
                    /* Set original values */
                    if ((rv = bcm_cosq_gport_color_size_set(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &orig_size_words)) != BCM_E_NONE) {
                        printf ("failed to set original queue size in words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_set(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BYTES, &orig_size_sram_words)) != BCM_E_NONE) {
                        printf ("failed to set original queue size in sram-words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_set(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC, &orig_size_sram_pds)) != BCM_E_NONE) {
                        printf ("failed to set original queue size in sram-pds 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } 

                    if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_words)) != BCM_E_NONE) {
                        printf ("failed to get (again) original queue size in words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } 
                    if (orig_size_words.size_min != exact_q_size_words.size_min || orig_size_words.size_max != exact_q_size_words.size_max || 
                               orig_size_words.size_alpha_max != exact_q_size_words.size_alpha_max || orig_size_words.size_fadt_min != exact_q_size_words.size_fadt_min) {
                        printf ("found wrong original queue size data in words at 0x%x+%d,%d\n", i, cosq, j);
                        return BCM_E_FAIL;
                    } 

                    if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_sram_words)) != BCM_E_NONE) {
                        printf ("failed to get (again) original queue size in sram-words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } 
                    if (orig_size_sram_words.size_min != exact_q_size_sram_words.size_min || orig_size_sram_words.size_max != exact_q_size_sram_words.size_max || 
                               orig_size_sram_words.size_alpha_max != exact_q_size_sram_words.size_alpha_max || orig_size_sram_words.size_fadt_min != exact_q_size_sram_words.size_fadt_min) {
                        printf ("found wrong original queue size data in sram-words at 0x%x+%d,%d\n", i, cosq, j);
                        return BCM_E_FAIL;
                    } 

                    if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC, &exact_q_size_sram_pds)) != BCM_E_NONE) {
                        printf ("failed to get (again) original queue size in sram-pds 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } 
                    if (orig_size_sram_pds.size_min != exact_q_size_sram_pds.size_min || orig_size_sram_pds.size_max != exact_q_size_sram_pds.size_max || 
                               orig_size_sram_pds.size_alpha_max != exact_q_size_sram_pds.size_alpha_max || orig_size_sram_pds.size_fadt_min != exact_q_size_sram_pds.size_fadt_min) {
                        printf ("found wrong original queue size data in sram-pds at 0x%x+%d,%d\n", i, cosq, j);
                        return BCM_E_FAIL;
                    } 
                }
            }
        } else {
            exp_a[0] = q_size_bytes.size_min; exp_a[1] = q_size_bytes.size_max;
            desc_a[0] = "guaranteed size in bytes"; desc_a[1] = "maximal size in bytes"; desc_a[2] = NULL;
            exp_a[3] = q_size_bds.size_min; exp_a[4] = q_size_bds.size_max;
            desc_a[3] = "guaranteed size in BDs"; desc_a[4] = "maximal size in BDs"; desc_a[5] = NULL;

            for (cosq = 0; cosq < test_q_num_in_q_bundle; ++cosq) {
                for (j = 0; j < 4; ++j) {
                    color = colors[j];  /* j == colors[j] , is the DP */
                    if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &orig_size_bytes)) != BCM_E_NONE) {
                        printf ("failed to get original queue size in bytes 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BUFFER_DESC, &orig_size_bds)) != BCM_E_NONE) {
                        printf ("failed to get original queue size in buffer descriptors 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_set(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &q_size_bytes)) != BCM_E_NONE) {
                        printf ("failed to set queue size in bytes 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_bytes)) != BCM_E_NONE) {
                        printf ("failed to get queue size in bytes 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    }
                    found_a[0] = exact_q_size_bytes.size_min; found_a[1] = exact_q_size_bytes.size_max;
                    if (is_array_close (exp_a, found_a, desc_a)) {
                        printf ("found wrong queue size data at 0x%x+%d,%d\n", i, cosq, color);
                        return BCM_E_FAIL;
                    } else if ((rv = bcm_cosq_gport_color_size_set(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BUFFER_DESC, &q_size_bds)) != BCM_E_NONE) {
                        printf ("failed to set queue size in buffer descriptors 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BUFFER_DESC, &exact_q_size_bds)) != BCM_E_NONE) {
                        printf ("failed to get queue size in buffer descriptors 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    }
                    found_a[0] = exact_q_size_bds.size_min; found_a[1] = exact_q_size_bds.size_max;
                    if (is_array_close (exp_a + 3, found_a, desc_a + 3)) {
                        printf ("found wrong queue size data in BDs at 0x%x+%d,%d\n", i, cosq, j);
                        print exact_q_size_bds;
                        return BCM_E_FAIL;
                    } else if ((rv = bcm_cosq_gport_color_size_set(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &orig_size_bytes)) != BCM_E_NONE) {
                        printf ("failed to set original queue size in bytes 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_set(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BUFFER_DESC, &orig_size_bds)) != BCM_E_NONE) {
                        printf ("failed to set original queue size in buffer descriptors 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_bytes)) != BCM_E_NONE) {
                        printf ("failed to get (again) original queue size in bytes 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if (orig_size_bytes.size_min != exact_q_size_bytes.size_min || orig_size_bytes.size_max != exact_q_size_bytes.size_max) {
                        printf ("found wrong original queue size data in bytes at 0x%x+%d,%d\n", i, cosq, j);
                        return BCM_E_FAIL;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BUFFER_DESC, &exact_q_size_bds)) != BCM_E_NONE) {
                        printf ("failed to get (again) original queue size in buffer descriptors 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if (orig_size_bds.size_min != exact_q_size_bds.size_min || orig_size_bds.size_max != exact_q_size_bds.size_max) {
                        printf ("found wrong original queue size data in BDs at 0x%x+%d,%d\n", i, cosq, j);
                        return BCM_E_FAIL;
                    }
                }
            }
        }
    }

    printf("3. Configure WRED discard profiles.\n");

    bcm_cosq_gport_discard_t wred, exact_wred, orig_wred;
    uint32 color_flags[4] = {BCM_COSQ_DISCARD_COLOR_GREEN, BCM_COSQ_DISCARD_COLOR_YELLOW, BCM_COSQ_DISCARD_COLOR_RED, BCM_COSQ_DISCARD_COLOR_BLACK};
    wred.min_thresh = 16;
    wred.max_thresh = 100;
    wred.drop_probability = 100;
    wred.gain = 1;
    wred.ecn_thresh = 0;
    for (i = (((test_first_queue + 3 *test_last_queue + 3)/4) / test_q_num_in_q_bundle) * test_q_num_in_q_bundle, /* use a quarter of the queues to make the test faster */
         j = 0; i <= test_last_queue; i += test_q_num_in_q_bundle) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, i);
        for (cosq = 0; cosq < test_q_num_in_q_bundle; ++cosq) {
            for (j = 0; j < 4; ++j) {
                wred.flags = exact_wred.flags = orig_wred.flags = BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_BYTES | color_flags[j];
                if ((rv = bcm_cosq_gport_discard_get(unit, base_q, cosq, &orig_wred)) != BCM_E_NONE) {
                    printf ("failed getting original WRED parameters at 0x%x+%d,%d\n", i, cosq, j);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_set(unit, base_q, cosq, &wred)) != BCM_E_NONE) {
                    printf ("failed setting WRED parameters at 0x%x+%d,%d\n", i, cosq, j);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_get(unit, base_q, cosq, &exact_wred)) != BCM_E_NONE) {
                    printf ("failed getting WRED parameters at 0x%x+%d,%d\n", i, cosq, j);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_set(unit, base_q, cosq, &orig_wred)) != BCM_E_NONE) {
                    printf ("failed setting original WRED parameters at 0x%x+%d,%d\n", i, cosq, j);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_get(unit, base_q, cosq, &exact_wred)) != BCM_E_NONE) {
                    printf ("failed re-reading the original WRED parameters at 0x%x+%d,%d\n", i, cosq, j);
                    return rv;
                } else if (
                           orig_wred.flags != exact_wred.flags ||
                           orig_wred.min_thresh != exact_wred.min_thresh ||
                           orig_wred.max_thresh != exact_wred.max_thresh ||
                           orig_wred.drop_probability != exact_wred.drop_probability ||
                           orig_wred.gain != exact_wred.gain ||
                           orig_wred.ecn_thresh != exact_wred.ecn_thresh) {
                    printf ("The original WRED parameters at 0x%x+%d,%d were not restores properly\n", i, cosq, j);
                    print orig_wred; print exact_wred;
                    return BCM_E_FAIL;
                }
            }
        }
    }
                
    if (chip_rev < 5) {
        printf("4. Configure queues' Header Compensation.\n");
        int orig_discount;
        j = 13; /* current discount to use */
        for (i = test_first_queue, j = 0; i <= test_last_queue; i += test_q_num_in_q_bundle) {
            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, i);
            for (cosq = 0; cosq < test_q_num_in_q_bundle; ++cosq) {
                if ((rv = bcm_cosq_control_get(unit, base_q, cosq, bcmCosqControlPacketLengthAdjust, &orig_discount)) != BCM_E_NONE) {
                    printf ("failed reading the original credit discount at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ((rv = bcm_cosq_control_set(unit, base_q, cosq, bcmCosqControlPacketLengthAdjust, j)) != BCM_E_NONE) {
                    printf ("failed writing credit discount at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ((rv = bcm_cosq_control_get(unit, base_q, cosq, bcmCosqControlPacketLengthAdjust, &mode)) != BCM_E_NONE) {
                    printf ("failed reading credit discount at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ( j != mode) { 
                    printf ("The read credit discount %d at 0x%x+%d is different than the expected %d\n", mode, i, cosq, j);
                    return BCM_E_FAIL;
                } else if ((rv = bcm_cosq_control_set(unit, base_q, cosq, bcmCosqControlPacketLengthAdjust, orig_discount)) != BCM_E_NONE) {
                    printf ("failed writing original credit discount at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ((rv = bcm_cosq_control_get(unit, base_q, cosq, bcmCosqControlPacketLengthAdjust, &mode)) != BCM_E_NONE) {
                    printf ("failed re-reading the original credit discount at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ( orig_discount != mode) { 
                    printf ("The read credit discount %d at 0x%x+%d is different than the expected original %d\n", mode, i, cosq, orig_discount);
                    return BCM_E_FAIL;
                }
                j += 19;
                if (j > 127) { j -=255 ; } /* discount needs to be between -127 and 127 */
            }
        }
    } else { /*chip_rev == 5) || chip_rev == 6)*/
        
    }
    
    if (chip_rev <= 3) { 
        printf("5. Configure queues as High or Low Priority.\n"); /*Not supported in Jericho - moved to IPS credit request profile*/
        bcm_cosq_gport_priority_profile_t pri_profile, pri_profile2, orig_pri_profile;
        int pri_profile_id, pri_profile_id2;

        i = (test_first_queue / 64) * 64; /* start of queue range */ 
        j = (test_last_queue / 64) * 64 + 63; /* end of queue range */ 
        /* Get a the common profile id for the whole range */
        if ((rv = bcm_cosq_priority_get(unit, i, j, &pri_profile_id)) != BCM_E_NONE) {
            printf ("failed getting the original priority profile id for the whole range, perhaps it was not the same\n");
            return rv;
        } else if ((rv = bcm_cosq_priority_profile_get(unit, pri_profile_id, &weight, &orig_pri_profile)) != BCM_E_NONE || weight != 64) {
            printf ("failed getting the original priority profile\n");
            return rv;
        } else if ((rv = bcm_cosq_priority_profile_get(unit, pri_profile_id, &weight, &pri_profile)) != BCM_E_NONE || weight != 64) {
            printf ("failed getting the original priority profile the second time\n");
            return rv;
        }
        mode = BCM_COSQ_PRI_PROFILE_GET(pri_profile, 32); /* is queue 32 in the profile high */
        if (mode) {
            BCM_COSQ_PRI_PROFILE_CLEAR(pri_profile, 32);
        } else {
            BCM_COSQ_PRI_PROFILE_SET(pri_profile, 32);
        }
        if (BCM_COSQ_PRI_PROFILE_GET(orig_pri_profile, 32) != mode || BCM_COSQ_PRI_PROFILE_GET(pri_profile, 32) == mode) {
            printf ("bit arithmetic error\n");
            return BCM_E_FAIL;
        } else if ((rv = bcm_cosq_priority_profile_set(unit, pri_profile_id, 64, &pri_profile)) != BCM_E_NONE) {
            printf ("failed setting the original priority profile\n");
            return rv;
        } else if ((rv = bcm_cosq_priority_profile_get(unit, pri_profile_id, &weight, &pri_profile2)) != BCM_E_NONE || weight != 64) {
            printf ("failed getting the original priority profile\n");
            return rv;
        }
        for (cosq = 0; cosq < 64; ++cosq) {
            if (BCM_COSQ_PRI_PROFILE_GET(pri_profile, cosq) != BCM_COSQ_PRI_PROFILE_GET(pri_profile2, cosq)) {
                printf ("read profile2 is different at profile queue %d\n", cosq);
                return BCM_E_FAIL;
            }
        }
        if ((rv = bcm_cosq_priority_profile_set(unit, pri_profile_id, 64, &orig_pri_profile)) != BCM_E_NONE) {
            printf ("failed setting the original priority profile\n");
            return rv;
        } else if ((rv = bcm_cosq_priority_profile_get(unit, pri_profile_id, &weight, &pri_profile2)) != BCM_E_NONE || weight != 64) {
            printf ("failed getting the original priority profile\n");
            return rv;
        }
        for (cosq = 0; cosq < 64; ++cosq) {
            if (BCM_COSQ_PRI_PROFILE_GET(orig_pri_profile, cosq) != BCM_COSQ_PRI_PROFILE_GET(pri_profile2, cosq)) {
                printf ("read profile2 is different than the original at profile queue %d\n", cosq);
                return BCM_E_FAIL;
            }
        }

        pri_profile_id2 = pri_profile_id ^ 1;
        for (cosq = i; cosq < j; cosq +=64) {
            if ((rv = bcm_cosq_priority_get(unit, cosq, cosq + 63, &weight)) != BCM_E_NONE) {
                printf ("failed getting the original priority profile id for 64 queues at %d\n", cosq);
                return rv;
            } else if (weight != pri_profile_id) {
                printf ("Different profile ID than the expected one of the whole range for 64 queues at %d\n", cosq);
                return BCM_E_FAIL;
            } else if ((rv = bcm_cosq_priority_set(unit, cosq, cosq + 63, pri_profile_id2)) != BCM_E_NONE) {
                printf ("failed getting the original priority profile id for 64 queues at %d\n", cosq);
                return rv;
            } else if ((rv = bcm_cosq_priority_get(unit, cosq, cosq + 63, &weight)) != BCM_E_NONE) {
                printf ("failed getting the changed priority profile id for 64 queues at %d\n", cosq);
                return rv;
            } else if (weight != pri_profile_id2) {
                printf ("Different profile ID than the changed one  for 64 queues at %d\n", cosq);
                return BCM_E_FAIL;
            }
        }
        if ((rv = bcm_cosq_priority_get(unit, i, j, &weight)) != BCM_E_NONE) {
            printf ("failed getting the changed priority profile id for the whole range, perhaps it was not the same\n");
            return rv;
        } else if (weight != pri_profile_id2) {
            printf ("Different profile ID than the changed one of the whole range\n");
            return BCM_E_FAIL;
        } else if ((rv = bcm_cosq_priority_set(unit, i, j, pri_profile_id)) != BCM_E_NONE) { /* restore original profile ID */
            printf ("failed getting the changed priority profile id for the whole range, perhaps it was not the same\n");
            return rv;
        } else if ((rv = bcm_cosq_priority_get(unit, i, j, &weight)) != BCM_E_NONE) {
            printf ("failed getting the changed priority profile id for the whole range, perhaps it was not the same\n");
            return rv;
        } else if (weight != pri_profile_id) {
            printf ("Different profile ID than the restored one of the whole range\n");
            return BCM_E_FAIL;
        } 
    } else { /*chip_rev >= 4)*/
        
    }

    if (chip_rev < 5) {
        printf("6. Configure queues Action Signature.\n");
        int orig_discount;
        for (i = test_first_queue, j = 0; i <= test_last_queue; i += test_q_num_in_q_bundle) {
            j = (i / test_q_num_in_q_bundle) % 4; /* current signature to use */
            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, i);
            for (cosq = 0; cosq < test_q_num_in_q_bundle; ++cosq) {
                if ((rv = bcm_cosq_control_get(unit, base_q, cosq, bcmCosqControlHeaderUpdateField, &orig_discount)) != BCM_E_NONE) {
                    printf ("failed reading the original action signature at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ((rv = bcm_cosq_control_set(unit, base_q, cosq, bcmCosqControlHeaderUpdateField, j)) != BCM_E_NONE) {
                    printf ("failed writing action signature at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ((rv = bcm_cosq_control_get(unit, base_q, cosq, bcmCosqControlHeaderUpdateField, &mode)) != BCM_E_NONE) {
                    printf ("failed reading action signature at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ( j != mode) { 
                    printf ("The read action signature %d at 0x%x+%d is different than the expected %d\n", mode, i, cosq, j);
                    return BCM_E_FAIL;
                } else if ((rv = bcm_cosq_control_set(unit, base_q, cosq, bcmCosqControlHeaderUpdateField, orig_discount)) != BCM_E_NONE) {
                    printf ("failed writing original action signature at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ((rv = bcm_cosq_control_get(unit, base_q, cosq, bcmCosqControlHeaderUpdateField, &mode)) != BCM_E_NONE) {
                    printf ("failed re-reading the original action signature at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ( orig_discount != mode) { 
                    printf ("The read action signature %d at 0x%x+%d is different than the expected original %d\n", mode, i, cosq, orig_discount);
                    return BCM_E_FAIL;
                }
                j = (j + 1) % 4; /* queue action signature is between 0 and 3 */
            }
        }
    }

    if (chip_rev > 0) { /* ECN is not supported in A0 */
        printf("7. Configure ECN of QUEUES.\n");

        bcm_cosq_gport_discard_t wred2 = {0}, *cur_wred;
        wred.min_thresh = wred2.min_thresh = 32;
        wred.max_thresh = wred2.max_thresh = 240;
        wred.drop_probability = wred2.drop_probability = 81;
        wred.gain = wred2.gain = 1; /* Field not used since it can not be set for ECN */
        wred.flags = BCM_COSQ_DISCARD_MARK_CONGESTION | BCM_COSQ_DISCARD_BYTES | BCM_COSQ_DISCARD_ENABLE;
        wred.ecn_thresh = (chip_rev > 1) ? 100352 : 0x80000000;
        wred2.flags = BCM_COSQ_DISCARD_MARK_CONGESTION | BCM_COSQ_DISCARD_BUFFER_DESC | BCM_COSQ_DISCARD_ENABLE;
        wred2.ecn_thresh = 8000;
        for (i = (((test_first_queue + 3 *test_last_queue + 3)/4) / test_q_num_in_q_bundle) * test_q_num_in_q_bundle; /* use a quarter of the queues to make the test faster */
             i <= test_last_queue; i += test_q_num_in_q_bundle) {
            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, i);
            for (cosq = 0; cosq < test_q_num_in_q_bundle; ++cosq) {
                cur_wred = ((cosq & 1) && chip_rev < 5) ? &wred2 : &wred;
                orig_wred.flags = exact_wred.flags = cur_wred->flags;
                if ((rv = bcm_cosq_gport_discard_get(unit, base_q, cosq, &orig_wred)) != BCM_E_NONE) {
                    printf ("failed getting original ECN WRED parameters at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_set(unit, base_q, cosq, cur_wred)) != BCM_E_NONE) {
                    printf ("failed setting ECN WRED parameters at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_get(unit, base_q, cosq, &exact_wred)) != BCM_E_NONE) {
                    printf ("failed getting ECN WRED parameters at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if (
                           cur_wred->flags != exact_wred.flags ||
                           cur_wred->min_thresh != exact_wred.min_thresh ||
                           cur_wred->max_thresh != exact_wred.max_thresh ||
                           cur_wred->drop_probability != exact_wred.drop_probability ||
                           cur_wred->ecn_thresh != exact_wred.ecn_thresh) {
                    printf ("The ECN parameters at 0x%x+%d were not set properly\n", i, cosq);
                    print *cur_wred; print exact_wred;
                    return BCM_E_FAIL;
                } else if ((rv = bcm_cosq_gport_discard_set(unit, base_q, cosq, &orig_wred)) != BCM_E_NONE) {
                    printf ("failed setting original ECN WRED parameters at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_get(unit, base_q, cosq, &exact_wred)) != BCM_E_NONE) {
                    printf ("failed re-reading the original WRED parameters at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if (
                           orig_wred.flags != exact_wred.flags ||
                           orig_wred.min_thresh != exact_wred.min_thresh ||
                           orig_wred.max_thresh != exact_wred.max_thresh ||
                           orig_wred.drop_probability != exact_wred.drop_probability ||
                           orig_wred.ecn_thresh != exact_wred.ecn_thresh) {
                    printf ("The original ECN parameters at 0x%x+%d were not restores properly\n", i, cosq);
                    print orig_wred; print exact_wred;
                    return BCM_E_FAIL;
                }
            }
        }
    }

    if ((chip_rev >= 3) && (chip_rev != 6)) { /* check Arad+ two credit values selected per Q type (no remote credit on QUX) */
        printf("8. Arad+ two credit values.\n");

        int mod1=6;
        int mod2=16;
        int local_credit_worth, remote_credit_worth, orig_remote_credit_worth, int_var;
        int orig_profile;
        int sysport_ids[3] = {3000, 3001, 3002};
        int is_remote_credit_value[3] = {0, 1, 0};
        bcm_gport_t sysport_destinations[3], orig_sysport_destinations[3], local_q, remote_q1, remote_q2;
        bcm_gport_t *base_queues[3] = {&remote_q1, &remote_q2, &local_q};
        bcm_gport_t sysport_gports[3];
        bcm_cosq_delay_tolerance_t dt3;
        int cr_req_prof1 = BCM_COSQ_DELAY_TOLERANCE_40G_SLOW_ENABLED;
        int cr_req_prof2 = BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY;
        int expected_remote_fail = BCM_E_PARAM; /* expected failure when attempting to set a credit request profile with the wrong credit worth remoteness */

        if ((rv = bcm_fabric_control_get(unit, bcmFabricCreditSize, &local_credit_worth)) != BCM_E_NONE || !local_credit_worth) {
            printf ("failed getting local credit worth\n");
            return rv ? rv : -99;
        }
        remote_credit_worth = local_credit_worth * 2; /* using a remote credit value which is different from the local one */

        /* get & set (global to the chip) remote/local credit values */
        if ((rv = bcm_fabric_control_get(unit, bcmFabricCreditSizeRemoteDefault, &orig_remote_credit_worth)) != BCM_E_NONE) {
            printf ("failed getting original remote credit worth\n");
            return rv;
        } else if ((rv = bcm_fabric_control_set(unit, bcmFabricCreditSizeRemoteDefault, remote_credit_worth)) != BCM_E_NONE) {
            printf ("failed setting remote credit worth %d\n", remote_credit_worth);
            return rv;
        } else if ((rv = bcm_fabric_control_get(unit, bcmFabricCreditSizeRemoteDefault, &int_var)) != BCM_E_NONE) {
            printf ("failed getting remote credit worth after setting it\n");
            return rv;
        } else if (remote_credit_worth != int_var) {
            printf ("remote credit worth found after setting it is %d and not the expected %d\n", int_var, remote_credit_worth);
            return 101;
        }

        BCM_GPORT_MODPORT_SET(sysport_destinations[0], mod1, a_tm_port);
        BCM_GPORT_MODPORT_SET(sysport_destinations[1], mod2, a_tm_port);
        BCM_GPORT_LOCAL_SET(sysport_destinations[2], a_port);
        /* create queues and sysports for the destinations in sysport_destinations */
        for (i = 0; i < 3; ++i) {
            if ((rv = test_create_sysport_and_queue_bundle_from_port(unit, test_q_num_in_q_bundle, sysport_destinations[i], sysport_ids[i], base_queues[i], &orig_sysport_destinations[i])) != BCM_E_NONE) {
                return rv;
            }
        }

        /* verify and change credit request profiles used in the test */
        if ((rv = bcm_cosq_delay_tolerance_level_get(unit, cr_req_prof1, &dt1)) != BCM_E_NONE) {
            printf ("failed getting original credit request profile 1\n");
            return rv;
        } else if ((rv = bcm_cosq_delay_tolerance_level_get(unit, cr_req_prof2, &dt2)) != BCM_E_NONE) {
            printf ("failed getting original credit request profile 2\n");
            return rv;
        } else if ((dt1.flags | dt2.flags) & BCM_COSQ_DELAY_TOLERANCE_REMOTE_CREDIT_VALUE) {
            printf ("found an unexpected remote credit value setting in credit request profile %d\n",
              (dt1.flags & BCM_COSQ_DELAY_TOLERANCE_REMOTE_CREDIT_VALUE) ? 1 : 2);
            return 107;
        }
        if (chip_rev == 3) { /*In Jericho we cannot configure the credit value per credit request profile, it is configured per fap. so this configuration is not supported*/
            sal_memcpy(&dt2, &dt1, sizeof(dt2));
            dt2.flags |= BCM_COSQ_DELAY_TOLERANCE_REMOTE_CREDIT_VALUE;
            if ((rv = bcm_cosq_delay_tolerance_level_set(unit, cr_req_prof1, &dt2)) != BCM_E_NONE) {
                printf ("failed changing credit request profile 1\n");
                return rv;
            } else if ((rv = bcm_cosq_delay_tolerance_level_get(unit, cr_req_prof1, &dt3)) != BCM_E_NONE) {
                printf ("failed getting changed credit request profile 1\n");
                return rv;
            } else if (compare_struct(&dt3, &dt2, sizeof(dt3))) {
                printf ("changed credit request profile 1 does not match the set value\n");
                return 114;
            }
        }
        /* verify and set the mapping from modules to credit sizes used in the test */
        uint32 credit_size = -1;
        if ((rv = bcm_cosq_dest_credit_size_get(unit, mod1, credit_size)) != BCM_E_NONE) {
            printf ("failed getting original credit size of module %d\n", mod1);
            return rv;
        } else if (credit_size != local_credit_worth) {
            printf ("original credit size of module %d  is %d and not the expected local %d\n", mod1, credit_size, local_credit_worth);
            return 131;
        } else if ((rv = bcm_cosq_dest_credit_size_get(unit, mod2, credit_size)) != BCM_E_NONE) {
            printf ("failed getting original credit size of module %d\n", mod2);
            return rv;
        } else if (credit_size != local_credit_worth) {
            printf ("original credit size of module %d  is %d and not the expected local %d\n", mod2, credit_size, local_credit_worth);
            return 132;
        } else if ((rv = bcm_cosq_dest_credit_size_set(unit, mod2, local_credit_worth / 2)) != BCM_E_PARAM) { /* attempt to set mod2 to credit value which is not local or remote */
            printf ("attempt to set a credit value which is not remote or local returned %d and not the expected BCM_E_PARAM\n", rv);
            return 115;
        } else if ((rv = bcm_cosq_dest_credit_size_get(unit, mod2, credit_size)) != BCM_E_NONE) {
            printf ("failed getting original credit size of module after attempting to set it to an illegal value %d\n", mod2);
            return rv;
        } else if (credit_size != local_credit_worth) {
            printf ("original credit size of module %d  is %d and not the expected local %d after attempting to set it to an illegal value\n", mod2, credit_size, local_credit_worth);
        } else if ((rv = bcm_cosq_dest_credit_size_set(unit, mod2, remote_credit_worth)) != BCM_E_NONE) { /* set mod2 to the remote credit value */
            printf ("failed setting remote credit size of module %d\n", mod2);
            return rv;
        } else if ((rv = bcm_cosq_dest_credit_size_get(unit, mod2, credit_size)) != BCM_E_NONE) {
            printf ("failed getting remote credit size of module %d\n", mod2);
            return rv;
        } else if (credit_size != remote_credit_worth) {
            printf ("changed credit size of module %d  is %d and not the expected local %d\n", mod2, credit_size, remote_credit_worth);
            return 133;
        }
        if (chip_rev == 3) { /*In Jericho we cannot configure the credit value per credit request profile, it is configured per fap. so this configuration is not supported*/
            /* test setting and getting credit requests profiles assinged to queues, and restore the original */
            for (i = 0; i < 3; ++i) {
                int profile_matching_locality = is_remote_credit_value[i] ? cr_req_prof1 : cr_req_prof2;
                int profile_failing = is_remote_credit_value[i] ? cr_req_prof2 : cr_req_prof1;
                j = is_remote_credit_value[i] ? expected_remote_fail : BCM_E_NONE; /* expected return value when restoring original profile */
                for (cosq = 0; cosq < test_q_num_in_q_bundle; ++cosq) {
                    if ((rv = bcm_cosq_gport_sched_get(unit, *base_queues[i], cosq, &orig_profile, &weight)) != BCM_E_NONE) {
                        printf ("failed to get original/default credit rate template queue dest index=%d csoq=%d\n", i, cosq);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_sched_set(unit, *base_queues[i], cosq, profile_matching_locality, 0)) != BCM_E_NONE) {
                        printf ("failed to set credit rate profile %d matching locality  queue dest index=%d csoq=%d\n", profile_matching_locality, i, cosq);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_sched_get(unit, *base_queues[i], cosq, &mode, &weight)) != BCM_E_NONE) {
                        printf ("failed to get credit rate profile %d matching locality  queue dest index=%d csoq=%d\n", profile_matching_locality, i, cosq);
                        return rv;
                    } else if (mode != profile_matching_locality) {
                        printf ("found profile %d instead of %d after setting it  index=%d csoq=%d\n", mode, profile_matching_locality, i, cosq);
                        return 141;
                    } else if ((rv = bcm_cosq_gport_sched_set(unit, *base_queues[i], cosq, profile_failing, 0)) != expected_remote_fail) {
                        printf ("When setting credit rate profile %d not matching locality, return value is %d and not the expected %d  queue dest index=%d csoq=%d\n", profile_failing, rv, expected_remote_fail, i, cosq);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_sched_get(unit, *base_queues[i], cosq, &mode, &weight)) != BCM_E_NONE) {
                        printf ("Failed getting credit rate profile after attempting to set one with wrong locality  queue dest index=%d csoq=%d\n", i, cosq);
                        return rv;
                    } else if (mode != profile_matching_locality) {
                        printf ("found profile %d instead of %d after attempting to set one with wrong locality  index=%d csoq=%d\n", mode, profile_matching_locality, i, cosq);
                        return 143;
                    } else if ((rv = bcm_cosq_gport_sched_set(unit, *base_queues[i], cosq, orig_profile, 0)) != j) {
                        printf ("When restoring credit rate profile return value was %d and not the expected %d  queue dest index=%d csoq=%d\n", rv, j, i, cosq);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_sched_get(unit, *base_queues[i], cosq, &mode, &weight)) != BCM_E_NONE) {
                        printf ("Failed getting credit rate profile after attempting to restore it  queue dest index=%d csoq=%d\n", i, cosq);
                        return rv;
                    } else if (mode != (is_remote_credit_value[i] ? profile_matching_locality : orig_profile)) {
                        printf ("found wrong profile %d  after attempting to set one with wrong locality  index=%d csoq=%d\n", mode, i, cosq);
                        return 145;
                    }
                }
            }
        }

        /* perform cleanup */

        /* delete queues and sysports for the destinations in sysport_destinations */
        for (i = 0; i < 3; ++i) {
            if ((rv = test_delete_sysport_and_queue_bundle(unit, *base_queues[i], sysport_ids[i], orig_sysport_destinations[i])) != BCM_E_NONE) {
                return rv;
            }
        }

        /* restore changed change credit request profile */
        if ((rv = bcm_cosq_delay_tolerance_level_set(unit, cr_req_prof1, &dt1)) != BCM_E_NONE) {
            printf ("failed restoring credit request profile 1\n");
            return rv;
        } else if ((rv = bcm_cosq_delay_tolerance_level_get(unit, cr_req_prof1, &dt2)) != BCM_E_NONE) {
            printf ("failed getting changed credit request profile 1 after restoring it\n");
            return rv;
        } else if (compare_struct(&dt1, &dt2, sizeof(dt1))) {
            printf ("restored credit request profile 1 does not match the original value\n");
            return 114;
        }


        /* restore module mapping to local credit value */
        if ((rv = bcm_cosq_dest_credit_size_set(unit, mod2, local_credit_worth)) != BCM_E_NONE) {
            printf ("failed restoring the original credit size of module %d\n", mod2);
            return rv;
        } else if ((rv = bcm_cosq_dest_credit_size_get(unit, mod2, credit_size)) != BCM_E_NONE) {
            printf ("failed getting the restored original credit size of module %d\n", mod2);
            return rv;
        } else if (credit_size != local_credit_worth) {
            printf ("changed credit size of module %d  is %d and not the expected restored local %d\n", mod2, credit_size, local_credit_worth);
            return 133;
        }
        /* restore the original remote credit value */
        if ((rv = bcm_fabric_control_set(unit, bcmFabricCreditSizeRemoteDefault, orig_remote_credit_worth)) != BCM_E_NONE) {
            printf ("failed setting original remote credit worth %d\n", orig_remote_credit_worth);
            return rv;
        } else if ((rv = bcm_fabric_control_get(unit, bcmFabricCreditSizeRemoteDefault, &int_var)) != BCM_E_NONE) {
            printf ("failed getting remote credit worth after setting back the original value\n");
            return rv;
        } else if (orig_remote_credit_worth != int_var) {
            printf ("remote credit worth found after setting back the original value is %d and not the expected %d\n", int_var, orig_remote_credit_worth);
            return 101;
        }

    }
    return rv;
}




/* functions for credit watchdog testing */


int get_verify_credit_watchdog_config(int unit, int required_mode, int *out_first_q, int *out_last_q) {
    int rv, mode;

    if ((rv = bcm_fabric_control_get(unit, bcmFabricWatchdogQueueEnable, &mode)) != BCM_E_NONE) {
        printf ("failed to get credit watchdog mode/enabled information\n");
        return rv;
    } else if (mode != required_mode) {
        printf ("found mode %d which is different than the expected credit watchdog mode %d\n", mode, required_mode);
        return BCM_E_FAIL;
    } else if ((rv = bcm_fabric_control_get(unit, bcmFabricWatchdogQueueMin, out_first_q)) != BCM_E_NONE) {
        printf ("failed to get first queue the watchdog is configured to work on\n");
        return rv;
    } else if ((rv = bcm_fabric_control_get(unit, bcmFabricWatchdogQueueMax, out_last_q)) != BCM_E_NONE) {
        printf ("failed to get last queue the watchdog is configured to work on\n");
        return rv;
    }
    return BCM_E_NONE;
}

int verify_credit_watchdog_config(int unit, int required_mode, int first_q, int last_q, int required_common_fsm) {
    int rv, val;

    reg_val f;
    char *reg_name = "IPS_CREDIT_WATCHDOG_CONFIGURATION.IPS0";
    if ((rv = bcm_fabric_control_get(unit, bcmFabricWatchdogQueueEnable, &val)) != BCM_E_NONE) {
        printf ("failed to get credit watchdog mode/enabled information\n");
        return rv;
    } else if (val != required_mode) {
        printf ("found mode %d which is different than the expected credit watchdog mode %d\n", val, required_mode);
        return BCM_E_FAIL;
    } else if ((rv = diag_reg_field_get(unit, reg_name, "CR_WD_MIN_SCAN_CYCLE_PERIOD", f))) {
        printf ("Failed reading the credit watchdog configuration register, status=%d\n", rv);
        return BCM_E_FAIL;
    } else if ((f[0] == 0 ? 0 : 1) ^ (val == BCM_FABRIC_WATCHDOG_QUEUE_DISABLE ? 0 : 1)) {
        printf ("scan time %u inconsistent (disabled/enabled) with mode %d\n", f[0], val);
        return BCM_E_FAIL;
    } else if (val == BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE) {
        bcm_cosq_delay_tolerance_t dt;
        dt.credit_request_watchdog_status_msg_gen = required_common_fsm + 3;
        if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG, &dt))) {
            printf ("Failed getting common message time, output error: %d\n", rv);
            return BCM_E_FAIL;
        } else if (dt.credit_request_watchdog_status_msg_gen != required_common_fsm) {
            printf ("Found common message time %d, is different than the expected %d\n", dt.credit_request_watchdog_status_msg_gen, required_common_fsm);
            return BCM_E_FAIL;
        } else if ((rv = diag_reg_field_get(unit, reg_name, "CR_WD_FSM_MODE", f))) {
            printf ("Failed getting fsm mode field. Error %d\n", rv);
            return BCM_E_FAIL;
        }
        if (required_common_fsm <=0) {
            val = 0;
        } else if (required_common_fsm > 2000) {
            val = 2;
        } else {
            val = 3;
        }
        if (f[0] != val) {
            printf ("Found FSM mode %u, is different than the expected %d for message time %d\n", f[0], val, required_common_fsm);
            return BCM_E_FAIL;
        } else if ((rv = diag_reg_field_get(unit, reg_name, "CR_WD_DELETE_Q_EXP_MODE", f) || f[0] != 1)) {
            printf ("Failed exponent mode, status=%d   value=%u !=1\n", rv, f[0]);
            return BCM_E_FAIL;
        }
    } else if ((rv = bcm_fabric_control_get(unit, bcmFabricWatchdogQueueMin, &val)) != BCM_E_NONE) {
        printf ("failed to get first queue the watchdog is configured to work on\n");
        return rv;
    } else if (val != first_q) {
        printf ("found first queue %d which is different than the expected %d\n", val, first_q);
        return BCM_E_FAIL;
    } else if ((rv = bcm_fabric_control_get(unit, bcmFabricWatchdogQueueMax, &val)) != BCM_E_NONE) {
        printf ("failed to get last queue the watchdog is configured to work on\n");
        return rv;
    } else if (val != last_q) {
        printf ("found last queue %d which is different than the expected %d\n", val, last_q);
        return BCM_E_FAIL;
    }
    return BCM_E_NONE;
}

int set_credit_watchdog_queue_range(int unit, int first_q, int last_q) {
    int rv, mode;
    if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueMin, first_q)) != BCM_E_NONE) {
        printf ("failed to set first queue the watchdog is configured to work on\n");
        return rv;
    } else if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueMax, last_q)) != BCM_E_NONE) {
        printf ("failed to set last queue the watchdog is configured to work on\n");
        return rv;
    }
    return BCM_E_NONE;
}

int verify_credit_watchdog_thresholds (int unit, bcm_cosq_delay_tolerance_t *dt1, bcm_cosq_delay_tolerance_t *dt2, bcm_cosq_delay_tolerance_t *dt3,
                                       int dt1_status_msg_th, int dt1_delete_q_th,int dt2_status_msg_th, int dt2_delete_q_th,
                                       int dt3_status_msg_th, int dt3_delete_q_th, char *message) {
    int rv;
    if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_14, dt3)) != BCM_E_NONE) {
        printf ("%s: failed to get delay tolerance 15\n", message);
        return rv;
    } else if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED, dt1)) != BCM_E_NONE) {
        printf ("%s: failed to get delay tolerance normal\n", message);
        return rv;
    } else if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_LOW_DELAY, dt2)) != BCM_E_NONE) {
        printf ("%s: failed to get delay tolerance low delay\n", message);
        return rv;
    } else if (dt1->credit_request_watchdog_status_msg_gen != dt1_status_msg_th) {
        printf ("%s: status message threshold of BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED is %d and not the expected %d\n",
                message, dt1->credit_request_watchdog_status_msg_gen, dt1_status_msg_th);
        return BCM_E_FAIL;
    } else if (dt1->credit_request_watchdog_delete_queue_thresh != dt1_delete_q_th) {
        printf ("%s: delete queue threshold of BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED is %d and not the expected %d\n",
                message, dt1->credit_request_watchdog_delete_queue_thresh, dt1_delete_q_th);
        return BCM_E_FAIL;
    } else if (dt2->credit_request_watchdog_status_msg_gen != dt2_status_msg_th) {
        printf ("%s: status message threshold of BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY is %d and not the expected %d\n",
                message, dt2->credit_request_watchdog_status_msg_gen, dt2_status_msg_th);
        return BCM_E_FAIL;
    } else if (dt2->credit_request_watchdog_delete_queue_thresh != dt2_delete_q_th) {
        printf ("%s: delete queue threshold of BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY is %d and not the expected %d\n",
                message, dt2->credit_request_watchdog_delete_queue_thresh, dt2_delete_q_th);
        return BCM_E_FAIL;
    } else if (dt3->credit_request_watchdog_status_msg_gen != dt3_status_msg_th) {
        printf ("%s: status message threshold of BCM_COSQ_DELAY_TOLERANCE_14 is %d and not the expected %d\n",
                message, dt3->credit_request_watchdog_status_msg_gen, dt3_status_msg_th);
        return BCM_E_FAIL;
    } else if (dt3->credit_request_watchdog_delete_queue_thresh != dt3_delete_q_th) {
        printf ("%s: delete queue threshold of BCM_COSQ_DELAY_TOLERANCE_14 is %d and not the expected %d\n",
                message, dt3->credit_request_watchdog_delete_queue_thresh, dt3_delete_q_th);
        return BCM_E_FAIL;
    }
    return BCM_E_NONE;
}

/*
 Test the credit watchdog on pre-plus versions of Arad.
 Works on queues it creates, using set and get operations.
 */
int test_credit_watchdog_old(int unit, int is_qax, int is_qux)
{
    int rv;
    bcm_cosq_delay_tolerance_t dt1 = {0}, dt2, dt3;
    int orig_q_min, orig_q_max;
    char message[80] = {0};

    printf ("Testing the original Arad credit watchdog modes\n");
    if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED, &dt1)) != BCM_E_NONE) {
        printf ("failed to get BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED delay tolerance\n");
        return rv;
    }
    int normal_dt_status_msg_th = dt1.credit_request_watchdog_status_msg_gen;
    int normal_dt_delete_q_th = dt1.credit_request_watchdog_delete_queue_thresh;
    if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY, &dt1)) != BCM_E_NONE) {
        printf ("failed to get BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY delay tolerance\n");
        return rv;
    }
    int lowdelay_dt_status_msg_th = dt1.credit_request_watchdog_status_msg_gen;
    int lowdelay_dt_delete_q_th = dt1.credit_request_watchdog_delete_queue_thresh;

    if ((rv = get_verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL, orig_q_min, orig_q_max)) != BCM_E_NONE) {
        printf ("Initial configuration not acceptable.\ntest must run when the credit watchdog is enabled in normal mode.\nIt will test other modes and return to this mode.\n");
        return rv;
    }

    int dt_status_msg_th = 66;
    int dt_delete_q_th = 466;

    dt1.credit_request_watchdog_status_msg_gen = dt_status_msg_th;
    dt1.credit_request_watchdog_delete_queue_thresh = dt_delete_q_th;
    dt1.credit_request_hungry_off_to_slow_thresh = -2700;
    dt1.credit_request_hungry_off_to_normal_thresh = -2000;
    dt1.credit_request_hungry_slow_to_normal_thresh = 20000;
    dt1.credit_request_hungry_normal_to_slow_thresh = 10000;
    dt1.credit_request_hungry_multiplier = 4000;
    dt1.credit_request_satisfied_backoff_enter_thresh = 17000;
    dt1.credit_request_satisfied_backoff_exit_thresh = 16000;
    dt1.credit_request_satisfied_backlog_enter_thresh = 4099;
    dt1.credit_request_satisfied_backlog_exit_thresh = 2040;
    dt1.credit_request_satisfied_empty_queue_thresh = 2000;
    dt1.credit_request_satisfied_empty_queue_max_balance_thresh = 4000;
    dt1.credit_request_satisfied_empty_queue_exceed_thresh = 1;
    dt1.flags = 0;
    if ((rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_14, &dt1)) != BCM_E_NONE) {
        printf ("failed to set delay tolerance 14\n");
        return rv;
    }
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, normal_dt_status_msg_th, normal_dt_delete_q_th,
         lowdelay_dt_status_msg_th, lowdelay_dt_delete_q_th, dt_status_msg_th, dt_delete_q_th, "initial check"))) { return rv; }

    /* change Q range */
    int f=8, l= (is_qux) ? 14599 : ((is_qax) ? 29999 : 89999);
    if ((rv = set_credit_watchdog_queue_range(unit, f, l))) {
         printf ("failed setting credit watchdog range to %d - %d\n", f, l);
         return rv;
    } else if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL, f, l, 0)) != BCM_E_NONE) {
         printf ("wrong configuration after setting queue range to %d - %d\n", f, l);
         return rv;
    }
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, normal_dt_status_msg_th, normal_dt_delete_q_th,
         lowdelay_dt_status_msg_th, lowdelay_dt_delete_q_th, dt_status_msg_th, dt_delete_q_th, "check after setting queue range to 8-89999"))) { return rv; }

    /* disable watchdog */
    if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_DISABLE))) {
         printf ("failed disabling credit watchdog\n");
         return rv;
    } else if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_DISABLE, f, l, 0)) != BCM_E_NONE) {
         printf ("wrong configuration after setting queue range to %d - %d and disabling\n", f, l);
         return rv;
    }
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, normal_dt_status_msg_th, normal_dt_delete_q_th,
         lowdelay_dt_status_msg_th, lowdelay_dt_delete_q_th, dt_status_msg_th, dt_delete_q_th, "after disabling in queue range 8-89999"))) { return rv; }

    /* enable watchdog */
    if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL))) {
         printf ("failed enabling credit watchdog\n");
         return rv;
    } else if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL, f, l, 0)) != BCM_E_NONE) {
         printf ("wrong configuration after setting queue range to %d - %d and enabling\n", f, l);
         return rv;
    }
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, normal_dt_status_msg_th, normal_dt_delete_q_th,
         lowdelay_dt_status_msg_th, lowdelay_dt_delete_q_th, dt_status_msg_th, dt_delete_q_th, "after enabling in queue range 8-89999"))) { return rv; }

    /* change Q range */
    f=64; l=7999;
    if ((rv = set_credit_watchdog_queue_range(unit, f, l))) {
         printf ("failed setting credit watchdog range to %d - %d\n", f, l);
         return rv;
    } else if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL, f, l, 0)) != BCM_E_NONE) {
         printf ("wrong configuration after setting queue range to %d - %d and enabling\n", f, l);
         return rv;
    }
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, normal_dt_status_msg_th, normal_dt_delete_q_th,
         lowdelay_dt_status_msg_th, lowdelay_dt_delete_q_th, dt_status_msg_th, dt_delete_q_th, "after setting queue range to 64 - 7999"))) { return rv; }

    /* move watchdog to aggressive status messages mode, first need to disable delete thresholds */
    int profile;
    for (profile = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED; profile <= BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY; ++profile) {
        if ((rv = bcm_cosq_delay_tolerance_level_get(unit, profile, &dt1)) != BCM_E_NONE) {
            printf ("failed to get a predefined delay tolerance (credit request profile) when disabling delete thresholds\n");
            return rv;
        } else if (dt1.credit_request_watchdog_delete_queue_thresh) {
            dt1.credit_request_watchdog_delete_queue_thresh = 0;
            if ((rv = bcm_cosq_delay_tolerance_level_set(unit, profile, &dt1)) != BCM_E_NONE) {
                printf ("failed to set a predefined delay tolerance (credit request profile) when disabling delete thresholds\n");
                return rv;
            }
        }
    }
    for (profile = BCM_COSQ_DELAY_TOLERANCE_02; profile <= BCM_COSQ_DELAY_TOLERANCE_14; ++profile) {
        if ((rv = bcm_cosq_delay_tolerance_level_get(unit, profile, &dt1)) != BCM_E_NONE) {
            if (rv != BCM_E_UNAVAIL) {
                printf ("failed to get a user defined delay tolerance (credit request profile) when disabling delete thresholds\n");
                return rv;
            }
        } else if (dt1.credit_request_watchdog_delete_queue_thresh) {
            dt1.credit_request_watchdog_delete_queue_thresh = 0;
            if ((rv = bcm_cosq_delay_tolerance_level_set(unit, profile, &dt1)) != BCM_E_NONE) {
                printf ("failed to set a user defined delay tolerance (credit request profile) when disabling delete thresholds\n");
                return rv;
            }
        }
    }
    if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_FAST_STATUS_MESSAGE))) {
         printf ("failed moving watchdog to aggressive status messages mode\n");
         return rv;
    } else if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_FAST_STATUS_MESSAGE, f, l, 0)) != BCM_E_NONE) {
         printf ("wrong configuration after setting queue range to %d - %d and enabling\n", f, l);
         return rv;
    }
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, 3, 0,
         3, 0, 3, 0, "after moving to aggressive status message mode"))) { return rv; }

    if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY, &dt1)) != BCM_E_NONE) {
        printf ("failed to get BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY delay tolerance\n");
        return rv;
    }
    dt1.credit_request_watchdog_status_msg_gen = 1;
    if ((rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_LOW_DELAY, &dt1)) != BCM_E_NONE) {
        printf ("failed to set BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY delay tolerance\n");
        return rv;
    } else if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_14, &dt1)) != BCM_E_NONE) {
        printf ("failed to get BCM_COSQ_DELAY_TOLERANCE_14 delay tolerance\n");
        return rv;
    }
    dt1.credit_request_watchdog_status_msg_gen = 10;
    if ((rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_14, &dt1)) != BCM_E_NONE) {
        printf ("failed to set BCM_COSQ_DELAY_TOLERANCE_14 delay tolerance\n");
        return rv;
    }
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, 3, 0,
         1, 0, 10, 0, "after changing thresholds in aggressive status message mode"))) { return rv; }

    /* disable watchdog */
    if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_DISABLE))) {
         printf ("failed disabling credit watchdog\n");
         return rv;
    } else if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_DISABLE, f, l, 0)) != BCM_E_NONE) {
         printf ("wrong configuration after disabling watchdog with queue range to %d - %d and disabling\n", f, l);
         return rv;
    }
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, 3, 0,
         1, 0, 10, 0, "after disabling in aggressive status message mode"))) { return rv; }

    /* change Q range */
    f=160; l=1024*((is_qux)?15:((is_qax)?30:90))-1;
    if ((rv = set_credit_watchdog_queue_range(unit, f, l))) {
         printf ("failed setting credit watchdog range to %d - %d\n", f, l);
         return rv;
    } else if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_DISABLE, f, l, 0)) != BCM_E_NONE) {
         printf ("wrong configuration after setting queue range to %d - %d and enabling\n", f, l);
         return rv;
    }
    sprintf(message, "after setting queue range to %d - %d", f, l);
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, 3, 0,
         1, 0, 10, 0, message))) { return rv; }

    /* enable watchdog */
    if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_FAST_STATUS_MESSAGE))) {
         printf ("failed enabling credit watchdog\n");
         return rv;
    } else if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_FAST_STATUS_MESSAGE, f, l, 0)) != BCM_E_NONE) {
         printf ("wrong configuration after setting queue range to %d - %d and enabling\n", f, l);
         return rv;
    }
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, 3, 0,
         1, 0, 10, 0, "after re-enabling in aggressive status message mode"))) { return rv; }

    /* change Q range */
    f=16; l=((is_qux)?9:((is_qax)?20:80))*1024-1;
    if ((rv = set_credit_watchdog_queue_range(unit, f, l))) {
         printf ("failed setting credit watchdog range to %d - %d\n", f, l);
         return rv;
    } else if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_FAST_STATUS_MESSAGE, f, l, 0)) != BCM_E_NONE) {
         printf ("wrong configuration after setting queue range to %d - %d and enabling\n", f, l);
         return rv;
    }
    sprintf(message, "after setting queue range to %d - %d", f, l);
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, 3, 0,
         1, 0, 10, 0, message))) { return rv; }

    /* disable watchdog */
    if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_DISABLE))) {
         printf ("failed disabling credit watchdog\n");
         return rv;
    } else if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_DISABLE, f, l, 0)) != BCM_E_NONE) {
         printf ("wrong configuration after disabling watchdog with queue range to %d - %d and disabling\n", f, l);
         return rv;
    }
    sprintf(message, "after disabling with queue range to %d - %d", f, l);
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, 3, 0,
         1, 0, 10, 0, message))) { return rv; }


    /* enable and move watchdog to normal mode */
    if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL))) {
         printf ("failed moving watchdog to aggressive status messages mode\n");
         return rv;
    } else if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL, f, l, 0)) != BCM_E_NONE) {
         printf ("wrong configuration after returning to normal mode with queue range to %d - %d and enabling\n", f, l);
         return rv;
    }
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, 33, 0,
         33, 0, 33, 0, "after returning to normal mode"))) { return rv; }

    /* restore original configuration for the two preconfigured profiles */
    if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY, &dt1)) != BCM_E_NONE) {
        printf ("failed to get BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY delay tolerance\n");
        return rv;
    }
    dt1.credit_request_watchdog_status_msg_gen = lowdelay_dt_status_msg_th;
    dt1.credit_request_watchdog_delete_queue_thresh = lowdelay_dt_delete_q_th;
    if ((rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY, &dt1)) != BCM_E_NONE) {
        printf ("failed to set BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY delay tolerance\n");
        return rv;
    } else if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED, &dt1)) != BCM_E_NONE) {
        printf ("failed to get BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED delay tolerance\n");
        return rv;
    }
    dt1.credit_request_watchdog_status_msg_gen = normal_dt_status_msg_th ;
    dt1.credit_request_watchdog_delete_queue_thresh = normal_dt_delete_q_th ;
    if ((rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED, &dt1)) != BCM_E_NONE) {
        printf ("failed to set BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED delay tolerance\n");
        return rv;
    }

    if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL, f, l, 0)) != BCM_E_NONE) {
         printf ("wrong configuration after restoring normal & low latency profiles\n");
         return rv;
    }
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, normal_dt_status_msg_th, normal_dt_delete_q_th,
         lowdelay_dt_status_msg_th, lowdelay_dt_delete_q_th, 33, 0, "after restoring normal & low latency profiles"))) { return rv; }
   

    /* restore original Q range */
    if ((rv = set_credit_watchdog_queue_range(unit, orig_q_min, orig_q_max))) {
         printf ("failed setting credit watchdog range to %d - %d\n", f, l);
         return rv;
    } else if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL, orig_q_min, orig_q_max, 0)) != BCM_E_NONE) {
         printf ("wrong configuration after restoring queue range to %d - %d\n", orig_q_min, orig_q_max);
         return rv;
    }
    if ((rv = verify_credit_watchdog_thresholds (unit, &dt1, &dt2, &dt3, normal_dt_status_msg_th, normal_dt_delete_q_th,
         lowdelay_dt_status_msg_th, lowdelay_dt_delete_q_th, 33, 0, "after restoring queue range"))) { return rv; }
   
    return BCM_E_NONE;
}


/* get the delete thresholds of the given credit request profiles, and check that their values match the hardware */
int get_delete_ths_common_fsm(int unit, int nof_profiles, int *profiles, char **profile_names, int common_fsm_time, int *out_delete_ths)
{
    int i, rv, t, fsm_exp, exp;
    mem_val v;
    bcm_cosq_delay_tolerance_t dt;
    for (fsm_exp = 4; fsm_exp > 0 && (2000 >> fsm_exp) < common_fsm_time; --fsm_exp); /* get fsm exponent - implementation defined */

    for (i = 0; i < nof_profiles; ++i) {
        if ((rv = bcm_cosq_delay_tolerance_level_get(unit, profiles[i], &dt))) {
            printf("Failed to get delete time of profile %s, status:%s\n", profile_names[i], rv);
            return rv;
        } else if (dt.credit_request_watchdog_status_msg_gen) {
            printf("Found a non zero profile message time %d in profile %s\n", dt.credit_request_watchdog_status_msg_gen, profile_names[i]);
            return BCM_E_FAIL;
        }
        /* The rest of this function is implementation dependent */
        out_delete_ths[i] = rv = dt.credit_request_watchdog_delete_queue_thresh;
        if (rv < 0 || rv > 7680 || (rv & 1)) {
            printf("Found an illegal delete time %d in profile %s\n", rv, profile_names[i]);
            return BCM_E_FAIL;
        }
        t = rv / 2;
        if (rv) {
            for (exp = 0; exp <= 8 && (t >> exp) > 15; ++exp);
            t = t >> exp;
            if (exp > 8 || (t << exp) * 2 != rv || t > 15 || t <= 0) {
                printf("Failed to calculate fields for delete time %d in profile %s\n", rv, profile_names[i]);
                printf("t=%d exp=%d rv=%d  %d   fsm_exp=%d\n",t,exp,rv, common_fsm_time, fsm_exp);
                return BCM_E_FAIL;
            }
        } else {
            exp = -fsm_exp; /* expect exponent 0 for disabled fsm */
        }
        int index = profiles[i] - BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED;
        v[0] = -1;
        if (diag_mem_field_get(unit, "IPS_CRWDTH.IPS0", "WD_DELETE_Q_TH", index, v) || v[0] != t) {
            printf("profile %d %s delete threshold %d has mantissa %u and not the expected %d\n", index, profile_names[i], dt.credit_request_watchdog_delete_queue_thresh, v[0], t);
            return BCM_E_FAIL;
        } else if (diag_mem_field_get(unit, "IPS_CRWDTH.IPS0", "WD_STATUS_MSG_GEN_PERIOD", index, v) || v[0] != exp + fsm_exp) {
            printf("profile %d %s delete threshold %d has exponent %u and not the expected %d (base=%d + fsm=%d)\n", index, profile_names[i], dt.credit_request_watchdog_delete_queue_thresh, v[0], exp + fsm_exp, exp, fsm_exp);
            return BCM_E_FAIL;
        }
    }
    return BCM_E_NONE;
}

/* set the delete thresholds of the given credit request profiles, and check that their values match the hardware */
int set_delete_ths_common_fsm(int unit, int nof_profiles, int *profiles, char **profile_names, int common_fsm_time, int *delete_ths)
{
    int i, rv;
    bcm_cosq_delay_tolerance_t dt;

    for (i = 0; i < nof_profiles; ++i) {
        if ((rv = bcm_cosq_delay_tolerance_level_get(unit, profiles[i], &dt))) {
            printf("Failed to get the delete time of profile %s for changing it, status:%s\n", profile_names[i], rv);
            return rv;
        } else if (dt.credit_request_watchdog_status_msg_gen) {
            printf("Found a non zero profile message time %d in profile %s\n", dt.credit_request_watchdog_status_msg_gen, profile_names[i]);
            return BCM_E_FAIL;
        }
        dt.credit_request_watchdog_delete_queue_thresh = delete_ths[i];
        if ((rv = bcm_cosq_delay_tolerance_level_set(unit, profiles[i], &dt))) {
            printf("Failed to set the delete time of profile %s, status:%s\n", profile_names[i], rv);
            return rv;
        }
    }

    return get_delete_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm_time, delete_ths);
}

/* verify the credit watchdog configuration and the delete thresholds of the given credit request profiles */
int verify_credit_watchdog_and_ths_common_fsm(int unit, int nof_profiles, int *profiles, char **profile_names,
                                              int common_fsm_time, int *delete_ths, int required_mode, int q_min, int q_max)
{
    int rv;
    if ((rv = verify_credit_watchdog_config(unit, required_mode, q_min, q_max, common_fsm_time))) {
        printf("verify_credit_watchdog_and_ths_common_fsm: failed verifying watchdog config\n");
    } else if ((rv = get_delete_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm_time, delete_ths))) {
        printf("verify_credit_watchdog_and_ths_common_fsm: failed verifying credit request profiles\n");
    }
    return rv;
}

/*
 * Test the common message time credit watchdog mode on Arad+.
 * We assume we are already in the mode.
 */
int test_credit_watchdog_common_fsm(int unit, int is_arad_plus, int is_qax, int is_qux)
{
    int i, rv;
    bcm_cosq_delay_tolerance_t dt;
    int orig_q_min, orig_q_max, q_min, q_max, orig_common_fsm, common_fsm;
    int nof_profiles = 4;
    int profiles[nof_profiles] = {BCM_COSQ_DELAY_TOLERANCE_1G, BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED, BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY, BCM_COSQ_DELAY_TOLERANCE_40G_SLOW_ENABLED};
    char *profile_names[nof_profiles] = {"BCM_COSQ_DELAY_TOLERANCE_1G", "BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED", "BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY", "BCM_COSQ_DELAY_TOLERANCE_40G_SLOW_ENABLED"};
    int delete_ths1[nof_profiles] = {4, 7680, 30, 0};
    int delete_ths2[nof_profiles] = {4096, 448, 7168, 2};
    int orig_delete_ths[nof_profiles] = {0};
    int tmp_delete_ths[nof_profiles] = {0};
    int test_common_fsms[] = {4000, 2000, 4000, 125, 1000, 250, 500, -1};
    reg_val v;

    printf ("Testing the Arad+ common message time credit watchdog mode\n");
    if (!is_arad_plus) {
        printf ("Must be run on Arad +\n");
        return BCM_E_FAIL;
    }

    if ((rv = get_verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, &orig_q_min, &orig_q_max)) != BCM_E_NONE) {
        printf ("Initial configuration not acceptable.\ntest must run when the credit watchdog is enabled in the common message time mode.\nIt will test other modes and return to this mode.\n");
        return rv;
    } else if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG, &dt))) {
        printf("Failed getting original common FSM value, status=%d\n", rv);
        return rv;
    }
    orig_common_fsm = dt.credit_request_watchdog_status_msg_gen;

    if ((rv = get_delete_ths_common_fsm(unit, nof_profiles, profiles, profile_names, orig_common_fsm, orig_delete_ths))) {
        printf("failed getting current profile delete thresholds, orig common message time is %d\n", orig_common_fsm);
        return rv;
    }

    q_min = 8; q_max = (is_qux) ? (16*1024-1) : ((is_qax) ? (32*1024-1) : (95*1024-1));
    if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, orig_q_min, orig_q_max, orig_common_fsm))) {
        printf("failed verifying original watchdog config\n");
        return rv;
    } else if ((rv = set_credit_watchdog_queue_range(unit, q_min, q_max))) {
        printf("failed setting Q range to %d - %d\n", q_min, q_max);
        return rv;
    } else if ((rv = verify_credit_watchdog_config(unit, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, q_min, q_max, orig_common_fsm))) {
        printf("failed verifying watchdog config after setting Q range to %d - %d\n", q_min, q_max);
        return rv;
    } else if ((rv = set_delete_ths_common_fsm(unit, nof_profiles, profiles, profile_names, orig_common_fsm, delete_ths1))) {
        printf("failed to initially configure profile delete thresholds\n");
        return rv;
    } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, orig_common_fsm, delete_ths1,
                BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, q_min, q_max))) {
        printf("failed to initially verify profile delete thresholds\n");
        return rv;
    }

    /* loop with each iteration using a different common message time */
    for (i = 0; ; ++i) {
        if (test_common_fsms[i] < 0) {
            break;
        }
        common_fsm = test_common_fsms[i];

        /* set credit watchdog common message time */
        dt.credit_request_watchdog_status_msg_gen = common_fsm;
        if ((rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG, &dt))) {
            printf("Failed setting common FSM value %d, status=%d\n", common_fsm, rv);
            return rv;
        } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths1,
                BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, q_min, q_max))) {
            printf("failed verifying watchdog config at iteration %d\n", i);
            return rv;
        }

        /* set credit watchdog queue range */
        q_min = 1024; q_max = 1047 + i *1000;
        if ((rv = set_credit_watchdog_queue_range(unit, q_min, q_max))) {
            printf("failed setting Q range to %d - %d\n", q_min, q_max);
            return rv;
        } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths1,
                BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, q_min, q_max))) {
            printf("failed verifying watchdog config after setting Q range to %d - %d at iteration %d\n", q_min, q_max, i);
            return rv;
        }

        /* disable credit watchdog */
        if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_DISABLE))) {
            printf("failed disabling the credit watchdog in common FSM mode\n");
            return rv;
        } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths1,
                BCM_FABRIC_WATCHDOG_QUEUE_DISABLE, q_min, q_max))) {
            printf("failed verifying watchdog config after disabling at iteration %d\n", i);
            return rv;
        }

        /* enable credit watchdog */
        if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE))) {
            printf("failed enabling the credit watchdog in common FSM mode at iteration %d\n", i);
            return rv;
        } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths1,
                BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, q_min, q_max))) {
            printf("failed verifying watchdog config after enabling at iteration %d\n", i);
            return rv;
        }

        /* change the credit watchdog queue range */
        q_min = 0; q_max = ((is_qux) ? (16383) : ((is_qax) ? (32767) : (98271))) - 800 * i;
        if ((rv = set_credit_watchdog_queue_range(unit, q_min, q_max))) {
            printf("failed setting Q range to %d - %d\n", q_min, q_max);
            return rv;
        } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths1,
                BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, q_min, q_max))) {
            printf("failed verifying watchdog config after setting Q range to %d - %d at iteration %d\n", q_min, q_max, i);
            return rv;
        }

        /* disable credit watchdog */
        if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable,BCM_FABRIC_WATCHDOG_QUEUE_DISABLE))) {
            printf("failed disabling the credit watchdog in common FSM mode at iteration %d 2nd time\n", i);
            return rv;
        } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths1,
                BCM_FABRIC_WATCHDOG_QUEUE_DISABLE, q_min, q_max))) {
            printf("failed verifying watchdog config after disabling at iteration %d 2nd time\n", i);
            return rv;
        }

        /* change the profiles */
        if ((rv = set_delete_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths2))) {
            printf("failed to configure profile delete thresholds 2 at iteration %d\n", i);
            return rv;
        } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths2,
                    BCM_FABRIC_WATCHDOG_QUEUE_DISABLE, q_min, q_max))) {
            printf("failed to verify profile delete thresholds 2 at iteration %d\n", i);
            return rv;
        }

        /* enable credit watchdog */
        if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE))) {
            printf("failed enabling the credit watchdog in common FSM mode at iteration %d 2nd time\n", i);
            return rv;
        } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths2,
                BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, q_min, q_max))) {
            printf("failed verifying watchdog config after enabling at iteration %d 2nd time\n", i);
            return rv;
        }

        /* change the profiles */
        if ((rv = set_delete_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths1))) {
            printf("failed to configure profile delete thresholds 1 at iteration %d\n", i);
            return rv;
        } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths2,
                    BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, q_min, q_max))) {
            printf("failed to verify profile delete thresholds 1 at iteration %d\n", i);
            return rv;
        }
    }

    /* disable credit watchdog */
    if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable,BCM_FABRIC_WATCHDOG_QUEUE_DISABLE))) {
        printf("failed disabling the credit watchdog in common FSM mode after the loop\n");
        return rv;
    } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths1,
            BCM_FABRIC_WATCHDOG_QUEUE_DISABLE, q_min, q_max))) {
        printf("failed verifying watchdog config after disabling after the loop\n");
        return rv;
    }
    /* set credit watchdog common message time */
    common_fsm = 125;
    dt.credit_request_watchdog_status_msg_gen = common_fsm;
    if ((rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG, &dt))) {
        printf("Failed setting common FSM value %d when credit watchdog is disaled, status=%d\n", common_fsm, rv);
        return rv;
    } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths1,
            BCM_FABRIC_WATCHDOG_QUEUE_DISABLE, q_min, q_max))) {
        printf("failed verifying watchdog config after changing common FSM when credit watchdog is disabled\n");
        return rv;
    }
    /* enable credit watchdog */
    if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE))) {
        printf("failed enabling the credit watchdog in common FSM mode after the loop\n");
        return rv;
    } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths1,
            BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, q_min, q_max))) {
        printf("failed verifying watchdog config after re-enabling after the loop\n");
        return rv;
    }

    /* restore original configuration */
    if ((rv = set_credit_watchdog_queue_range(unit, orig_q_min, orig_q_max))) {
        printf("failed setting original Q range %d - %d\n", orig_q_min, orig_q_max);
        return rv;
    } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, delete_ths1,
            BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, orig_q_min, orig_q_max))) {
        printf("failed verifying watchdog config after setting original Q range %d - %d\n", orig_q_min, orig_q_max);
        return rv;
    }
    if ((rv = set_delete_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, orig_delete_ths))) {
        printf("failed to configure original profile delete thresholds\n");
        return rv;
    } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, common_fsm, orig_delete_ths,
                BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, q_min, q_max))) {
        printf("failed to verify original profile delete thresholds\n");
        return rv;
    }
    dt.credit_request_watchdog_status_msg_gen = orig_common_fsm;
    if ((rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG, &dt))) {
        printf("Failed setting original common FSM value %d, status=%d\n", orig_common_fsm, rv);
        return rv;
    } else if ((rv = verify_credit_watchdog_and_ths_common_fsm(unit, nof_profiles, profiles, profile_names, orig_common_fsm, orig_delete_ths,
            BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE, orig_q_min, orig_q_max))) {
        printf("failed verifying restored original watchdog config\n");
        return rv;
    }

    return BCM_E_NONE;
}



/*
 Test the credit watchdog on different Arad versions.
 */
int test_credit_watchdog(int unit, int is_arad_plus, int is_qax, int is_qux)
{
    int i, j, rv;
    bcm_cosq_delay_tolerance_t dt;
    int orig_common_fsm = 1000;
    int orig_delete_ths[10];

    if (is_arad_plus) {
        if ((rv = test_credit_watchdog_common_fsm(unit, is_arad_plus, is_qax, is_qux))) {
            printf("First test of common credit mode failed\n");
            return rv;
        }
        /* move to normal mode, storing previous information */
        if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG, &dt))) {
            printf("Failed getting original common FSM value for restoring later, status=%d\n", rv);
            return rv;
        }
        orig_common_fsm = dt.credit_request_watchdog_status_msg_gen;

        for (i = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED, j = 0; ; ++i, ++j) {
            if (i > BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY) {i = BCM_COSQ_DELAY_TOLERANCE_14;}
            if ((rv = bcm_cosq_delay_tolerance_level_get(unit, i, &dt)) != BCM_E_UNAVAIL) {
                if (rv) {
                    printf("failed to get profile %d to move out of common FSM mode", i);
                    return rv;
                }
                orig_delete_ths[j] = dt.credit_request_watchdog_delete_queue_thresh;
                dt.credit_request_watchdog_delete_queue_thresh = 0;
                if ((rv = bcm_cosq_delay_tolerance_level_set(unit, i, &dt))) {
                    printf("failed to set profile %d to move out of common FSM mode", i);
                    return rv;
                }
            } else {
                orig_delete_ths[j] = 0;
            }
            if (i == BCM_COSQ_DELAY_TOLERANCE_14) {break;}
        }

        if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL))) {
            printf("failed to move to BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL credit watchdog mode");
            return rv;
        }

        /* set default common message times for the predefined profiles in this mode, since the tests expects it not to be disabled */
        for (i = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED; i <= BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY; ++i) {
            if ((rv = bcm_cosq_delay_tolerance_level_get(unit, i, &dt))) {
                printf("failed to get profile %d to set a common message time", i);
                return rv;
            }
            dt.credit_request_watchdog_status_msg_gen = 33;
            if ((rv = bcm_cosq_delay_tolerance_level_set(unit, i, &dt))) {
                printf("failed to set profile %d to set a common message time", i);
                return rv;
            }
        }
    }

    /* test the original credit watchdog modes */
    if ((rv = test_credit_watchdog_old(unit, is_qax, is_qux)) != BCM_E_NONE) {
        printf ("failed verifying the original Arad credit watchdog modes\n");
        return rv;
    }

    if (is_arad_plus) {
        /* move to common message time mode, restoring previous information */
        for (i = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED; ; ++i) {
            if (i > BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY) {i = BCM_COSQ_DELAY_TOLERANCE_14;}
            if ((rv = bcm_cosq_delay_tolerance_level_get(unit, i, &dt)) != BCM_E_UNAVAIL) {
                if (rv) {
                    printf("failed to get profile %d to move back to common FSM mode", i);
                    return rv;
                }
                dt.credit_request_watchdog_status_msg_gen = dt.credit_request_watchdog_delete_queue_thresh = 0;
                if ((rv = bcm_cosq_delay_tolerance_level_set(unit, i, &dt))) {
                    printf("failed to set profile %d to move back to common FSM mode", i);
                    return rv;
                }
            }
            if (i == BCM_COSQ_DELAY_TOLERANCE_14) {break;}
        }

        if ((rv = bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE))) {
            printf("failed to move to BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE credit watchdog mode");
        }

        dt.credit_request_watchdog_status_msg_gen = orig_common_fsm; /* restore original common message time */
        if ((rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG, &dt))) {
            printf("Failed setting original common message time %d, status=%d\n", orig_common_fsm, rv);
            return rv;
        }

        for (i = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED, j = 0; ; ++i, ++j) {
            if (i > BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY) {i = BCM_COSQ_DELAY_TOLERANCE_14;}
            if ((rv = bcm_cosq_delay_tolerance_level_get(unit, i, &dt)) != BCM_E_UNAVAIL) {
                if (rv) {
                    printf("failed to get profile %d to restore delete thresholds", i);
                    return rv;
                }
                dt.credit_request_watchdog_delete_queue_thresh = orig_delete_ths[j];
                if ((rv = bcm_cosq_delay_tolerance_level_set(unit, i, &dt))) {
                    printf("failed to set profile %d to restore delete thresholds", i);
                    return rv;
                }
            }
            if (i == BCM_COSQ_DELAY_TOLERANCE_14) {break;}
        }
        if ((rv = test_credit_watchdog_common_fsm(unit, is_arad_plus, is_qax, is_qux))) {
            printf("Second test of common credit mode failed\n");
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 Test the VOQ attributes.
 Works on queues it creates, using set and get operations.
 Since most of the attributes use a limited number of hardware profiles the test tries not to consume many profiles.
 The testing is done one queue per time, and usually returning it to its original configuration.
 */
int nof_rate_classes = 64;
int test_voq_attributes_rate_class_configured_directly(int unit, int chip_rev, int wb_mode) /* chip_rev == 0 means ARAD_A0, 3 is Arad+ */
{
    int rv, i, j;
    bcm_cosq_delay_tolerance_t dt1 = {0}, dt2 = {0};
    bcm_gport_t base_q;
    bcm_gport_t rate_class_gport;
    bcm_cos_queue_t cosq;
    int rate_class_id, rate_class_id_get;
    bcm_switch_profile_mapping_t profile_mapping, profile_mapping_get;

    int test_q_num_in_q_bundle = 4;
    int test_first_queue = ((chip_rev > 4) ? 16 : 64) * 1024 - 256;
    int test_last_queue = test_first_queue + (wb_mode ? 0 : 127);
    bcm_port_config_t port_config;
    int a_port = 7;

    if ((rv = bcm_port_config_get(unit, &port_config)) != BCM_E_NONE) {
        printf("failed to get a port for the test\n");
        return rv;
    }
    BCM_PBMP_ITER(port_config.port, a_port) {break;}

    profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
    profile_mapping_get.profile_type = bcmCosqIngressQueueToRateClass;

    /* delete the test queues if they existed before */
    if ((rv = delete_test_queues(unit, 1, test_q_num_in_q_bundle, test_first_queue, test_last_queue)) != BCM_E_NONE) {
        return rv;
    }
    /* create the test queues */
    if ((rv = open_test_queues(unit, test_q_num_in_q_bundle, test_first_queue, test_last_queue)) != BCM_E_NONE) {
        return rv;
    }
    for (i = (((test_first_queue + 3 *test_last_queue + 3)/4) / test_q_num_in_q_bundle) * test_q_num_in_q_bundle; /* use a quarter of the queues to make the test faster */
             i <= test_last_queue; i += test_q_num_in_q_bundle) 
    {
        /*for each test queue: remap it ro a random rate class -all but 0*/
        rate_class_id = (sal_rand()%(nof_rate_classes-1)) + 1;
        BCM_GPORT_PROFILE_SET(rate_class_gport, rate_class_id);
        profile_mapping.mapped_profile = rate_class_gport;

        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, i);
        for (cosq = 0 ; cosq < test_q_num_in_q_bundle; cosq++) {
            if (wb_mode) {bshell(unit, "warmboot test_mode on");}
            rv = bcm_cosq_profile_mapping_set(unit, base_q, cosq, 0, &profile_mapping);
            if (wb_mode) {bshell(unit, "warmboot test_mode off");}
            if (rv != BCM_E_NONE) {
                printf ("failed to map queue %d to rate class %d \n", i + cosq, rate_class_id);
                return rv;
            }
            rv = bcm_cosq_profile_mapping_get(unit, base_q, cosq, 0, &profile_mapping_get);
            rate_class_id_get = BCM_GPORT_PROFILE_GET(profile_mapping_get.mapped_profile);
            if (rv != BCM_E_NONE) {
                printf ("failed to map queue %d to rate class %d \n", i + cosq, rate_class_id_get);
                return rv;
            }
            if (rate_class_id != rate_class_id_get) {
                printf ("failed to map queue %d to rate class %d: different values for set(%d) and get(%d) \n" ,i + cosq, rate_class_id, rate_class_id_get);
            }
        }
    }
    /* 1. Configure Credit Request profile */
    dt1.credit_request_watchdog_status_msg_gen = (chip_rev >= 3) ? 0 : 125;
    dt1.credit_request_watchdog_delete_queue_thresh = 160;
    dt1.credit_request_hungry_off_to_slow_thresh = -2700;
    dt1.credit_request_hungry_off_to_normal_thresh = -2000;
    dt1.credit_request_hungry_slow_to_normal_thresh = 20000;
    dt1.credit_request_hungry_normal_to_slow_thresh = 10000;
    dt1.credit_request_hungry_multiplier = 4000;
    dt1.credit_request_satisfied_backoff_enter_thresh = 17000;
    dt1.credit_request_satisfied_backoff_exit_thresh = 16000;
    dt1.credit_request_satisfied_backlog_enter_thresh = 4099;
    dt1.credit_request_satisfied_backlog_exit_thresh = 2040;
    dt1.credit_request_satisfied_empty_queue_thresh = 2000;
    dt1.credit_request_satisfied_empty_queue_max_balance_thresh = 4000;
    dt1.credit_request_satisfied_empty_queue_exceed_thresh = 1;
    dt1.flags = 0;
    if ((rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_14, &dt1)) != BCM_E_NONE) {
        printf ("failed to set delay tolerance 14\n");
        return BCM_E_FAIL;
    }
    if ((rv = bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_14, &dt2)) != BCM_E_NONE) {
        printf ("failed to get delay tolerance 14\n");
        return BCM_E_FAIL;
    }

    int32 exp_a[15] = {
    dt1.credit_request_watchdog_status_msg_gen,
    dt1.credit_request_watchdog_delete_queue_thresh,
    dt1.credit_request_hungry_off_to_slow_thresh,
    dt1.credit_request_hungry_off_to_normal_thresh,
    dt1.credit_request_hungry_slow_to_normal_thresh,
    dt1.credit_request_hungry_normal_to_slow_thresh,
    dt1.credit_request_hungry_multiplier,
    dt1.credit_request_satisfied_backoff_enter_thresh,
    dt1.credit_request_satisfied_backoff_exit_thresh,
    dt1.credit_request_satisfied_backlog_enter_thresh,
    dt1.credit_request_satisfied_backlog_exit_thresh,
    dt1.credit_request_satisfied_empty_queue_thresh,
    dt1.credit_request_satisfied_empty_queue_max_balance_thresh,
    dt1.credit_request_satisfied_empty_queue_exceed_thresh,
    0 };
    int32 found_a[15] = {
    dt2.credit_request_watchdog_status_msg_gen,
    dt2.credit_request_watchdog_delete_queue_thresh,
    dt2.credit_request_hungry_off_to_slow_thresh,
    dt2.credit_request_hungry_off_to_normal_thresh,
    dt2.credit_request_hungry_slow_to_normal_thresh,
    dt2.credit_request_hungry_normal_to_slow_thresh,
    dt2.credit_request_hungry_multiplier,
    dt2.credit_request_satisfied_backoff_enter_thresh,
    dt2.credit_request_satisfied_backoff_exit_thresh,
    dt2.credit_request_satisfied_backlog_enter_thresh,
    dt2.credit_request_satisfied_backlog_exit_thresh,
    dt2.credit_request_satisfied_empty_queue_thresh,
    dt2.credit_request_satisfied_empty_queue_max_balance_thresh,
    dt2.credit_request_satisfied_empty_queue_exceed_thresh,
    0 };
    char* desc_a[15] = {
    "credit_request_watchdog_status_msg_gen",
    "credit_request_watchdog_delete_queue_thresh",
    "credit_request_hungry_off_to_slow_thresh",
    "credit_request_hungry_off_to_normal_thresh",
    "credit_request_hungry_slow_to_normal_thresh",
    "credit_request_hungry_normal_to_slow_thresh",
    "credit_request_hungry_multiplier",
    "credit_request_satisfied_backoff_enter_thresh",
    "credit_request_satisfied_backoff_exit_thresh",
    "credit_request_satisfied_backlog_enter_thresh",
    "credit_request_satisfied_backlog_exit_thresh",
    "credit_request_satisfied_empty_queue_thresh",
    "credit_request_satisfied_empty_queue_max_balance_thresh",
    "credit_request_satisfied_empty_queue_exceed_thresh",
    NULL };

    if (is_array_close (exp_a, found_a, desc_a)) {
        printf("found significant difference in delay tolerance retrieved after set\n");
        return BCM_E_FAIL;
    }

    printf("1. Configure max and guaranteed queue size per DP.\n");
    bcm_color_t color;
    bcm_color_t colors[4] = {bcmColorGreen, bcmColorYellow, bcmColorRed, bcmColorBlack};
    bcm_cosq_gport_size_t q_size, orig_size_bytes, orig_size_bds, q_size_bytes = {0, 20000}, exact_q_size_bytes, exact_q_size_bytes_voq, q_size_bds = {0, 25}, exact_q_size_bds, exact_q_size_bds_voq;
    bcm_cosq_gport_size_t orig_size_words, orig_size_sram_words, orig_size_sram_pds;
    bcm_cosq_gport_size_t q_size_words = {100, 20000, 1, 10000}, exact_q_size_words, exact_q_size_words_voq, q_size_sram_words = {50, 10000, -1, 5000}, exact_q_size_sram_words, exact_q_size_sram_words_voq, q_size_sram_pds = {50, 10000, 3, 5000}, exact_q_size_sram_pds, exact_q_size_sram_pds_voq;

    for (i = (((test_first_queue + 3 *test_last_queue + 3)/4) / test_q_num_in_q_bundle) * test_q_num_in_q_bundle, /* use a quarter of the queues to make the test faster */
         j = 0; i <= test_last_queue; i += test_q_num_in_q_bundle) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, i);
        if (chip_rev >= 5) {
            exp_a[0] = q_size_words.size_min; exp_a[1] = q_size_words.size_max; exp_a[2] = q_size_words.size_alpha_max; exp_a[3] = q_size_words.size_fadt_min;
            desc_a[0] = "guaranteed size in Words"; desc_a[1] = "FADT maximal threshold in Words"; desc_a[2] = "FADT alpha"; desc_a[3] = "FADT minimal threshold in Words"; desc_a[4] = NULL;
            exp_a[5] = q_size_sram_words.size_min; exp_a[6] = q_size_sram_words.size_max; exp_a[7] = q_size_sram_words.size_alpha_max; exp_a[8] = q_size_sram_words.size_fadt_min;
            desc_a[5] = "guaranteed size in SRAM-Words"; desc_a[6] = "FADT maximal threshold in SRAM-Words"; desc_a[7] = "FADT alpha"; desc_a[8] = "FADT minimal threshold in SRAM-Words"; desc_a[9] = NULL;
            exp_a[10] = q_size_sram_pds.size_min; exp_a[11] = q_size_sram_pds.size_max; exp_a[12] = q_size_sram_pds.size_alpha_max; exp_a[13] = q_size_sram_pds.size_fadt_min;
            desc_a[10] = "guaranteed size in SRAM-PDs"; desc_a[11] = "FADT maximal threshold in SRAM-PDs"; desc_a[12] = "FADT alpha"; desc_a[13] = "FADT minimal threshold in SRAM-PDs"; desc_a[14] = NULL;

            for (cosq = 0; cosq < test_q_num_in_q_bundle; ++cosq) {
                for (j = 0; j < 4; ++j) {
                    color = colors[j];  /* j == colors[j] , is the DP */

                    /*get rate class id*/
                    rv = bcm_cosq_profile_mapping_get(unit, base_q, cosq, 0, &profile_mapping_get);
                    if (rv != BCM_E_NONE) {
                        printf ("failed to get queue %d to rate class mapping \n", i + cosq);
                        return rv;
                    }
                    /*set rate class gport*/
                    rate_class_gport = profile_mapping_get.mapped_profile;
                    if (wb_mode) {bshell(unit, "warmboot test_mode on");}
                    if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &orig_size_words)) != BCM_E_NONE) {
                        printf ("failed to get original queue size in words 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BYTES, &orig_size_sram_words)) != BCM_E_NONE) {
                        printf ("failed to get original queue size in sram-words 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC, &orig_size_sram_pds)) != BCM_E_NONE) {
                        printf ("failed to get original queue size in sram-pds 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    } 
                    
                    /* Test Words */
                    if ((rv = bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &q_size_words)) != BCM_E_NONE) {
                        printf ("failed to set queue size in words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_words)) != BCM_E_NONE) {
                        printf ("failed to get queue size in words 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_words_voq)) != BCM_E_NONE) {
                        printf ("failed to get queue size in words via voq 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    }
                    if (wb_mode) {bshell(unit, "warmboot test_mode off");}
                    /*compare getting queue result via rate class or voq*/
                    if (exact_q_size_words.size_min != exact_q_size_words_voq.size_min || 
                        exact_q_size_words.size_max != exact_q_size_words_voq.size_max ||
                        exact_q_size_words.size_alpha_max != exact_q_size_words_voq.size_alpha_max ||
                        exact_q_size_words.size_fadt_min != exact_q_size_words_voq.size_fadt_min) {
                        printf ("found wrong queue size data at 0x%x+%d,%d when getting via rate class or voq,%d != %d || %d != %d\n || %d != %d || %d != %d", i, cosq, j,
                                exact_q_size_words.size_min, exact_q_size_words_voq.size_min, 
                                exact_q_size_words.size_max, exact_q_size_words_voq.size_max,
                                exact_q_size_words.size_alpha_max, exact_q_size_words_voq.size_alpha_max, 
                                exact_q_size_words.size_fadt_min, exact_q_size_words_voq.size_fadt_min);
                        return BCM_E_FAIL;
                    } 

                    found_a[0] = exact_q_size_words.size_min; found_a[1] = exact_q_size_words.size_max;
                    found_a[2] = exact_q_size_words.size_alpha_max; found_a[3] = exact_q_size_words.size_fadt_min;
                    if (wb_mode) {bshell(unit, "warmboot test_mode on");}
                    if (is_array_close (exp_a, found_a, desc_a)) {
                        printf ("found wrong queue size data in words at 0x%x+%d,%d\n", i, cosq, j);
                        print exact_q_size_words;
                        return BCM_E_FAIL;
                    } 
                    
                    /* Test SRAM-Words */
                    if ((rv = bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BYTES, &q_size_sram_words)) != BCM_E_NONE) {
                        printf ("failed to set queue size in sram-words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_sram_words)) != BCM_E_NONE) {
                        printf ("failed to get queue size in sram-words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_sram_words_voq)) != BCM_E_NONE) {
                        printf ("failed to get queue size in sram-words via voq at 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    }
                    if (wb_mode) {bshell(unit, "warmboot test_mode off");}

                    /*compare getting queue result via rate class or voq*/
                    if (exact_q_size_sram_words.size_min != exact_q_size_sram_words_voq.size_min || 
                        exact_q_size_sram_words.size_max != exact_q_size_sram_words_voq.size_max ||
                        exact_q_size_sram_words.size_alpha_max != exact_q_size_sram_words_voq.size_alpha_max ||
                        exact_q_size_sram_words.size_fadt_min != exact_q_size_sram_words_voq.size_fadt_min) {
                        printf ("found wrong queue size data at 0x%x+%d,%d when getting via rate class or voq,%d != %d || %d != %d\n || %d != %d || %d != %d", i, cosq, j,
                                exact_q_size_sram_words.size_min, exact_q_size_sram_words_voq.size_min, 
                                exact_q_size_sram_words.size_max, exact_q_size_sram_words_voq.size_max,
                                exact_q_size_sram_words.size_alpha_max, exact_q_size_sram_words_voq.size_alpha_max, 
                                exact_q_size_sram_words.size_fadt_min, exact_q_size_sram_words_voq.size_fadt_min);
                        return BCM_E_FAIL;
                    } 

                    found_a[0] = exact_q_size_sram_words.size_min; found_a[1] = exact_q_size_sram_words.size_max;
                    found_a[2] = exact_q_size_sram_words.size_alpha_max; found_a[3] = exact_q_size_sram_words.size_fadt_min;
                    if (wb_mode) {bshell(unit, "warmboot test_mode on");}
                    if (is_array_close (exp_a + 5, found_a, desc_a + 5)) {
                        printf ("found wrong queue size data in sram-words at 0x%x+%d,%d\n", i, cosq, j);
                        print exact_q_size_sram_words;
                        return BCM_E_FAIL;
                    } 
                    
                    /* Test SRAM-PDs */
                    if ((rv = bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC, &q_size_sram_pds)) != BCM_E_NONE) {
                        printf ("failed to set queue size in sram-pds 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC, &exact_q_size_sram_pds)) != BCM_E_NONE) {
                        printf ("failed to get queue size in sram-pds 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC, &exact_q_size_sram_pds_voq)) != BCM_E_NONE) {
                        printf ("failed to get queue size in sram-pds via voq at 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    }
                    if (wb_mode) {bshell(unit, "warmboot test_mode off");}

                    /*compare getting queue result via rate class or voq*/
                    if (exact_q_size_sram_pds.size_min != exact_q_size_sram_pds_voq.size_min || 
                        exact_q_size_sram_pds.size_max != exact_q_size_sram_pds_voq.size_max ||
                        exact_q_size_sram_pds.size_alpha_max != exact_q_size_sram_pds_voq.size_alpha_max ||
                        exact_q_size_sram_pds.size_fadt_min != exact_q_size_sram_pds_voq.size_fadt_min) {
                        printf ("found wrong queue size data at 0x%x+%d,%d when getting via rate class or voq,%d != %d || %d != %d\n || %d != %d || %d != %d", i, cosq, j,
                                exact_q_size_sram_pds.size_min, exact_q_size_sram_pds_voq.size_min, 
                                exact_q_size_sram_pds.size_max, exact_q_size_sram_pds_voq.size_max,
                                exact_q_size_sram_pds.size_alpha_max, exact_q_size_sram_pds_voq.size_alpha_max, 
                                exact_q_size_sram_pds.size_fadt_min, exact_q_size_sram_pds_voq.size_fadt_min);
                        return BCM_E_FAIL;
                    } 

                    found_a[0] = exact_q_size_sram_pds.size_min; found_a[1] = exact_q_size_sram_pds.size_max;
                    found_a[2] = exact_q_size_sram_pds.size_alpha_max; found_a[3] = exact_q_size_sram_pds.size_fadt_min;
                    if (wb_mode) {bshell(unit, "warmboot test_mode on");}
                    if (is_array_close (exp_a + 10, found_a, desc_a + 10)) {
                        printf ("found wrong queue size data in sram-pds at 0x%x+%d,%d\n", i, cosq, j);
                        print exact_q_size_sram_pds;
                        return BCM_E_FAIL;
                    } 

                    /* Return original thresholds */
                    if ((rv = bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &orig_size_words)) != BCM_E_NONE) {
                        printf ("failed to set original queue size in words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BYTES, &orig_size_sram_words)) != BCM_E_NONE) {
                        printf ("failed to set original queue size in sram-words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC, &orig_size_sram_pds)) != BCM_E_NONE) {
                        printf ("failed to set original queue size in sram-pds 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_words)) != BCM_E_NONE) {
                        printf ("failed to get (again) original queue size in words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if (orig_size_words.size_min != exact_q_size_words.size_min || orig_size_words.size_max != exact_q_size_words.size_max ||
                               orig_size_words.size_alpha_max != exact_q_size_words.size_alpha_max || orig_size_words.size_fadt_min != exact_q_size_words.size_fadt_min) {
                        printf ("found wrong original queue size data in words at 0x%x+%d,%d\n", i, cosq, j);
                        return BCM_E_FAIL;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_sram_words)) != BCM_E_NONE) {
                        printf ("failed to get (again) original queue size in sram-words 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if (orig_size_sram_words.size_min != exact_q_size_sram_words.size_min || orig_size_sram_words.size_max != exact_q_size_sram_words.size_max ||
                               orig_size_sram_words.size_alpha_max != exact_q_size_sram_words.size_alpha_max || orig_size_sram_words.size_fadt_min != exact_q_size_sram_words.size_fadt_min) {
                        printf ("found wrong original queue size data in sram-words at 0x%x+%d,%d\n", i, cosq, j);
                        return BCM_E_FAIL;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC, &exact_q_size_sram_pds)) != BCM_E_NONE) {
                        printf ("failed to get (again) original queue size in sram-pds 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if (orig_size_sram_pds.size_min != exact_q_size_sram_pds.size_min || orig_size_sram_pds.size_max != exact_q_size_sram_pds.size_max ||
                               orig_size_sram_pds.size_alpha_max != exact_q_size_sram_pds.size_alpha_max || orig_size_sram_pds.size_fadt_min != exact_q_size_sram_pds.size_fadt_min) {
                        printf ("found wrong original queue size data in sram-pds at 0x%x+%d,%d\n", i, cosq, j);
                        return BCM_E_FAIL;
                    }
                    if (wb_mode) {bshell(unit, "warmboot test_mode off");}
                }
            }
        } else {

            exp_a[0] = q_size_bytes.size_min; exp_a[1] = q_size_bytes.size_max;
            desc_a[0] = "guaranteed size in bytes"; desc_a[1] = "maximal size in bytes"; desc_a[2] = NULL;
            exp_a[3] = q_size_bds.size_min; exp_a[4] = q_size_bds.size_max;
            desc_a[3] = "guaranteed size in BDs"; desc_a[4] = "maximal size in BDs"; desc_a[5] = NULL;
            for (cosq = 0; cosq < test_q_num_in_q_bundle; ++cosq) {
                for (j = 0; j < 4; ++j) {
                    color = colors[j];  /* j == colors[j] , is the DP */
                    /*get rate class id*/
                    rv = bcm_cosq_profile_mapping_get(unit, base_q, cosq, 0, &profile_mapping_get);
                    if (rv != BCM_E_NONE) {
                        printf ("failed to get queue %d to rate class mapping \n", i + cosq);
                        return rv;
                    }
                    /*set rate class gport*/
                    rate_class_gport = profile_mapping_get.mapped_profile;
                    if (wb_mode) {bshell(unit, "warmboot test_mode on");}
                    if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &orig_size_bytes)) != BCM_E_NONE) {
                        printf ("failed to get original queue size in bytes 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BUFFER_DESC, &orig_size_bds)) != BCM_E_NONE) {
                        printf ("failed to get original queue size in buffer descriptors 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &q_size_bytes)) != BCM_E_NONE) {
                        printf ("failed to set queue size in bytes 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_bytes)) != BCM_E_NONE) {
                        printf ("failed to get queue size in bytes 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_bytes_voq)) != BCM_E_NONE) {
                        printf ("failed to get queue size in bytes via voq 0x%x+%d,%d\n", i, cosq, color);
                        return rv;
                    }
                    if (wb_mode) {bshell(unit, "warmboot test_mode off");}
                    /*compare getting queue result via rate class or voq*/
                    if (exact_q_size_bytes.size_min != exact_q_size_bytes_voq.size_min || 
                        exact_q_size_bytes.size_max != exact_q_size_bytes_voq.size_max) {
                        printf ("found wrong queue size data at 0x%x+%d,%d when getting via rate class or voq,%d != %d || %d != %d\n", i, cosq, j,
                                exact_q_size_bytes.size_min, exact_q_size_bytes_voq.size_min, 
                                exact_q_size_bytes.size_max, exact_q_size_bytes_voq.size_max);
                        return BCM_E_FAIL;
                    } 

                    found_a[0] = exact_q_size_bytes.size_min; found_a[1] = exact_q_size_bytes.size_max;
                    if (wb_mode) {bshell(unit, "warmboot test_mode on");}
                    if (is_array_close (exp_a, found_a, desc_a)) {
                        printf ("found wrong queue size data at 0x%x+%d,%d\n", i, cosq, j);
                        return BCM_E_FAIL;
                    } else if ((rv = bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BUFFER_DESC, &q_size_bds)) != BCM_E_NONE) {
                        printf ("failed to set queue size in buffer descriptors 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BUFFER_DESC, &exact_q_size_bds)) != BCM_E_NONE) {
                        printf ("failed to get queue size in buffer descriptors 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BUFFER_DESC, &exact_q_size_bds_voq)) != BCM_E_NONE) {
                        printf ("failed to get queue size in buffer descriptors via voq at 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    }
                    if (wb_mode) {bshell(unit, "warmboot test_mode off");}

                    /*compare getting queue result via rate class or voq*/
                    if (exact_q_size_bds.size_min != exact_q_size_bds_voq.size_min || 
                        exact_q_size_bds.size_max != exact_q_size_bds_voq.size_max) {
                        printf ("found wrong queue size data at 0x%x+%d,%d when getting via rate class or voq,%d != %d || %d != %d\n", i, cosq, j,
                                exact_q_size_bds.size_min, exact_q_size_bds_voq.size_min, 
                                exact_q_size_bds.size_max, exact_q_size_bds_voq.size_max);
                        return BCM_E_FAIL;
                    } 

                    found_a[0] = exact_q_size_bds.size_min; found_a[1] = exact_q_size_bds.size_max;
                    if (wb_mode) {bshell(unit, "warmboot test_mode on");}
                    if (is_array_close (exp_a + 3, found_a, desc_a + 3)) {
                        printf ("found wrong queue size data in BDs at 0x%x+%d,%d\n", i, cosq, j);
                        print exact_q_size_bds;
                        return BCM_E_FAIL;
                    } else if ((rv = bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &orig_size_bytes)) != BCM_E_NONE) {
                        printf ("failed to set original queue size in bytes 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BUFFER_DESC, &orig_size_bds)) != BCM_E_NONE) {
                        printf ("failed to set original queue size in buffer descriptors 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &exact_q_size_bytes)) != BCM_E_NONE) {
                        printf ("failed to get (again) original queue size in bytes 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if (orig_size_bytes.size_min != exact_q_size_bytes.size_min || orig_size_bytes.size_max != exact_q_size_bytes.size_max) {
                        printf ("found wrong original queue size data in bytes at 0x%x+%d,%d\n", i, cosq, j);
                        return BCM_E_FAIL;
                    } else if ((rv = bcm_cosq_gport_color_size_get(unit, rate_class_gport, cosq, color, BCM_COSQ_GPORT_SIZE_BUFFER_DESC, &exact_q_size_bds)) != BCM_E_NONE) {
                        printf ("failed to get (again) original queue size in buffer descriptors 0x%x+%d,%d\n", i, cosq, j);
                        return rv;
                    } else if (orig_size_bds.size_min != exact_q_size_bds.size_min || orig_size_bds.size_max != exact_q_size_bds.size_max) {
                        printf ("found wrong original queue size data in BDs at 0x%x+%d,%d\n", i, cosq, j);
                        return BCM_E_FAIL;
                    }
                    if (wb_mode) {bshell(unit, "warmboot test_mode off");}
                }
            }
        }
    }

    printf("2. Configure WRED discard profiles.\n");

    bcm_cosq_gport_discard_t wred, exact_wred, exact_wred_voq, orig_wred;
    uint32 color_flags[4] = {BCM_COSQ_DISCARD_COLOR_GREEN, BCM_COSQ_DISCARD_COLOR_YELLOW, BCM_COSQ_DISCARD_COLOR_RED, BCM_COSQ_DISCARD_COLOR_BLACK};
    wred.min_thresh = 16;
    wred.max_thresh = 100;
    wred.drop_probability = 100;
    wred.gain = 1;
    wred.ecn_thresh = 0;
    for (i = (((test_first_queue + 3 *test_last_queue + 3)/4) / test_q_num_in_q_bundle) * test_q_num_in_q_bundle, /* use a quarter of the queues to make the test faster */
         j = 0; i <= test_last_queue; i += test_q_num_in_q_bundle) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, i);
        for (cosq = 0; cosq < test_q_num_in_q_bundle; ++cosq) {
            for (j = 0; j < 4; ++j) {
                /*get rate class id*/
                rv = bcm_cosq_profile_mapping_get(unit, base_q, cosq, 0, &profile_mapping_get);
                if (rv != BCM_E_NONE) {
                    printf ("failed to get queue %d to rate class mapping \n", i + cosq);
                    return rv;
                }
                rate_class_gport = profile_mapping_get.mapped_profile;
                wred.flags = exact_wred.flags = exact_wred_voq.flags = orig_wred.flags = BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_BYTES | color_flags[j];
                if (wb_mode) {bshell(unit, "warmboot test_mode on");}
                if ((rv = bcm_cosq_gport_discard_get(unit, rate_class_gport, cosq, &orig_wred)) != BCM_E_NONE) {
                    printf ("failed getting original WRED parameters at 0x%x+%d,%d\n", i, cosq, j);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_set(unit, rate_class_gport, cosq, &wred)) != BCM_E_NONE) {
                    printf ("failed setting WRED parameters at 0x%x+%d,%d\n", i, cosq, j);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_get(unit, rate_class_gport, cosq, &exact_wred)) != BCM_E_NONE) {
                    printf ("failed getting WRED parameters at 0x%x+%d,%d\n", i, cosq, j);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_set(unit, rate_class_gport, cosq, &orig_wred)) != BCM_E_NONE) {
                    printf ("failed setting original WRED parameters at 0x%x+%d,%d\n", i, cosq, j);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_get(unit, rate_class_gport, cosq, &exact_wred)) != BCM_E_NONE) {
                    printf ("failed re-reading the original WRED parameters at 0x%x+%d,%d\n", i, cosq, j);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_get(unit, base_q, cosq, &exact_wred_voq)) != BCM_E_NONE) {
                    printf ("failed re-reading the original WRED parameters via voq at 0x%x+%d,%d\n", i, cosq, j);
                    return rv;
                } else if (
                           orig_wred.flags != exact_wred.flags ||
                           orig_wred.min_thresh != exact_wred.min_thresh ||
                           orig_wred.max_thresh != exact_wred.max_thresh ||
                           orig_wred.drop_probability != exact_wred.drop_probability ||
                           orig_wred.gain != exact_wred.gain ||
                           orig_wred.ecn_thresh != exact_wred.ecn_thresh) {
                    printf ("The original WRED parameters at 0x%x+%d,%d were not restores properly\n", i, cosq, j);
                    print orig_wred; print exact_wred;
                    return BCM_E_FAIL;
                } else if (
                           exact_wred_voq.flags != exact_wred.flags ||
                           exact_wred_voq.min_thresh != exact_wred.min_thresh ||
                           exact_wred_voq.max_thresh != exact_wred.max_thresh ||
                           exact_wred_voq.drop_probability != exact_wred.drop_probability ||
                           exact_wred_voq.gain != exact_wred.gain ||
                           exact_wred_voq.ecn_thresh != exact_wred.ecn_thresh) {
                    printf ("The WRED parameters at 0x%x+%d,%d were restores disserentlly when getting value via voq or voa rate class\n", i, cosq, j);
                    print exact_wred_voq; print exact_wred;
                    return BCM_E_FAIL;
                    if (wb_mode) {bshell(unit, "warmboot test_mode off");}
                }
            }
        }
    }

    if (chip_rev > 0) { /* ECN is not supported in A0 */
        printf("3. Configure ECN of QUEUES.\n");

        bcm_cosq_gport_discard_t wred2 = {0}, *cur_wred;
        wred.min_thresh = wred2.min_thresh = 32;
        wred.max_thresh = wred2.max_thresh = 240;
        wred.drop_probability = wred2.drop_probability = 81;
        wred.gain = wred2.gain = 1; /* Field not used since it can not be set for ECN */
        wred.flags = BCM_COSQ_DISCARD_MARK_CONGESTION | BCM_COSQ_DISCARD_BYTES | BCM_COSQ_DISCARD_ENABLE;
        wred.ecn_thresh = (chip_rev > 1) ? 100352 : 0x80000000;
        wred2.flags = BCM_COSQ_DISCARD_MARK_CONGESTION | BCM_COSQ_DISCARD_BUFFER_DESC | BCM_COSQ_DISCARD_ENABLE;
        wred2.ecn_thresh = 8000;
        for (i = (((test_first_queue + 3 *test_last_queue + 3)/4) / test_q_num_in_q_bundle) * test_q_num_in_q_bundle; /* use a quarter of the queues to make the test faster */
             i <= test_last_queue; i += test_q_num_in_q_bundle) {
            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, i);
            for (cosq = 0; cosq < test_q_num_in_q_bundle; ++cosq) {
                /*get rate class id*/
                rv = bcm_cosq_profile_mapping_get(unit, base_q, cosq, 0, &profile_mapping_get);
                if (rv != BCM_E_NONE) {
                    printf ("failed to get queue %d to rate class mapping \n", i + cosq);
                    return rv;
                }
                /*set rate class gport*/
                rate_class_gport = profile_mapping_get.mapped_profile;
                cur_wred = ((cosq & 1) && chip_rev < 5) ? &wred2 : &wred;
                orig_wred.flags = exact_wred.flags = exact_wred_voq.flags = cur_wred->flags;
                if (wb_mode) {bshell(unit, "warmboot test_mode on");}
                if ((rv = bcm_cosq_gport_discard_get(unit, rate_class_gport, cosq, &orig_wred)) != BCM_E_NONE) {
                    printf ("failed getting original ECN WRED parameters at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_set(unit, rate_class_gport, cosq, cur_wred)) != BCM_E_NONE) {
                    printf ("failed setting ECN WRED parameters at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_get(unit, rate_class_gport, cosq, &exact_wred)) != BCM_E_NONE) {
                    printf ("failed getting ECN WRED parameters at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if (
                           cur_wred->flags != exact_wred.flags ||
                           cur_wred->min_thresh != exact_wred.min_thresh ||
                           cur_wred->max_thresh != exact_wred.max_thresh ||
                           cur_wred->drop_probability != exact_wred.drop_probability ||
                           cur_wred->ecn_thresh != exact_wred.ecn_thresh) {
                    printf ("The ECN parameters at 0x%x+%d were not set properly\n", i, cosq);
                    print *cur_wred; print exact_wred;
                    return BCM_E_FAIL;
                } else if ((rv = bcm_cosq_gport_discard_set(unit, rate_class_gport, cosq, &orig_wred)) != BCM_E_NONE) {
                    printf ("failed setting original ECN WRED parameters at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_get(unit, rate_class_gport, cosq, &exact_wred)) != BCM_E_NONE) {
                    printf ("failed re-reading the original WRED parameters at 0x%x+%d\n", i, cosq);
                    return rv;
                } else if ((rv = bcm_cosq_gport_discard_get(unit, base_q, cosq, &exact_wred_voq)) != BCM_E_NONE) {
                    printf ("failed re-reading the original WRED parameters via voq at 0x%x+%d,\n", i, cosq);
                    return rv;
                } else if (
                           orig_wred.flags != exact_wred.flags ||
                           orig_wred.min_thresh != exact_wred.min_thresh ||
                           orig_wred.max_thresh != exact_wred.max_thresh ||
                           orig_wred.drop_probability != exact_wred.drop_probability ||
                           orig_wred.ecn_thresh != exact_wred.ecn_thresh) {
                    printf ("The original ECN parameters at 0x%x+%d were not restores properly\n", i, cosq);
                    print orig_wred; print exact_wred;
                    return BCM_E_FAIL;
                } else if (
                           exact_wred_voq.flags != exact_wred.flags ||
                           exact_wred_voq.min_thresh != exact_wred.min_thresh ||
                           exact_wred_voq.max_thresh != exact_wred.max_thresh ||
                           exact_wred_voq.drop_probability != exact_wred.drop_probability ||
                           exact_wred_voq.ecn_thresh != exact_wred.ecn_thresh) {
                    printf ("The original ECN parameters at 0x%x+%d were restores disserentlly when getting value via voq or voa rate class\n", i, cosq);
                    print exact_wred_voq; print exact_wred;
                    return BCM_E_FAIL;
                    if (wb_mode) {bshell(unit, "warmboot test_mode off");}
                }
            }
        }
    }
    /*Test invalidity: try to set a voqs ecn*/
    printf("4. Test invalid parameters.\n");
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, ((test_first_queue + 3 *test_last_queue + 3)/4));
    color = colors[0];
    cosq = 0;
    if (wb_mode) {bshell(unit, "warmboot test_mode on");}
    if (bcm_cosq_gport_color_size_set(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BYTES, &orig_size_bytes) == BCM_E_NONE) {
        printf ("Expexted to recieve a failure for setting a voq discard using voq\n", i, cosq);
        return BCM_E_FAIL;
    }
    if (bcm_cosq_gport_color_size_set(unit, base_q, cosq, color, BCM_COSQ_GPORT_SIZE_BUFFER_DESC, &orig_size_bytes) == BCM_E_NONE) {
        printf ("Expexted to recieve a failure for setting a voq discard using voq\n", i, cosq);
        return BCM_E_FAIL;
    }
    if ((bcm_cosq_gport_discard_set(unit, base_q, cosq, &orig_wred)) == BCM_E_NONE) {
        printf ("Expexted to recieve a failure for setting a voq discard using voq\n", i, cosq);
        return BCM_E_FAIL;
    }
    if (wb_mode) {bshell(unit, "warmboot test_mode off");}
    /* delete the test queues */
    if ((rv = delete_test_queues(unit, 0, test_q_num_in_q_bundle, test_first_queue, test_last_queue)) != BCM_E_NONE) {
        return rv;
    }
    return rv;
}

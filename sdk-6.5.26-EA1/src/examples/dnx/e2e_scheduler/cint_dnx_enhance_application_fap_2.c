/*~~~~~~~~~~~~~~~~~~~~~~~~~Multicast Options: Fabric Enhance Application~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_enhance_application_fap_2.c
 * Purpose: Example for enhance multicast fabric application
 *
 * Settings include:
 *  o        Enable Enhance mode and accordingly set FMQ queues range.
 *  o        Obtain HRs (FMQ classes) from enhance application.
 *  o        Set bandwidth and max burst limitations as seen in the figure below.
 *  o        Create E2E hierarchy.
 *  o        Set multicast groups.
 *
 * Default settings:
 * In this example, the following is assumed:
 *  o        Fabric is connected (make sure fabric_connect_mode is "FE").
 *  o        diag_init application is executed without cosq configuration (parameter cosq_disable is 1).
 * See figure below, with all settings as specified (with default parameters).
 *
 * Testing traffic can be done via TM or PP packets.
 * You need to set traffic with three streams: Destination Multicast, Destination ID MC 1-8.
 * For expected results, follow this example.
 *
 * You can also use the individual functions that are included in the example
 * in order to configure different settings for FMQ APIs (refer to documentation in code below).
 *
 * Note: You can specify different parameters for these examples including
 * traffic port, different FMQs, multicast groups.
 *
 * |
 * |
 * | +----------------------+    +-----------------+        +----+
 * | |  FMQ 10296-10303     |--> | VOQ Connector 1 |--SP0-->|    |   +------------+
 * | +----------------------+    +-----------------+        | HR |-->|            |
 * |                                                        |    |   |            |
 * | +----------------------+    +-----------------+        |    |   |            |
 * | |  FMQ 10288-10295     |--> | VOQ Connector 2 |--SP1-->|    |   |            |
 * | +----------------------+    +-----------------+        +----+   |            |
 * | +----------------------+    +-----------------+        +----+   |            |
 * | |  FMQ 10280-10287     |--> | VOQ Connector 3 |--SP0-->|    |   |            |
 * | +----------------------+    +-----------------+        | HR |-->|Best Effort1|
 * |                                                        |    |   |            |
 * | +----------------------+    +-----------------+        |    |   |            |
 * | |  FMQ 10272-10279     |--> | VOQ Connector 4 |--SP1-->|    |   |            |
 * | +----------------------+    +-----------------+        +----+   |            |
 * |                                                        +----+   |            |
 * | +--------------------- +    +-----------------+        |    |   |            |
 * | |  FMQ 10264-10271     |--> | VOQ Connector 5 |--SP0-->| HR |-->|            |       |\
 * | +--------------------- +    +-----------------+        |    |   |            |       | \
 * | +--------------------- +    +-----------------+        |    |   |            |       |  \
 * | |  FMQ 10256-10263     |--> | VOQ Connector 6 |--SP1-->|    |   |            |-SPR-> |   \ 7.5Gbps
 * | +----------------------+    +-----------------+        +----+   +------------+       |    |--SPR-->
 * | +----------------------+    +-----------------+        +----+                        | SP |
 * | |  FMQ 10248-10255     |--> | VOQ Connector 7 |--SP0-->|    |                        |    |
 * | +----------------------+    +-----------------+        | HR |   +------------+5Gbps  |    |
 * |                                                        |    |-->| Guaranteed |-SPR-> |   /
 * | +----------------------+    +-----------------+        |    |   +------------+       |  /
 * | |  FMQ 10240-10247     |--> | VOQ Connector 8 |--SP1-->|    |                        | /
 * | +----------------------+    +-----------------+        +----+                        |/
 *
 * Fabric Bandiwdth limitation: Guaranteed 5GB, Fabric root (credit generator) 7.5GB
 *
 * Multicast Configuration:
 *
 * MC ingress id 1 + TC => Destination FMQ + Multicast Fabric 10
 * MC ingress id 2 + TC => Destination FMQ + Multicast Fabric 10
 * MC ingress id 3 + TC => Destination FMQ + Multicast Fabric 10
 * MC ingress id 4 + TC => Destination FMQ + Multicast Fabric 10
 * MC ingress id 5 + TC => Destination FMQ + Multicast Fabric 10
 * MC ingress id 6 + TC => Destination FMQ + Multicast Fabric 10
 * MC ingress id 7 + TC => Destination FMQ + Multicast Fabric 10
 * MC ingress id 8 + TC => Destination FMQ + Multicast Fabric 10
 *
 * MC egress 10 => port 1 (assumes local<->system port 1 is up)
 *
 * Assumes Multicast Fabric 10 is open and send packets to all FAPs.
 *
 * The enhance application creates FMQ over the range of the first 1K,
 * thus using this script can be done only if no other application being set
 * Assumed diag init cosq configuration not being run.
 * This can be set by calling parameter diag_cosq_disable=1
 * Also fabric connect mode must be enabled i.e. fabric_connect_mode=FE
 */

cint_reset();

/*
 * Utilities functions
 */

/*
 * Set new FMQ Range
 */
int set_fmq_range(int unit, int bottom_queue, int top_queue)
{
    bcm_error_t rv;
    bcm_fabric_control_t control_type;

    control_type = bcmFabricMulticastQueueMax;
    rv = bcm_fabric_control_set(unit,control_type,top_queue);
    if (rv != BCM_E_NONE) {
        printf("Error, in queue range set, $control_type $rv \n");
        return rv;
    }

    return rv;
}

/*
 * Create FMQ queues
 */
int create_fmq_queue(int unit, int base_queue, int num_cos_level, int speed, int my_modid, bcm_gport_t port)
{
    bcm_error_t rv;
    bcm_gport_t gport;
    int get_cos_levels;
    bcm_gport_t get_physical_port;
    uint32 get_flags;
    bcm_cosq_ingress_queue_bundle_gport_config_t config;
    int cosq, delay_tolerance_level , rate_class;
    int profile_is_rate_class = *dnxc_data_get(unit, "iqs", "credit", "profile_is_rate_class", NULL);

    BCM_GPORT_MCAST_QUEUE_GROUP_SET(gport,base_queue);
    rv = appl_dnx_e2e_voq_profiles_get(unit, speed, &delay_tolerance_level, &rate_class);
    if (rv != BCM_E_NONE) {
        printf("appl_dnx_e2e_voq_profiles_get failed $rv\n");
        return(rv);
    }

    config.flags = BCM_COSQ_GPORT_MCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID;
    config.port = port;
    config.local_core_id = BCM_CORE_ALL;
    config.numq = num_cos_level;
    for (cosq = 0; cosq < num_cos_level; cosq++)
    {
        if (profile_is_rate_class)
        {
            config.queue_atrributes[cosq].delay_tolerance_level = -1;
        }
        else
        {
            config.queue_atrributes[cosq].delay_tolerance_level = delay_tolerance_level;
        }
        config.queue_atrributes[cosq].rate_class = rate_class;
    }
    rv = bcm_cosq_ingress_queue_bundle_gport_add(unit, &config, &gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in creating fmq, fmq: %d, num_cos: %d \n",base_queue,num_cos_level);
    }
    rv = bcm_cosq_gport_get(unit, gport, &get_physical_port, &get_cos_levels, &get_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, in getting fmq, fmq gport(0x%x)\n",base_queue);
    return rv;
    }
    if (get_cos_levels != num_cos_level) {
        printf("Error in gport_get of mcast queue, num_cos_levels set(%d) returned(%d)\n",
        num_cos_level, get_cos_levels);
    }

    if (get_flags != BCM_COSQ_GPORT_MCAST_QUEUE_GROUP) {
        printf("Error in gport_get of mcast queue, flags set(0x%x) returned(0x%x)\n",
        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP, get_flags);
    }
    return rv;
}

/*
 * Set Enable / Disable enhance
 */
int set_enhance(int unit,int mode)
{
    bcm_error_t rv;
    bcm_fabric_control_t control_type = bcmFabricMulticastSchedulerMode;

    rv = bcm_fabric_control_set(unit,control_type,mode);
    if (rv != BCM_E_NONE) {
        printf("Error, in enable / disable enhance, $mode \n");
    }

    return rv;
}

/*
 * Set FMQ bandwidth
 */
int set_fmqs_bandwidth(int unit, int kbits_sec_max_root,int kbits_sec_max_gaurentee,int kbits_sec_max_be_agr)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;

    gport_info.in_gport = 0; /* This is a general gport, in gport not in use */

    if (kbits_sec_max_root != -1) {
        /* Get handle to FMQ root and set rate */
        gport_type = bcmCosqGportTypeGlobalFmqRoot;
        rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, in handle get, gport_type $gport_type \n");
            return rv;
        }

        out_gport = gport_info.out_gport;
        rv = bcm_cosq_gport_bandwidth_set(unit,out_gport,0,0,kbits_sec_max_root,0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bandwidth set, out_gport $out_gport kbits_sec_max $kbits_sec_max_root \n");
            return rv;
        }
    }

    if (kbits_sec_max_gaurentee != -1) {
        /* Get handle to FMQ guaranteed and set rate */
        gport_type = bcmCosqGportTypeGlobalFmqGuaranteed;
        rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, in handle get, gport_type $gport_type \n");
            return rv;
        }

        out_gport = gport_info.out_gport;
        rv = bcm_cosq_gport_bandwidth_set(unit,out_gport,0,0,kbits_sec_max_gaurentee,0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bandwidth set, out_gport $out_gport kbits_sec_max $kbits_sec_max_gaurentee \n");
            return rv;
        }
    }

    if (kbits_sec_max_be_agr != -1) {
        /* Get handle to FMQ Best effort aggregate and set rate */
        gport_type = bcmCosqGportTypeGlobalFmqBestEffortAggregate;
        rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, in handle get, gport_type $gport_type \n");
            return rv;
        }

        out_gport = gport_info.out_gport;
        rv = bcm_cosq_gport_bandwidth_set(unit,out_gport,0,0,kbits_sec_max_be_agr,0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bandwidth set, out_gport $out_gport kbits_sec_max $kbits_sec_max_be_agr \n");
            return rv;
        }
    }

    return rv;
}

/*
 * Set Max Burst
 */
int set_fmqs_max_burst(int unit,int max_burst_gaurentee,int max_burst_be_agr)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;

    gport_info.in_gport = 0; /* This is a general gport, in gport not in use */

    if (max_burst_gaurentee != -1) {
        /* Get handle to FMQ guaranteed and set rate */
        gport_type = bcmCosqGportTypeGlobalFmqGuaranteed;
        rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, in handle get, gport_type $gport_type \n");
            return rv;
        }

        out_gport = gport_info.out_gport;
        rv = bcm_cosq_control_set(unit,out_gport,0,bcmCosqControlBandwidthBurstMax,max_burst_gaurentee);
        if (rv != BCM_E_NONE) {
            printf("Error, in bandwidth set, out_gport $out_gport max_burst $max_burst_gaurentee \n");
            return rv;
        }
    }

    if (max_burst_be_agr != -1) {
        /* Get handle to FMQ Best effort aggregate and set rate */
        gport_type = bcmCosqGportTypeGlobalFmqBestEffortAggregate;
        rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, in handle get, gport_type $gport_type \n");
            return rv;
        }

        out_gport = gport_info.out_gport;
        rv = bcm_cosq_control_set(unit,out_gport,0,bcmCosqControlBandwidthBurstMax,max_burst_be_agr);
        if (rv != BCM_E_NONE) {
            printf("Error, in bandwidth set, out_gport $out_gport max_burst $max_burst_be_agr \n");
            return rv;
        }
    }

    return rv;
}

/*
 * Set Best effort weights
 */
int set_fmqs_be_weights(int unit, int *best_efforts_weight)
{
    bcm_error_t rv = BCM_E_NONE;
    int mode;
    int weight, index;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;
    int fmq_be_nof_classes = 3;

    gport_info.in_gport = 0; /* This is a general gport, in gport not in use */

    for (index = 0; index < fmq_be_nof_classes; index++) {
        weight = best_efforts_weight[index];
        /* If one of the weights is -1, mode will be SP. That is MC2 > MC1 > MC0 */
        mode = (weight == -1) ? 0:-1;

        switch (index) {
        case 0:
            gport_type = bcmCosqGportTypeGlobalFmqBestEffort0;
            break;
        case 1:
            gport_type = bcmCosqGportTypeGlobalFmqBestEffort1;
            break;
        case 2:
            gport_type = bcmCosqGportTypeGlobalFmqBestEffort2;
            break;
        default:
            printf("Error in Set BE weights index out of bound\n");
            print index;
            return BCM_E_PARAM;
        }

        /* Get handle */
        rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, in handle get, gport_type $gport_type \n");
            return rv;
        }

        out_gport = gport_info.out_gport;
        rv = bcm_cosq_gport_sched_set(unit,out_gport,0,mode,weight);
        if (rv != BCM_E_NONE) {
            printf("Error, in sched set, out_gport(0x%08x) mode: %d weight: %d \n",out_gport,mode,weight);
            return rv;
        }
    }

    return rv;
}


/*
 * Create FQ element
 */
int create_fq(int unit, int destination_local_port, bcm_gport_t* gport_fq)
{
    bcm_error_t rv;
    bcm_cosq_scheduler_gport_params_t params;

    params.type = bcmCosqSchedulerGportTypeFq;
    params.mode = bcmCosqSchedulerGportModeSimple;
    params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;
    params.shared_order = bcmCosqSchedulerGportSharedOrderNone;
    params.core = core;
    rv = bcm_cosq_scheduler_gport_add(unit, 0, params, gport_fq);
    if (rv != BCM_E_NONE) {
        printf("Error in creating FQ\n");
        return rv;
    }

    return rv;
}

/*
 * Set bandwidth of Scheduler Element
 */
int set_scheduler_element_bandwidth(int unit, bcm_gport_t se,int kbits_sec_max)
{
    bcm_error_t rv;

    rv = bcm_cosq_gport_bandwidth_set(unit,se,0,0,kbits_sec_max,0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bandwidth set for scheduler element, gport (0x%08x) kbits_sec_max %d \n",se,kbits_sec_max);
        return rv;
    }

    return rv;
}

/*
 * Retreive 4 HRs from the enhance application configuration
 */
int get_enhance_hrs(int unit, int core, bcm_gport_t *hrs_classes)
{
    bcm_error_t rv = BCM_E_NONE;
    int index;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_core_t gport_info;
    bcm_gport_t out_gport;

    for (index = 0; index < 4; index++) {
        switch (index) {
        case 0:
            gport_info.gport_type = bcmCosqGportTypeGlobalFmqClass1;
            break;
        case 1:
            gport_info.gport_type = bcmCosqGportTypeGlobalFmqClass2;
            break;
        case 2:
            gport_info.gport_type = bcmCosqGportTypeGlobalFmqClass3;
            break;
        case 3:
            gport_info.gport_type = bcmCosqGportTypeGlobalFmqClass4;
            break;
        default:
            printf("Error in Obtain HRs from enhance application, index is out of bound\n");
            print index;
            return BCM_E_PARAM;
        }

        /* Get handle */
        rv = bcm_cosq_gport_handle_core_get(unit, core, &gport_info, &out_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, in handle get, gport_type $gport_info.gport_type \n");
            return rv;
        }
        hrs_classes[index] = out_gport;
    }

    return rv;
}


/*
 * Connect Scheduler Element to Strict Priority
 */
int connect_scheduler_element(int unit, bcm_gport_t se,int priority)
{
    bcm_error_t rv;

    rv = bcm_cosq_gport_sched_set(unit,se,0,priority,0);
    if (rv != BCM_E_NONE) {
        printf("Error in connecting FQ (0x%08x) to priority %d\n",se,priority);
        return rv;
    }

    return rv;
}

/*
 * Attach Scheduler element
 */
int attach_scheduler_element(int unit,bcm_gport_t sched_port,bcm_gport_t port,int priority)
{
    bcm_error_t rv;

    rv = bcm_cosq_gport_attach(unit,sched_port,port,priority);
    if (rv != BCM_E_NONE) {
        printf("Error in Attach (0x%08x) to sched port (0x%08x) priority %d\n",port,sched_port,priority);
    }

    return rv;
}

/*
 * Create VOQ connector
 */
int create_voq_connector(int unit, int core, int num_cos_levels, int destination_local_port ,bcm_gport_t *voq_connector)
{
    bcm_error_t rv;
    int my_modid;
    bcm_gport_t local_gport;
    bcm_cosq_voq_connector_gport_t config;

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

    BCM_GPORT_LOCAL_SET(config.port, destination_local_port);
    config.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR;
    config.numq = num_cos_levels;
    config.remote_modid = my_modid + core;
    config.nof_remote_cores = 1;
    rv = bcm_cosq_voq_connector_gport_add(unit, &config, voq_connector);
    if (rv != BCM_E_NONE) {
        printf("Error in creating VOQ connector with %d cos levels \n",num_cos_levels);
    }
    return rv;
}

/*
 * Connect VOQ connector
 */
int connect_voq_connector(int unit,bcm_gport_t voq_connector,int voq_connector_tc,int mode,int weight)
{
    bcm_error_t rv;
    rv = bcm_cosq_gport_sched_set(unit,voq_connector,voq_connector_tc,mode,weight);
    if (rv != BCM_E_NONE) {
        printf("Error in connect VOQ connector with ID %d tc %d to mode: %d, weight: %d \n",
               BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(voq_connector),voq_connector_tc,mode,weight);
    }
    return rv;
}
/* ******************************************************************** */

/*
 * Enhance application sequence
 * Running 3 Streams all toghether 10Gbps on the same port:
 * I. ITMH Destination MC 1
 * II. ITMH Destination MC 2
 * III. ITMH Destination MC 3
 *  (Multicast destination can be also obtained from PP)
 * Run packet size that align with credit size (1024).
 * Expect -  I. 3Gbps, II. 2Gbps, III. 2.5Gbps
 * User can run higher stream of MC3 and expect all to be of 2.5Gpbs
 */

/* User defined parameters */
/* Multicast ingress start: Create 3 conseutive multicast ingress from base given */
bcm_multicast_t start_of_multicast_ingress = 1;
/* Multicast fabric: Assumed multicast fabric is already allocated where destination is to the same local FAP */
/* Note: Multicast egress is the same as multicast fabric. I.E. application add to multicast egress 10 the destination local port */
bcm_multicast_t multicast_fabric_1 = 10;
/* Base FMQs the application create scheduleing Hirerchy. */
/* Destination local port to see expected results */
int destination_local_port = 13;

/* Destination System port ids in case system red enabled */
int system_port_ids[4] = {100,101,102,103};
int unit = 0;

int start_enhance_application(int unit, int destination_local_port)
{
    bcm_error_t rv;
    int num_cos_level = 8;
    int mode;
    int weight;
    int kbits_sec_max_all, kbits_sec_max_gaurentee;
    int max_burst_gaurentee;
    int max_burst_be_agr;
    int best_efforts_weight[3];
    bcm_gport_t mg1_voq_connector,mg2_voq_connector, mg3_voq_connector, mg4_voq_connector;
    bcm_gport_t hr_classes[4];
    bcm_gport_t mod_gport,sysport;
    int kbits_sec_fq;
    int voq_connector_tc;
    int index, index_2;
    bcm_cosq_gport_connection_t connect_info;
    int my_modid, core, speed;
    int num_of_queues = 8;
    bcm_gport_t voq_connectors[8];
    int base_queues[8];
    int multicast_1k_region_base = 10240;
    int multicast_1k_region_max = 1023+multicast_1k_region_base;
    for (index=0;index<num_of_queues;++index) {
        base_queues[index]=(index * num_cos_level) + multicast_1k_region_base;
    }
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_resource_t resource;

    rv = bcm_port_get(unit, destination_local_port, &dummy_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_get scheduler failed $rv\n");
        return rv;
    }
    core = mapping_info.core;

    rv = bcm_port_resource_get(unit, destination_local_port, &resource);
    if (rv != BCM_E_NONE) {
            printf("bcm_port_resource_get failed $rv\n");
            return(rv);
    }
    speed = resource.speed;

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

    /* Enable enhance application */
    mode = 1;
    rv = set_enhance(unit,mode);
    if (rv != BCM_E_NONE) {
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /** Get number of queues in a region */
    uint32 *max_val = dnxc_data_get(unit, "ipq", "regions", "nof_queues", NULL);
    multicast_1k_region_max = (*max_val * 2) - 1;

    for (index=0;index<num_of_queues;++index) {
        base_queues[index]=(index * num_cos_level) + *max_val;
    }

    /* Set new fmq range */
    rv = set_fmq_range(unit,multicast_1k_region_base, multicast_1k_region_max);
    if (rv != BCM_E_NONE) {
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* Obtain HR Class 0 from Enhance Application and Attach those 2 FQ to SP1 */
    rv = get_enhance_hrs(unit, core, hr_classes);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Create FMQ queues */
    /* Connect FMQs to relevant Destination:
       0-3  to HR 0,
       4-7  to HR 1
       8-11 to HR 2,
       12-15 to HR 2,
    */
    for (index=0;index<num_of_queues;++index) {
        rv = create_fmq_queue(unit,base_queues[index],num_cos_level,speed,my_modid,hr_classes[index/2]);
        if (rv != BCM_E_NONE) {
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Set Bandwidth */
    kbits_sec_max_all = 7500000; /* 7.5 Gbps */
    kbits_sec_max_gaurentee = 5000000; /* 5 Gbps */
    /* kbits_sec_max_all = -1; 7.5 Gbps */
    /* kbits_sec_max_gaurentee = -1; 5 Gbps */
    rv = set_fmqs_bandwidth(unit,kbits_sec_max_all,kbits_sec_max_gaurentee,-1);
    if (rv != BCM_E_NONE) {
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set Max Burst */
    max_burst_gaurentee = 63;
    max_burst_be_agr = -1;

    /* max_burst_gaurentee = -1;*/
    /* max_burst_be_agr = -1;*/
    rv = set_fmqs_max_burst(unit,max_burst_gaurentee,max_burst_be_agr);
    if (rv != BCM_E_NONE) {
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* Connection stage FMQs <-> VOQ connectors
     * At the Egress Do the follows:
     * 1. Create VOQ connector
     * 2. Connect VOQ connector
     * 3. Attach VOQ connector to a FQ
     * 4. Back connecting VOQ connector to FMQ
     */

    /*  1. Create VOQ connector */
    for (index = 0;index<num_of_queues;index++ ) {
        rv = create_voq_connector(unit, core, num_cos_level,destination_local_port,voq_connectors+index);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
    /* 2. Connect VOQ connector */
    for (index = 0; index < num_of_queues; index++) {
        mode = BCM_COSQ_SP0;
        weight = 0;
        for (index_2=0;index_2<num_cos_level;index_2++) {
            voq_connector_tc = index_2;
            rv = connect_voq_connector(unit,voq_connectors[index],voq_connector_tc,mode,weight);
            if (rv != BCM_E_NONE) {
                print rv;
                printf("(%s) \n",bcm_errmsg(rv));
                return rv;
            }
        }
        index++;
        mode = BCM_COSQ_SP1;
        for (index_2=0;index_2<num_cos_level;index_2++) {
            voq_connector_tc = index_2;
            rv = connect_voq_connector(unit,voq_connectors[index],voq_connector_tc,mode,weight);
            if (rv != BCM_E_NONE) {
                print rv;
                printf("(%s) \n",bcm_errmsg(rv));
                return rv;
            }
        }
    }

    /* 3. Attach VOQ connector */
    for (index = 0; index < num_of_queues; index++) {
        for (index_2=0;index_2 < num_cos_level;index_2++) {
            voq_connector_tc = index_2;
            rv = attach_scheduler_element(unit,hr_classes[index/2],voq_connectors[index],voq_connector_tc);
            if (rv != BCM_E_NONE) {
                print rv;
                printf("(%s) \n",bcm_errmsg(rv));
                return rv;
            }
        }
    }

    /* 4. Back connecting VOQ connector to FMQ */
    /* Since FMQ E2E this configuration is local, remote modid is exactly as mymodid */
    connect_info.remote_modid = my_modid + core;
    connect_info.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    for (index=0;index<num_of_queues;index++) {
        BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(connect_info.voq, mapping_info.core, base_queues[index]);
        connect_info.voq_connector = voq_connectors[index];
        rv = bcm_cosq_gport_connection_set(unit,&connect_info);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
    /* At the Ingress, Do the follows */
    /* 1. Connect the FMQ to the VOQ-Connector */
    /* Since FMQ E2E configuration is local, remote modid is exactly as mymodid */
    connect_info.remote_modid = my_modid + core;
    connect_info.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    for (index = 0 ; index < num_of_queues ; index++)
    {
        BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(connect_info.voq, mapping_info.core, base_queues[index]);
        connect_info.voq_connector = voq_connectors[index];
        rv = bcm_cosq_gport_connection_set(unit,&connect_info);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
    /*
     * Create 3 ingress MC groups,
     * id 1 + TC => Destination FMQ + Multicast Fabric 10
     * id 2 + TC => Destination FMQ + Multicast Fabric 10
     * id 3 + TC => Destination FMQ + Multicast Fabric 10
     * id 4 + TC => Destination FMQ + Multicast Fabric 10
     * id 5 + TC => Destination FMQ + Multicast Fabric 10
     * id 6 + TC => Destination FMQ + Multicast Fabric 10
     * id 7 + TC => Destination FMQ + Multicast Fabric 10
     * id 8 + TC => Destination FMQ + Multicast Fabric 10
     * All multicast groups points to the same multicast fabric (same cud: cud=10)
     */
    {
        bcm_gport_t gport;
        int cud = multicast_fabric_1;
        bcm_gport_t dest_port;
        uint32 flags = 0;
        bcm_multicast_replication_t replication_array;

        BCM_GPORT_LOCAL_SET(dest_port, destination_local_port);

        flags = BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID;
        rv = bcm_multicast_create(unit, flags, &multicast_fabric_1);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("bcm_multicast_create failed (%s) \n",bcm_errmsg(rv));
            return rv;
        }
        printf("Create EGRESS MC with id %d and dest port %d \n",multicast_fabric_1, destination_local_port);

        flags = BCM_MULTICAST_EGRESS_GROUP;
        replication_array.port = dest_port;
        replication_array.encap1 = 0;

        rv = bcm_multicast_add(unit, multicast_fabric_1, flags, 1, replication_array);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("bcm_multicast_add failed (%s) \n",bcm_errmsg(rv));
            return rv;
        }

        bcm_module_t mreps[1];
        mreps[0] = mapping_info.core;
        rv = bcm_fabric_multicast_set(unit, multicast_fabric_1, 0, 1, mreps);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("bcm_fabric_multicast_set failed (%s) \n",bcm_errmsg(rv));
            return rv;
        }

        for (index = 0; index < num_of_queues; index++)
        {
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
            if (start_of_multicast_ingress != 1) {
                /**
                 * MC ID 1 is created by default for Jericho2
                 */
                rv = bcm_multicast_create(unit, flags, &start_of_multicast_ingress);
                if (rv != BCM_E_NONE) {
                    print rv;
                    printf("bcm_multicast_create failed(%s) \n",bcm_errmsg(rv));
                    return rv;
                }
            }

            BCM_GPORT_MCAST_QUEUE_GROUP_SET(gport, base_queues[index]);

            flags = BCM_MULTICAST_INGRESS_GROUP;


            printf("Create INGRESS MC with id %d to MC queue %d \n",start_of_multicast_ingress, base_queues[index]);

            replication_array.port = gport;
            replication_array.encap1 = cud;
            rv = bcm_multicast_add(unit, start_of_multicast_ingress, flags, 1, replication_array);
            if (rv != BCM_E_NONE) {
                print rv;
                printf("bcm_multicast_add failed (%s) \n",bcm_errmsg(rv));
                return rv;
            }
            start_of_multicast_ingress++;
        }
    }

    /* set bandwidth to all voq connectors where:
     * tc=0->max_rate=7500000
     * tc=1->max_rate=7000000
     * tc=2->max_rate=6500000
     * tc=3->max_rate=6000000
     * tc=4->max_rate=5500000
     * tc=5->max_rate=5000000
     * tc=6->max_rate=4500000
     * tc=7->max_rate=4000000
     */
    {
        int max_bandwidth_per_cosq[8] = {7500000,
                                         7000000,
                                         6500000,
                                         6000000,
                                         5500000,
                                         5000000,
                                         4500000,
                                         4000000};
        for (index=0; index < num_of_queues; index++)
        {
            for (index_2=0;index_2<num_cos_level;index_2++)
            {
                rv = bcm_cosq_gport_bandwidth_set(unit,voq_connectors[index],index_2,0,max_bandwidth_per_cosq[index_2],0);
                if (rv != BCM_E_NONE) {
                    print rv;
                    printf("(%s) \n",bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }

   /*
    *  for each FMQ create a TC Mapping:
    *  0->7   7->0
    *  1->6   6->1
    *  2->5   5->2
    *  3->4   4->3
    */
    {
        bcm_gport_t gport;
        int in_tc,mapping_tc;
        for (index=0; index < num_of_queues; index++)
        {
            BCM_GPORT_MCAST_QUEUE_GROUP_SET(gport, base_queues[index]);
            for (index_2=0; index_2 < num_cos_level; index_2++)
            {
                in_tc = index_2;
                mapping_tc = (num_of_queues - (index_2 + 1));
                rv = bcm_cosq_port_mapping_set(unit, gport, in_tc, mapping_tc);
                if (rv != BCM_E_NONE) {
                    print rv;
                    printf("(%s) \n",bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }

    printf("cint_dnx_enhance_application_2.c completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}


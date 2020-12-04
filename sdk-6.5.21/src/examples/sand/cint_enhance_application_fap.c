/*~~~~~~~~~~~~~~~~~~~~~~~~~Multicast Options: Fabric Enhance Application~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_enhance_application_fap.c
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
 * You need to set traffic with three streams: Destination Multicast, Destination ID MC 1-3.
 * For expected results, follow this example.
 * 
 * You can also use the individual functions that are included in the example
 * in order to configure different settings for FMQ APIs (refer to documentation in code below).
 * 
 * Note: You can specify different parameters for these examples including
 * traffic port, different FMQs, multicast groups.
 * 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * |                                                                                                 |
 * |                                                                                                 |
 * | +-----------+   +-----------------+   +----+        +----+                       |\             |
 * | |  FMQ 4-7  |-->| VOQ Connector 1 |-->| FQ |--SPR-->|    |   +------------+ 5Gbp | \            |
 * | +-----------+   +-----------------+   +----+        |    |-->| Guaranteed |-SPR->|  \           |
 * |                                Guaranteed           | HR |   +------------+      |   \ 7.5Gbps  |
 * | +-----------+   +-----------------+   +----+  2Gbp  |    |                       |    |--SPR--> |
 * | |  FMQ 8-11 |-->| VOQ Connector 2 |-->| FQ |--SPR-->|    |                       | SP |         |
 * | +-----------+   +-----------------+   +----+        +----+                       |    |         |
 * |                                Best Effort1         +----+                       |    |         |
 * | +-----------+   +-----------------+   +----+        |    |   +------------+      |   /          |
 * | | FMQ 12-16 |-->| VOQ Connector 3 |-->| FQ |------->| HR |-->|Best Effort1|-SPR->|  /           |
 * | +-----------+   +-----------------+   +----+        |    |   +------------+      | /            |
 * |                                                     +----+                       |/             |
 * |                                                                                                 |
 * |                                                                          +----------------+     |
 * |                                                                          |      KEY       |     |
 * |                                                                          +----------------+     |
 * |                                                                          |SPR- Rate Shaper|     |
 * |                                                                          |                |     |
 * |                        +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+      +----------------+     |
 * |                        |   Figure 2: Enhanced Fabric Multicast    |                             |  
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * Fabric Bandiwdth limitation: Guaranteed 5GB, Fabric root (credit generator) 7.5GB
 * 
 * Multicast Configuration:
 * 
 * MC ingress id 1 + TC => Destination FMQ 4-7 + Multicast Fabric 10
 * MC ingress id 2 + TC => Destination FMQ 8-11 + Multicast Fabric 10
 * MC ingress id 3 + TC => Destination FMQ 12-16 + Multicast Fabric 10
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
    int is_dnx;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }

    if (!is_dnx)
    {
        /** For Jericho2 Min FMQ is always 0 */
        control_type = bcmFabricMulticastQueueMin;
        rv = bcm_fabric_control_set(unit,control_type,bottom_queue);
        if (rv != BCM_E_NONE) {
            printf("Error, in queue range set, $control_type $rv \n");
            return rv;
        }
    }

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
int create_fmq_queue(int unit,int base_queue, int num_cos_level, int my_modid, bcm_gport_t port)
{
    bcm_error_t rv;
    bcm_gport_t gport;
    int get_cos_levels;
    bcm_gport_t get_physical_port;
    uint32 get_flags;

    BCM_GPORT_MCAST_QUEUE_GROUP_SET(gport,base_queue);    
    rv = bcm_cosq_gport_add(unit,port,num_cos_level,BCM_COSQ_GPORT_WITH_ID | BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,&gport);
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
    bcm_gport_t local_gport = 0;
    bcm_error_t rv;
    int num_cos_level = 1;
    uint32 flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ;

    BCM_GPORT_LOCAL_SET(local_gport, destination_local_port);
    rv = bcm_cosq_gport_add(unit,local_gport,num_cos_level,flags,gport_fq);
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

int my_modid_get(int unit, int *my_modid)
{
    bcm_error_t rv;
    int is_dnx;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }

    if (is_dnx) {
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
        *my_modid = modid_array[0].modid;
    }
    else {

        rv = bcm_stk_my_modid_get(unit, my_modid);
        if (rv != BCM_E_NONE) {
            printf("error in get my modid\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}

/* 
 * Create VOQ connector 
 */
int create_voq_connector(int unit, int core, int num_cos_levels, int destination_local_port, bcm_gport_t *voq_connector)
{
    bcm_error_t rv;
    int my_modid;
    bcm_gport_t local_gport;
    bcm_cosq_voq_connector_gport_t config;

    rv = my_modid_get(unit, &my_modid);
    if (rv != BCM_E_NONE) {
        printf("error in get my modid\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

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
int base_queues[3] = {4,8,12};
/* Destination local port to see expected results */
int destination_local_port = 13;

/* Destination System port ids in case system red enabled */
int system_port_ids[4] = {100,101,102,103};
int unit = 0;

int start_enhance_application(int unit, int destination_local_port)
{    
    int multicast_1k_region_base = 0;
    int multicast_1k_region_max;
    bcm_error_t rv;
    int num_cos_level = 4;
    int mode;
    int weight;
    int kbits_sec_max_all, kbits_sec_max_gaurentee;
    int max_burst_gaurentee;
    int max_burst_be_agr;
    int best_efforts_weight[3];
    bcm_gport_t mg1_fq,mg2_fq;
    bcm_gport_t mg1_voq_connector,mg2_voq_connector, mg3_voq_connector;
    bcm_gport_t hr_classes[4];
    bcm_gport_t mod_gport,sysport, local_gport;;
    int kbits_sec_fq;
    int voq_connector_tc;
    int index, core;
    bcm_cosq_gport_connection_t connect_info;
    int my_modid;
    int is_dnx;
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }

    /** The max region for Jericho2 must be ((multiple of 2048)-1) or 3 */
    if (is_dnx) {
        /** Get number of queues in a region */
        uint32 *max_val = dnxc_data_get(unit, "ipq", "regions", "nof_queues", NULL);
        multicast_1k_region_max = *max_val - 1;
    } else {
        multicast_1k_region_max = 1023;
    }

    rv = bcm_port_get(unit, destination_local_port, &dummy_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_get scheduler failed $rv\n");
        return rv;
    }
    core = mapping_info.core;

    rv = my_modid_get(unit,&my_modid);
    if (rv != BCM_E_NONE) {
        printf("error in get my modid\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* Enable enhance application */
    mode = 1;
    rv = set_enhance(unit,mode);
    if (rv != BCM_E_NONE) {
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
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

    /** In Jericho a port is created for each FMQ. Need to map this
     *  port to systemport.
     *  In Jericho2 no port is created and no mapping is required */
    if (!is_dnx) {
        for (index = 0; index < 4; index++) {
          mod_gport=hr_classes[index]; /* modport */
          BCM_GPORT_SYSTEM_PORT_ID_SET(sysport,system_port_ids[index]); /* system port */
          rv = bcm_stk_sysport_gport_set(unit,sysport,mod_gport);
          if (rv != BCM_E_NONE) {
              printf("rv is: $rv \n");
              printf("(%s)\n", bcm_errmsg(rv));
              return rv;
          }
        }
    }

    /* Create FMQ queues */
    /* Connect FMQs to relevant Destination: 0-7 to HR 0, 8-11 to HR 1 */    
    rv = create_fmq_queue(unit,base_queues[0],num_cos_level,my_modid,hr_classes[0]);
    if (rv != BCM_E_NONE) {
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_fmq_queue(unit,base_queues[1],num_cos_level,my_modid,hr_classes[0]);
    if (rv != BCM_E_NONE) {
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_fmq_queue(unit,base_queues[2],num_cos_level,my_modid,hr_classes[1]);
    if (rv != BCM_E_NONE) {
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }    

    /* Set Bandwidth */
    kbits_sec_max_all = 7500000; /* 7.5 Gbps */  
    kbits_sec_max_gaurentee = 5000000; /* 5 Gbps */
    rv = set_fmqs_bandwidth(unit,kbits_sec_max_all,kbits_sec_max_gaurentee,-1);
    if (rv != BCM_E_NONE) {
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    
    /* Set Max Burst */
    max_burst_gaurentee = 63;    
    max_burst_be_agr = -1;

    rv = set_fmqs_max_burst(unit,max_burst_gaurentee,max_burst_be_agr);
    if (rv != BCM_E_NONE) {
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create E2E scheduler hirerchy for Enhance */    
    /* Create 2 FQs, one for each FMQ group (0-3,4-7) */
    rv = create_fq(unit, destination_local_port, &mg1_fq);
    if (rv != BCM_E_NONE) {
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    
    rv = create_fq(unit, destination_local_port, &mg2_fq);
    if (rv != BCM_E_NONE) {
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }    
    
    /* Set bandwidth for each FQ. MG1: limit to 3GBps,MG2: limit to 2GBps. */      
    kbits_sec_fq = 3000000;
    rv = set_scheduler_element_bandwidth(unit,mg1_fq,kbits_sec_fq);
    if (rv != BCM_E_NONE) {
        printf("rv is: %d, (%s) \n",rv,bcm_errmsg(rv));
        return rv;
    }
    
    kbits_sec_fq = 2000000;
    rv = set_scheduler_element_bandwidth(unit,mg2_fq,kbits_sec_fq);
    if (rv != BCM_E_NONE) {
        printf("rv is: %d, (%s) \n",rv,bcm_errmsg(rv));
        return rv;
    }
     
    /* Start Hirarchey */
    /* Replace HR class 0 mode to */
    BCM_GPORT_LOCAL_SET(local_gport, destination_local_port);
    rv = bcm_cosq_gport_add(unit,local_gport,1,BCM_COSQ_GPORT_SCHEDULER
                             | BCM_COSQ_GPORT_WITH_ID | BCM_COSQ_GPORT_SCHEDULER_HR_SINGLE_WFQ
                             | BCM_COSQ_GPORT_REPLACE,&hr_classes[0]);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Connect and Attach 2 FQ to SP0 of HR class 0 */  
    rv = connect_scheduler_element(unit,mg1_fq,BCM_COSQ_SP0);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = connect_scheduler_element(unit,mg2_fq,BCM_COSQ_SP0);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = attach_scheduler_element(unit,hr_classes[0],mg1_fq,0);    
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    rv = attach_scheduler_element(unit,hr_classes[0],mg2_fq,0);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
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
    rv = create_voq_connector(unit, mapping_info.core, num_cos_level, destination_local_port, &mg1_voq_connector);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }   

    rv = create_voq_connector(unit, mapping_info.core, num_cos_level, destination_local_port, &mg2_voq_connector);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    } 
    
    rv = create_voq_connector(unit, mapping_info.core, num_cos_level, destination_local_port, &mg3_voq_connector);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }   

    /* 2. Connect VOQ connector */    
    for (index = 0; index < num_cos_level; index++) {
        mode = BCM_COSQ_SP0;
        weight = 0;
        voq_connector_tc = index;
        rv = connect_voq_connector(unit,mg1_voq_connector,voq_connector_tc,mode,weight);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }   
        rv = connect_voq_connector(unit,mg2_voq_connector,voq_connector_tc,mode,weight);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }  
        
        rv = connect_voq_connector(unit,mg3_voq_connector,voq_connector_tc,mode,weight);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }   
    }
    
    /* 3. Attach VOQ connector */
    for (index = 0; index < num_cos_level; index++) {
        voq_connector_tc = index;
        rv = attach_scheduler_element(unit,mg1_fq,mg1_voq_connector,voq_connector_tc);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }   
        rv = attach_scheduler_element(unit,mg2_fq,mg2_voq_connector,voq_connector_tc);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        } 
        
        rv = attach_scheduler_element(unit,hr_classes[1],mg3_voq_connector,voq_connector_tc);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }   
    }

    /* 4. Back connecting VOQ connector to FMQ */    
    /* Since FMQ E2E this configuration is local, remote modid is exactly as mymodid */
    connect_info.remote_modid = my_modid + mapping_info.core;
    connect_info.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;    
    BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(connect_info.voq, mapping_info.core, base_queues[0]);
    connect_info.voq_connector = mg1_voq_connector;
    rv = bcm_cosq_gport_connection_set(unit,&connect_info);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    connect_info.voq_connector = mg2_voq_connector;    
    BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(connect_info.voq, mapping_info.core, base_queues[1]);
    rv = bcm_cosq_gport_connection_set(unit,&connect_info);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    connect_info.voq_connector = mg3_voq_connector;    
    BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(connect_info.voq, mapping_info.core, base_queues[2]);
    rv = bcm_cosq_gport_connection_set(unit,&connect_info);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
     
    /* At the Ingress, Do the follows */
    /* 1. Connect the FMQ to the VOQ-Connector */
    /* Since FMQ E2E configuration is local, remote modid is exactly as mymodid */
    connect_info.remote_modid = my_modid + mapping_info.core;
    connect_info.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;    
    BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(connect_info.voq, mapping_info.core, base_queues[0]);
    connect_info.voq_connector = mg1_voq_connector;
    rv = bcm_cosq_gport_connection_set(unit,&connect_info);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(connect_info.voq, mapping_info.core, base_queues[1]);
    connect_info.voq_connector = mg2_voq_connector;
    rv = bcm_cosq_gport_connection_set(unit,&connect_info);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(connect_info.voq, mapping_info.core, base_queues[2]);
    connect_info.voq_connector = mg3_voq_connector;
    rv = bcm_cosq_gport_connection_set(unit,&connect_info);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /*
     * Create 3 ingress MC groups, 
     * ID 1 => FMQ 4-7, 
     * ID 2 => FMQ 8-11, 
     * ID 3 => FMQ 12-14. 
     * All multicast groups points to the same multicast fabric (same cud) 
     */
    {
        bcm_gport_t gport;
        bcm_gport_t dest_port;
        uint32 flags = 0;
        bcm_multicast_replication_t replication_array;

        BCM_GPORT_LOCAL_SET(dest_port, destination_local_port);

        if (is_dnx) {
            flags = BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID;
            rv = bcm_multicast_create(unit, flags, &multicast_fabric_1);
            if (rv != BCM_E_NONE) {
                print rv;
                printf("bcm_multicast_create failed (%s) \n",bcm_errmsg(rv));
                return rv;
            }
            flags = BCM_MULTICAST_EGRESS_GROUP;
            replication_array.port = dest_port;
            replication_array.encap1 = 0;

            rv = bcm_multicast_add(unit, multicast_fabric_1, flags, 1, replication_array);
            if (rv != BCM_E_NONE) {
                print rv;
                printf("bcm_multicast_add failed (%s) \n",bcm_errmsg(rv));
                return rv;
            }
            
            if (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)) {
                bcm_module_t mreps[1];
                mreps[0] = mapping_info.core;
                rv = bcm_fabric_multicast_set(unit, multicast_fabric_1, 0, 1, mreps);
                if (rv != BCM_E_NONE) {
                    print rv;
                    printf("bcm_multicast_create failed (%s) \n",bcm_errmsg(rv));
                    return rv;
                }
            }
        } else {
            rv = bcm_multicast_egress_add(unit, multicast_fabric_1, dest_port, 0);
            if (rv != BCM_E_NONE) {
                print rv;
                printf("(%s) \n",bcm_errmsg(rv));
                return rv;
            }
        }
        int cud = multicast_fabric_1;

        for (index = 0; index < 3; index++) {

            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
            if (!is_dnx || start_of_multicast_ingress != 1) {
                /**
                 * MC ID 1 is created by default for Jericho2
                 */
                rv = bcm_multicast_create(unit, flags, &start_of_multicast_ingress);
                if (rv != BCM_E_NONE) {
                    print rv;
                    printf("bcm_multicast_create failed (%s) \n",bcm_errmsg(rv));
                    return rv;
                }
            }
            BCM_GPORT_MCAST_QUEUE_GROUP_SET(gport, base_queues[index]);

            if (is_dnx) {
                flags = BCM_MULTICAST_INGRESS_GROUP;
                replication_array.port = gport;
                replication_array.encap1 = cud;
                rv = bcm_multicast_add(unit, start_of_multicast_ingress, flags, 1, replication_array);
                if (rv != BCM_E_NONE) {
                    print rv;
                    printf("bcm_multicast_add failed (%s) \n",bcm_errmsg(rv));
                    return rv;
                }
            } else {
                rv = bcm_multicast_ingress_add(unit, start_of_multicast_ingress, gport, cud);
                if (rv != BCM_E_NONE) {
                    print rv;
                    printf("(%s) \n",bcm_errmsg(rv));
                    return rv;
                }
            }
    
            start_of_multicast_ingress++;
        }        
    }

    printf("cint_enhance_application_petra.c completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}

int dnx_delete_voq_cb(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t gport,
    void *user_data)
{
    bcm_cosq_gport_connection_t connection;
    int rv = 0;

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) || BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        /** first disconnect voq from its connector */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.voq = gport;
        connection.remote_modid = 0;
        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connection.voq_connector, 0, 0);

        rv = bcm_cosq_gport_connection_set(unit, &connection);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }

        /** delete the voq */
        rv = bcm_cosq_gport_delete(unit, gport);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

int dnx_delete_voq_connector_cb(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t gport,
    void *user_data)
{
    bcm_cosq_gport_connection_t connection;
    int rv = 0;
    int cosq;

    if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport))
    {
        /** first disconnect voq connector from its voq */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.voq_connector = gport;
        connection.remote_modid = 0;
        BCM_GPORT_UCAST_QUEUE_GROUP_SET(connection.voq, 0);

        rv = bcm_cosq_gport_connection_set(unit, &connection);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }

        /** detach connector from HR */
        for(cosq = 0; cosq < 8; ++cosq)
        {
            bcm_gport_t parent_port;
            rv = bcm_cosq_gport_parent_get(unit, gport, cosq, &parent_port);
            if (rv != BCM_E_NONE)
            {
                printf("(%s) \n",bcm_errmsg(rv));
                return rv;
            }

            rv = bcm_cosq_gport_detach(unit, parent_port, gport, cosq);
            if (rv != BCM_E_NONE)
            {
                printf("(%s) \n",bcm_errmsg(rv));
                return rv;
            }
        }


        /** delete the voq connector */
        rv = bcm_cosq_gport_delete(unit, gport);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

int dnx_delete_e2e(int unit)
{
    int rv = 0;

    /** Delete all existing VOQs */
    rv = bcm_cosq_gport_traverse(unit, dnx_delete_voq_cb, (void*)0);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /** Delete all existing VOQ Connectors */
    rv = bcm_cosq_gport_traverse(unit, dnx_delete_voq_connector_cb, (void*)0);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*start_enhance_application(unit, destination_local_port);*/

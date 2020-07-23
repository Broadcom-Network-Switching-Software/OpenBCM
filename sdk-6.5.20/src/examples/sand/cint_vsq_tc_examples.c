/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~VSQ Traffic Class~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_vsq_tc_examples.c
 * Purpose: Exampe of how to configure WRED and Tail Drop for specific VSQs
 * 
 * TM example script.
 * The focus of the CINT is to show how to enable a tail drop and WRED settings using this property.
 * VSQ used here are from vsq group: traffic class (CTTC).
 * In this example we try to provide how to use VSQ traffic class
 * to be a property of a queue, such that a VSQ realte to a specific queues.
 * 
 * In the tail drop example, we show a typical example to signify the priority of the packets in
 * respective VSQ queues such that, higher priority have higher tail drop maximum size.
 * 
 * In the wred example, we show how to run a wred application using a vsq property.
 * 
 * There are 2 main examples in this cint script:
 *  1.  WRED - configuring VSQ WRED.
 *      Enterance point: run_wred_example
 *  2.  Tail Drop - configuring Max Queue size for 2 different VSQs from category Traffic Class.
 *      Enterance point: run_tail_drop_example
 * 
 * Application Sequence:
 *  1.  Set VSQ category ranges by settings queues type ranges.
 *  2.  Create a VSQ-CTTC.
 *  3.  Assign VOQs to the VSQ.
 *  4.  Set tail drop/wred settings to be triggered by the VSQ.
 *  5.  Enable admission test of the traffic class category.
 * 
 *  The application assumes that queue/port mappings are configured before running the examples.
 *  Current parameters defined are according to diag_init application.
 * 
 *  Tail drop and WRED settings are also provided in the ingress traffic management section.
 */


/* Utils functions */

enum device_type_t { 
    DEVICE_TYPE_ARAD=0,
    DEVICE_TYPE_JERICHO=1,
    DEVICE_TYPE_QAX=2,
    DEVICE_TYPE_QUX=3,
    DEVICE_TYPE_DNX=4
};

device_type_t device_type = -1;

/*
 * drop_threshols variable allows external code to determine drop thresholds 
 */
uint32 drop_threshols[3] = {0};

/*
 * Get device type
 */
int get_device_type(int unit, device_type_t *device_type)
{
    bcm_info_t info;
    int is_dnx;

    int rv = bcm_info_get(unit, &info);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_info_get, rv=%d\n", rv);
        return rv;
    }

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }
    if (is_dnx) {
        *device_type = DEVICE_TYPE_DNX;
    }
    else {
        if ( info.device == 0x8270 ) {
            *device_type = DEVICE_TYPE_QUX;
        } 
        else if ( info.device == 0x8470 ) {
            *device_type = DEVICE_TYPE_QAX;
        } else if ( (info.device == 0x8675) || (info.device == 0x8375)|| (info.device == 0x8680) ) {
            *device_type = DEVICE_TYPE_JERICHO;
        } else {
            *device_type = DEVICE_TYPE_ARAD;
        }
    }

    return rv;
}



/* 
 * Map incoming TC
 * Use this to map the packet TC to a new TC 
 */ 
int set_global_tc_mapping(int unit,int packet_tc,int new_tc)
{
    bcm_error_t rv = BCM_E_NONE;
    
    printf("Set Global tc mapping unit %d, packet_tc %d, new_tc %d \n",
        unit,packet_tc,new_tc);
    
    rv = bcm_cosq_mapping_set(unit,packet_tc,new_tc);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_mapping_set() failed $rv\n");
        return rv;
    }

    return rv;
}

/*
 * Set WRED configuration
 */
int set_wred_settings(int unit, bcm_gport_t gport, int is_enable, int drop_precedence, int max_thresh, int min_thresh, int max_prob, int gain)
{
    bcm_error_t rv = BCM_E_NONE;
   
    printf("Set WRED Settings unit %d, gport 0x%x, is_enable %d, drop index %d, max thresh %d, min thresh %d, max prob %d gain %d \n",
        unit,gport,is_enable,drop_precedence,max_thresh,min_thresh,max_prob,gain);

    bcm_cosq_gport_discard_t discard;
    bcm_cosq_gport_discard_t_init(&discard);
   
   
    /* Fill the bcm_cosq_gport_discard_t struct with the requierd data */
    discard.flags = BCM_COSQ_DISCARD_BYTES;
    if (is_enable == 1)
        discard.flags |= BCM_COSQ_DISCARD_ENABLE;
   
    switch (drop_precedence) {
        case 0:
            discard.flags |= BCM_COSQ_DISCARD_COLOR_GREEN;
            break;
        case 1:
            discard.flags |= BCM_COSQ_DISCARD_COLOR_YELLOW;
            break;
        case 2:
            discard.flags |= BCM_COSQ_DISCARD_COLOR_RED;
            break;
        case 3:
            discard.flags |= BCM_COSQ_DISCARD_COLOR_BLACK;
            break;
        default:
            discard.flags |= BCM_COSQ_DISCARD_COLOR_ALL;
    }
   
    discard.min_thresh = min_thresh;
    discard.max_thresh = max_thresh;
    discard.drop_probability = max_prob;
    discard.gain = gain;
    /* For VSQ gport , cosq parameter always 0 */    
    rv = bcm_cosq_gport_discard_set(unit,gport,0,&discard);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_discard_set() failed $rv\n");
        return rv;
    }

    return rv;
}

/*
 * Set Tail Drop setting for a certain queue. 
 * If DP == -1 , set tail drop over all DPs 
 */
int set_vsq_tail_drop_settings(int unit, bcm_gport_t gport, uint32 flags, int dp, uint32 max_size)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_size_t gport_size;
    int cosq = 0;

    printf("Set Tail drop Settings unit %d, gport 0x%x, flags 0x%x, max_size %d \n",
        unit, gport, flags, max_size);

    if (dp == -1) {
        flags |= BCM_COSQ_GPORT_SIZE_COLOR_BLIND;
        dp = 0;
    }

    gport_size.size_max = max_size; 
    if (device_type >= DEVICE_TYPE_QAX) 
    {
        /* set FADT drop to behave like tail drop */
        gport_size.size_alpha_max = 0;
        gport_size.size_fadt_min = max_size;

    }

    rv = bcm_cosq_gport_color_size_set(unit,gport,cosq,dp,flags,&gport_size);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_gport_color_size_set() failed $rv\n");
        printf("gport: (0x%08x), cosq: %d, dp: %d, flags 0x%08x, gport_size: %u\n",
            gport,cosq,dp,flags);
        return rv;
    }

    return rv;
}

/* Set OFP Bandwidth (max KB per sec)
 * Configure the Port Shaper's max rate
 */
int set_ofp_bandwidth(int unit, int local_port_id, int max_kbits_sec){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;

    int min_kbits_rate = 0;
    int flags = 0;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport, local_port_id); 

    gport_type = bcmCosqGportTypeLocalPort; 

    rv = bcm_cosq_gport_handle_get(unit, gport_type, &gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }

    out_gport = gport_info.out_gport;
    rv = bcm_cosq_gport_bandwidth_set(unit, out_gport, 0, min_kbits_rate, max_kbits_sec, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, in bandwidth set, out_gport $out_gport max_kbits_sec $max_kbits_sec \n");
        return rv;
    }

    return rv;
}

/* 
 *  WRED Example
 *  This example assumes the port is oversubscriped , for example by port's shaper.
 *  This example enables WRED.
 *  
 *  The process: 
 *  1. Set category ranges. 
 *  2. Create VSQ gport: 1. VSQ Category 2 TC 0 
 *  3. Assign VOQ (base_queue parameter + cosq 0) to VSQ
 *  4. Set wred settings.
 *  5. Set admission tests.
 *  6. Set tail drop settings such that it will not take affect on the application. 
 *  7. Run traffic to base_queue where traffic class is set to 0 (cosq 0). 
 *  
 *  Expectation: to see that the latency is low from just over subscription traffic without the application.
 *  
 *  Example:
 *  Run to destination port 2. TC 0.
 *  Example run: run_wred_example(unit,2,8);
 *  
 *  Note: incoming base_queue parameter should be aligned with the destination port id. 
 *  For example, when using diag init application, destination port id 2 is aligned with VOQ ID 8. 
 *  Assumptions: destination_port_id exists, base_queue is created and connected to destination_port_id. 
 */
int run_wred_example(int unit,int destination_local_port, int base_queue)
{

    return wred_generic_example(unit, destination_local_port, base_queue, BCM_COSQ_VSQ_CTTC, 0, 0, 0, NULL, NULL);
}

/* 
 *  WRED Example
 *  This example assumes the port is oversubscriped , for example by port's shaper.
 *  This example enables WRED.
 *  
 *  The process: 
 *  1. Set category ranges. 
 *  2. Create VSQ gport according to flags, traffic class and connection class
 *  3. Assign VOQ (base_queue parameter + cosq internal_pri) to VSQ
 *  4. Set wred settings.
 *  5. Set admission tests.
 *  6. Set tail drop settings such that it will not take affect on the application. 
 *  7. Run traffic to base_queue where traffic class is set to 0 (cosq 0). 
 *  
 *  Note: incoming base_queue parameter should be aligned with the destination port id. 
 *  For example, when using diag init application, destination port id 2 is aligned with VOQ ID 8. 
 *  Assumptions: destination_port_id exists, base_queue is created and connected to destination_port_id. 
 */
int wred_generic_example(int unit,int destination_local_port, int base_queue, uint32 vsq_flags,
                         int traffic_class, int connection_class, int internal_pri, 
                         bcm_gport_t* voq_gport, bcm_gport_t* vsq_gport)
{
    bcm_error_t rv;
    bcm_gport_t vsqA,voq;
    bcm_cosq_vsq_info_t vsq_info;  
    int admission_test;
    uint32 flags;
    int nof_rsrc = 0;
    uint32 rsrc_flags[3] = {0};
    int dp = 0;
    int i = 0;

    rv = get_device_type(unit, &device_type);
    if (BCM_FAILURE(rv)) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    if (device_type != DEVICE_TYPE_DNX) {
        /* not supported for Jericho2 and above*/

        /* 1. Set vsq categories */
        rv = bcm_fabric_control_set(unit,bcmFabricVsqCategory,bcmFabricVsqCatagoryMode2);
        if (rv != BCM_E_NONE) {
            printf("error in bcm_fabric_control_set type: bcmFabricVsqCategory\n");
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_fabric_control_set(unit,bcmFabricQueueMin,0);
        if (rv != BCM_E_NONE) {
            printf("error in bcm_fabric_control_set type: bcmFabricQueueMin\n");
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_fabric_control_set(unit,bcmFabricQueueMax,(device_type == DEVICE_TYPE_QUX)?16383:32767);
        if (rv != BCM_E_NONE) {
            printf("error in bcm_fabric_control_set type: bcmFabricQueueMax\n");
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
    
    /* 2. Create VSQs gport with category=2, traffic class=traffic_class */
    vsq_info.flags = vsq_flags;
    vsq_info.ct_id = 2; /** Unicast */
    vsq_info.core_id = BCM_CORE_ALL;
    vsq_info.traffic_class = traffic_class;
    vsq_info.cc_id = connection_class;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_info,&vsqA);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_cosq_gport_vsq_create\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    /* 3. Assign VOQ queue to VSQA, queue should be related to base_queue */  
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq,base_queue);
    
    rv = bcm_cosq_gport_vsq_add(unit,vsqA,voq,internal_pri);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_cosq_gport_vsq_add\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    /* WRED parameters */
    int is_enable = 1;
    int drop_precedence = -1;
    int max_thresh = (device_type == DEVICE_TYPE_DNX ? 256 :1);
    int min_thresh = 0;
    int max_prob = 100;
    int gain = 1;
    int cosq = 0;
    int max_queue_size;
    int max_queue_size_bds;
    uint32* queue_size_ptr;
    
    rv = set_wred_settings(unit,vsqA,is_enable,drop_precedence,max_thresh,min_thresh,max_prob,gain);    
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }  
  
    if (device_type < DEVICE_TYPE_QAX) {
        /* not supported for QAX and above */
        /* 4. Set enable for admission test profile A,B */
        admission_test = BCM_COSQ_CONTROL_ADMISSION_CTTC;
        rv = bcm_cosq_control_set(unit,0,0,bcmCosqControlAdmissionTestProfileA,admission_test);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }  
        rv = bcm_cosq_control_set(unit,0,0,bcmCosqControlAdmissionTestProfileB,admission_test);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
   
    /* 5. disable tail drop settings (by set them to high thresholds) so it will be affected by wred only. */
    if (device_type >= DEVICE_TYPE_QAX) { /* qax and above */
        nof_rsrc = 3;
        rsrc_flags[0] = BCM_COSQ_GPORT_SIZE_BYTES;

        rsrc_flags[1] = BCM_COSQ_GPORT_SIZE_SRAM | (device_type == DEVICE_TYPE_DNX ? BCM_COSQ_GPORT_SIZE_BUFFERS : BCM_COSQ_GPORT_SIZE_BUFFER_DESC);
        rsrc_flags[2] = BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC;
    } else if (device_type == DEVICE_TYPE_JERICHO) { 
        nof_rsrc = 1;
        rsrc_flags[0] = BCM_COSQ_GPORT_SIZE_BUFFER_DESC;
    } else {
        nof_rsrc = 2;
        rsrc_flags[0] = BCM_COSQ_GPORT_SIZE_BYTES;
        rsrc_flags[1] = BCM_COSQ_GPORT_SIZE_BUFFER_DESC;
    }

    dp = -1;
    for (i = 0; i < nof_rsrc; ++i) {
        flags = rsrc_flags[i];

        /*
         * Max queue size should be set to maximal resource size for specific resource.
         * Since this number is device specific,
         * it is obtained from internal SDK database.
         * Customer code should replace this by a specific number.
         */
        if (device_type == DEVICE_TYPE_DNX) {
            queue_size_ptr = dnxc_data_1d_get(unit, "ingr_congestion", "info", "resource", "max", i);
            max_queue_size = *queue_size_ptr;
        }
        else {
            max_queue_size = 1000000;
        }
        rv = set_vsq_tail_drop_settings(unit,vsqA,flags,drop_precedence,max_queue_size);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }   
    }

    /** set output gport parameters */
    if (voq_gport)
    {
        *voq_gport = voq;
    }
    if (vsq_gport)
    {
        *vsq_gport = vsqA;
    }

    /* 6. Run traffic */
    return rv;
}

/* 
 * Tail Drop Example
 * This example sets Tail Drop per Vsq, and configure the TC mapping. 
 * Also sets the port's shaper (to create over subscription)  
 * The example sets for specific queues different VSQ IDs such that: 
 * base_queue + cosq0-1 => VSQ A.
 * base_queue + cosq2-3 => VSQ B. 
 *
 * The process: 
 * 1. Set category ranges example. Have only two categories (Mode2). 
 * 2. Create 2 VSQ gports: 1. VSQ Category 0 TC 0 (VSQ A) . 2. VSQ Category 0 TC 2 (VSQ B). 
 * 3. Assign VOQ queues based upon base_queue + cosq 0-1 to VSQ A and cosq 2-3 to VSQ B. 
 * 4. Set tail drop settings. 
 * 5. Set admission test. 
 * 6. Set global traffic class such that going to cosq x is done by running TC x*2 or x*2+1. 
 * 6. Run traffic to destination port id that is accordingly to base_queue. 
 *
 * Expectation: to see that the queue maximum size is set accordingly. In this example : we configure the maximum size 
 * for VSQ A to 1000 Bytes, VSQ B to 1000 Bytes. 
 * Run traffic to base_queue using 2 different streams: 
 * one stream that the packet assign to the TM with traffic class 0
 * one stream that the packet assign to the TM with traffic class 4
 *
 * Results: 
 * Running without the application, over subscription configuration, will show that the stream with TC 0 will get higher priority. 
 * Running with the application, over subscription configuration, will show that the streams will get almost equally priority since 
 * they have low  max.queue sizes (where tc 4 will get less more than tc 0). 
 *
 * Example run: run_tail_drop_example(unit,2,8); 
 *
 * Note: incoming base_queue parameter should be aligned with the destination port id. 
 * For example, when using diag init application, destination port id 2 is aligned with VOQ ID 8. 
 * Assumptions: destination_port_id exists, base_queue is created and connected to destination_port_id. 
 */
int run_tail_drop_example(int unit,int destination_port_id,int base_queue)
{
    bcm_error_t rv = BCM_E_NONE;  
    uint32 max_queue_size;
    int dp;
    int index;
    bcm_gport_t vsqA, vsqB, voq;
    bcm_cosq_vsq_info_t vsq_info;
    int admission_test;  
    uint32 flags = 0;
    int nof_rsrc = 0;
    uint32 rsrc_flags[3] = {0};
    uint32 rsrc_max_queue_size[3] = {0};
    int i = 0;

    rv = get_device_type(unit, &device_type);
    if (BCM_FAILURE(rv)) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }
    
    if (device_type != DEVICE_TYPE_DNX) {
        /* not supported for Jericho2 and above */
        /* 1. Set category ranges: this is done by: declaring a vsq category mode and ranges */
        rv = bcm_fabric_control_set(unit,bcmFabricVsqCategory,bcmFabricVsqCatagoryMode2);
        if (rv != BCM_E_NONE) {
            printf("error in bcm_fabric_control_set type: bcmFabricVsqCategory\n");
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_fabric_control_set(unit,bcmFabricQueueMin,0);
        if (rv != BCM_E_NONE) {
            printf("error in bcm_fabric_control_set type: bcmFabricQueueMin\n");
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_fabric_control_set(unit,bcmFabricQueueMax,(device_type == DEVICE_TYPE_QUX)?16383:32767);
        if (rv != BCM_E_NONE) {
            printf("error in bcm_fabric_control_set type: bcmFabricQueueMax\n");
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
    
    /* 2. Create 2 VSQ gport: 1. VSQ Category 0 TC 0 (VSQ A) . 2. VSQ Category 0 TC 2 (VSQ B). */
    vsq_info.flags = BCM_COSQ_VSQ_CTTC;
    vsq_info.ct_id = 2;
    vsq_info.traffic_class = 0;
    vsq_info.core_id = BCM_CORE_ALL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_info,&vsqA);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_cosq_gport_vsq_create\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    vsq_info.traffic_class = 2;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_info,&vsqB);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_cosq_gport_vsq_create\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    /* 3. Assign VOQ base queue + cosq:0,1 to VSQ A and queue base + cosq:2,3 to VSQ B. */  
    /* VSQ A is configured for Traffic class: 0-1. VSQ B is configured for Traffic class: 2-3. */
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq,base_queue);
    
    rv = bcm_cosq_gport_vsq_add(unit,vsqA,voq,0);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_cosq_gport_vsq_add\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_cosq_gport_vsq_add(unit,vsqA,voq,1);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_cosq_gport_vsq_add\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    rv = bcm_cosq_gport_vsq_add(unit,vsqB,voq,2);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_cosq_gport_vsq_add\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_cosq_gport_vsq_add(unit,vsqB,voq,3);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_cosq_gport_vsq_add\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    /* 4. Set tail drop settings. */
    /* An example of tail drop settings */
    /* vsqA will have more packets dropped than vsqB (TC 2/3) */
	
    /*This test used to fail due to instant zero queue size.
     *The maximal queue size was increased to handle this issue.
     *The queue size could be increased further if required.
     */
    if (device_type == DEVICE_TYPE_DNX) { 
        nof_rsrc = 3;
        rsrc_flags[0] = BCM_COSQ_GPORT_SIZE_BYTES;
        rsrc_flags[1] = BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BUFFERS;
        rsrc_flags[2] = BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC;
        for (i = 0; i < nof_rsrc; i++)
        {
            /** in DNX tests - the max size is set externally */
            rsrc_max_queue_size[i] = drop_threshols[i];
        }

    } 
    else if (device_type >= DEVICE_TYPE_QAX) {  /* qax and above */
        nof_rsrc = 3;
        rsrc_flags[0] = BCM_COSQ_GPORT_SIZE_BYTES;
        rsrc_flags[1] = BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BUFFER_DESC;
        rsrc_flags[2] = BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC;
        rsrc_max_queue_size[0] = 10*1024;
        rsrc_max_queue_size[1] = 32;
        rsrc_max_queue_size[2] = 64;
    } 

    else if (device_type == DEVICE_TYPE_JERICHO) { 
        nof_rsrc = 1;
        rsrc_flags[0] = BCM_COSQ_GPORT_SIZE_BUFFER_DESC;
        rsrc_max_queue_size[0] = 10;
    } else {
        nof_rsrc = 2;
        rsrc_flags[0] = BCM_COSQ_GPORT_SIZE_BYTES;
        rsrc_flags[1] = BCM_COSQ_GPORT_SIZE_BUFFER_DESC;
        rsrc_max_queue_size[0] = 8000;
        rsrc_max_queue_size[1] = 160;
    }

    dp = -1;

    for (i = 0; i < nof_rsrc; ++i) {
        flags = rsrc_flags[i];
        max_queue_size = rsrc_max_queue_size[i];
        rv = set_vsq_tail_drop_settings(unit, vsqA, flags, dp, max_queue_size);
        if (BCM_FAILURE(rv)) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }            

        max_queue_size *= 2;
        rv = set_vsq_tail_drop_settings(unit, vsqB, flags, dp, max_queue_size);
        if (BCM_FAILURE(rv)) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }            
    }
    
    if (device_type < DEVICE_TYPE_QAX) {
        /* not supported for QAX and above */

        /* 5. Set enable for admission test profile A,B */
        admission_test = BCM_COSQ_CONTROL_ADMISSION_CTTC;
        rv = bcm_cosq_control_set(unit,0,0,bcmCosqControlAdmissionTestProfileA,admission_test);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }  
        rv = bcm_cosq_control_set(unit,0,0,bcmCosqControlAdmissionTestProfileB,admission_test);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    /* 6. Run traffic to queues */

    return rv;
}

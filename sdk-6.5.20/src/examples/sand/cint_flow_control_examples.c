/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * File: cint_flow_control_examples.c
 * Purpose: Examples of flow control generation and receive for JR1, QAX, QUX, JR2
 * Example include:
 *        LLFC,PFC receive
 *        LLFC VSQ -> LLFC generation
 *        PFC VSQ -> PFC generation
 *        Global resource -> LLFC generation
 *        Global resource -> PFC generation
 *        Global resource -> OOB generation
 *        VSQA~D -> OOB generation
 *        CTTC VSQ -> PFC generation
 *        OOB -> interface receive
 *        OOB -> port receive
 *        OOB -> port+cosq receive
 *        OOB -> bitmap receive
 *        NIF almost full -> LLFC generation
 *        NIF almost full -> PFC generation
 *        NIF almost full -> OOB generation
 *        Global resource threshold setting
 * 
 * Configuration Examples:
 * 1. PFC VSQ -> PFC generation
 *                      10G              Force forward
 *          Ixia port ======> Port 13  ================> Port 13(shaping)
 *    Run:
 *        BCM> cint cint_flow_control_examples.c
 *        BCM> cint
 *        cint> int rv;
 *        cint> int unit=0;
 *        cint> int port=13;
 *        cint> int flags = BCM_COSQ_PFC_BYTES;
 *        cint> rv = cint_test_fc_gen_vsqf_pfc_set_example(unit,port,flags);
 *    Then sending traffic to port 13, PFC will be generated.
 *
 * 2. Global resource -> OOB/ILKN Inband generation
 *  OOB:
 *                10G                            Loopback                1G(10%)
 *     Ixia ======> port 13 ======> port X ========> port X ======> port 13 ======> Ixia
 *        cint> int rv;
 *        cint> int unit=0;
 *        cint> int port=13;
 *        cint> int oob_port_tx=0;
 *        cint> int oob_port_rx=0;
 *        cint> int glb_src_p0_byte=13;
 *        cint> int calander_index=0;
 *        cint> int priority=0;
 *        cint> int global_thr =0xFFFFFE00;
 *        cint> rv = cint_test_fc_gen_gl_to_cal_set_example(unit,glb_src_p0_byte,oob_port_tx,calander_index,priority,global_thr,1);
 *        cint> rv = cint_test_fc_rcv_cal_to_intf_set_example(unit,port,oob_port_rx,calander_index,1);
 *     Then sending traffic to port 13, FC will be generated to OOB
 *     OOB receive target is interface, which will be stopped from sending traffic
 *
 *  ILKN Inband(For Q2A above devices):
 *            10G                               Loopback
 *     Ixia ======> port 13 ======> ILKN port X ========> ILKN port X ======> port 13 ======> Ixia
 *        cint> int rv;
 *        cint> int unit=0;
 *        cint> int port=13;
 *        cint> int IXIA_PORT=5;
 *        cint> int glb_src_ocb_db=3;
 *        cint> int calander_index=0;
 *        cint> int priority=0;
 *        cint> int global_thr =0x7fff;
 *        cint> rv = cint_test_fc_gen_gl_to_cal_set_example(unit,glb_src_ocb_db,IXIA_PORT,calander_index,priority,global_thr,0);
 *        cint> rv = cint_test_fc_rcv_cal_to_intf_set_example(unit,port,IXIA_PORT,calander_index,0);
 *     Then sending traffic to port 13, FC will be generated to ILKN Inband calendar 0
 *     ILKN Inband receive target is interface, which will be stopped from sending traffic
 *
 * 3. VSQ A -> OOB generation
 *    For other VSQ types, the configuration is similar
 *                10G                            Loopback                 1G(10%)
 *          Ixia ======> port 13 ======> port X ========> port X ======> port 13 ======> Ixia
 *        cint> int rv;
 *        cint> int unit=0;
 *        cint> int port=13;
 *        cint> int oob_port_tx=0;
 *        cint> int oob_port_rx=0;
 *        cint> int calander_index=0;
 *        cint> int priority=0;
 *        cint> int flags=BCM_COSQ_PFC_BYTES;
 *        cint> int vsq_thr =0;
 *        cint> rv = cint_test_fc_gen_ct_oob_set_example(unit,oob_port_tx,calander_index,flags,vsq_thr);
 *        cint> rv = cint_test_fc_rcv_oob_intf_set_example(unit,port,oob_port_rx,calander_index);
 *     The set example creates fc path,and configures vsq threshold, and before setting threshold
 *     the default threshold is obtained and put to a global variable:VSQ_THRESHOLD
 *        cint> rv = cint_test_fc_gen_ct_oob_unset_example(unit,oob_port_tx,calander_index,flags);
 *     The unset example is called to delete fc path and reset the threshold to default using global variable:VSQ_THRESHOLD
 *     The unset example can't be called solely, it's always in pair with set example
 *     And after set example, an unset example has to be called, otherwise the fc path and threshold is not reset
 *
 *     Verification procedure:
 *         Sending traffic to port 13, FC will be generated to OOB
 *         OOB receive target is interface, which will be stopped from sending traffic
 *         After unset, traffic will resume
 */

/* Global resource type */
enum global_vsq_src_e {
    global_vsq_src_bdb,     /* For Jericho,QAX,JR2 */
    global_vsq_src_mini_db, /* For Jericho */
    global_vsq_src_full_db, /* For Jericho */
    global_vsq_src_ocb_db,  /* For Jericho,QAX,JR2 */
    global_vsq_src_mix_pool0, /* For Jericho */
    global_vsq_src_mix_pool1, /* For Jericho */
    global_vsq_src_ocb_pool0, /* For Jericho */
    global_vsq_src_ocb_pool1, /* For Jericho */
    global_vsq_src_ocb_pdb, /* For QAX,JR2*/
    global_vsq_src_p0_db,   /* For QAX,JR2*/
    global_vsq_src_p1_db,   /* For QAX,JR2*/
    global_vsq_src_p0_pd,   /* For QAX,JR2*/
    global_vsq_src_p1_pd,   /* For QAX,JR2*/
    global_vsq_src_p0_byte, /* For QAX,JR2*/
    global_vsq_src_p1_byte, /* For QAX,JR2*/
    global_vsq_src_hdrm_db, /* For QAX,JR2*/
    global_vsq_src_hdrm_pd  /* For QAX,JR2*/
};


/* device type */
enum DEVICE_TYPE_E {
    DEVICE_TYPE_JERICHO,
    DEVICE_TYPE_QAX,
    DEVICE_TYPE_JR2,
    DEVICE_TYPE_Q2A
};

DEVICE_TYPE_E device_type;

/* Global vars */
int CT = 2; /* category */
int TC = 0; /* traffic class */
int CC = 0; /* connection class */
int ST_VSQ_ID = 0; /* VSQ D */
int TARGET_PORT_IMPLICIT = 0; /* indicate if using BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT or not */
int CORE = 0;
int IS_OCB_ONLY = 0;
int POOL_ID = 0; /* indicate which pool to use for src port VSQ or PG VSQ */
int IS_LOSSLESS = 0; /* indicate if using headroom resources */
/* Define a global VSQ threshold for API get */
bcm_cosq_pfc_config_t VSQ_THRESHOLD;
int NUM_COSQ=2; /* port_priorities, default is 2 */


/*   Get device type   */
int get_device_type(int unit, DEVICE_TYPE_E *device_type)
{
    bcm_info_t info;
    int device_id_masked;
    int device_id_masked_j2c;

    int rv = bcm_info_get(unit, &info);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_info_get, rv=%d\n", rv);
        return rv;
    }

    device_id_masked = (info.device & 0xfff0);
    device_id_masked_j2c = (info.device & 0xffc0);

    if ( (info.device == 0x8675) || (info.device == 0x8375)|| (info.device == 0x8680) ) {
        *device_type = DEVICE_TYPE_JERICHO;
    } else if ( (info.device == 0x8470) || (info.device == 0x8270) ) {
        *device_type = DEVICE_TYPE_QAX;
    } else if ((device_id_masked == 0x8480) || (device_id_masked == 0x8280)) {
        *device_type = DEVICE_TYPE_Q2A;
    } else if ( (device_id_masked == 0x8690) || (device_id_masked_j2c == 0x8800) || (device_id_masked == 0x8850) ) {
        *device_type = DEVICE_TYPE_JR2;
    } else {
        return BCM_E_PARAM;
    }
    return rv;
}

/*   Set egress port rate   */
int set_egg_bandwidth(int unit, int shaped_port, int rate)
{
    bcm_gport_t local_gport;

    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    return bcm_cosq_gport_bandwidth_set(unit, local_gport, 0, 0, rate, 0);
}

/*   Force forward source port to destination port   */
int set_force_forward(int unit, int src_port, int dst_port)
{
    return bcm_port_force_forward_set(unit, src_port, dst_port, 1);
}

/*    Enable LLFC receive    */
int cint_test_fc_rcv_llfc_set(int unit, int port)
{
    return bcm_port_control_set(unit, port, bcmPortControlLLFCReceive, 1);
}

/* This function configures LLFC VSQ to LLFC generation
 *   1. Enable LLFC transmission
 *   2. Create source port VSQ
 *   3. Set threshold to 0 so that any traffic will trigger LLFC generation
 *   Note: For threshold, on JR1, only BD is supported, flags should be set to 0
 *         On QAX and JR2, three types of threshold are supported, set by flags as below
 *         BCM_COSQ_PFC_BYTES
 *         BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_PACKET_DESC
 *         BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_BUFFERS
*/
int cint_test_fc_gen_vsqe_llfc_set_example(int unit, int source_port, int flags)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t local_gport;
    int thresh = 0;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Setting source port bcmPortControlLLFCTransmit enable*/
    rv = bcm_port_control_set(unit, source_port, bcmPortControlLLFCTransmit, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port LLFC transmit\n");
        return rv;
    }

    bcm_gport_t src_port_vsq_gport;
    bcm_gport_t pg_base_vsq_gport;
    bcm_cosq_src_vsqs_gport_config_t config;
    int src_port_vsq_index = 0;
    int pg_base = 0;
    int i = 0;

    /* Create VSQ-E and VSQ-F */
    config.flags = 0;
    config.numq = 8;
    for (i = 0; i < config.numq; i++) {
        config.pg_attributes[i].cosq_bitmap = (1<<i);
        config.pg_attributes[i].ocb_only    = IS_OCB_ONLY;
        config.pg_attributes[i].pool_id     = POOL_ID;
        config.pg_attributes[i].is_lossles  = IS_LOSSLESS;
    }
    
    rv = bcm_cosq_src_vsqs_gport_add(unit, source_port, &config, &src_port_vsq_gport, &pg_base_vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("Failed to create src vsqs\n");
        return rv;
    }

    /* Set FC threshold */
    flags = (device_type == DEVICE_TYPE_JERICHO) ? 0 : (flags);
    bcm_cosq_pfc_config_t threshold;
    /* In Q2A behavior of the thresholds is different. If they are set to 0 the signal transmitted 
     * to CFC in order to generate FC will never stop even if there is no traffic. In order to avoid
     * this we need to set the thresholds to the smallest value possible. This means to write 1 in the 
     * relevant registers, but the registers have granularity. The API automatically calculates what 
     * to write in the registers according to that granularity. For example for BCM_COSQ_PFC_BYTES 
     * the granularity is 256 words, so when we pass to the API value of 256 it will write 1 in the register.
     */
    if (device_type == DEVICE_TYPE_Q2A) {
        if (flags == BCM_COSQ_PFC_BYTES) {
            thresh=*(dnxc_data_get(unit,  "ingr_congestion", "info", "bytes_threshold_granularity", NULL));
        } else {
            thresh=*(dnxc_data_get(unit,  "ingr_congestion", "info", "threshold_granularity", NULL));
        }
        threshold.xoff_fadt_threshold.thresh_min = thresh;
        threshold.xoff_fadt_threshold.thresh_max = thresh;
        threshold.xoff_fadt_threshold.alpha = 0;
        threshold.xon_fadt_offset = 0;
    } else {
        threshold.xoff_fadt_threshold.thresh_min = 0;
        threshold.xoff_fadt_threshold.thresh_max = 0;
        threshold.xoff_fadt_threshold.alpha = 0;
        threshold.xon_fadt_offset = 0;
    }
    threshold.xoff_threshold_bd = 0;
    threshold.xon_threshold_bd = 0;
    rv = bcm_cosq_pfc_config_set(unit, src_port_vsq_gport, 0, flags, &threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/* This function configures LLFC VSQ to LLFC generation according to threshold 
 *   1. Enable LLFC transmission
 *   2. Create source port VSQ
 *   3. Set threshold to a specific value so that only threshold is crossed, then LLFC is generated
 *   Note: For threshold, on JR1, only BD is supported, flags should be set to 0
 *         On QAX and JR2, three types of threshold are supported, set by flags as below
 *         BCM_COSQ_PFC_BYTES
 *         BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_PACKET_DESC
 *         BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_BUFFERS
*/
int cint_test_fc_gen_vsqe_llfc_thr_set_example(int unit, int source_port, int flags, int vsq_thr)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t local_gport;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Setting source port bcmPortControlLLFCTransmit enable*/
    rv = bcm_port_control_set(unit, source_port, bcmPortControlLLFCTransmit, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port LLFC transmit\n");
        return rv;
    }

    bcm_gport_t src_port_vsq_gport;
    bcm_gport_t pg_base_vsq_gport;
    bcm_cosq_src_vsqs_gport_config_t config;
    int src_port_vsq_index = 0;
    int pg_base = 0;
    int i = 0;

    /* Create VSQ-E and VSQ-F */
    config.flags = 0;
    config.numq = 8;
    for (i = 0; i < config.numq; i++) {
        config.pg_attributes[i].cosq_bitmap = (1<<i);
        config.pg_attributes[i].ocb_only    = IS_OCB_ONLY;
        config.pg_attributes[i].pool_id     = POOL_ID;
        config.pg_attributes[i].is_lossles  = IS_LOSSLESS;
    }

    rv = bcm_cosq_src_vsqs_gport_add(unit, source_port, &config, &src_port_vsq_gport, &pg_base_vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("Failed to create src vsqs\n");
        return rv;
    }

    /* Set FC threshold */
    bcm_cosq_pfc_config_t threshold;
    /* For QAX, JR2, threshold use BYTEs/OCB PD/OCB DB */
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
        threshold.xoff_threshold_bd = vsq_thr;
        threshold.xon_threshold_bd = vsq_thr;
    } else {
        threshold.xoff_threshold = vsq_thr;
        threshold.xon_threshold = vsq_thr;
    }
    rv = bcm_cosq_pfc_config_set(unit, src_port_vsq_gport, 0, flags, &threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*    Enable PFC receive    */
int cint_test_fc_rcv_pfc_set(int unit, int port)
{
    int rv = BCM_E_NONE;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    rv = bcm_port_control_set(unit, port, bcmPortControlPFCReceive, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set PFC receive\n");
        return rv;
    }

    /* On JR2, PFC to NIF mapping isn't created on init, have to create manually */
    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        bcm_cosq_fc_endpoint_t fc_reception;
        bcm_cosq_fc_endpoint_t fc_target;
        int i;

        /* setting port */
        BCM_GPORT_LOCAL_SET(fc_target.port, port);
        BCM_GPORT_LOCAL_SET(fc_reception.port, port);
        fc_target.flags = fc_reception.flags = 0;
        fc_reception.calender_index = -1;

        /*
        * Setting source cosq equal to target cosq,
        * meaning if port_priorities = 8, then PFC 0 -> EGQ 0...PFC 7 -> EGQ 7
        */
        for (i = 0; i < NUM_COSQ; i++) {
            fc_target.cosq = i;
            fc_reception.cosq = i;
            rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
            if (rv != BCM_E_NONE) {
                printf("fc reception configuration failed(%d) in cint_test_fc_rcv_pfc_set\n", rv);
            }
        }
    }

    return rv;
}

/*  DNX only  Enable PFC receive to stop interface   */
int cint_test_fc_rcv_pfc_stop_if_set(int unit, int port, int source_pfc)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    if (device_type < DEVICE_TYPE_JR2) {
        printf("Error, cint example is only for DNX devices! \n");
        return BCM_E_FAIL;
    }

    rv = bcm_port_control_set(unit, port, bcmPortControlPFCReceive, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set PFC receive\n");
        return rv;
    }

    /** Setting reception path from PFC to Interface */
    BCM_GPORT_LOCAL_INTERFACE_SET(fc_target.port, port);
    fc_reception.calender_index = -1;
    fc_target.flags = 0;
    fc_target.cosq = -1;
    BCM_GPORT_LOCAL_SET(fc_reception.port, port);
    fc_reception.flags = 0;
    fc_reception.cosq = source_pfc;
    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_pfc_stop_if_set\n", rv);
    }

    return rv;
}

/*  DNX only  Enable PFC receive to stop interface   */
int cint_test_fc_rcv_pfc_stop_if_unset(int unit, int port, int source_pfc)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    if (device_type < DEVICE_TYPE_JR2) {
        printf("Error, cint example is only for DNX devices! \n");
        return BCM_E_FAIL;
    }

    rv = bcm_port_control_set(unit, port, bcmPortControlPFCReceive, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set PFC receive\n");
        return rv;
    }

    /** Setting reception path from PFC to Interface */
    BCM_GPORT_LOCAL_INTERFACE_SET(fc_target.port, port);
    fc_reception.calender_index = -1;
    fc_target.flags = 0;
    fc_target.cosq = -1;
    BCM_GPORT_LOCAL_SET(fc_reception.port, port);
    fc_reception.flags = 0;
    fc_reception.cosq = source_pfc;
    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_pfc_stop_if_set\n", rv);
    }

    return rv;
}

/* This function configures PFC to generic bitmap mapping
 * 3 flags are supported
 *    BCM_COSQ_FC_PORT_OVER_PFC
 *         additional targets are used as reaction points to the same FC reception source
 *         For example, if receive PFC1, then all target cosqs will react to it.
 *         Note: target_cosq has to be configured to -1
 *    BCM_COSQ_FC_INHERIT_UP
 *         can be used to set the same path for cosqs above the given cosq
 *         For example, receive PFC1, target_cosq configured to 4, then all egq >= 4 will react to it
 *    BCM_COSQ_FC_INHERIT_DOWN
 *         can be used to set the same path for cosqs bellow the given cosq
 *         For example, receive PFC1, target_cosq configured to 4, then all egq <= 4 will react to it
 */
int cint_test_fc_rcv_pfc_bitmap_set_example(int unit, int port, int source_cosq, int target_cosq, int flags)
{
    int rv = BCM_E_NONE;

    /*Setting source port bcmPortControlPFCReceive enable*/
    rv = bcm_port_control_set(unit, port, bcmPortControlPFCReceive, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port PFC receive\n");
        return rv;
    }

    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;

    BCM_GPORT_LOCAL_SET(fc_handler.port, port);
    BCM_GPORT_LOCAL_SET(fc_trigger.port, port);
    /* setting flags */
    fc_trigger.flags = 0;
    fc_handler.flags = flags;
    /* setting cosq */
    fc_trigger.cosq = source_cosq; /* PFC */
    fc_handler.cosq = target_cosq;
    /* setting calender index, has to be -1 */
    fc_trigger.calender_index = -1;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc receive configuration failed(%d) in cint_test_fc_rcv_pfc_bitmap_set_example\n", rv);
        return rv;
    }

    return rv;
}

/*    Reset PFC to generic bitmap mapping    */
int cint_test_fc_rcv_pfc_bitmap_unset_example(int unit, int port, int source_cosq, int target_cosq, int flags)
{
    int rv = BCM_E_NONE;

    /*Setting source port bcmPortControlPFCReceive enable*/
    rv = bcm_port_control_set(unit, port, bcmPortControlPFCReceive, 0);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port PFC receive\n");
        return rv;
    }

    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;

    BCM_GPORT_LOCAL_SET(fc_handler.port, port);
    BCM_GPORT_LOCAL_SET(fc_trigger.port, port);
    /* setting flags */
    fc_trigger.flags = 0;
    fc_handler.flags = flags;
    /* setting cosq */ 
    fc_trigger.cosq = source_cosq; /* PFC */
    fc_handler.cosq = target_cosq;
    /* setting calender index, has to be -1 */
    fc_trigger.calender_index = -1;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc receive configuration failed(%d) in cint_test_fc_rcv_pfc_bitmap_unset_example\n", rv);
        return rv;
    }

    return rv;
}

/* This function configures PFC VSQ to PFC generation
 *   1. Enable PFC transmission
 *   2. Create PG base VSQ
 *   3. Set threshold of all 8 cosqs to 0 so that any traffic will trigger PFC generation
 *   Note: For threshold, on JR1, only BD is supported, flags should be set to 0
 *         On QAX and JR2, three types of threshold are supported, set by flags as below
 *         BCM_COSQ_PFC_BYTES
 *         BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_PACKET_DESC
 *         BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_BUFFERS
*/
int cint_test_fc_gen_vsqf_pfc_set_example(int unit, int source_port, int flags)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t local_gport;
    bcm_gport_t src_port_vsq_gport;
    bcm_gport_t pg_base_vsq_gport;
    bcm_cosq_src_vsqs_gport_config_t config;
    int src_port_vsq_index = 0;
    int pg_base = 0;
    int core_id = 0;
    int cosq;
    int i = 0;
    bcm_gport_t vsq_gport;
    int pg_vsq_id;
    int thresh = 0;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Setting source port bcmPortControlPFCTransmit enable*/
    rv = bcm_port_control_set(unit, source_port, bcmPortControlPFCTransmit, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port PFC transmit\n");
        return rv;
    }

    if (device_type == DEVICE_TYPE_JERICHO) {
        int base_voq;
        base_voq = 0x80; /* egress port 13. "IRR_DESTINATION_TABLE" */

        /*Set voq for each traffic class*/
        bcm_gport_t vsq_gport_cttc;
        bcm_gport_t queue;
        bcm_cos_queue_t cosq;
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(queue, BCM_CORE_ALL, base_voq);
        for (cosq = 0; cosq < 8; cosq++){
            BCM_COSQ_GPORT_VSQ_CTTC_SET(vsq_gport_cttc, BCM_CORE_ALL, 0, 2, cosq);
            rv = bcm_cosq_gport_vsq_add(unit, vsq_gport_cttc, queue, cosq);
            if (rv != BCM_E_NONE) {
                printf("Failed to add vsq\n");
                return rv;
            }
        }
    }

    /* Create VSQ-E and VSQ-F */
    config.flags = 0;
    config.numq = 8;
    for (i = 0; i < config.numq; i++) {
        config.pg_attributes[i].cosq_bitmap = (1<<i);
        config.pg_attributes[i].ocb_only    = IS_OCB_ONLY;
        config.pg_attributes[i].pool_id     = POOL_ID;
        config.pg_attributes[i].is_lossles  = IS_LOSSLESS;
    }

    rv = bcm_cosq_src_vsqs_gport_add(unit, source_port, &config, &src_port_vsq_gport, &pg_base_vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("Failed to create src vsqs\n");
        return rv;
    }

    /* Set FC threshold */
    pg_base = BCM_COSQ_GPORT_VSQ_PG_GET(pg_base_vsq_gport);
    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(pg_base_vsq_gport);

    bcm_cosq_pfc_config_t threshold;

    for (cosq = 0; cosq < 8; cosq++) {
        if (device_type == DEVICE_TYPE_JERICHO) {
            flags = 0;
            threshold.xon_threshold_bd = 0;
            threshold.xoff_threshold_bd = 0;
        } else if (device_type == DEVICE_TYPE_Q2A) {
           /* In Q2A behavior of the thresholds is different. If they are set to 0 the signal transmitted 
            * to CFC in order to generate FC will never stop even if there is no traffic. In order to avoid
            * this we need to set the thresholds to the smallest value possible. This means to write 1 in the 
            * relevant registers, but the registers have granularity. The API automatically calculates what 
            * to write in the registers according to that granularity. For example for BCM_COSQ_PFC_BYTES 
            * the granularity is 256 words, so when we pass to the API value of 256 it will write 1 in the register.
            */
            if (flags == BCM_COSQ_PFC_BYTES) {
                thresh=*(dnxc_data_get(unit,  "ingr_congestion", "info", "bytes_threshold_granularity", NULL));
            } else {
                thresh=*(dnxc_data_get(unit,  "ingr_congestion", "info", "threshold_granularity", NULL));
            }
            threshold.xoff_fadt_threshold.thresh_min = thresh;
            threshold.xoff_fadt_threshold.thresh_max = thresh;
            threshold.xoff_fadt_threshold.alpha = 0;
            threshold.xon_fadt_offset = 0;
        } else {
            threshold.xoff_fadt_threshold.thresh_min = 0;
            threshold.xoff_fadt_threshold.thresh_max = 0;
            threshold.xoff_fadt_threshold.alpha = 0;
            threshold.xon_fadt_offset = 0;
        }
        pg_vsq_id = pg_base + cosq;
        BCM_COSQ_GPORT_VSQ_PG_SET(vsq_gport, core_id, pg_vsq_id);
        rv = bcm_cosq_pfc_config_set(unit, vsq_gport, -1, flags, &threshold);
        if (rv != BCM_E_NONE) {
            printf("Failed to set threshold\n");
            return rv;
        }
    }

    return rv;
}

/* This function configures PFC VSQ to PFC generation according to threshold
 *   1. Enable PFC transmission
 *   2. Create PG base VSQ
 *   3. Set threshold of cosq 0~6 to vsq_thr_1, and cosq 7 to vsq_thr_2
        In this example, vsq_thr_1 sets a small value, vsq_thr_2 sets a big value
        so when threshold is below cosq 0~6 's value, no PFC generated
        when threshold is above cosq 0~6 's value and below cosq 7's, PFC0~6 will be generated
*/
int cint_test_fc_gen_vsqf_pfc_thr_set_example(int unit, int source_port, int flags, int vsq_thr_1, int vsq_thr_2)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t local_gport;
    bcm_gport_t src_port_vsq_gport;
    bcm_gport_t pg_base_vsq_gport;
    bcm_cosq_src_vsqs_gport_config_t config;
    int src_port_vsq_index = 0;
    int pg_base = 0;
    int core_id = 0;
    int cosq;
    int i = 0;
    bcm_gport_t vsq_gport;
    int pg_vsq_id;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Setting source port bcmPortControlPFCTransmit enable*/
    rv = bcm_port_control_set(unit, source_port, bcmPortControlPFCTransmit, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port PFC transmit\n");
        return rv;
    }

    if (device_type == DEVICE_TYPE_JERICHO) {
        int base_voq;
        base_voq = 0x80; /* egress port 13. "IRR_DESTINATION_TABLE" */

        /*Set voq for each traffic class*/
        bcm_gport_t vsq_gport_cttc;
        bcm_gport_t queue;
        bcm_cos_queue_t cosq;
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(queue, BCM_CORE_ALL, base_voq);
        for (cosq = 0; cosq < 8; cosq++){
            BCM_COSQ_GPORT_VSQ_CTTC_SET(vsq_gport_cttc, BCM_CORE_ALL, 0, 2, cosq);
            rv = bcm_cosq_gport_vsq_add(unit, vsq_gport_cttc, queue, cosq);
            if (rv != BCM_E_NONE) {
                printf("Failed to add vsq\n");
                return rv;
            }
        }
    }

    /* Create VSQ-E and VSQ-F */
    config.flags = 0;
    config.numq = 8;
    for (i = 0; i < config.numq; i++) {
        config.pg_attributes[i].cosq_bitmap = (1<<i);
        config.pg_attributes[i].ocb_only    = IS_OCB_ONLY;
        config.pg_attributes[i].pool_id     = POOL_ID;
        config.pg_attributes[i].is_lossles  = IS_LOSSLESS;
    }

    rv = bcm_cosq_src_vsqs_gport_add(unit, source_port, &config, &src_port_vsq_gport, &pg_base_vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("Failed to create src vsqs\n");
        return rv;
    }

    /* Set FC threshold */
    pg_base = BCM_COSQ_GPORT_VSQ_PG_GET(pg_base_vsq_gport);
    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(pg_base_vsq_gport);

    bcm_cosq_pfc_config_t threshold;
    /* For cosq 0~6, set threshold the same */
    for (cosq = 0; cosq < 7; cosq++) {
        if (device_type == DEVICE_TYPE_JERICHO) {
            flags = 0;
            threshold.xon_threshold_bd = vsq_thr_1;
            threshold.xoff_threshold_bd = vsq_thr_1;
        } else {
            threshold.xoff_fadt_threshold.thresh_min = vsq_thr_1;
            threshold.xoff_fadt_threshold.thresh_max = vsq_thr_1;
            threshold.xoff_fadt_threshold.alpha = 0;
            threshold.xon_fadt_offset = 0;
        }
        pg_vsq_id = pg_base + cosq;
        BCM_COSQ_GPORT_VSQ_PG_SET(vsq_gport, core_id, pg_vsq_id);
        rv = bcm_cosq_pfc_config_set(unit, vsq_gport, -1, flags, &threshold);
        if (rv != BCM_E_NONE) {
            printf("Failed to set threshold\n");
            return rv;
        }
    }

    /* For cosq 7, set it to a large value so that threshold won't be reached */
    cosq = 7;
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
        threshold.xon_threshold_bd = vsq_thr_2;
        threshold.xoff_threshold_bd = vsq_thr_2;
    } else {
        threshold.xoff_fadt_threshold.thresh_min = vsq_thr_2;
        threshold.xoff_fadt_threshold.thresh_max = vsq_thr_2;
        threshold.xoff_fadt_threshold.alpha = 0;
        threshold.xon_fadt_offset = 0;
    }
    pg_vsq_id = pg_base + cosq;
    BCM_COSQ_GPORT_VSQ_PG_SET(vsq_gport, core_id, pg_vsq_id);
    rv = bcm_cosq_pfc_config_set(unit, vsq_gport, -1, flags, &threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/* This function configures global VSQ to LLFC generation according to threshold
 *   1. Enable LLFC transmission
 *   2. Create global VSQ
 *   3. Create FC path,default global VSQ type is global_vsq_src
 *   4. Set global VSQ threshold set and clear to the same value:global_thr for simplicity
 *   For global vsq srouce type, refer to global_vsq_src_e
 *   Note: on QAX and JR2, for pool0/1 byte/pd/buffer resources
 *         the threshold configured is not free resource left, other resource types indicate free resource left
 *         for example:
 *         global resource is BDB, thr_set is 1000
 *           then FC will be triggered only when BDB resource left is less than 1000
 *         global resource is pool 0 bytes, thr_set is 1000
 *           then FC will be triggered only when pool 0 bytes used is more than 1000
 
*/
int cint_test_fc_gen_gl_llfc_set_example(int unit, global_vsq_src_e global_vsq_src, int port, int global_thr)
{
    int rv = BCM_E_NONE;
    int flags = 0;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Setting port bcmPortControlLLFCTransmit enable*/
    rv = bcm_port_control_set(unit, port, bcmPortControlLLFCTransmit, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port LLFC transmit\n");
        return rv;
    }

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    /* qax has only one core */
    if (device_type == DEVICE_TYPE_QAX) {
        vsq_inf.core_id = 0;
    } else {
        vsq_inf.core_id = CORE;
    }
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            switch (device_type) {
                case DEVICE_TYPE_JERICHO:
                    flags = BCM_COSQ_FC_BDB | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_mini_db:
            flags = BCM_COSQ_FC_MINI_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_full_db:
            flags = BCM_COSQ_FC_FULL_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_ocb_db:
            switch (device_type) {
                case DEVICE_TYPE_JERICHO:
                    flags = BCM_COSQ_FC_OCB_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
            }
            break;
        case global_vsq_src_mix_pool0:
            flags = 0;
            break;
        case global_vsq_src_mix_pool1:
            flags = 0;
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_ocb_pool0:
            flags = BCM_COSQ_FC_IS_OCB_ONLY;
            break;
        case global_vsq_src_ocb_pool1:
            flags = BCM_COSQ_FC_IS_OCB_ONLY;
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_ocb_pdb:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
            }
            break;
        case global_vsq_src_p0_db:
            fc_trigger.cosq = 0;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p1_db:
            fc_trigger.cosq = 1;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p0_pd:
            fc_trigger.cosq = 0;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p1_pd:
            fc_trigger.cosq = 1;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p0_byte:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    fc_trigger.cosq = 0;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = 0;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = 0;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_p1_byte:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    fc_trigger.cosq = 1;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = 1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = 1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_hdrm_db:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
            }
            break;
        case global_vsq_src_hdrm_pd:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
            }
            break;
        default:
            printf("wrong global resource type\n");
            return BCM_E_PARAM;
    }

    BCM_GPORT_LOCAL_SET(fc_handler.port, port);
    fc_trigger.port = vsq_gport;
    /* setting flags */
    fc_trigger.flags = flags;
    fc_handler.flags = 0;
    /* setting priority */
    fc_trigger.priority = 0; /* high priority */
    /* setting cosq */
    fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_gl_llfc_set_example\n", rv);
        return rv;
    }

    /* Set global vsq threshold */
    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        cint_test_jr2_fc_gl_thr_set(unit, global_vsq_src, 0, global_thr, global_thr);
    } else {
        cint_test_fc_gl_thr_set(unit, global_vsq_src, 0, global_thr, global_thr);
    }

    return rv;
}

/*    Reset global resource to LLFC generation    */
int cint_test_fc_gen_gl_llfc_unset_example(int unit, global_vsq_src_e global_vsq_src, int port)
{
    int rv = BCM_E_NONE;
    int flags = 0;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    /* qax has only one core */
    if (device_type == DEVICE_TYPE_QAX) {
        vsq_inf.core_id = 0;
    } else {
        vsq_inf.core_id = CORE;
    }
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            switch (device_type) {
                case DEVICE_TYPE_JERICHO:
                    flags = BCM_COSQ_FC_BDB | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_mini_db:
            flags = BCM_COSQ_FC_MINI_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_full_db:
            flags = BCM_COSQ_FC_FULL_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_ocb_db:
            switch (device_type) {
                case DEVICE_TYPE_JERICHO:
                    flags = BCM_COSQ_FC_OCB_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
            }
            break;
        case global_vsq_src_mix_pool0:
            flags = 0;
            break;
        case global_vsq_src_mix_pool1:
            flags = 0;
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_ocb_pool0:
            flags = BCM_COSQ_FC_IS_OCB_ONLY;
            break;
        case global_vsq_src_ocb_pool1:
            flags = BCM_COSQ_FC_IS_OCB_ONLY;
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_ocb_pdb:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
            }
            break;
        case global_vsq_src_p0_db:
            fc_trigger.cosq = 0;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p1_db:
            fc_trigger.cosq = 1;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p0_pd:
            fc_trigger.cosq = 0;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p1_pd:
            fc_trigger.cosq = 1;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p0_byte:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    fc_trigger.cosq = 0;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = 0;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = 0;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_p1_byte:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    fc_trigger.cosq = 1;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = 1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = 1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_hdrm_db:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
            }
            break;
        case global_vsq_src_hdrm_pd:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
            }
            break;
        default:
            printf("wrong global resource type\n");
            return BCM_E_PARAM;
    }

    BCM_GPORT_LOCAL_SET(fc_handler.port, port);
    fc_trigger.port = vsq_gport;
    /* setting flags */
    fc_trigger.flags = flags;
    fc_handler.flags = 0;
    /* setting priority */
    fc_trigger.priority = 0; /* high priority */
    /* setting cosq */
    fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_gl_llfc_unset_example\n", rv);
        return rv;
    }

    return rv;
}

/* This function configures global VSQ to PFC generation according to threshold
 *   1. Enable PFC transmission
 *   2. Create global VSQ,default global VSQ type is:global_vsq_src
 *   3. Create FC path.
 *   4. Set global VSQ threshold set and clear to same value for simplicity
*/
int cint_test_fc_gen_gl_pfc_set_example(int unit, global_vsq_src_e global_vsq_src,int port, int priority,
                                        int target_cosq, int global_thr)
{
    int rv = BCM_E_NONE;
    int flags = 0;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Setting port bcmPortControlPFCTransmit enable*/
    rv = bcm_port_control_set(unit, port, bcmPortControlPFCTransmit, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port PFC transmit\n");
        return rv;
    }

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    /* qax has only one core */
    if (device_type == DEVICE_TYPE_QAX) {
        vsq_inf.core_id = 0;
    } else {
        vsq_inf.core_id = CORE;
    }
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            switch (device_type) {
                case DEVICE_TYPE_JERICHO:
                    flags = BCM_COSQ_FC_BDB | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_mini_db:
            flags = BCM_COSQ_FC_MINI_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_full_db:
            flags = BCM_COSQ_FC_FULL_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_ocb_db:
            switch (device_type) {
                case DEVICE_TYPE_JERICHO:
                    flags = BCM_COSQ_FC_OCB_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
            }
            break;
        case global_vsq_src_mix_pool0:
            flags = 0;
            break;
        case global_vsq_src_mix_pool1:
            flags = 0;
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_ocb_pool0:
            flags = BCM_COSQ_FC_IS_OCB_ONLY;
            break;
        case global_vsq_src_ocb_pool1:
            flags = BCM_COSQ_FC_IS_OCB_ONLY;
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_ocb_pdb:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
            }
            break;
        case global_vsq_src_p0_db:
            fc_trigger.cosq = 0;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p1_db:
            fc_trigger.cosq = 1;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p0_pd:
            fc_trigger.cosq = 0;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p1_pd:
            fc_trigger.cosq = 1;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p0_byte:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    fc_trigger.cosq = 0;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = 0;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = 0;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_p1_byte:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    fc_trigger.cosq = 1;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = 1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = 1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_hdrm_db:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
            }
            break;
        case global_vsq_src_hdrm_pd:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
            }
            break;
        default:
            printf("wrong global resource type\n");
            return BCM_E_PARAM;
    }

    BCM_GPORT_LOCAL_SET(fc_handler.port, port);
    fc_trigger.port = vsq_gport;
    /* setting flags */
    fc_trigger.flags = flags;
    fc_handler.flags = 0;
    /* setting priority */
    fc_trigger.priority = priority;
    /* setting cosq */
    fc_handler.cosq = target_cosq;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_gl_pfc_set_example\n", rv);
        return rv;
    }

    /* Set global vsq threshold */
    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        cint_test_jr2_fc_gl_thr_set(unit, global_vsq_src, priority, global_thr, global_thr);
    } else {
        cint_test_fc_gl_thr_set(unit, global_vsq_src, priority, global_thr, global_thr);
    }

    return rv;
}

/*    Reset global resource to PFC generation    */
int cint_test_fc_gen_gl_pfc_unset_example(int unit, global_vsq_src_e global_vsq_src, int port, int priority, int target_cosq)
{
    int rv = BCM_E_NONE;
    int flags = 0;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    /* qax has only one core */
    if (device_type == DEVICE_TYPE_QAX) {
        vsq_inf.core_id = 0;
    } else {
        vsq_inf.core_id = CORE;
    }
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            switch (device_type) {
                case DEVICE_TYPE_JERICHO:
                    flags = BCM_COSQ_FC_BDB | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_mini_db:
            flags = BCM_COSQ_FC_MINI_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_full_db:
            flags = BCM_COSQ_FC_FULL_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_ocb_db:
            switch (device_type) {
                case DEVICE_TYPE_JERICHO:
                    flags = BCM_COSQ_FC_OCB_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
            }
            break;
        case global_vsq_src_mix_pool0:
            flags = 0;
            break;
        case global_vsq_src_mix_pool1:
            flags = 0;
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_ocb_pool0:
            flags = BCM_COSQ_FC_IS_OCB_ONLY;
            break;
        case global_vsq_src_ocb_pool1:
            flags = BCM_COSQ_FC_IS_OCB_ONLY;
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_ocb_pdb:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
            }
            break;
        case global_vsq_src_p0_db:
            fc_trigger.cosq = 0;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p1_db:
            fc_trigger.cosq = 1;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p0_pd:
            fc_trigger.cosq = 0;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p1_pd:
            fc_trigger.cosq = 1;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p0_byte:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    fc_trigger.cosq = 0;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = 0;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = 0;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_p1_byte:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    fc_trigger.cosq = 1;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = 1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = 1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_hdrm_db:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
            }
            break;
        case global_vsq_src_hdrm_pd:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
            }
            break;
        default:
            printf("wrong global resource type\n");
            return BCM_E_PARAM;
    }

    BCM_GPORT_LOCAL_SET(fc_handler.port, port);
    fc_trigger.port = vsq_gport;
    /* setting flags */
    fc_trigger.flags = flags;
    fc_handler.flags = 0;
    /* setting priority */
    fc_trigger.priority = priority;
    /* setting cosq */
    fc_handler.cosq = target_cosq;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_gl_pfc_unset_example\n", rv);
        return rv;
    }

    return rv;
}

/* This function configures global VSQ to OOB/ILKN_Inband generation according to threshold
 *   1. Create global VSQ,default global VSQ type: global_vsq_src
 *   2. Create FC path.
 *   3. Set global VSQ threshold set and clear to same value for simplicity
*/
int cint_test_fc_gen_gl_to_cal_set_example(int unit, global_vsq_src_e global_vsq_src, int port,
                                        int calendar_idx, int priority, int global_thr, int is_oob)
{
    int rv = BCM_E_NONE;
    int flags = 0;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    /* qax has only one core */
    if (device_type == DEVICE_TYPE_QAX) {
        vsq_inf.core_id = 0;
    } else {
        vsq_inf.core_id = CORE;
    }
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            switch (device_type) {
                case DEVICE_TYPE_JERICHO:
                    flags = BCM_COSQ_FC_BDB | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_mini_db:
            flags = BCM_COSQ_FC_MINI_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_full_db:
            flags = BCM_COSQ_FC_FULL_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_ocb_db:
            switch (device_type) {
                case DEVICE_TYPE_JERICHO:
                    flags = BCM_COSQ_FC_OCB_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
            }
            break;
        case global_vsq_src_mix_pool0:
            flags = 0;
            break;
        case global_vsq_src_mix_pool1:
            flags = 0;
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_ocb_pool0:
            flags = BCM_COSQ_FC_IS_OCB_ONLY;
            break;
        case global_vsq_src_ocb_pool1:
            flags = BCM_COSQ_FC_IS_OCB_ONLY;
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_ocb_pdb:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
            }
            break;
        case global_vsq_src_p0_db:
            fc_trigger.cosq = 0;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p1_db:
            fc_trigger.cosq = 1;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p0_pd:
            fc_trigger.cosq = 0;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p1_pd:
            fc_trigger.cosq = 1;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p0_byte:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    fc_trigger.cosq = 0;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = 0;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = 0;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_p1_byte:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    fc_trigger.cosq = 1;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = 1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = 1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_hdrm_db:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
            }
            break;
        case global_vsq_src_hdrm_pd:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
            }
            break;
        default:
            printf("wrong global resource type\n");
            return BCM_E_PARAM;
    }

    if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_handler.port, port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_handler.port, port);
    }
    fc_trigger.port = vsq_gport;
    /* setting flags */
    fc_trigger.flags = flags;
    fc_handler.flags = 0;
    /* setting priority */
    fc_trigger.priority = priority;
    /* setting cosq */ 
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_gl_to_cal_set_example\n", rv);
        return rv;
    }

    /* Set global vsq threshold */
    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        cint_test_jr2_fc_gl_thr_set(unit, global_vsq_src, priority, global_thr, global_thr);
    } else {
        cint_test_fc_gl_thr_set(unit, global_vsq_src, priority, global_thr, global_thr);
    }

    return rv;
}

/*    Reset global resource to OOB/ILKN_Inband generation    */
int cint_test_fc_gen_gl_to_cal_unset_example(int unit, global_vsq_src_e global_vsq_src, int port, int calendar_idx, int priority, int is_oob)
{
    int rv = BCM_E_NONE;
    int flags = 0;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    /* qax has only one core */
    if (device_type == DEVICE_TYPE_QAX) {
        vsq_inf.core_id = 0;
    } else {
        vsq_inf.core_id = CORE;
    }
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            switch (device_type) {
                case DEVICE_TYPE_JERICHO:
                    flags = BCM_COSQ_FC_BDB | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_mini_db:
            flags = BCM_COSQ_FC_MINI_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_full_db:
            flags = BCM_COSQ_FC_FULL_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_ocb_db:
            switch (device_type) {
                case DEVICE_TYPE_JERICHO:
                    flags = BCM_COSQ_FC_OCB_DB | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
            }
            break;
        case global_vsq_src_mix_pool0:
            flags = 0;
            break;
        case global_vsq_src_mix_pool1:
            flags = 0;
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_ocb_pool0:
            flags = BCM_COSQ_FC_IS_OCB_ONLY;
            break;
        case global_vsq_src_ocb_pool1:
            flags = BCM_COSQ_FC_IS_OCB_ONLY;
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_ocb_pdb:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_IS_OCB_ONLY;
                    break;
            }
            break;
        case global_vsq_src_p0_db:
            fc_trigger.cosq = 0;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p1_db:
            fc_trigger.cosq = 1;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p0_pd:
            fc_trigger.cosq = 0;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p1_pd:
            fc_trigger.cosq = 1;
            if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
                flags = BCM_COSQ_FC_IS_OCB_ONLY;
            }
            break;
        case global_vsq_src_p0_byte:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    fc_trigger.cosq = 0;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = 0;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = 0;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_p1_byte:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    fc_trigger.cosq = 1;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = 1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = 1;
                    flags = BCM_COSQ_FC_BDB;
                    break;
            }
            break;
        case global_vsq_src_hdrm_db:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
            }
            break;
        case global_vsq_src_hdrm_pd:
            switch (device_type) {
                case DEVICE_TYPE_QAX:
                    flags = BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
                    break;
                case DEVICE_TYPE_JR2:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
                case DEVICE_TYPE_Q2A:
                    fc_trigger.cosq = -1;
                    flags = BCM_COSQ_FC_HEADROOM;
                    break;
            }
            break;
        default:
            printf("wrong global resource type\n");
            return BCM_E_PARAM;
    }

    if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_handler.port, port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_handler.port, port);
    }
    fc_trigger.port = vsq_gport;
    /* setting flags */
    fc_trigger.flags = flags;
    fc_handler.flags = 0;
    /* setting priority */
    fc_trigger.priority = priority;
    /* setting cosq */ 
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_gl_to_cal_unset_example\n", rv);
        return rv;
    }

    return rv;
}

/* This function configures CTTC VSQ to PFC generation according to threshold
 *   1. Create CTTC VSQ
 *   2. Create FC path.
 *   3. Set VSQ threshold: vsq_thr
 *   src_port: PFC will generate to this port
 *   dest_port: traffic sent to this port, used as VOQ to VSQ mapping
 *   Note:
 *      On JR1, use port 13 as example, set base VOQ for port 13, on JR2, we use APPL to get base VOQ.
 *      For threshold, on JR1, only BD is supported, flags should be set to 0
 *         On QAX and JR2, three types of threshold are supported, set by flags as below
 *         BCM_COSQ_PFC_BYTES
 *         BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_PACKET_DESC
 *         BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_BUFFERS
*/
int cint_test_fc_gen_cttc_pfc_set_example(int unit, int src_port, int dest_port, int target_cosq, int flags, int vsq_thr)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t vsq_gport_cttc;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Setting port bcmPortControlPFCTransmit enable*/
    rv = bcm_port_control_set(unit, src_port, bcmPortControlPFCTransmit, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port PFC transmit\n");
        return rv;
    }

    /* Example, use port 13
     * JR1: IRR_DESTINATION_TABLE
     * QAX: TAR_DESTINATION_TABLE
    */
    int base_voq;
    /* On JR2, get base voq from application code */
    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
         int modid;
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
        modid = modid_array[0].modid;
        rv = appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit,modid,dest_port,&base_voq);
        if (rv != BCM_E_NONE) {
            printf("Failed in appl_dnx_e2e_scheme_logical_port_to_base_voq_get\n");
            return rv;
        }
    } else {
        base_voq= 0x80;
    }

    bcm_gport_t queue;
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(queue, BCM_CORE_ALL, base_voq);

    /*Mapping VOQ to VSQ*/
    for ( i = 0; i < 8; i++) {
        BCM_COSQ_GPORT_VSQ_CTTC_SET(vsq_gport_cttc, BCM_CORE_ALL, 0, CT, i);

        rv = bcm_cosq_gport_vsq_add(unit, vsq_gport_cttc, queue, i);
        if (rv != BCM_E_NONE) {
            printf("Failed in bcm_cosq_gport_vsq_add\n");
            return rv;
        }
    }

    /*Get specific cttc VSQ with specific traffic class*/
    BCM_COSQ_GPORT_VSQ_CTTC_SET(vsq_gport_cttc, BCM_CORE_ALL, 0, CT, TC);

    BCM_GPORT_LOCAL_SET(fc_handler.port, src_port);
    fc_trigger.port = vsq_gport_cttc;
    fc_trigger.cosq = -1;
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = target_cosq;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_jer_test_fc_gen_cttc_pfc_set_example\n", rv);
        return rv;
    }

    /* Get default threshold used for restore */
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    rv = bcm_cosq_pfc_config_get(unit, vsq_gport_cttc, 0, flags, &VSQ_THRESHOLD);
    if (rv != BCM_E_NONE) {
        printf("Failed to get threshold\n");
        return rv;
    }

    /* Set FC threshold */
    bcm_cosq_pfc_config_t threshold;
    if (device_type == DEVICE_TYPE_JERICHO) {
        threshold.xoff_threshold_bd = vsq_thr;
        threshold.xon_threshold_bd = vsq_thr;
    } else {
        threshold.xoff_threshold = vsq_thr;
        threshold.xon_threshold = vsq_thr;
    }
    rv = bcm_cosq_pfc_config_set(unit, vsq_gport_cttc, 0, flags, &threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*    Reset CTTC to PFC generation    */
int cint_test_fc_gen_cttc_pfc_unset_example(int unit, int port, int target_cosq, int flags)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t vsq_gport_cttc;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Setting port bcmPortControlPFCTransmit enable*/
    rv = bcm_port_control_set(unit, port, bcmPortControlPFCTransmit, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port PFC transmit\n");
        return rv;
    }

    /*Get specific cttc VSQ with specific traffic class*/
    BCM_COSQ_GPORT_VSQ_CTTC_SET(vsq_gport_cttc, BCM_CORE_ALL, 0, CT, TC);

    BCM_GPORT_LOCAL_SET(fc_handler.port, port);
    fc_trigger.port = vsq_gport_cttc;
    fc_trigger.cosq = -1;
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = target_cosq;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_jer_test_fc_gen_cttc_pfc_unset_example\n", rv);
        return rv;
    }

    /* Reset FC threshold to default*/
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    rv = bcm_cosq_pfc_config_set(unit, vsq_gport_cttc, 0, flags, &VSQ_THRESHOLD);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*    CT VSQ to OOB generation    */
int cint_test_fc_gen_ct_oob_set_example(int unit, int oob_port, int oob_calendar_idx, int flags, int vsq_thr)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t vsq_gport_ct;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Get specific cttc VSQ with TC*/
    BCM_COSQ_GPORT_VSQ_CT_SET(vsq_gport_ct, BCM_CORE_ALL, 0, CT);

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = vsq_gport_ct;
    fc_trigger.cosq = -1;
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_ct_oob_set_example\n", rv);
        return rv;
    }

    /* Get default threshold used for restore */
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    rv = bcm_cosq_pfc_config_get(unit, vsq_gport_ct, 0, flags, &VSQ_THRESHOLD);
    if (rv != BCM_E_NONE) {
        printf("Failed to get threshold\n");
        return rv;
    }

    /* Set FC threshold */
    bcm_cosq_pfc_config_t threshold;
    if (device_type == DEVICE_TYPE_JERICHO) {
        threshold.xoff_threshold_bd = vsq_thr;
        threshold.xon_threshold_bd = vsq_thr;
    } else {
        threshold.xoff_threshold = vsq_thr;
        threshold.xon_threshold = vsq_thr;
    }
    rv = bcm_cosq_pfc_config_set(unit, vsq_gport_ct, 0, flags, &threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*    Reset CT VSQ to OOB generation    */
int cint_test_fc_gen_ct_oob_unset_example(int unit, int oob_port, int oob_calendar_idx, int flags)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t vsq_gport_ct;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Get specific cttc VSQ with TC*/
    BCM_COSQ_GPORT_VSQ_CT_SET(vsq_gport_ct, BCM_CORE_ALL, 0, CT);

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = vsq_gport_ct;
    fc_trigger.cosq = -1;
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_ct_oob_unset_example\n", rv);
        return rv;
    }

    /* Reset FC threshold to default*/
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    rv = bcm_cosq_pfc_config_set(unit, vsq_gport_ct, 0, flags, &VSQ_THRESHOLD);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*
 *  CTTC to OOB generation
 *  port 13 is used as example
 */
int cint_test_fc_gen_cttc_oob_set_example(int unit, int port, int oob_port, int oob_calendar_idx, int flags, int vsq_thr)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t vsq_gport_cttc;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /* Example, use port 13
     * JR1: IRR_DESTINATION_TABLE
     * QAX: TAR_DESTINATION_TABLE
    */
    int base_voq;
    /* On JR2, get base voq from application code */
    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        int modid;
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
        modid = modid_array[0].modid;
        rv = appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit,modid,port,&base_voq);
        if (rv != BCM_E_NONE) {
            printf("Failed in appl_dnx_e2e_scheme_logical_port_to_base_voq_get\n");
            return rv;
        }
    } else {
        base_voq= 0x80;
    }

    bcm_gport_t queue;
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(queue, BCM_CORE_ALL, base_voq);

    /*Mapping VOQ and TC to VSQ*/
    for ( i = 0; i < 8; i++) {
        BCM_COSQ_GPORT_VSQ_CTTC_SET(vsq_gport_cttc, BCM_CORE_ALL, 0, CT, i);

        rv = bcm_cosq_gport_vsq_add(unit, vsq_gport_cttc, queue, i);
        if (rv != BCM_E_NONE) {
            printf("Failed in bcm_cosq_gport_vsq_add\n");
            return rv;
        }
    }

    /*Get specific cttc VSQ with TC*/
    BCM_COSQ_GPORT_VSQ_CTTC_SET(vsq_gport_cttc, BCM_CORE_ALL, 0, CT, TC);

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = vsq_gport_cttc;
    fc_trigger.cosq = -1;
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_cttc_oob_set_example\n", rv);
        return rv;
    }

    /* Get default threshold used for restore */
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    rv = bcm_cosq_pfc_config_get(unit, vsq_gport_cttc, 0, flags, &VSQ_THRESHOLD);
    if (rv != BCM_E_NONE) {
        printf("Failed to get threshold\n");
        return rv;
    }

    /* Set FC threshold */
    bcm_cosq_pfc_config_t threshold;
    if (device_type == DEVICE_TYPE_JERICHO) {
        threshold.xoff_threshold_bd = vsq_thr;
        threshold.xon_threshold_bd = vsq_thr;
    } else {
        threshold.xoff_threshold = vsq_thr;
        threshold.xon_threshold = vsq_thr;
    }
    rv = bcm_cosq_pfc_config_set(unit, vsq_gport_cttc, 0, flags, &threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*    Reset CTTC to OOB generation    */
int cint_test_fc_gen_cttc_oob_unset_example(int unit, int port, int oob_port, int oob_calendar_idx, int flags)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t vsq_gport_cttc;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Get specific cttc VSQ with TC*/
    BCM_COSQ_GPORT_VSQ_CTTC_SET(vsq_gport_cttc, BCM_CORE_ALL, 0, CT, TC);

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = vsq_gport_cttc;
    fc_trigger.cosq = -1;
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_cttc_oob_unset_example\n", rv);
        return rv;
    }

    /* Reset FC threshold to default*/
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    rv = bcm_cosq_pfc_config_set(unit, vsq_gport_cttc, 0, flags, &VSQ_THRESHOLD);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*    CTCC VSQ to OOB generation    */
int cint_test_fc_gen_ctcc_oob_set_example(int unit, int oob_port, int oob_calendar_idx, int flags, int vsq_thr)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t vsq_gport_ctcc;
    int i;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /* Example, use port 13
     * JR1: IRR_DESTINATION_TABLE
     * QAX: TAR_DESTINATION_TABLE
    */
    int base_voq;
    /* On JR2, get base voq from application code */
    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        int modid;
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
        modid = modid_array[0].modid;
        rv = appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit,modid,port,&base_voq);
        if (rv != BCM_E_NONE) {
            printf("Failed in appl_dnx_e2e_scheme_logical_port_to_base_voq_get\n");
            return rv;
        }
    } else {
        base_voq= 0x80;
    }

    bcm_gport_t queue;
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(queue, BCM_CORE_ALL, base_voq);

    /*Mapping VOQ and CC to VSQ*/
    for ( i = 0; i < 8; i++) {
        BCM_COSQ_GPORT_VSQ_CTCC_SET(vsq_gport_ctcc, BCM_CORE_ALL, 0, CT, i);

        rv = bcm_cosq_gport_vsq_add(unit, vsq_gport_ctcc, queue, i);
        if (rv != BCM_E_NONE) {
            printf("Failed in bcm_cosq_gport_vsq_add\n");
            return rv;
        }
    }

    /*Get specific ctcc VSQ with CC*/
    BCM_COSQ_GPORT_VSQ_CTCC_SET(vsq_gport_ctcc, BCM_CORE_ALL, 0, CT, CC);

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = vsq_gport_ctcc;
    fc_trigger.cosq = -1;
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_ctcc_oob_set_example\n", rv);
        return rv;
    }

    /* Get default threshold used for restore */
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    rv = bcm_cosq_pfc_config_get(unit, vsq_gport_ctcc, 0, flags, &VSQ_THRESHOLD);
    if (rv != BCM_E_NONE) {
        printf("Failed to get threshold\n");
        return rv;
    }

    /* Set FC threshold */
    bcm_cosq_pfc_config_t threshold;
    if (device_type == DEVICE_TYPE_JERICHO) {
        threshold.xoff_threshold_bd = vsq_thr;
        threshold.xon_threshold_bd = vsq_thr;
    } else {
        threshold.xoff_threshold = vsq_thr;
        threshold.xon_threshold = vsq_thr;
    }
    rv = bcm_cosq_pfc_config_set(unit, vsq_gport_ctcc, 0, flags, &threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*    Reset CTCC VSQ to OOB generation    */
int cint_test_fc_gen_ctcc_oob_unset_example(int unit, int oob_port, int oob_calendar_idx, int flags)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t vsq_gport_ctcc;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Get specific cttc VSQ with TC*/
    BCM_COSQ_GPORT_VSQ_CTCC_SET(vsq_gport_ctcc, BCM_CORE_ALL, 0, CT, CC);

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = vsq_gport_ctcc;
    fc_trigger.cosq = -1;
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_ctcc_oob_unset_example\n", rv);
        return rv;
    }

    /* Reset FC threshold to default*/
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    rv = bcm_cosq_pfc_config_set(unit, vsq_gport_ctcc, 0, flags, &VSQ_THRESHOLD);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*    PP VSQ to OOB generation    */
int cint_test_fc_gen_pp_vsq_oob_set_example(int unit, int oob_port, int oob_calendar_idx, int flags, int vsq_thr)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t st_vsq_gport;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    vsq_inf.flags = BCM_COSQ_VSQ_PP;
    vsq_inf.pp_vsq_id = ST_VSQ_ID;
    vsq_inf.core_id = BCM_CORE_ALL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&st_vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = st_vsq_gport;
    fc_trigger.cosq = -1;
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_pp_vsq_oob_set_example\n", rv);
        return rv;
    }

    /* Get default threshold used for restore */
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    rv = bcm_cosq_pfc_config_get(unit, st_vsq_gport, 0, flags, &VSQ_THRESHOLD);
    if (rv != BCM_E_NONE) {
        printf("Failed to get threshold\n");
        return rv;
    }

    /* Set FC threshold */
    bcm_cosq_pfc_config_t threshold;
    if (device_type == DEVICE_TYPE_JERICHO) {
        threshold.xoff_threshold_bd = vsq_thr;
        threshold.xon_threshold_bd = vsq_thr;
    } else {
        threshold.xoff_threshold = vsq_thr;
        threshold.xon_threshold = vsq_thr;
    }
    rv = bcm_cosq_pfc_config_set(unit, st_vsq_gport, 0, flags, &threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*    Reset PP VSQ to OOB generation    */
int cint_test_fc_gen_pp_vsq_oob_unset_example(int unit, int oob_port, int oob_calendar_idx, int flags)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t st_vsq_gport;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    vsq_inf.flags = BCM_COSQ_VSQ_PP;
    vsq_inf.pp_vsq_id = ST_VSQ_ID;
    vsq_inf.core_id = BCM_CORE_ALL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&st_vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = st_vsq_gport;
    fc_trigger.cosq = -1;
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_pp_vsq_oob_unset_example\n", rv);
        return rv;
    }

    /* Reset FC threshold to default*/
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    rv = bcm_cosq_pfc_config_set(unit, st_vsq_gport, 0, flags, &VSQ_THRESHOLD);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*    LLFC VSQ to OOB generation    */
int cint_test_fc_gen_vsqe_oob_set_example(int unit, int source_port,int oob_port,int oob_calendar_idx, int flags, int vsq_thr)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t local_gport;
    bcm_gport_t src_port_vsq_gport;
    bcm_gport_t pg_base_vsq_gport;
    bcm_cosq_src_vsqs_gport_config_t config;
    int src_port_vsq_index = 0;
    int pg_base = 0;
    int i = 0;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /* Create VSQ-E and VSQ-F */
    config.flags = 0;
    config.numq = 8;
    for (i = 0; i < config.numq; i++) {
        config.pg_attributes[i].cosq_bitmap = (1<<i);
        config.pg_attributes[i].ocb_only    = IS_OCB_ONLY;
        config.pg_attributes[i].pool_id     = POOL_ID;
        config.pg_attributes[i].is_lossles  = IS_LOSSLESS;
    }

    rv = bcm_cosq_src_vsqs_gport_add(unit, source_port, &config, &src_port_vsq_gport, &pg_base_vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("Failed to create src vsqs\n");
        return rv;
    }

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = src_port_vsq_gport;
    fc_trigger.cosq = -1;
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_vsqe_oob_set_example\n", rv);
        return rv;
    }

    /* Get default threshold used for restore */
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    rv = bcm_cosq_pfc_config_get(unit, src_port_vsq_gport, 0, flags, &VSQ_THRESHOLD);
    if (rv != BCM_E_NONE) {
        printf("Failed to get threshold\n");
        return rv;
    }

    /* Set FC threshold */
    bcm_cosq_pfc_config_t threshold;
    if (device_type == DEVICE_TYPE_JERICHO) {
        threshold.xoff_threshold_bd = vsq_thr;
        threshold.xon_threshold_bd = vsq_thr;
    } else {
        threshold.xoff_threshold = vsq_thr;
        threshold.xon_threshold = vsq_thr;
    }
    rv = bcm_cosq_pfc_config_set(unit, src_port_vsq_gport, 0, flags, &threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*    Reset LLFC VSQ to OOB generation    */
int cint_test_fc_gen_vsqe_oob_unset_example(int unit, int source_port,int oob_port,int oob_calendar_idx, int flags)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t local_gport;
    bcm_gport_t src_port_vsq_gport;
    bcm_gport_t pg_base_vsq_gport;
    bcm_cosq_src_vsqs_gport_config_t config;
    int src_port_vsq_index = 0;
    int pg_base = 0;
    int i = 0;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /* Create VSQ-E and VSQ-F */
    config.flags = 0;
    config.numq = 8;
    for (i = 0; i < config.numq; i++) {
        config.pg_attributes[i].cosq_bitmap = (1<<i);
        config.pg_attributes[i].ocb_only    = IS_OCB_ONLY;
        config.pg_attributes[i].pool_id     = POOL_ID;
        config.pg_attributes[i].is_lossles  = IS_LOSSLESS;
    }

    rv = bcm_cosq_src_vsqs_gport_get(unit, source_port, &config, &src_port_vsq_gport, &pg_base_vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("Failed to create src vsqs\n");
        return rv;
    }

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = src_port_vsq_gport;
    fc_trigger.cosq = -1;
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_vsqe_oob_unset_example\n", rv);
        return rv;
    }

    /* Reset FC threshold to default*/
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    rv = bcm_cosq_pfc_config_set(unit, src_port_vsq_gport, 0, flags, &VSQ_THRESHOLD);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*    PFC VSQ to OOB generation    */
int cint_test_fc_gen_vsqf_oob_set_example(int unit, int source_port,int oob_port,int oob_calendar_idx,
                                          int flags, int vsq_thr_1, int vsq_thr_2)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t local_gport;
    bcm_gport_t src_port_vsq_gport;
    bcm_gport_t pg_base_vsq_gport;
    bcm_cosq_src_vsqs_gport_config_t config;
    int src_port_vsq_index = 0;
    int pg_base = 0;
    int core_id = 0;
    int cosq;
    int i = 0;
    bcm_gport_t vsq_gport;
    int pg_vsq_id;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    if (device_type == DEVICE_TYPE_JERICHO) {
        int base_voq;
        base_voq = 0x20; /* egress port 1. "IRR_DESTINATION_TABLE" */

        /*Set voq for each traffic class*/
        bcm_gport_t vsq_gport_cttc;
        bcm_gport_t queue;
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(queue, BCM_CORE_ALL, base_voq);
        for (cosq = 0; cosq < 8; cosq++){
            BCM_COSQ_GPORT_VSQ_CTTC_SET(vsq_gport_cttc, BCM_CORE_ALL, 0, 2, cosq);
            rv = bcm_cosq_gport_vsq_add(unit, vsq_gport_cttc, queue, cosq);
            if (rv != BCM_E_NONE) {
                printf("Failed to add vsq\n");
                return rv;
            }
        }
    }

    /* Create VSQ-E and VSQ-F */
    config.flags = 0;
    config.numq = 8;
    for (i = 0; i < config.numq; i++) {
        config.pg_attributes[i].cosq_bitmap = (1<<i);
        config.pg_attributes[i].ocb_only    = IS_OCB_ONLY;
        config.pg_attributes[i].pool_id     = POOL_ID;
        config.pg_attributes[i].is_lossles  = IS_LOSSLESS;
    }

    rv = bcm_cosq_src_vsqs_gport_add(unit, source_port, &config, &src_port_vsq_gport, &pg_base_vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("Failed to create src vsqs\n");
        return rv;
    }

    printf("pg_base_vsq_gport 0x%x\n", pg_base_vsq_gport);

    /* Get default threshold used for restore */
    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    rv = bcm_cosq_pfc_config_get(unit, pg_base_vsq_gport, 0, flags, &VSQ_THRESHOLD);
    if (rv != BCM_E_NONE) {
        printf("Failed to get threshold\n");
        return rv;
    }

    pg_base = BCM_COSQ_GPORT_VSQ_PG_GET(pg_base_vsq_gport);
    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(pg_base_vsq_gport);

    bcm_cosq_pfc_config_t threshold;

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = pg_base_vsq_gport;
    fc_handler.cosq = -1;
    fc_trigger.flags = fc_handler.flags = 0;
    /* For cosq 0~6, set threshold the same */
    for (cosq = 0; cosq < 7; cosq++) {
        /* Set FC path */
        fc_trigger.cosq = cosq;
        /* setting calender index */
        fc_handler.calender_index = oob_calendar_idx + cosq;
        
        rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
        if (rv != BCM_E_NONE) {
            printf("fc generation configuration failed(%d) in cint_test_fc_gen_vsqf_oob_set_example\n", rv);
            return rv;
        }

        /* Set FC threshold */
        if (device_type == DEVICE_TYPE_JERICHO) {
            threshold.xon_threshold_bd = vsq_thr_1;
            threshold.xoff_threshold_bd = vsq_thr_1;
        } else {
            threshold.xoff_fadt_threshold.thresh_min = vsq_thr_1;
            threshold.xoff_fadt_threshold.thresh_max = vsq_thr_1;
            threshold.xoff_fadt_threshold.alpha = 0;
            threshold.xon_fadt_offset = 0;
        }
        pg_vsq_id = pg_base + cosq;
        BCM_COSQ_GPORT_VSQ_PG_SET(vsq_gport, core_id, pg_vsq_id);
        rv = bcm_cosq_pfc_config_set(unit, vsq_gport, -1, flags, &threshold);
        if (rv != BCM_E_NONE) {
            printf("Failed to set threshold\n");
            return rv;
        }
    }

    /* For cosq 7, set it to a large value so that threshold won't be reached */
    cosq = 7;

    /* Set FC path */
    fc_trigger.cosq = cosq;
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx + cosq;
    
    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed(%d) in cint_test_fc_gen_vsqf_oob_set_example\n", rv);
        return rv;
    }

    /* Set FC threshold */
    if (device_type == DEVICE_TYPE_JERICHO) {
        threshold.xon_threshold_bd = vsq_thr_2;
        threshold.xoff_threshold_bd = vsq_thr_2;
    } else {
        threshold.xoff_fadt_threshold.thresh_min = vsq_thr_2;
        threshold.xoff_fadt_threshold.thresh_max = vsq_thr_2;
        threshold.xoff_fadt_threshold.alpha = 0;
        threshold.xon_fadt_offset = 0;
    }
    pg_vsq_id = pg_base + cosq;
    BCM_COSQ_GPORT_VSQ_PG_SET(vsq_gport, core_id, pg_vsq_id);
    rv = bcm_cosq_pfc_config_set(unit, vsq_gport, -1, flags, &threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed to set threshold\n");
        return rv;
    }

    return rv;
}

/*    Reset PFC VSQ to OOB generation    */
int cint_test_fc_gen_vsqf_oob_unset_example(int unit, int source_port, int oob_port, int oob_calendar_idx, int flags)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t local_gport;
    bcm_gport_t src_port_vsq_gport;
    bcm_gport_t pg_base_vsq_gport;
    bcm_cosq_src_vsqs_gport_config_t config;
    int src_port_vsq_index = 0;
    int pg_base = 0;
    int core_id = 0;
    int cosq;
    int i = 0;
    bcm_gport_t vsq_gport;
    int base_voq;
    int pg_vsq_id;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /* Create VSQ-E and VSQ-F */
    config.flags = 0;
    config.numq = 8;
    for (i = 0; i < config.numq; i++) {
        config.pg_attributes[i].cosq_bitmap = (1<<i);
        config.pg_attributes[i].ocb_only    = IS_OCB_ONLY;
        config.pg_attributes[i].pool_id     = POOL_ID;
        config.pg_attributes[i].is_lossles  = IS_LOSSLESS;
    }

    rv = bcm_cosq_src_vsqs_gport_get(unit, source_port, &config, &src_port_vsq_gport, &pg_base_vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("Failed to create src vsqs\n");
        return rv;
    }

    pg_base = BCM_COSQ_GPORT_VSQ_PG_GET(pg_base_vsq_gport);
    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(pg_base_vsq_gport);

    fc_trigger.port = pg_base_vsq_gport;

    if (device_type == DEVICE_TYPE_JERICHO) {
        flags = 0;
    }
    for (cosq = 0; cosq < 8; cosq++) {
        BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
        fc_trigger.cosq = cosq;
        fc_trigger.flags = fc_handler.flags = 0;
        /* setting target cosq */
        fc_handler.cosq = -1;
        /* setting calender index */
        fc_handler.calender_index = oob_calendar_idx + cosq;

        rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
        if (rv != BCM_E_NONE) {
            printf("fc generation configuration failed(%d) in cint_test_fc_gen_vsqf_oob_unset_example\n", rv);
            return rv;
        }

        pg_vsq_id = pg_base + cosq;
        BCM_COSQ_GPORT_VSQ_PG_SET(vsq_gport, core_id, pg_vsq_id);
        /* Reset FC threshold to default*/
        rv = bcm_cosq_pfc_config_set(unit, vsq_gport, 0, flags, &VSQ_THRESHOLD);
        if (rv != BCM_E_NONE) {
            printf("Failed to set threshold\n");
            return rv;
        }
    }

    return rv;
}

/* set fc reception(an rx slot on OOB or inband calender) and fc target(interface) */
int cint_test_fc_rcv_cal_to_intf_set_example(int unit, int target_port, int port, int calendar_idx, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    /* Config oob fc */
    /* setting port */
    BCM_GPORT_LOCAL_SET(child_port, target_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_target.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }

    if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, port);
    }
    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = -1;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_cal_to_intf_set_example\n", rv);
    }

    return rv;
}

/* Unset oob/inband calender -> interface receive */
int cint_test_fc_rcv_cal_to_intf_unset_example(int unit, int target_port, int port, int calendar_idx, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    /* Config oob fc */
    /* setting port */
    BCM_GPORT_LOCAL_SET(child_port, target_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_target.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }

    if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, port);
    }
    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = -1;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_cal_to_intf_unset_example\n", rv);
    }

    return rv;
}


/* set fc reception(an rx slot on OOB or inband calender) and fc target(a port) */
int cint_test_fc_rcv_cal_to_port_set_example(int unit, int target_port, int port, int calendar_idx, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port) /*Can be local, or modport etc..*/;
    if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, port);
    }
    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = -1;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_cal_to_port_set_example\n", rv);
    }

    return rv;
}

/* Unset oob/inband calender -> port receive */
int cint_test_fc_rcv_cal_to_port_unset_example(int unit, int target_port, int port, int calendar_idx, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port) /*Can be local, or modport etc..*/;
    if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, port);
    }
    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = -1;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_cal_to_port_unset_example\n", rv);
    }

    return rv;
}

/* set fc reception(an rx slot on OOB or inband calender) and fc target(port + cosq) */
int cint_test_fc_rcv_cal_to_port_cosq_set_example(int unit, int target_port, int port, int calendar_idx, int target_cosq, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, port);
    }
    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = target_cosq;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_cal_to_port_cosq_set_example\n", rv);
    }

    return rv;
}

/* Unset oob/inband calender -> port+cosq receive */
int cint_test_fc_rcv_cal_to_port_cosq_unset_example(int unit, int target_port, int port, int calendar_idx, int target_cosq, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, port);
    }
    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = target_cosq;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_cal_to_port_cosq_unset_example\n", rv);
    }

    return rv;
}

/*
 * set fc reception(an rx slot on OOB or inband calender) and fc target(traffic specified by bitmap)
 * flags can be one of below:
 *    BCM_COSQ_FC_INHERIT_UP
 *    BCM_COSQ_FC_INHERIT_DOWN
 *    BCM_COSQ_FC_PORT_OVER_PFC
 */
int cint_test_fc_rcv_cal_to_bitmap_set_example(int unit, int target_port, int port, int calendar_idx, int target_cosq, int flags, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config oob fc */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port) /*Can be local, or modport etc..*/;
    if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, port);
    }
    /* setting flags */
    fc_target.flags = flags;
    fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = target_cosq;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_cal_to_bitmap_set_example\n", rv);
    }

    return rv;
}

/* Unset oob/inband calender -> generic bitmap receive */
int cint_test_fc_rcv_cal_to_bitmap_unset_example(int unit, int target_port, int port, int calendar_idx, int target_cosq, int flags, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config oob fc */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port) /*Can be local, or modport etc..*/;
    if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, port);
    }
    /* setting flags */
    fc_target.flags = flags;
    fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = target_cosq;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_cal_to_bitmap_unset_example\n", rv);
    }

    return rv;
}

/* 
 *set fc reception from COE calender to fc target(interface) 
 * variable: is_oob is only for JR1 to differentiate using OOB or inband calender
*/
int cint_test_fc_rcv_coe_oob_intf_set_example(int unit, int target_port, int calendar_port, int calendar_idx, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /* Config oob fc */
    /* setting port */
    BCM_GPORT_LOCAL_SET(child_port, target_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_target.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }

    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        BCM_GPORT_CONGESTION_COE_SET(fc_reception.port, calendar_port);
    } else if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, calendar_port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, calendar_port);
    }

    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = -1;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_test_fc_rcv_coe_oob_intf_set_example\n", rv);
    }

    return rv;
}

/* Unset coe oob -> interface receive */
int cint_test_fc_rcv_coe_oob_intf_unset_example(int unit, int target_port, int calendar_port, int calendar_idx, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /* Config oob fc */
    /* setting port */
    BCM_GPORT_LOCAL_SET(child_port, target_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_target.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }

    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        BCM_GPORT_CONGESTION_COE_SET(fc_reception.port, calendar_port);
    } else if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, calendar_port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, calendar_port);
    }

    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = -1;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_test_fc_rcv_coe_oob_intf_unset_example\n", rv);
    }

    return rv;
}


/* set fc reception from COE calender to fc target(a port) */
int cint_test_fc_rcv_coe_oob_port_set_example(int unit, int target_port, int calendar_port, int calendar_idx, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port);

    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        BCM_GPORT_CONGESTION_COE_SET(fc_reception.port, calendar_port);
    } else if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, calendar_port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, calendar_port);
    }

    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = -1;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_test_fc_rcv_coe_oob_port_set_example\n", rv);
    }

    return rv;
}

/* Unset coe oob -> port receive */
int cint_test_fc_rcv_coe_oob_port_unset_example(int unit, int target_port, int calendar_port, int calendar_idx, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port);

    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        BCM_GPORT_CONGESTION_COE_SET(fc_reception.port, calendar_port);
    } else if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, calendar_port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, calendar_port);
    }

    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = -1;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_test_fc_rcv_coe_oob_port_unset_example\n", rv);
    }

    return rv;
}

/* set fc reception from COE calender to fc target(port + cosq) */
int cint_test_fc_rcv_coe_oob_port_cosq_set_example(int unit, int target_port, int calendar_port, int calendar_idx, int target_cosq, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port);

    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        BCM_GPORT_CONGESTION_COE_SET(fc_reception.port, calendar_port);
    } else if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, calendar_port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, calendar_port);
    }

    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = target_cosq;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_test_fc_rcv_coe_oob_port_cosq_set_example\n", rv);
    }

    return rv;
}

/* Unset coe oob -> port+cosq receive */
int cint_test_fc_rcv_coe_oob_port_cosq_unset_example(int unit, int target_port, int calendar_port, int calendar_idx, int target_cosq, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port);

    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        BCM_GPORT_CONGESTION_COE_SET(fc_reception.port, calendar_port);
    } else if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, calendar_port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, calendar_port);
    }

    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */
    fc_target.cosq = target_cosq;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_test_fc_rcv_coe_oob_port_cosq_unset_example\n", rv);
    }

    return rv;
}

/* set fc reception from COE calender to generic bitmap mapping
 * 3 flags are supported
 *    BCM_COSQ_FC_PORT_OVER_PFC
 *         additional targets are used as reaction points to the same FC reception source
 *         For example, if receive PFC1, then all target cosqs will react to it.
 *         Note: target_cosq has to be configured to -1
 *    BCM_COSQ_FC_INHERIT_UP
 *         can be used to set the same path for cosqs above the given cosq
 *         For example, receive PFC1, target_cosq configured to 4, then all egq >= 4 will react to it
 *    BCM_COSQ_FC_INHERIT_DOWN
 *         can be used to set the same path for cosqs bellow the given cosq
 *         For example, receive PFC1, target_cosq configured to 4, then all egq <= 4 will react to it
 */
int cint_test_fc_rcv_coe_bitmap_set_example(int unit, int target_port, int calendar_port, int calendar_idx, int target_cosq, int flags, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port);

    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        BCM_GPORT_CONGESTION_COE_SET(fc_reception.port, calendar_port);
    } else if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, calendar_port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, calendar_port);
    }

    /* setting flags */
    fc_reception.flags = 0;
    fc_target.flags = flags;
    /* setting cosq */
    fc_target.cosq = target_cosq;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_test_fc_rcv_coe_bitmap_set_example\n", rv);
    }

    return rv;
}

/* Unset coe oob -> bitmap */
int cint_test_fc_rcv_coe_bitmap_unset_example(int unit, int target_port, int calendar_port, int calendar_idx, int target_cosq, int flags, int is_oob)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port);

    if ((device_type == DEVICE_TYPE_JR2) || (device_type == DEVICE_TYPE_Q2A)) {
        BCM_GPORT_CONGESTION_COE_SET(fc_reception.port, calendar_port);
    } else if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception.port, calendar_port);
    } else {
        BCM_GPORT_LOCAL_SET(fc_reception.port, calendar_port);
    }

    /* setting flags */
    fc_reception.flags = 0;
    fc_target.flags = flags;
    /* setting cosq */
    fc_target.cosq = target_cosq;
    fc_reception.cosq = -1;
    /* setting calender index */
    fc_reception.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_test_fc_rcv_coe_bitmap_unset_example\n", rv);
    }

    return rv;
}

/*
 * NIF almost full indication to LLFC/PFC generation
 *   1. Enable LLFC or PFC transmission
 *   2. Set threshold, flag: BCM_COSQ_THRESHOLD_ETH_PORT_LLFC or BCM_COSQ_THRESHOLD_ETH_PORT_PFC
 */
int cint_test_fc_gen_mlf_set_example(int unit, bcm_port_control_t fc_control,
                                     int port, int target_cosq, int set_thr, int clear_thr)
{
    int rv = BCM_E_NONE;
    bcm_gport_t child_port;
    bcm_cosq_threshold_t threshold;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t local_trigger_gport;

    if ((fc_control != bcmPortControlLLFCTransmit) && (fc_control != bcmPortControlPFCTransmit)) {
        printf("fc_control in cint_test_fc_mlf_thr_set_example should be either bcmPortControlPFCTransmit or bcmPortControlLLFCTransmit\n");
        return BCM_E_PARAM;
    }

    /* set target port fc inbnd mode*/
    rv = bcm_port_control_set(unit, port, fc_control, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to set port fc inbnd mode in bcm_port_control_set\n", rv);
        return rv;
    }

    /* configure inbnd fc path */
    BCM_GPORT_LOCAL_SET(child_port, port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_trigger.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }

    fc_trigger.flags = 0;
    if (TARGET_PORT_IMPLICIT == 0) {
        BCM_GPORT_LOCAL_SET(fc_handler.port, port);
        fc_handler.flags = 0;
    } else {
        fc_handler.port = 0;
        fc_handler.flags = BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;
    }
    fc_trigger.cosq = -1;
    fc_handler.cosq = target_cosq;   /* -1 indicates LLFC */

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed in cint_test_fc_gen_mlf_set_example\n");
        return rv;
    }

    /* configure threshold */
    BCM_GPORT_LOCAL_SET(local_trigger_gport, port);

    if (fc_control == bcmPortControlLLFCTransmit) {
        threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_LLFC | BCM_COSQ_THRESHOLD_SET;
        threshold.value = set_thr;
        rv = bcm_cosq_gport_threshold_set(unit, local_trigger_gport, 0, &threshold);
        if (rv != BCM_E_NONE) {
            printf("setting fc threshold failed(%d) in bcm_cosq_gport_threshold_set\n", rv);
            return rv;
        }

        threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_LLFC | BCM_COSQ_THRESHOLD_CLEAR;
        threshold.value = clear_thr;
        rv = bcm_cosq_gport_threshold_set(unit, local_trigger_gport, 0, &threshold);
        if (rv != BCM_E_NONE) {
            printf("setting fc threshold failed(%d) in bcm_cosq_gport_threshold_set\n", rv);
            return rv;
        }
    } else if (fc_control == bcmPortControlPFCTransmit) {
        threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_PFC | BCM_COSQ_THRESHOLD_SET;
        threshold.value = set_thr;
        rv = bcm_cosq_gport_threshold_set(unit, local_trigger_gport, 0, &threshold);
        if (rv != BCM_E_NONE) {
            printf("setting fc threshold failed(%d) in bcm_cosq_gport_threshold_set\n", rv);
            return rv;
        }

        threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_PFC | BCM_COSQ_THRESHOLD_CLEAR;
        threshold.value = clear_thr;
        rv = bcm_cosq_gport_threshold_set(unit, local_trigger_gport, 0, &threshold);
        if (rv != BCM_E_NONE) {
            printf("setting fc threshold failed(%d) in bcm_cosq_gport_threshold_set\n", rv);
            return rv;
        }
    }

    return rv;
}

/*
 *  Unset NIF almost full indication to LLFC/PFC generation
 *  the threshold in the example is a fixed value
 *    for different device, the default value is different, need to set accordingly
 */
int cint_test_fc_gen_mlf_unset_example(int unit, bcm_port_control_t fc_control, int port, int target_cosq)
{
    int rv = BCM_E_NONE;
    bcm_gport_t child_port;
    bcm_cosq_threshold_t threshold;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t local_trigger_gport;

    /* configure inbnd fc path */
    BCM_GPORT_LOCAL_SET(child_port, port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_trigger.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }

    fc_trigger.flags = 0;
    if (TARGET_PORT_IMPLICIT == 0) {
        BCM_GPORT_LOCAL_SET(fc_handler.port, port);
        fc_handler.flags = 0;
    } else {
        fc_handler.port = 0;
        fc_handler.flags = BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;
    }
    fc_trigger.cosq = -1;
    fc_handler.cosq = target_cosq;   /* -1 indicates LLFC */

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed in cint_test_fc_gen_mlf_unset_example\n");
        return rv;
    }

    /* configure threshold */
    BCM_GPORT_LOCAL_SET(local_trigger_gport, port);

    if (fc_control == bcmPortControlLLFCTransmit) {
        threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_LLFC | BCM_COSQ_THRESHOLD_SET;
        threshold.value = 0x80;
        rv = bcm_cosq_gport_threshold_set(unit, local_trigger_gport, 0, &threshold);
        if (rv != BCM_E_NONE) {
            printf("setting fc threshold failed(%d) in bcm_cosq_gport_threshold_set\n", rv);
            return rv;
        }

        threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_LLFC | BCM_COSQ_THRESHOLD_CLEAR;
        threshold.value = 0x78;
        rv = bcm_cosq_gport_threshold_set(unit, local_trigger_gport, 0, &threshold);
        if (rv != BCM_E_NONE) {
            printf("setting fc threshold failed(%d) in bcm_cosq_gport_threshold_set\n", rv);
            return rv;
        }
    } else if (fc_control == bcmPortControlPFCTransmit) {
        threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_PFC | BCM_COSQ_THRESHOLD_SET;
        threshold.value = 0x80;
        rv = bcm_cosq_gport_threshold_set(unit, local_trigger_gport, 0, &threshold);
        if (rv != BCM_E_NONE) {
            printf("setting fc threshold failed(%d) in bcm_cosq_gport_threshold_set\n", rv);
            return rv;
        }

        threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_PFC | BCM_COSQ_THRESHOLD_CLEAR;
        threshold.value = 0x78;
        rv = bcm_cosq_gport_threshold_set(unit, local_trigger_gport, 0, &threshold);
        if (rv != BCM_E_NONE) {
            printf("setting fc threshold failed(%d) in bcm_cosq_gport_threshold_set\n", rv);
            return rv;
        }
    }

    return rv;
}

/*
 * Set NIF almost full indication to OOB generation
 * For threshold, in this example, use LLFC threshold
 */
int cint_test_fc_gen_mlf_oob_set_example(int unit, int source_port, int oob_port, int oob_calendar_idx, int set_thr, int clear_thr)
{
    int rv = BCM_E_NONE;
    bcm_gport_t child_port;
    bcm_cosq_threshold_t threshold;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t local_trigger_gport;

    /*Setting port bcmPortControlLLFCTransmit diable*/
    rv = bcm_port_control_set(unit, source_port, bcmPortControlLLFCTransmit, 0);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port LLFC transmit\n");
        return rv;
    }

    /* configure fc path */
    BCM_GPORT_LOCAL_SET(child_port, source_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_trigger.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.flags = 0;
    fc_trigger.cosq = -1;
    fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed in cint_test_fc_gen_mlf_oob_set_example\n");
        return rv;
    }

    /* configure threshold, use LLFC as example */
    BCM_GPORT_LOCAL_SET(local_trigger_gport, source_port);

    threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_LLFC | BCM_COSQ_THRESHOLD_SET;
    threshold.value = set_thr;
    rv = bcm_cosq_gport_threshold_set(unit, local_trigger_gport, 0, &threshold);
    if (rv != BCM_E_NONE) {
        printf("setting fc threshold failed(%d) in bcm_cosq_gport_threshold_set\n", rv);
        return rv;
    }

    threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_LLFC | BCM_COSQ_THRESHOLD_CLEAR;
    threshold.value = clear_thr;
    rv = bcm_cosq_gport_threshold_set(unit, local_trigger_gport, 0, &threshold);
    if (rv != BCM_E_NONE) {
        printf("setting fc threshold failed(%d) in bcm_cosq_gport_threshold_set\n", rv);
        return rv;
    }

    return rv;
}

/*    Unset NIF almost full indication to OOB generation    */
int cint_test_fc_gen_mlf_oob_unset_example(int unit, int source_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_gport_t child_port;
    bcm_cosq_threshold_t threshold;
    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;

    /* configure fc path */
    BCM_GPORT_LOCAL_SET(child_port, source_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_trigger.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.flags = 0;
    fc_trigger.cosq = -1;
    fc_handler.flags = 0;
    /* setting target cosq */
    fc_handler.cosq = -1;
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("fc generation configuration failed in cint_test_fc_gen_mlf_oob_unset_example\n");
        return rv;
    }

    return rv;
}

/* set fc reception from source:CMIC to:target(interface) */
int cint_test_fc_rcv_cmic_intf_set_example(int unit, int cpu_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_source;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_source.port, cpu_port);
    BCM_GPORT_LOCAL_SET(child_port, cpu_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_target.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }

    /* setting flags */
    fc_target.flags = fc_source.flags = 0;
    /* setting cosq */
    fc_target.cosq = -1;
    fc_source.cosq = -1;
    /* setting calender index */
    fc_source.calender_index = -1;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_source, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_cmic_intf_set_example\n", rv);
    }

    return rv;
}

/* delete fc reception from source:CMIC to:target(interface) */
int cint_test_fc_rcv_cmic_intf_unset_example(int unit, int cpu_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_source;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_source.port, cpu_port);
    BCM_GPORT_LOCAL_SET(child_port, cpu_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_target.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }

    /* setting flags */
    fc_target.flags = fc_source.flags = 0;
    /* setting cosq */
    fc_target.cosq = -1;
    fc_source.cosq = -1;
    /* setting calender index */
    fc_source.calender_index = -1;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_source, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_cmic_intf_unset_example\n", rv);
    }

    return rv;
}

/* set fc reception from source:CMIC to:target port + cosq */
int cint_test_fc_rcv_cmic_port_cosq_set_example(int unit, int cpu_port, int cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_source;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_source.port, cpu_port);

    if (TARGET_PORT_IMPLICIT == 0) {
        BCM_GPORT_LOCAL_SET(fc_target.port, cpu_port);
        fc_target.flags = 0;
    } else {
        fc_target.port = 0;
        fc_target.flags = BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;
    }

    /* setting flags */
    fc_source.flags = 0;
    /* setting cosq */
    fc_source.cosq = cosq;
    /* target cosq isn't used here, register: CFC_CMIC_RX_FC_MAP is configured on init */
    fc_target.cosq = 0;
    /* setting calender index */
    fc_source.calender_index = -1;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_source, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_cmic_intf_set_example\n", rv);
    }

    return rv;
}


/* Delete fc reception from source:CMIC to:target port + cosq */
int cint_test_fc_rcv_cmic_port_cosq_unset_example(int unit, int cpu_port, int cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_source;
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_source.port, cpu_port);

    if (TARGET_PORT_IMPLICIT == 0) {
        BCM_GPORT_LOCAL_SET(fc_target.port, cpu_port);
        fc_target.flags = 0;
    } else {
        fc_target.port = 0;
        fc_target.flags = BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;
    }

    /* setting flags */
    fc_source.flags = 0;
    /* setting cosq */
    fc_source.cosq = cosq;
    /* target cosq isn't used here, register: CFC_CMIC_RX_FC_MAP is configured on init */
    fc_target.cosq = 0;
    /* setting calender index */
    fc_source.calender_index = -1;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_source, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("fc reception configuration failed(%d) in cint_test_fc_rcv_cmic_port_cosq_unset_example\n", rv);
    }

    return rv;
}

/* Global VSQ threshold setting for JR1, QAX */
int cint_test_fc_gl_thr_set(int unit, global_vsq_src_e global_vsq_src, int priority, int thr_set, int thr_clr)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_cosq_threshold_t fc_threshold;

    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    /* qax has only one core */
    if (device_type == DEVICE_TYPE_QAX) {
        vsq_inf.core_id = 0;
    } else {
        vsq_inf.core_id = CORE;
    }
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    /*Setting GL vsq threshold*/
    bcm_cosq_threshold_t_init(&fc_threshold);
    switch (priority) {
        case 0:
            fc_threshold.priority = BCM_COSQ_HIGH_PRIORITY;
            break;
        case 1:
            fc_threshold.priority = BCM_COSQ_LOW_PRIORITY;
            break;
        defalut:
            if (priority != BCM_COSQ_HIGH_PRIORITY && priority != BCM_COSQ_LOW_PRIORITY)
            return BCM_E_PARAM;
    }

    fc_threshold.flags = BCM_COSQ_THRESHOLD_INGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL|BCM_COSQ_THRESHOLD_SET;
    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            fc_threshold.type = bcmCosqThresholdBufferDescriptorBuffers;
            break;
        case global_vsq_src_mini_db:
            fc_threshold.type = bcmCosqThresholdMiniDbuffs;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_MULTICAST;
            break;
        case global_vsq_src_full_db:
            fc_threshold.type = bcmCosqThresholdFullDbuffs;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_MULTICAST;
            break;
        case global_vsq_src_ocb_db:
            fc_threshold.type = bcmCosqThresholdDbuffs;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_OCB;
            break;
        case global_vsq_src_mix_pool0:
            fc_threshold.type = bcmCosqThresholdBufferDescriptors;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_POOL0;
            break;
        case global_vsq_src_mix_pool1:
            fc_threshold.type = bcmCosqThresholdBufferDescriptors;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_POOL1;
            break;
        case global_vsq_src_ocb_pool0:
            fc_threshold.type = bcmCosqThresholdBufferDescriptors;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_POOL0 | BCM_COSQ_THRESHOLD_OCB;
            break;
        case global_vsq_src_ocb_pool1:
            fc_threshold.type = bcmCosqThresholdBufferDescriptors;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_POOL1 | BCM_COSQ_THRESHOLD_OCB;
            break;
        case global_vsq_src_ocb_pdb:
            fc_threshold.type = bcmCosqThresholdPacketDescriptorBuffers;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_OCB;
            break;
        case global_vsq_src_p0_db:
            fc_threshold.type = bcmCosqThresholdDbuffs;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_POOL0;
            break;
        case global_vsq_src_p1_db:
            fc_threshold.type = bcmCosqThresholdDbuffs;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_POOL1;
            break;
        case global_vsq_src_p0_pd:
            fc_threshold.type = bcmCosqThresholdPacketDescriptors;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_POOL0;
            break;
        case global_vsq_src_p1_pd:
            fc_threshold.type = bcmCosqThresholdPacketDescriptors;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_POOL1;
            break;
        case global_vsq_src_p0_byte:
            fc_threshold.type = bcmCosqThresholdBytes;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_POOL0;
            break;
        case global_vsq_src_p1_byte:
            fc_threshold.type = bcmCosqThresholdBytes;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_POOL1;
            break;
        case global_vsq_src_hdrm_db:
            fc_threshold.type = bcmCosqThresholdDbuffs;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_HEADROOM;
            break;
        case global_vsq_src_hdrm_pd:
            fc_threshold.type = bcmCosqThresholdPacketDescriptors;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_HEADROOM;
            break;
        default: 
            return BCM_E_PARAM;
    }

    fc_threshold.value = thr_set;
    rv = bcm_cosq_gport_threshold_set(unit, vsq_gport, 0, &fc_threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to set fc set threshold in cint_test_fc_gl_thr_set_example\n", rv);
        return rv;
    }

    fc_threshold.flags &= ~(BCM_COSQ_THRESHOLD_SET);
    fc_threshold.flags |= BCM_COSQ_THRESHOLD_CLEAR;
    fc_threshold.value = thr_clr;
    rv = bcm_cosq_gport_threshold_set(unit, vsq_gport, 0, &fc_threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to set fc clear threshold in cint_test_fc_gl_thr_set_example\n", rv);
        return rv;
    }

    return rv;
}

/* Global VSQ threshold setting for JR2 */
int cint_test_jr2_fc_gl_thr_set(int unit, global_vsq_src_e global_vsq_src, int priority, int thr_set, int thr_clr)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_cosq_static_threshold_info_t threshold_info;
    bcm_cosq_static_threshold_t threshold;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    vsq_inf.core_id = CORE;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    /*Setting GL vsq threshold*/
    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            threshold_info.index = BCM_COSQ_TH_INDEX_PRIO_SET(priority);
            threshold_info.resource_type = bcmResourceDramBundleDescriptorsBuffers;
            threshold_info.is_free_resource = 1;
            break;
        case global_vsq_src_ocb_db:
            threshold_info.index = BCM_COSQ_TH_INDEX_PRIO_SET(priority);
            threshold_info.resource_type = bcmResourceOcbBuffers;
            threshold_info.is_free_resource = 1;
            break;
        case global_vsq_src_ocb_pdb:
            threshold_info.index = BCM_COSQ_TH_INDEX_PRIO_SET(priority);
            threshold_info.resource_type = bcmResourceOcbPacketDescriptorsBuffers;
            threshold_info.is_free_resource = 1;
            break;
        case global_vsq_src_p0_db:
            threshold_info.index = BCM_COSQ_TH_INDEX_POOL_PRIO_SET(0, priority);
            threshold_info.resource_type = bcmResourceOcbBuffers;
            threshold_info.is_free_resource = 1;
            break;
        case global_vsq_src_p1_db:
            threshold_info.index = BCM_COSQ_TH_INDEX_POOL_PRIO_SET(1, priority);
            threshold_info.resource_type = bcmResourceOcbBuffers;
            threshold_info.is_free_resource = 1;
            break;
        case global_vsq_src_p0_pd:
            threshold_info.index = BCM_COSQ_TH_INDEX_POOL_PRIO_SET(0, priority);
            threshold_info.resource_type = bcmResourceOcbPacketDescriptors;
            threshold_info.is_free_resource = 1;
            break;
        case global_vsq_src_p1_pd:
            threshold_info.index = BCM_COSQ_TH_INDEX_POOL_PRIO_SET(1, priority);
            threshold_info.resource_type = bcmResourceOcbPacketDescriptors;
            threshold_info.is_free_resource = 1;
            break;
        case global_vsq_src_p0_byte:
            threshold_info.index = BCM_COSQ_TH_INDEX_POOL_PRIO_SET(0, priority);
            threshold_info.resource_type = bcmResourceBytes;
            threshold_info.is_free_resource = 1;
            break;
        case global_vsq_src_p1_byte:
            threshold_info.index = BCM_COSQ_TH_INDEX_POOL_PRIO_SET(1, priority);
            threshold_info.resource_type = bcmResourceBytes;
            threshold_info.is_free_resource = 1;
            break;
        case global_vsq_src_hdrm_db:
            threshold_info.index = BCM_COSQ_TH_INDEX_INVALID;
            threshold_info.resource_type = bcmResourceOcbBuffers;
            threshold_info.is_free_resource = 1;
            break;
        case global_vsq_src_hdrm_pd:
            threshold_info.index = BCM_COSQ_TH_INDEX_INVALID;
            threshold_info.resource_type = bcmResourceOcbPacketDescriptors;
            threshold_info.is_free_resource = 1;
            break;
        default: 
            return BCM_E_PARAM;
    }

    threshold_info.threshold_action = bcmCosqThreshActionFc;
    threshold_info.gport = vsq_gport;
    threshold.thresh_set = thr_set;
    threshold.thresh_clear = thr_clr;
    rv = bcm_cosq_gport_static_threshold_set( unit, 0, &threshold_info, &threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to set fc set threshold in cint_test_jr2_fc_gl_thr_set\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure dram bound to 0 for the port's rate class,
 * this should cause certain percentage of the traffic to
 * arrive to the DRAM.
 */
int cint_redirect_traffic_to_dram(int unit, bcm_port_t logical_port)
{
    bcm_cosq_fadt_info_t fadt_info;
    bcm_cosq_fadt_threshold_t fadt_threshold;
    bcm_cosq_threshold_t dram_bound_recovery_threshold;
    bcm_switch_profile_mapping_t profile_mapping;
    bcm_gport_t voq_gport, rate_class_gport;
    int base_voq;
    int rv = 0;

    /** get VOQ ID - assuming DNX reference application */
    rv = appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit, 0, logical_port, &base_voq);
    if (BCM_FAILURE(rv))
    {
        printf("appl_dnx_e2e_scheme_logical_port_to_base_voq_get failed \n");
        return rv;
    }
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, base_voq);

    /** get rate class */
    profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
    rv = bcm_cosq_profile_mapping_get(unit, voq_gport, 0, 0, &profile_mapping);
    if (BCM_E_NONE != rv)
    {
        printf("bcm_cosq_profile_mapping_get failed \n");
        return rv;
    }

    /** set rate class to DRAM only */
    rate_class_gport = profile_mapping.mapped_profile;

    /** configure DRAM bound info */
    fadt_info.gport = rate_class_gport;
    fadt_info.cosq = 0;
    fadt_info.thresh_type = bcmCosqFadtDramBound;
    fadt_info.resource_type = bcmResourceOcbPacketDescriptors;
    fadt_threshold.thresh_min = 0;
    fadt_threshold.thresh_max = 0;
    fadt_threshold.alpha = 0;
    fadt_threshold.resource_range_min = 0x100;
    fadt_threshold.resource_range_max = 0x1000;
    /** set configuration */
    rv = bcm_cosq_gport_fadt_threshold_set(unit, 0, &fadt_info, &fadt_threshold);
    if (rv != BCM_E_NONE)
    {
        printf("failed - bcm_cosq_gport_fadt_threshold_set");
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * Configure PFC Deadlock recovery mechanism
 */
int cint_pfc_deadlock_recovery_config(int unit, bcm_port_t logical_port, int priority, uint32 detection_time_ms, int recovery_time_ms, int recovery_action)
{
    int rv = 0;
    bcm_instru_synced_counters_config_t config;
    uint32 start_time_hi_bits = 0xFFFFFFFF;
    uint32 start_time_lo_bits = 0xFFFFFFFF;
    uint64 period;

    /**
     * Configure pulse generator.
     * Detection timer must be be contained within the pulse generator period,
     * as each pulse zeroes the counters. So we add 100ms to the pulse period.
     * Also the period is in nanosecond, and detection_time_ms is in milliseconds, so multiply by a million.
     */
    COMPILER_64_SET(config.period, 0, detection_time_ms);
    COMPILER_64_ADD_32(config.period, 100);
    COMPILER_64_UMUL_32(config.period, 1000000);
    config.enable=1;
    config.is_eventor_collection=0;
    config.is_continuous=1;
    COMPILER_64_SET(config.start_time, start_time_hi_bits, start_time_lo_bits);
    rv = bcm_instru_synced_counters_config_set(unit, 0, bcmInstruSyncedCountersTypeNif, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Failed - bcm_instru_synced_counters_config_set");
        return rv;
    }

    /**
     * Configure the Chip behavior during recovery
     */
    rv = bcm_switch_control_set(unit, bcmSwitchPFCDeadlockRecoveryAction,recovery_action);
    if (rv != BCM_E_NONE)
    {
        printf("Failed - bcm_switch_control_set");
        return rv;
    }

    /**
     * Configure the Detection timer's granularity to 1 ms.
     */
    rv = bcm_cosq_pfc_deadlock_control_set(unit, logical_port, priority, bcmCosqPFCDeadlockTimerGranularity, bcmCosqPFCDeadlockTimerInterval1MiliSecond);
    if (rv != BCM_E_NONE)
    {
        printf("Failed - bcm_cosq_pfc_deadlock_control_set with bcmCosqPFCDeadlockTimerGranularity");
        return rv;
    }

    /**
     * Configure the Detection timer
     */
    rv = bcm_cosq_pfc_deadlock_control_set(unit, logical_port, priority, bcmCosqPFCDeadlockDetectionTimer, detection_time_ms);
    if (rv != BCM_E_NONE)
    {
        printf("Failed - bcm_cosq_pfc_deadlock_control_set with bcmCosqPFCDeadlockDetectionTimer");
        return rv;
    }

    /**
     * Configure the Recovery timer
     */
    rv = bcm_cosq_pfc_deadlock_control_set(unit, logical_port, priority, bcmCosqPFCDeadlockRecoveryTimer, recovery_time_ms);
    if (rv != BCM_E_NONE)
    {
        printf("Failed - bcm_cosq_pfc_deadlock_control_set with bcmCosqPFCDeadlockRecoveryTimer");
        return rv;
    }

    /**
     * Enable the PFC Deadlock recovery after all configuration is done
     */
    rv = bcm_cosq_pfc_deadlock_control_set(unit, logical_port, priority, bcmCosqPFCDeadlockDetectionAndRecoveryEnable, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Failed - bcm_cosq_pfc_deadlock_control_set with bcmCosqPFCDeadlockDetectionAndRecoveryEnable");
        return rv;
    }

    return BCM_E_NONE;
}


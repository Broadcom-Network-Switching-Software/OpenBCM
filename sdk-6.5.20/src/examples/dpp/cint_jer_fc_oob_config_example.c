/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_jer_fc_oob_config_example.c
 */
/*
 * Purpose: 
 *     Example of fc generation and reception on OOB interface.
 */

/*##############################################################*/
/*###################    OOB GENERATION     ####################*/
/*##############################################################*/
/* set fc generation(global resource) and fc target(a tx slot on OOB)  for Jericho, not including QAX/QUX */
int cint_jer_test_fc_oob_gl_gen_set_example(int unit, int oob_port, int oob_calendar_idx, int global_vsq_src, int priority, int shaped_port)
{
    int rv = BCM_E_NONE;
    int flags = 0;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    bcm_cosq_fc_endpoint_t fc_target_array[12];
    int target_max = 12;
    int target_count = 0;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    /* Trigger global fc : Setting FC threshold and Shaping local port*/
    rv = cint_jer_test_fc_gl_trigger_set_example(unit, global_vsq_src, priority, shaped_port);
    if (rv != BCM_E_NONE) {
        printf("Trigger global fc failed(%d) in cint_jer_test_fc_oob_gl_gen_set_example\n", rv);
        return rv;
    }

    /* Configuring oob fc { */
    /* setting port */

    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            flags = BCM_COSQ_FC_BDB;
            break;
        case global_vsq_src_mini_db: 
            flags = BCM_COSQ_FC_MINI_DB;
            break;
        case global_vsq_src_full_db: 
            flags = BCM_COSQ_FC_FULL_DB;
            break;
        case global_vsq_src_ocb_db: 
            flags = BCM_COSQ_FC_OCB_DB;
            break;
        default: 
            return BCM_E_PARAM;
    }

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = flags | BCM_COSQ_FC_MASK_POOL_INDICATION;
    fc_handler.flags = 0;
    /* setting priority */    
    fc_trigger.priority = priority;
    /* setting cosq */ 
    fc_trigger.cosq = 0;
    fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("oob fc generation configuration failed(%d) in cint_jer_test_fc_oob_gl_gen_set_example\n", rv);
        return rv;
    }
    /* Configuring oob fc } */

    print bcm_cosq_fc_path_get(unit, bcmCosqFlowControlGeneration, &fc_trigger, target_max, fc_target_array, &target_count);
    print fc_target_array;
    print target_count;

    return rv;
}

/* set fc generation(global resource) and fc target(a tx slot on OOB)  for QAX/QUX */
int cint_qax_test_fc_oob_gl_gen_set_example(int unit, int oob_port, int oob_calendar_idx, int global_vsq_src, int priority, int shaped_port)
{
    int rv = BCM_E_NONE;
    int flags = 0;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    /* Trigger global fc : Setting FC threshold and Shaping local port*/
    rv = cint_jer_test_fc_gl_trigger_set_example(unit, global_vsq_src, priority, shaped_port);
    if (rv != BCM_E_NONE) {
        printf("Trigger global fc failed(%d) in cint_jer_test_fc_oob_gl_gen_set_example\n", rv);
        return rv;
    }

    /* Configuring oob fc { */
    sal_memset(&fc_trigger, 0, sizeof(fc_trigger));
    sal_memset(&fc_handler, 0, sizeof(fc_handler));

    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            flags &= ~(BCM_COSQ_FC_IS_OCB_ONLY);
            flags |= BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_ocb_db:
        case global_vsq_src_ocb_pdb:
            flags |= BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_p0_db: 
        case global_vsq_src_p0_pd:
        case global_vsq_src_p0_byte:
            fc_trigger.cosq = 0;
            break;
        case global_vsq_src_p1_db: 
        case global_vsq_src_p1_pd:
        case global_vsq_src_p1_byte:
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_hdrm_db: 
        case global_vsq_src_hdrm_pd:
            flags |= BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        default: 
            return BCM_E_PARAM;
    }

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = flags;
    fc_handler.flags = 0;
    /* setting priority */    
    fc_trigger.priority = priority;
    /* setting cosq */ 
    fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("oob fc generation configuration failed(%d) in cint_qax_test_fc_oob_gl_gen_set_example\n", rv);
        return rv;
    }
    /* Configuring oob fc } */

    return rv;
}

/* unset fc generation(global resource) and fc target(a tx slot on OOB)  for Jericho, not including QAX/QUX */
int cint_jer_test_fc_oob_gl_gen_unset_example(int unit, int oob_port, int oob_calendar_idx, int global_vsq_src, int priority, int shaped_port)
{
    int rv = BCM_E_NONE;
    int flags = 0;

    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;

    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    
    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    /* Trigger global fc : Setting FC threshold and Shaping local port*/
    rv = cint_jer_test_fc_gl_trigger_unset_example(unit, global_vsq_src, priority, shaped_port);
    if (rv != BCM_E_NONE) {
        printf("Trigger global fc failed(%d) in cint_jer_test_fc_oob_gl_gen_unset_example\n", rv);
        return rv;
    }

    /* Configuring oob fc { */
    /* setting port */

    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            flags = BCM_COSQ_FC_BDB;
            break;
        case global_vsq_src_mini_db: 
            flags = BCM_COSQ_FC_MINI_DB;
            break;
        case global_vsq_src_full_db: 
            flags = BCM_COSQ_FC_FULL_DB;
            break;
        case global_vsq_src_ocb_db: 
            flags = BCM_COSQ_FC_OCB_DB;
            break;
        default: 
            return BCM_E_PARAM;
    }

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = flags | BCM_COSQ_FC_MASK_POOL_INDICATION;
    fc_handler.flags = 0;
    /* setting priority */    
    fc_trigger.priority = priority;
    /* setting cosq */ 
    fc_trigger.cosq = 0;
    fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("oob fc generation configuration failed(%d) in cint_jer_test_fc_oob_gl_gen_unset_example\n", rv);
        return rv;
    }
    /* Configuring oob fc } */

    return rv;
}

/* unset fc generation(global resource) and fc target(a tx slot on OOB)  for QAX/QUX */
int cint_qax_test_fc_oob_gl_gen_unset_example(int unit, int oob_port, int oob_calendar_idx, int global_vsq_src, int priority, int shaped_port)
{
    int rv = BCM_E_NONE;
    int flags = 0;

    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;

    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    
    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    /* Trigger global fc : Setting FC threshold and Shaping local port*/
    rv = cint_jer_test_fc_gl_trigger_unset_example(unit, global_vsq_src, priority, shaped_port);
    if (rv != BCM_E_NONE) {
        printf("Trigger global fc failed(%d) in cint_jer_test_fc_oob_gl_gen_unset_example\n", rv);
        return rv;
    }

    /* Configuring oob fc { */
    sal_memset(&fc_trigger, 0, sizeof(fc_trigger));
    sal_memset(&fc_handler, 0, sizeof(fc_handler));

    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            flags &= ~(BCM_COSQ_FC_IS_OCB_ONLY);
            flags |= BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_ocb_db:
        case global_vsq_src_ocb_pdb:
            flags |= BCM_COSQ_FC_IS_OCB_ONLY | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        case global_vsq_src_p0_db: 
        case global_vsq_src_p0_pd:
        case global_vsq_src_p0_byte:
            fc_trigger.cosq = 0;
            break;
        case global_vsq_src_p1_db: 
        case global_vsq_src_p1_pd:
        case global_vsq_src_p1_byte:
            fc_trigger.cosq = 1;
            break;
        case global_vsq_src_hdrm_db: 
        case global_vsq_src_hdrm_pd:
            flags |= BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_MASK_POOL_INDICATION;
            break;
        default: 
            return BCM_E_PARAM;
    }

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = flags;
    fc_handler.flags = 0;
    /* setting priority */    
    fc_trigger.priority = priority;
    /* setting cosq */ 
    fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("oob fc generation configuration failed(%d) in cint_qax_test_fc_oob_gl_gen_unset_example\n", rv);
        return rv;
    }
    /* Configuring oob fc } */

    return rv;
}

/* set fc generation(VSQ Group A~D) and fc target(a tx slot on OOB)  for Jericho, QAX and QUX */
/*
vsq_flag :  Designate the VSQ Group A~C
      BCM_COSQ_VSQ_CT
      BCM_COSQ_VSQ_CTTC
      BCM_COSQ_VSQ_CTCC
*/
int cint_jer_test_fc_oob_vsq_gen_set_example(int unit, int oob_port, int oob_calendar_idx, int vsq_flag, int priority, int shaped_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    /* Trigger global fc : Setting FC threshold and Shaping local port*/
    rv = cint_jer_test_fc_vsq_trigger_set_example(unit, vsq_flag, shaped_port, &vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("Trigger global fc failed(%d) in cint_jer_test_fc_oob_gl_gen_set_example\n", rv);
        return rv;
    }

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = 0;
    fc_handler.flags = 0;
    /* setting priority */    
    fc_trigger.priority = priority;
    /* setting cosq */ 
    fc_trigger.cosq = 0;
    fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("oob fc generation configuration failed(%d) in cint_jer_test_fc_oob_gl_gen_set_example\n", rv);
        return rv;
    }
    /* Configuring oob fc } */

    return rv;
}

/* unset fc generation(VSQ Group A~C) and fc target(a tx slot on OOB)  for Jericho, QAX and QUX */
/*
vsq_flag :  Designate the VSQ Group A~C
      BCM_COSQ_VSQ_CT
      BCM_COSQ_VSQ_CTTC
      BCM_COSQ_VSQ_CTCC
*/
int cint_jer_test_fc_oob_vsq_gen_unset_example(int unit, int oob_port, int oob_calendar_idx, int vsq_flag, int priority, int shaped_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    /* Trigger global fc : Setting FC threshold and Shaping local port*/
    rv = cint_jer_test_fc_vsq_trigger_unset_example(unit, vsq_flag, shaped_port, &vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("Trigger global fc failed(%d) in cint_jer_test_fc_oob_gl_gen_set_example\n", rv);
        return rv;
    }

    BCM_GPORT_CONGESTION_SET(fc_handler.port, oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = 0;
    fc_handler.flags = 0;
    /* setting priority */    
    fc_trigger.priority = priority;
    /* setting cosq */ 
    fc_trigger.cosq = 0;
    fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("oob fc generation configuration failed(%d) in cint_jer_test_fc_oob_gl_gen_set_example\n", rv);
        return rv;
    }
    /* Configuring oob fc } */

    return rv;
}

/*##############################################################*/
/*##################     OOB RECEPTION     #####################*/
/*##############################################################*/

/* set fc reception(an rx slot on OOB) and fc target(a port traffic) for Jericho/QAX/QUX */
int cint_jer_test_fc_oob_llfc_rec_set_example(int unit, int target_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception; 
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(child_port, target_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_target.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }

    BCM_GPORT_CONGESTION_SET(fc_reception.port, oob_port);
    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */    
    fc_target.cosq = -1;
    fc_reception.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_jer_test_fc_oob_llfc_rec_set_example\n", rv);
    }
    /* Config oob fc } */

    return rv;
}

int cint_jer_test_fc_oob_llfc_rec_unset_example(int unit, int target_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception; 
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(child_port, target_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_target.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }

    BCM_GPORT_CONGESTION_SET(fc_reception.port, oob_port);
    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */    
    fc_target.cosq = -1;
    fc_reception.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_jer_test_fc_oob_llfc_rec_unset_example\n", rv);
    }
    /* Config oob fc } */

    return rv;
}


/* set fc reception(an rx slot on OOB) and fc target(a port traffic) for Jericho/QAX/QUX */
int cint_jer_test_fc_oob_port_rec_set_example(int unit, int target_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_CONGESTION_SET(fc_reception.port, oob_port);
    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */    
    fc_target.cosq = -1;
    fc_reception.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_jer_test_fc_oob_port_rec_set_example\n", rv);
    }
    /* Config oob fc } */

    return rv;
}

int cint_jer_test_fc_oob_port_rec_unset_example(int unit, int target_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_CONGESTION_SET(fc_reception.port, oob_port);
    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */    
    fc_target.cosq = -1;
    fc_reception.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_jer_test_fc_oob_port_rec_unset_example\n", rv);
    }
    /* Config oob fc } */

    return rv;
}

/* set fc reception(an rx slot on OOB) and fc target(a port priority traffic)  for Jericho/QAX/QUX */
int cint_jer_test_fc_oob_pfc_rec_set_example(int unit, int target_port, int oob_port, int oob_calendar_idx, int target_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_CONGESTION_SET(fc_reception.port, oob_port);
    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */    
    fc_target.cosq = target_cosq;
    fc_reception.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_jer_test_fc_oob_pfc_rec_set_example\n", rv);
    }
    /* Config oob fc } */

    return rv;
}

int cint_jer_test_fc_oob_pfc_rec_unset_example(int unit, int target_port, int oob_port, int oob_calendar_idx, int target_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_CONGESTION_SET(fc_reception.port, oob_port);
    /* setting flags */
    fc_target.flags = fc_reception.flags = 0;
    /* setting cosq */    
    fc_target.cosq = target_cosq;
    fc_reception.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_jer_test_fc_oob_pfc_rec_unset_example\n", rv);
    }
    /* Config oob fc } */

    return rv;
}

/* set fc reception(an rx slot on OOB) and fc target(traffic specified by PFC bitmap) for Jericho/QAX/QUX */
int cint_jer_test_fc_oob_pfc_bmp_rec_set_example(int unit, int target_port, int oob_port, int oob_calendar_idx, int target_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_CONGESTION_SET(fc_reception.port, oob_port);
    /* setting flags */
    fc_target.flags = BCM_COSQ_FC_PORT_OVER_PFC;
    fc_reception.flags = 0;
    /* setting cosq */    
    fc_target.cosq = target_cosq;
    fc_reception.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_jer_test_fc_oob_pfc_bmp_rec_set_example\n", rv);
    }
    /* Config oob fc } */

    return rv;
}

int cint_jer_test_fc_oob_pfc_bmp_rec_unset_example(int unit, int target_port, int oob_port, int oob_calendar_idx, int target_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_CONGESTION_SET(fc_reception.port, oob_port);
    /* setting flags */
    fc_target.flags = BCM_COSQ_FC_PORT_OVER_PFC;
    fc_reception.flags = 0;
    /* setting cosq */    
    fc_target.cosq = target_cosq;
    fc_reception.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("oob fc reception configuration failed(%d) in cint_jer_test_fc_oob_pfc_bmp_rec_unset_example\n", rv);
    }
    /* Config oob fc } */

    return rv;
}


/*
   This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
   
   Copyright 2007-2020 Broadcom Inc. All rights reserved. */
/*  */

/*##############################################################*/
/*###################    OOB GENERATION     ####################*/
/*##############################################################*/

bcm_gport_t cttc_vsq_gport;

/*Sets a cttc_vsq as a fc trigger and a corresponding oob calendar slot as a fc handler 
  | cttc |                ___________________________________________
  | vsq  | ------------> | calendar|                                 |
  |      |               |_slot_n__|_________________________________|  
  |      | 
  ||||||||  
  the shaped port parameter is for test puposes only*/
int cint_arad_test_fc_oob_vsq_cttc_gen_set_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port, int base_voq, int cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t voq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Creating a vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_CTTC /*or BCM_COSQ_VSQ_CTCC...*/;
    vsq_inf.ct_id = 2; /*All queues*/
    vsq_inf.traffic_class = cosq;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }
    cttc_vsq_gport = vsq_gport;

    /*Associating the vsq with a voq(s)*/
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport,base_voq);
    rv = bcm_cosq_gport_vsq_add(unit, vsq_gport, voq_gport, cosq);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }

    /*Setting vsq threshold*/
    threshold.xon_threshold = 0xFF;
    threshold.xoff_threshold = 0xFF;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting cosq */
    fc_trigger.cosq = fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }
    /* Config oob fc } */

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,1000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_test_fc_oob_vsq_cttc_gen_set_example. port %d\n",local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}

int cint_arad_test_fc_oob_vsq_cttc_gen_unset_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port, int base_voq, int cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t voq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Getting vsq*/
    vsq_gport = cttc_vsq_gport;

    /*Setting vsq threshold*/
    threshold.xon_threshold = 0xFFFFFFF;
    threshold.xoff_threshold = 0xFFFFFFF;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_cttc_gen_unset_example\n");
        return rv;
    }

    /* Config oob fc { */
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting cosq */
    fc_trigger.cosq = fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_unset_example\n");
        return rv;

    }
    /* Config oob fc } */

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,10000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_test_fc_oob_vsq_cttc_gen_unset_example. port %d\n",local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}



int cint_arad_test_fc_oob_nif_mlf_gen_set_example(int unit,int local_traffic_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;
    bcm_gport_t child_port;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(child_port, local_traffic_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_trigger.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    /* setting flags */    
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting cosq */
    fc_trigger.cosq = fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_llfc_gen_set_example\n");
        return rv;
    }
    /* Config oob fc } */

    return rv;
}



/*Sets a gl_vsq as a fc trigger and a corresponding oob calendar slot as a fc handler 
  | gl   |                ___________________________________________
  | vsq  | ------------> | calendar|                                 |
  |      |               |_slot_n__|_________________________________|  
  |      | 
  ||||||||  
  the shaped port parameter is for test puposes only
 
 *                                                                                         *  
 *requires a calendar configuration of a minimum length of three slots (via soc properties)*
 *Make sure that the oob_calendar_index parameter < cal_len-2                              */
int cint_arad_test_fc_oob_gl_gen_set_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n", rv);
        return rv;
    }

    /*Setting vsq threshold*/
    threshold.xon_threshold = 0x7FFFFF;
    threshold.xoff_threshold = 0x7FFFFF;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,BCM_COSQ_HIGH_PRIORITY,BCM_COSQ_THRESHOLD_UNICAST,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed(%d) in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n", rv);
        return rv;
    }

    /* Configuring oob fc { */
    /* setting port */
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting cosq */ 
    fc_trigger.cosq = fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed(%d) in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n", rv);
        return rv;
    }
    /* Configuring oob fc } */

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,1000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to shape in cint_arad_fc_oob_llfc_gen_config_test_example. port %d\n", rv, local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}    

int cint_arad_test_fc_oob_gl_gen_unset_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed in cint_arad_test_fc_oob_vsq_gl_gen_unset_example\n");
        return rv;
    }

    /*Setting vsq threshold*/
    threshold.xon_threshold = 0x0;
    threshold.xoff_threshold = 0x0;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,BCM_COSQ_HIGH_PRIORITY,BCM_COSQ_THRESHOLD_UNICAST,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_gl_gen_unset_example\n");
        return rv;
    }

    /* Deleting oob configuration { */
    /* setting port */
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting cosq */
    fc_trigger.cosq = fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_gl_gen_unset_example\n");
        return rv;
    }
    /* Deleting oob configuration } */

    /*UNShaping local port {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,10000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_test_fc_oob_vsq_gl_gen_unset_example. port %d\n",local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}    


/*Sets a ct_vsq as a fc trigger and a corresponding oob calendar slot as a fc handler 
  | ct   |                ___________________________________________
  | vsq  | ------------> | calendar|                                 |
  |      |               |_slot_n__|_________________________________|  
  |      | 
  ||||||||  
  the shaped port parameter is for test puposes only*/
int cint_arad_test_fc_oob_vsq_ct_gen_set_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_CT;
    vsq_inf.ct_id = 2; /*All queues*/
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);

    /*Setting vsq threshold*/
    threshold.xon_threshold = 0xFF;
    threshold.xoff_threshold = 0xFF;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_ct_gen_set_example\n");
        return rv;
    }

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting cosq */
    fc_trigger.cosq = fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration cint_arad_test_fc_oob_vsq_llfc_gen_set_example\n");
        return rv;
    }
    /* Config oob fc } */

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,1000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_fc_oob_llfc_gen_config_test_example. port %d\n",local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}

int cint_arad_test_fc_oob_vsq_ct_gen_unset_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_CT;
    vsq_inf.ct_id = 2; /*All queues*/
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);

    /*UNSetting vsq threshold*/
    threshold.xon_threshold = 0xFFFFFFF;
    threshold.xoff_threshold = 0xFFFFFFF;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_ct_gen_unset_example\n");
        return rv;
    }

    /* Deleting oob configuration { */
    /* setting port */
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting cosq */
    fc_trigger.cosq = fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration cint_arad_test_fc_oob_vsq_llfc_gen_unset_example\n");
        return rv;
    }
    /* Deleting oob configuration } */

    /*UNshaping local port {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,10000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_test_fc_oob_vsq_llfc_gen_unset_example. port %d\n",local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}




bcm_gport_t llfc_vsq_gport;
bcm_gport_t pfc_vsq_gport;

/*Sets a llfc_vsq as a fc trigger and a corresponding oob calendar slot as a fc handler 
  | llfc |                ___________________________________________
  | vsq  | ------------> | calendar|                                 |
  |      |               |_slot_n__|_________________________________|  
  |      | 
  ||||||||  
  the shaped port parameter is for test puposes only*/
int cint_arad_test_fc_oob_vsq_llfc_gen_set_example(int unit, int oob_port, int oob_calendar_idx,int local_traffic_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;


    /*Creating vsq*/
    vsq_inf.flags=BCM_COSQ_VSQ_LLFC;
    BCM_GPORT_LOCAL_SET(vsq_inf.src_port,local_traffic_port);
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    llfc_vsq_gport = vsq_gport;

    /*Setting vsq threshold*/
    threshold.xoff_threshold = 0;
    threshold.xon_threshold = 0;
    threshold.xoff_threshold_bd = 0;
    threshold.xon_threshold_bd = 0; 
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_llfc_gen_set_example\n");
        return rv;
    }

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting cosq */
    fc_trigger.cosq = fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration cint_arad_test_fc_oob_vsq_llfc_gen_set_example\n");
        return rv;
    }
    /* Config oob fc } */

    return rv;
}


int cint_arad_test_fc_oob_vsq_llfc_gen_unset_example(int unit, int oob_port, int oob_calendar_idx,int local_traffic_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;


    /*Getting vsq*/
    vsq_gport = llfc_vsq_gport;

    /*Setting vsq threshold*/
    threshold.xoff_threshold = 0xFFFFF;
    threshold.xon_threshold = 0xFFFFF;
    threshold.xoff_threshold_bd = 0xFFFFF;
    threshold.xon_threshold_bd = 0xFFFFF; 
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_llfc_gen_unset_example\n");
        return rv;
    }

    /* Config oob fc { */
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting cosq */
    fc_trigger.cosq = fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration cint_arad_test_fc_oob_vsq_llfc_gen_unset_example\n");
        return rv;
    }
    /* Config oob fc } */

    return rv;
}


/*Sets a cttc_vsq as a fc trigger and a corresponding oob calendar slot as a fc handler 
  | pfc  |                ___________________________________________
  | vsq  | ------------> | calendar|                                 |
  |      |               |_slot_n__|_________________________________|  
  |      | 
  ||||||||  
  the shaped port parameter is for test puposes only*/
int cint_arad_test_fc_oob_vsq_pfc_gen_set_example(int unit, int oob_port, int oob_calendar_idx, int cos,int local_traffic_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;


    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_PFC;
    vsq_inf.traffic_class = cos;
    BCM_GPORT_LOCAL_SET(vsq_inf.src_port,local_traffic_port);
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    pfc_vsq_gport = vsq_gport;

    /*Setting vsq threshold*/
    threshold.xoff_threshold = 0;
    threshold.xon_threshold = 0;
    threshold.xoff_threshold_bd = 0;
    threshold.xon_threshold_bd = 0;   
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_pfc_gen_set_example\n");
        return rv;
    }

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting cosq */
    fc_trigger.cosq = fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration cint_arad_test_fc_oob_vsq_pfc_gen_set_example\n");
        return rv;
    }
    /* Config oob fc } */

    return rv;
}

int cint_arad_test_fc_oob_vsq_pfc_gen_unset_example(int unit, int oob_port, int oob_calendar_idx, int cos,int local_traffic_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;


    /*Getting vsq*/
    vsq_gport = pfc_vsq_gport;

    /*Setting vsq threshold*/
    threshold.xoff_threshold = 0xFFFFF;
    threshold.xon_threshold = 0xFFFFF;
    threshold.xoff_threshold_bd = 0xFFFFF;
    threshold.xon_threshold_bd = 0xFFFFF;   
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_pfc_gen_unset_example\n");
        return rv;
    }

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting cosq */
    fc_trigger.cosq = fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration cint_arad_test_fc_oob_vsq_pfc_gen_unset_example\n");
        return rv;
    }
    /* Config oob fc } */

    return rv;
}

int cint_arad_test_hcfc_fc_oob_vsq_cttc_gen_set_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port, int base_voq, int cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t voq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Creating a vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_CTTC /*or BCM_COSQ_VSQ_CTCC...*/;
    vsq_inf.ct_id = 2; /*All queues*/
    vsq_inf.traffic_class = cosq;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }
    cttc_vsq_gport = vsq_gport;

    /*Associating the vsq with a voq(s)*/
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport,base_voq);
    rv = bcm_cosq_gport_vsq_add(unit, vsq_gport, voq_gport, cosq);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }

    /*Setting vsq threshold*/
    threshold.xon_threshold = 0xFF;
    threshold.xoff_threshold = 0xFF;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */
    fc_handler.flags = 0;
    fc_trigger.flags = BCM_COSQ_FC_HCFC_BITMAP;
    /* setting cosq */
    fc_handler.cosq = fc_trigger.cosq = -1;
    /* setting calender index */
    fc_trigger.calender_index = 0;
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }
    /* Config oob fc } */

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,1000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_test_fc_oob_vsq_cttc_gen_set_example. port %d\n",local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}


/*##############################################################*/
/*##################     OOB RECEPTION     #####################*/
/*##############################################################*/

/*Sets an oob rx slot as a fc reception object and a port as the fc target*/
int cint_arad_test_fc_oob_llfc_rec_set_example(int unit,int target_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(child_port, target_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_target.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }

    BCM_GPORT_CONGESTION_SET(fc_reception_port.port,oob_port);
    /* setting flags */    
    fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */
    fc_target.cosq = fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index=oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("OOB fc reception configuration failed(%d) in cint_arad_test_fc_oob_llfc_rec_set_example\n", rv);
    }
    return rv;
    /* Config oob fc } */
}


int cint_arad_test_fc_oob_llfc_rec_unset_example(int unit,int target_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;
    bcm_gport_t child_port;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(child_port,target_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_target.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }
    BCM_GPORT_CONGESTION_SET(fc_reception_port.port,oob_port);
    /* setting flags */    
    fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */
    fc_target.cosq = fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("OOB fc reception configuration failed(%d) in cint_arad_test_fc_oob_llfc_rec_unset_example\n", rv);
    }
    return rv;
    /* Config oob fc } */
}

/*Sets an oob rx slot as a fc reception object and a port priority traffic as the fc target*/
int cint_arad_test_fc_oob_pfc_rec_set_example(int unit,int target_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_CONGESTION_SET(fc_reception_port.port,oob_port);
    /* setting flags */    
    fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */    
    fc_target.cosq = 0;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("OOB fc reception configuration failed(%d) in cint_arad_test_fc_oob_pfc_rec_set_example\n", rv);
    }
    /* Config oob fc } */

    return rv;
}

int cint_arad_test_fc_oob_pfc_rec_unset_example(int unit,int target_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_CONGESTION_SET(fc_reception_port.port,oob_port);
    /* setting flags */    
    fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */    
    fc_target.cosq=0;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index=oob_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("OOB fc reception configuration failed in cint_arad_test_fc_oob_pfc_rec_unset_example\n");
    }
    /* Config oob fc } */
    
    return rv;
}

/*Traffic Setting func. for tests purposes*/
int cint_arad_test_fc_device_traffic_set(int ix_unit, int local_ix_port, int local_d2d_unit1_port_1, int local_d2d_unit1_port_2, int unit_2, int local_d2d_unit2_port_1 , int local_d2d_unit2_port_2)
{
    int rv = BCM_E_NONE;

    rv = bcm_port_force_forward_set(ix_unit,local_ix_port,local_d2d_unit1_port_1,1);
    if (rv != BCM_E_NONE) {
        printf("cint_arad_test_fc_rec_device_traffic_set failed to set traffic from ixia port to d2d port\n");
        return rv;
    }
    rv = bcm_port_force_forward_set(ix_unit,local_d2d_unit1_port_2,local_ix_port,1);
    if (rv != BCM_E_NONE) {
        printf("cint_arad_test_fc_rec_device_traffic_set failed to set traffic from d2d port to ixia port\n");
        return rv;
    }
    rv = bcm_port_force_forward_set(unit_2,local_d2d_unit2_port_1,local_d2d_unit2_port_2,1);
    if (rv != BCM_E_NONE) {
        printf("cint_arad_test_fc_rec_device_traffic_set failed to set traffic from d2d port to itself\n");
        return rv;
    }
    return rv;
}


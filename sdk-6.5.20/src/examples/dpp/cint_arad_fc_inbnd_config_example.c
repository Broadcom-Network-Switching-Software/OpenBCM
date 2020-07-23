
/*
   This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
   
   Copyright 2007-2020 Broadcom Inc. All rights reserved. */
/*  */

/*##############################################################*/
/*##################   INBND FC GENERATION  ####################*/
/*##############################################################*/

int cint_arad_test_fc_inbnd_gl_gen_set_example(int unit, int target_port, int shaped_port, bcm_port_control_t fc_control /*LLFC or PFC*/)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    if ((fc_control != bcmPortControlLLFCTransmit) && (fc_control != bcmPortControlPFCTransmit)) {
        printf("fc_control to cint_arad_test_fc_inbnd_gl_gen_set_example can be either bcmPortControlPFCTransmit or bcmPortControlLLFCTransmit\n");
        return rv;
    }
    /*Setting target port fc inbnd mode*/
    rv = bcm_port_control_set(unit, target_port, fc_control, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port fc inbnd mode in cint_arad_test_fc_inbnd_gl_gen_set_example\n");
        return rv;
    }
    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n");
        return rv;
    }

    /*Setting vsq threshold*/
    threshold.xon_threshold = 0x7FFFFF;
    threshold.xoff_threshold = 0x7FFFFF;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,BCM_COSQ_HIGH_PRIORITY,BCM_COSQ_THRESHOLD_UNICAST,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_inbnd_gl_gen_set_example\n");
        return rv;
    }

    /*Configuring inbnd fc*/
    BCM_GPORT_LOCAL_SET(fc_handler.port,target_port);
    fc_trigger.port = vsq_gport;
    fc_handler.flags = BCM_COSQ_FC_ENDPOINT_PORT_ALL;
    fc_trigger.flags = 0;
    fc_trigger.cosq = fc_handler.cosq = 0;
    fc_trigger.calender_index = fc_handler.calender_index = 0;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("Inbnd fc generation configuration failed in cint_arad_test_fc_inbnd_gl_gen_set_example\n");
        return rv;
    }

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,1000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_test_fc_inbnd_gl_gen_set_example. port %d\n",shaped_port);
        return rv;
    }
    /*}*/
    return rv;
}    


int cint_arad_test_fc_inbnd_mlf_gen_set_example(int unit, int target_port, bcm_port_control_t fc_control/*LLFC or PFC*/, int target_cosq /*for PFC*/)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;
    bcm_gport_t child_port;

    bcm_cosq_threshold_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;
    uint32 flags;

    if ((fc_control != bcmPortControlLLFCTransmit) && (fc_control != bcmPortControlPFCTransmit)) {
        printf("fc_control to cint_arad_test_fc_inbnd_gl_gen_set_example can be either bcmPortControlPFCTransmit or bcmPortControlLLFCTransmit\n");
        return BCM_E_PARAM;
    }

    if (fc_control == bcmPortControlLLFCTransmit) {
        if (target_cosq != -1) {
            printf("target_cosq must be -1 in case of LLFC\n");
            return BCM_E_PARAM;        
        }
    }
    else {
        if ((target_cosq < 0 ) || (target_cosq >= 8)) {
            printf("target_cosq (%d) out of range\n", target_cosq);
			return BCM_E_PARAM;        
        }
    }

    /* set target port fc inbnd mode*/
    rv = bcm_port_control_set(unit, target_port, fc_control, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to set port fc inbnd mode in bcm_port_control_set\n", rv);
        return rv;
    }

    /* set fc threshold*/
    BCM_GPORT_LOCAL_SET(local_gport, target_port);
    threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_LLFC;
    if (fc_control == bcmPortControlLLFCTransmit) {
        threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_LLFC|BCM_COSQ_THRESHOLD_SET;
        threshold.value = 5;
        rv = bcm_cosq_gport_threshold_set(unit, local_gport, 0, &threshold);
        if (rv != BCM_E_NONE) {
            printf("setting fc threshold failed(%d) in bcm_petra_cosq_gport_threshold_set\n", rv);
            return rv;
        }

        threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_LLFC|BCM_COSQ_THRESHOLD_CLEAR;
        threshold.value = 1;
        rv = bcm_cosq_gport_threshold_set(unit, local_gport, 0, &threshold);
        if (rv != BCM_E_NONE) {
            printf("setting fc threshold failed(%d) in bcm_petra_cosq_gport_threshold_set\n", rv);
            return rv;
        }
    }
    else if (fc_control == bcmPortControlPFCTransmit) {
        threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_PFC|BCM_COSQ_THRESHOLD_SET;
        threshold.value = 5;
        rv = bcm_cosq_gport_threshold_set(unit, local_gport, 0, &threshold);
        if (rv != BCM_E_NONE) {
            printf("setting fc threshold failed(%d) in bcm_petra_cosq_gport_threshold_set\n", rv);
            return rv;
        }

        threshold.flags = BCM_COSQ_THRESHOLD_ETH_PORT_PFC|BCM_COSQ_THRESHOLD_CLEAR;
        threshold.value = 1;
        rv = bcm_cosq_gport_threshold_set(unit, local_gport, 0, &threshold);
        if (rv != BCM_E_NONE) {
            printf("setting fc threshold failed(%d) in bcm_petra_cosq_gport_threshold_set\n", rv);
            return rv;
        }
    }

    /* configure inbnd fc path */
    BCM_GPORT_LOCAL_SET(child_port, target_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_trigger.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }
    fc_handler.port = 0;
    fc_trigger.flags = BCM_COSQ_FC_ETH_PORT;
    fc_handler.flags = BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;
    fc_trigger.cosq = -1;            /* cosq can be 0 or -1, but -1 is recommended */
    fc_handler.cosq = target_cosq;   /* -1 indicates LLFC */
    fc_trigger.calender_index = fc_handler.calender_index = 0;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("Inbnd fc generation configuration failed in bcm_cosq_fc_path_add\n");
        return rv;
    }
    
    return rv;
}

int cint_arad_test_fc_inbnd_mlf_gen_unset_example(int unit, int target_port, bcm_port_control_t fc_control/*LLFC or PFC*/, int target_cosq /*for PFC*/)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;
    bcm_gport_t child_port;

    bcm_cosq_threshold_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    if ((fc_control != bcmPortControlLLFCTransmit) && (fc_control != bcmPortControlPFCTransmit)) {
        printf("fc_control to cint_arad_test_fc_inbnd_gl_gen_set_example can be either bcmPortControlPFCTransmit or bcmPortControlLLFCTransmit\n");
        return BCM_E_PARAM;
    }

    if (fc_control == bcmPortControlLLFCTransmit) {
        if (target_cosq != -1) {
            printf("target_cosq must be -1 in case of LLFC\n");
            return BCM_E_PARAM;        
        }
    }
    else {
        if ((target_cosq < 0 ) || (target_cosq >= 8)) {
            printf("target_cosq (%d) out of range\n", target_cosq);
			return BCM_E_PARAM;
        }
    }

    rv = bcm_port_control_set(unit, target_port, fc_control, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to set port fc inbnd mode in bcm_port_control_set\n", rv);
        return rv;
    }

    /* no need to reset fc threshold */

    /* delete inbnd fc path */
    BCM_GPORT_LOCAL_SET(child_port, target_port);
    rv = bcm_fabric_port_get(unit, child_port, 0, &fc_trigger.port);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_fabric_port_get\n", rv);
    }
    fc_handler.port = 0;
    fc_trigger.flags = BCM_COSQ_FC_ETH_PORT;
    fc_handler.flags = BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;
    fc_trigger.cosq = -1;            /* cosq can be 0 or -1, but -1 is recommended */
    fc_handler.cosq = target_cosq;   /* -1 indicates LLFC */
    fc_trigger.calender_index = fc_handler.calender_index = 0;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("Inbnd fc generation configuration failed in bcm_cosq_fc_path_add\n");
        return rv;
    }

    return rv;
}

/*##############################################################*/
/*##################     ILKN INBAND RECEPTION     #####################*/
/*##############################################################*/

/*Sets an oob rx slot as a fc reception object and a port as the fc target*/
int cint_arad_test_fc_ilkn_inb_llfc_rec_set_example(int unit, int ilkn_port, int inb_calendar_idx, int target_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port); 
    BCM_GPORT_LOCAL_SET(fc_reception_port.port, ilkn_port);
    /* setting flags */    
    fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */
    fc_target.cosq = fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index=inb_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("OOB fc reception configuration failed(%d) in cint_arad_test_fc_oob_llfc_rec_set_example\n", rv);
    }
    return rv;
    /* Config oob fc } */
}

int cint_arad_test_fc_ilkn_inb_llfc_rec_unset_example(int unit, int ilkn_port, int inb_calendar_idx, int target_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config oob fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port); 
    BCM_GPORT_LOCAL_SET(fc_reception_port.port, ilkn_port);
    /* setting flags */    
    fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */
    fc_target.cosq = fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index=inb_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("OOB fc reception configuration failed(%d) in cint_arad_test_fc_oob_llfc_rec_set_example\n", rv);
    }
    return rv;
    /* Config oob fc } */
}

/*##############################################################*/
/*##################     ILKN INBAND GENERATION     #####################*/
/*##############################################################*/
int cint_arad_test_fc_ilkn_inb_gl_gen_set_example(int unit, int ilkn_port, int inb_calendar_idx, int shaped_port)
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

    /* Configuring fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_handler.port, ilkn_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting cosq */ 
    fc_trigger.cosq = fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = inb_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed(%d) in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n", rv);
        return rv;
    }
    /* Configuring fc } */

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

int cint_arad_test_fc_ilkn_inb_gl_gen_unset_example(int unit, int ilkn_port, int inb_calendar_idx, int shaped_port)
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
    threshold.xon_threshold = 0;
    threshold.xoff_threshold = 0;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,BCM_COSQ_HIGH_PRIORITY,BCM_COSQ_THRESHOLD_UNICAST,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed(%d) in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n", rv);
        return rv;
    }

    /* Configuring fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_handler.port, ilkn_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = fc_handler.flags = 0;
    /* setting cosq */ 
    fc_trigger.cosq = fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = inb_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed(%d) in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n", rv);
        return rv;
    }
    /* Configuring fc } */

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,10000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to shape in cint_arad_fc_oob_llfc_gen_config_test_example. port %d\n", rv, local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}    


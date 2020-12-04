
/* $Id: 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: Egress Transmit~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  
 * File:        cint_egress_threshold.c
 * Purpose:     Example of configuration for egress thresholds 
 * 
 * It is assumed diag_init is executed.
 *
 *  CINT usage:
 *  o    Run application: 
 *          egress_threshold_jericho_example(unit,local_port_id)
 *  o    The application sets different egress thresholds
 *  o    Run traffic to the above port. You can see the drop rate change according to the threshold type/value.
 */

static int uc = 0;
static int mc = 1;
static int mc_sp0 = 2;
static int mc_sp1 = 3;

/*  Set Queue Threshold
*   uc_mc = 0 for UC
*   1 for MC
*   threshold_type according to bcm_cosq_threshold_type_t types
*/ 
 int set_queue_threshold(int unit, int local_port_id,int uc_mc, int queue,  int color, 
                         bcm_cosq_threshold_type_t threshold_type, unsigned int action_flags, int value) {
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;
    bcm_cosq_threshold_t threshold;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport,local_port_id); 

    /* Set gport_type and threshold flag according to uc_mc argument */
    if(uc_mc == 0) {
      gport_type = bcmCosqGportTypeUnicastEgress; 
      threshold.flags = BCM_COSQ_THRESHOLD_UNICAST;
    } else {
      gport_type = bcmCosqGportTypeMulticastEgress; 
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST;

      threshold.flags |= BCM_COSQ_THRESHOLD_PER_DP;
    }
    

    threshold.dp = color;

    rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
    if (rv != BCM_E_NONE) {
      printf("Error, in handle get, gport_type $gport_type \n");
      return rv;
    }

    out_gport = gport_info.out_gport;

    /* Fill the Threshold Struct
     */
    threshold.type = threshold_type;
    threshold.value = value;
    threshold.flags = threshold.flags |
      BCM_COSQ_THRESHOLD_EGRESS | 
      action_flags;

    rv = bcm_cosq_gport_threshold_set(unit, out_gport, queue, threshold);

    if (rv != BCM_E_NONE) {
      printf("Error, in queue threshold set, out_gport $out_gport threshold_type $threshold_type value $value \n");
      return rv;
    }
    
    return rv;
}

/*  Set Queue Drop Threshold
*   queue = BCM_COSQ_LOW_PRIORITY
*   BCM_COSQ_HIGH_PRIORITY
*   uc_mc = 0 for UC
*   1 for MC
*   threshold_type according to bcm_cosq_threshold_type_t types
*/ 
 int set_queue_drop_threshold(int unit, int local_port_id,int uc_mc, int queue,  int color, bcm_cosq_threshold_type_t threshold_type, int value) {
   return set_queue_threshold(unit, local_port_id, uc_mc, queue, color, threshold_type, BCM_COSQ_THRESHOLD_DROP, value); 
 }



/*  Set Port Threshold
*   uc_mc = 0 for UC
*   1 for MC
*   threshold_type according to bcm_cosq_threshold_type_t types
*/ 
int set_port_threshold(int unit, int local_port_id, int uc_mc, 
                            bcm_cosq_threshold_type_t threshold_type, unsigned int action_flags, int value) {
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;
    bcm_cosq_threshold_t threshold;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport,local_port_id); 

    if(uc_mc == 0) {
      gport_type = bcmCosqGportTypeUnicastEgress; 
      threshold.flags = BCM_COSQ_THRESHOLD_UNICAST;
    } else {
      gport_type = bcmCosqGportTypeMulticastEgress; 
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST;
    }

    rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
    if (rv != BCM_E_NONE) {
      printf("Error, in handle get, gport_type $gport_type \n");
      return rv;
    }

    out_gport = gport_info.out_gport;

    /* Fill the Threshold Struct */
    threshold.type = threshold_type;
    threshold.value = value;
    if(uc_mc == 0) {
      threshold.flags = BCM_COSQ_THRESHOLD_UNICAST;
    } else {
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST;
    }
    threshold.flags = threshold.flags |
      BCM_COSQ_THRESHOLD_EGRESS | 
      action_flags;

    rv = bcm_cosq_gport_threshold_set(unit, out_gport, -1, threshold);

    if (rv != BCM_E_NONE) {
      printf("Error, in port threshold set, out_gport $out_gport threshold_type $threshold_type value $value \n");
      return rv;
    }
    
    return rv;
}
/*  Set Port Drop Threshold
*   uc_mc = 0 for UC
*   1 for MC
*   threshold_type according to bcm_cosq_threshold_type_t types
*/ 
int set_port_drop_threshold(int unit, int local_port_id, int uc_mc, bcm_cosq_threshold_type_t threshold_type, int value){
  return set_port_threshold( unit, local_port_id, uc_mc, threshold_type, BCM_COSQ_THRESHOLD_DROP, value);
}


/* used for Arad */
int set_device_drop_threshold(int unit, int uc_mc, int cosq, bcm_cosq_threshold_type_t threshold_type, int value)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t out_gport;
    bcm_cosq_threshold_t threshold;

    /* Set GPORT to be global */
    out_gport = 0; 

    if(uc_mc == 0) { /* UC */
      threshold.flags = BCM_COSQ_THRESHOLD_UNICAST;
    } else if (uc_mc == 1) { /* MC */
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST;
    } else if (uc_mc == 2) { /* SP 0 */
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST_SP0;
    } else if (uc_mc == 3) { /* SP 1 */
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST_SP1;
    }
    
    /* Fill the Threshold Struct */
    threshold.type = threshold_type;
    threshold.value = value;
    
    threshold.flags = threshold.flags |
      BCM_COSQ_THRESHOLD_EGRESS | 
      BCM_COSQ_THRESHOLD_DROP;

    rv = bcm_cosq_gport_threshold_set(unit, out_gport, cosq, threshold);

    if (rv != BCM_E_NONE) {
      printf("Error, in port threshold set, out_gport $out_gport threshold_type $threshold_type value $value \n");
      return rv;
    }
    
    return rv;
}


/* used for Jericho */
int set_core_threshold(int unit, int core, int uc_mc, int cosq, 
                       bcm_cosq_threshold_type_t threshold_type, unsigned int action_flags, int value)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_info_core_t gport_info;
    bcm_gport_t out_gport;
    bcm_cosq_threshold_t threshold;

    /* Set GPORT to be core */
    gport_info.gport_type = bcmCosqGportTypeCore;
    bcm_cosq_gport_handle_core_get(unit, core, gport_info, &out_gport);

    if(uc_mc == 0) { /* UC */
      threshold.flags = BCM_COSQ_THRESHOLD_UNICAST;
    } else if (uc_mc == 1) { /* MC */
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST;
    } else if (uc_mc == 2) { /* SP 0 */
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST_SP0;
    } else if (uc_mc == 3) { /* SP 1 */
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST_SP1;
    }
    
    /* Fill the Threshold Struct */
    threshold.type = threshold_type;
    threshold.value = value;
    
    threshold.flags = threshold.flags |
      BCM_COSQ_THRESHOLD_EGRESS | 
      action_flags;

    rv = bcm_cosq_gport_threshold_set(unit, out_gport, cosq, threshold);

    if (rv != BCM_E_NONE) {
      printf("Error, in port threshold set, out_gport $out_gport threshold_type $threshold_type value $value \n");
      return rv;
    }
    
    return rv;
}

/*  Set Interface Threshold
*   uc_mc = 0 for UC
*   1 for MC
*   threshold_type according to bcm_cosq_threshold_type_t types
*/ 
int set_interface_threshold(int unit, int local_port_id, int uc_mc, 
                            bcm_cosq_threshold_type_t threshold_type, unsigned int action_flags, int value) {
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;
    bcm_cosq_threshold_t threshold;

    /* Set GPORT according to the given local_port_id */
    gport_info.in_gport = local_port_id;
    rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPort, &gport_info) ;
    if (rv != BCM_E_NONE) {
      printf("Error, in handle get, gport_type $gport_type \n");
      return rv;
    }
    out_gport = gport_info.out_gport;


    /* Fill the Threshold Struct */
    threshold.type = threshold_type;
    threshold.value = value;
    if(uc_mc == 0) {
      threshold.flags = BCM_COSQ_THRESHOLD_UNICAST;
    } else {
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST;
    }
    threshold.flags = threshold.flags |
      BCM_COSQ_THRESHOLD_EGRESS | 
      action_flags;

    rv = bcm_cosq_gport_threshold_set(unit, out_gport, -1, threshold);

    if (rv != BCM_E_NONE) {
      printf("Error, in port threshold set, out_gport $out_gport threshold_type $threshold_type value $value \n");
      return rv;
    }
    
    return rv;
}




/*
 * Main function 
 * ARAD functionality example 
 */
int egress_threshold_arad_example(int unit, int bcm_local_port_id) {
    bcm_error_t rv = BCM_E_NONE;
    int sp, cosq, val;    

    /* Queue settings example */
    /* QDCT table QueueUcPdDisTh */
    rv = set_queue_drop_threshold(unit , bcm_local_port_id, 0, 1, 0, bcmCosqThresholdPacketDescriptors, 1000); 
    if (rv != BCM_E_NONE) return rv;

    /* QQST table QueueUcDbDisTh */
    rv = set_queue_drop_threshold(unit , bcm_local_port_id, 0, 1, 0, bcmCosqThresholdDataBuffers, 1000); 
    if (rv != BCM_E_NONE) return rv;

    /* QDCT table QUEUE_MC_PD_TH_DP_0 */
    rv = set_queue_drop_threshold(unit , bcm_local_port_id, 1, 1, 1 /* DP */, bcmCosqThresholdPacketDescriptors, 1000); 
    if (rv != BCM_E_NONE) return rv;

    /* Port settings example */
    /* PDCT table PortUcPdDisTh */
    rv = set_port_drop_threshold(unit , bcm_local_port_id, 0, bcmCosqThresholdPacketDescriptors, 1000); 
    if (rv != BCM_E_NONE) return rv;

    /* PDCT table PortMcPdSheardTh */
    rv = set_port_drop_threshold(unit , bcm_local_port_id, 1, bcmCosqThresholdPacketDescriptors, 1000); 
    if (rv != BCM_E_NONE) return rv;

    /* General settings */
    /* CgmGeneralPdTh register Uc_Pd_Th */
    rv = set_device_drop_threshold(unit, 0, -1, bcmCosqThresholdPacketDescriptors, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralPdTh register MC_Pd_Th */
    rv = set_device_drop_threshold(unit, 1, -1, bcmCosqThresholdPacketDescriptors, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralPdTh register Total_Pd_Th */
    rv = set_device_drop_threshold(unit, -1, -1, bcmCosqThresholdPacketDescriptors, 2000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralPdTh register MC_PD_SP_0_TH */
    rv = set_device_drop_threshold(unit, 2, -1, bcmCosqThresholdPacketDescriptors, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralPdTh register MC_PD_SP_1_TH */
    rv = set_device_drop_threshold(unit, 3, -1, bcmCosqThresholdPacketDescriptors, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralDbTh register UC_DB_Th */
    rv = set_device_drop_threshold(unit, 0, -1, bcmCosqThresholdDataBuffers, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralDbTh register MC_DB_Th */
    rv = set_device_drop_threshold(unit, 1, -1, bcmCosqThresholdDataBuffers, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralDbTh register Total_DB_Th */
    rv = set_device_drop_threshold(unit, -1, -1, bcmCosqThresholdDataBuffers, 2000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralDbTh register MC_DB_SP_0_TH */
    rv = set_device_drop_threshold(unit, 2, -1, bcmCosqThresholdDataBuffers, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralDbTh register MC_DB_SP_1_TH */
    rv = set_device_drop_threshold(unit, 3, -1, bcmCosqThresholdDataBuffers, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* Per TC */    
    for (sp = 0; sp < 2; sp++) {       

        for (cosq = 0; cosq < 8; cosq++) {   
            /* CgmMcPdSpTcTh register CgmMcPdSpTcTh0-CgmMcPdSpTcTh15 */         
            rv = set_device_drop_threshold(unit, 2+sp, cosq, bcmCosqThresholdPacketDescriptors, 1000);
            if (rv != BCM_E_NONE) return rv;
            

            /* CgmMcDbSpTcTh register CgmMcDbSpTcTh0-CgmMcDbSpTcTh15 */
            rv = set_device_drop_threshold(unit, 2+sp, cosq, bcmCosqThresholdDataBuffers, 10000);
            if (rv != BCM_E_NONE) return rv;

        }
    }
          

    return rv;
}

/*
 * Main function 
 * Jericho functionality example 
 */
int egress_threshold_jericho_example(int unit, int bcm_local_port_id) {
    bcm_error_t rv = BCM_E_NONE;
    int sp, cosq, dp, val; 
    int core = 0;
    int num_of_cores;

    bcm_stk_modid_count(unit, &num_of_cores);
    

    /* Core Traffic Resources */
    cosq = -1;
    for (core = 0; core < num_of_cores; core++) {
        rv = set_core_threshold(unit, core, -1, cosq, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_DROP, 1000);
        if (rv != BCM_E_NONE) return rv;
        rv = set_core_threshold(unit, core, -1, cosq, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
        if (rv != BCM_E_NONE) return rv;
        
        rv = set_core_threshold(unit, core, -1, cosq, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_DROP, 1000);
        if (rv != BCM_E_NONE) return rv;
        rv = set_core_threshold(unit, core, -1, cosq, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
        if (rv != BCM_E_NONE) return rv;
    }

    /* Unicast Core Level */
    cosq = -1;
    for (core = 0; core < num_of_cores; core++) {
        rv = set_core_threshold(unit, core, uc, cosq, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_DROP, 1000);
        if (rv != BCM_E_NONE) return rv;
        rv = set_core_threshold(unit, core, uc, cosq, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
        if (rv != BCM_E_NONE) return rv;
        
        rv = set_core_threshold(unit, core, uc, cosq, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_DROP, 1000);
        if (rv != BCM_E_NONE) return rv;
        rv = set_core_threshold(unit, core, uc, cosq, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
        if (rv != BCM_E_NONE) return rv;
    }

    /* Unicast Interface Level */
    rv = set_interface_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
    if (rv != BCM_E_NONE) return rv;
    rv = set_interface_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
    if (rv != BCM_E_NONE) return rv;

    rv = set_interface_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdPacketDescriptorsMin, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
    if (rv != BCM_E_NONE) return rv;
    rv = set_interface_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdPacketDescriptorsMax, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 2000);
    if (rv != BCM_E_NONE) return rv;
    rv = set_interface_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdPacketDescriptorsAlpha, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 2);
    if (rv != BCM_E_NONE) return rv;

    rv = set_interface_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdDataBuffersMin, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
    if (rv != BCM_E_NONE) return rv;
    rv = set_interface_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdDataBuffersMax, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 2000);
    if (rv != BCM_E_NONE) return rv;
   rv = set_interface_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdDataBuffersAlpha, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 2);
    if (rv != BCM_E_NONE) return rv;



    /* Unicast Port/Queue Level */
    /* port */
    rv = set_port_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_DROP, 1000);
    if (rv != BCM_E_NONE) return rv;
    rv = set_port_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_DROP, 1000); 
    if (rv != BCM_E_NONE) return rv;

    rv = set_port_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
    if (rv != BCM_E_NONE) return rv;
    rv = set_port_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000); 
    if (rv != BCM_E_NONE) return rv;

    rv = set_port_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdPacketDescriptorsMin, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
    if (rv != BCM_E_NONE) return rv;
    rv = set_port_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdPacketDescriptorsMax, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 2000);
    if (rv != BCM_E_NONE) return rv;
    rv = set_port_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdPacketDescriptorsAlpha, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 2);
    if (rv != BCM_E_NONE) return rv;

    rv = set_port_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdDataBuffersMin, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000); 
    if (rv != BCM_E_NONE) return rv;
    rv = set_port_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdDataBuffersMax, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 2000); 
    if (rv != BCM_E_NONE) return rv;
    rv = set_port_threshold(unit, bcm_local_port_id, uc, bcmCosqThresholdDataBuffersAlpha, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 2); 
    if (rv != BCM_E_NONE) return rv;


    /* queue */
    for (dp = 0 ; dp < 4; dp++) {
       for (cosq = 0 ; cosq < 8; cosq++) {
           rv = set_queue_threshold(unit , bcm_local_port_id, uc, cosq, dp,
                                    bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_DROP, 1000); 
           if (rv != BCM_E_NONE) return rv;
           rv = set_queue_threshold(unit , bcm_local_port_id, uc, cosq, dp,
                                    bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_DROP, 1000); 
           if (rv != BCM_E_NONE) return rv;
           
           rv = set_queue_threshold(unit , bcm_local_port_id, uc, cosq, dp,
                                    bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000); 
           if (rv != BCM_E_NONE) return rv;
           rv = set_queue_threshold(unit , bcm_local_port_id, uc, cosq, dp,
                                    bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000); 
           if (rv != BCM_E_NONE) return rv;
           
           rv = set_queue_threshold(unit , bcm_local_port_id, uc, cosq, dp,
                                    bcmCosqThresholdPacketDescriptorsMin, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000); 
           if (rv != BCM_E_NONE) return rv;
           rv = set_queue_threshold(unit , bcm_local_port_id, uc, cosq, dp,
                                    bcmCosqThresholdPacketDescriptorsMax, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 2000); 
           if (rv != BCM_E_NONE) return rv;
           rv = set_queue_threshold(unit , bcm_local_port_id, uc, cosq, dp,
                                    bcmCosqThresholdPacketDescriptorsAlpha, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 2); 
           if (rv != BCM_E_NONE) return rv;
           
           rv = set_queue_threshold(unit , bcm_local_port_id, uc, cosq, dp,
                                    bcmCosqThresholdDataBuffersMin, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000); 
           if (rv != BCM_E_NONE) return rv;
           rv = set_queue_threshold(unit , bcm_local_port_id, uc, cosq, dp,
                                    bcmCosqThresholdDataBuffersMax, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 2000); 
           if (rv != BCM_E_NONE) return rv;
           rv = set_queue_threshold(unit , bcm_local_port_id, uc, cosq, dp,
                                    bcmCosqThresholdDataBuffersAlpha, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 2); 
           if (rv != BCM_E_NONE) return rv;
       }
    }

    /* Multicast Reserved Resources */
    /* requires traffic disabled */
    /* not included in this example */


    /* Multicast Core Level */
    cosq = -1;
    for (core = 0; core < num_of_cores; core++) {
        rv = set_core_threshold(unit, core, mc, cosq, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_DROP, 1000);
        if (rv != BCM_E_NONE) return rv;
        rv = set_core_threshold(unit, core, mc, cosq, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
        if (rv != BCM_E_NONE) return rv;
        
        rv = set_core_threshold(unit, core, mc, cosq, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_DROP, 1000);
        if (rv != BCM_E_NONE) return rv;
        rv = set_core_threshold(unit, core, mc, cosq, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
        if (rv != BCM_E_NONE) return rv;
    }

    /* Multicast TC Level */
    for (core = 0; core < num_of_cores; core++) {
        for (cosq = 0; cosq < 8; cosq++) {
        rv = set_core_threshold(unit, core, mc, cosq, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
        if (rv != BCM_E_NONE) return rv;

        rv = set_core_threshold(unit, core, mc, cosq, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
        if (rv != BCM_E_NONE) return rv;
        }
    }

    /* Multicast Interface Level */
    rv = set_interface_threshold(unit, bcm_local_port_id, mc, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_DROP, 1000);
    if (rv != BCM_E_NONE) return rv;


    /* Multicast Port/Queue Level */
    /* port */
    rv = set_port_threshold(unit, bcm_local_port_id, mc, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_DROP, 1000);
    if (rv != BCM_E_NONE) return rv;
    rv = set_port_threshold(unit, bcm_local_port_id, mc, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_DROP, 1000); 
    if (rv != BCM_E_NONE) return rv;

    rv = set_port_threshold(unit, bcm_local_port_id, mc, bcmCosqThresholdPacketDescriptorsMin, BCM_COSQ_THRESHOLD_DROP, 1000);
    if (rv != BCM_E_NONE) return rv;
    rv = set_port_threshold(unit, bcm_local_port_id, mc, bcmCosqThresholdPacketDescriptorsMax, BCM_COSQ_THRESHOLD_DROP, 2000);
    if (rv != BCM_E_NONE) return rv;
    rv = set_port_threshold(unit, bcm_local_port_id, mc, bcmCosqThresholdPacketDescriptorsAlpha, BCM_COSQ_THRESHOLD_DROP, 2);
    if (rv != BCM_E_NONE) return rv;

    /* queue */
    for (cosq = 0; cosq < 8; cosq++) {
        for (dp = 0; dp < 4; dp++) {
            rv = set_queue_threshold(unit, bcm_local_port_id, mc, cosq, dp,
                                     bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_DROP, 1000); 
            if (rv != BCM_E_NONE) return rv;
            rv = set_queue_threshold(unit, bcm_local_port_id, mc, cosq, dp,
                                     bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_DROP, 1000); 
            if (rv != BCM_E_NONE) return rv;
            
            rv = set_queue_threshold(unit, bcm_local_port_id, mc, cosq, dp,
                                     bcmCosqThresholdPacketDescriptorsMin, BCM_COSQ_THRESHOLD_DROP, 1000); 
            if (rv != BCM_E_NONE) return rv;
            rv = set_queue_threshold(unit, bcm_local_port_id, mc, cosq, dp,
                                     bcmCosqThresholdPacketDescriptorsMax, BCM_COSQ_THRESHOLD_DROP, 2000); 
            if (rv != BCM_E_NONE) return rv;
            rv = set_queue_threshold(unit, bcm_local_port_id, mc, cosq, dp,
                                     bcmCosqThresholdPacketDescriptorsAlpha, BCM_COSQ_THRESHOLD_DROP, 2); 
            if (rv != BCM_E_NONE) return rv;
        }
    }
    
    /* Multicast Service Pool--Total */
    cosq = -1;
    for (core = 0; core < num_of_cores; core++) {
        rv = set_core_threshold(unit, core, mc_sp0 , cosq, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_DROP, 1000);
        if (rv != BCM_E_NONE) return rv;
        rv = set_core_threshold(unit, core, mc_sp1 , cosq, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_DROP, 1000);
        if (rv != BCM_E_NONE) return rv;

        rv = set_core_threshold(unit, core, mc_sp0 , cosq, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
        if (rv != BCM_E_NONE) return rv;
        rv = set_core_threshold(unit, core, mc_sp1 , cosq, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
        if (rv != BCM_E_NONE) return rv;
        
        rv = set_core_threshold(unit, core, mc_sp0 , cosq, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_DROP, 1000);
        if (rv != BCM_E_NONE) return rv;
        rv = set_core_threshold(unit, core, mc_sp1 , cosq, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_DROP, 1000);
        if (rv != BCM_E_NONE) return rv;
        
        rv = set_core_threshold(unit, core, mc_sp0 , cosq, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
        if (rv != BCM_E_NONE) return rv;
        rv = set_core_threshold(unit, core, mc_sp1 , cosq, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
        if (rv != BCM_E_NONE) return rv;
    }


    /* Multicast Service Pool--per TC */
    for (core = 0; core < num_of_cores; core++) {
        for (sp = mc_sp0; sp <= mc_sp1; sp++) {       
            for (cosq = 0; cosq < 8; cosq++) {   
                rv = set_core_threshold(unit, core, sp, cosq, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_DROP, 1000);
                if (rv != BCM_E_NONE) return rv;
                rv = set_core_threshold(unit, core, sp, cosq, bcmCosqThresholdPacketDescriptors, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 1000);
                if (rv != BCM_E_NONE) return rv;
                
                rv = set_core_threshold(unit, core, sp, cosq, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_DROP, 10000);
                if (rv != BCM_E_NONE) return rv;
                rv = set_core_threshold(unit, core, sp, cosq, bcmCosqThresholdDataBuffers, BCM_COSQ_THRESHOLD_FLOW_CONTROL, 10000);
                if (rv != BCM_E_NONE) return rv;
                
                rv = set_core_threshold(unit, core, sp, cosq, bcmCosqThresholdDataBuffersMin, BCM_COSQ_THRESHOLD_DROP, 1000);
                if (rv != BCM_E_NONE) return rv;
                rv = set_core_threshold(unit, core, sp, cosq, bcmCosqThresholdDataBuffersMax, BCM_COSQ_THRESHOLD_DROP, 10000);
                if (rv != BCM_E_NONE) return rv;
                rv = set_core_threshold(unit, core, sp, cosq, bcmCosqThresholdDataBuffersAlpha, BCM_COSQ_THRESHOLD_DROP, 2);
                if (rv != BCM_E_NONE) return rv;
            }
        }
    }
          
    return rv;
}

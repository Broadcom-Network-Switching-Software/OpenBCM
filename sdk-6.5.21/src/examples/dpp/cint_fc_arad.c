/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



/* Arad Flow Control CINT
 * This file contains examples of configuring the Arad FC Feature.
 *
 * The Arad FC Feature can be devided into the following categories:
 * - Reception - recieving FC indication from a link parter or an OOB interface:
 * - Generation - generating FC indication (pause frmes for example) to a link partner:
 * 
 * Both options can be Calendar based or Non-Calendar based.
 * The functions in this file demonstrate how to configure the Calednar based FC.
 *
 * For each function you can choose between OOB and INBAND calendar using the "is_oob" parameter.
 */

/* Creates a VSQ Entry                  */
/* in the ILKN-OOB Generation Calendar  */
/* flags should be one of the following:                      */
/*    BCM_COSQ_VSQ_LLFC, BCM_COSQ_VSQ_PFC, BCM_COSQ_VSQ_CTTC  */
int setVsqGeneration(int unit, int is_oob, int local_port, int ilkn_interface, int calendar_index, int flags)
{
  bcm_error_t rv = BCM_E_NONE;
  bcm_cosq_fc_endpoint_t source, target;
  bcm_cosq_vsq_info_t vsq_info;
  bcm_gport_t ilkn_gport;
  int my_modid;

  bcm_stk_modid_get(unit, &my_modid);
  BCM_GPORT_LOCAL_SET(ilkn_gport, local_port);  

  sal_memset(&source, 0, sizeof(source));
  sal_memset(&target, 0, sizeof(target));

  target.calender_index = calendar_index;

  vsq_info.flags = flags;
  vsq_info.src_port = ilkn_gport;

  /* Source port is a VSQ for the ILKN local port */
  bcm_cosq_gport_vsq_create(unit,&vsq_info,&source.port);

  if(is_oob) {
    /* Target port is OOB Interface                           */
    /* Thats because we are configuring the ILKN-OOB Calendar */
    BCM_GPORT_CONGESTION_SET(target.port, ilkn_interface);
  } else {
    BCM_GPORT_MODPORT_SET(target.port, my_modid, local_port);
  }

  rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &source, &target);
  if (rv != BCM_E_NONE) {
    printf("setVsqGeneration Failed: $rv\n");
    return rv;
  }

  return rv;
}

/* Creates a Retransmit Entry in the ILKN-OOB Generation Calendar  */
int setRetransmitGeneration(int unit, int is_oob, int is_for_delete, int local_port, int ilkn_interface, int calendar_index)
{
  bcm_error_t rv = BCM_E_NONE;
  bcm_cosq_fc_endpoint_t source, target;
  bcm_cosq_vsq_info_t vsq_info;
  bcm_gport_t ilkn_gport;
  int my_modid;

  bcm_stk_modid_get(unit, &my_modid);
  BCM_GPORT_LOCAL_SET(ilkn_gport, local_port);  

  sal_memset(&source, 0, sizeof(source));
  sal_memset(&target, 0, sizeof(target));

  /* Target type is Retransmit */
  target.flags = BCM_COSQ_FC_RETRANSMIT;
  target.calender_index = calendar_index;

  vsq_info.flags = BCM_COSQ_VSQ_CT;
  vsq_info.src_port = ilkn_gport;

  /* Source port should be a legal VSQ port */
  /* The actual VSQ info will not be used   */
  bcm_cosq_gport_vsq_create(unit,&vsq_info,&source.port);

  if(is_oob) {
    /* Target port is OOB Interface                           */
    /* Thats because we are configuring the ILKN-OOB Calendar */
    BCM_GPORT_CONGESTION_SET(target.port, ilkn_interface);
  } else {
    BCM_GPORT_MODPORT_SET(target.port, my_modid, local_port);
  }

  if(is_for_delete) {
    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &source, &target);
  } else {
    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &source, &target);
  }
  if (rv != BCM_E_NONE) {
    printf("setRetransmitGeneration Failed: $rv\n");
    return rv;
  }

  return rv;
}

/* Creates a Global Resources Entry     */
/* in the ILKN-OOB Generation Calendar  */
/* priority should be 0 or 1            */
int setGlobalResourcesGeneration(int unit, int is_oob, int local_port, int ilkn_interface, int calendar_index, int priority)
{
  bcm_error_t rv = BCM_E_NONE;
  bcm_cosq_fc_endpoint_t source, target;
  bcm_cosq_vsq_info_t vsq_info;
  bcm_gport_t ilkn_gport;
  int my_modid;

  bcm_stk_modid_get(unit, &my_modid);
  BCM_GPORT_LOCAL_SET(ilkn_gport, local_port);  

  sal_memset(&source, 0, sizeof(source));
  sal_memset(&target, 0, sizeof(target));

  target.calender_index = calendar_index;

  /* Cosq/Priority = 0/BCM_COSQ_HIGH_PRIORITY for HP */
  /* Cosq/Priority = 1/BCM_COSQ_LOW_PRIORITY  for LP */
  source.cosq = priority;

  vsq_info.flags = BCM_COSQ_VSQ_GL;
  vsq_info.src_port = ilkn_gport;

  /* Source port is a VSQ for the Global Resources */
  bcm_cosq_gport_vsq_create(unit,&vsq_info,&source.port);

  if(is_oob) {
    /* Target port is OOB Interface                           */
    /* Thats because we are configuring the ILKN-OOB Calendar */
    BCM_GPORT_CONGESTION_SET(target.port, ilkn_interface);
  } else {
    BCM_GPORT_MODPORT_SET(target.port, my_modid, local_port);
  }
  
  rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &source, &target);
  if (rv != BCM_E_NONE) {
    printf("setGlobalResourcesGeneration Failed: $rv\n");
    return rv;
  }

  return rv;
}

/* Creates a Retransmit Entry in the ILKN-OOB Reception Calendar  */
int setRetransmitReception(int unit, int is_oob, int is_for_delete, int local_port, int ilkn_interface, int calendar_index)
{
  bcm_error_t rv = BCM_E_NONE;
  bcm_cosq_fc_endpoint_t source, target;
  bcm_gport_t local_gport;
  int my_modid;

  bcm_stk_modid_get(unit, &my_modid);
  BCM_GPORT_LOCAL_SET(local_gport, local_port);  

  sal_memset(&source, 0, sizeof(source));
  sal_memset(&target, 0, sizeof(target));

  /* Target type is Retransmit */
  target.flags = BCM_COSQ_FC_RETRANSMIT;
  target.port = local_gport;

  source.calender_index = calendar_index;

  if(is_oob) {
    /* Source port is OOB Interface                           */
    /* Thats because we are configuring the ILKN-OOB Calendar */
    BCM_GPORT_CONGESTION_SET(source.port, ilkn_interface);
  } else {
    BCM_GPORT_MODPORT_SET(source.port, my_modid, local_port);
  }

  if(is_for_delete) {
    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &source, &target);
  } else {
    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &source, &target);
  }
  if (rv != BCM_E_NONE) {
    printf("setRetransmitReception Failed: $rv\n");
    return rv;
  }

  return rv;
}

/* Creates a NIF LL Entry              */
/* in the ILKN-OOB Reception Calendar  */
int setLLReception(int unit, int is_oob, int local_port, int ilkn_interface, int calendar_index)
{
  bcm_error_t rv = BCM_E_NONE;
  bcm_cosq_fc_endpoint_t source, target;
  bcm_gport_t ilkn_gport;
  int my_modid;

  bcm_stk_modid_get(unit, &my_modid);
  BCM_GPORT_LOCAL_SET(ilkn_gport, local_port);  

  sal_memset(&source, 0, sizeof(source));
  sal_memset(&target, 0, sizeof(target));

  /* Cosq = -1 indicates LL FC */
  target.cosq = -1;

  source.calender_index = calendar_index;

  if(is_oob) {
    /* Source port is OOB Interface                           */
    /* Thats because we are configuring the ILKN-OOB Calendar */
    BCM_GPORT_CONGESTION_SET(source.port, ilkn_interface);
  } else {
    BCM_GPORT_MODPORT_SET(source.port, my_modid, local_port);
  }
  
  /* Target port is a local port                            */
  BCM_GPORT_MODPORT_SET(target.port, 0, local_port);

  rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &source, &target);
  if (rv != BCM_E_NONE) {
    printf("setLLReception Failed: $rv\n");
    return rv;
  }

  return rv;
}

/* Creates a Port (2/8 Priority) Entry  */
/* in the ILKN-OOB Reception Calendar   */
int setPortReception(int unit, int is_oob, int local_port, int ilkn_interface, int calendar_index)
{
  bcm_error_t rv = BCM_E_NONE;
  bcm_cosq_fc_endpoint_t source, target;
  bcm_gport_t ilkn_gport;
  int my_modid;

  bcm_stk_modid_get(unit, &my_modid);
  BCM_GPORT_LOCAL_SET(ilkn_gport, local_port);  

  sal_memset(&source, 0, sizeof(source));
  sal_memset(&target, 0, sizeof(target));

  source.calender_index = calendar_index;

  if(is_oob) {
    /* Source port is OOB Interface                           */
    /* Thats because we are configuring the ILKN-OOB Calendar */
    BCM_GPORT_CONGESTION_SET(source.port, ilkn_interface);
  } else {
    BCM_GPORT_MODPORT_SET(source.port, my_modid, local_port);
  }
  
  /* Target port is a local port                            */
  BCM_GPORT_MODPORT_SET(target.port, 0, local_port);

  rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &source, &target);
  if (rv != BCM_E_NONE) {
    printf("setPortReception Failed: $rv\n");
    return rv;
  }

  return rv;
}

/* Creates a Queue PFC Entry  */
/* in the ILKN-OOB Reception Calendar   */
/* ilkn_port should be 1 or 2           */
/* ilkn_interface should be 0 or 1      */
int setQueueReception(int unit, int is_oob, int local_port, int ilkn_interface, int calendar_index, int is_mc, int tc)
{
  bcm_error_t rv = BCM_E_NONE;
  bcm_cosq_fc_endpoint_t source, target;
  bcm_gport_t ilkn_gport;
  int my_modid;

  bcm_stk_modid_get(unit, &my_modid);

  BCM_GPORT_LOCAL_SET(ilkn_gport, local_port);  

  sal_memset(&source, 0, sizeof(source));
  sal_memset(&target, 0, sizeof(target));

  /* Target Cosq is the Traffic Class (0-7) */
  target.cosq = tc;

  source.calender_index = calendar_index;

  if(is_oob) {
    /* Source port is OOB Interface                           */
    /* Thats because we are configuring the ILKN-OOB Calendar */
    BCM_GPORT_CONGESTION_SET(source.port, ilkn_interface);
  } else {
    BCM_GPORT_MODPORT_SET(source.port, my_modid, local_port);
  }
  
  /* Target port is a MC/UC Egress Queue */
  if(is_mc) {
    BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(target.port, local_port);
  } else {
    BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(target.port, local_port);
  }

  rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &source, &target);
  if (rv != BCM_E_NONE) {
    printf("setQueueReception Failed: $rv\n");
    return rv;
  }

  return rv;
}



/* Creates an HCFC Entry                  */
/* in the SPI-OOB Generation Calendar  */
/* bitmap is an array of INT[8] */
int setHCFCGeneration(int unit, int local_port, int oob_interface, int calendar_index, int tc, int *bitmap)
{
  bcm_error_t rv = BCM_E_NONE;
  bcm_cosq_fc_endpoint_t source, target;
  bcm_cosq_vsq_info_t vsq_info;
  bcm_gport_t ilkn_gport;
  int my_modid, i, bit;

  bcm_stk_modid_get(unit, &my_modid);
  BCM_GPORT_LOCAL_SET(ilkn_gport, local_port);

  sal_memset(&source, 0, sizeof(source));
  sal_memset(&target, 0, sizeof(target));

  target.flags = 0x20;
  target.cosq = tc;
  target.calender_index = calendar_index;

  vsq_info.flags = BCM_COSQ_VSQ_CT;
  vsq_info.src_port = ilkn_gport;

  /* Source port should be a legal VSQ port */
  /* The actual VSQ info will not be used   */
  bcm_cosq_gport_vsq_create(unit,&vsq_info,&source.port);

  /* Source port is a VSQ for the ILKN local port */
  bcm_cosq_gport_vsq_create(unit,&vsq_info,&source.port);

  BCM_GPORT_CONGESTION_SET(target.port, oob_interface);

  for(i=0; i<256; i++)
  {
    bit = i % 32;
    target.priority = i;
    if(bitmap[i / 32] & (0x1 << bit))
    {
      rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &source, &target);
      if (rv != BCM_E_NONE) {
        printf("setHCFCGeneration Failed: $rv (for bit $bit)\n");
        return rv;
      }
    }
    else
    {
      rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &source, &target);
      if (rv != BCM_E_NONE) {
        printf("setHCFCGeneration Failed: $rv (for bit $bit)\n");
        return rv;
      }
    }
  }

  return rv;
}

/* Creates a NIF RX (almost full indicaiton) Entry            */
/* in the ILKN-OOB Generation Calendar  */
int setNifGeneration(int unit, int is_oob, int local_port, int ilkn_interface, int calendar_index)
{
  bcm_error_t rv = BCM_E_NONE;
  bcm_cosq_fc_endpoint_t source, target;
  bcm_gport_t ilkn_gport;
  int my_modid;

  bcm_stk_modid_get(unit, &my_modid);
  BCM_GPORT_LOCAL_SET(ilkn_gport, local_port);  

  sal_memset(&source, 0, sizeof(source));
  sal_memset(&target, 0, sizeof(target));

  target.calender_index = calendar_index;

  BCM_GPORT_LOCAL_SET(source.port, local_port); 

  if(is_oob) {
    /* Target port is OOB Interface                           */
    /* Thats because we are configuring the ILKN-OOB Calendar */
    BCM_GPORT_CONGESTION_SET(target.port, ilkn_interface);
  } else {
    BCM_GPORT_MODPORT_SET(target.port, my_modid, local_port);
  }

  rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &source, &target);
  if (rv != BCM_E_NONE) {
    printf("setVsqGeneration Failed: $rv\n");
    return rv;
  }

  return rv;
}
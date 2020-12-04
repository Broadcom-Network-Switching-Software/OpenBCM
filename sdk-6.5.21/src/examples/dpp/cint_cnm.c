/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


int cnm_support(int unit, int enable)
{
    bcm_error_t           rv = BCM_E_NONE;
    
    rv = bcm_switch_control_set(unit, bcmSwitchCongestionCnm, enable);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_switch_control_set $rv \n");
      return rv;
    }

    return rv;
}

int cnm_queue_range(int unit, int min_queue, int max_queue)
{
  bcm_error_t           rv = BCM_E_NONE;

  rv = bcm_cosq_control_set(unit, 0, 0, bcmCosqControlCongestionManagedQueueMin, min_queue);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_cosq_control_set (min) $rv \n");
    return rv;
  }

  rv = bcm_cosq_control_set(unit, 0, 0, bcmCosqControlCongestionManagedQueueMax, max_queue);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_cosq_control_set (max) $rv \n");
    return rv;
  }

  return rv;
}

int cnm_cpq_enable(int unit, int voq_index, int tc, int enable)
{
  bcm_error_t           rv = BCM_E_NONE;
  bcm_gport_t           voq_gport;
  
  BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, voq_index);

  rv = bcm_cosq_control_set(unit, voq_gport, tc, bcmCosqControlCongestionManagedQueue, enable);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_cosq_control_set $rv \n");
    return rv;
  }

  return rv;
}

int cnm_cpq_config_example(int unit, int voq_index, int tc)
{
  bcm_error_t           rv = BCM_E_NONE;
  bcm_gport_t           voq_gport;
  bcm_cosq_qcn_config_t config;
  int i;

  BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, voq_index);

  config.size = 100;                        /* set point for queue. In Bytes */
  config.weight = 10;                       /* weight to be given to change in queue length */
  config.nbr_samples = 8;                   /* number of valid samples */
  for(i = 0; i < config.nbr_samples; i++)
  {
    config.sample_bytes[i] = 10 + (i * 10); /* sample values */
  }
  config.max_feedback = 5;                  /* maximum value of congestion feedback */
  config.quantization = 2;                  /* quantization value */
  config.randomize_sample_threshold = 0;    /* TRUE/FALSE */

  rv = bcm_cosq_qcn_config_set(unit, voq_gport, tc, 0, &config);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_cosq_qcn_config_set $rv \n");
    return rv;
  }

  return rv;
}

int cnm_transport_attributes_example(int unit)
{
  bcm_error_t                   rv = BCM_E_NONE;
  bcm_port_congestion_config_t  config;
  int                           src_local_port = 13;
  int                           my_modid;
  int                           vlan_priority = 2, vlan_cfi = 0, vlan_id = 1;
  bcm_mac_t                     mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x01, 0x23};

  bcm_stk_modid_get(unit, &my_modid);

  config.flags = BCM_PORT_CONGESTION_CONFIG_QCN | BCM_PORT_CONGESTION_CONFIG_TX;
  BCM_GPORT_MODPORT_SET(config.src_port, my_modid, src_local_port);
  config.traffic_class = 2;
  config.vlan = vlan_id;
  config.cfi = vlan_cfi;
  config.pri = vlan_priority;
  config.src_mac = mac_address;
  config.ethertype = 0x22E7;
  config.version = 1;
  config.reserved_v = 0;
  
  rv = bcm_port_congestion_config_set(unit, 0, &config);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_port_congestion_config_set $rv \n");
    return rv;
  }

  return rv;
}

/*
 * This function cnm_field_group_set creates a Field Processor Field Group of 
 * Direct Extraction type. 
 * The Field group sets the Statistic-Tag to be equal to the local port.  
 *  
 * run:
 * cint cint_field_cnm_stag_from_in_port.c
 * cint
 * cnm_field_group_example(int unit); 
 */

int cnm_field_group_set(/* in */  int unit,
                        /* in */  int group_priority,
                        /* in */ bcm_field_group_t group) 
{
  int result;
  bcm_field_group_config_t grp;
  bcm_field_aset_t aset;
  bcm_field_entry_t ent;
  bcm_field_extraction_action_t extract;
  bcm_field_extraction_field_t ext;
  
  bcm_field_group_config_t_init(&grp);
  grp.group = group;

  /* 
   * Define the QSET - use InLIF as qualifier. 
   */
  BCM_FIELD_QSET_INIT(grp.qset);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyInPort);

  /*
   *  Define the ASET - Statistic-Tag.
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionStatTag);

  /*  Create the Field group with type Direct Extraction */
  grp.priority = group_priority;
  grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID;
  grp.mode = bcmFieldGroupModeDirectExtraction;
  result = bcm_field_group_config_create(unit, &grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      return result;
  }

  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp.group, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set\n");
    return result;
  }

  /* Create the Direct Extraction entry:
   *  1. create the entry
   *  2. Construct the action:
   *     bits 7:0 = InPort
   *  3. Install entry (HW configuration)
   */
  result = bcm_field_entry_create(unit, grp.group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  bcm_field_extraction_field_t_init(&ext);

  extract.action = bcmFieldActionStatTag;
  extract.bias = 0;

  /* Extraction structure indicates to use InPort qualifier */
  ext.flags = 0;
  ext.bits  = 8;
  ext.lsb   = 0;
  ext.qualifier = bcmFieldQualifyInPort;

  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  extract,
                                                  1 /* count */,
                                                  &ext);

  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_direct_extraction_action_add\n");
    return result;
  }

  /* Write entry to HW */
  result = bcm_field_entry_install(unit, ent);
  if (BCM_E_NONE != result) {
    return result;
  }
  
  return result;
}


/*
 *  Set an example configuration of this cint
 */
int cnm_field_group_example(int unit) 
{
  int result;
  int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
  bcm_field_group_t group = 7;
  result = cnm_field_group_set(unit, group_priority, group);
  if (BCM_E_NONE != result) {
      printf("Error in cnm_field_group_set\n");
      return result;
  }

  return result;
}


int cnm_example_test_run(int unit)
{
    int i;
    int rv;
    int cos;
    int port=13;
    int BW=50000;
    bcm_gport_t gport;

    rv=cnm_support(unit, 1);
    if(0 != rv){
            return  -1;
        }
    rv=cnm_queue_range(unit,136,143);
    if(0 != rv){
            return  -1;
        }

    for (cos=0 ; cos<8 ; ++cos) {
        rv=cnm_cpq_enable(unit, 136, cos, 1);
        if(0 != rv){
            printf("cos=%d\n",cos);
            return  -1;
        }
    }

    for (cos=0 ; cos<8 ; ++cos) {
        rv=cnm_cpq_config_example(unit, 136, cos);
        if(0 != rv){
            printf("cos=%d\n",cos);
            return  -1;
        }
    }

    rv=cnm_transport_attributes_example(unit);
    if(0 != rv){
            return  -1;
        }

    rv=cnm_field_group_example(unit);
    if(0 != rv){
            return  -1;
        }

    rv=bcm_port_force_forward_set(unit,port,14,1);
    if(0 != rv){
            return  -1;
        }
    
    BCM_GPORT_LOCAL_SET(&gport, 14);
    rv= bcm_cosq_gport_bandwidth_set(unit, gport, 0, 0, BW, 0);
    if(0 != rv){
            return  -1;
        }
    return 0;
}

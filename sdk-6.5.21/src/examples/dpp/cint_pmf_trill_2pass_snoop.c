/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*-----------------------------------------------------------------------------------------
    Build qualifier set for the group we want to create.  Entries must exist
    within groups, and groups define what kind of things the entries can
    match, by their inclusion in the group's qualitier set.
   
    The Field group is done at the ingress stage
    The Field group qualifiers are EtherType (to match only IPv4 packets),
    Build the action set for this Field group.  Entries can only do
    something that is included on this list.
   
    This Field Group can perform Snooping and change the Internal
    priority */
   

int trill_2pass_field_group_set( int unit, bcm_gport_t *snoop_trap_gport_id, int snoop_trap_num, bcm_field_group_t *group) 
{
  int result;
  int auxRes;
  int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  uint8 data, mask;
  uint8 mbit_data, mbit_mask;
  int class_port;
  int presel_id = 0;
  int presel_flags = 0;
  bcm_field_presel_set_t psset;

  printf("\t===============  PMF rule set   ==================\n");

  /* Create a presel entity */
  if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
      presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
      result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
  } else {
      result = bcm_field_presel_create(unit, &presel_id);
  }
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_presel_create\n");
      return result;
  }

  result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_Stage\n");
      return result;
  }

  /* Ethernet forwarding */
  result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 
                                            bcmFieldForwardingTypeTrill);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_ForwardingType\n");
      return result;
  }


  /* Define the preselector-set */
  BCM_FIELD_PRESEL_INIT(psset);
  BCM_FIELD_PRESEL_ADD(psset, presel_id);

  /*define the qualifier*/
  BCM_FIELD_QSET_INIT(&qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpTunnelHit);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyHeaderFormat);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassPort);
  
  printf("\t-----------  Qset  ------------\n");
  print qset;
  printf("\t--------------------------------------------------\n");

  /*define the action*/
  BCM_FIELD_ASET_INIT(&aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionSnoop);
  printf("\t-----------  Aset  ------------\n");
  print aset;
  printf("\t--------------------------------------------------\n");

  group_priority = 61;
  /*  Create the Field group */
  printf("\t----------------- field group create ----------------------------\n");

  result = bcm_field_group_create(unit, qset ,group_priority ,&grp);
  if ( result !=BCM_E_NONE) {
      printf("Error in bcm_field_group_create\n");
      return result;
  }
  printf("\t Group 0x%x \n",grp);

  result = bcm_field_group_presel_set(unit, grp, &psset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        return result;
  }

  printf("\t------------------ field group action set --------------------------\n");

  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp, aset);
  if (result != BCM_E_NONE) {
      printf("Error in bcm_field_group_action_set\n");
    return result;
  }
  /*printf("\t--------------------------------------------------\n");*/

  /*
   *  Add multiple entries to the Field group.
   *  For Arad
   *  class_port=0: Normal port, we want to snoop it
   *  class_port=1: recycle port, we don't want to recycle
   *  For Jericho
   *  class_port=0: Normal port for core 0, we want to snoop it to core 0 recycle port
   *  class_port=1: Normal port for core 1, we want to snoop it to core 1 recycle port
   *  class_port=2: recycle port, we don't want to recycle
   */

  for (class_port = 0; class_port <= snoop_trap_num; ++class_port) {

      printf("\t------------------ bcm_field_entry_create  ---------------------\n");
      result = bcm_field_entry_create(unit, grp, &snoop_ent);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
        return result;
      }
      printf("\t Entry 0x%x \n",snoop_ent);
      printf("\t----------------- MPSL term ---------------------------\n");
      data = 1;
      mask=1;
      
      result = bcm_field_qualify_IpTunnelHit(unit, snoop_ent, data, mask);
      if (result != BCM_E_NONE) {
          printf("Error in bcm_field_qualify_IpTunnelHit\n");
        return result;
      }
      printf("\t-------------------- Header Format ------------------------\n");

      /* bcmFieldHeaderFormatEthTrillEth*/
      result = bcm_field_qualify_HeaderFormat(unit, snoop_ent,bcmFieldHeaderFormatEthTrillEth);
      if (result != BCM_E_NONE) {
          printf("Error in bcm_field_qualify_HeaderFormat\n");
        return result;
      }

      data = class_port;
      mask = 0x3; 
      result = bcm_field_qualify_InterfaceClassPort(unit, snoop_ent, data, mask);
      if (result != BCM_E_NONE) {
          printf("Error in bcm_field_qualify_HeaderFormat\n");
        return result;
      }
      
      printf("\t------------------- field action add --------------------------\n");

      /* Snoop if did not arrive from recycle port */
      if (class_port != snoop_trap_num) {
          result = bcm_field_action_add(unit, snoop_ent, bcmFieldActionSnoop, snoop_trap_gport_id[class_port], 0);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_action_add\n");
            return result;
          }
      }

      /* Set higher priority on recycle port entry (class_port=1) */
      result= bcm_field_entry_prio_set(unit, snoop_ent, class_port);
      if (result != BCM_E_NONE)
      {
        printf("ERROR: bcm_field_entry_prio_set returned %s\n", bcm_errmsg(result));
      }
  }

  printf("\t------------------- group install----------------------------\n");

  /*
   *  Commit the entire group to hardware.  We could do individual entries,
   *  but since we just created the group here and want the entry/entries all
   *  in hardware at this point, it is quicker to commit the whole group.
   */
  result = bcm_field_group_install(unit, grp);
  if (BCM_E_NONE != result) {
     printf("Error in bcm_field_group_install\n");
   return result;
  }
  
  /*
   *  Everything went well; return the group ID that we allocated earlier.
   */
  *group = grp; 

  return result;
}

/* 
 * Motivation:
 * In order to perform same interface filter, we need to have inLif = outLif and src port = dst port.
 * For Trill Transit and Trill egress packet, need a Trill LL inLif. For Trill Transit, only resolve a AC inLif.
 * The idea is to get the Trill outLif from adjacency check lookup result. This is done by PMF. 
 * 
 * Description: 
 * Set the In-LIF to be equal to the EEP (1st LEM lookup result. 1st LEM lookup is adjacency check): 
 * - if the packet is TRILL 
 * - if 1st LEM lookup is found 
 * - if the in-port is not recycle 
 * Otherwise, trap the packet if  1st LEM lookup not found and the 
 * port is not recycle 
 */
int trill_in_lif_field_group_set(/* in */  int unit,
                                /* in */  int group_priority,
                                /* in */  int group_priority_2, /* Trap if not found */
                                /* in */ bcm_field_group_t group,
                                /* in */ bcm_field_group_t group_2, /* Trap if not found */
                                /* in */ int trap_id) 
 {

  int result;
  bcm_field_group_config_t grp;
  bcm_field_aset_t aset;
  bcm_field_entry_t ent;
  bcm_field_extraction_action_t extract;
  bcm_field_extraction_field_t ext;
  bcm_field_data_qualifier_t dq_trill_lif[2];
  bcm_field_qualify_t dq_qualifier[2] = {bcmFieldQualifyL2SrcValue, bcmFieldQualifyInterfaceClassPort}; 
  uint16 dq_offset[2] = {19 /* LIF located in LEM-Lookup-Result[34:19] */, 0}; /* note: for Jericho, it's LEM-Lookup-Result[36:19]*/
  int dq_length[2] = {SOC_DPP_DEFS_GET_OUT_LIF_NOF_BITS(unit), 1};   /* lif size has changed between arad+ (16) and jer (18) */
  int dq_ndx, nof_dqs = 2;
  bcm_field_presel_set_t psset;
  int presel_id = 0;
  int presel_flags = 0;
  bcm_field_stage_t stage = bcmFieldStageIngress;
  uint8 dq_data=0x0 /*is-recycle*/, dq_mask=0x1; 
  int trap_gport;

  /* 
   *  class_port=0/1: used for normal port
   *  class_port=2: used for recycle port
   */
  if (is_device_or_above(unit, JERICHO)) {
      dq_length[1] = 2;
      dq_mask = 0x2;
  }

  /* Preselect on Trill and 1st lookup found */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
            presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
            result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
    } else {
            result = bcm_field_presel_create(unit, &presel_id);
    }
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_presel_create\n");
      return result;
  }
  result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, stage);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_Stage\n");
      return result;
  }
  result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldForwardingTypeTrill);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_ForwardingType\n");
      return result;
  }
  result = bcm_field_qualify_L2SrcHit(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x1, 0x1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_L2SrcHit\n");
      return result;
  }

  BCM_FIELD_PRESEL_INIT(psset);
  BCM_FIELD_PRESEL_ADD(psset, presel_id);

  /* 
   * Init the data qualifiers: LEM result and if recycle port.
   */
  for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
      bcm_field_data_qualifier_t_init(&dq_trill_lif[dq_ndx]); /* inner source MAC address */
      dq_trill_lif[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      dq_trill_lif[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
      dq_trill_lif[dq_ndx].offset = dq_offset[dq_ndx]; 
      dq_trill_lif[dq_ndx].length = dq_length[dq_ndx]; 
      result = bcm_field_data_qualifier_create(unit, &dq_trill_lif[dq_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }
  }
  
  bcm_field_group_config_t_init(&grp);
  grp.group = group;

  /* 
   * Define the QSET - use InLIF as qualifier. 
   */
  BCM_FIELD_QSET_INIT(grp.qset);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
  /* Add the Data qualifiers to the QSET */
  for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
      result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, dq_trill_lif[dq_ndx].qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }
  }

  /*
   *  Define the ASET - Statistic-Tag.
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionIngressGportSet);

  /*  Create the Field group with type Direct Extraction */
  grp.priority = group_priority;
  grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
  grp.preselset = psset;
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

  /* not recycle port */
  result = bcm_field_qualify_data(unit,
                                  ent,
                                  dq_trill_lif[1].qual_id,
                                  &dq_data,
                                  &dq_mask,
                                  1 /* len here always in bytes */);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_data\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  extract.action = bcmFieldActionIngressGportSet;
  extract.bias = 0;

  /* Extraction structure indicates to use InPort qualifier */
  bcm_field_extraction_field_t_init(&ext);
  ext.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
  ext.bits  = SOC_DPP_DEFS_GET_OUT_LIF_NOF_BITS(unit);
  ext.lsb   = 0;
  ext.qualifier = dq_trill_lif.qual_id;

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
      printf("Error in bcm_field_entry_install\n");
    return result;
  }

  /* Trap packet if not found */
  /* Preselect on Trill and 1st lookup not found */
  presel_id++;
  if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
      presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
      result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
  } else {
      result = bcm_field_presel_create(unit, &presel_id);
  }
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_presel_create\n");
      return result;
  }
  result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, stage);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_Stage\n");
      return result;
  }
  result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldForwardingTypeTrill);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_ForwardingType\n");
      return result;
  }
  result = bcm_field_qualify_L2SrcHit(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x0, 0x1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_L2SrcHit\n");
      return result;
  }

  BCM_FIELD_PRESEL_INIT(psset);
  BCM_FIELD_PRESEL_ADD(psset, presel_id);


  bcm_field_group_config_t_init(&grp);
  grp.group = group_2;

  /* 
   * Define the QSET - use InLIF as qualifier. 
   */
  BCM_FIELD_QSET_INIT(grp.qset);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
  /* Add the Data qualifiers to the QSET */
  result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, dq_trill_lif[1].qual_id);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qset_data_qualifier_add\n");
      return result;
  }

  /*
   *  Define the ASET - Statistic-Tag.
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);

  /*  Create the Field group with type Direct Extraction */
  grp.priority = group_priority_2;
  grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
  grp.preselset = psset;
  grp.mode = bcmFieldGroupModeAuto;
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

  /* 
   * Create the TCAM entry: trap if not recycle
   */
  result = bcm_field_entry_create(unit, grp.group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  /* not recycle port */
  result = bcm_field_qualify_data(unit,
                                  ent,
                                  dq_trill_lif[1].qual_id,
                                  &dq_data,
                                  &dq_mask,
                                  1 /* len here always in bytes */);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_data\n");
      return result;
  }

  /* encode trap as gport */   
  BCM_GPORT_TRAP_SET(trap_gport, trap_id, 7, 0);
  result = bcm_field_action_add(unit, ent, bcmFieldActionTrap, trap_gport, 0);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_action_add\n");
      return result;
  }

  result = bcm_field_group_install(unit, grp.group);
  if (BCM_E_NONE != result) {
     printf("Error in bcm_field_group_install\n");
   return result;
  }


  printf("Exit in lif\n");
  
  return result;
}

int trill_2pass_field_group_unset( int unit,  bcm_field_group_t group) 
{
  int result;
  int auxRes;
  int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  uint8 data, mask;
  uint8 mbit_data, mbit_mask;
  bcm_field_entry_t ent[128];
  int entCount;
  int entIndex;


  
  do {
    /* get a bunch of entries in this group */
    result = bcm_field_entry_multi_get(unit, group, 128, &(ent[0]), entCount);
    if (BCM_E_NONE != result) {
      return result;
    }
    for (entIndex = 0; entIndex < entCount; entIndex++) {
      /* remove each entry from hardware and destroy it */
      result = bcm_field_entry_remove(unit, ent[entIndex]);
      if (BCM_E_NONE != result) {
        return result;
      }
      result = bcm_field_entry_destroy(unit, ent[entIndex]);
      if (BCM_E_NONE != result) {
        return result;
      }
    }
    /* all as long as there were entries to remove & destroy */
  } while (entCount > 0);

  result = bcm_field_group_remove(unit, group);
  if (BCM_E_NONE != result) {
     printf("Error in bcm_field_group_remove\n");
     return result;
  }

  
   result = bcm_field_group_destroy(unit,group);
   if (BCM_E_NONE != result) {
     printf("Error in bcm_field_group_install\n");
   return result;
  }
  return result;
}

/* 
 * Unset the trill in lif configuation: 
 * - remove and destroy the created entries
 * - remove and destroy the created groups
 */
int trill_in_lif_field_group_unset( int unit,  bcm_field_group_t group, bcm_field_group_t group2) 
{
  int i;
  int result;
  bcm_field_group_t grp[2] = {group, group2};
  bcm_field_entry_t ent[128];
  int entCount;
  int entIndex;


  for (i = 0; i < 2; i++) {
    if (grp[i] == 0) {
        continue;
    }   
    do {
      /* get a bunch of entries in this group */
      result = bcm_field_entry_multi_get(unit, grp[i], 128, &(ent[0]), entCount);
      if (BCM_E_NONE != result) {
        return result;
      }
      for (entIndex = 0; entIndex < entCount; entIndex++) {
        /* remove each entry from hardware and destroy it */
        result = bcm_field_entry_remove(unit, ent[entIndex]);
        if (BCM_E_NONE != result) {
          return result;
        }
        result = bcm_field_entry_destroy(unit, ent[entIndex]);
        if (BCM_E_NONE != result) {
          return result;
        }
      }
      /* all as long as there were entries to remove & destroy */
    } while (entCount > 0);
  
    result = bcm_field_group_remove(unit, grp[i]);
    if (BCM_E_NONE != result) {
       printf("Error in bcm_field_group_remove\n");
       return result;
    }
    
     result = bcm_field_group_destroy(unit,grp[i]);
     if (BCM_E_NONE != result) {
       printf("Error in bcm_field_group_install\n");
     return result;
    }
  }
  return result;
}



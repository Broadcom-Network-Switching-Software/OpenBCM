
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * Rebuild the Learn-Data for MAC-in-MAC application. 
 * Used in case of Egress PBB, MAC-in-MAC termination.
 *  
 *  Sequence:
 *  1. Define all the qualifiers / data qualifiers
 *  2. Create the Field groups (need multiple field groups due to
 *  implementation limitations)
 *  3. Add Direct extracion entry per Field group
 */

/*
 *  Set the Field groups that: 
 *  - Set the Learnt-information
 *  - Set Learn-Enable
 *  
 *  Match according to EoE and Forwarding offset index 2 and LEM 1st lookup found.
 *  Note: User choice of settings Learn-enable. Can be ingress or egress learning (depends on system configuration).
 *  In this CINT example we set egress learning.
 */
int mim_learn_info_field_group_set(/* in */ int unit,
                                       /* in */ int group_priority,
                                       /* out */ bcm_field_group_t *group0, 
                                       /* out */ bcm_field_group_t *group1, 
                                       /* out */ bcm_field_group_t *group2) 
{
  int result;
  int auxRes;
  bcm_field_group_config_t grp[3];
  uint32 grp_ndx, grp_ndx2;
  bcm_field_data_qualifier_t data_qual[5]; /* the 4 data qualifiers */
  uint8 dq_data[2], dq_mask[2];
  uint32 dq_id, dq_ndx, dq_ndx2, 
      dq_lsb[3] = {0, 2, 4}, /* in bytes */
      dq_length[3] = {16, 16, 8};
  bcm_field_qualify_t dq_qualifier[3] = {bcmFieldQualifyL2SrcValue, bcmFieldQualifyL2SrcValue, 
      bcmFieldQualifyL2SrcValue};
  bcm_field_action_t grp_action[2] = {bcmFieldActionLearnSrcPortNew, bcmFieldActionLearnInVPortNew};
  bcm_field_entry_t ent[3];
  bcm_field_extraction_field_t ext[3];
  bcm_field_extraction_action_t extact;
  uint8 dqMask[2], dqData[2];
  int i, presel_id;
  bcm_field_presel_set_t psset;
  bcm_pbmp_t pbm;
  bcm_pbmp_t pbm_mask;
  bcm_field_entry_t ent_invport;
  bcm_gport_t vlan_port_id;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  int presel_flags = 0;

  if ((NULL == group0) || (NULL == group1)) {
    printf("Pointer to group ID must not be NULL\n");
    return BCM_E_PARAM;
  }
          
  /* Create a presel entity */
  if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
    presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
    if (BCM_E_NONE != result) {
      printf("Error in bcm_field_presel_create_stage_id\n");
      auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
      return result;
    }
  } else {
    result = bcm_field_presel_create_id(unit, presel_id);
    if (BCM_E_NONE != result) {
      printf("Error in bcm_field_presel_create_id\n");
      auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
      return result;
    }
  }

  /* Match on packets with Outer-Ethernet removed */
  result = bcm_field_qualify_ForwardingHeaderOffset(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldDataOffsetBaseFirstHeader, 0, 0);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_qualify_ForwardingHeaderOffset\n");
    return result;
  }

  result = bcm_field_qualify_HeaderFormat(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldHeaderFormatEthEth);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_qualify_HeaderFormat\n");
    return result;
  }

  /* Source lookup in the forwarding table is found */
  result = bcm_field_qualify_L2SrcHit(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x1, 0x1);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_qualify_L2SrcHit\n");
    return result;
  }

  BCM_FIELD_PRESEL_INIT(psset);
  BCM_FIELD_PRESEL_ADD(psset, presel_id);

  /* 
   * 2 Field groups must be created, because in the HW the Learn-Data 
   * action is split into 2 actions, one of 16b (regular Direct 
   * Extraction DB), and one of 24b (large DE DB, particular 
   * treatment). 
   */

  for (dq_ndx = 0; dq_ndx < 3; dq_ndx++) {
      bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]); 
      data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      data_qual[dq_ndx].offset = dq_lsb[dq_ndx];  
      data_qual[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
      data_qual[dq_ndx].length = dq_length[dq_ndx]; 
      result = bcm_field_data_qualifier_create(unit, &data_qual[dq_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }
  }


  for (grp_ndx = 0; grp_ndx < 2; grp_ndx++) {
      bcm_field_group_config_t_init(&grp[grp_ndx]);
      grp[grp_ndx].group = -1;
      /* Define the QSET */
      BCM_FIELD_QSET_INIT(grp[grp_ndx].qset);
      BCM_FIELD_QSET_ADD(grp[grp_ndx].qset, bcmFieldQualifyStageIngress);

      for (dq_ndx = (grp_ndx? 1: 0); dq_ndx < (grp_ndx? 3: 1); dq_ndx++) {
          /* Add the Data qualifier to the QSET */
          result = bcm_field_qset_data_qualifier_add(unit, &grp[grp_ndx].qset, data_qual[dq_ndx].qual_id);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qset_data_qualifier_add\n");
              return result;
          }
      }

      /*
       *  This Field Group can change the User-Header-0 value
       */
      BCM_FIELD_ASET_INIT(grp[grp_ndx].aset);
      BCM_FIELD_ASET_ADD(grp[grp_ndx].aset, grp_action[grp_ndx]);

      /*  Create the Field group */
      grp[grp_ndx].priority = group_priority + grp_ndx;
      grp[grp_ndx].flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
      grp[grp_ndx].mode = bcmFieldGroupModeDirectExtraction;
      grp[grp_ndx].preselset = psset;
      result = bcm_field_group_config_create(unit, &grp[grp_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_create\n");
          return result;
      }

      /*
       *  Add a single entry to the Field group.
       */
      result = bcm_field_entry_create(unit, grp[grp_ndx].group, &ent[grp_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
          return result;
      }


      bcm_field_extraction_action_t_init(&extact);
      extact.action = grp_action[grp_ndx];
      extact.bias = 0;
      dq_id = 0;
      for (dq_ndx = (grp_ndx? 1: 0); dq_ndx < (grp_ndx? 3: 1); dq_ndx++) {
          bcm_field_extraction_field_t_init(&(ext[dq_id]));
          ext[dq_id].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
          ext[dq_id].qualifier = data_qual[dq_ndx].qual_id;
          ext[dq_id].lsb = 0;
          ext[dq_id].bits = dq_length[dq_ndx];
          dq_id++;
      }
      result = bcm_field_direct_extraction_action_add(unit,
                                                      ent[grp_ndx],
                                                      extact,
                                                      dq_id /* count */,
                                                      ext);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_direct_extraction_action_add\n");
          return result;
      }

      result = bcm_field_group_install(unit, grp[grp_ndx].group);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_install\n");
          return result;
      }
  }

  /* 
   * Set another Field group to enable learning
   */
  grp_ndx = 2;
  BCM_FIELD_QSET_INIT(grp[grp_ndx].qset);
  BCM_FIELD_QSET_ADD(grp[grp_ndx].qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(grp[grp_ndx].qset, bcmFieldQualifyHeaderFormat);
  grp[grp_ndx].preselset = psset;
  grp[grp_ndx].priority = group_priority + 2;
  BCM_FIELD_ASET_INIT(grp[grp_ndx].aset);
  BCM_FIELD_ASET_ADD(grp[grp_ndx].aset, bcmFieldActionDoNotLearn);
  grp[grp_ndx].flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
  grp[grp_ndx].mode = bcmFieldGroupModeDirect;
  result = bcm_field_group_config_create(unit, &grp[grp_ndx]);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_group_config_create\n");
    return result;
  }

  result = bcm_field_entry_create(unit, grp[grp_ndx].group, &(ent[grp_ndx]));
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_entry_create\n");
    return result;
  }
  result = bcm_field_qualify_HeaderFormat(unit, ent[grp_ndx], bcmFieldHeaderFormatEthEth);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_qualify_HeaderFormat\n");
    return result;
  }

  /* Learn-Enable: 1 for ingress, 2 for egress */
  result = bcm_field_action_add(unit, ent[grp_ndx], bcmFieldActionDoNotLearn, 2, 0);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_action_add\n");
    return result;
  }

  result = bcm_field_group_install(unit, grp[grp_ndx].group);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_group_install\n");
    return result;
  }


  /*
   *  Everything went well; return the group ID that we allocated earlier.
   */
  *group0 = grp[0].group; 
  *group1 = grp[1].group; 
  *group2 = grp[2].group; 

  return result;
}

/*
 *  Set the previous configurations for an example configuration
 *  Set MIM learning information example.
 */
int mim_learn_info_example(int unit) 
{
  int result;
  int group_priority = 20;
  bcm_field_group_t group[3];

 
  result = mim_learn_info_field_group_set(unit, group_priority, &group[0], &group[1], &group[2]);
  if (BCM_E_NONE != result) {
      printf("Error in mim_learn_info_set\n");
      return result;
  }

  return result;
}

int mim_learn_info_set_teardown(/* in */ int unit,
                        /* in */ bcm_field_group_t group) 
{
  int result;
  int auxRes;

  /* Destroy the Field group (and all its entries) */  
  result = bcm_field_group_destroy(unit, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_destroy\n");
      return result;
  }

  return result;
}


int mim_qualify_ethertype_config(/* in */ int unit,
                                 /* in */ int group_priority,
                                 /* in */ bcm_field_group_t group,
                                 /* in */ uint16 etype,
                                 /* in */ int stat_id)
{
  int result;
  int auxRes;
  bcm_field_entry_t ent;
  bcm_field_group_config_t mim_grp;
  bcm_field_group_config_t_init(&mim_grp);
  mim_grp.priority = group_priority;
  mim_grp.group = group;
  bcm_field_stat_t stats = bcmFieldStatPackets;

  BCM_FIELD_QSET_INIT(mim_grp.qset);
  BCM_FIELD_QSET_ADD(mim_grp.qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(mim_grp.qset, bcmFieldQualifyInnerEtherType);

  BCM_FIELD_ASET_INIT(mim_grp.aset);
  BCM_FIELD_ASET_ADD(mim_grp.aset,bcmFieldActionDrop);
  BCM_FIELD_ASET_ADD(mim_grp.aset,bcmFieldActionStat);

  mim_grp.flags |= BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_MODE;
  mim_grp.mode = bcmFieldGroupModeAuto;

  result = bcm_field_group_config_create(unit, &mim_grp);
  if (result != BCM_E_NONE) {
    printf("Error in bcm_field_group_config_create %d (%s)\n", result, bcm_errmsg(result));
    return result;
  }
  result = bcm_field_group_action_set(unit, mim_grp.group, mim_grp.aset);
  if (result != BCM_E_NONE) {
    printf("Error in bcm_field_group_action_set, grp=%d %d (%s)\n ", mim_grp.group, result, bcm_errmsg(result));
    auxRes = bcm_field_group_destroy(unit, group);
    if (BCM_E_NONE != auxRes) {
      printf("Error in bcm_field_group_destroy\n");
    }
    return result;
  }

  result = bcm_field_entry_create(unit, mim_grp.group, &ent);
  if (result != BCM_E_NONE) {
    printf("Error in bcm_field_entry_create %d (%s)\n", result, bcm_errmsg(result));
    auxRes = bcm_field_group_destroy(unit, group);
    if (BCM_E_NONE != auxRes) {
      printf("Error in bcm_field_group_destroy\n");
    }
    return result;
  }
  result = bcm_field_qualify_InnerEtherType(unit, ent, etype, 0xffff);
  if (result != BCM_E_NONE) {
    printf("Error in bcm_field_qualify_InnerEtherType %d (%s)\n", result, bcm_errmsg(result));
    auxRes = bcm_field_group_flush(unit, group);
    if (BCM_E_NONE != auxRes) {
      printf("Error in bcm_field_group_flush\n");
    }
    return result;
  }
  result = bcm_field_action_add(unit, ent, bcmFieldActionDrop,0,0);
  if (result != BCM_E_NONE) {
    printf("Error in bcm_field_action_add (%s)\n", result, bcm_errmsg(result));
    auxRes = bcm_field_group_flush(unit, group);
    if (BCM_E_NONE != auxRes) {
      printf("Error in bcm_field_group_flush\n");
    }
    return result;
  }

  result= bcm_field_stat_create_id(unit, group, 1, &stats, stat_id);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_stat_create_id (%s), ent = %d \n", bcm_errmsg(result), ent);
    return rv;
  }

  result = bcm_field_action_add(unit,ent,bcmFieldActionStat, stat_id,0);

  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_action_add (%s), ent = %d \n", bcm_errmsg(result), ent);
    return rv;
  }

  result = bcm_field_group_install(unit, mim_grp.group);
  if (result != BCM_E_NONE) {
    printf("Error in bcm_field_group_install %d (%s)\n",result, bcm_errmsg(result));
    auxRes = bcm_field_group_flush(unit, group);
    if (BCM_E_NONE != auxRes) {
      printf("Error in bcm_field_group_flush\n");
    }
    return result;
  }

  return result;
}

int field_destroy(/* in */ int unit,
                  /* in */ bcm_field_group_t group)
{
  int result;

  /* Destroy the Field group (and all its entries) */
  result = bcm_field_group_flush(unit, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_flush\n");
      return result;
  }

  return result;
}

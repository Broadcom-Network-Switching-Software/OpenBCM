
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
   for VXLAN & L2Gre build the learning record to have destination FEC with value according to SIP lookup
*/


/* 
 * Rebuild the Learn-Data according to the FEC-Pointer 
 * found in a previous SIP lookup for NVGRE and VxLAN packets. 
 *  
 *  Sequence:
 *  1. Define all the qualifiers / data qualifiers
 *  2. Create the Field groups (need 2 field groups due to
 *  implementation limitations)
 *  3. Add Direct extracion entry per Field group
 */

/*
 *  Set the Field groups that:
 *  - Set the Learnt-Port
 *  - Set the Learnt-VPort
 *  
 *  A preselection is done on ETHoIPv4oETH packets that have a
 *  lookup-found result in the FEC-Pointer lookup.
 *  The rebuilt learn-data corresponds to Destination = this FEC-Pointer,
 *  all the other fields are zero.
 */
int gre_learn_data_rebuild_field_group_set(/* in */ int unit,
                                           /* in */ int group_priority,
                                           /* out */ bcm_field_group_t *group0, 
                                           /* out */ bcm_field_group_t *group1) 
{
  int result;
  int auxRes;
  bcm_field_group_config_t grp[2];
  uint32 grp_ndx, grp_ndx2;
  bcm_field_data_qualifier_t data_qual[5]; /* the 4 data qualifiers */
  uint8 dq_data[2], dq_mask[2];
  uint32 dq_id, dq_ndx, dq_ndx2, dq_length[5] = {16, 1, 1, 11, 11};
  bcm_field_qualify_t dq_qualifier[5] = {bcmFieldQualifyTunnelTerminatedValue, 
                                          bcmFieldQualifyConstantZero, 
                                          bcmFieldQualifyConstantOne, 
                                          bcmFieldQualifyConstantZero,
                                          bcmFieldQualifyConstantZero};

  bcm_field_action_t grp_action[2] = {bcmFieldActionLearnSrcPortNew, 
                                      bcmFieldActionLearnInVPortNew};

  bcm_field_entry_t ent[2];
  bcm_field_extraction_field_t ext[5];
  bcm_field_extraction_action_t extact;
  uint8 dqMask[2], dqData[2];
  int i, presel_id;
  bcm_field_presel_set_t psset;
  bcm_pbmp_t pbm;
  bcm_pbmp_t pbm_mask;
  bcm_field_group_t group2;
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

  result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_qualify_Stage\n");
    auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
    return result;
  }

  result = bcm_field_qualify_HeaderFormat(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldHeaderFormatEthIp4Eth /*bcmFieldHeaderFormatIp4*/);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_qualify_HeaderFormat\n");
    auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
    return result;
  }

  result = bcm_field_qualify_TunnelTerminated(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x1, 0x1);
  if (BCM_E_NONE != result) {
	printf("Error in bcm_field_qualify_TunnelTerminated\n");
	auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
	return result;
  }

  result = bcm_field_qualify_MplsTerminated(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x1, 0x1);
  if (BCM_E_NONE != result) {
	printf("Error in bcm_field_qualify_MplsTerminated\n");
	auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
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
  for (dq_ndx = 0; dq_ndx < 5; dq_ndx++) {
      bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]); 
      data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      data_qual[dq_ndx].offset = 0;  
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

      for (dq_ndx = (grp_ndx? 1: 0); dq_ndx < (grp_ndx? 5: 1); dq_ndx++) {
          /* Add the Data qualifier to the QSET */
          result = bcm_field_qset_data_qualifier_add(unit, &grp[grp_ndx].qset, data_qual[dq_ndx].qual_id);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qset_data_qualifier_add\n");
              return result;
          }
      }

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
      for (dq_ndx = (grp_ndx? 1: 0); dq_ndx < (grp_ndx? 5: 1); dq_ndx++) {
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
          printf("Error in bcm_field_entry_create\n");
          return result;
      }

      result = bcm_field_group_install(unit, grp[grp_ndx].group);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
          return result;
      }
  }


  /*
   *  Everything went well; return the group ID that we allocated earlier.
   */
  *group0 = grp[0].group; 
  *group1 = grp[1].group; 

  return result;
}

/*
 *  Set the previous configurations for an example configuration
 *  If match by a Field entry, snoop the packet to port 1
 *  Match according to given ports (in_pbmp)
 */
int gre_learn_data_rebuild_example(int unit) 
{
  int result;
  int group_priority = 30;
  bcm_field_group_t group[2];

 
  result = gre_learn_data_rebuild_field_group_set(unit, group_priority, &group[0], &group[1]);
  if (BCM_E_NONE != result) {
      printf("Error in gre_learn_data_rebuild_field_group_set\n");
      return result;
  }

  return result;
}


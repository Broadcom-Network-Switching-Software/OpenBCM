
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * Rebuild the Learn-Data with the current Source-System-Port 
 * and current In-LIF 
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
 *  same_interface_filter_basic_mode - Should be 1 if the SOC property bcm886xx_logical_interface_bridge_filter_enable is 1.
 */
int learn_data_rebuild_field_group_set(/* in */ int unit,
                                       /* in */ bcm_pbmp_t in_pbmp,
                                       /* in */ int group_priority,
                                       /* out */ bcm_field_group_t *group0, 
                                       /* out */ bcm_field_group_t *group1,
                                       /* in */ int same_interface_filter_basic_mode) 
{
  int result;
  int auxRes;
  bcm_field_group_config_t grp[2];
  uint32 grp_ndx, grp_ndx2;
  bcm_field_data_qualifier_t data_qual[6]; /* the 4 data qualifiers */
  uint8 dq_data[2], dq_mask[2];
  uint32 dq_id, dq_ndx, dq_ndx2, dq_length[6] = {16, 2, 1, 
      16, 1, 4};
  bcm_field_qualify_t dq_qualifier[6] = {bcmFieldQualifySrcPort, bcmFieldQualifyConstantZero, bcmFieldQualifyConstantOne, 
      bcmFieldQualifyInVPort, bcmFieldQualifyConstantOne, bcmFieldQualifyConstantZero};
  bcm_field_action_t grp_action[2] = {bcmFieldActionLearnSrcPortNew, bcmFieldActionLearnInVPortNew};
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

  /* 
   * Set the preselector to be on the ports
   */
  BCM_PBMP_CLEAR(pbm);
  BCM_PBMP_ASSIGN(pbm, in_pbmp);
  for(i=0; i<256; i++) { BCM_PBMP_PORT_ADD(pbm_mask, i); }

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

  result = bcm_field_qualify_InPorts(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, pbm, pbm_mask);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_qualify_InPorts\n");
    auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
    return result;
  }

  if (same_interface_filter_basic_mode) {
    /* Select on disable same i/f filter (inlif profile lsb) = 0x1 */
    result = bcm_field_qualify_IngressVPortBridgeDisable(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x1, 0x1);
    if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_IngressVPortBridgeDisable\n");
      auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
      return result;
    }
  } else {
    /* Select on LIF-Profile[0] = 0x1 */
    result = bcm_field_qualify_InterfaceClassVPort(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x1, 0x1);
    if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_InterfaceClassVPort\n");
      auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
      return result;
    }
  }

  BCM_FIELD_PRESEL_INIT(psset);
  BCM_FIELD_PRESEL_ADD(psset, presel_id);

  /* 
   * 2 Field groups must be created, because in the HW the Learn-Data 
   * action is split into 2 actions, one of 16b (regular Direct 
   * Extraction DB), and one of 24b (large DE DB, particular 
   * treatment). 
   *  
   * The second Field group is a Direct extraction group done 
   * on an action with a size higher than 19b. 
   * Due to hardware limitations, these groups are supported 
   * only under very strict constraints: 
   * - the field extractions must be stricly equal 
   * to the qualifier, i.e. all the qualifier size must be 
   * extracted. In case only part of the qualifier is required 
   * for the extraction, use data qualifiers (like here) predefined- 
   * based. 
   * - the field extractions must be have a size under 16 bits 
   * - they must have no lost bits (except the first extraction), 
   * which means they must be nibble-aligned in the HW when the 
   * HW instruction is defined 
   * - the extraction filter (whether to do or not) can be at the 
   *  most according to the LSB in the action construction
   * - it is recommended to define these Field groups first, e.g. 
   * after init   
   */

  for (dq_ndx = 0; dq_ndx < 6; dq_ndx++) {
      bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]); 
      data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      data_qual[dq_ndx].offset = 0;  
      data_qual[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
      data_qual[dq_ndx].length = dq_length[dq_ndx]; 
      result = bcm_field_data_qualifier_create(unit, &data_qual[dq_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          auxRes = bcm_field_presel_destroy(unit, presel_id);
          for (dq_ndx2 = 0; dq_ndx2 < 6; dq_ndx2++) {
              auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
          }
          return result;
      }
  }


  for (grp_ndx = 0; grp_ndx < 2; grp_ndx++) {
      bcm_field_group_config_t_init(&grp[grp_ndx]);
      grp[grp_ndx].group = -1;
      /* Define the QSET */
      BCM_FIELD_QSET_INIT(grp[grp_ndx].qset);
      BCM_FIELD_QSET_ADD(grp[grp_ndx].qset, bcmFieldQualifyStageIngress);

      for (dq_ndx = (grp_ndx? 1: 0); dq_ndx < (grp_ndx? 6: 1); dq_ndx++) {
          /* Add the Data qualifier to the QSET */
          result = bcm_field_qset_data_qualifier_add(unit, &grp[grp_ndx].qset, data_qual[dq_ndx].qual_id);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qset_data_qualifier_add\n");
              auxRes = bcm_field_presel_destroy(unit, presel_id);
              for (dq_ndx2 = 0; dq_ndx2 < 6; dq_ndx2++) {
                  auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
              }
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
          auxRes = bcm_field_presel_destroy(unit, presel_id);
          for (grp_ndx2 = 0; grp_ndx2 < 2; grp_ndx2++) {
              auxRes = bcm_field_group_destroy(unit, grp[grp_ndx].group);
          }
          for (dq_ndx2 = 0; dq_ndx2 < 4; dq_ndx2++) {
              auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
          }
          return result;
      }

      /*
       *  Add a single entry to the Field group.
       */
      result = bcm_field_entry_create(unit, grp[grp_ndx].group, &ent[grp_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
          auxRes = bcm_field_presel_destroy(unit, presel_id);
          for (grp_ndx2 = 0; grp_ndx2 < 2; grp_ndx2++) {
              auxRes = bcm_field_entry_remove(unit, ent[grp_ndx]);
              auxRes = bcm_field_group_destroy(unit, grp[grp_ndx].group);
          }
          for (dq_ndx2 = 0; dq_ndx2 < 6; dq_ndx2++) {
              auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
          }
          return result;
      }


      bcm_field_extraction_action_t_init(&extact);
      extact.action = grp_action[grp_ndx];
      extact.bias = 0;
      dq_id = 0;
      for (dq_ndx = (grp_ndx? 1: 0); dq_ndx < (grp_ndx? 6: 1); dq_ndx++) {
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
          auxRes = bcm_field_presel_destroy(unit, presel_id);
          for (grp_ndx2 = 0; grp_ndx2 < 2; grp_ndx2++) {
              auxRes = bcm_field_entry_remove(unit, ent[grp_ndx]);
              auxRes = bcm_field_group_destroy(unit, grp[grp_ndx].group);
          }
          for (dq_ndx2 = 0; dq_ndx2 < 6; dq_ndx2++) {
              auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
          }
          return result;
      }

      result = bcm_field_group_install(unit, grp[grp_ndx].group);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
          auxRes = bcm_field_presel_destroy(unit, presel_id);
          for (grp_ndx2 = 0; grp_ndx2 < 2; grp_ndx2++) {
              auxRes = bcm_field_entry_remove(unit, ent[grp_ndx]);
              auxRes = bcm_field_group_destroy(unit, grp[grp_ndx].group);
          }
          for (dq_ndx2 = 0; dq_ndx2 < 6; dq_ndx2++) {
              auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
          }
          return result;
      }
  }


  /* 
   * Create Field group setting the In-LIF always to 0
   */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  if (same_interface_filter_basic_mode) {
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIngressVPortBridgeDisable);
  } else {
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassVPort);
  }

  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionIngressGportSet);

  result = bcm_field_group_create(unit, qset, group_priority + 2, &group2);
  if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_create\n");
          return result;
  }
  result = bcm_field_group_presel_set(unit, group2, &psset);
  if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_presel_set\n");
          return result;
  }
  result = bcm_field_group_action_set(unit, group2, aset);
  if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_action_set\n");
          return result;
  }
  result = bcm_field_entry_create(unit, group2, &ent_invport);
  if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create_id\n");
          return result;
  }
  if (same_interface_filter_basic_mode) {
    result = bcm_field_qualify_IngressVPortBridgeDisable(unit, ent_invport, 0x1, 0x1);
    if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_InterfaceClassVPort\n");
            return result;
    }

  } else {
  
    result = bcm_field_qualify_InterfaceClassVPort(unit, ent_invport, 0x1, 0x1);
    if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_InterfaceClassVPort\n");
            return result;
    }
  }

  BCM_GPORT_VLAN_PORT_ID_SET(vlan_port_id,0x0);
  result = bcm_field_action_add(unit, ent_invport, bcmFieldActionIngressGportSet, vlan_port_id, 0);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_action_add\n");
    return result;
  }
  result = bcm_field_group_install(unit, group2);
  if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_install\n");
          return result;
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
 *  
 *  same_interface_filter_basic_mode - Should be 1 if the SOC property bcm886xx_logical_interface_bridge_filter_enable is 1.
 */
int learn_data_rebuild_example(int unit, bcm_pbmp_t in_pbmp, int same_interface_filter_basic_mode) 
{
  int result;
  int group_priority = 20;
  bcm_field_group_t group[2];

 
  result = learn_data_rebuild_field_group_set(unit, in_pbmp, group_priority, &group[0], &group[1], same_interface_filter_basic_mode);
  if (BCM_E_NONE != result) {
      printf("Error in learn_data_rebuild_set\n");
      return result;
  }

  return result;
}

int learn_data_rebuild_set_teardown(/* in */ int unit,
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



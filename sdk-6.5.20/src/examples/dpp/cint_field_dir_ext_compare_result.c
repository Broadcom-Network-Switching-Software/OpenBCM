
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * This cint creates two 80 bit field groups for comparison 
 * before the second cycle. Both FGs are created in the 
 * second cycle, using the same key. The result of the 
 * comparison is written to the MSB bits of same FG's key, 
 * and can be used for direct extraction in the second cycle. 
 * Can be also combined with cascaded. 
 *  
 * Run:
 * cint cint_field_dir_ext_compare_result.c
 * cint
 * compare_field_group_setup(int unit, int is_cascaded); 
 *  
 * Clean: 
 * compare_field_group_destroy(int unit);  
 */

int g_is_cascaded;
bcm_field_group_config_t grp_tcam, grp_de, grp_cascaded;
bcm_field_entry_t ent_tcam, ent_de, ent_cascaded;


/* ***************************************************************** */
/* 
 * compare_field_group_set_a(): 
 *  
 * 1. Create the first field group for compare in the FP: 
 *    TCAM Database of 80 bits.    
 * 2. Set the QSET of to be user defined.    
 */
/* ***************************************************************** */
int compare_field_group_set_a(/* in */  int unit,
                              /* in */  int is_cascaded,
                              /* in */  bcm_field_group_t group,
                              /* in */  bcm_field_group_t group_cascaded,
                              /* in */  int group_priority,
                              /* in */  int group_priority_cascaded,
                              /* in */  int in_port) 
{
  int result;
  bcm_field_aset_t aset, aset_cascaded;
  bcm_field_data_qualifier_t data_qual;
  
  /* 
   * Define the first Field Group (LSB bits of key 7).
   */
  bcm_field_group_config_t_init(&grp_tcam);
  grp_tcam.group = group;
  grp_tcam.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | 
      BCM_FIELD_GROUP_CREATE_WITH_ID | 
      BCM_FIELD_GROUP_CREATE_IS_EQUAL;
  grp_tcam.mode = bcmFieldGroupModeAuto;
  grp_tcam.priority = group_priority;

  /* 
   * Define the QSET. 
   * No need to use IsEqual as qualifier for this field group.
   */
  BCM_FIELD_QSET_INIT(grp_tcam.qset);
  BCM_FIELD_QSET_ADD(grp_tcam.qset, bcmFieldQualifyStageIngress);
  
  if (is_cascaded) 
  {
      /* Set cascaded field size */
      result = bcm_field_control_set(unit,
                                     bcmFieldControlCascadedKeyWidth,
                                     16 /* bits in cascaded key */);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_control_set\n");
          return result;
      }

      /* Add cascaded qualifier to field group */
      BCM_FIELD_QSET_ADD(grp_tcam.qset, bcmFieldQualifyCascadedKeyValue);
  }
  else
  {
      /* If is_cascaded=TRUE, then this addition of
       * qualifer must be last.
       */
      bcm_field_data_qualifier_t_init(&data_qual); 
      data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES |
          BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      data_qual.offset = 20;  
      data_qual.qualifier = bcmFieldQualifyL2SrcValue; 
      data_qual.length = 16; 
      data_qual.stage = bcmFieldStageIngress;
      result = bcm_field_data_qualifier_create(unit, &data_qual);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }

      result = bcm_field_qset_data_qualifier_add(unit, &grp_tcam.qset, data_qual.qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }
  }

  /*
   *  Define the ASET - use counter 0.
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);

  /*  Create the Field group with type TCAM */
  result = bcm_field_group_config_create(unit, &grp_tcam);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create for group %d\n", grp_tcam.group);
      return result;
  }

  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp_tcam.group, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set for group %d\n", grp_tcam.group);
    return result;
  }

  if (is_cascaded) 
  {
      /* Create initial field group if cascaded */
      bcm_field_group_config_t_init(&grp_cascaded);
      grp_cascaded.group = group_cascaded;
      grp_cascaded.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE | 
          BCM_FIELD_GROUP_CREATE_WITH_ID;
      grp_cascaded.mode = bcmFieldGroupModeAuto;
      grp_cascaded.priority = group_priority_cascaded;

      BCM_FIELD_QSET_INIT(grp_cascaded.qset);
      BCM_FIELD_QSET_ADD(grp_cascaded.qset, bcmFieldQualifyStageIngress);
      BCM_FIELD_QSET_ADD(grp_cascaded.qset, bcmFieldQualifyInPort);

      result = bcm_field_group_config_create(unit, &grp_cascaded);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_config_create for group %d\n", grp_cascaded.group);
          return result;
      }
      
      BCM_FIELD_ASET_INIT(aset_cascaded);
      BCM_FIELD_ASET_ADD(aset_cascaded, bcmFieldActionCascadedKeyValueSet);

      result = bcm_field_group_action_set(unit, grp_cascaded.group, aset_cascaded);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_action_set for group %d\n", grp_cascaded.group);
          return result;
      }
  }

  result = bcm_field_entry_create(unit, grp_tcam.group, &ent_tcam);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  if (is_cascaded) 
  {
      result = bcm_field_qualify_CascadedKeyValue(unit, ent_tcam, 0x1, 0xffff);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_CascadedKeyValue\n");
          return result;
      }
  }

  result = bcm_field_entry_install(unit, ent_tcam);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_install\n");
      return result;
  }
  
  if (is_cascaded) 
  {
      result = bcm_field_entry_create(unit, grp_cascaded.group, &ent_cascaded);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
          return result;
      }

      result = bcm_field_qualify_InPort(unit, ent_cascaded, in_port, 0xffffffff);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_InPort\n");
          return result;
      }
      
      result = bcm_field_action_add(unit, ent_cascaded, bcmFieldActionCascadedKeyValueSet, 0x1, 0xffff);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_action_add\n");
          return result;
      }

      result = bcm_field_entry_install(unit, ent_cascaded);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_install\n");
          return result;
      }
  }

  return result;

} /* compare_field_group_set_a */


/* ***************************************************************** */
/* 
 * compare_field_group_set_b(): 
 *  
 * 1. Create Second field group for compare in the FP: 
 *    Direct Extraction Database of 80 bits. 
 * 2. Set the QSET of to be user defined + bcmFieldQualifyIsEqualValue.
 * 3. Set the action of the Direct Extraction database to 
 *    be according to result of the compare operation
 *    (located in the 5 MSB bits of Key 7). 
 */
/* ***************************************************************** */
int compare_field_group_set_b(/* in */  int unit,
                              /* in */  int is_cascaded,
                              /* in */  bcm_field_group_t group,
                              /* in */  int group_priority)
{
  int result;
  uint8 data_mask;
  bcm_field_aset_t aset;
  bcm_field_extraction_action_t extract;
  bcm_field_extraction_field_t is_equal[2];
  bcm_field_data_qualifier_t data_qual;
  
  /* 
   * Define the second Field Group (MSB bits of key 7).
   */
  bcm_field_group_config_t_init(&grp_de);
  grp_de.group = group;
  grp_de.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | 
      BCM_FIELD_GROUP_CREATE_WITH_ID | 
      BCM_FIELD_GROUP_CREATE_IS_EQUAL;
  grp_de.mode = bcmFieldGroupModeDirectExtraction;
  grp_de.priority = group_priority;
  
  /* 
   * Define the QSET - use InLIF as qualifier. 
   */
  BCM_FIELD_QSET_INIT(grp_de.qset);
  BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyIsEqualValue);
  
  if(is_cascaded)
  {
      BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyL2DestHit);
  }
  else
  {
      bcm_field_data_qualifier_t_init(&data_qual); 
      data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | 
          BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      data_qual.offset = 0;  
      data_qual.qualifier = bcmFieldQualifyInVPort; 
      data_qual.length = 16; 
      data_qual.stage = bcmFieldStageIngress;
      result = bcm_field_data_qualifier_create(unit, &data_qual);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }

      result = bcm_field_qset_data_qualifier_add(unit, &grp_de.qset, data_qual.qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }
  }
 
  /*
   *  Define the ASET.
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

  /*  Create the Field group with type Direct Extraction */
  result = bcm_field_group_config_create(unit, &grp_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create for group %d\n", grp_de.group);
      return result;
  }

  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp_de.group, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set for group %d\n", grp_de.group);
      return result;
  }

  /* 
   * Create the Direct Extraction entry:
   *  1. create the entry
   *  2. Construct the action:
   *     bit 0 = action valid 
   *     bits 3:0 = IsEqual
   *  3. Install entry (HW configuration)
   */
  result = bcm_field_entry_create(unit, grp_de.group, &ent_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  /* 
   * IsEqualValue Qualifier: 
   *    IsEqualValue qualifier has 4 bits, each indicated 
   *    match of 20 bits in the compare key. The LSB bit 
   *    corresponds to the LSB 20 compared bits, and so on. 
   * In this example:
   *    If cascaded is used, look at different bits in
   *    order to use compare result for Drop  
   */
  data_mask = 1;
  result = bcm_field_qualify_IsEqualValue(unit, ent_de, 0, data_mask);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_IsEqualValue\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  bcm_field_extraction_field_t_init(&is_equal[0]);
  bcm_field_extraction_field_t_init(&is_equal[1]);

  extract.action = bcmFieldActionDrop;
  extract.bias = 0;

  /* First extraction structure indicates action is valid */
  is_equal[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
  is_equal[0].bits  = 1;
  is_equal[0].value = 1;
  is_equal[0].qualifier = bcmFieldQualifyConstantOne;

  if (!is_device_or_above(unit,JERICHO_PLUS)) {
      /* second extraction structure indicates to use IsEqual qualifier */
      is_equal[1].flags = 0;
      is_equal[1].bits  = 4;
      is_equal[1].lsb   = 0;
      is_equal[1].qualifier = bcmFieldQualifyIsEqualValue;
  }

  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent_de,
                                                  extract,
                                                  is_device_or_above(unit,JERICHO_PLUS)? 1 : 2 /* count */,
                                                  &is_equal);

  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_direct_extraction_action_add\n");
      return result;
  }

  /* Write entry to HW */
  result = bcm_field_entry_install(unit, ent_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_install\n");
      return result;
  } 

  return result;

} /* compare_field_group_set_b */


/* ***************************************************************** */
/*
 *  Set an example configuration of this cint
 */
/* ***************************************************************** */
int compare_field_group_setup(int unit, int is_cascaded, int in_port) 
{
  int result;
  int group_priority_cascaded = BCM_FIELD_GROUP_PRIO_ANY,
      group_priority_a = BCM_FIELD_GROUP_PRIO_ANY,
      group_priority_b = BCM_FIELD_GROUP_PRIO_ANY;
  bcm_field_group_t 
      group_cascaded = 1,
      group_a = 2,
      group_b = 3;
  
  g_is_cascaded = is_cascaded;

  /* Create LSB field group */
  result = compare_field_group_set_a(
                unit, 
                is_cascaded,
                group_a, 
                group_cascaded, 
                group_priority_a, 
                group_priority_cascaded,
                in_port
            );
  if (BCM_E_NONE != result) {
      printf("Error in compare_field_group_set_a\n");
      return result;
  } 

  /* Create MSB field group */
  result = compare_field_group_set_b(
                unit, 
                is_cascaded,
                group_b,
                group_priority_b
           );
  if (BCM_E_NONE != result) {
      printf("Error in compare_field_group_set_b\n");
      return result;
  }

  return result;

} /* compare_field_group_example */

/* ***************************************************************** */
/*
 *  Destroy configuration of this cint
 */
/* ***************************************************************** */
int compare_field_group_destroy(int unit) 
{
  int result;

  if (g_is_cascaded) 
  {
      result = bcm_field_entry_destroy(unit, ent_cascaded);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_destroy entry %d\n", ent_cascaded);
          return result;
      }

      result = bcm_field_group_destroy(unit, grp_cascaded.group);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_destroy group %d\n", grp_cascaded.group);
          return result;
      }
  }

  result = bcm_field_entry_destroy(unit, ent_tcam);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_destroy entry %d\n", ent_tcam);
      return result;
  }

  result = bcm_field_group_destroy(unit, grp_tcam.group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_destroy group %d\n", grp_tcam.group);
      return result;
  }

  result = bcm_field_entry_destroy(unit, ent_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_destroy entry %d\n", ent_de);
      return result;
  }

  result = bcm_field_group_destroy(unit, grp_de.group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_destroy group %d\n", grp_de.group);
      return result;
  }

  return result;

} /* compare_field_group_destroy */




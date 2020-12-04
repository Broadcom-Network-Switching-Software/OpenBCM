
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * This cint creates a Field Processor Field Group of 
 * Direct Extraction type. 
 * The action is set to be a counter which returns the value 
 * of the InLIF.
 *  
 * run:
 * cint cint_field_dir_ext_counter_inlif.c
 * cint
 * inlif_field_group_example(int unit); 
 */

/* 
 * Create a Direct Extraction Database in the field processor. 
 * Set the qualfier for this database to be InLIF. 
 * Set the action to be a counter, with value set to the 
 * value of the InLIF from the incoming data.  
 */
int inlif_field_group_counter_processor_set(/* in */  int unit,
                                            /* in */  int group_priority,
                                            /* in */ bcm_field_group_t group) 
{
  int result;
  bcm_field_group_config_t grp;
  bcm_field_aset_t aset;
  bcm_field_entry_t ent;
  bcm_field_extraction_action_t extract;
  bcm_field_extraction_field_t ext_inlif[2];
  
  bcm_field_group_config_t_init(&grp);
  grp.group = group;

  /* 
   * Define the QSET - use InLIF as qualifier. 
   */
  BCM_FIELD_QSET_INIT(grp.qset);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyInVPort);

  /*
   *  Define the ASET - use counter 0.
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);

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
   *     bit 0 = action valid 
   *     bits 16:0 = InLIF
   *  3. Install entry (HW configuration)
   */
  result = bcm_field_entry_create(unit, grp.group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  bcm_field_extraction_field_t_init(&ext_inlif[0]);
  bcm_field_extraction_field_t_init(&ext_inlif[1]);

  extract.action = bcmFieldActionStat;
  extract.bias = 0;

  /* First extraction structure indicates action is valid */
  ext_inlif[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
  ext_inlif[0].bits  = 1;
  ext_inlif[0].value = 1;

  /* second extraction structure indicates to use InLif qualifier */
  ext_inlif[1].flags = 0;
  ext_inlif[1].bits  = 16;
  ext_inlif[1].lsb   = 0;
  ext_inlif[1].qualifier = bcmFieldQualifyInVPort;

  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  extract,
                                                  2 /* count */,
                                                  &ext_inlif);

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
 * If the statistic-interface is set in the billing mode, 
 * the Counter action has 22 bits instead of 16 bits. 
 * Thus, the large Direct extraction Field group must be used 
 * with all its constraints (see cint_field_learn_data_rebuild.c remarks) 
 */
int inlif_field_group_stat_if_set(/* in */  int unit,
                                  /* in */  int group_priority,
                                  /* in */ bcm_field_group_t group) 
{
  int result;
  bcm_field_group_config_t grp;
  bcm_field_aset_t aset;
  bcm_field_entry_t ent;
  bcm_field_extraction_action_t extract;
  bcm_field_extraction_field_t ext_inlif[3];
  bcm_field_data_qualifier_t data_qual[3]; /* the 3 data qualifiers */
  uint32 dq_ndx, dq_length[3] = {1, 16, 5};
  bcm_field_qualify_t dq_qualifier[3] = {bcmFieldQualifyConstantOne, bcmFieldQualifyInVPort, bcmFieldQualifyConstantZero};

  for (dq_ndx = 0; dq_ndx < 3; dq_ndx++) {
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

  bcm_field_group_config_t_init(&grp);
  grp.group = group;

  /* 
   * Define the QSET - use InLIF as qualifier. 
   */
  BCM_FIELD_QSET_INIT(grp.qset);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
  for (dq_ndx = 0; dq_ndx < 3; dq_ndx++) {
      /* Add the Data qualifier to the QSET */
      result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual[dq_ndx].qual_id);
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
   *     bit 0 = action valid 
   *     bits 16:0 = InLIF
   *  3. Install entry (HW configuration)
   */
  result = bcm_field_entry_create(unit, grp.group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  bcm_field_extraction_field_t_init(&ext_inlif[0]);
  bcm_field_extraction_field_t_init(&ext_inlif[1]);

  extract.action = bcmFieldActionStat;
  extract.bias = 0;

  for (dq_ndx = 0; dq_ndx < 3; dq_ndx++) {
      bcm_field_extraction_field_t_init(&(ext_inlif[dq_ndx]));
      ext_inlif[dq_ndx].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
      ext_inlif[dq_ndx].qualifier = data_qual[dq_ndx].qual_id;
      ext_inlif[dq_ndx].lsb = 0;
      ext_inlif[dq_ndx].bits = dq_length[dq_ndx];
  }

  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  extract,
                                                  3 /* count */,
                                                  &ext_inlif);

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
  
  return result;
}

/*
 *  Set an example configuration of this cint
 */
int inlif_field_group_example(int unit) 
{
  int result;
  int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
  bcm_field_group_t group = 1;
  int stat_if_billing_mode = 1;

  if (stat_if_billing_mode) {
      result = inlif_field_group_stat_if_set(unit, group_priority, group);
      if (BCM_E_NONE != result) {
          printf("Error in inlif_field_group_stat_if_set\n");
          return result;
      }
  }
  else {
      result = inlif_field_group_counter_processor_set(unit, group_priority, group);
      if (BCM_E_NONE != result) {
          printf("Error in inlif_field_group_counter_processor_set\n");
          return result;
      }
  }

  return result;
}


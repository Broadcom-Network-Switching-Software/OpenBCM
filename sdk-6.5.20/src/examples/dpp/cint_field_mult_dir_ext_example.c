
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This cint creates a Field Processor Field Group of 
 * Direct Extraction type. 
 * It demonstrate how to use BCM_FIELD_GROUP_CREATE_LARGE flag in order to create multiple extractions on the same half (80-bits) key.
 *  
 * run:
 * cint cint_field_mult_dir_ext_example.c
 * cint
 * field_mult_dir_ext_example(int unit); 
 * 
 * 1. Configure data qualifiers for all used qualifiers.
 *
 * 2. Construct the key with BCM_FIELD_GROUP_CREATE_LARGE flag set.
 *         key = { data_qualifier_n, ... data_qualifier_1, data_qualifier_0 }
 *
 *     First created data qualifier (with lower ID) will be present in LSB of the lookup key.
 *
 * 3. User can now directly extract consequent bits from the key as an action. 
 *     Hardware has two kinds of resources for action resolution - FES/FEM, 
 *     of which FEM is a more rare resource.
 *
 *     A FES will be allocated in the following case:
 *         If size of an action is large than the maximum size a FEM supported (typically 24 bits).
 *             In such a case, direct extraction should match the following conditions.
 *
 *         Or, the action matches the following condition:
 *             Conditional match
 *                 A direct extraction entry can have a qualifier, the action only takes effect when the qualifier matches.
 *                 FES only supports qualifying one bit (used as the valid bit). In which case, action can be defined as:
 *                     { action, valid(1b) } = key[m:n]
 *                         m = n + action_size
 *                 The valid bit is identified when:
 *                     A DE entry has a valid qualifier with valid bits all set to 0, or
 *                     A DE entry has a valid qualifier, with valid bit as the LSBit in key[m:n]
 *                         DE qualifier & mask = key[bit n]
 *    
 *             Always match
 *                 A DE action will always take effect if the DE entry does not have a qualifier.
 *                 In which case, action can be defined as:
 *                     { action } = key[m:n] (and the action will always valid)
 *                         m = n + action_size - 1
 *
 *         If the above condition could not match, a FEM will be allocated.
 *
 * 4. Multiple FES actions in one DE entry must be consequent and must extract consequent bits from the key.
 *        No gap in the key between the FES actions of the same entry.
 *
 * 5. Direct extraction now supports the allocation of a mix of FES and FEM. 
 *
 * Note:
 *     Data qualifiers must be nibble aligned, e.g., when creating the data qualifier, the data qualifier offset should be 0/4/8/...
 *     When using the flag BCM_FIELD_GROUP_CREATE_LARGE, the data qualifiers sizes are limited to 16b. 
 *         In case there are qualifiers that are bigger than 16b, it is user's responsibility to split them to 2 or more, and order them inside key using  the data qualifier ID.
 *     There are two ways to pad constant value bits for an action:
 *         a. Create data qualifier with bcmFieldQualifyConstantOne/bcmFieldQualifyConstantZero and use them in the key, in such case, FES can be used. Or
 *         b. Use constant value in direct extraction (BCM_FIELD_EXTRACTION_FLAG_CONSTANT), in such case, FEM will be allocated.
 *     Direct extraction creates 80bits key, in case 80bits is not enough, two direct extraction groups can share a 160bits key (each of them takes high/low 80bits)
 *     by setting the following SOC property:
 *         custom_feature_optimized_de_allocation=1
 *     Create direct extraction for large actions (>24bits) without setting the BCM_FIELD_GROUP_CREATE_LARGE flag is deprecated.
 */
int field_direct_extraction_lookup_set(/* in */  int unit,
		                                  /* in */  int group_priority,
		                                  /* in */ bcm_field_group_t group) 
{
  int result;
  bcm_field_group_config_t grp;
  bcm_field_aset_t aset;
  bcm_field_entry_t ent;
  bcm_field_extraction_action_t extract;
  bcm_field_extraction_field_t ext_inlif[3];
  bcm_field_data_qualifier_t data_qual[5]; /* the 5 data qualifiers */
  uint32 dq_ndx, dq_length[5] = {1, 16, 15,16,3};
  bcm_field_qualify_t dq_qualifier[5] = {bcmFieldQualifyConstantOne, bcmFieldQualifyInVPort, bcmFieldQualifyConstantZero,bcmFieldQualifyEtherType,bcmFieldQualifyOuterVlanPri};

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

  bcm_field_group_config_t_init(&grp);
  grp.group = group;

  /* 
   * Define the QSET - use InLIF as qualifier. 
   */
  BCM_FIELD_QSET_INIT(grp.qset);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
  for (dq_ndx = 0; dq_ndx < 5; dq_ndx++) {
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
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionForward);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionVSwitchNew);

  /*  Create the Field group with type Direct Extraction */
  grp.priority = group_priority;
  grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_LARGE;
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
  bcm_field_extraction_field_t_init(&ext_inlif[2]);

  extract.action = bcmFieldActionStat;
  extract.bias = 0;

  for (dq_ndx = 0; dq_ndx < 2; dq_ndx++) {
      bcm_field_extraction_field_t_init(&(ext_inlif[dq_ndx]));
      ext_inlif[dq_ndx].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
      ext_inlif[dq_ndx].qualifier = data_qual[dq_ndx].qual_id;
      ext_inlif[dq_ndx].lsb = 0;
      ext_inlif[dq_ndx].bits = dq_length[dq_ndx];
  }

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
      printf("Error in bcm_field_entry_install\n");
      return result;
  }
  
   result = bcm_field_entry_create(unit, grp.group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  bcm_field_extraction_field_t_init(&ext_inlif[0]);

  extract.action = bcmFieldActionForward;
  extract.bias = 0;


      ext_inlif[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
      ext_inlif[0].qualifier = data_qual[2].qual_id;
      ext_inlif[0].lsb = 0;
      ext_inlif[0].bits = dq_length[2];


  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  extract,
                                                  1 /* count */,
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


  result = bcm_field_entry_create(unit, grp.group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  bcm_field_extraction_field_t_init(&ext_inlif[0]);

  extract.action = bcmFieldActionVSwitchNew;
  extract.bias = 0;

  bcm_field_extraction_field_t_init(&(ext_inlif[0]));
  ext_inlif[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
  ext_inlif[0].qualifier = data_qual[3].qual_id;
  ext_inlif[0].lsb = 0;
  ext_inlif[0].bits = dq_length[3];

  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  extract,
                                                  1 /* count */,
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
  
    result = bcm_field_entry_create(unit, grp.group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  bcm_field_extraction_field_t_init(&ext_inlif[0]);

  extract.action = bcmFieldActionForward;
  extract.bias = 0;


      ext_inlif[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
      ext_inlif[0].qualifier = data_qual[4].qual_id;
      ext_inlif[0].lsb = 0;
      ext_inlif[0].bits = dq_length[4];


  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  extract,
                                                  1 /* count */,
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
int field_mult_dir_ext_example(int unit) 
{
  int result;
  int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
  bcm_field_group_t group = 1;
  
  /*configure Direct Extraction Lookup*/
  result = field_direct_extraction_lookup_set(unit, group_priority, group);
  if (BCM_E_NONE != result) {
      printf("Error in field_direct_extraction_lookup_set\n");
      return result;
  }

  return result;
}


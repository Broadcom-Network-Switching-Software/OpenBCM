
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



/*
 *  For TM packets (ITMH header), extract the Scheduler-Compensation value after the ITMH
 *  Indicate also how many bytes to remove
 *  
 *  Sequence:
 *  1. Create a TM preselector
 *  2. Create a Direct Extraction (also known as Direct Mapping) Field group for the
 *  TM packets. Define the location of the Scheduler-Compensation value (and of
 *  the Bytes-To-Remove field) and extract it
 *  
 *  Besides, the user can set a different header length to remove (bytes-to-remove field)
 *  from the end of ITMH. All the used ports must use a TM header type (ITMH expected).
 */


/*
 *  Set the Direct mapping Field group that maps the Scheduler-Compensation value
 */
int tm_field_group_set(/* in */ int unit,
                        /* in */ int group_priority,
                        /* in */ int is_compensation,
                        /* out */ bcm_field_group_t *group) 
{
  int result;
  int auxRes;
  bcm_field_group_config_t grp;
  bcm_field_aset_t aset;
  bcm_field_presel_t            presel = 0;
  int                           presel_flags = 0;
  bcm_field_presel_set_t        presel_set;
  bcm_field_data_qualifier_t    data_qual;


  if (NULL == group) {
    printf("Pointer to group ID must not be NULL\n");
    return BCM_E_PARAM;
  }
  /* 
   * Create a preselector according to Forwading-Type = TM
   */
  if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
      presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
      result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_presel_create_stage_id\n");
          auxRes = bcm_field_presel_destroy(unit, presel | presel_flags);
          return result;
      }
  } else {
      result = bcm_field_presel_create(unit, &presel);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_presel_create\n");
          auxRes = bcm_field_presel_destroy(unit, presel);
          return result;
      }
  }

  result = bcm_field_qualify_ForwardingType(unit, presel | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldForwardingTypeTrafficManagement);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_ForwardingType\n");
      auxRes = bcm_field_presel_destroy(unit, presel | presel_flags);
    return result;
  }

  /* Create the Preslector-set with a single preselector */
  BCM_FIELD_PRESEL_INIT(presel_set);
  BCM_FIELD_PRESEL_ADD(presel_set, presel);

  /*
   *  The Field group is done at the ingress stage
   *  The Field group qualifier is a Data Qualifier with:
   *  - Base Header = Post-Forwarding
   *  - Exact location of the Scheduler-Compensation-Value / Bytes-to-remove.
   *  Prefer defining it at a byte resolution
   *  
   *  Following format:
   *  PTCH-2 | ITMH | Bytes-To-Remove[7:0] | Scheduler-Compensation-Value 
   *  
   *  Encoding of the Bytes-To-Remove field:
   *  [Header-index (2 bits) | Local-Offset in bytes (6b)], where
   *  header-index encoding is: 0 - Start-of-Packet, 
   *  2 - Start-of-ITMH, 3 - End of ITMH
   *  For example, to remove here up to payload, use the value:
   *  [End-of-ITMH = 3 | Remove 4 Bytes] = 0xC4
   *  
   *  Note: if the bit after ITMH is bit 159, and 160 bits are
   *  copied to the Key, the entire field must be located in
   *  the following zones:
   *  [159-128], [143-112], [127-96], [111-80],
   *  [79-48], [63-32], [47-16], [31-0]  
   */
  bcm_field_data_qualifier_t_init(&data_qual);
  data_qual.offset_base = bcmFieldDataOffsetBaseNextForwardingHeader;
  if (is_compensation) {
      data_qual.offset = 1;
      data_qual.length = 1; /* The Scheduler-Compensation value is 1 Byte, so take 1 byte after ITMH */
  }
  else {
      data_qual.offset = 0;
      data_qual.length = 1; 
  }
  result = bcm_field_data_qualifier_create(unit, &data_qual);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_data_qualifier_create\n");
      auxRes = bcm_field_presel_destroy(unit, presel | presel_flags);
      auxRes = bcm_field_data_qualifier_destroy(unit, data_qual.qual_id);
      return result;
  }

  /* 
   * Create the TM Field Group
   */
  bcm_field_group_config_t_init(&grp);
  grp.group = -1;


  /* Define the QSET */
  BCM_FIELD_QSET_INIT(grp.qset);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
  result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qset_data_qualifier_add\n");
      auxRes = bcm_field_presel_destroy(unit, presel | presel_flags);
      auxRes = bcm_field_data_qualifier_destroy(unit, data_qual.qual_id);
      return result;
  }

  /*
   *  Set the field group's action
   */
  BCM_FIELD_ASET_INIT(aset);
  if (is_compensation) {
      BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassDestSet);
  }
  else {
      BCM_FIELD_ASET_ADD(aset, bcmFieldActionStartPacketStrip);
  }

  /*  Create the Field group */
  grp.priority = group_priority;
  grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
  grp.mode = bcmFieldGroupModeDirectExtraction;
  result = bcm_field_group_config_create(unit, &grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      auxRes = bcm_field_group_destroy(unit, grp.group);
      auxRes = bcm_field_presel_destroy(unit, presel | presel_flags);
      auxRes = bcm_field_data_qualifier_destroy(unit, data_qual.qual_id);
      return result;
  }

  /* Attach the Preselector-Set */
  result = bcm_field_group_presel_set(unit, grp.group, &presel_set);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_presel_set\n");
    auxRes = bcm_field_group_destroy(unit, grp.group);
    auxRes = bcm_field_presel_destroy(unit, presel | presel_flags);
    auxRes = bcm_field_data_qualifier_destroy(unit, data_qual.qual_id);
    return result;
  }

  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp.group, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set\n");
    auxRes = bcm_field_group_destroy(unit, grp.group);
    auxRes = bcm_field_presel_destroy(unit, presel | presel_flags);
    auxRes = bcm_field_data_qualifier_destroy(unit, data_qual.qual_id);
    return result;
  }

  /*
   *  Everything went well; return the group ID that we allocated earlier.
   */
  *group = grp.group; 

  return result;
}


/*
 *  Set the previous configurations for an example configuration
 */
int tm_scheduler_compensation_field_group_example(int unit) 
{
  int result;
  int group_priority = 1;
  bcm_field_group_t group;


 result = tm_field_group_set(unit, group_priority, TRUE, &group);
  if (BCM_E_NONE != result) {
      printf("Error in tm_scheduler_compensation_field_group_example\n");
      return result;
  }

  return result;
}

int tm_bytes_to_remove_field_group_example(int unit) 
{
  int result;
  int group_priority = 2;
  bcm_field_group_t group;


 result = tm_field_group_set(unit, group_priority, FALSE, &group);
  if (BCM_E_NONE != result) {
      printf("Error in tm_lag_lb_field_group_set\n");
      return result;
  }

  return result;
}

int tm_field_group_set_teardown(/* in */ int unit,
                        /* in */ bcm_field_group_t group) 
{
  int result;

  /* Destroy the Field group (and all its entries) */  
  result = bcm_field_group_destroy(unit, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_destroy\n");
      return result;
  }

  return result;
}



/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 *  For TM packets (ITMH header), extract the LAG Load-Balancing Key after the ITMH
 *  Indicate also how many bytes to remove
 *  
 *  Sequence:
 *  1. Create a TM preselector
 *  2. Create a Direct Extraction (also known as Direct Mapping) Field group for the
 *  TM packets. Define the location of the Load-Balancing Key (and of
 *  the Bytes-To-Remove field) and extract it
 *  
 *  Besides, the user can set a different header length to remove (bytes-to-remove field)
 *  from the end of ITMH. In this case, the post_headers_size_<port-id> indicates
 *  the number of bytes to remove. Besides, PMF-Extension-Headers destinated to specific headers
 *  (e.g. below the LAG-LB-Key Field group is only for Port 0) can be set with
 *  specific preselectors. All the used ports must use a TM header type (ITMH expected).
 */

/* Please note:
*  If  mode= programmable-ITMH mode, entries should be added for the group, based on direct extractions for the group Database. 
*   each action has it's own entry: one for lag_lb (with action= bcmFieldActionTrunkHashKeySet) and one for bytes to remove
*   action=bcmFieldActionStartPacketStrip) 
 */



/*
 *  Set the Direct mapping Field group that maps the LAG-LB-Key
 */
int tm_field_group_set(/* in */ int unit,
                        /* in */ int group_priority,
                        /* in */ int is_lag_lb,
                        /* in */ int is_for_port_0,
                        /* in */ int is_itmh_programmable_mode,
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
  bcm_field_entry_t ent;
  bcm_field_extraction_field_t ext_field[2];
  bcm_field_extraction_action_t ext_action;



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
  if (is_for_port_0) {
      result = bcm_field_qualify_InPort(unit, presel | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0, (~0));
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_ForwardingType\n");
          auxRes = bcm_field_presel_destroy(unit, presel | presel_flags);
        return result;
      }
  }

  /* Create the Preslector-set with a single preselector */
  BCM_FIELD_PRESEL_INIT(presel_set);
  BCM_FIELD_PRESEL_ADD(presel_set, presel);

  /*
   *  The Field group is done at the ingress stage
   *  The Field group qualifier is a Data Qualifier with:
   *  - Base Header = Post-Forwarding
   *  - Exact location of the LAG-LB-Key / Bytes-to-remove.
   *  Prefer defining it at a byte resolution
   *  
   *  Following format:
   *  ITMH | 4'b0 | LAG-LB-Key[19:0] | Bytes-To-Remove[7:0] | Payload
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
  if (is_lag_lb) {
      data_qual.offset = 0;
      data_qual.length = 3; /* The LAG-LB-Key is 20 bits, so take 3 bytes after ITMH */
  }
  else {
      data_qual.offset = 3;
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
   *  This Field Group can change the DSCP
   */
  BCM_FIELD_ASET_INIT(aset);
  if (is_lag_lb) {
      BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrunkHashKeySet);
  }
  else {
      BCM_FIELD_ASET_ADD(aset, bcmFieldActionStartPacketStrip);
  }

  /*  Create the Field group */
  grp.priority = group_priority;
  grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE  | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;;
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

  if (is_itmh_programmable_mode) {

      result = bcm_field_entry_create(unit, grp.group, &ent);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create \n");
          return result;
      }

      if (is_lag_lb) {

          bcm_field_extraction_field_t_init(&ext_field);
          ext_field[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
          ext_field[0].qualifier = data_qual.qual_id;
          ext_field[0].lsb = 0;
          ext_field[0].bits = 20; 

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionTrunkHashKeySet;
          ext_action.bias = 0;

          

      } else {


          bcm_field_extraction_field_t_init(&ext_field);
          ext_field[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
          ext_field[0].qualifier = data_qual.qual_id;
          ext_field[0].lsb = 0;
          ext_field[0].bits = 6; 

          ext_field[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
          ext_field[1].bits  = 2;
          ext_field[1].value = 0x1; /*bcmFieldStartToL2Strip*/

          bcm_field_extraction_action_t_init(&ext_action);
          ext_action.action = bcmFieldActionStartPacketStrip;
          ext_action.bias = 0;

      }
      result = bcm_field_direct_extraction_action_add(unit,
                                                      ent,
                                                      ext_action,
                                                      (is_lag_lb) ? 1 : 2 /* count */,
                                                      &ext_field);

      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_direct_extraction_action_add entry %d\n", ent);
          return result;
      }

      /* Write entry to HW */
      result = bcm_field_entry_install(unit, ent);
      
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_install entry %d\n", ent);
          return result;
      }
      printf("entry %d install success\n", ent);

  }


  /*
   *  Everything went well; return the group ID that we allocated earlier.
   */
  *group = grp.group; 

  return result;
}



/*
 *  Set the previous configurations for an example configuration
 *  If match by a Field entry, snoop the packet to port 1
 */
int tm_lag_lb_field_group_example(int unit, int is_itmh_programmable_mode) 
{
  int result;
  int group_priority = 1;
  bcm_field_group_t group;


 result = tm_field_group_set(unit, group_priority, TRUE, TRUE, is_itmh_programmable_mode, &group);
  if (BCM_E_NONE != result) {
      printf("Error in tm_lag_lb_field_group_set\n");
      return result;
  }

  return result;
}

int tm_bytes_to_remove_field_group_example(int unit, int is_itmh_programmable_mode) 
{
  int result;
  int group_priority = 2;
  bcm_field_group_t group;


 result = tm_field_group_set(unit, group_priority, FALSE, FALSE, is_itmh_programmable_mode, &group);
  if (BCM_E_NONE != result) {
      printf("Error in tm_lag_lb_field_group_set\n");
      return result;
  }

  return result;
}

int tm_lag_lb_field_group_set_teardown(/* in */ int unit,
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



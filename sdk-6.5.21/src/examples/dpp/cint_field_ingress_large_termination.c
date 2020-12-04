
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * According to the Forwarding-Header, if the Forwarding-Header-Offset 
 * is over 32 Bytes, remove up to 28 Bytes after Ethernet header. 
 * This rule is a walk-around due to the limited capacities of 
 * Egress stage to terminate large termination headers. 
 * Mirroring and snooping on these packets are impacted since 
 * the Ethernet header (and the following headers) are removed at ingress.
 *  
 *  Sequence:
 *  1. Define a Field Group matching on different Forwarding-Headers and
 *  Forwarding-Header-Offset, and removing start-of-packet 
 *  2. Create the Field group and add entries.
 */
static int Large_termination_nof_base_headers = 6 * 2; /* 5 base-headers, 32B and 64B match */
int large_termination_field_group_set(/* in */ int unit,
                                      /* in */ int group_priority,
                                      /* out */ bcm_field_group_t *group) 
{
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  bcm_field_entry_t ent;
  int entry_iter;
  int nof_entries; 
  int offset_value; 
  bcm_field_data_offset_base_t base_header[Large_termination_nof_base_headers] = 
      {bcmFieldDataOffsetBasePacketStart, bcmFieldDataOffsetBaseL2Header,
      bcmFieldDataOffsetBaseFirstHeader, bcmFieldDataOffsetBaseSecondHeader,
      bcmFieldDataOffsetBaseThirdHeader, bcmFieldDataOffsetBaseFourthHeader};

  if (NULL == group) {
    printf("Pointer to group ID must not be NULL\n");
    return BCM_E_PARAM;
  }

  /*
   *  The Field group is done at the ingress stage
   *  The Field group qualifier is the Forwarding-Header-Offset
   *  (in bytes)
   */


  /* Define the QSET */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingHeaderOffset);

  /*
   *  This Field Group can change the Packet-Strip (i.e., how many bytes to
   *  remove from the start-of-packet) value.
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionStartPacketStrip);

  /*  Create the Field group */
  result = bcm_field_group_create(unit, qset, group_priority, &grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      auxRes = bcm_field_group_destroy(unit, grp);
      return result;
  }
  
  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set\n");
    auxRes = bcm_field_group_destroy(unit, grp);
    return result;
  }


  /*
   *  Add multiple entries to the Field group.
   */
  for (entry_iter = 0; entry_iter < Large_termination_nof_base_headers; entry_iter++) {
      result = bcm_field_entry_create(unit, grp, &ent);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }

      /* 
       * Match for each possible Forwarding base-header, if its local-offset is over 32
       */
      offset_value = 32 * (1 + (entry_iter % 2)); /* 32B or 64B */
      result = bcm_field_qualify_ForwardingHeaderOffset(unit, ent, base_header[entry_iter / 2], offset_value, offset_value);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_ForwardingType\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }

      /*
       * Build the action value: remove 28B (the Forwarding base-header is located afterwards) 
       * The ingress can remove up to 31B, including potential PTCH headers. 
       */
      result = bcm_field_action_add(unit, ent, bcmFieldActionStartPacketStrip, bcmFieldStartToL2Strip, 28);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_action_add\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }
  }

  /*
   *  Commit the entire group to hardware.  We could do individual entries,
   *  but since we just created the group here and want the entry/entries all
   *  in hardware at this point, it is quicker to commit the whole group.
   */
  result = bcm_field_group_install(unit, grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
    auxRes = bcm_field_group_destroy(unit, grp);
    return result;
  }
  
  /*
   *  Everything went well; return the group ID that we allocated earlier.
   */
  *group = grp; 

  return result;
}

/*
 *  Set the previous configurations for an example configuration
 *  If match by a Field entry, snoop the packet to port 1
 */
int large_termination_example(int unit) 
{
  int result;
  int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
  bcm_field_group_t group;


 result = large_termination_field_group_set(unit, group_priority, &group);
  if (BCM_E_NONE != result) {
      printf("Error in large_termination_set\n");
      return result;
  }

  return result;
}

int large_termination_set_teardown(/* in */ int unit,
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




/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 *  Mirror ARP Reply packets to local Port 0 (in general CPU)
 *  Sequence:
 *  1. Define the mirror profile to send to port 0
 *  2. Define a QSET including the EtherType, Destination-MAC-Address,
 *  ARP-Opcode (as a data qualifier) and STP-Status
 *  3. Create a Field group and add entries
 */


/*
 *  Set a Mirror profile to send to some destination
 *  local port. (Alternatively, a system port can be the
 *  destination of the mirror port).
 *  We receive as an output the mirror gport. This gport
 *  will be the parameter of Inbound mirror action  
 */
int arp_mirror_set(/* in */ int unit,
                   /* in */ int dest_local_port,
                   /* out */ bcm_gport_t *mirror_gport_id) 
{
  int result;
  int auxRes;
  bcm_mirror_destination_t mirror_dest;


  if (NULL == mirror_gport_id) {
    printf("Pointer to Mirror destination must not be NULL\n");
    return BCM_E_PARAM;
  }

  bcm_mirror_destination_t_init(&mirror_dest);

  /* Set the Mirror destination to go to the Destination local port*/
  BCM_GPORT_LOCAL_SET(mirror_dest.gport, dest_local_port);

  result = bcm_mirror_destination_create(unit, &mirror_dest);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_mirror_destination_create\n");
    auxRes = bcm_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id);
    return result;
  }

  *mirror_gport_id = mirror_dest.mirror_dest_id;

  return result;
}


/*
 *  Set the Field group that points to the Mirror
 *  for the key defined above
 */
int arp_field_group_set(/* in */ int unit,
                        /* in */ int group_priority,
                        /* in */ int nof_entries,
                        /* in */ bcm_gport_t mirror_gport_id, 
                        /* out */ bcm_field_group_t *group) 
{
  int result;
  int entry_iter;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  bcm_field_entry_t ent;
  bcm_mac_t macData;
  bcm_mac_t macMask;
  bcm_field_data_qualifier_t dq_arp_opcode;
  uint8 dqArpCodeData[2];
  uint8 dqArpCodeMask[2];


  if (NULL == group) {
    printf("Pointer to group ID must not be NULL\n");
    return BCM_E_PARAM;
  }

  /*
   *  Build qualifier set for the group we want to create.  Entries must exist
   *  within groups, and groups define what kind of things the entries can
   *  match, by their inclusion in the group's qualitier set.
   * 
   *  The Field group is done at the ingress stage
   *  The Field group qualifiers are EtherType (to match only ARP packets),
   *  Destination-MAC-Address, STP-Status and ARP-Opcode
   * 
   *  The ARP-Opcode is not part of the regular qualifiers. A data qualfier
   *  (User-Defined Field) is defined to extract the correct field from the
   *  header
   * 
   *  Note that only qualifiers specified in the QSET can be used by any of the
   *  entries in the group.  See the output from bcm_field_show(unit,"") for a
   *  list of the supported qualifiers per stage.
   */

  /* 
   * Define the Data qualifier: 
   * Take the 6th and 7th byte of the ARP header 
   * We assume that the ARP header is just after the L2 header 
   * To ensure this, entries will be match with EtherType = 0x0806 
   */
  bcm_field_data_qualifier_t_init(&dq_arp_opcode); /* ARP Opcode */
  dq_arp_opcode.offset_base = bcmFieldDataOffsetBaseFirstHeader; /* Consider the ARP header following Eth */
  dq_arp_opcode.offset = 6; /* Start at the 6th bytes from the start of ARP Header */
  dq_arp_opcode.length = 2; /* 2 bytes */
  result = bcm_field_data_qualifier_create(unit, &dq_arp_opcode);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_data_qualifier_create\n");
    print bcm_field_show(unit,"");
    auxRes = bcm_field_data_qualifier_destroy(unit, dq_arp_opcode.qual_id);
    return result;
  }

  /* Define the QSET */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIngressStpState);

  /* Add the Data qualifier to the QSET */
  result = bcm_field_qset_data_qualifier_add(unit, &qset, dq_arp_opcode.qual_id);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qset_data_qualifier_add\n");
    print bcm_field_show(unit,"");
    auxRes = bcm_field_data_qualifier_destroy(unit, dq_arp_opcode.qual_id);
    return result;
  }

  /*
   *  Build the action set for this Field group.  Entries can only do
   *  something that is included on this list.
   * 
   *  This Field Group can perform Inbound Mirroring 
   *
   *  See the output from bcm_field_show(unit,"") for a list of the supported
   *  actions per stage.
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionMirrorIngress);

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
   * 
   *  Match the packets on the EtherType = 0x0806 (ARP packets),
   *  STP-Status Forward, and different MAC-DAS and ARP.Opcode
   */
  for (entry_iter = 0; entry_iter < nof_entries; entry_iter++) {
      result = bcm_field_entry_create(unit, grp, &ent);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }

      /* Use a MAC-DA = XX:XX:XX:00:00:Entry-ID */
      macData[0] = 0x00;
      macData[1] = 0x00;
      macData[2] = 0x00;
      macData[3] = 0x00;
      macData[4] = 0x00;
      macData[5] = 0x00 + entry_iter;
      macMask[0] = 0x00;
      macMask[1] = 0x00;
      macMask[2] = 0x00;
      macMask[3] = 0xFF;
      macMask[4] = 0xFF;
      macMask[5] = 0xFF;
      result = bcm_field_qualify_DstMac(unit, ent, macData, macMask);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_DstMac\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }

      /* ARP EtherType */
      result = bcm_field_qualify_EtherType(unit, ent, 0x0806, 0xFFFF);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_EtherType\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }

      /* ARP.Opcode = ARP-Reply (0x2) */
      dqArpCodeData[0] = 0x0;
      dqArpCodeData[1] = 0x2;
      dqArpCodeMask[0] = 0xFF;
      dqArpCodeMask[1] = 0xFF;
      result = bcm_field_qualify_data(unit,
                                      ent,
                                      dq_arp_opcode.qual_id,
                                      &(dqArpCodeData[0]),
                                      &(dqArpCodeMask[0]),
                                      2 /* length here always in bytes */);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_data\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }

      /*
       *  A matched frame is mirrored to the defined gport.
       */
      result = bcm_field_action_add(unit, ent, bcmFieldActionMirrorIngress, 0, mirror_gport_id);
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
 *  If match by a Field entry, mirror the packet to local port 0
 */
int arp_example(int unit) 
{
  int result;
  int dest_local_port = 1;
  bcm_gport_t mirror_gport_id;
  int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
  int nof_entries = 2;
  bcm_field_group_t arp_group;


  /* Set the mirror destination */
  result = arp_mirror_set(unit, dest_local_port, &mirror_gport_id); 
  if (BCM_E_NONE != result) {
      printf("Error in arp_mirror_set\n");
      return result;
  }


  result = arp_field_group_set(unit, group_priority, nof_entries, mirror_gport_id, &arp_group);
  if (BCM_E_NONE != result) {
      printf("Error in arp_field_group_set\n");
      return result;
  }

  return result;
}

int arp_mirror_teardown(/* in */ int unit,
                        /* in */ bcm_gport_t mirror_gport_id,
                        /* in */ bcm_field_group_t group) 
{
  int result;
  int auxRes;

  /* Destroy the Field group (and all its entries) */  
  result = bcm_field_group_destroy(unit, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_destroy\n");
      auxRes = bcm_mirror_destination_destroy(unit, mirror_gport_id);
      return result;
  }

  /* Destroy the mirror destination */
  result = bcm_mirror_destination_destroy(unit, mirror_gport_id);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_mirror_destination_destroy\n");
    return result;
  }

  return result;
}


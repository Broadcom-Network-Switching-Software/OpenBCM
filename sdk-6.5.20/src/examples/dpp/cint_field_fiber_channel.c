
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * Multiple examples of Field Processor configurations 
 * related to the Fiber Channel application 
 */

/* 
 * FC Transit: 
 * for bridged Fiber Channel packets, add zones according to 
 * MAC-SA, MAC-DA, and VFT field F_ID 
 */
int fc_transit_setup(/* in */ int unit,
                    /* in */ int group_priority,
                    /* in */ bcm_field_group_t group) {
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  bcm_field_entry_t ent;
  bcm_field_data_qualifier_t vft[2]; /* 0 for rctl, 1 for f_id */
  int dq_ndx, entry_ndx;
  bcm_mac_t macData;
  bcm_mac_t macMask;
  uint8 dqMask[2], dqData[2];

  /* 
   * Define the data qualifiers
   */
  for (dq_ndx = 0; dq_ndx < 2; dq_ndx++) {
      bcm_field_data_qualifier_t_init(&vft[dq_ndx]); 
      vft[dq_ndx].flags = 0; 
      vft[dq_ndx].offset_base = bcmFieldDataOffsetBaseFirstHeader; 
      vft[dq_ndx].offset = (dq_ndx == 0)? 0: 2;  /* Local offset = 0/2 Bytes */
      vft[dq_ndx].length = (dq_ndx == 0)? 1: 2; /* 1/2 Bytes */
      result = bcm_field_data_qualifier_create(unit, &vft[dq_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }
  }

  /* 
   * Build the Qualifier-Set 
   */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingType);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcMac);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpType);
  for (dq_ndx = 0; dq_ndx < 2; dq_ndx++) {
      /* Add the Data qualifier to the QSET */
      result = bcm_field_qset_data_qualifier_add(unit, qset, vft[dq_ndx].qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }
  }

  /*
   *  Build action set for the group we want to create. 
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

  /*
   *  Create the group and set is actions.  
   */
  grp = group;
  result = bcm_field_group_create_id(unit, qset, group_priority, grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create_id\n");
      return result;
  }
  result = bcm_field_group_action_set(unit, grp, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set\n");
      return result;
  }

  /*
   *  Add a default entry to the group dropping unknown bridged FCoE packets
   *  if the packet is encapsulated, or not
   */
  for (entry_ndx = 0; entry_ndx < 3; entry_ndx++) {
      result = bcm_field_entry_create(unit, grp, &ent);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
          return result;
      }

      result = bcm_field_qualify_IpType(unit, ent, bcmFieldIpTypeFCoE);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_HeaderFormat\n");
          return result;
      }

      /* Bridged packet */
      result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeL2);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_ForwardingHeaderOffset\n");
          return result;
      }

      /* See if packet format is Ethernet | VFT | Standard */
      dqData[0] = (entry_ndx == 0)? 0x50 /* VFT */: ((entry_ndx == 1)? 0x51 /* Standard*/: 0x52 /* Encapsulated */);
      dqMask[0] = 0xFF;
      result = bcm_field_qualify_data(unit,
                                    ent,
                                    vft[0].qual_id,
                                    &(dqData[0]),
                                    &(dqMask[0]),
                                    1 /* len here always in bytes */);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_data\n");
          return result;
      }

      if (entry_ndx == 0 /* VFT */) {
          /* Match the F_ID field 12:1 for value 0 */
          dqData[0] = 0x0;
          dqMask[0] = 0xFE;
          dqData[1] = 0x0;
          dqMask[1] = 0x0F;
          result = bcm_field_qualify_data(unit,
                                        ent,
                                        vft[1].qual_id,
                                        &(dqData[0]),
                                        &(dqMask[0]),
                                        2 /* len here always in bytes */);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qualify_data\n");
              return result;
          }
      }

      /* Drop the packet */
      result = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_action_add\n");
          return result;
      }
  }

  /*
   *  Commit the entire group to hardware.  
   */
  result = bcm_field_group_install(unit, grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
      return result;
  }
  
  return result;
}

/* 
 * FCF Zoning: 
 * for routed Fiber Channel packets, change the Destination 
 * according to S_ID and D_ID
 */
int fcf_zoning_setup(/* in */ int unit,
                    /* in */ int group_priority,
                    /* in */ int destination_gport,
                    /* in */ bcm_field_group_t group) {
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  bcm_field_entry_t ent;
  bcm_field_data_qualifier_t vft[6]; /* 0 for rctl, 1 for f_id, 2 - S_ID if no VFT, 3 - S_ID if VFT, 4 - D_ID if no VFT, 5 - D_ID if VFT */
  uint32 dq_length[6] = {1, 2, 3, 3, 3, 3};
  uint32 dq_offset[6] = {0, 2, 5, (5+8), 1, (1+8)};
  int dq_ndx, dq_id, entry_ndx;
  bcm_mac_t macData;
  bcm_mac_t macMask;
  uint8 dqMask[3], dqData[3];

  /* 
   * Define the data qualifiers
   */
  for (dq_ndx = 0; dq_ndx < 6; dq_ndx++) {
      bcm_field_data_qualifier_t_init(&vft[dq_ndx]); 
      vft[dq_ndx].flags = 0; 
      vft[dq_ndx].offset_base = bcmFieldDataOffsetBaseFirstHeader; 
      vft[dq_ndx].offset = dq_offset[dq_ndx]; /* Local offset in Byte */
      vft[dq_ndx].length = dq_length[dq_ndx]; /* length in Byte */
      result = bcm_field_data_qualifier_create(unit, &vft[dq_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }
  }

  /* 
   * Build the Qualifier-Set 
   */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingType);
  for (dq_ndx = 0; dq_ndx < 6; dq_ndx++) {
      /* Add the Data qualifier to the QSET */
      result = bcm_field_qset_data_qualifier_add(unit, qset, vft[dq_ndx].qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }
  }

  /*
   *  Build action set for the group we want to create. 
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);

  /*
   *  Create the group and set is actions.  
   */
  grp = group;
  result = bcm_field_group_create_id(unit, qset, group_priority, grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create_id\n");
      return result;
  }
  result = bcm_field_group_action_set(unit, grp, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set\n");
      return result;
  }

  /*
   *  Add a default entry to the group changing the destination for routed
   *  FCoE packets if the packet is encapsulated, or not
   */
  for (entry_ndx = 0; entry_ndx < 3; entry_ndx++) {
      result = bcm_field_entry_create(unit, grp, &ent);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
          return result;
      }

      /* Routed packet */
      result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeFCoE);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_HeaderFormat\n");
          return result;
      }

      /* See if packet format is Ethernet | VFT | Standard */
      dqData[0] = (entry_ndx == 0)? 0x50 /* VFT */: ((entry_ndx == 1)? 0x51 /* Standard*/: 0x52 /* Encapsulated */);
      dqMask[0] = 0xFF;
      result = bcm_field_qualify_data(unit,
                                    ent,
                                    vft[0].qual_id,
                                    &(dqData[0]),
                                    &(dqMask[0]),
                                    1 /* len here always in bytes */);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_data\n");
          return result;
      }

      if (entry_ndx == 0 /* VFT */) {
          /* Match the F_ID field for value 0 */
          dqData[0] = 0x0;
          dqMask[0] = 0xFE;
          dqData[1] = 0x0;
          dqMask[1] = 0x0F;
          result = bcm_field_qualify_data(unit,
                                        ent,
                                        vft[1].qual_id,
                                        &(dqData[0]),
                                        &(dqMask[0]),
                                        2 /* len here always in bytes */);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qualify_data\n");
              return result;
          }

      }

      /* Match the S_ID and D_ID fields for value 0 */
      for (dq_ndx = 0; dq_ndx < 2; dq_ndx++) {
          dqData[0] = 0x0;
          dqMask[0] = 0xFF;
          dqData[1] = 0x0;
          dqMask[1] = 0xFF;
          dqData[2] = 0x0;
          dqMask[2] = 0xFF;
          dq_id = (entry_ndx == 0)? (3 + (2 * dq_ndx) /*3/5*/): (2 + (2 * dq_ndx) /*2/4*/);
          result = bcm_field_qualify_data(unit,
                                          ent,
                                          vft[dq_id].qual_id,
                                          &(dqData[0]),
                                          &(dqMask[0]),
                                          3 /* len here always in bytes */);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qualify_data\n");
              return result;
          }
      }

      /* Drop the packet */
      result = bcm_field_action_add(unit, ent, bcmFieldActionRedirect, destination_gport, 0);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_action_add\n");
          return result;
      }
  }

  /*
   *  Commit the entire group to hardware.  
   */
  result = bcm_field_group_install(unit, grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
      return result;
  }
  
  return result;
}


/* 
 * FCF Walk-around 
 */
int fcf_setup(/* in */ int unit,
                    /* in */ int group_priority,
                    /* in */ int gport_trap,
                    /* in */ bcm_field_group_t group) {
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  bcm_field_entry_t ent;

  /* 
   * Build the Qualifier-Set 
   */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingType);

  /*
   *  Build action set for the group we want to create. 
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);

  /*
   *  Create the group and set is actions.  
   */
  grp = group;
  result = bcm_field_group_create_id(unit, qset, group_priority, grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create_id\n");
      return result;
  }
  result = bcm_field_group_action_set(unit, grp, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set\n");
      return result;
  }

  /*
   *  Add an entry to the group trapping routed FCoE packets
   */
  result = bcm_field_entry_create(unit, grp, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  /* Routed packet */
  result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeFCoE);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_HeaderFormat\n");
      return result;
  }

  /* Trap the packet */
  result = bcm_field_action_add(unit, ent, bcmFieldActionTrap, gport_trap, 0);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_action_add\n");
      return result;
  }

  /*
   *  Commit the entire group to hardware.  
   */
  result = bcm_field_group_install(unit, grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
      return result;
  }
  
  return result;
}



/* 
 * LEM zoning - enable zoning.
 * this function starts a pmf rule that drops all the packets that didn't found in the zoning. 
 *  
 * zoning lookup failed-> drop packet. 
 */
int lem_zoning_setup(/* in */ int unit,
                        /* in */ int group_priority,
                        /* in */ bcm_field_group_t group) {
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  bcm_field_entry_t ent;

  /* 
   * Build the Qualifier-Set 
   */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingType);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2SrcHit);/* zoning lookup*/

  /*
   *  Build action set for the group we want to create. 
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

  /*
   *  Create the group and set is actions.  
   */
  grp = group;
  result = bcm_field_group_create_id(unit, qset, group_priority, grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create_id\n");
      return result;
  }
  result = bcm_field_group_action_set(unit, grp, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set\n");
      return result;
  }

  /*
   *  Add an entry to the group trapping routed FCoE packets
   */
  result = bcm_field_entry_create(unit, grp, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  /* Routed packet */
  result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeFCoE);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_HeaderFormat\n");
      return result;
  }

  /* zoning lookup found packet */
  result = bcm_field_qualify_L2SrcHit(unit, ent, 0x0, 0x1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_L3SrcHostHit\n");
      return result;
  }

  /* Trap the packet */
  result = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_action_add\n");
      return result;
  }

  /*
   *  Commit the entire group to hardware.  
   */
  result = bcm_field_group_install(unit, grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
      return result;
  }
  
  return result;
}


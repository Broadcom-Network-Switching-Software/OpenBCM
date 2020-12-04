
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * Example of Field Processor configurations 
 * related to the IP Multicast Bidirectional
 */
int ipmc_bidir_setup(/* in */ int unit,
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
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMplsTerminated);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTunnelTerminated);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2SrcHit);

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

  /* IP MC packet */
  result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeIp4Mcast);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_ForwardingType\n");
      return result;
  }

  result = bcm_field_qualify_L2SrcHit(unit, ent, 0x0, 0x1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_L2SrcHit\n");
      return result;
  }

  result = bcm_field_qualify_TunnelTerminated(unit, ent, 0x1, 0x1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_TunnelTerminated\n");
      return result;
  }

  result = bcm_field_qualify_MplsTerminated(unit, ent, 0x0, 0x1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_MplsTerminated\n");
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


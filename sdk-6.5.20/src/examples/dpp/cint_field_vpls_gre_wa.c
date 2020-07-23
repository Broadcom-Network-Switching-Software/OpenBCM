
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


/* 
 * VPLS over GRE application field
 */
int cint_field_vpls_gre_wa_main(/* in */ int unit,
                                 /* in */ int group_priority,
                                 /* in */ bcm_field_group_t group) {
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  bcm_field_entry_t ent;
  int dq_ndx, entry_ndx;
  bcm_mac_t macData;
  bcm_mac_t macMask;
  uint8 dqMask[2], dqData[2];

  /* 
   * Build the Qualifier-Set 
   */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingType);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTunnelType);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocolCommon);

  /*
   *  Build action set for the group we want to create. 
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionForwardingTypeNew);

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
   *  Add an entry to the group 
   */
  result = bcm_field_entry_create(unit, grp, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  /* Bridged packets */
  result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeL2);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_ForwardingType\n");
      return result;
  }

  /* MPLS IP-NextProtocol */
  result = bcm_field_qualify_IpProtocolCommon(unit, ent, bcmFieldIpProtocolCommonMplsInIp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_IpProtocolCommon\n");
      return result;
  }

  /* IP over Eth termination */
  result = bcm_field_qualify_TunnelType(unit, ent, bcmFieldTunnelTypeIp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_TunnelType\n");
      return result;
  }

  /* Set the ForwardingType to be MPLS */
  result = bcm_field_action_add(unit, ent, bcmFieldActionForwardingTypeNew, bcmFieldForwardingTypeMpls, 3 /* Third header */);
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

/* Call cint_field_vpls_gre_wa_main only for arad plus and below (not needed for jericho) */
int cint_field_vpls_gre_wa_setup(/* in */ int unit,
                                 /* in */ int group_priority,
                                 /* in */ bcm_field_group_t group) {
    int rv = BCM_E_NONE;
    int yesno = 0;


    if (!is_device_or_above(unit,JERICHO)) {
        return cint_field_vpls_gre_wa_main(unit,group_priority,group);
    }

    return rv;
}



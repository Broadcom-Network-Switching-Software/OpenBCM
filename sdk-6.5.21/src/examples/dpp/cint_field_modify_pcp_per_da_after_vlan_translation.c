
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 *  Modify the PCP value according to the Destination-MAC
 *  after a VLAN translation for Outer-Tag packets.
 *  Sequence:
 *  1. Define the PCP QoS Map profile for the PCP mapping
 *  2. Define a VLAN Action which modifies only the PCP
 *  for a single-tagged packet according to the defined mapping
 *  3. Define a QSET including the Destination-MAC-Address
 *  4. Define an ASET which changes the VLAN-Action and the
 *  overrride the PCP and DEI values
 *  5. Create a Field group and add entries
 */

#include <cint_qos_l2.c>

/*
 *  Set the Field group that points to
 *  the VLAN-Action and the PCP DEI values
 *  for the key defined above
 */
int pcp_field_group_set(/* in */ int unit,
                        /* in */ int group_priority,
                        /* in */ int nof_entries,
                        /* in */ int vlan_action_id, 
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


  if (NULL == group) {
    printf("Pointer to group ID must not be NULL\n");
    return BCM_E_PARAM;
  }

  /*
   *  The Field group is done at the ingress stage
   *  The Field group qualifier is Destination-MAC-Address
   */

  /* Define the QSET */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyVlanFormat);

  /*
   *  Build the action set for this Field group. 
   *  This Field Group can perform a change in the VLAN Action and
   *  in the PCP profile
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionOuterVlanPrioNew);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionVlanActionSetNew);

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
   *  Match the packets on different MAC-DAs
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

      /* Qualify only single Outer-Tag packets */
      result = bcm_field_qualify_VlanFormat(unit, ent, BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED, -1);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_DstMac\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }
      /*
       *  Change the VLAN-Action for Outer-Tagged packets
       *  It is recommended to qualify also on the VLAN-Tag-Format
       */
      result = bcm_field_action_add(unit, ent, bcmFieldActionVlanActionSetNew, vlan_action_id, BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_action_add\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }

      /*
       *  Change the PCP-DEI value to (7,1)
       */
      result = bcm_field_action_add(unit, ent, bcmFieldActionOuterVlanPrioNew, 0x7 /* PCP */, 0x1 /* DEI */);
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
 *  The packet VLAN Action will be to copy the VLAN Action of another VLAN
 */
int pcp_example(int unit) 
{
  int result;
  int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
  int nof_entries = 2;
  int qos_map_id, vlan_action_id;
  bcm_field_group_t group;
  bcm_vlan_action_set_t action;
  bcm_gport_t gport;
  bcm_vlan_t vlan = 4;
  bcm_vlan_t vlan_to_copy = 1;

  /* Set the PCP mapping */
  result = qos_map_l2_ingress_profile(unit, BCM_QOS_MAP_L2_OUTER_TAG, 1 /* vlan_if_set_else_lif_cos */);
  if (BCM_E_NONE != result) {
      printf("Error in qos_map_l2_ingress_profile\n");
      return result;
  }

  /* Get the PCP-mapping profile ID */
  qos_map_id = qos_map_id_l2_pcp_ingress_get(unit);

  /* 
   * Create VLAN
   */
  result = bcm_vlan_create(unit, vlan);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_vlan_create\n");
      return result;
  }
  
  /* 
   * VLAN membership
   */
  BCM_GPORT_LOCAL_SET(gport, 1);
  result = bcm_vlan_gport_add(unit, vlan, gport, 0);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_vlan_gport_add\n");
      return result;
  }
  
  BCM_GPORT_LOCAL_SET(gport, 13);
  result = bcm_vlan_gport_add(unit, vlan, gport, 0);
  /* 
   * Set the VLAN Action: 
   * - Remove the Outer-Tag and copy the Innner-Tag from the Outer-Tag 
   * - Set the PCP profile 
   */
  bcm_vlan_action_set_t_init(&action);
  action.ot_outer = bcmVlanActionDelete;
  action.ot_inner = bcmVlanActionCopy;
  action.ot_outer_pkt_prio = bcmVlanActionAdd;
  action.priority = qos_map_id;
  result = bcm_vlan_translate_action_create(unit, gport, bcmVlanTranslateKeyPortOuter, vlan_to_copy /* Outer-VLAN */, BCM_VLAN_NONE, &action);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_vlan_translate_action_create\n");
      return result;
  }

  result = pcp_field_group_set(unit, group_priority, nof_entries, action.action_id, &group);
  if (BCM_E_NONE != result) {
      printf("Error in pcp_field_group_set\n");
      return result;
  }

  return result;
}


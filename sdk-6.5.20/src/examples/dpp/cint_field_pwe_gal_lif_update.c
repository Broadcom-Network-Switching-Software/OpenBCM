/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved. 
*/


/*
 *  Create Field group such that for MPLS switched packets,
 *  when their Tunnel-Termination PWE lookup is found,
 *  cascaded Field groups are created:
 *  1. The first Field group transmits a one bit if Expect-BoS is set,
 *  if the Forwarding-Label.BOS is unset, and if the next label
 *  (after the forwarding label) is GAL
 *  2. If so, the second Field group overrides the In-LIF by
 *  the Tunnel-Termination PWE lookup result
 *  
 *  Due to an HW limitation, the In-LIF cannot be updated in a
 *  cascaded Field group by Tunnel-Termination PWE lookup result.
 *  Instead, the In-LIF is always overriden by the Tunnel-Termination
 *  PWE lookup result, and overrides again to the original In-LIF
 *  if the first cascaded lookup was not found.
 */
int pwe_gal_lif_update_set(/* in */ int unit,
                           /* in */ int group_priority_filter,
                           /* in */ bcm_field_group_t group_filter,
                           /* in */ int *group_priority_in_lif,
                           /* in */ bcm_field_group_t *group_in_lif) {
  int result;
  int auxRes;
  int i;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_entry_t ent;
  bcm_field_data_qualifier_t dq_gal; /* MPLS label after the Forwarding Label */
  uint16 dq_offset = 4;
  uint8 dq_length = 4;
  uint32 fg_ndx, nof_fgs = 3;
  bcm_pbmp_t pbm_mask;
  int presel_id = 0;
  int presel_flags = 0;
  bcm_field_presel_set_t psset;
  bcm_field_extraction_field_t ext[3];
  bcm_field_extraction_action_t extact;
  bcm_field_group_config_t grp_config;
  int value;
  uint8 data[4], mask[4];
  int ent_ndx, nof_entries;

  /* Define the preselector-set */
  BCM_FIELD_PRESEL_INIT(psset);
  /* 
   * Set the preselector 
   */
  /* Create a presel entity */
  if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
      presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
      result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
  } else {
      result = bcm_field_presel_create(unit, &(presel_id));
  }
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_presel_create\n");
      return result;
  }

  result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_Stage\n");
      return result;
  }

  /* Ethernet forwarding */
  result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldForwardingTypeMpls);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_ForwardingType\n");
      return result;
  }

  /* Tunnel-Termination PWE lookup is found */
  result = bcm_field_qualify_TunnelTerminated(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x1, 0x1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_TunnelTerminatedHit\n");
      return result;
  }

  BCM_FIELD_PRESEL_ADD(psset, presel_id);

  /*
   *  Set the cascaded key length to 4 bits
   */
  result = bcm_field_control_set(unit, bcmFieldControlCascadedKeyWidth, 4 /* bits in cascaded key */);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_control_set\n");
      return result;
  }

  /* 
   * Create the data qualifier for post-forwarding label (expected to be GAL)
   */
  bcm_field_data_qualifier_t_init(&dq_gal); 
  dq_gal.offset_base = bcmFieldDataOffsetBaseForwardingHeader; 
  dq_gal.offset = dq_offset; 
  dq_gal.length = dq_length; 
  result = bcm_field_data_qualifier_create(unit, &dq_gal);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_data_qualifier_create\n");
      return result;
  }      


  /* 
   * Create the Field groups   
   */
  for (fg_ndx = 0; fg_ndx < nof_fgs; fg_ndx++) {
      bcm_field_group_config_t_init(&grp_config);
      grp_config.group = (fg_ndx < 2)? group_in_lif[fg_ndx]: group_filter;

      /* Define the QSET */
      BCM_FIELD_QSET_INIT(grp_config.qset);
      BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyStageIngress);
      if (fg_ndx < 2) {
          if (fg_ndx == 0) {
              BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyTunnelTerminatedValue);
          } else {
              BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyCascadedKeyValue);
              BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyInVPort);
          }
      }
      else {
          BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyRxTrapCode);
          BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyIpType);
          BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyMplsForwardingLabelBos);
          /* Add the Data qualifier to the QSET */
          result = bcm_field_qset_data_qualifier_add(unit, &grp_config.qset, dq_gal.qual_id);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qset_data_qualifier_add\n");
              return result;
          }
      }

      /*
       *  This Field Group can change the EEI value
       */
      BCM_FIELD_ASET_INIT(grp_config.aset);
      if (fg_ndx < 2) {
          BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionIngressGportSet);
      } else {
          BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionCascadedKeyValueSet);
      }

      /*  Create the Field group */
      grp_config.priority = (fg_ndx < 2)? group_priority_in_lif[fg_ndx]: group_priority_filter;
      grp_config.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
      if (fg_ndx < 2) {
          grp_config.mode = bcmFieldGroupModeDirectExtraction;
      } else {
          grp_config.mode = bcmFieldGroupModeAuto;
      }
      grp_config.preselset = psset;
      result = bcm_field_group_config_create(unit, &grp_config);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_create\n");
          return result;
      }

      /*
       *  Add an entry to the group.
       */

      /* Add two entries for the filter Field group */
      nof_entries = (fg_ndx == 2)? 2: 1;
      for (ent_ndx = 0; ent_ndx < nof_entries; ent_ndx++) {
          result = bcm_field_entry_create(unit, grp_config.group, &ent);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_entry_create\n");
              return result;
          }

          if (fg_ndx < 2) {
              if (fg_ndx == 1) {
                  /* First field group lookup NOT found */
                  result = bcm_field_qualify_CascadedKeyValue(unit, ent, 0x0, 0x1);
                  if (BCM_E_NONE != result) {
                      printf("Error in bcm_field_qualify_CascadedKeyValue\n");
                      return result;
                  }
              }

              /* Rebuild the In-LIF from the Tunnel-Termination PWE lookup result */
              bcm_field_extraction_action_t_init(&extact);
              extact.action = bcmFieldActionIngressGportSet;
              extact.bias = 0;
              bcm_field_extraction_field_t_init(&(ext[0]));
              if (fg_ndx == 0) {
                  ext[0].qualifier = bcmFieldQualifyTunnelTerminatedValue;
              } else {
                  ext[0].qualifier = bcmFieldQualifyInVPort;
              }
              ext[0].lsb = 0;
              ext[0].bits = 16;
              result = bcm_field_direct_extraction_action_add(unit,
                                                              ent,
                                                              extact,
                                                              1 /* count */,
                                                              ext);
              if (BCM_E_NONE != result) {
                  printf("Error in bcm_field_direct_extraction_action_add\n");
                  return result;
              }
          } /* Add entry to the direct extraction field group */
          else {
              /* Trap ID is unexpected No Bos */
              result = bcm_field_qualify_RxTrapCode(unit, ent, bcmRxTrapMplsUnexpectedNoBos);
              if (BCM_E_NONE != result) {
                  printf("Error in bcm_field_qualify_RxTrapCode\n");
                  return result;
              }

              /* Forward-Label.Bos = 0 */
              result = bcm_field_qualify_MplsForwardingLabelBos(unit, ent, 0x0, 0x1);
              if (BCM_E_NONE != result) {
                  printf("Error in bcm_field_qualify_MplsForwardingLabelBos\n");
                  return result;
              }

              /* MPLS packet */
              result = bcm_field_qualify_IpType(unit, ent, ((ent_ndx == 0)? bcmFieldIpTypeMplsUnicast: bcmFieldIpTypeMplsMulticast));
              if (BCM_E_NONE != result) {
                  printf("Error in bcm_field_qualify_ForwardingType\n");
                  return result;
              }

              /* Label-ID = 13 */
              data[0] = 0x0;
              mask[0] = 0xFF;
              data[1] = 0x0;
              mask[1] = 0xFF;
              data[2] = 0xD0;
              mask[2] = 0xF0;
              data[3] = 0x00;
              mask[3] = 0x00;
              result = bcm_field_qualify_data(unit, ent, dq_gal.qual_id, data, mask, 4 /* len here always in bytes */);
              if (BCM_E_NONE != result) {
                  printf("Error in bcm_field_qualify_data\n");
                  return result;
              }

              result = bcm_field_action_add(unit, ent, bcmFieldActionCascadedKeyValueSet, 0x1, 0);
              if (BCM_E_NONE != result) {
                  printf("Error in bcm_field_action_add\n");
                  return result;
              }
          }
      }

      /* Install all to the HW */
      result = bcm_field_group_install(unit, grp_config.group);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_install\n");
          return result;
      }
  }

  return result;
}

int pwe_gal_lif_update_example(int unit) 
{
  int result;
  int group_priority_filter = 1;
  int group_filter = 1;
  int group_priority_in_lif[2] = {2,3};
  int group_in_lif[2] = {2,3};

 result = pwe_gal_lif_update_set(unit, 
                                 group_priority_filter,
                                 group_filter,
                                 group_priority_in_lif,
                                 group_in_lif);
  if (BCM_E_NONE != result) {
      printf("Error in pwe_gal_lif_update_set\n");
      return result;
  }

  return result;
}

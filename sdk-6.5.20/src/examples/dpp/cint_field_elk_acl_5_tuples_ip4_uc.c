
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * Using the KBP device as external TCAM lookups (ELK), this CINT describes 
 * a configuration example of an ELK ACL. 
 * The exact configuration consists in:
 *  1. Indicate via SOC property that the KBP device is connected and
 *  can be used for external lookups
 *  2. Indicate if the IPv4 routing tables (including IPv4 Unicast with and
 *  without RPF lookups, and IPv4 Multicast lookups) are located either in
 *  BCM88650 or in KBP via SOC properties
 *  3. Initialize the BCM88650 device
 *  4. Define the Field Groups:
 *   - in the Forwarding stage, define a 5-tuples of IPv4 for the IPv4 Unicast
 *   packets (both bridged and routed). This lookup is the third lookup in the
 *   KBP device. The lookup hit indication and the lookup result are passed
 *   to the ingress field processor (FP) stage.
 *   - In the ingress FP stage, define a Field Group preselecting only on the
 *   same IPv4 unicast packets and on the hit indication. If matched, define
 *   the lookup result as meter value (direct extraction field group).
 */


/*
 *  Set the Field groups:
 *  1. An ELK ACL at the Forwarding stage setting a general value
 *  for a 5-tuple IPv4 key and preselecting on all IPv4 Unicast packets
 *  2. An ingress FP Field group, preselecting also on IPv4 unicast packets,
 *  and setting the Meter-ID according to the lookup result if hit.
 */
int elk_acl_5_tuples_ipv4_uc_field_group_set(/* in */ int unit,
                                             /* in */ int group_priority_elk,
                                             /* in */ int group_priority_ingress_fp,
                                             /* in */ bcm_field_group_t group_elk, 
                                             /* in */ bcm_field_group_t group_ingress_fp,
                                             /* in */ int is_serial_rpf) 
{
  int result;
  int auxRes;
  bcm_field_group_config_t grp[2];
  bcm_field_action_t grp_action[2];
  int i, presel_id[2], stage_idx, app_type_idx,
      nof_stages = 2, nof_app_types = 3;
  bcm_field_presel_set_t psset[2];
  bcm_field_stage_t field_stage[2] = {bcmFieldStageExternal, bcmFieldStageIngress};
  int presel_flags[2] = {BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EXTERNAL, BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS};
  bcm_field_AppType_t app_type[3] = {bcmFieldAppTypeL2, bcmFieldAppTypeIp4Ucast, bcmFieldAppTypeIp4UcastRpf};
  uint8 dq_data[2], dq_mask[2];
  uint32 dq_ndx, dq_length[2] = {1, 15}, nof_data_quals = 2;
  bcm_field_qualify_t dq_qualifier[2];
  bcm_field_data_qualifier_t data_qual[2]; 
  uint32 qual_id[2] = {1, 2}; 

  if(is_serial_rpf) {
      grp_action[0] = bcmFieldActionExternalValue1Set;
      grp_action[1] = bcmFieldActionPolicerLevel0;

      dq_qualifier[0] = bcmFieldQualifyConstantOne; 
      dq_qualifier[1] = bcmFieldQualifyExternalValue1;
  }
  else {
      grp_action[0] = bcmFieldActionExternalValue2Set;
      grp_action[1] = bcmFieldActionPolicerLevel0;

      dq_qualifier[0] = bcmFieldQualifyConstantOne; 
      dq_qualifier[1] = bcmFieldQualifyExternalValue2;
  }
  /* Create a presel entity for both stages */
  for (stage_idx = 0; stage_idx < nof_stages; stage_idx++) {
      presel_id[stage_idx] = stage_idx;
      if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
          result = bcm_field_presel_create_stage_id(unit, field_stage[stage_idx], presel_id[stage_idx]);
          if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return result;
          }
      } else {
          presel_flags[stage_idx] = 0;
          result = bcm_field_presel_create(unit, &(presel_id[stage_idx]));
          if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create\n");
            return result;
          }
      }

      result = bcm_field_qualify_Stage(unit, presel_id[stage_idx] | BCM_FIELD_QUALIFY_PRESEL | presel_flags[stage_idx], field_stage[stage_idx]);
      if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
      }

      /* 
       * In the External stage, the preselection must be done according to the bcmFieldQualifyAppType qualifier. 
       * In HW, up to 8 profiles can be defined for separate sets of applications                              .
       * In the Ingress FP stage, the same preselection must be used, with a different stage value.                                                                                                      .
       */
      for (app_type_idx = 0; app_type_idx < nof_app_types; app_type_idx++) {
          result = bcm_field_qualify_AppType(unit, presel_id[stage_idx] | BCM_FIELD_QUALIFY_PRESEL | presel_flags[stage_idx], app_type[app_type_idx]);
          if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_AppType\n");
            return result;
          }
      }

      /* 
       * Preselect also according to the Lookup-Hit indication in the Ingress FP stage 
       */
      if (stage_idx == 1) { /* Ingress FP stage */
          if (is_serial_rpf) {
              /* Use the hit indication 2 because the ELK ACL will be in lookup 1 */
              result = bcm_field_qualify_ExternalHit1(unit, presel_id[stage_idx] | BCM_FIELD_QUALIFY_PRESEL | presel_flags[stage_idx], 0x1, 0x1);
          }
          else {
             /* Use the hit indication 2 because the ELK ACL will be in lookup 2 */
              result = bcm_field_qualify_ExternalHit2(unit, presel_id[stage_idx] | BCM_FIELD_QUALIFY_PRESEL | presel_flags[stage_idx], 0x1, 0x1);
          }
          if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_Stage\n");
            return result;
          }
      }
       
      BCM_FIELD_PRESEL_INIT(psset[stage_idx]);
      BCM_FIELD_PRESEL_ADD(psset[stage_idx], presel_id[stage_idx]);
  }

  /* 
   * Create the data qualifiers for the second Direct Extraction Field group
   */
  for (dq_ndx = 0; dq_ndx < nof_data_quals; dq_ndx++) {
      bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]); 
      data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_WITH_ID; 
      data_qual[dq_ndx].offset = 0;  
      data_qual[dq_ndx].qual_id = qual_id[dq_ndx];  
      data_qual[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
      data_qual[dq_ndx].length = dq_length[dq_ndx]; 
      result = bcm_field_data_qualifier_create(unit, &data_qual[dq_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }
  }

  /* 
   * Create the 2 Field groups 
   */
  for (stage_idx = 0; stage_idx < nof_stages; stage_idx++) {
      bcm_field_group_config_t_init(&grp[stage_idx]);
      grp[stage_idx].group = (stage_idx == 0)? group_priority_elk: group_priority_ingress_fp;
      /* Define the QSET */
      BCM_FIELD_QSET_INIT(grp[stage_idx].qset);

      if (stage_idx == 0) {
          BCM_FIELD_QSET_ADD(grp[stage_idx].qset, bcmFieldQualifyStageExternal);
          BCM_FIELD_QSET_ADD(grp[stage_idx].qset, bcmFieldQualifyIp4);
          BCM_FIELD_QSET_ADD(grp[stage_idx].qset, bcmFieldQualifySrcIp);
          BCM_FIELD_QSET_ADD(grp[stage_idx].qset, bcmFieldQualifyDstIp);
          BCM_FIELD_QSET_ADD(grp[stage_idx].qset, bcmFieldQualifyL4SrcPort);
          BCM_FIELD_QSET_ADD(grp[stage_idx].qset, bcmFieldQualifyL4DstPort);
          BCM_FIELD_QSET_ADD(grp[stage_idx].qset, bcmFieldQualifyIpProtocol);
          BCM_FIELD_QSET_ADD(grp[stage_idx].qset, bcmFieldQualifyInPort);
      }
      else {
          BCM_FIELD_QSET_ADD(grp[stage_idx].qset, bcmFieldQualifyStageIngress);
          for (dq_ndx = 0; dq_ndx < nof_data_quals; dq_ndx++) {
              /* Add the Data qualifier to the QSET */
              result = bcm_field_qset_data_qualifier_add(unit, &grp[stage_idx].qset, data_qual[dq_ndx].qual_id);
              if (BCM_E_NONE != result) {
                  printf("Error in bcm_field_qset_data_qualifier_add\n");
                  return result;
              }
          }

      }

      BCM_FIELD_ASET_INIT(grp[stage_idx].aset);
      BCM_FIELD_ASET_ADD(grp[stage_idx].aset, grp_action[stage_idx]);

      /*  Create the Field group */
      grp[stage_idx].priority = (stage_idx == 0)? group_priority_elk: group_priority_ingress_fp;
      grp[stage_idx].group = (stage_idx == 0)? group_elk: group_ingress_fp;
      grp[stage_idx].flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;
      grp[stage_idx].mode = (stage_idx == 0)? bcmFieldGroupModeAuto: bcmFieldGroupModeDirectExtraction;
      grp[stage_idx].preselset = psset[stage_idx];

      if (stage_idx == 0) {
          /* max_entry_priorities is supported only for external TCAM.
           * It determines the priority range that the user can set for
           * entries in this group. 
           * In this case, the user may set entries with priorities from 0 to 1000. 
           * Setting this parameter improves ELK insertion performance and reduces shuffling. */ 
          grp[stage_idx].max_entry_priorities = 1000;
      }
      result = bcm_field_group_config_create(unit, &grp[stage_idx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_create\n");
          return result;
      }
  }

  return result;
}
int elk_acl_5_tuples_ipv4_uc_entry_add(/* in */ int unit,
                                       /* in */ bcm_field_group_t group_elk, 
                                       /* in */ bcm_field_group_t group_ingress_fp,
                                       /* in */ int is_serial_rpf)
{
    int result;
    int stage_idx, nof_stages = 2;
    bcm_field_group_t group[2] = {group_elk, group_ingress_fp};
    bcm_field_entry_t ent[2];
    bcm_field_extraction_field_t ext[5];
    bcm_field_extraction_action_t extact;
    uint8 dq_data[2], dq_mask[2];
    uint32 dq_ndx, dq_length[2] = {1, 15}, nof_data_quals = 2;
    bcm_field_qualify_t dq_qualifier[2];
    uint32 qual_id[2] = {1, 2}; 
    bcm_field_action_t grp_action[2];
      if(is_serial_rpf) {
          grp_action[0] = bcmFieldActionExternalValue1Set;
          grp_action[1] = bcmFieldActionPolicerLevel0;

          dq_qualifier[0] = bcmFieldQualifyConstantOne; 
          dq_qualifier[1] = bcmFieldQualifyExternalValue1;
      }
      else {
          grp_action[0] = bcmFieldActionExternalValue2Set;
          grp_action[1] = bcmFieldActionPolicerLevel0;

          dq_qualifier[0] = bcmFieldQualifyConstantOne; 
          dq_qualifier[1] = bcmFieldQualifyExternalValue2;
      }

  for (stage_idx = 0; stage_idx < nof_stages; stage_idx++) {
      /*
       *  Add a single entry to the Field group.
       */
      result = bcm_field_entry_create(unit, group[stage_idx], &ent[stage_idx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
          return result;
      }

      if (stage_idx == 1) {
          /* Meter = ELK-Lookup-Result */
          bcm_field_extraction_action_t_init(&extact);
          extact.action = grp_action[stage_idx];
          extact.bias = 0;
          for (dq_ndx = 0; dq_ndx < nof_data_quals; dq_ndx++) {
              bcm_field_extraction_field_t_init(&(ext[dq_ndx]));
              ext[dq_ndx].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
              ext[dq_ndx].qualifier = qual_id[dq_ndx];
              ext[dq_ndx].lsb = 0;
              ext[dq_ndx].bits = dq_length[dq_ndx];
          }
          result = bcm_field_direct_extraction_action_add(unit,
                                                          ent[stage_idx],
                                                          extact,
                                                          nof_data_quals /* count */,
                                                          ext);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_entry_create\n");
              return result;
          }
      }
      else {
          /* 
           * Qualify an entry on: 
           * - its InPort is 1 
           * - the L4 Protocol is TCP (6) with Destination port 17 
           * - the packet is IPv4 
           */ 
          result = bcm_field_qualify_InPort(unit, ent[stage_idx], 0x1, -1);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qualify_InPort\n");
            return result;
          }
          result = bcm_field_qualify_IpProtocol(unit, ent[stage_idx], 0x6, 0xFF);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qualify_IpProtocol\n");
              return result;
          }
          result = bcm_field_qualify_L4DstPort(unit, ent[stage_idx], 17, 0xFFFF);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qualify_L4DstPort\n");
              return result;
          }

          /* Set the Meter-ID (action result) to 0xA5A */
          result = bcm_field_action_add(unit, ent[stage_idx], grp_action[stage_idx], 0xA5A, 0);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_action_add\n");
              return result;
          }
      }

      result = bcm_field_group_install(unit, group[stage_idx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_install\n");
          return result;
      }
  }

  return result;
}

/* 
 * The sequence for KBP ACL configurations is: 
 * 1. Initialize the BCM88650 device (SOC and BCM init) 
 * 2. Configure the ELK Field groups 
 * 3. Initialize the KBP 
 * 4. Add and remove entries dynamically
 */
int elk_acl_5_tuples_ipv4_uc_example(int unit) 
{
  int result;
  int group_priority_elk = 42, group_priority_pmf = 43;
  bcm_field_group_t group[2] = {21, 22};
  int is_serial_rpf = 0;

  if(is_device_or_above(unit,ARAD_PLUS)) {
      is_serial_rpf = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_rpf_fwd_parallel", 0) == 0);
  }

  /* Configure the ELK Field group */ 
  result = elk_acl_5_tuples_ipv4_uc_field_group_set(unit, group_priority_elk, group_priority_pmf, group[0], group[1], is_serial_rpf);
  if (BCM_E_NONE != result) {
      printf("Error in elk_acl_5_tuples_ipv4_uc_field_group_set\n");
      return result;
  }

  /* Initialize the KBP */
  bshell(unit, "kbp deinit_appl");
  bshell(unit, "kbp init_appl");


  /* Add entries */
  result = elk_acl_5_tuples_ipv4_uc_entry_add(unit, group[0], group[1], is_serial_rpf);
  if (BCM_E_NONE != result) {
      printf("Error in elk_acl_5_tuples_ipv4_uc_entry_add\n");
      return result;
  }

  return result;
}


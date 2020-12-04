
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

/*
 * This CINT defines an example of usage of Stateful Load balancing: 
 * 1. Define how to build the hash-flow (identifier of the same packet 
 * flow in 47 bits) and for which packets: 
 *   a. Define the preselectors
 *   b. Define the pre-hash Field group key
 *   c. Define the post-hash Field group key
 * Per packet, a single pre-hash and post-hash key must be defined.
 * Here, only IPv4overEthenet packets are 
 * selected, and the pre-hash key is {Source-IPv4, Destination-IPv4, 
 * L4-Source-Port, L4-Destination-Port}. 
 * The final key (hash-flow) is {7 bits of zero, the 32 bits result of the 32b-XOR, 
 * 8 bits of InPort}. 
 *
 * SOC PROPERTIES
 * To use stateful load balancing you must set the following SOC property:
 * resilient_hash_enable=1
 *  
 * run:
   cint cint_stateful_load_balancing.c
   cint
   slb_field_group_example(int unit); 
 *  
 * 2. Create stateful ECMP group 
 *    Use ecmp_hashing_main in cint_ecmp_hashing.c to create FECs and an ECMP group using them, and then set it to be stateful. 
 *  
 *    run:
      cint utility/cint_utils_l3.c
      cint cint_ip_route.c 
      cint cint_ecmp_hashing.c
      cint cint_stateful_load_balancing.c
      cint
      print slb_create_stateful_ecmp(unit, 13, 13, 4);
 * 
 * 3. Create stateful LAG group
 *    Use trunk_create in cint_trunk.c to create a LAG group (with TID = 2) and then set it to be stateful.
 *    The members in the LAG group will be ports base_out_port, base_out_port + 1, .. , base_out_port + out_port_num - 1.
 *    Additionally an entry will be added to the MACT mapping DA,VLAN = {01:02:03:04:05:00, 1} to TID 2. 
 *
 *    run:
      cint cint_trunk.c
      cint cint_stateful_load_balancing.c
      cint
      slb_create_stateful_trunk(unit, 200, 201, 3);
 * 
 * 4. Graceful stateful group member deletion
 *    Given an ECMP group and a member index to remove, a graceful deletion is performed.
 *    The traffic to all members remains equal, even during the deletion (hence the name).
 *    This deletion method can only be used with ECMP groups.
 *
 *    a) Copy the FEC group to a new base without the deleted member.
 *    b) Change the ECMP group to point to the new location (with the new group size).
 *       [ All new flows will be learned on the new FEC group. ]
 *    c) Delete all flows to the member to be deleted.
 *       [ New packets will be learned on the new group, and therefore load-balanced equally on all members. ]
 *    d) Replace existing flows with their new members.
 *    e) (Optional) Delete/destroy the old FECs.
 *
 *    run:
      cint cint_stateful_load_balancing.c
      cint
      print slb_graceful_member_removal_ecmp(unit, ecmp, 1, 2000);
 *    
 * 5. Disgraceful stateful group member deletion (LAG)
 *    Given a LAG group and a member index to remove, a disgraceful deletion is performed.
 *    In a disgraceful deletion, the last port receives more traffic than it should (hence the name) during a short time period.
 *    The disgraceful deletion is completely contained in the bcm_trunk_member_delete API.
 *    The following is done internally in bcm_trunk_member_delete:
 *    
 *    a) Replace the member to remove by the last member.
 *    b) Reduce the size of the LAG.
 *       [ During the time between a) and b), the last member will receive 1 / Group-Size                           ]
 *       [ more traffic than the other members, in particular all the flows that deleted member was receiving.      ]
 *    c) Delete all the flows pointing to the deleted member.
 *       [ All the flows the deleted member was receiving are now load-balanced equally between the group members. ]
 *    d) Replace all the flows pointing to the last member index with the deleted member index (where the last member was copied).
 *
 *    REMARK: It is possible to use this same deletion method with ECMP groups.
 *
 *    run:
      cint cint_stateful_load_balancing.c
      cint
      print slb_disgraceful_member_removal_lag(unit, tid, 1);
 * 6. Disgraceful stateful group member deletion (ECMP)
 *    Given an ECMP group and a member to remove, a disgraceful deletion is performed.
 *    In a disgraceful deletion, the last port receives more traffic than it should (hence the name) during a short time period.
 *    
 *    Deatils:
 *    a) Copy the last FEC info to the FEC to be removed.
 *    b) Reduce the size of the ECMP group.
 *       [ During the time between a) and b), the last member will receive 1 / Group-Size                           ]
 *       [ more traffic than the other members, in particular all the flows that deleted member was receiving.      ]
 *    c) Delete all the flows pointing to the deleted member.
 *       [ All the flows the deleted member was receiving are now load-balanced equally between the group members.  ]
 *    d) Replace all the flows pointing to the last member index with the deleted member index (where the last member was copied).
 *    e) Delete the last FEC.
 *
 *    REMARK: It is possible to use this same deletion method with ECMP groups.
 *
 *    run:
      cint cint_stateful_load_balancing.c
      cint
      print slb_disgraceful_member_removal_lag(unit, tid, 1);
 *
 * 7. Stateful group member addition is identical to non-stateful group member addition.
 */

struct cint_stateful_load_balancing_cfg_s {
  int slb_flow_label_key_only;
};

cint_stateful_load_balancing_cfg_s cint_stateful_load_balancing_cfg = {
/* slb_flow_label_key_only */
 0
};


/* 
 * Create the 2 field groups of pre/post hashing for 
 * the IPv4 packets: 
 * 1. Create the preselector matching only IPv4 packets 
 * 2. Create all the data qualifiers to be used in the Field 
 * group. Indeed, since the HW qualifier order is decisive during
 * the key construction, all the qualifier orders are defined 
 * according to the data qualifier ID. 
 * 3. Create the 2 Field groups, attaching them to the preselector. 
 * Note: in the post Field group, the qualifier bcmFieldQualifyDstTrunk 
 * is mandatory in the key after the 47 bits Hash-Flow. 
 */
int slb_field_group_set(/* in */  int unit,
                        /* in */  int group_priority_pre_hash,
                        /* in */  bcm_field_group_t group_pre_hash,
                        /* in */  int group_priority_post_hash,
                        /* in */  bcm_field_group_t group_post_hash) 
{
  int result;
  uint32 fg_ndx, dq_ndx, nof_dqs, nof_fgs, dq_min[2], dq_max[2], dq_length[8];
  uint16 dq_offset[8];
  bcm_field_group_config_t grp[2];
  bcm_field_data_qualifier_t data_qual[8];
  bcm_field_qualify_t dq_qualifier[8];
  int presel_id;
  int presel_flags = 0;
  bcm_field_presel_set_t psset;

  nof_fgs = 2;
  dq_min[0] = 0;/*min data-qualifier index for pre*/
  dq_max[0] = 3;/*max data-qualifier index for pre*/
  dq_min[1] = 4;/*min data-qualifier index for post*/

  /* Pre data qualifiers*/
  dq_qualifier[0] = bcmFieldQualifyL4DstPort;
  dq_length[0] = 16;
  dq_offset[0] = 0;
  dq_qualifier[1] = bcmFieldQualifyL4SrcPort;
  dq_length[1] = 16;
  dq_offset[1] = 0;
  dq_qualifier[2] = bcmFieldQualifyDstIp;
  dq_length[2] = 32;
  dq_offset[2] = 0;
  dq_qualifier[3] = bcmFieldQualifySrcIp;
  dq_length[3] = 32;
  dq_offset[3] = 0;

  if (cint_stateful_load_balancing_cfg.slb_flow_label_key_only == 0) {
	  nof_dqs = 8;
	  dq_max[1] = 7;/*max data-qualifier index for post*/

	  /* Post data qualifiers*/
	  dq_qualifier[4] = bcmFieldQualifyHashValue;
	  dq_length[4] = 32;
	  dq_offset[4] = 80; /* 4 CRCs of 16 bits and 1 XOR of 16 bits before in the hash value */
	  dq_qualifier[5] = bcmFieldQualifyInPort;
	  dq_length[5] = 8;
	  dq_offset[5] = 0;
	  dq_qualifier[6] = bcmFieldQualifyConstantZero;
	  dq_length[6] = 7;
	  dq_offset[6] = 0;
	  dq_qualifier[7] = bcmFieldQualifyDstTrunk; /*mandatory after 47 bits*/
	  dq_length[7] = 16;
	  dq_offset[7] = 0;
  } else {
	  nof_dqs = 6;
	  dq_max[1] = 5; /*max data-qualifier index for post*/

	  /* Post data qualifiers, total 47 bit*/
	  dq_qualifier[4] = bcmFieldQualifyHashValue;
	  dq_length[4] = 15; /*15 lsb of CRC-D*/
	  dq_offset[4] = 0; 
	  dq_qualifier[5] = bcmFieldQualifyHashValue;
	  dq_length[5] = 32; /*32bit XOR*/
	  dq_offset[5] = 80; /* 4 CRCs of 16 bits and 1 XOR of 16 bits before in the hash value */
  }

  /* 
   * 1. Create the Preselector on IPv4 packets
   */
  /* Create a presel entity */
  if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
      presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_HASH;
      result = bcm_field_presel_create_stage_id(unit, bcmFieldStageHash, presel_id);
      if (BCM_E_NONE != result) {
        printf("Error in bcm_field_presel_create_stage_id\n");
        return result;
      }

  } else {
      result = bcm_field_presel_create_id(unit, presel_id);
      if (BCM_E_NONE != result) {
        printf("Error in bcm_field_presel_create_id\n");
        return result;
      }
  }

  result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageHash);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_qualify_Stage\n");
    return result;
  }

  /* Match IPv4 packets */
  result = bcm_field_qualify_HeaderFormat(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldHeaderFormatIp4AnyL2L3);
  if (BCM_E_NONE != result) {
    printf("Error in bcm_field_qualify_HeaderFormat\n");
    return result;
  }

  BCM_FIELD_PRESEL_INIT(psset);
  BCM_FIELD_PRESEL_ADD(psset, presel_id);

  /* 
   * 2. Define all the data qualifiers
   */
  for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
      bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]); 
      data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES; 
      data_qual[dq_ndx].offset = dq_offset[dq_ndx];  
      data_qual[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
      data_qual[dq_ndx].length = dq_length[dq_ndx]; 
      data_qual[dq_ndx].stage = bcmFieldStageHash; 
      result = bcm_field_data_qualifier_create(unit, &data_qual[dq_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create %d\n", dq_ndx);
          return result;
      }
  }

  /* 
   * 3. Create the pre and post hashing Field groups 
   */
  for (fg_ndx = 0; fg_ndx < nof_fgs; fg_ndx++) {
      bcm_field_group_config_t_init(&grp[fg_ndx]);
      grp[fg_ndx].group = (fg_ndx == 0)? group_pre_hash: group_post_hash;
      grp[fg_ndx].priority = (fg_ndx == 0)? group_priority_pre_hash: group_priority_post_hash;

      /* 
       * Define the QSET
       */
      BCM_FIELD_QSET_INIT(grp[fg_ndx].qset);
      BCM_FIELD_QSET_ADD(grp[fg_ndx].qset, bcmFieldQualifyStageHash);
      for (dq_ndx = dq_min[fg_ndx]; dq_ndx <= dq_max[fg_ndx]; dq_ndx++) {
          /* Add the Data qualifier to the QSET */
          result = bcm_field_qset_data_qualifier_add(unit, &grp[fg_ndx].qset, data_qual[dq_ndx].qual_id);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qset_data_qualifier_add DQ %d FG %d\n", dq_ndx, fg_ndx);
              return result;
          }
      }

      /*
       *  Define the ASET - use counter 0.
       */
      BCM_FIELD_ASET_INIT(grp[fg_ndx].aset);
      if (fg_ndx == 0) {
          BCM_FIELD_ASET_ADD(grp[fg_ndx].aset, bcmFieldActionHashValueSet);
      }
      else {
          BCM_FIELD_ASET_ADD(grp[fg_ndx].aset, bcmFieldActionTrunkHashKeySet);
      }

      /*  Create the Field group with type Direct Extraction */
      grp[fg_ndx].flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ASET;
      grp[fg_ndx].mode = bcmFieldGroupModeHashing;
      grp[fg_ndx].preselset = psset;
      result = bcm_field_group_config_create(unit, &grp[fg_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_create\n");
          return result;
      }
  }

  return result;
}

/*
 *  Set an example configuration of this cint
 */
int slb_field_group_example(const int unit) 
{
  int result;
  int group_priority[2] = {BCM_FIELD_GROUP_PRIO_ANY, BCM_FIELD_GROUP_PRIO_ANY};
  bcm_field_group_t group[2] = {22, 23};

  result = slb_field_group_set(unit, group_priority[0], group[0], group_priority[1], group[1]);
  if (BCM_E_NONE != result) {
      printf("Error in slb_field_group_set\n");
      return result;
  }

  return result;
}


/* Use ecmp_hashing_main in cint_ecmp_hashing.c to create FECs and an ECMP group using them, and then set it to be stateful. */
/* For help about parameters see ecmp_hashing_main. */
int slb_create_stateful_ecmp(const int unit, const int in_port, const int out_port, const int ecmp_size)
{
  bcm_l3_egress_ecmp_t ecmp;
  int rc = BCM_E_NONE;

  rc = ecmp_hashing_main(unit, in_port, out_port, ecmp_size, 0);
  if (BCM_E_NONE != rc) {
      printf("Error in ecmp_hashing_main\n");
      print rc;
      return rc;
  }

  printf("ECMP 0x%08x created successfully.\n", cint_ecmp_hashing_data.ecmp.ecmp_intf);

  /* Get the ECMP and interfaces. */

  /* Set the ECMP to be resilient. */
  ecmp = cint_ecmp_hashing_data.ecmp;

  ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
  ecmp.flags = BCM_L3_WITH_ID | BCM_L3_REPLACE;

  rc = bcm_l3_egress_ecmp_create(unit, &ecmp, ecmp_size, cint_ecmp_hashing_data.egress_intfs);
  if (BCM_E_NONE != rc) {
      printf("Error in bcm_l3_egress_ecmp_create\n");
      print rc;
      return rc;
  }

  printf("ECMP 0x%08x set to perform stateful load balancing.\n", ecmp.ecmp_intf);

  return rc;
}

int slb_create_stateful_trunk(const int unit, const int in_port, const int base_out_port, const int out_port_num)
{
  int rc = BCM_E_NONE;
  bcm_trunk_t tid = 2;
  int use_lb_key = 1;
  int double_lbk_rng_disable = 0;
  int out_port_arr[out_port_num];
  unsigned int port_idx;
  bcm_trunk_t tid_example = 2;
  int rv = BCM_E_NONE;
  
  /* Fill out_port_arr with ports base_out_port + port_idx. */
  for (port_idx = 0; port_idx < out_port_num; port_idx++ ) {
    out_port_arr[port_idx] = base_out_port + port_idx;
  }

  /* Create the LAG group. */
  /* Must use LB key for SLB. */
  rv = trunk_create(unit, tid_example, in_port, out_port_num, out_port_arr, use_lb_key, double_lbk_rng_disable);
  if (rv != BCM_E_NONE) {
      printf("Error, trunk_create(), rv=%d.\n", rv);
      return rv;
  }
  printf("Trunk #%d create with %d ports\n", tid_example, out_port_num);

  /* Add a MAC entry, mapping DA,VLAN {01:02:03:04:05:00, 1} -> TID 2 */
  /* Ports 14, 15, 16, 17 are used because those ports are defined by default, other ports may be used as well */
  rv = l2_addr_config(unit, tid_example, 0, 0, 14, 15, 16, 17); 
  if(rv != BCM_E_NONE){
       printf("Error: l2_addr_config() uc\n");
  }

  /* Set the LAG group to be dynamic. */
  rc = bcm_trunk_psc_set(unit, tid, BCM_TRUNK_PSC_DYNAMIC_RESILIENT);
  if (BCM_E_NONE != rc) {
      printf("Error in bcm_trunk_psc_set\n");
      print rc;
      return rc;
  }

  return rc;
}

/*
 * Graceful ECMP member deletion. 
 * ecmp - The group 
 * member_to_delete_index - The index of the member to delete. 
 * new_fec_base - The FECs will be copied to this new FEC index. 
*/
int slb_graceful_member_removal_ecmp(const int unit, const bcm_if_t user_ecmp_intf, const int member_to_delete_index, const int new_fec_base)
{
  int max_members = 101;
  bcm_if_t members[max_members];
  bcm_if_t new_members[max_members];
  int nof_members;
  int rc;
  unsigned int fec_idx;
  bcm_l3_egress_ecmp_t ecmp;
  bcm_l3_egress_ecmp_resilient_entry_t match_rule;
  bcm_l3_egress_ecmp_resilient_entry_t replace_entry;
  int nof_matched_entries;
  bcm_if_t deleted_fec;

  ecmp.ecmp_intf = user_ecmp_intf;

  /* Get the ECMP group. */
  rc = bcm_l3_egress_ecmp_get(unit, &ecmp, max_members, members, &nof_members);
  if (BCM_E_NONE != rc) {
    printf("Error doing:\n");
    printf("print bcm_l3_egress_ecmp_get(%d, &ecmp(ecmp.ecmp_intf=0x%08x), %d, members, &nof_members);\n", unit, ecmp.ecmp_intf, max_members);
    print rc;
    return rc;
  }

  /* Make sure the ECMP group is resilient. */
  if (ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
    printf("Error - The ECMP group must be resilient.\n");
    return BCM_E_PARAM;
  }

  /* Make sure we have all the ecmp members in the members array. */
  if (nof_members >= max_members) {
    printf("Error - The ECMP group is too big. At most %d members allowed for the group.\n", max_members - 1);
    return BCM_E_PARAM;
  }

  /* Make sure the index is valid. */
  if (member_to_delete_index >= nof_members) {
    printf("Error - Invalid member specified (group size is %d).\n");
    return BCM_E_PARAM;
  }

  /* Make sure this is not the last member being deleted. */
  /* Deletion of the last member is handled in disgraceful deletion, and does not require the special treatment done here. */
  if (member_to_delete_index == nof_members - 1) {
    printf("Error - The deleted member must not be the last member (for an example on how to delete the last member see the disgraceful member removal).\n");
    return BCM_E_PARAM;
  }

  deleted_fec = members[member_to_delete_index];

  /* Change the old member to the new member in the array. */
  members[member_to_delete_index] = members[nof_members - 1];

  /* Copy the FECs to the new base with size - 1. */
  nof_members--;
  for (fec_idx = 0; fec_idx < nof_members; fec_idx ++) {
    bcm_l3_egress_t fec_info;
    /* Get the FEC info. */
    rc = bcm_l3_egress_get(unit, members[fec_idx], &fec_info);
    if (BCM_E_NONE != rc) {
      printf("Error doing:\n");
      printf("print bcm_l3_egress_get(%d, 0x%08x, &fec_info);\n", unit, unit, members[fec_idx]);
      print rc;
      return rc;
    }

    /* Use ID new_base + fec_idx for the new FECs. */
    new_members[fec_idx] = new_fec_base + fec_idx;
    
    rc = bcm_l3_egress_create(unit, BCM_L3_WITH_ID | BCM_L3_INGRESS_ONLY, &fec_info, new_members + fec_idx);
    if (BCM_E_NONE != rc) {
      printf("Error doing:\n");
      printf("print bcm_l3_egress_create(%d, BCM_L3_WITH_ID, &fec_info, new_members + %d);\n", unit, fec_idx);
      print rc;
      return rc;
    }

  }
  
  /* Change the ECMP group to point to the new FEC base. */
  ecmp.flags = BCM_L3_WITH_ID | BCM_L3_REPLACE;
  rc = bcm_l3_egress_ecmp_create(unit, &ecmp, nof_members, new_members);
  if (BCM_E_NONE != rc) {
      printf("Error doing:\n");
      printf("print bcm_l3_egress_ecmp_get(%d, %d, %d, members, &nof_members);\n", unit, ecmp->ecmp_intf, max_members);
      print rc;
      return rc;
  }

  /* Delete flows using the deleted member. */
  match_rule.ecmp = ecmp;
  match_rule.intf = deleted_fec;
  rc = bcm_l3_egress_ecmp_resilient_replace(unit, BCM_L3_ECMP_RESILIENT_MATCH_ECMP | BCM_L3_ECMP_RESILIENT_MATCH_INTF | BCM_L3_ECMP_RESILIENT_DELETE, 
                                   &match_rule, &nof_matched_entries, NULL);
  if (BCM_E_NONE != rc) {
      printf("Error doing:\n");
      printf("print bcm_l3_egress_ecmp_resilient_replace(%d, BCM_L3_ECMP_RESILIENT_MATCH_ECMP | BCM_L3_ECMP_RESILIENT_MATCH_INTF | BCM_L3_ECMP_RESILIENT_DELETE", unit);
      printf(", &match_rule (ecmp=%d,intf=%d), &nof_matched_entries, NULL).\n", ecmp.ecmp_intf, match_rule.intf);
      print rc;
      return rc;
  }

  /* Replace all flows using old FECs with new FECs. */
  for (fec_idx = 0; fec_idx < nof_members; fec_idx++) {
    match_rule.intf = members[fec_idx];
    replace_entry.intf = new_members[fec_idx];
    
    rc = bcm_l3_egress_ecmp_resilient_replace(unit, BCM_L3_ECMP_RESILIENT_MATCH_ECMP | BCM_L3_ECMP_RESILIENT_MATCH_INTF | BCM_L3_ECMP_RESILIENT_REPLACE, 
                                    &match_rule, &nof_matched_entries, &replace_entry);
    if (BCM_E_NONE != rc) {
      printf("Error doing:\n");
      printf("print bcm_l3_egress_ecmp_resilient_replace(%d, BCM_L3_ECMP_RESILIENT_MATCH_ECMP | BCM_L3_ECMP_RESILIENT_MATCH_INTF | BCM_L3_ECMP_RESILIENT_DELETE", unit);
      printf(", &match_rule (ecmp=%d,intf=%d), &nof_matched_entries, &replace_entry (intf=%d)).\n", ecmp.ecmp_intf, match_rule.intf, replace_entry.intf);
      print rc;
      return rc;
    }
  }

  /* Delete the old FECs*/
  for (fec_idx = 0; fec_idx < nof_members; fec_idx++) {
    rc = bcm_l3_egress_destroy(unit, members[fec_idx]);
    if (BCM_E_NONE != rc) {
      printf("Error doing:\n");
      printf("print bcm_l3_egress_destroy(%d, 0x%08x);\n", unit, members[fec_idx]);
      print rc;
      return rc;
    }
  }

  return BCM_E_NONE;
}

int slb_disgraceful_member_removal_ecmp(const int unit, const bcm_if_t user_ecmp_intf, const int member_to_delete_index)
{
  int max_members = 101;
  bcm_if_t members[max_members];
  bcm_if_t new_members[max_members];
  int nof_members;
  int rc;
  bcm_l3_egress_ecmp_t ecmp;
  bcm_l3_egress_ecmp_resilient_entry_t match_rule;
  bcm_l3_egress_ecmp_resilient_entry_t replace_entry;
  int nof_matched_entries;
  bcm_if_t deleted_fec;
  bcm_l3_egress_t fec_info;

  ecmp.ecmp_intf = user_ecmp_intf;

  /* Get the ECMP group. */
  rc = bcm_l3_egress_ecmp_get(unit, &ecmp, max_members, members, &nof_members);
  if (BCM_E_NONE != rc) {
    printf("Error doing:\n");
    printf("print bcm_l3_egress_ecmp_get(%d, &ecmp(ecmp.ecmp_intf=0x%08x), %d, members, &nof_members);\n", unit, ecmp.ecmp_intf, max_members);
    print rc;
    return rc;
  }

  /* Make sure the ECMP group is resilient. */
  if (ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
    printf("Error - The ECMP group must be resilient.\n");
    return BCM_E_PARAM;
  }

  /* Make sure we have all the ecmp members in the members array. */
  if (nof_members >= max_members) {
    printf("Error - The ECMP group is too big. At most %d members allowed for the group.\n", max_members - 1);
    return BCM_E_PARAM;
  }

  /* Make sure the index is valid. */
  if (member_to_delete_index >= nof_members) {
    printf("Error - Invalid member specified (group size is %d).\n");
    return BCM_E_PARAM;
  }

  deleted_fec = members[member_to_delete_index];

  /* Is the last member is being deleted ? */
  if (member_to_delete_index == nof_members - 1) {
    /* Reduce the ECMP size. */
    ecmp.flags = BCM_L3_WITH_ID | BCM_L3_REPLACE;
    rc = bcm_l3_egress_ecmp_create(unit, &ecmp, nof_members - 1, new_members);
    if (BCM_E_NONE != rc) {
        printf("Error doing:\n");
        printf("print bcm_l3_egress_ecmp_get(%d, %d, %d, members, &nof_members);\n", unit, ecmp->ecmp_intf, max_members);
        print rc;
        return rc;
    }

    /* Delete all flows using the last FEC. */
    match_rule.ecmp = ecmp;
    match_rule.intf = deleted_fec;
    rc = bcm_l3_egress_ecmp_resilient_replace(unit, BCM_L3_ECMP_RESILIENT_MATCH_ECMP | BCM_L3_ECMP_RESILIENT_MATCH_INTF | BCM_L3_ECMP_RESILIENT_DELETE, 
                                     &match_rule, &nof_matched_entries, NULL);
    if (BCM_E_NONE != rc) {
        printf("Error doing:\n");
        printf("print bcm_l3_egress_ecmp_resilient_replace(%d, BCM_L3_ECMP_RESILIENT_MATCH_ECMP | BCM_L3_ECMP_RESILIENT_MATCH_INTF | BCM_L3_ECMP_RESILIENT_DELETE", unit);
        printf(", &match_rule (ecmp=%d,intf=%d), &nof_matched_entries, NULL).\n", ecmp.ecmp_intf, match_rule.intf);
        print rc;
        return rc;
    }

    /* Delete the last FEC. */
    rc = bcm_l3_egress_destroy(unit, members[nof_members - 1]);
    if (BCM_E_NONE != rc) {
      printf("Error doing:\n");
      printf("print bcm_l3_egress_destroy(%d, 0x%08x);\n", unit, members[fec_idx]);
      print rc;
      return rc;
    }

    return BCM_E_NONE;
  }

  /* Copy the last member to the deleted member. */
  /* During this time the traffic sent to the deleted member will be sent to the last member instead. */

  /* Get the last member info. */
  rc = bcm_l3_egress_get(unit, members[nof_members - 1], &fec_info);
  if (BCM_E_NONE != rc) {
    printf("Error doing:\n");
    printf("print bcm_l3_egress_get(%d, 0x%08x, &fec_info);\n", unit, unit, members[nof_members - 1]);
    print rc;
    return rc;
  }

  /* Copy the last FEC info to the deleted FEC. */
  rc = bcm_l3_egress_create(unit, BCM_L3_WITH_ID | BCM_L3_REPLACE, &fec_info, &deleted_fec);
  if (BCM_E_NONE != rc) {
    printf("Error doing:\n");
    printf("print bcm_l3_egress_create(%d, BCM_L3_WITH_ID | BCM_L3_REPLACE, &fec_info, &deleted_fec);\n", unit);
    print rc;
    return rc;
  }

  /* Reduce the group size. */
  ecmp.flags = BCM_L3_WITH_ID | BCM_L3_REPLACE;
  rc = bcm_l3_egress_ecmp_create(unit, &ecmp, nof_members - 1, members);
  if (BCM_E_NONE != rc) {
      printf("Error doing:\n");
      printf("print bcm_l3_egress_ecmp_create(%d, ecmp(.ecmp_intf=0x%08x), %d, members);\n", unit, ecmp->ecmp_intf, nof_members - 1);
      print rc;
      return rc;
  }

  /* Delete flows using the deleted member. */
  /* The flows will be redistributed among all members. */
  match_rule.ecmp = ecmp;
  match_rule.intf = deleted_fec;
  rc = bcm_l3_egress_ecmp_resilient_replace(unit, BCM_L3_ECMP_RESILIENT_MATCH_ECMP | BCM_L3_ECMP_RESILIENT_MATCH_INTF | BCM_L3_ECMP_RESILIENT_DELETE, 
                                   &match_rule, &nof_matched_entries, NULL);
  if (BCM_E_NONE != rc) {
      printf("Error doing:\n");
      printf("print bcm_l3_egress_ecmp_resilient_replace(%d, BCM_L3_ECMP_RESILIENT_MATCH_ECMP | BCM_L3_ECMP_RESILIENT_MATCH_INTF | BCM_L3_ECMP_RESILIENT_DELETE", unit);
      printf(", &match_rule (ecmp=%d,intf=%d), &nof_matched_entries, NULL).\n", ecmp.ecmp_intf, match_rule.intf);
      print rc;
      return rc;
  }

  /* Replace all flows using the last member to the deleted member. */
  match_rule.ecmp = ecmp;
  match_rule.intf = members[nof_members - 1];
  replace_entry.intf = deleted_fec;
  
  rc = bcm_l3_egress_ecmp_resilient_replace(unit, BCM_L3_ECMP_RESILIENT_MATCH_ECMP | BCM_L3_ECMP_RESILIENT_MATCH_INTF | BCM_L3_ECMP_RESILIENT_REPLACE, 
                                  &match_rule, &nof_matched_entries, &replace_entry);
  if (BCM_E_NONE != rc) {
    printf("Error doing:\n");
    printf("print bcm_l3_egress_ecmp_resilient_replace(%d, BCM_L3_ECMP_RESILIENT_MATCH_ECMP | BCM_L3_ECMP_RESILIENT_MATCH_INTF | BCM_L3_ECMP_RESILIENT_REPLACE", unit);
    printf(", &match_rule (ecmp=%d,intf=%d), &nof_matched_entries, &replace_entry (intf=%d)).\n", ecmp.ecmp_intf, match_rule.intf, replace_entry.intf);
    print rc;
    return rc;
  }

  /* Delete the last FEC. */
  rc = bcm_l3_egress_destroy(unit, members[nof_members - 1]);
  if (BCM_E_NONE != rc) {
    printf("Error doing:\n");
    printf("print bcm_l3_egress_destroy(%d, 0x%08x);\n", unit, members[fec_idx]);
    print rc;
    return rc;
  }

  return BCM_E_NONE;
}

int slb_disgraceful_member_removal_lag(const int unit, const bcm_trunk_t tid, const bcm_trunk_member_t *delete_member)
{
  int rc; 

  rc = bcm_trunk_member_delete(unit, tid, delete_member);
  if (BCM_E_NONE != rc) {
      printf("Error doing:\n");
      printf("print bcm_trunk_member_delete(%d, %d, delete_member(.gport=0x%08x).\n", unit, tid, delete_member->gport);
      print rc;
      return rc;
  }

  return BCM_E_NONE;
}


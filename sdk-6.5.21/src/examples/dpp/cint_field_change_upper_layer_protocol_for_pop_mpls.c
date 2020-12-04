
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 *  For MPLS-forwarded packets, in case of MPLS pop,
 *  modify the identification of the Upper-Layer protocol
 *  
 *  Sequence:
 *  1. Define a Field Group match MPLS forwarded packets
 *  and modify the internal EEI header
 *  2. Create the Field group and add entries.
 *  If the BoS is not reached, set the Upper-Layer protocol
 *  to be MPLS. If the BoS is reached, identify if the
 *  next Layer protocol is IPv4 or IPv6
 */


/*
 *  Set the Field group that:
 *  - Match MPLS-forwarded packets that performed a POP operation
 *  - If match, modify the internal EEI header according to the
 *  next header and if the BoS bit is encountered
 */
int mpls_upper_protocol_field_group_set(/* in */ int unit,
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
  int nof_entries = 4 * 3; /* 2 bits in EEI to preserve * 3 entries (Is-BoS, IPv4, IPv6) */
  bcm_field_ForwardingType_t forwarding_type;
  uint32 eei_data, eei_mask;
  uint8 bos_data;
  bcm_field_data_qualifier_t ip_version; /* Take the 8 bits after the MPLS header */
  uint8 dqIpVersionData;
  uint8 dqIpVersionMask;
  uint32 eei_value;
  uint8 entry_mpls, entry_ipv4, entry_ipv6;
  bcm_field_presel_set_t psset;
  int presel_flags = BCM_FIELD_QUALIFY_PRESEL;

  if (NULL == group) {
    printf("Pointer to group ID must not be NULL\n");
    return BCM_E_PARAM;
  }

	/* Create Pre-Selector */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, 0);
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    } else {
        bcm_field_presel_create_id(unit, 0);
    }
	bcm_field_qualify_ForwardingType(unit, 0 | presel_flags, bcmFieldForwardingTypeMpls);
	BCM_FIELD_PRESEL_INIT(psset);
	BCM_FIELD_PRESEL_ADD(psset, 0);
	
	/*
   *  The Field group is done at the ingress stage
   *  The Field group qualifiers are Forwarding-Type, MPLS-BoS,
   *  EEI (MPLS-Label-Action) and Ip Version (in case of IP)
   */
    /* 
   * Define the Data qualifier: 
   * Take the 1st byte of the header after Forwarding Header
   */
  bcm_field_data_qualifier_t_init(&ip_version); 
  ip_version.offset_base = bcmFieldDataOffsetBaseNextForwardingHeader;
  ip_version.offset = 0; /* Start of the header */
  ip_version.length = 1; /* 1 byte */
  result = bcm_field_data_qualifier_create(unit, &ip_version);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_data_qualifier_create\n");
    auxRes = bcm_field_data_qualifier_destroy(unit, ip_version.qual_id);
    return result;
  }


  /* Define the QSET */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  /*BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingType);*/
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMplsForwardingLabelBos);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMplsForwardingLabelAction);

  /* Add the Data qualifier to the QSET */
  result = bcm_field_qset_data_qualifier_add(unit, &qset, ip_version.qual_id);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qset_data_qualifier_add\n");
      auxRes = bcm_field_data_qualifier_destroy(unit, ip_version.qual_id);
      return result;
  }

  /*
   *  This Field Group can change the EEI system header
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionSystemHeaderSet);

  /*  Create the Field group */
  result = bcm_field_group_create(unit, qset, group_priority, &grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      auxRes = bcm_field_group_destroy(unit, grp);
      auxRes = bcm_field_data_qualifier_destroy(unit, ip_version.qual_id);
      return result;
  }
  
  /* Attach a Preselection set */
  result = bcm_field_group_presel_set(unit, grp, &psset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      auxRes = bcm_field_group_destroy(unit, grp);
      auxRes = bcm_field_data_qualifier_destroy(unit, ip_version.qual_id);
      return result;
  }

  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set\n");
    auxRes = bcm_field_group_destroy(unit, grp);
    auxRes = bcm_field_data_qualifier_destroy(unit, ip_version.qual_id);
    return result;
  }
	
	/*
   *  Add multiple entries to the Field group.
   * 
   *  Match the packets on the Forwarding = IPv4 / IPv6
   *  Unicast / Multicast
   */
  for (entry_iter = 0; entry_iter < nof_entries; entry_iter++) {
      result = bcm_field_entry_create(unit, grp, &ent);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_data_qualifier_destroy(unit, ip_version.qual_id);
        return result;
      }

      /* 
       * Forwarding type: MPLS
       */
      /*
      forwarding_type = bcmFieldForwardingTypeMpls;
      result = bcm_field_qualify_ForwardingType(unit, ent, forwarding_type);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_ForwardingType\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_data_qualifier_destroy(unit, ip_version.qual_id);
        return result;
      }*/
      
      /* 
       * MPLS Action label encoding: 
       * Action[23:20] = 8 means POP action 
       * Action[5:4] must be saved  = Entry-Iter / 3
       * Action[3:0] indicates the Upper-Layer protocol 
       */
      eei_data = (entry_iter / 3) * 0x10;
      eei_data |= 0 << 8;
      eei_data |= (8 * 0x10) << 16;
      eei_mask = 0x30; /* bits 5:4 set */
      eei_mask |= 0 << 8;
      eei_mask |= 0xF0 << 16; /* bits 7:4 set */
      result = bcm_field_qualify_MplsForwardingLabelAction32(unit, ent, eei_data, eei_mask); 
      /* Note: the API will be modified to accept more than 8 bits */
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_MplsForwardingLabelAction32\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_data_qualifier_destroy(unit, ip_version.qual_id);
        return result;
      }

      /* 
       * BoS-encountered:
       * 2 out of 3 entries has BoS-encountered 
       * The other has not 
       */
      if ((entry_iter % 3) == 0) {
          /* MPLS */
          entry_mpls = 1;
          entry_ipv4 = 0;
          entry_ipv6 = 0;
          bos_data = 0x0;
      }
      else if ((entry_iter % 3) == 1) {
          /* IPv4 */
          entry_mpls = 0;
          entry_ipv4 = 1;
          entry_ipv6 = 0;
          bos_data = 0x1;
      }
      else { /* if ((entry_iter % 3) == 2) */
          /* IPv6 */
          entry_mpls = 0;
          entry_ipv4 = 0;
          entry_ipv6 = 1;
          bos_data = 0x1;
      }

      result = bcm_field_qualify_MplsForwardingLabelBos(unit, ent, bos_data, 0x1);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_MplsBos\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_data_qualifier_destroy(unit, ip_version.qual_id);
        return result;
      }

      /* 
       * If not-Bos, take IP Version Header and compare to 
       * IPv4 and IPv6 
       */
      if (entry_mpls) {
          dqIpVersionData = 0x0;
          dqIpVersionMask = 0x0;
      }
      else if (entry_ipv4) {
          /* IPv4 */
          dqIpVersionData = 4 * 0x10;
          dqIpVersionMask = 0xF0;
      }
      else { /* if (entry_ipv6) */
          /* IPv6 */
          dqIpVersionData = 6 * 0x10;
          dqIpVersionMask = 0xF0;
      }
      result = bcm_field_qualify_data(unit,
                                      ent,
                                      ip_version.qual_id,
                                      &dqIpVersionData,
                                      &dqIpVersionMask,
                                      1 /* length here always in bytes */);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_data\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_data_qualifier_destroy(unit, ip_version.qual_id);
        return result;
      }



      /*
       *  A matched frame changes the MPLS Forward Action (EEI)
       *  The mapping here is EEI = EEI-Inserted and
       *  Upper-Layer-Protocol = MPLS / IPv4 / IPv6 for each entry
       */
      eei_value = (8 * 0x100000) /* 8 to indicate POP */
                  + (entry_iter / 3) * 0x10 /* Same value for bits 5:4 */;
      if (entry_mpls) {
          /* MPLS for Upper-Layer-Protocol - encoded 5 */
          eei_value = eei_value + 5;
      }
      else if (entry_ipv4) {
          /* IPv4 - 1 */
          eei_value = eei_value + 1;
      }
      else { /* if (entry_ipv6) */
          /* IPv6 - 3 */
          eei_value = eei_value + 3;
      }
      result = bcm_field_action_add(unit, ent, bcmFieldActionSystemHeaderSet, 
                                    bcmFieldSystemHeaderPphEei /* Modify the Eei system header */, eei_value);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_action_add\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_data_qualifier_destroy(unit, ip_version.qual_id);
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
int mpls_upper_protocol_example(int unit) 
{
  int result;
  int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
  bcm_field_group_t group;


 result = mpls_upper_protocol_field_group_set(unit, group_priority, &group);
  if (BCM_E_NONE != result) {
      printf("Error in mpls_upper_protocol_set\n");
      return result;
  }

  return result;
}

int mpls_upper_protocol_set_teardown(/* in */ int unit,
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



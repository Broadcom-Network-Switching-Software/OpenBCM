
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * According to the packet format, if not simple bridged, 
 * set the rules to extract the ECN value (2 bits), and set: 
 *  - if the ECN is supported (ECN-capable)
 *  - in this case, if the packet is congested (CNI bit)
 *  
 *  Sequence:
 *  1. Define a Field Group matching possible header locations of
 *  the ECN bits, the header-format and forwarding header location
 *  2. Create the Field group and add entries.
 */

/* Static table where to take the ECN bits from (in general Forwarding-Header and its tunnels) */
int is_fretta_mode = 0;
static int Num_of_ecn_cases=45;
static int Ecn_cases[Num_of_ecn_cases][13] = {
    /* Header-Format | Forwarding-Relevant | Fwd-Code      | Header-ndx-After-Ethernet  | IPv4-Inner | IPv6-Inner | IPv4-Outer | IPv6-Outer | MPLS-3 | MPLS-2 | MPLS-1 | Inner-Eth-IPv4 | Inner-Eth-IPv6  */
                                                             /*  0 if not relevant  */                                                                                                                   
    {bcmFieldHeaderFormatIp4,            0,          0,                         0,             0,         0,           1,           0,         0,       0,       0,            0,               0,       }, 
    {bcmFieldHeaderFormatIp6,            0,          0,                         0,             0,         0,           0,           1,         0,       0,       0,            0,               0,       }, 
    {bcmFieldHeaderFormatMplsLabel1,     0,          0,                         0,             0,         0,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatMplsLabel2,1,bcmFieldForwardingTypeMplsLabel1,         0,             0,         0,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatMplsLabel2,1,bcmFieldForwardingTypeMplsLabel2,         0,             0,         0,           0,           0,         0,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatMplsLabel3,1,bcmFieldForwardingTypeMplsLabel1,         0,             0,         0,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatMplsLabel3,1,bcmFieldForwardingTypeMplsLabel2,         0,             0,         0,           0,           0,         0,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatMplsLabel3,1,bcmFieldForwardingTypeMplsLabel3,         0,             0,         0,           0,           0,         1,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp4Ip4,        0,          0,                          1,             0,         0,           1,           0,         0,       0,       0,            0,               0,       }, 
    {bcmFieldHeaderFormatIp4Ip4,        0,          0,                          2,             1,         0,           1,           0,         0,       0,       0,            0,               0,       }, 
    {bcmFieldHeaderFormatIp6Ip4,        0,          0,                          1,             0,         0,           1,           0,         0,       0,       0,            0,               0,       }, 
    {bcmFieldHeaderFormatIp6Ip4,        0,          0,                          2,             0,         1,           1,           0,         0,       0,       0,            0,               0,       }, 
    {bcmFieldHeaderFormatEthIp4Eth, 	0,			0,							1,			   0,		  0,		   1,			0,		   0,		0,		 0, 		   0,				0,		 },
    {bcmFieldHeaderFormatEthIp4Eth, 	0,			0,							2,			   0,		  0,		   1,			0,		   0,		0,		 0, 		   1,				0,		 },
    {bcmFieldHeaderFormatEthIp4Eth, 	0,			0,							2,			   0,		  0,		   1,			0,		   0,		0,		 0, 		   0,				1,		 },
    {bcmFieldHeaderFormatIp4MplsLabel1, 0,          0,                          1,             0,         0,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp4MplsLabel1, 0,          0,                          2,             1,         0,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp4MplsLabel2,1,bcmFieldForwardingTypeMplsLabel1,      1,             0,         0,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp4MplsLabel2,1,bcmFieldForwardingTypeMplsLabel2,      1,             0,         0,           0,           0,         0,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp4MplsLabel2, 0,          0,                          2,             1,         0,           0,           0,         0,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp4MplsLabel3,1,bcmFieldForwardingTypeMplsLabel1,      1,             0,         0,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp4MplsLabel3,1,bcmFieldForwardingTypeMplsLabel2,      1,             0,         0,           0,           0,         0,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp4MplsLabel3,1,bcmFieldForwardingTypeMplsLabel3,      1,             0,         0,           0,           0,         1,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp4MplsLabel3, 0,          0,                          2,             1,         0,           0,           0,         1,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp6MplsLabel1, 0,          0,                          1,             0,         0,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp6MplsLabel1, 0,          0,                          2,             0,         1,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp6MplsLabel2,1,bcmFieldForwardingTypeMplsLabel1,      1,             0,         0,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp6MplsLabel2,1,bcmFieldForwardingTypeMplsLabel2,      1,             0,         0,           0,           0,         0,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp6MplsLabel2, 0,          0,                          2,             0,         1,           0,           0,         0,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp6MplsLabel3,1,bcmFieldForwardingTypeMplsLabel1,      1,             0,         0,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp6MplsLabel3,1,bcmFieldForwardingTypeMplsLabel2,      1,             0,         0,           0,           0,         0,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp6MplsLabel3,1,bcmFieldForwardingTypeMplsLabel3,      1,             0,         0,           0,           0,         1,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatIp6MplsLabel3, 0,          0,                          2,             0,         1,           0,           0,         1,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatEthMplsLabel1, 0,          0,                          1,             0,         0,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatEthMplsLabel1, 0,          0,                          2,             0,         0,           0,           0,         0,       0,       1,            1,               0,       }, 
    {bcmFieldHeaderFormatEthMplsLabel1, 0,          0,                          2,             0,         0,           0,           0,         0,       0,       1,            0,               1,       }, 
    {bcmFieldHeaderFormatEthMplsLabel2,1,bcmFieldForwardingTypeMplsLabel1,      1,             0,         0,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatEthMplsLabel2,1,bcmFieldForwardingTypeMplsLabel2,      1,             0,         0,           0,           0,         0,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatEthMplsLabel2, 0,          0,                          2,             0,         0,           0,           0,         0,       1,       1,            1,               0,       }, 
    {bcmFieldHeaderFormatEthMplsLabel2, 0,          0,                          2,             0,         0,           0,           0,         0,       1,       1,            0,               1,       }, 
    {bcmFieldHeaderFormatEthMplsLabel3,1,bcmFieldForwardingTypeMplsLabel1,      1,             0,         0,           0,           0,         0,       0,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatEthMplsLabel3,1,bcmFieldForwardingTypeMplsLabel2,      1,             0,         0,           0,           0,         0,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatEthMplsLabel3,1,bcmFieldForwardingTypeMplsLabel3,      1,             0,         0,           0,           0,         1,       1,       1,            0,               0,       }, 
    {bcmFieldHeaderFormatEthMplsLabel3, 0,          0,                          2,             0,         0,           0,           0,         1,       1,       1,            1,               0,       }, 
    {bcmFieldHeaderFormatEthMplsLabel3, 0,          0,                          2,             0,         0,           0,           0,         1,       1,       1,            0,               1,       }, 
  };                                                                                                                      

/*
 *  Set the Field group that:
 *  - Match almost any packet and extract the CNI bits
 *  
 *  The ECN field is always bits 1:0 of IP.ToS (IPv4 and IPv6),
 *  with encoding:
 *  0 - non capable, 1/2 - non congested, 3 - congested.
 *  For MPLS, 2 options:
 *  1. 1b encoding: LSB of Exp (MPLS-EXP[0]) to indicate
 *     0 - non congested, 1 - congested.
 *     Here, ECN is always capable.
 *  2. 2b encoding: LSBs of Exp (MPLS-EXP[1:0])
 *  with same encoding than IP
 */
int ecn_extract_field_group_set(/* in */ int unit,
                                /* in */ int group_priority,
                                /* in */ int exp_bit_encoding,
                                /* out */ bcm_field_group_t *group) 
{
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  bcm_field_entry_t ent;
  int entry_iter, entry_iter_2, header_iter;
  int ecn_cases_nof_entries = Num_of_ecn_cases /* according to Ecn_cases number of lines */; 
  int nof_entries; 
  int nof_bits_encoding; 
  bcm_field_ForwardingType_t forwarding_type;
  /* Take the 1st Byte of IPv6 Traffic Class. 0 - outer, 1 - inner, 2 - 3rd header, 3 - ToS of 3rd Header IPv4, 4 - Inner-Eth Ethertype */
  bcm_field_data_qualifier_t ipv6_tc[5]; 
  bcm_field_data_offset_base_t offset_base[5] = {bcmFieldDataOffsetBaseFirstHeader, bcmFieldDataOffsetBaseSecondHeader,
      bcmFieldDataOffsetBaseThirdHeader, bcmFieldDataOffsetBaseThirdHeader, bcmFieldDataOffsetBaseThirdHeader};
  uint8 ipv6_tc_data, ipv6_tc_mask;
  uint8 ethertype_data[2], ethertype_mask[2];
  int ipv6_tc_ndx;
  int ecn_capable, ecn_capable_set, cni, ecn_value_curr_header, ecn_value_new;
  int bit_ndx_curr; /* Curent bits to look in the entry_iter_2 */
  bcm_field_data_offset_base_t base_header[3] = {bcmFieldDataOffsetBasePacketStart, 
      bcmFieldDataOffsetBaseFirstHeader, bcmFieldDataOffsetBaseSecondHeader};

  if (NULL == group) {
    printf("Pointer to group ID must not be NULL\n");
    return BCM_E_PARAM;
  }

  /*
   *  The Field group is done at the ingress stage
   *  The Field group qualifiers are Header-Code,
   *  Forwarding Header location,
   *  Inner/Outer IP ToS, MPLS-Exp 1/2/3
   */


  /* Define the QSET */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIp4);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyHeaderFormat); 
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingType); /* To be replaced by Forwarding-header location */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTos);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInnerTos);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMplsLabel1Exp);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMplsLabel2Exp);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMplsLabel3Exp);

  /* Create data qualifiers for IPv6 Inner/Outer TrafficClass location */
  for (ipv6_tc_ndx = 0; ipv6_tc_ndx < 5; ipv6_tc_ndx++) {
      bcm_field_data_qualifier_t_init(&ipv6_tc[ipv6_tc_ndx]); 
      ipv6_tc[ipv6_tc_ndx].offset_base = offset_base[ipv6_tc_ndx]; 
      ipv6_tc[ipv6_tc_ndx].offset = (ipv6_tc_ndx == 4)? 2: 1; 
      ipv6_tc[ipv6_tc_ndx].flags = (ipv6_tc_ndx == 4)? BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE: 0; 
      ipv6_tc[ipv6_tc_ndx].length = (ipv6_tc_ndx == 4)? 2: 1; /* 1 byte, ECN in bits 5:4 for IPv6,  1:0 for Ipv4 */
      result = bcm_field_data_qualifier_create(unit, &ipv6_tc[ipv6_tc_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[0].qual_id);
          auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[1].qual_id);
        return result;
      }

      /* Add the Data qualifier to the QSET */
      result = bcm_field_qset_data_qualifier_add(unit, &qset, ipv6_tc[ipv6_tc_ndx].qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[0].qual_id);
          auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[1].qual_id);
          return result;
      }
  }

  /*
   *  This Field Group can change the ECN value
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionEcnNew);

  /*  Create the Field group */
  result = bcm_field_group_create(unit, qset, group_priority, &grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      auxRes = bcm_field_group_destroy(unit, grp);
      auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[0].qual_id);
      auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[1].qual_id);
      return result;
  }
  
  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set\n");
    auxRes = bcm_field_group_destroy(unit, grp);
    auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[0].qual_id);
    auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[1].qual_id);
    return result;
  }


  /*
   *  Add multiple entries to the Field group.
   *  
   *  Match the packets on the Forwarding = IPv4 / IPv6
   *  Unicast / Multicast
   */
  for (entry_iter = 0; entry_iter < ecn_cases_nof_entries; entry_iter++) {
      /* printf("entry_iter =%d\n", entry_iter); */
      /* 
       * Compute the number of entries in TCAM to add for this case: 
       * - 4 cases per IP header (2 bits) 
       * - 2 or 4 cases per MPLS header according to encoding 
       */
      nof_entries = 1;
      for (header_iter = 0; header_iter < 9; header_iter++) {
          nof_bits_encoding = ((header_iter <= 4) || (header_iter >= 7))? 2 /* IP header */: exp_bit_encoding/* MPLS header */;
          nof_entries = nof_entries * ((Ecn_cases[entry_iter][4 + header_iter])? (1 << nof_bits_encoding): 1);
      }
      /* printf("entry_iter =%d, nof_entries =%d\n", entry_iter, nof_entries); */
      for (entry_iter_2 = 0; entry_iter_2 < nof_entries; entry_iter_2++) {
          result = bcm_field_entry_create(unit, grp, &ent);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_entry_create\n");
            auxRes = bcm_field_group_destroy(unit, grp);
            auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[0].qual_id);
            auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[1].qual_id);
            return result;
          }

          result = bcm_field_qualify_HeaderFormat(unit, ent, Ecn_cases[entry_iter][0]);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qualify_HeaderFormat\n");
            auxRes = bcm_field_group_destroy(unit, grp);
            auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[0].qual_id);
            auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[1].qual_id);
            return result;
          }

          /* Qualify on Forwarding-Code if necessary */
          if (Ecn_cases[entry_iter][1]) {
              result = bcm_field_qualify_ForwardingType(unit, ent, Ecn_cases[entry_iter][2]);
              if (BCM_E_NONE != result) {
                  printf("Error in bcm_field_qualify_ForwardingType\n");
                auxRes = bcm_field_group_destroy(unit, grp);
                auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[0].qual_id);
                auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[1].qual_id);
                return result;
              }
          }

          /*
           * First, computation of the ECN-Capable and is-Congested (CNI) bits 
           * according to the rule: 
           * - ECN-Capable according to the Forwarding Header 
           * - CNI is a big OR (all the CNI bits of all the headers
           * up to forwarding header) 
           */
          ecn_capable_set = FALSE;
          cni = 0;
          ecn_capable = 0;
          bit_ndx_curr = 0;
          for (header_iter = 0; header_iter < 9; header_iter++) {
              if (Ecn_cases[entry_iter][4 + header_iter]) { 
                  /* printf("header_iter =%d\n", header_iter); */
                  nof_bits_encoding = ((header_iter <= 4) || (header_iter >= 7))? 2 /* IP header */: exp_bit_encoding/* MPLS header */;
                  ecn_value_curr_header = (entry_iter_2 >> bit_ndx_curr) % (1 << nof_bits_encoding);
                  if (ecn_capable_set == FALSE) {
                      ecn_capable_set = TRUE;
                  ecn_capable = 1; /* MPLS 1b encoding */
                  if (nof_bits_encoding == 2) { /* 2b encoding, MPLS or IP */
                      ecn_capable = (ecn_value_curr_header > 0)? 1: 0;
                      }
                  }
                  if (nof_bits_encoding == 2) { /* 2b encoding, MPLS or IP */
                      cni |= (ecn_value_curr_header == 0x3)? 1: 0;
                  }
                  else {
                      /* 1b encoding */
                      cni |= (ecn_value_curr_header == 0x1)? 1: 0;
                  }
                  bit_ndx_curr += nof_bits_encoding;
                   /*printf("  bit_ndx_curr =%d, ", bit_ndx_curr);
                  printf("nof_bits_encoding =%d, ", nof_bits_encoding);
                  printf("ecn_value_curr_header =%d, ", ecn_value_curr_header);
                  printf("ecn_capable =%d, ", ecn_capable);
                  printf("ecn_capable_final =%d, ", ecn_capable_final);
                  printf("cni =%d\n", cni); */

                  /* Qualify on this */
                  switch (header_iter) {
                  case 0: /* IPv4 Inner */
                      result = bcm_field_qualify_InnerTos(unit, ent, ecn_value_curr_header, 0x3 /* mask */);
                      break;
                  case 1: /* IPv6 Inner */
                      ipv6_tc_data = (ecn_value_curr_header << 4);
                      ipv6_tc_mask = 0x30;
                      result = bcm_field_qualify_data(unit, ent, ipv6_tc[1].qual_id, &ipv6_tc_data, &ipv6_tc_mask, 1);
                      break;
                  case 2: /* IPv4 Outer */
                      result = bcm_field_qualify_Tos(unit, ent, ecn_value_curr_header, 0x3 /* mask */);
                      break;
                  case 3: /* IPv6 Outer */
                      ipv6_tc_data = (ecn_value_curr_header << 4);
                      ipv6_tc_mask = 0x30;
                      result = bcm_field_qualify_data(unit, ent, ipv6_tc[0].qual_id, &ipv6_tc_data, &ipv6_tc_mask, 1);
                      break;
                  case 4: /* MPLS 3 */
                      result = bcm_field_qualify_MplsLabel3Exp(unit, ent, ecn_value_curr_header, (1 << nof_bits_encoding) - 1 /* mask */);
                      break;
                  case 5: /* MPLS 2 */
                      result = bcm_field_qualify_MplsLabel2Exp(unit, ent, ecn_value_curr_header, (1 << nof_bits_encoding) - 1 /* mask */);
                      break;
                  case 6: /* MPLS 1 */
                      result = bcm_field_qualify_MplsLabel1Exp(unit, ent, ecn_value_curr_header, (1 << nof_bits_encoding) - 1 /* mask */);
                      break;
                  case 7: /* Inner-Eth-IPv4 */
                      ipv6_tc_data = (ecn_value_curr_header << 0);
                      ipv6_tc_mask = 0x3;
                      result = bcm_field_qualify_data(unit, ent, ipv6_tc[3].qual_id, &ipv6_tc_data, &ipv6_tc_mask, 1);
                      ethertype_data[0] = 0x00;
                      ethertype_data[1] = 0x08;
                      ethertype_mask[0] = 0xFF;
                      ethertype_mask[1] = 0xFF;
                      result = bcm_field_qualify_data(unit, ent, ipv6_tc[4].qual_id, ethertype_data, ethertype_mask, 2); 
                      break;
                  case 6: /* Inner-Eth-IPv6 */
                      ipv6_tc_data = (ecn_value_curr_header << 4);
                      ipv6_tc_mask = 0x30;
                      result = bcm_field_qualify_data(unit, ent, ipv6_tc[2].qual_id, &ipv6_tc_data, &ipv6_tc_mask, 1);
                      ethertype_data[0] = 0xDD;
                      ethertype_data[1] = 0x86;
                      ethertype_mask[0] = 0xFF;
                      ethertype_mask[1] = 0xFF;
                      result = bcm_field_qualify_data(unit, ent, ipv6_tc[4].qual_id, ethertype_data, ethertype_mask, 2); 
                      break;
                  default: 
                      break;
                  }
                  if (BCM_E_NONE != result) {
                      printf("Error in switch (header_iter)\n");
                    auxRes = bcm_field_group_destroy(unit, grp);
                    auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[0].qual_id);
                    auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[1].qual_id);
                    return result;
                  }
              }
          }

          /*
           * Build the action value
           */
          ecn_value_new = (ecn_capable == 0)? 0x0: ((cni == 1)? 0x3: 0x1);
          result = bcm_field_action_add(unit, ent, bcmFieldActionEcnNew, ecn_value_new, 0);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_action_add\n");
            auxRes = bcm_field_group_destroy(unit, grp);
            auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[0].qual_id);
            auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[1].qual_id);
            return result;
          }
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
    auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[0].qual_id);
    auxRes = bcm_field_data_qualifier_destroy(unit, ipv6_tc[1].qual_id);
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
int ecn_extract_example(int unit) 
{
  int result;
  int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
  bcm_field_group_t group;

  if (is_fretta_mode == 1) {
      Num_of_ecn_cases = 15;
  }
  result = ecn_extract_field_group_set(unit, group_priority, 2 /* 2b encoding */, &group);
  if (BCM_E_NONE != result) {
      printf("Error in ecn_extract_set\n");
      return result;
  }

  return result;
}

int ecn_extract_set_teardown(/* in */ int unit,
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



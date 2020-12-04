
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * VxLAN application : 
 * - only for the VxLAN application 
 * - match VxLAN packets: UDP over IPv4 over Eth, 
 * with a specific UDP Destination port 
 * - if the Source-IP lookup in Tunnel Termination has failed, snoop 
 * the packet according to the snooop-command
 * - if the VNI map lookup has failed, trap the packet according 
 * to the Gport-trap 
 */
int cint_field_vxlan_setup(/* in */ int unit,
                              /* in */ int group_priority,
                              /* in */ bcm_field_group_t group,
                              /* in */ uint32 udp_port_vxlan,
                              /* in */ int snoop_cmnd,
                              /* in */ int gport_trap) {
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  bcm_field_entry_t ent;
  int entry_ndx, nof_entries = 2;
  uint8 dqMask[2] = {0xFF, 0xFF}, dqData[2];
  bcm_field_data_qualifier_t qual;

  /* 
   * Build the Qualifier-Set 
   */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMplsTerminated); /* IP termination hit */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTunnelTerminated); /* SIP lookup hit */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpTunnelHit); /* VNI map lookup hit */

  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpType);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocolCommon);

  /* 
   * Create Data qualifier to match the Destination UDP Port: 
   * due to internal processing, the UDP header is located at the 
   * end of Header-After-Ethernet (IP) and not at the beginning of 
   * the header after Header-After-Ethernet.
   */
  bcm_field_data_qualifier_t_init(&qual);
  qual.offset_base = bcmFieldDataOffsetBaseSecondHeader;
  qual.offset = 16; /* Take offset=-16 Byte from start of After-IP header */
  qual.length = 2; /* Extract 2 bytes */
  qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE;
  result = bcm_field_data_qualifier_create(unit, &qual);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_data_qualifier_create\n");
      return result;
  }
  result = bcm_field_qset_data_qualifier_add(unit, &qset, qual.qual_id);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qset_data_qualifier_add\n");
      return result;
  }

  /*
   *  Build action set for the group we want to create. 
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionSnoop);
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
   *  Add two entries to the group 
   */
  for (entry_ndx = 0; entry_ndx < nof_entries; entry_ndx++) {
      result = bcm_field_entry_create(unit, grp, &ent);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
          return result;
      }

      /* IPv4 packets */
      result = bcm_field_qualify_IpType(unit, ent, bcmFieldIpTypeIpv4Any);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_IpType\n");
          return result;
      }

      /* UDP packets */
      result = bcm_field_qualify_IpProtocolCommon(unit, ent, bcmFieldIpProtocolCommonUdp);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_IpType\n");
          return result;
      }

      /* IP termination hit */
      result = bcm_field_qualify_MplsTerminated(unit, ent, 0x1, 0x1);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_MplsTerminated\n");
          return result;
      }

      dqData[0] = udp_port_vxlan & 0xFF;
      dqData[1] = (udp_port_vxlan >> 8) & 0xFF;
      result = bcm_field_qualify_data(unit, ent, qual.qual_id,
                                      &(dqData[0]),
                                      &(dqMask[0]),
                                      2 /* len here always in bytes */);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_data\n");
          return result;
      }

      if (entry_ndx == 0 /* Snoop */) {
          /* SIP lookup fails */
          result = bcm_field_qualify_TunnelTerminated(unit, ent, 0x0, 0x1);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qualify_TunnelTerminated\n");
              return result;
          }

          result = bcm_field_action_add(unit, ent, bcmFieldActionSnoop, snoop_cmnd, 0);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_action_add\n");
              return result;
          }
      }
      else {
          /* Trap */
          /* SIP lookup fails */
          result = bcm_field_qualify_IpTunnelHit(unit, ent, 0x0, 0x1);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qualify_IpTunnelHit\n");
              return result;
          }

          result = bcm_field_action_add(unit, ent, bcmFieldActionTrap, gport_trap, 0);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_action_add\n");
              return result;
          }
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



/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_field_copy_pcp_to_dscp_for_ip_packets.c
 * Purpose: PCP to DSCP mapping for IP packets example.
 *
 *  
 * Run Script:
   cint ../sand/utility/cint_sand_utils_global.c
   cint cint_multi_device_utils.c
   cint utility/cint_utils_l3.c
   cint cint_ip_route.c
   cint cint_qos_l3_rif_cos.c
   cint cint_field_copy_pcp_to_dscp_for_ip_packets.c
   cint
   // print ip_pcp_dscp(@unit, @port)
   print ip_pcp_dscp(unit, 13);
  
 * 1. Configure InRIF, OutRIF and routing from InRIF to OutRIF 
 * using basic_example_single_unit from cint_ip_route.c (for more info 
 * see basic_example in cint_ip_route.c). 
 *   
 * 2. Configure InRIF to map PCP to DSCP for IP packets:
 * [ARAD] 
 *   Configure PCP to DSCP mapping for all IPv4 packets (not
 *   just for InRIF but for all IPv4 packets).
 *   The configuration is made using the field processor.
 * [ARAD+]
 *   Create a QoS profile that maps PCP to DSCP,
 *   and configure InRIF to use it.
 *   The mapping is set only for InRIF.
 *   In Arad+ the DSCP is taken from PCP-DEI (4 bits)
 *   instead of PCP (in Arad).
 *   To map the PCP without the DEI into the DSCP,
 *   the DSCP remarking is set to map a PCP-DEI value
 *   to half the value.
 *  
 * REMARK 
 *   In both Arad and Arad+ the PCP to DSCP mapping works by
 *   passing through the TC.
 *   A TC to DSCP mapping is configured.
 *   When Ethernet packets arrive, the PCP is mapped to TC
 *   and that in turn is mapped to DSCP.
 *  
 * TRAFFIC 
 * The expected traffic on @in_port are IP packets with the following 
 * header values: 
 * 1) ETH, VLAN=15
 * 2) ETH, DA=00:0c:00:02:00:00 
 * 3) IP, DIP = 0x7fffff03 (127.255.255.03) 
 *  
 * The resulting traffic on @out_port is IP packets with the 
 * DSCP set to the value of the PCP in the sent Ethernet header.
 */

int verbose = 2;

/*
 *  Set the Direct mapping Field group that:
 *  - Matches IP-forwarded packets and each value of PCP
 *  - If match, set DSCP = PCP
 */
int ip_pcp_dscp_field_group_set(/* in */ int unit,
                                /* in */ int group_priority,
                                /* out */ bcm_field_group_t *group) 
{
  int result;
  int auxRes;
  bcm_field_group_config_t grp;
  bcm_field_aset_t aset;
  bcm_field_entry_t ent;
  int entry_iter;
  int nof_entries = 4; /* 4 Forwarding types */
  bcm_field_ForwardingType_t forwarding_type;
  bcm_field_extraction_field_t ext_pcp; /* 1x1 PCP copy */
  bcm_field_extraction_action_t extract;


  if (NULL == group) {
    printf("Pointer to group ID must not be NULL\n");
    return BCM_E_PARAM;
  }

  /*
   *  The Field group is done at the ingress stage
   *  The Field group qualifiers are Forwarding-Type and
   *  PCP 
   */
  bcm_field_group_config_t_init(&grp);
  grp.group = -1;


  /* Define the QSET */
  BCM_FIELD_QSET_INIT(grp.qset);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyForwardingType);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyOuterVlanPri);

  /*
   *  This Field Group can change the DSCP
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionDscpNew);

  /*  Create the Field group */
  grp.priority = group_priority;
  grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
  grp.mode = bcmFieldGroupModeDirectExtraction;
  result = bcm_field_group_config_create(unit, &grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      auxRes = bcm_field_group_destroy(unit, grp.group);
      return result;
  }

  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp.group, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set\n");
    auxRes = bcm_field_group_destroy(unit, grp.group);
    return result;
  }

  /*
   *  Add multiple entries to the Field group.
   * 
   *  Match the packets on the Forwarding = IPv4 / IPv6
   *  Unicast / Multicast
   */
  for (entry_iter = 0; entry_iter < nof_entries; entry_iter++) {
      result = bcm_field_entry_create(unit, grp.group, &ent);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
          auxRes = bcm_field_group_destroy(unit, grp.group);
          return result;
      }

      /* 
       * Forwarding type: 4 groups of 8 entries 
       * with distinct Forwarding types
       */
      if (entry_iter == 0) {
          forwarding_type = bcmFieldForwardingTypeIp4Ucast;
      }
      else if (entry_iter == 1) {
          forwarding_type = bcmFieldForwardingTypeIp4Mcast;
      }
      else if (entry_iter == 2) {
          forwarding_type = bcmFieldForwardingTypeIp6Ucast;
      }
      else {/* if (entry_iter == 3) */
          forwarding_type = bcmFieldForwardingTypeIp6Mcast;
      }

      result = bcm_field_qualify_ForwardingType(unit, ent, forwarding_type);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_ForwardingType\n");
        auxRes = bcm_field_group_destroy(unit, grp.group);
        return result;
      }

      /*
       *  A matched frame changes the Dscp
       *  The mapping here is Dscp = VLAN-Priority if the
       *  Forwarding type is as above
       */
      bcm_field_extraction_action_t_init(&extract);
      bcm_field_extraction_field_t_init(&ext_pcp);
      /* Build the action to change the DSCP without Base-Value */
      extract.action = bcmFieldActionDscpNew;
      extract.bias = 0;
      /* Indicate that the 1st bits (actually all) are from the PCP[2:0] */
      ext_pcp.flags = 0;
      ext_pcp.bits = 3;
      ext_pcp.lsb = 0;
      ext_pcp.qualifier = bcmFieldQualifyOuterVlanPri;
      result = bcm_field_direct_extraction_action_add(unit,
                                                      ent,
                                                      extract,
                                                      1 /* count */,
                                                      &ext_pcp);

      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_direct_extraction_action_add\n");
        auxRes = bcm_field_group_destroy(unit, grp.group);
        return result;
      }
  }

  /*
   *  Commit the entire group to hardware.  We could do individual entries,
   *  but since we just created the group here and want the entry/entries all
   *  in hardware at this point, it is quicker to commit the whole group.
   */
  result = bcm_field_group_install(unit, grp.group);
  if (BCM_E_NONE != result) {
    auxRes = bcm_field_group_destroy(unit, grp.group);
    return result;
  }
  
  /*
   *  Everything went well; return the group ID that we allocated earlier.
   */
  *group = grp.group; 

  return result;
}



/*
 *  Set the previous configurations for an example configuration
 *  If match by a Field entry, snoop the packet to port 1
 */
int ip_pcp_dscp_field_group_example(int unit) 
{
  int result;
  int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
  bcm_field_group_t group;


 result = ip_pcp_dscp_field_group_set(unit, group_priority, &group);
  if (BCM_E_NONE != result) {
      printf("Error in ip_pcp_dscp_field_group_set\n");
      return result;
  }

  return result;
}

int ip_pcp_dscp_field_group_set_teardown(/* in */ int unit,
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

int ip_pcp_dscp(int unit, int in_port, int out_port)
{
    int qos_map_id;
    int rv = BCM_E_NONE;
    int tc_idx;
    int pcp_dei;
    int pcp_dei_max = 15;
    bcm_vlan_t inrif_vlan = 15;
    bcm_qos_map_t qos_map;
    bcm_l3_ingress_t l3_ing_if;
    /* We assume that InRIF id == InRIF vlan. */
    bcm_if_t inrif_id = inrif_vlan;

    if (!is_device_or_above(unit,ARAD_PLUS)) {
        rv = ip_pcp_dscp_field_group_example(unit);
        if (rv) {
            printf("Error configuring device to copy pcp to dscp.\n");
            print rv;
            return rv;
        }
    } else {
        /* Create an ingress QoS profile */
        if (verbose >= 1) {
            printf("Creating an ingress QoS profile...\n");
        }
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &qos_map_id);
        if (rv) {
            printf("Error doing bcm_qos_map_create for flags BCM_QOS_MAP_INGRESS.\n");
            print rv;
            return rv;
        }

        if (verbose >= 1) {
            printf("qos profile %d created.\n", qos_map_id);
            printf("Configuring profile %d to perform pcp to dscp mapping.\n", qos_map_id);
        }

        /* Configure qos profile to copy PCP-DEI to DSCP. */
        bcm_qos_map_t_init(&qos_map);
        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3_L2, &qos_map, qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("Error configuring qos profile to perform PCP-DEI to In-DSCP mapping:\n");
            printf("Error doing bcm_qos_map_add(%d,BCM_QOS_MAP_L3_L2,NULL,map_id=%d).\n", unit, qos_map_id);
            print rv;
            return rv;
        }

        if (rv != BCM_E_NONE) {
            printf("Error doing bcm_qos_map_add(unit,0,map=<tc=%d,dscp=%d>,map_id=%d).\n", tc_idx, tc_idx, qos_map_id);
            print rv;
            return rv;
        }

        /* For all PCP-DEI values, configure the DSCP remarking to halve the value to get rid of the DEI bit. */
        if (verbose >= 1) {
            printf("Configuring DSCP remarking for profile %d to use only the PCP bits and ignore the DEI bit.\n", qos_map_id);
        }

        for (pcp_dei = 0; pcp_dei <= pcp_dei_max; pcp_dei++) {
            qos_map.dscp = pcp_dei;
            qos_map.remark_int_pri = pcp_dei / 2;
            rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3, &qos_map, qos_map_id);
            if (rv != BCM_E_NONE) {
                printf("Error configuring DSCP remarking for qos profile %d:\n", qos_map_id);
                printf("Error doing bcm_qos_map_add(%d,BCM_QOS_MAP_L3,qos_map=<int_pri=%d,dscp=%d>,map_id=%d).\n", 
                       unit, qos_map.int_pri, qos_map.dscp, qos_map_id);
                print rv;
                return rv;
            }
        }
    }
    if (verbose >= 1) {
        printf("Configuring routing...\n");
    }

    /* Configure routing. */
    basic_example_single_unit(unit, in_port, out_port);
    if (rv) {
        printf("Error - Could not configure routing.\n");
        print rv;
        return rv;
    }

    if (verbose >= 1) {
        printf("Setting QoS map to InRIF...\n", inrif_id);
    }

    l3_ing_if.urpf_mode = bcmL3IngressUrpfDisable;
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
    l3_ing_if.qos_map_id = qos_map_id;

    /* Set the qos mapping to the InRIF. */
    rv = bcm_l3_ingress_create(unit, &l3_ing_if, &inrif_id);
    if (rv) {
        printf("Error in bcm_l3_ingress_create.\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

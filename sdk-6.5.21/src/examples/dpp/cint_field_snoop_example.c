
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 *  Set a Snoop profile to send to some destination
 *  local port. (Alternatively, a system port can be the
 *  destination of the mirror port).
 *  We receive as an output the snoop gport. This gport
 *  will be the parameter of snoop action  
 */

int ip_snoop_set(int unit, bcm_gport_t *snoop_trap_gport_id_p)
{
    int result;
    int auxRes;
    int flags = 0; /* Do not specify any ID for the snoop command / trap */
    int snoop_command; /* Snoop command */
    int trap_id;
    int trap_dest_strength = 0; /* No influence on the destination update */
    int trap_snoop_strength = 3; /* Strongest snoop strength for this trap */
    bcm_rx_snoop_config_t snoop_config; /* Snoop attributes */
    bcm_rx_trap_config_t trap_config;
    bcm_gport_t snoop_trap_gport_id;
    bcm_gport_t local_gport_id;
    int port_id;
    /*
     * Create a Snoop Command
     * The snoop command defines the attribute of the Snoop
     */
    result = bcm_rx_snoop_create(unit, flags, &snoop_command); /* Receive a snoop command handle */
    if (BCM_E_NONE != result) {
        printf("Error in bcm_rx_snoop_create\n");
      auxRes = bcm_rx_snoop_destroy(unit, flags, snoop_command);
      return driverConvertBcmRv(result);
    }

    /*
     * Specify the snoop attributes
     * Here, we set the destination and the probability to 100%
     * Snoop the whole packet
     */
    /* Initialize the structure */
    bcm_rx_snoop_config_t_init(&snoop_config);
    snoop_config.flags |= BCM_RX_SNOOP_UPDATE_DEST/* | BCM_RX_TRAP_REPLACE */;

    port_id = 200;
    BCM_GPORT_LOCAL_SET(local_gport_id, port_id);
    printf("Local GPORT:0x%x Port:%d flags:0x%x\n", local_gport_id, port_id, snoop_config.flags);
    snoop_config.dest_port = local_gport_id;
    snoop_config.size = -1; /* Full packet snooping */
    snoop_config.probability= 100000; /* 100000 is 100.000% */

    /*Set snoop configuration*/
    result = bcm_rx_snoop_set(unit, snoop_command, &snoop_config);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_rx_snoop_set\n");
        auxRes = bcm_rx_snoop_destroy(unit, flags, snoop_command);
        return driverConvertBcmRv(result);
    }

    /*
     * Create a User-defined trap for snooping
     */
    result = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &trap_id);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_type_create\n");
        auxRes = bcm_rx_snoop_destroy(unit, flags, snoop_command);
        auxRes = bcm_rx_trap_type_destroy(unit, trap_id);
        return driverConvertBcmRv(result);
    }

    /*
     * Configure the trap to the snoop command
     */
    bcm_rx_trap_config_t_init(&trap_config);

    /*for port dest change*/
    trap_config.flags |= BCM_RX_TRAP_REPLACE;
    trap_config.trap_strength = trap_dest_strength;
    trap_config.snoop_cmnd = snoop_command; /* Snoop any frame matched by this trap */
    trap_config.snoop_strength = trap_snoop_strength;

    result = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_set\n");
        auxRes = bcm_rx_snoop_destroy(unit, flags, snoop_command);
        auxRes = bcm_rx_trap_type_destroy(unit, trap_id);
        return driverConvertBcmRv(result);
    }

    /* Get the trap gport to snoop */
    BCM_GPORT_TRAP_SET(snoop_trap_gport_id, trap_id, trap_dest_strength, trap_snoop_strength);
    *snoop_trap_gport_id_p = snoop_trap_gport_id;

    printf("Trap ID:%d Trap GPORT:0x%x\n", trap_id, snoop_trap_gport_id);

    return BCM_E_NONE;
}

/*
 *  Set the Field group that:
 *  - Match UDPoIPoEth packets, routed by the IP Host table,
 *  and whose UDP Port is in a specific range
 *  - If match, snoop the packet and modify its Traffic Class  
 */
int ip_field_group_set(/* in */ int unit,
                        /* in */ int group_priority,
                        /* in */ bcm_port_t in_port,
                        /* in */ bcm_gport_t snoop_trap_gport_id,
                        /* out */ bcm_field_group_t *group) 
{
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  bcm_field_entry_t ent;

  if (NULL == group) {
    printf("Pointer to group ID must not be NULL\n");
    return BCM_E_PARAM;
  }

  /*
   *  Build qualifier set for the group we want to create.  Entries must exist
   *  within groups, and groups define what kind of things the entries can
   *  match, by their inclusion in the group's qualitier set.
   * 
   *  The Field group is done at the ingress stage
   *  The Field group qualifiers are EtherType (to match only IPv4 packets),
   *  IP-Protocol (UDP as L4), L4-Port range, and
   *  hit in the Destination Host Table for specific Port. 
   */

  /* Define the QSET */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
/*  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort); */

  /*
   *  Build the action set for this Field group.  Entries can only do
   *  something that is included on this list.
   * 
   *  This Field Group can perform Snooping and change the Internal
   *  priority
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionSnoop);

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
      auxRes = bcm_field_range_destroy(unit, range);
    auxRes = bcm_field_group_destroy(unit, grp);
    return result;
  }

  /*
   *  Add multiple entries to the Field group.
   * 
   *  Match the packets on the EtherType = 0x0800 (IPv4 packets),
   *  IpProtocol = 0x11 (UDP), Range of the UDP Destination Ports,
   *  and Host-Destination table hit
   */
  result = bcm_field_entry_create(unit, grp, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
    auxRes = bcm_field_group_destroy(unit, grp);
    auxRes = bcm_field_range_destroy(unit, range);
    return result;
  }

  /* IPv4 EtherType */
  result = bcm_field_qualify_EtherType(unit, ent, 0x0800, 0xFFFF);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_EtherType\n");
    auxRes = bcm_field_group_destroy(unit, grp);
    auxRes = bcm_field_range_destroy(unit, range);
    return result;
  }

  /* In-Port according to the input */
/*  result = bcm_field_qualify_InPort(unit, ent, in_port, -1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_InPort\n");
    auxRes = bcm_field_group_destroy(unit, grp);
    auxRes = bcm_field_range_destroy(unit, range);
    return result;
  }
*/

  /*
   *  A matched frame is snooped to the defined gport.
   */
  result = bcm_field_action_add(unit, ent, bcmFieldActionSnoop, snoop_trap_gport_id, 0);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_action_add\n");
    auxRes = bcm_field_group_destroy(unit, grp);
    auxRes = bcm_field_range_destroy(unit, range);
    return result;
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

bcm_rx_t driverPktReceive(int unit, bcm_pkt_t* pkt, void* cookie)
{
    int i;
    uint8 *data;
    printf("Packet received with pkt->rx_port:%hhu pkt->src_port:%hhu\n", pkt->rx_port, pkt->src_port);
    printf("Packet received with pkt->pkt_len:%d pkt->tot_len:%d pkt->fwd_hdr_offset:%d\n\t", pkt->pkt_len, pkt->tot_len, pkt->fwd_hdr_offset);

    data = pkt->pkt_data->data;

    for(i = 0; i < pkt->tot_len; i++) {
            if ((i % 4) == 0)
                printf(" ");
            if ((i % 32) == 0)
                printf("\n\t");

            printf("%02hhx", (pkt->_pkt_data.data)[i]);
    }

    printf("\n\n");

    return 3;
}

int rx_activate(int unit)
{

    /* RX Configuration Structure */
    bcm_rx_cfg_t rx_cfg;
    int rc;

    bcm_rx_cfg_t_init(&rx_cfg);
    rx_cfg.pkt_size = 16*1024;
    rx_cfg.pkts_per_chain = 15;
    rx_cfg.global_pps = 0;
    rx_cfg.max_burst = 200;
     /* rx_cfg.flags |= BCM_RX_F_CRC_STRIP;  */     /* Strip CRC from incoming packets */
    rx_cfg.chan_cfg[1].chains = 4;
    rx_cfg.chan_cfg[1].cos_bmp = 0xFFFFFFff;             /* Packets with all CoS values come through this channel */

    /*
     * Per unit settings go in this loop.
     */
    /* Register to receive packets punted to CPU */
    rc = bcm_rx_register(unit, "test", driverPktReceive, 1, NULL, BCM_RCO_F_ALL_COS);
    if (BCM_FAILURE(rc))
      {
          printf("Failed to register packet receive callback for unit %d, rc = %d (%s).\r\n", unit, rc, bcm_errmsg(rc));
        }

    if (!bcm_rx_active(unit)) {
          rc = bcm_rx_start(unit, &rx_cfg);
          if (BCM_FAILURE(rc)) {
            printf("bcm_rx_start() failed on unit %d with rc %d (%s).\r\n", unit, rc, bcm_errmsg(rc));
          }
    }
    return 0;
}

/*
 *  Set the previous configurations for an example configuration
 *  If match by a Field entry, snoop the packet to port 1
 */
int ip_example(int unit) 
{
  int result;
  int dest_local_port = 1;
  bcm_gport_t snoop_trap_gport_id;
  int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
  bcm_port_t in_port = 200;
  bcm_field_group_t group;


  /* Set the snoop destination */
  result = ip_snoop_set(unit, &snoop_trap_gport_id);
  if (BCM_E_NONE != result) {
      printf("Error in ip_snoop_set\n");
      return result;
  }

  result = ip_field_group_set(unit, group_priority,
                             in_port, snoop_trap_gport_id, &group);
  if (BCM_E_NONE != result) {
      printf("Error in ip_field_group_set\n");
      return result;
  }

  rx_activate(unit);
  return result;
}

int ip_snoop_teardown(/* in */ int unit,
                        /* in */ bcm_gport_t snoop_trap_gport_id,
                        /* in */ bcm_field_group_t group) 
{
  int result;
  int auxRes;
  bcm_rx_trap_config_t trap_config;
  int trap_id;

  /* Destroy the Field group (and all its entries) */  
  result = bcm_field_group_destroy(unit, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_destroy\n");
      return result;
  }

  /* Destroy the snoop destination */

    /* Get the trap id and its atributes */
    trap_id = BCM_GPORT_TRAP_GET_ID(snoop_trap_gport_id);
    result = bcm_rx_trap_get(unit, trap_id, &trap_config);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_rx_trap_get\n");
        return result;
    }

    /* Destroy the snoop command */
    result = bcm_rx_snoop_destroy(unit, 0, trap_config.snoop_cmnd);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_rx_snoop_destroy\n");
        return result;
    }

    /* Destroy the User-Defined trap type */
    result = bcm_rx_trap_type_destroy(unit, trap_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_rx_trap_type_destroy\n");
        return result;
    }


  return result;
}



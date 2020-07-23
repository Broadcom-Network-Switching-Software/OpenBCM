/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*-----------------------------------------------------------------------------------------

  Snooping

-----------------------------------------------------------------------------------------*/


/*
 *  Set a Snoop profile to send to some destination
 *  local port. (Alternatively, a system port can be the
 *  destination of the mirror port).
 *  We receive as an output the snoop gport. This gport
 *  will be the parameter of snoop action  
 */
int snoop_trap_id;
int snoop_command;                        /* Snoop command */
bcm_field_entry_t snoop_ent;

int two_pass_snoop_set( int unit, bcm_rx_trap_t type, int dest_local_port, bcm_gport_t *snoop_trap_gport_id) {

  int result;
  int auxRes;
  int flags = 0;                            /* Do not specify any ID for the snoop command / trap */
  int trap_dest_strength = 0;               /* No influence on the destination update */
  int trap_snoop_strength = 3;              /* Strongest snoop strength for this trap */
  bcm_rx_snoop_config_t snoop_config;       /* Snoop attributes */
  bcm_rx_trap_config_t trap_config;

  int snp_g_id;
  
  /* 
   * Create a Snoop Command 
   * The snoop command defines the attribute of the Snoop 
   */ 
  printf("============---------------- Creating snoop and trap ---------------================\n");
  result = bcm_rx_snoop_create(unit, flags, &snoop_command); /* Receive a snoop command handle */
  if (result != BCM_E_NONE) {
      printf("Error in bcm_rx_snoop_create\n");
      return result;
  }
  printf("\t Snoop command %x \n",snoop_command );
    /*
      Specify the snoop attributes
      Here, we set the destination and the probability to 100% 
      Snoop the whole packet */
 
  /* Initialize the structure */
  bcm_rx_snoop_config_t_init(&snoop_config);
  /*  snoop_config.flags |= BCM_RX_SNOOP_UPDATE_DEST;*/
   snoop_config.flags = (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_REPLACE);

  /* Set the Mirror destination to go to the Destination local port*/
  BCM_GPORT_LOCAL_SET(snoop_config.dest_port, dest_local_port);

  /*  sal_memset(&snoop_config, 0, sizeof(snoop_config));*/
  /* snoop_config.dest_port = dest_local_port; */
  snoop_config.size = -1;               /* Full packet snooping */
  snoop_config.probability= 100000;     /* 100000 is 100.000% */
  /* snoop_config.dest_group = dest_local_port ;*/

  /*Set snoop configuration*/
  result = bcm_rx_snoop_set(unit, snoop_command, &snoop_config);
  if (result != BCM_E_NONE) {
      printf("Error in bcm_rx_snoop_set\n");
      return result;
  }
  print snoop_config;

  /* 
   * Create a User-defined trap for snooping
   */
  printf("\t User trap \n");
  result = bcm_rx_trap_type_create(unit, flags, type, &snoop_trap_id);
  if (result != BCM_E_NONE) {
      printf("Error in bcm_rx_trap_type_create\n");
      return result;
  }
  printf("\t User trap 0x%x \n",snoop_trap_id);
  /* 
   * Configure the trap to the snoop command 
   */ 
  bcm_rx_trap_config_t_init(&trap_config);
  
  /*for port dest change*/
  trap_config.flags |= BCM_RX_TRAP_REPLACE; 
  trap_config.trap_strength = trap_dest_strength; 
  trap_config.snoop_cmnd = snoop_command; /* Snoop any frame matched by this trap */
  trap_config.snoop_strength = trap_snoop_strength; 

  result = bcm_rx_trap_set(unit, snoop_trap_id, &trap_config);
  if (result != BCM_E_NONE) {
      printf("Error in bcm_rx_trap_set\n");
      return result;
  }

  /* Get the trap gport to snoop */
  BCM_GPORT_TRAP_SET(snoop_trap_gport_id, snoop_trap_id, trap_dest_strength, trap_snoop_strength);
  snp_g_id = *snoop_trap_gport_id;
  printf("Snoop gport id 0x%x \n",snp_g_id);
  printf("============------------------------------------------------------================\n");

  return result;
}

int two_pass_snoop_unset( int unit) {

  int result;
  int flags = 0;                            /* Do not specify any ID for the snoop command / trap */

  
  result =  bcm_rx_snoop_destroy(unit, flags, snoop_command);
  if (result != BCM_E_NONE) {
     printf("Error, in snoop destroy\n");
     return result;
  }

  result = bcm_rx_trap_type_destroy(unit,snoop_trap_id);
  if (result != BCM_E_NONE) {
      printf("Error, in snoop trap destroy\n");
      return result;
  }

  
  return result;
}


/* 
 *  Define the Egress Field group, with Key: {Source-System-Port, Out-Port}
 *  and actions {Redirect, Set internal-data}
 */
int system_port_lookup_setup(/* in */ int unit,
                             /* in */ int group_priority,
                             /* in */ bcm_field_group_t group) {
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  char *proc_name ;

  proc_name = "system_port_lookup_setup" ;
  printf("%s(): Enter. group_priority %d group %d\r\n",proc_name,group_priority,group) ;

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort); /* Local Out-Port */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort); /* Source-System-Port */

  /*
   *  Create the group
   */
  result = bcm_field_group_create_id(unit, qset, group_priority, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      return result;
  }
  printf("%s(): After bcm_field_group_create_id\n",proc_name);
 
  /*
   *  Commit the entire group to hardware.  
   */
  result = bcm_field_group_install(unit, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
    return result;
  }
  printf("%s(): After bcm_field_group_install\n",proc_name);
  printf("%s(): Exit. group_priority %d group %d\r\n",proc_name,group_priority,group) ;
  return result;
}

/* 
 *  Define the Egress Field group, with Key: {Source-System-Port, Out-Port, bcmFieldQualifyFheiSize, bcmFieldQualifyInVPort}
*  and actions {Redirect, Set internal-data}
*/
int system_port_fhei_lookup_setup(/* in */ int unit,
                             /* in */ int group_priority,
                             /* in */ bcm_field_group_t group) {
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  char *proc_name ;
 
  proc_name = "system_port_lookup_setup" ;
  printf("%s(): Enter. group_priority %d group %d\r\n",proc_name,group_priority,group) ;
 
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort); /* Local Out-Port */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort); /* Source-System-Port */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyFheiSize); /* Fhei.Size */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInVPort); /* In-lif */
 
 
  /*
   *  Create the group
   */
 result = bcm_field_group_create_id(unit, qset, group_priority, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      return result;
  }
  printf("%s(): After bcm_field_group_create_id\n",proc_name);
  /*
   *  Commit the entire group to hardware.  
   */
  result = bcm_field_group_install(unit, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
    return result;
  }
  printf("%s(): After bcm_field_group_install\n",proc_name);
  printf("%s(): Exit. group_priority %d group %d\r\n",proc_name,group_priority,group) ;
  return result;
}
 
/* 
 *  Add a new rule: Redirect to the current OutPort,
 *  Internal-Data = InPort (local port), where the InPort is
 *  part of the local ports of this device.
 *  We retrieve its System-Port and set it as Source-System-Port
 *  in the matching rule.
 */
int system_port_lookup_entry_add(/* in */ int unit,
                                 /* in */ bcm_field_group_t group,
                                 /* in */ bcm_port_t in_port, 
                                 /* in */ bcm_port_t out_port) {
  int result;
  int auxRes;
  bcm_field_entry_t ent;
  bcm_gport_t system_port;
  int statId;
  bcm_gport_t local_gport;
  unsigned long local_gport_as_long ;
  int action_stat ;
  int cores_num, ii ;
  char *proc_name ;
  bcm_field_action_core_config_t core_config_arr[SOC_DPP_DEFS_MAX_NOF_CORES()] ;
  int core_config_arr_len ;

  /* Translate the port from logical port to pp port */
  uint32 dummy_flags ;
  bcm_port_interface_info_t interface_info ;
  bcm_port_mapping_info_t mapping_info ;
  result = bcm_port_get(unit, in_port, &dummy_flags, &interface_info, &mapping_info);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_port_get in_port\n");
      return result;
  }
  bcm_port_t in_pp_port = mapping_info.tm_port;

  /* Retrieve out_port's tm_port and modid */
  result = bcm_port_get(unit, out_port, &dummy_flags, &interface_info, &mapping_info);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_port_get out_port\n");
      return result;
  }

  proc_name = "system_port_lookup_entry_add" ;
  printf("%s(): Enter. group %d in_port %d out_port %d\r\n",proc_name,group,in_port,out_port) ;

  cores_num = SOC_DPP_DEFS_GET_NOF_CORES(unit);

  result = bcm_field_entry_create(unit, group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  printf("%s(): Created field entry %d\r\n",proc_name,ent) ;
  result = bcm_field_qualify_DstPort(unit, ent, mapping_info.core, -1, mapping_info.tm_port, -1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_DstPort\n");
    return result;
  }

  /*
   * Retrieve the Source-System-Port and match it
   */
  result = bcm_stk_gport_sysport_get (unit, in_port, &system_port);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_stk_gport_sysport_get\n");
    return result;
  }
  result = bcm_field_qualify_SrcPort(unit, ent, 0, -1, system_port, -1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_SrcPort\n");
    return result;
  }

  /* Redirect to same out-port */
  BCM_GPORT_LOCAL_SET(local_gport, out_port);

  /*
   * Due to HW reasons, the Counter-ID can be only at egress between 1024 and 
   * 3839. Besides, the user must re-indicate the redirect gport as 
   * second parameter. 
   *  
   * When redirecting at egress without counting, the user must still 
   * set the stat function with stat-id equal to 0 (do not count) 
   *  
   * When trying to set different PP-Ports for the same recycling port, 
   * the configured Stat-ID must be different, for example being 
   * initialized to 1024 + entry-id. 
   * It implies that the Egress-Receive Counter-Pointer is set to this value. 
   */
  statId = (in_pp_port << 19); /* Set internal data = in-local-port */

  action_stat = bcmFieldActionStat ;
  local_gport_as_long = local_gport ;

  if (is_device_or_above(unit, JERICHO)) {
    /*
     * Enter for Jericho and up
     */
    /*
     * Request 'counter + pp-port redirect' (param2=-1) and set the counter at the same location as in ARAD:
     * For Arad, when the LS 19 bits are zero, on statId, selected counter will be as the destination port on the
     * last 256 entries of the Ace table (which is 4K entries long).
     */
    statId |= (4*1024 - 256 - 1 + out_port) ;
  
    printf("%s(): JERICHO: statId %d (0x%08lX) in_port %d out_port %d\r\n",proc_name,statId,statId,in_port,out_port) ;

    for (ii = 0 ; ii < cores_num ; ii++) {
      core_config_arr[ii].param0 = statId ;
      core_config_arr[ii].param1 = local_gport ;
      core_config_arr[ii].param2 = BCM_ILLEGAL_ACTION_PARAMETER ;
    }
    printf("%s(): Call bcm_field_action_config_add(ent %d bcmFieldActionStat %d\r\n",proc_name,ent,action_stat) ;
    printf("==> statId %d local_gport %d (0x%08lX))\r\n",statId,local_gport,local_gport_as_long) ;
    /*
     * Setting 'core_config_arr_len' to '1' results in setting all cores the same way.
     */
    core_config_arr_len = cores_num ;
    result = bcm_field_action_config_add(unit, ent, bcmFieldActionStat, core_config_arr_len, &core_config_arr[0]);
  } else {
    /*
     * Enter for Arad and Arad+
     */
    printf("%s(): ARAD: statId %d (0x%08lX) in_port %d out_port %d\r\n",proc_name,statId,statId,in_port,out_port) ;
    printf("%s(): Call bcm_field_action_add(ent %d bcmFieldActionStat %d statId %d local_gport %d (%08lX))\r\n",proc_name,ent,action_stat,statId,local_gport,local_gport_as_long) ;

    result = bcm_field_action_add(unit, ent, bcmFieldActionStat, statId, local_gport);
  }
  if (BCM_E_NONE != result) {
      if (is_device_or_above(unit, JERICHO)) {
          printf("%s(): Error in bcm_field_action_config_add\n",proc_name);
      } else {
          printf("%s(): Error in bcm_field_action_add\n",proc_name);
      }
    return result;
  }
  if (is_device_or_above(unit, JERICHO)) {
      printf("%s(): Called bcm_field_action_config_add(ent %d bcmFieldActionStat %d statId %d local_gport %d (%08lX))\r\n",proc_name,ent,action_stat,statId,local_gport,local_gport_as_long) ;
  } else {
      printf("%s(): Called bcm_field_action_add(ent %d bcmFieldActionStat %d statId %d local_gport %d (%08lX))\r\n",proc_name,ent,action_stat,statId,local_gport,local_gport_as_long) ;
  }

  /*
   *  Commit the entire group to hardware.  We could do individual entries,
   *  but since we just created the group here and want the entry/entries all
   *  in hardware at this point, it's a bit quicker to commit the whole group.
   */
  result = bcm_field_group_install(unit, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
    return result;
  }

  /*
   * Original was: configure manually the ace type: 1 (bit 8)
   * Take this clause out. No such manual setting is acceptable.
   */
  /*
  if (is_device_or_above(unit, JERICHO)) {
      bshell(unit, "mod PPDB_A_TCAM_ACTION[6] 0 1 ACTION=0x3c9d00");
  }
  */

  bshell(unit, "dump chg PPDB_A_TCAM_ACTION[6]");

  printf("%s(): Exit. group %d in_port %d out_port %d\r\n",proc_name,group,in_port,out_port) ;
  return result;
}
 
 
 
/* 
 *  Add a new rule: Redirect to the current OutPort,
*  Internal-Data = InPort (local port), where the InPort is
*  part of the local ports of this device.
*  We retrieve its System-Port and set it as Source-System-Port
*  in the matching rule.
*  for FHEI.size=3 & inlif != 0 drop packet
*/
int system_port_fhei_lookup_entry_add(/* in */int unit,
                                      /* in */ bcm_field_group_t group,
                                      /* in */ bcm_port_t in_port,
                                      /* in */ bcm_port_t out_port,
                                      /* in */ int fhei_size_port,
                                      /* in */ int in_lif) {
    int result;
    int auxRes;
    bcm_field_entry_t ent;
    bcm_gport_t system_port;
    int statId;
    bcm_gport_t local_gport;
    unsigned long local_gport_as_long;
    int action_stat;
    int cores_num, ii;
    char *proc_name;
    bcm_field_action_core_config_t core_config_arr[SOC_DPP_DEFS_MAX_NOF_CORES()];
    int core_config_arr_len;
 
    /* Translate the port from logical port to pp port */
    uint32 dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int port_mask = -1;
 
    result = bcm_port_get(unit, in_port, &dummy_flags, &interface_info, &mapping_info);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_port_get in_port\n");
        return result;
    }
    bcm_port_t in_pp_port = mapping_info.tm_port;
 
    /* Retrieve out_port's tm_port and modid */
    result = bcm_port_get(unit, out_port, &dummy_flags, &interface_info, &mapping_info);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_port_get out_port\n");
        return result;
    }
 
    proc_name = "system_port_lookup_entry_add";
    printf("%s(): Enter. group %d in_port %d out_port %d\r\n", proc_name, group, in_port, out_port);
 
    cores_num = SOC_DPP_DEFS_GET_NOF_CORES(unit);
 
    result = bcm_field_entry_create(unit, group, &ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }
 
    printf("%s(): Created field entry %d\r\n", proc_name, ent);
    result = bcm_field_qualify_DstPort(unit, ent, mapping_info.core, -1, mapping_info.tm_port, -1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_DstPort\n");
        return result;
    }
    /*No need to qualify according to src port for Routing over over IP-Tunnel because packet should be dropped */
    if ((fhei_size_port == 3) && (in_lif > 0)) {
        port_mask = 0;
    }
    /*
     * Retrieve the Source-System-Port and match it
     */
    result = bcm_stk_gport_sysport_get(unit, in_port, &system_port);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_stk_gport_sysport_get\n");
        return result;
    }
    if (port_mask != 0) {
        result = bcm_field_qualify_SrcPort(unit, ent, 0, -1, system_port, port_mask);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_SrcPort\n");
            return result;
        }
    }
 
    result = bcm_field_qualify_FheiSize(unit, ent, fhei_size_port, 0xffffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_FheiSize\n");
        return result;
    }
    result = bcm_field_qualify_InVPort32(unit, ent, in_lif, 0xffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_FheiSize\n");
        return result;
    }
 
 
    /* Redirect to same out-port */
    BCM_GPORT_LOCAL_SET(local_gport, out_port);
 
    /*
     * Due to HW reasons, the Counter-ID can be only at egress between 1024 and 
     * 3839. Besides, the user must re-indicate the redirect gport as 
     * second parameter. 
     *  
     * When redirecting at egress without counting, the user must still 
     * set the stat function with stat-id equal to 0 (do not count) 
     *  
     * When trying to set different PP-Ports for the same recycling port, 
     * the configured Stat-ID must be different, for example being 
     * initialized to 1024 + entry-id. 
     * It implies that the Egress-Receive Counter-Pointer is set to this value. 
     */
    statId = (in_pp_port << 19); /* Set internal data = in-local-port */
 
    action_stat = bcmFieldActionStat;
    local_gport_as_long = local_gport;
 
    /*Routing over over IP-Tunnel packet should be dropped */
    if ((fhei_size_port == 3) && (in_lif > 0)) {
        result = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    } else {
 
        if (is_device_or_above(unit, JERICHO)) {
            /*
             * Enter for Jericho and up
             */
            /*
             * Request 'counter + pp-port redirect' (param2=-1) and set the counter at the same location as in ARAD:
             * For Arad, when the LS 19 bits are zero, on statId, selected counter will be as the destination port on the
             * last 256 entries of the Ace table (which is 4K entries long).
             */
            statId |= (4 * 1024 - 256 - 1 + in_port);
 
            printf("%s(): JERICHO: statId %d (0x%08lX) in_port %d out_port %d\r\n", proc_name, statId, statId, in_port, out_port);
 
            for (ii = 0; ii < cores_num; ii++) {
                core_config_arr[ii].param0 = statId;
                core_config_arr[ii].param1 = local_gport;
                core_config_arr[ii].param2 = BCM_ILLEGAL_ACTION_PARAMETER;
            }
            printf("%s(): Call bcm_field_action_config_add(ent %d bcmFieldActionStat %d\r\n", proc_name, ent, action_stat);
            printf("==> statId %d local_gport %d (0x%08lX))\r\n", statId, local_gport, local_gport_as_long);
            /*
             * Setting 'core_config_arr_len' to '1' results in setting all cores the same way.
             */
            core_config_arr_len = cores_num;
            result = bcm_field_action_config_add(unit, ent, bcmFieldActionStat, core_config_arr_len, &core_config_arr[0]);
        } else {
            /*
             * Enter for Arad and Arad+
             */
            printf("%s(): ARAD: statId %d (0x%08lX) in_port %d out_port %d\r\n", proc_name, statId, statId, in_port, out_port);
            printf("%s(): Call bcm_field_action_add(ent %d bcmFieldActionStat %d statId %d local_gport %d (%08lX))\r\n", proc_name, ent, action_stat, statId, local_gport, local_gport_as_long);
 
            result = bcm_field_action_add(unit, ent, bcmFieldActionStat, statId, local_gport);
        }
        if (BCM_E_NONE != result) {
            if (is_device_or_above(unit, JERICHO)) {
                printf("%s(): Error in bcm_field_action_config_add\n", proc_name);
            } else {
                printf("%s(): Error in bcm_field_action_add\n", proc_name);
            }
            return result;
        }
        if (is_device_or_above(unit, JERICHO)) {
            printf("%s(): Called bcm_field_action_config_add(ent %d bcmFieldActionStat %d statId %d local_gport %d (%08lX))\r\n", proc_name, ent, action_stat, statId, local_gport, local_gport_as_long);
        } else {
            printf("%s(): Called bcm_field_action_add(ent %d bcmFieldActionStat %d statId %d local_gport %d (%08lX))\r\n", proc_name, ent, action_stat, statId, local_gport, local_gport_as_long);
        }
    }
    /*
     *  Commit the entire group to hardware.  We could do individual entries,
     *  but since we just created the group here and want the entry/entries all
     *  in hardware at this point, it's a bit quicker to commit the whole group.
     */
    result = bcm_field_group_install(unit, group);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        return result;
    }
 
    /*
     * Original was: configure manually the ace type: 1 (bit 8)
     * Take this clause out. No such manual setting is acceptable.
     */
    /*
    if (is_device_or_above(unit, JERICHO)) {
        bshell(unit, "mod PPDB_A_TCAM_ACTION[6] 0 1 ACTION=0x3c9d00");
    }
    */
 
    bshell(unit, "dump chg PPDB_A_TCAM_ACTION[6]");
 
    printf("%s(): Exit. group %d in_port %d out_port %d\r\n", proc_name, group, in_port, out_port);
    return result;
}
 
/*
 *  Set the previous configurations for an example configuration
 */
int system_port_lookup_ptch(int unit, int group_priority, bcm_field_group_t group, bcm_port_t in_port, bcm_port_t out_port) 
{
  int result;
  char *proc_name ;

  proc_name = "system_port_lookup_ptch" ;
  printf("%s(): Enter. group_priority %d group %d in_port %d out_port %d\r\n",proc_name,group_priority,group,in_port,out_port) ;

  result =  system_port_lookup_setup(unit, group_priority, group);
  if (BCM_E_NONE != result) {
      printf("Error in system_port_lookup_setup\n");
      return result;
  }
  result =  system_port_lookup_entry_add(unit, group, in_port, out_port);
  if (BCM_E_NONE != result) {
      printf("Error in system_port_lookup_entry_add\n");
      return result;
  }

  printf("%s(): Exit.group_priority %d group %d in_port %d out_port %d\r\n",proc_name,group_priority,group,in_port,out_port) ;
  return result;
}
 
/* set the below configuration per port
*  IP(MC)oEthernet - (Routing over .1q Bridge) Pass, MAP system port to local port
*  IP(MC)oEthernet - Routing over VPN Bridge Pass, MAP system port to local port
*  IP(MC)oL2-Tunnel - Drop(these packets  should not be flooded in the vlan)
*/ 
int system_port_fhei_lookup_ptch(int unit, int group_priority, bcm_field_group_t group, bcm_port_t in_port, bcm_port_t out_port) 
{
  int result;
  char *proc_name ;
  int in_lif;
  int fhei_size;
 
  proc_name = "system_port_lookup_ptch" ;
  printf("%s(): Enter. group_priority %d group %d in_port %d out_port %d\r\n",proc_name,group_priority,group,in_port,out_port) ;
 
  result =  system_port_fhei_lookup_setup(unit, group_priority, group);
  if (BCM_E_NONE != result) {
      printf("Error in system_port_lookup_setup\n");
      return result;
  }
  
  /* Routing over dot1q Bridge, packet should pass, map system port to local port */ 
  in_lif = 0;
  fhei_size =  3; 
  result =  system_port_fhei_lookup_entry_add(unit, group, in_port, out_port, fhei_size, in_lif);
  if (BCM_E_NONE != result) {
      printf("Error in system_port_lookup_entry_add\n");
      return result;
  }
 
/* Routing over VPN Bridge, packet should pass, map system port to local port */ 
  in_lif = 1000;
  fhei_size =  8; 
  result =  system_port_fhei_lookup_entry_add(unit, group, in_port, out_port, fhei_size, in_lif);
  if (BCM_E_NONE != result) {
      printf("Error in system_port_lookup_entry_add\n");
      return result;
  }
 
  /* Routing over over IP-Tunnel packet should be dropped */ 
  in_lif = 2000;
  fhei_size =  3; 
  result =  system_port_fhei_lookup_entry_add(unit, group, in_port, out_port, fhei_size, in_lif);
  if (BCM_E_NONE != result) {
      printf("Error in system_port_lookup_entry_add\n");
      return result;
  }
 
  printf("%s(): Exit.group_priority %d group %d in_port %d out_port %d\r\n",proc_name,group_priority,group,in_port,out_port) ;
  return result;
}


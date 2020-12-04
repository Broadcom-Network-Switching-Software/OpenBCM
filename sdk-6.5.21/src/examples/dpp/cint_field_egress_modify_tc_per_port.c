
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.3
*/

/*
 *  Create a bcm_field_group_t, then invoke field_api_setup(unit, prio, &group).
 *  Perform any frame testing desired.  Invoke field_api_teardown(unit, group).
 *
 *  Modify field_api_setup to fit your needs, or perhaps clone it several times
 *  for a number of different groups.  You can put more than one entry in each
 *  group, so each clone could set up several entries.
 */
int egress_acl_add(int unit, bcm_field_group_t group, int group_priority, bcm_port_t out_port, int counter_id, int counter_proc){
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_entry_t ent;
  bcm_field_aset_t aset;
  bcm_gport_t local_gport;
  bcm_field_stat_t stats[2];
  int statId;

  unsigned long local_gport_as_long ;
  int action_stat ;
  int cores_num, ii ;
  char *proc_name ;
  bcm_field_action_core_config_t core_config_arr[SOC_DPP_DEFS_GET_NOF_CORES(unit)] ;
  int core_config_arr_len ;

  proc_name = "egress_acl_add" ;
  printf("%s(): Enter. group %d group_priority %d out_port %d counter_id %d counter_proc %d\r\n",proc_name,group,group_priority,out_port,counter_id,counter_proc) ;
  cores_num = sizeof(core_config_arr) / sizeof(core_config_arr[0]);

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);
  
    /* Translate the port from logical port to pp port */
  uint32 dummy_flags ;
  bcm_port_interface_info_t interface_info ;
  bcm_port_mapping_info_t mapping_info ;
  /* Retrieve out_port's tm_port and modid */
  bcm_port_get(unit, out_port, &dummy_flags, &interface_info, &mapping_info); 

  /*
   *  Create the group and set is actions.  This is two distinct API calls due
   *  to historical reasons.  On soc_petra, the action set call is obligatory, and
   *  a group must have at least one action in its action set before any entry
   *  can be created in the group.
   */
  result = bcm_field_group_create_id(unit, qset, group_priority, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
    return result;
  }

  result = bcm_field_entry_create(unit, group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
    return result;
  }

  result = bcm_field_qualify_DstPort(unit, ent, mapping_info.core, -1, mapping_info.tm_port, -1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_DstPort\n");
    return result;
  }

  result = bcm_field_action_add(unit, ent, bcmFieldActionPrioIntNew, 5, 0);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_action_add\n");
    return result;
  }

  /* Redirect to port 1 */
  BCM_GPORT_LOCAL_SET(local_gport, out_port);
  /*
   * The stat-id is an encoding of the HW Counter-ID and the HW 
   * Counter-Processor (aka Counter-Engine). 
   * Due to HW reasons, the Counter-ID can be only at egress between 1024 and 
   * 3839. Besides, the user must re-indicate the redirect gport as 
   * second parameter. 
   *  
   * When redirecting at egress without counting, the user must still 
   * set the stat function with stat-id equal to 0 (do not count) 
   */
  action_stat = bcmFieldActionStat ;
  local_gport_as_long = local_gport ;
  BCM_FIELD_STAT_ID_SET(statId, counter_proc, counter_id); 

  if (is_device_or_above(unit, JERICHO)) {
    /*
     * Enter for Jericho and up
     */
    if (counter_id == 0) {
      /*
       * Request 'counter + pp-port redirect' (param2=-1) and set the counter at the same location as in ARAD:
       * For Arad,  when the LS 19 bits are zero, on statId, selected counter will be as the destination port on the
       * last 256 entries of the Ace table (which is 4K entries long).
       */
      statId |= (4*1024 - 256 - 1 + out_port) ; 
    }
    printf("%s(): JERICHO: statId %d (0x%08lX) out_port %d\r\n",proc_name,statId,statId,out_port) ;

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
    result = bcm_field_action_config_add(unit, ent, bcmFieldActionStat, core_config_arr_len, &core_config_arr[0]) ;
    if (BCM_E_NONE != result) {
        printf("%s(): Error in bcm_field_action_config_add\n",proc_name);
        return result;
    }
  } else {
    /*
     * Enter for Arad and Arad+
     */
    printf("%s(): ARAD: Call bcm_field_action_add(ent %d action %d statId 0x%08lX local_gport %d (0x%08lX))\r\n",proc_name,ent, action_stat, statId, local_gport,local_gport_as_long) ;

    result = bcm_field_action_add(unit, ent, bcmFieldActionStat, statId, local_gport);
    if (BCM_E_NONE != result) {
        printf("%s(): Error in bcm_field_action_add\n",proc_name);
        return result;
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
  printf("%s(): Exit OK. group %d group_priority %d\r\n",proc_name,group,group_priority) ;
  return BCM_E_NONE;
}
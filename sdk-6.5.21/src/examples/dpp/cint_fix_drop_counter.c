/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 
/**
 * cint check  when packet configured to be snoop & drop if its 
 * drop counter actually updated 
 * test function - drop_action_with_counting 
 * the sequnce of the test function include 
 *  create_dummy_voq - create voq that drop each packet arrived
 *  to it
 *  create_drop_action - which base on forward_action to the
 *  created voq
 */


int create_drop_action(int unit, bcm_field_entry_t i_ent, bcm_gport_t voq_gport) {
    int result;

    result = bcm_field_action_add(unit, i_ent, bcmFieldActionForward, 3<<17 | BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(voq_gport), 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    result = bcm_field_entry_install(unit, i_ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    return result;

}

int create_dummy_voq(int unit,int port,bcm_gport_t *voq_gport) {
    bcm_gport_t modport_gport,sysport_gport;
    int cosq;
    bcm_cosq_ingress_queue_bundle_gport_config_t ingress_config;
    uint32 flags = BCM_COSQ_GPORT_SIZE_BYTES | BCM_COSQ_GPORT_SIZE_COLOR_BLIND;
    bcm_cosq_gport_size_t gport_size;

    /* map  sysport modport*/
    BCM_GPORT_MODPORT_SET(modport_gport, port, 0);
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, 900);

    print bcm_stk_sysport_gport_set(unit, sysport_gport, modport_gport);


    /* allocate voq for the mapped sysport*/
    ingress_config.port = sysport_gport;
    ingress_config.flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
    ingress_config.numq = 8;
    ingress_config.local_core_id = BCM_CORE_ALL ;
    for (cosq = 0 ; cosq < 8; cosq++) {
        ingress_config.queue_atrributes[cosq].delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED;
        ingress_config.queue_atrributes[cosq].rate_class = 0;
    }

    print bcm_cosq_ingress_queue_bundle_gport_add(unit,&ingress_config, voq_gport);



    /* set the queue to drop every arriving packet*/
    gport_size.size_max = 0;    
    print bcm_cosq_gport_color_size_set(unit, *voq_gport, 0, 0, flags, &gport_size);

}

/* PART 1: PMF group to snoop packets to CPU */
int create_trap_snoop_destination(bcm_port_t dest, bcm_gport_t *trap_gport)
{
    int unit=0;
    bcm_error_t rv=0;
    int flags = 0; /* Do not specify any ID for the snoop command or trap */
    int snoop_command; /* Snoop command */
    int trap_id;
    int trap_dest_strength = 0; /* No influence on the destination update */
    int trap_snoop_strength = 3; /* Strongest snoop strength for this trap */
    bcm_rx_snoop_config_t snoop_config; /* Snoop attributes */
    bcm_rx_trap_config_t trap_config;

    rv |= bcm_rx_snoop_create(unit, flags, &snoop_command); 
    print rv;

    bcm_rx_snoop_config_t_init(&snoop_config);
    snoop_config.flags = flags | BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_COUNTER;
    snoop_config.counter_num = 900;     /* Increment counter at offset 900 */
    BCM_GPORT_LOCAL_SET (snoop_config.dest_port, dest);
    snoop_config.size = -1;
    snoop_config.probability = 100000;

    rv |= bcm_rx_snoop_set(unit, snoop_command, &snoop_config); 
    print rv;

    /* Now create trap */
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags |= BCM_RX_TRAP_REPLACE;
    trap_config.trap_strength = trap_dest_strength;
    trap_config.snoop_cmnd = snoop_command;
    trap_config.snoop_strength = trap_snoop_strength;

    rv |= bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &trap_id); print rv;

    rv |= bcm_rx_trap_set(unit, trap_id, &trap_config); 
    print rv;

    BCM_GPORT_TRAP_SET(*trap_gport, trap_id, trap_dest_strength, trap_snoop_strength);
    print *trap_gport; 
    print trap_id;
}


int create_snooping_pmf_group(bcm_field_group_t gid)
{
    int unit=0;
    bcm_error_t rv=0;
    bcm_field_presel_set_t presel_set;
    int presel_id = 10;
    int presel_flags = 0;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t grp = gid;
    int pri=5;
    bcm_field_group_config_t grp_conf;
    bcm_pbmp_t all_ports;
    bcm_pbmp_t all_ports_mask;
    BCM_PBMP_CLEAR(all_ports_mask);
    BCM_PBMP_NEGATE(all_ports_mask, all_ports_mask);

    int i;
    BCM_PBMP_PORT_ADD(all_ports, 13);
    BCM_PBMP_PORT_ADD(all_ports, 14);

    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        rv |= bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);  print rv;
    } else {
        rv |= bcm_field_presel_create(unit, &presel_id);  print rv;
    }
    rv |= bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress); print rv;
    print all_ports;
    rv |= bcm_field_qualify_InPorts(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, all_ports, all_ports_mask); print rv;

    BCM_FIELD_PRESEL_INIT(presel_set);
    BCM_FIELD_PRESEL_ADD(presel_set, presel_id);

    bcm_field_group_config_t_init(&grp_conf);
    grp_conf.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_SMALL;
    grp_conf.group = grp;
    grp_conf.priority = pri;

    BCM_FIELD_QSET_INIT(grp_conf.qset);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyL2Format);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyInterfaceClassL2);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyMplsTerminated);

    BCM_FIELD_ASET_INIT(grp_conf.aset);
    BCM_FIELD_ASET_ADD(grp_conf.aset,bcmFieldActionRedirect);
    BCM_FIELD_ASET_ADD(grp_conf.aset,bcmFieldActionStat0);
    BCM_FIELD_ASET_ADD(grp_conf.aset,bcmFieldActionSnoop);
    
    rv |= bcm_field_group_config_create(unit, &grp_conf); print rv;
    rv |= bcm_field_group_presel_set(unit, grp, &presel_set); print rv;
    rv |= bcm_field_group_action_set(unit, grp, grp_conf.aset); print rv;
}

int add_snoop_pmf_entry(bcm_field_group_t group, bcm_field_entry_t entry, bcm_gport_t dest)
{
    bcm_error_t rv=0;
    int unit=0;
    bcm_field_stat_t stats = bcmFieldStatPackets;
    bcm_mac_t mac_addr = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_mac_t mac_mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    int stats_id=entry+1;
    int stats_id0;
	printf("------dest=%d \n",dest);
    BCM_FIELD_STAT_ID_SET(stats_id0, 0, stats_id);
    rv |= bcm_field_entry_create_id(unit, group, entry);
     print rv;
    rv |= bcm_field_qualify_DstMac(unit,entry,mac_addr, mac_mask); 
    print rv;
    rv |= bcm_field_qualify_EtherType(unit, entry, 0x0806, 0xffff);
     print rv;

    rv |= bcm_field_stat_create_id(unit, group, 1, &stats, stats_id); 
    print rv;   
    rv |= bcm_field_action_add(unit,entry, bcmFieldActionSnoop, dest, 0); 
    print rv;
	rv |= bcm_field_action_add(unit,entry,bcmFieldActionStat0, stats_id0,0); 
    print rv;

}


int prepare_drop_acl_entry(int unit, bcm_field_entry_t *i_ent)
{
  int result;
  bcm_field_qset_t i_qset;
  bcm_field_aset_t i_aset;
  bcm_field_group_config_t i_grp_conf;
  bcm_field_group_t i_grp_tcam;
 
  /* Define the QSET. */
  BCM_FIELD_QSET_INIT(i_qset);
  BCM_FIELD_QSET_ADD(i_qset, bcmFieldQualifyStageIngress); 
  BCM_FIELD_QSET_ADD(i_qset, bcmFieldQualifyEtherType);
 
  /*  Create the Field group with type TCAM */
  result = bcm_field_group_create(unit, i_qset, 10, &i_grp_tcam);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      return result;
  }
 
  /* Define the ASET - user header */
  BCM_FIELD_ASET_INIT(i_aset);
  BCM_FIELD_ASET_ADD(i_aset, bcmFieldActionForward);
  
  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, i_grp_tcam, i_aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set for group %d\n", i_grp_tcam);
    return result;
  }
 
  /* Create one entry */
  result = bcm_field_entry_create(unit, i_grp_tcam, i_ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }
 
/* by Asi*/
  result = bcm_field_qualify_EtherType(unit, *i_ent, 0x806, 0xffff);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_ForwardingType\n");
      return result;
  }
  
  
  return result;
 
} /* generic_field_group_simple_tcam */

void disable_mac_learning_packets()
{

    int old_learn_mode;
    print bcm_switch_control_get(0, bcmSwitchL2LearnMode, &old_learn_mode);
    print bcm_switch_control_set(0, bcmSwitchL2LearnMode, old_learn_mode | BCM_L2_LEARN_DISABLE);
}


int test_counters()
{

    /* get counters api */
    int stat_id=0;
    uint32 count=0;
    uint32 count2=0;

    BCM_FIELD_STAT_ID_SET(stat_id,0,11);
    print bcm_field_stat_get32(0, stat_id, bcmFieldStatPackets, &count);
    printf ("original packets count=%d\n",count);
    BCM_FIELD_STAT_ID_SET(stat_id,0,900);
    print bcm_field_stat_get32(0, stat_id, bcmFieldStatPackets, &count2);
    printf ("snooping packets count=%d\n",count2);

    return count == 0;

}


int send_tested_packet(int unit) {

    print bcm_port_force_forward_set(unit,13,14,1); 
    /* disable mac learning */
    disable_mac_learning_packets();
    bshell(0,"tx 1 psrc=13 DATA=ffffffffffff0000050004008100000a08066030000006023BFF0000123456789000000000000000000020010000000000640000000000000014000102030405060708090A0B0C0D0E0F101112131415161718B5B6B7B8B9BABBBCBDBEBFC0C1C2C3C4C5C6C7C8C9CACBCCCDCECFD0D1D2D3D4D5D6D7D8D9DADBDCDDDEDFE0E1E2E3E4E5E6E7E8E9EAEBECEDEEEFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF0001DEC2F5A0 
");
    /* bshell(0,"diag pp sig name=tm_cmd");  */

    return test_counters();

}






int config_snoop_pmf(int unit,
                     bcm_field_group_t snoop_gid,
                     bcm_field_entry_t arp_snoop_entry,
                     bcm_port_t trap_dest_port
                     ) {

    bcm_gport_t trap_gport;
    create_trap_snoop_destination(trap_dest_port, &trap_gport);
    create_snooping_pmf_group(snoop_gid);
    add_snoop_pmf_entry(snoop_gid, arp_snoop_entry, trap_gport);
    print bcm_field_group_install(unit,snoop_gid);
}


int drop_action_with_counting(int unit) {
    bcm_field_group_t snoop_gid=0;
    bcm_field_entry_t arp_snoop_entry = 10;
    bcm_port_t trap_dest_port=0; /* cpu port */
    bcm_gport_t voq_gport;
    int dummy_port=500;
    bcm_field_entry_t i_ent;




   config_snoop_pmf(unit,snoop_gid,arp_snoop_entry,trap_dest_port);

   create_dummy_voq(unit,dummy_port,&voq_gport);



   print prepare_drop_acl_entry(unit,&i_ent); 

   /* config actuall drop command*/
    print create_drop_action(unit, i_ent,  voq_gport); 

    return send_tested_packet(unit);

}

/*
exit;


ctrp get proc=0 set=11
ctrp get proc=0 set=900

diag pp sig name=tm_cmd
*/

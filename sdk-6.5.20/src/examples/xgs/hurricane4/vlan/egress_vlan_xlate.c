/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

 /* Feature  : Egress VLAN translation.
 *
 *  Usage    : BCM.0> cint egress_vlan_xlate.c
 *
 *  config   : BCM56275_A1-PORT.bcm
 *
 *  Log file : egress_vlan_xlate_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *         port1     |                              |  port2
 *  +----------------+                              +-----------------+
 *                   |         SWITCH               |
 *                   |                              |
 *         port3     |                              |  port4
 *  +----------------+                              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *  Summary:
 *  ========
 *      This cint example illustrates egress VLAN translation for
 *      ADD/DELETE/REPLACE/COPY tag action using BCM API. This script also shows how to
 *      attach flex counters and get the stats for egress vlan translation.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Selects four ports and configure them in Loopback mode.
 *         Install a rule to copy incoming packets to CPU and start packet watcher.
 *
 *      Note: This is done to transmit the required test packets using "tx" command and dump the
 *            contents of the packets received on both the ports.
 *
 *    2) Step2 - Configuration (Done in configure_egress_xlate_entries()):
 *    =====================================================================
 *      a) Create  VLAN 10 and add port2, port3 and port4 as members.
 *
 *      b) Create  VLAN 20 and add port1, port3 and port4 as members.
 *
 *      b) On all ports, enable egress VLAN translation and set 0x8100 as
 *         outer TPID and 0x9100 as inner TPID.
 *
 *      c) Configure 6 egress vlan translate rules as below.
 *         Rule #0) For outer tagged packets(outer VLAN: 10) on port3,
 *                          add inner tag with vlan ID 20 and inner vlan priority 3.
 *         Rule #1) For outer tagged packets(outer VLAN: 20) on port1,
 *                          delete outer tag.
 *         Rule #2) For double tagged packets(outer VLAN: 10, inner VLAN: 20) on port2,
 *                          delete both inner and outer vlan.
 *         Rule #3) For outer tagged packets(outer VLAN: 20) on port3,
 *                          replace outer VLAN with 200.
 *         Rule #4) For double tagged packets(outer VLAN: 10, inner VLAN: 20) on port3,
 *                          swap the outer and inner vlans.
 *         Rule #5) For double tagged packets(outer VLAN: 20, inner VLAN: 10) on port3,
 *                          swap the outer and inner vlans.
 *
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) Send an outer tagged packet(outer VLAN:10) on port4 to verify rule 0.
 *
 *      b) Send an outer tagged packet(outer VLAN:20) on port3 to verify rule 1.
 *
 *      c) Send Double tagged packet(outer VLAN:10 and inner VLAN:20) on port3 to verify rule 2.
 *
 *      d) Send an outer tagged packet(outer VLAN:20) on port4 to verify rule 3.
 *
 *      e) Send Double tagged packet(outer VLAN:10 and inner VLAN:20) on port4 to verify rule 4.
*
 *      f) Send Double tagged packet(outer VLAN:20 and inner VLAN:10) on port4 to verify rule 5.
 *
 *      f) Expected Result:
 *      ===================
 *         After step 3.a, double tagged packet(outer VLAN:10, inner vlan:20, inner vlan pri: 3)
 *         egresses out of port3.
 *
 *         After step 3.b, un-tagged packet egresses out of port1.
 *
 *         After step 3.c, un-tagged packet egresses out of port2.
 *
 *         After step 3.d, outer tagged packet(outer VLAN:200) egresses out of port3.
 *
 *         After step 3.e, double tagged packet(outer VLAN:20, inner VLAN:10, egresses out of port3.
 *
 *         After step 3.f, double tagged packet(outer VLAN:10, inner VLAN:20, egresses out of port3.
 */
/* reset c interpreter*/
cint_reset();
uint16 vlanId1 = 10, vlanId2 = 20, defVlan=1;
int port1 = 1, port2 = 2, port3 = 3, port4 = 4;
uint16 inner_tpid=0x9100, outer_tpid=0x8100;
/* Number of rule to be installled */
uint8 rules = 6;
struct xlate_stat{
                        uint16 vlanid_key_outer;    /* outer VLAN id key for xlate rule */
                        uint16 vlanid_key_inner;    /* inner VLAN id key for xlate rule */
                        uint32 stat_counter_id;     /* stat counter id reserved for each rule */
                        uint32 counters_number;     /* number of counters for each rule i.e. 1 for bcmStatGroupModeSingle */
                        int port;                   /* physical port id */
                        bcm_vlan_action_set_t tagAction;    /* set of actions to be installed per port */
                        bcm_stat_value_t bcm_stat;  /* stat object */
                    };
bcm_field_group_t group = -1;
/* This function is written so that hardcoding of port
   numbers in Cint scripts is removed. This function gives
   required number of ports
   */
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{

  int i=0,port=0,rv=0;
  bcm_port_config_t configP;
  bcm_pbmp_t ports_pbmp;

  rv = bcm_port_config_get(unit,&configP);
  if(BCM_FAILURE(rv)) {
    printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
    return rv;
  }

  ports_pbmp = configP.e;
  for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
    if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
      port_list[port]=i;
      port++;
    }
  }

  if (( port == 0 ) || ( port != num_ports )) {
    printf("portNumbersGet() failed \n");
    return -1;
  }

  return BCM_E_NONE;

}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

  if (group == -1)
  {
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));
  }

  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
  return BCM_E_NONE;
}


/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

  if (group == -1)
  {
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));
  }
  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

  return BCM_E_NONE;
}

/*
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
  int port_list[4];

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  port1 = port_list[0];
  port2 = port_list[1];
  port3 = port_list[2];
  port4 = port_list[3];


  if (BCM_E_NONE != ingress_port_setup(unit, port1)) {
    printf("ingress_port_setup() failed for port %d\n", port1);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, port2)) {
    printf("egress_port_setup() failed for port %d\n", port2);
    return -1;
  }


  if (BCM_E_NONE != egress_port_setup(unit, port3)) {
    printf("egress_port_setup() failed for port %d\n", port3);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, port4)) {
    printf("egress_port_setup() failed for port %d\n", port4);
    return -1;
  }

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}
/* egress xlate rule information */
xlate_stat xlate_rule_info[rules] =
{
        {vlanId1,0,0,0,port3},       /* rule 0 only for port port3 and packets with outer tag of vlanId1 */
        {vlanId2,0,0,0,port1},
        {vlanId1,vlanId2,0,0,port2},
        {vlanId2,0,0,0,port3},
        {vlanId1,vlanId2,0,0,port3},
        {vlanId2,vlanId1,0,0,port3}
};

/* This function will initialize key and tag actions*/
int key_and_action_set_init(xlate_stat *info)
{
	/* For rule 0: for outer tagged egressing packets with vlanId1 (outer VLAN id key) on port port3,
	an inner tag will be added with vlan id 20 and packet priority 3 */
	bcm_vlan_action_set_t_init(&info[0].tagAction);  /* initialize the action set */
	info[0].tagAction.ot_inner=bcmVlanActionAdd;     /* define inner tag action for outer tagged packets */
	info[0].tagAction.ot_inner_pkt_prio=bcmVlanActionAdd;    /* define packet priority action for outer tagged packets */
	info[0].tagAction.new_inner_vlan=20;     /* new inner tag to be added in case of add or replace action */
	info[0].tagAction.new_inner_pkt_prio=3;  /* new packet priority to be added in case of add or replace action */
  info[0].vlanid_key_outer = vlanId1;
  info[0].port = port3;

	/* For rule 1: for outer tagged egressing packets with vlanId2 (outer VLAN id key) on port port1,
	outer tag will be deleted */
	bcm_vlan_action_set_t_init(&info[1].tagAction);
	info[1].tagAction.ot_outer=bcmVlanActionDelete; /* define outer tag action for outer tagged packets */
  info[1].vlanid_key_outer = vlanId2;
  info[1].port = port1;

	/* For rule 2: for double tagged egressing packets with vlanId1 and vlanId2 on port port2,
	the outer and inner tag will be deleted i.e. packet will be egressing untagged*/
	bcm_vlan_action_set_t_init(&info[2].tagAction);
	info[2].tagAction.dt_inner=bcmVlanActionDelete;  /* define inner and outer tag action for double tagged packets */
	info[2].tagAction.dt_outer=bcmVlanActionDelete;
  info[2].vlanid_key_outer = vlanId1;
  info[2].vlanid_key_inner = vlanId2;
  info[2].port = port2;

	/* For rule 3: for outer tagged egressing packets with vlanId2 on port port3,
	the outer tag vlanId2 will be replaced by vlanid 200 */
	bcm_vlan_action_set_t_init(&info[3].tagAction);
	info[3].tagAction.ot_outer=bcmVlanActionReplace; /* define outer tag action for outer tagged packets */
	info[3].tagAction.new_outer_vlan=200;
  info[3].vlanid_key_outer = vlanId2;
  info[3].port = port3;

	/* For rule 4: for double tagged egressing packets with vlanId1 and vlanId2 on port port3,
	both inner and outer tags will be swapped */
	bcm_vlan_action_set_t_init(&info[4].tagAction);
	info[4].tagAction.dt_outer=bcmVlanActionCopy;
	info[4].tagAction.dt_inner=bcmVlanActionCopy;
	info[4].tagAction.dt_outer_cfi=bcmVlanActionCopy;
	info[4].tagAction.dt_inner_cfi=bcmVlanActionCopy;
  info[4].vlanid_key_outer = vlanId1;
  info[4].vlanid_key_inner = vlanId2;
  info[4].port = port3;

	/* For rule 5: for double tagged egressing packets with vlanId2 and vlanId1 on port port3,
	both inner and outer tags will be swapped */
	bcm_vlan_action_set_t_init(&info[5].tagAction);
	info[5].tagAction.dt_outer=bcmVlanActionCopy;    /* define outer tag action i.e. copy inner tag to outer tag */
	info[5].tagAction.dt_inner=bcmVlanActionCopy;    /* define inner tag action i.e. copy outer tag to inner tag */
	info[5].tagAction.dt_outer_cfi=bcmVlanActionCopy;
	info[5].tagAction.dt_inner_cfi=bcmVlanActionCopy;
  info[5].vlanid_key_outer = vlanId2;
  info[5].vlanid_key_inner = vlanId1;
  info[5].port = port3;

    return 0;
}
/* This function will push egress xlate action rules and attach counter for each rule */
bcm_error_t  egress_xlate_action_set(int unit)
{
    uint8 index=0;
	  BCM_IF_ERROR_RETURN(key_and_action_set_init(xlate_rule_info));
    for (index = 0; index < rules; index++)
    {
        /* Add specific egress tanslation action other than default port action which will create new entry in egress translation table
        and may be in tag action profile table */
        BCM_IF_ERROR_RETURN(bcm_vlan_translate_egress_action_add(unit, xlate_rule_info[index].port, xlate_rule_info[index].vlanid_key_outer,
                                                    xlate_rule_info[index].vlanid_key_inner, &xlate_rule_info[index].tagAction));
       /* Attach reserved counter to the rule with specific keys i.e. inner and outer vlan ids*/
       printf("Attaching counter:%d for port:%d, outer VLAN:%d, inner VLAN:%d\n", index, xlate_rule_info[index].port,
                                   xlate_rule_info[index].vlanid_key_outer, xlate_rule_info[index].vlanid_key_inner);
       BCM_IF_ERROR_RETURN(bcm_vlan_translate_egress_stat_attach(unit, xlate_rule_info[index].port, xlate_rule_info[index].vlanid_key_outer,
                                                    xlate_rule_info[index].vlanid_key_inner, xlate_rule_info[index].stat_counter_id));
    }
    return BCM_E_NONE;
}
/* This function will reserve stat conters for the bcmStatObjectEgrVlanXlate objects */
bcm_error_t  reserve_counters(int unit)
{
    uint8 index=0;
    for(;index < rules; index++)
    {
        if (0 != xlate_rule_info[index].vlanid_key_inner)
        {
          /* Create stat group for egress xlate rule hit with bcmStatGroupModeSingle (single counter will be used) */
          BCM_IF_ERROR_RETURN( bcm_stat_group_create(unit, bcmStatObjectEgrVlanXlate, bcmStatGroupModeSingle,
                            &xlate_rule_info[index].stat_counter_id, &xlate_rule_info[index].counters_number));
        }
        else
        {
          BCM_IF_ERROR_RETURN( bcm_stat_group_create(unit, bcmStatObjectEgrVlanXlateSecondLookup, bcmStatGroupModeSingle,
                            &xlate_rule_info[index].stat_counter_id, &xlate_rule_info[index].counters_number));

        }
    }
    return BCM_E_NONE;
}
/* This function will read and display the hit-stats of each rule */
bcm_error_t  egress_xlate_stat_get(int unit)
{
    uint8 index=0;
    unsigned int ctr;
    for (;index < rules; index++)
    {
        /* Read packet stats and display it in hex format */
        BCM_IF_ERROR_RETURN(bcm_vlan_translate_egress_stat_counter_get (unit, xlate_rule_info[index].port, xlate_rule_info[index].vlanid_key_outer,
                                        xlate_rule_info[index].vlanid_key_inner, bcmVlanStatEgressPackets, 1, &ctr, &xlate_rule_info[index].bcm_stat));
        printf ("\n rule %d - packets:0x%x%x ",index, COMPILER_64_HI(xlate_rule_info[index].bcm_stat.packets64),COMPILER_64_LO(xlate_rule_info[index].bcm_stat.packets64));
        /* Read bytes stats and display it in hex format */
        BCM_IF_ERROR_RETURN(bcm_vlan_translate_egress_stat_counter_get (unit, xlate_rule_info[index].port, xlate_rule_info[index].vlanid_key_outer,
                                        xlate_rule_info[index].vlanid_key_inner, bcmVlanStatEgressBytes, 1, &ctr, &xlate_rule_info[index].bcm_stat));
        printf ("bytes:0x%x%x\n",COMPILER_64_HI(xlate_rule_info[index].bcm_stat.bytes),COMPILER_64_LO(xlate_rule_info[index].bcm_stat.bytes));
    }
    return BCM_E_NONE;
}
/* This function will set port specific properties */
bcm_error_t  port_property_set(int unit, bcm_port_t port)
{
  /* Add outer tag tpid to check if the packets are double tagged or outer tagged */
  BCM_IF_ERROR_RETURN(bcm_port_tpid_add(unit,port,outer_tpid,BCM_COLOR_PRIORITY));
  /* Set inner tag tpid to check if the packets are double tagged or inner tagged */
  BCM_IF_ERROR_RETURN(bcm_port_inner_tpid_set(unit,port,inner_tpid));
  /* Enable egress packet translation per port */
  BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit,port,bcmVlanTranslateEgressEnable,1));
    return BCM_E_NONE;
}
/* This function will create vlan with specified port bitmap */
bcm_error_t  create_vlan_add_ports(int unit, int vlanId, bcm_pbmp_t pbm, bcm_pbmp_t upbm)
{
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit,defVlan,pbm));    /* remove the ports from default vlan on device */
    BCM_IF_ERROR_RETURN( bcm_vlan_create(unit,vlanId));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit,vlanId,pbm,upbm));
    return BCM_E_NONE;
}
bcm_error_t  configure_egress_xlate_entries(int unit)
{
    bcm_error_t rv;
    bcm_pbmp_t pbm,upbm;
    /* tagged port bitmap */
    BCM_PBMP_PORT_ADD(pbm,port2);
    BCM_PBMP_PORT_ADD(pbm,port3);
    BCM_PBMP_PORT_ADD(pbm,port4);
    /* untagged port bitmap */
    BCM_PBMP_PORT_ADD(upbm,port2);
    BCM_PBMP_PORT_ADD(upbm,port4);
    rv = create_vlan_add_ports(unit, vlanId1, pbm, upbm);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in creating VLAN1 : %s.\n",bcm_errmsg(rv));
        return rv;
    }
    /* tagged port bitmap */
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_CLEAR(upbm);
    BCM_PBMP_PORT_ADD(pbm,port1);
    BCM_PBMP_PORT_ADD(pbm,port3);
    BCM_PBMP_PORT_ADD(pbm,port4);
    /* untagged port bitmap */
    BCM_PBMP_PORT_ADD(upbm,port4);
    rv = create_vlan_add_ports(unit, vlanId2, pbm, upbm);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in creating VLAN2 : %s.\n",bcm_errmsg(rv));
        return rv;
    }
    /* This will add specific tag to the untagged ingressing packets */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit,port1,vlanId2));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit,port2,vlanId1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit,port3,vlanId1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit,port4,vlanId1));

    rv = reserve_counters(unit);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in reserving xlate counters : %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = port_property_set(unit, port1);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in port_property_set(port:%d) function : %s.\n",port1, bcm_errmsg(rv));
        return rv;
    }
    rv = port_property_set(unit, port2);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in port_property_set(port:%d) function : %s.\n",port2, bcm_errmsg(rv));
        return rv;
    }
    rv = port_property_set(unit, port3);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in port_property_set(port:%d) function : %s.\n",port3, bcm_errmsg(rv));
        return rv;
    }
    rv = port_property_set(unit, port4);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in port_property_set(port:%d) function : %s.\n",port4, bcm_errmsg(rv));
        return rv;
    }

    rv = egress_xlate_action_set(unit);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in egress_xlate_action_set function : %s.\n",bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}
void verify(int unit)
{
  char   str[512];

  bshell(unit, "hm ieee");

  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port4, BCM_PORT_DISCARD_NONE));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port1, BCM_PORT_DISCARD_ALL));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port2, BCM_PORT_DISCARD_ALL));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port3, BCM_PORT_DISCARD_ALL));
  bshell(unit, "sleep quiet 1");
  printf("Sending an outer tagged packet(outer VLAN:10) on port:%d to verify rule 0\n", port4);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000330000000000448100000A08004500002A0000000040FF79D60000000000000000000102030405060708090A0B0C0D0E0F10111213141535763D54; sleep quiet 1", port4);
  bshell(unit, str);

  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port3, BCM_PORT_DISCARD_NONE));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port1, BCM_PORT_DISCARD_ALL));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port2, BCM_PORT_DISCARD_ALL));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port4, BCM_PORT_DISCARD_ALL));
  printf("Sending an outer tagged packet(outer VLAN:20) on port:%d to verify rule 1\n", port3);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000330000000000448100001408004500002A0000000040FF79D60000000000000000000102030405060708090A0B0C0D0E0F10111213141535763D54; sleep quiet 1", port3);
  bshell(unit, str);

  printf("Sending Double Tagged packet(outer VLAN:10 inner vlan:20) on port %d to verify rule 2\n", port3);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000770000000000888100000A91000014800450000260000000040FF79DA0000000000000000000102030405060708090A0B0C0D0E0F1011A3B56D10; sleep quiet 1", port3);
  bshell(unit, str);

  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port4, BCM_PORT_DISCARD_NONE));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port1, BCM_PORT_DISCARD_ALL));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port2, BCM_PORT_DISCARD_ALL));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port3, BCM_PORT_DISCARD_ALL));
  printf("Sending an outer tagged packet(outer VLAN:20) on port:%d to verify rule 3\n", port4);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000330000000000448100001408004500002A0000000040FF79D60000000000000000000102030405060708090A0B0C0D0E0F10111213141535763D54; sleep quiet 1", port4);
  bshell(unit, str);

  printf("Sending Double Tagged packet(outer VLAN:10 inner vlan:20) on port %d to verify rule 4\n", port4);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000770000000000888100000A91000014800450000260000000040FF79DA0000000000000000000102030405060708090A0B0C0D0E0F1011A3B56D10; sleep quiet 1", port4);
  bshell(unit, str);

  printf("Sending Double Tagged packet(outer VLAN:20 inner vlan:10) on port %d to verify rule 4\n", port4);
  snprintf(str, 512, "tx 1 pbm=%d data=0x000000000077000000000088810000149100000A800450000260000000040FF79DA0000000000000000000102030405060708090A0B0C0D0E0F1011A3B56D10; sleep quiet 1", port4);
  bshell(unit, str);

  egress_xlate_stat_get(unit);

}

/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in configure_egress_xlate_entries())
 * c)demonstrates the functionality (Done in verify()).
 */
bcm_error_t execute()
{
  bcm_error_t rv;
  int unit =0;

  bshell(unit, "config show; a ; version");

  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }
  printf("Completed test setup successfully.\n");

  if (BCM_FAILURE((rv = configure_egress_xlate_entries(unit)))) {
    printf("configure_egress_xlate_entries() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing configure_egress_xlate_entries()) successfully.\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

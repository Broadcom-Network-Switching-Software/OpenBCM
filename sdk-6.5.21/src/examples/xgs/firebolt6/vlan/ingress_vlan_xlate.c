/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

 /* Feature  : Ingress VLAN translation.
 *
 *  Usage    : BCM.0> cint ingress_vlan_xlate.c
 *
 *  config   : vlan_config.bcm
 *
 *  Log file : ingress_vlan_xlate_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *         port1     |                              |  port2
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *  Summary:
 *  ========
 *      This cint example illustrates ingress VLAN translation for
 *      ADD/DELETE/REPLACE/COPY tag action using BCM API. This script also shows how to
 *      attach flex counters and get the stats for ingress vlan translation.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Selects two ports and configure them in Loopback mode.
 *         Install a rule to copy incoming packets to CPU and start packet watcher.
 *
 *      Note: This is done to transmit the required test packets using "tx" command and dump the
 *            contents of the packets received on both the ports.
 *
 *    2) Step2 - Configuration (Done in configure_ingress_xlate_entries()):
 *    =====================================================================
 *      a) Create two VLANs(10, 20) and add port1 and port2 as members.
 *
 *      b) On both ports, enable ingress VLAN translation and set 0x8100 as
 *         outer TPID and 0x9100 as inner TPID.
 *
 *      c) Configure 5 ingress vlan translate rules as below.
 *         Rule #0) For outer tagged packets(outer VLAN: 10) ingressing on port2,
 *                          add inner tag with vlan ID 20 and inner vlan priority 3.
 *         Rule #1) For double tag packets (outer VLAN:20, inner VLAN: priority tag) on port1,
 *                          delete inner priority tag.
 *         Rule #2) For outer tagged packets(outer VLAN: 20) ingressing on port1,
 *                          outer VLAN ID is replaced by 10.
 *         Rule #3) For double tagged packet(outer VLAN:10, inner VLAN: 20) on port1,
 *                          swap the VLANs i.e (Outer VLAN 20, inner VLAN: 10).
 *         Rule #4) For untagged packets ingressing port2(),
 *                          add double tags(outer VLAN:20, inner VLAN:10, inner pri:3)
 *         Note: Rule #4 does not use ingress vlan translate table entry.
 *
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) Send an outer tagged packet(outer VLAN:10) on port2 to verify rule 0.
 *
 *      b) Send Double Tagged packet(Outer VLAN:20, Inner tag:PRIORITY tagged) on
 *         port1 to verify rule 1.
 *
 *      c) Send outer Tagged packet(outer VLAN:20) on port1 to verify rule 2.
 *
 *      d) Send Double tagged packet(outer VLAN:10 and inner VLAN:20) on port1 to verify rule 3.
 *
 *      e) Send untagged packet on port2.
 *
 *      f) Expected Result:
 *      ===================
 *         After step 3.a, double tagged packet(outer VLAN:10, inner vlan:20, inner vlan pri: 3)
 *         egresses out of port1.
 *
 *         After step 3.b, single outer tagged packet(outer VLAN:20) egresses out of port2.
 *
 *         After step 3.c, single outer tagged packet(outer VLAN:10) egresses out of port2.
 *
 *         After step 3.d, double tagged packet(outer VLAN:20, inner vlan:10)
 *         egresses out of port2.
 *
 *         After step 3.e, double tags(outer VLAN:10, inner VLAN:20, inner pri:3)
 *         egresses out of port1.
 */
/* Reset c interpreter*/
cint_reset();
bcm_port_t port1, port2;
uint16 vlanId1=10, vlanId2=20, defVlan=1;
int cpu=0;
uint16 inner_tpid=0x9100, outer_tpid=0x8100;
/* Number of rule to be installled */
uint8 rules =4;
struct xlate_stat{
                        uint16 vlanid_key_outer;    /* outer VLAN id key for xlate rule */
                        uint16 vlanid_key_inner;    /* inner VLAN id key for xlate rule */
                        bcm_vlan_translate_key_t key;   /* Ingress translation key type */
                        uint32 stat_counter_id;     /* stat counter id reserved for each rule */
                        uint32 counters_number;     /* number of counters for each rule i.e. 1 for bcmStatGroupModeSingle */
                        int port;                   /* physical port id */
                        bcm_vlan_action_set_t tagAction;    /* set of actions to be installed per port */
                        bcm_stat_value_t bcm_stat;  /* stat object */
                    };
/* ingress xlate rule information*/
xlate_stat xlate_rule_info[rules+1] =
{
        {vlanId1, 0, bcmVlanTranslateKeyPortOuter, 0, 0, port2},     /* rule 0 only for port port2 and packets with outer tag of vlanId1 */
        {vlanId2, 0, bcmVlanTranslateKeyPortDouble, 0, 0, port1},
        {vlanId2, 0, bcmVlanTranslateKeyPortOuter, 0, 0, port1},
        {vlanId1, vlanId2, bcmVlanTranslateKeyPortDouble, 0, 0, port1},
        {0, 0, bcmVlanTranslateKeyInvalid,0,0,port2}         /* Default action for untagged ingressing packets on port2 */
};

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
  bcm_field_group_t group;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

  BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
  return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU and drop the packets. This is
 * to avoid continuous loopback of the packet.
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_group_t group;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

  BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

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
  int port_list[2];

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  port1 = port_list[0];
  port2 = port_list[1];

  if (BCM_E_NONE != ingress_port_setup(unit, port1)) {
    printf("ingress_port_setup() failed for port %d\n", port1);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, port2)) {
      printf("egress_port_setup() failed for port %d\n", port2);
      return -1;
  }

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}

/* This function will initialize key and tag actions*/
int key_and_action_set_init(xlate_stat *info)
{
	/* For rule 0: for outer tagged ingressing packets with vlanId1 (outer VLAN id key) on port2,
	inner tag will be added with vlan id 20 and packet priority 3 */
	bcm_vlan_action_set_t_init(&info[0].tagAction);  /* initialize the action set */
	info[0].tagAction.ot_inner=bcmVlanActionAdd;     /* define inner tag action for outer tagged packets */
	info[0].tagAction.ot_inner_pkt_prio=bcmVlanActionAdd;    /* define packet priority action for outer tagged packets */
	info[0].tagAction.new_inner_vlan=20;     /* new inner tag to be added in case of add or replace action */
	info[0].tagAction.new_inner_pkt_prio=3;  /* new packet priority to be added in case of add or replace action */
  info[0].vlanid_key_outer = vlanId1;
  info[0].vlanid_key_inner = 0;
  info[0].key = bcmVlanTranslateKeyPortOuter;
  info[0].port = port2;

	/* For rule 1: for inner priority tagged ingressing packets with vlanId2 on port1,
	the inner priority tag  will be deleted */
	bcm_vlan_action_set_t_init(&info[1].tagAction);
	info[1].tagAction.dt_inner_prio=bcmVlanActionDelete; /* define tag action for inner priority tagged packets i.e. vlanid=0 */
  info[1].vlanid_key_outer = vlanId2;
  info[1].vlanid_key_inner = 0;
  info[1].key = bcmVlanTranslateKeyPortDouble;
  info[1].port = port1;

  /* For rule 2: for outer tagged ingressing packets with vlanId2 on port1,
	the outer tag vlanId2 will be replaced by vlanId1 */
	bcm_vlan_action_set_t_init(&info[2].tagAction);
	info[2].tagAction.ot_outer=bcmVlanActionReplace; /* define outer tag action for outer tagged packets */
	info[2].tagAction.new_outer_vlan= vlanId1;
  info[2].vlanid_key_outer = vlanId2;
  info[2].vlanid_key_inner = 0;
  info[2].key = bcmVlanTranslateKeyPortOuter;
  info[2].port = port1;

  /* For rule 3: for double tagged ingressing packets with vlanId1 and vlanId2 on port1,
	both inner and outer Vlanids will be swapped */
	bcm_vlan_action_set_t_init(&info[3].tagAction);
	info[3].tagAction.dt_inner=bcmVlanActionCopy;    /* define inner tag action i.e. copy outer vlanid  to inner vlanid  */
	info[3].tagAction.dt_outer=bcmVlanActionCopy;    /* define outer tag action i.e. copy inner vlanid to outer vlanid */
	info[3].tagAction.dt_outer_cfi=bcmVlanActionCopy;
	info[3].tagAction.dt_inner_cfi=bcmVlanActionCopy;
  info[3].vlanid_key_outer = vlanId1;
  info[3].vlanid_key_inner = vlanId2;
  info[3].key = bcmVlanTranslateKeyPortDouble;
  info[3].port = port1;

	/* Default action for untagged ingressing packets on port2*/
	bcm_vlan_action_set_t_init(&info[4].tagAction);
	info[4].tagAction.ut_inner=bcmVlanActionAdd;
	info[4].tagAction.ut_outer=bcmVlanActionAdd;
	info[4].tagAction.ut_inner_pkt_prio=bcmVlanActionAdd;
	info[4].tagAction.new_outer_vlan=10;
	info[4].tagAction.new_inner_vlan=20;
	info[4].tagAction.new_inner_pkt_prio=3;
  info[4].vlanid_key_outer = 0;
  info[4].vlanid_key_inner = 0;
  info[4].key = bcmVlanTranslateKeyInvalid;
  info[4].port = port2;
    return 0;
}
/* This function will push ingress xlate action rules and attach counter for each rule */
bcm_error_t config_xlate_action_and_stat(int unit)
{
    int index=0;
	  BCM_IF_ERROR_RETURN(key_and_action_set_init(xlate_rule_info));
    for (index = 0; index < rules; index++)
    {
        bcm_gport_t gport;
        BCM_GPORT_MODPORT_SET(gport, unit, xlate_rule_info[index].port);
        BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, gport, bcmVlanPortTranslateKeyFirst, bcmVlanTranslateKeyPortDouble));
        BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, gport, bcmVlanPortTranslateKeySecond, bcmVlanTranslateKeyPortOuter));
        /* Add specific ingress tanslation action other than default port action which will create new entry in ingress translation table
        and may be in tag action profile table */
        BCM_IF_ERROR_RETURN(bcm_vlan_translate_action_add(unit, gport, xlate_rule_info[index].key, xlate_rule_info[index].vlanid_key_outer,
                                                     xlate_rule_info[index].vlanid_key_inner,&xlate_rule_info[index].tagAction));
        /* Attach reserved counter to the rule with specific keys i.e. inner and outer vlan ids*/
        BCM_IF_ERROR_RETURN(bcm_vlan_translate_stat_attach(unit, gport, xlate_rule_info[index].key,xlate_rule_info[index].vlanid_key_outer,
                                                    xlate_rule_info[index].vlanid_key_inner, xlate_rule_info[index].stat_counter_id));
    }
        /* Default action rule will be applicable to each ingressing packets on port port2 */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_default_action_set(unit, xlate_rule_info[index].port, &xlate_rule_info[index].tagAction));
    return BCM_E_NONE;
}
/* This function will reserve stat counters for the bcmStatObjectIngVlanXlate objects */
bcm_error_t reserve_counters(int unit)
{
    uint8 index=0;
    for(;index < rules; index++)
    {
        /* Create stat group for ingress xlate rule hit with bcmStatGroupModeSingle (single counter will be used for all traffic types) */
      if( bcmVlanTranslateKeyPortDouble == xlate_rule_info[index].key)
      {
        BCM_IF_ERROR_RETURN( bcm_stat_group_create(unit, bcmStatObjectIngVlanXlate, bcmStatGroupModeSingle,
                            &xlate_rule_info[index].stat_counter_id, &xlate_rule_info[index].counters_number));
      }
      else
      {
         BCM_IF_ERROR_RETURN( bcm_stat_group_create(unit, bcmStatObjectIngVlanXlateSecondLookup, bcmStatGroupModeSingle,
                          &xlate_rule_info[index].stat_counter_id, &xlate_rule_info[index].counters_number));
      }

    }
    return BCM_E_NONE;
}
/* This function will read and display the hit-stats of each rule */
bcm_error_t ingress_xlate_stat_get(int unit)
{
    int index=0;
    unsigned int ctr;
    for (;index < rules; index++)
    {
        bcm_gport_t gport;
        BCM_GPORT_MODPORT_SET(gport, unit, xlate_rule_info[index].port);
        /* Read packet stats and display it in hex format */
        BCM_IF_ERROR_RETURN(bcm_vlan_translate_stat_counter_get (unit, gport,  xlate_rule_info[index].key, xlate_rule_info[index].vlanid_key_outer,
                                        xlate_rule_info[index].vlanid_key_inner, bcmVlanStatIngressPackets, 1, &ctr, &xlate_rule_info[index].bcm_stat));
        printf ("\n rule %d - packets:0x%x%x ",index, COMPILER_64_HI(xlate_rule_info[index].bcm_stat.packets64),COMPILER_64_LO(xlate_rule_info[index].bcm_stat.packets64));
        /* Read bytes stats and display it in hex format */
        BCM_IF_ERROR_RETURN(bcm_vlan_translate_stat_counter_get (unit, gport,  xlate_rule_info[index].key, xlate_rule_info[index].vlanid_key_outer,
                                        xlate_rule_info[index].vlanid_key_inner, bcmVlanStatIngressBytes, 1, &ctr, &xlate_rule_info[index].bcm_stat));
        printf ("bytes:0x%x%x\n",COMPILER_64_HI(xlate_rule_info[index].bcm_stat.bytes),COMPILER_64_LO(xlate_rule_info[index].bcm_stat.bytes));
    }
    return BCM_E_NONE;
}

bcm_error_t configure_port_property(int unit, bcm_port_t port, bcm_vlan_t vlan)
{
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port, vlan));
  /* Add outer tag tpid to check if the packets are double tagged or outer tagged */
  BCM_IF_ERROR_RETURN(bcm_port_tpid_add(unit, port, outer_tpid, BCM_COLOR_PRIORITY));
  /* Set inner tag tpid to check if the packets are double tagged or inner tagged */
  BCM_IF_ERROR_RETURN(bcm_port_inner_tpid_set(unit, port, inner_tpid));
  /* Enable ingress packet translation per port */
  BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, port, bcmVlanTranslateIngressEnable,1));
  return BCM_E_NONE;
}
/* This function will create vlan with specified port bitmap */
bcm_error_t create_vlan(int unit, int vlanId, bcm_pbmp_t pbm, bcm_pbmp_t upbm)
{
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit,defVlan,pbm));    /* remove the ports from default vlan on device */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit,vlanId));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit,vlanId,pbm,upbm));
    return BCM_E_NONE;
}
bcm_error_t configure_ingress_xlate_entries(int unit)
{
    bcm_error_t  rv;
	  bcm_pbmp_t pbm,upbm;

    BCM_PBMP_CLEAR(upbm);
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm,port1);
    BCM_PBMP_PORT_ADD(pbm,port2);
    rv = create_vlan(unit, vlanId1, pbm, upbm);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in creating VLAN1 : %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = create_vlan(unit, vlanId2, pbm, upbm);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in creating VLAN2 : %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = reserve_counters(unit);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in reserving xlate counters : %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = configure_port_property(unit, port1, vlanId1);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in configure_port_property function : %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = configure_port_property(unit, port2, vlanId1);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in configure_port_property function : %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = config_xlate_action_and_stat(unit);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in config_xlate_action_and_stat function : %s.\n",bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}
void verify(int unit)
{
  char   str[512];

  bshell(unit, "hm ieee");

  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port2, BCM_PORT_DISCARD_NONE));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port1, BCM_PORT_DISCARD_ALL));
  bshell(unit, "sleep quiet 1");
  printf("Sending an outer tagged packet(outer VLAN:10) on port:%d to verify rule 0\n", port2);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000330000000000448100000A08004500002A0000000040FF79D60000000000000000000102030405060708090A0B0C0D0E0F10111213141535763D54; sleep quiet 1", port2);
  bshell(unit, str);

  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port1, BCM_PORT_DISCARD_NONE));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port2, BCM_PORT_DISCARD_ALL));

  printf("Sending Double Tagged packet(outer VLAN:20 inner vlan:PRIORITY tagged) on port %d to verify rule 1\n", port1);
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000001100000000002281000014910000000800450000260000000040FF79DA0000000000000000000102030405060708090A0B0C0D0E0F1011A3B56D10; sleep quiet 1", port1);
  bshell(unit, str);

  printf("Sending an outer tagged packet(outer VLAN:20) on port:%d to verify rule 2\n", port1);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000550000000000668100001408004500002A0000000040FF79D60000000000000000000102030405060708090A0B0C0D0E0F10111213141535763D54; sleep quiet 1", port1);
  bshell(unit, str);

  printf("Sending Double Tagged packet(outer VLAN:10 inner vlan:20) on port %d to verify rule 3\n", port1);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000770000000000888100000A91000014800450000260000000040FF79DA0000000000000000000102030405060708090A0B0C0D0E0F1011A3B56D10; sleep quiet 1", port1);
  bshell(unit, str);

  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port2, BCM_PORT_DISCARD_NONE));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port1, BCM_PORT_DISCARD_ALL));
  printf("Sending an un-tagged packet on port:%d to verify rule 4\n", port2);
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000003300000000004408004500002A0000000040FF79D60000000000000000000102030405060708090A0B0C0D0E0F10111213141535763D54; sleep quiet 1", port2);
  bshell(unit, str);

  ingress_xlate_stat_get(unit);

}
/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in configure_ingress_xlate_entries())
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

  if (BCM_FAILURE((rv = configure_ingress_xlate_entries(unit)))) {
    printf("configure_ingress_xlate_entries() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing configure_ingress_xlate_entries()) successfully.\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : VLAN range translation.
 *
 *  Usage    : BCM.0> cint vlan_range_xlate.c
 *
 *  config   : vlan_config.bcm
 *
 *  Log file : vlan_range_xlate_log.txt
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
 *  This cint example is to show ingress vlan-range translation using BCM APIs.
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
 *    2) Step2 - Configuration (Done in configure_ingress_vlan_range()):
 *    =====================================================================
 *      a) Create two VLANs(10, 20) and add port1 and port2 as members.
 *
 *      b) On both ports, enable ingress VLAN translation and set 0x8100 as
 *         outer TPID and 0x9100 as inner TPID.
 *
 *      c) Add an vlan range translate entry on port1 such that any packet with
 *         (Outer vlan range: 0 -50, inner vlan range 50 -100), replace outer and inner vlans
 *         with vlan 10.
 *
 *      d) Add below ranges on port2 for both outer and inner vlans.
 *         Action is to add/replace outer vlan with 20.
 *          Range #1 {00 - 10}
 *          Range #2 {11 - 20}
 *          Range #3 {21 - 30}
 *          Range #4 {31 - 40}
 *          Range #5 {41 - 50}
 *          Range #6 {51 - 60}
 *          Range #7 {61 - 70}
 *          Range #8 {71 - 80}
 *
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) Send a double tagged packet(Outer VLAN:20, Inner VLAN:60) on port1.
 *
 *      b) Send a double tagged packet(Outer VLAN:2, Inner VLAN:60) on port2.
 *
 *      c) Send a Single outer tagged packet(Outer VLAN:5) on port2.
 *
 *      d) Send a Single inner tagged packet(Inner VLAN: 25) on port 2.
 *
 *      e) Expected Result:
 *      ===================
 *        After step 3.a, double tagged packet(Outer VLAN:10, Inner VLAN:10)
 *        egresses out of port2.
 *
 *        After step 3.b, double tagged packet(Outer VLAN:20, Inner VLAN:60)
 *        egresses out of port1.
 *
 *        After step 3.c, Single outer tagged packet(Outer VLAN:20)
 *        egresses out of port1.
 *
 *        After step 3.d, Single outer tagged packet(Outer VLAN:20)
 *        egresses out of port1.
 */
/* Reset c interpreter*/
cint_reset();
int port1, port2;
uint16 inner_tpid=0x9100, outer_tpid=0x8100;
/* Vlan id ranges to be added for port1 */
int vlan_ranges [8][2] =    {   {0,10},
                                {11,20},
                                {21,30},
                                {31,40},
                                {41,50},
                                {51,60},
                                {61,70},
                                {71,80} };
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

/* This function will push xlate action rules in VLAN_XLATE table */
int configure_xlate_range_action(int unit, bcm_port_t port, bcm_vlan_t outer_vlan_min,
              bcm_vlan_t outer_vlan_max, bcm_vlan_t inner_vlan_min,
              bcm_vlan_t inner_vlan_max, bcm_vlan_t new_vlan)
{
    bcm_vlan_action_set_t action;
    bcm_gport_t gport;
    bcm_vlan_action_set_t_init(&action);
	/* define inner tag action i.e. replace inner vlanid  with new_inner_vlan */
    action.dt_inner=bcmVlanActionReplace;
	/* define outer tag action i.e. replace outer vlanid  with new_outer_vlan */
    action.dt_outer=bcmVlanActionReplace;
    action.dt_outer_prio = bcmVlanActionReplace;
    action.new_outer_vlan= new_vlan;
    action.new_inner_vlan= new_vlan;
    BCM_GPORT_MODPORT_SET(gport, unit, port);
    /* Ingress range translate: outer vlan id range "outer_vlan_min"  to "outer_vlan_max"
       and inner vlan id range "inner_vlan_min" to "inner_vlan_max" will be considered
       for double tag actions i.e. each port will consume two vlan range profile entries (for inner tag & outer
       tag range declarations)lower limit of the ranges i.e. "outer_vlan_min" and "inner_vlan_max"
       will be used as keys for tag action
    */
    BCM_IF_ERROR_RETURN(bcm_vlan_translate_action_range_add(unit, gport, outer_vlan_min,
                                outer_vlan_max, inner_vlan_min, inner_vlan_max, &action));
    return BCM_E_NONE;
}
/* bcm_vlan_translate_range_add API will use same range for inner and outer tag translations
i.e. in 'SOURCE_TRUNK_MAP' Table corresponding entry will have same OUTER_VLAN_RANGE_IDX and INNER_VLAN_RANGE_IDX value */
int add_range_exhaust(int unit, bcm_vlan_t vlan)
{
    int index = 0;
    for(;index < 8;index++)
    {
        /* Maximum 8 non-overlapping ranges can be added for each port*/
        printf("bcm_vlan_translate_range_add for physical port %d entry %d: %s \n", port2, index+1,
        bcm_errmsg(bcm_vlan_translate_range_add(unit, port2, vlan_ranges[index][0], vlan_ranges[index][1], vlan, 2)));
    }
    return BCM_E_NONE;
}
/* This function will set port specific properties */
bcm_error_t configure_port_property(int unit, bcm_port_t port, bcm_vlan_t vlan)
{
    /* This will add specific tag to the untagged ingressing packets */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port, vlan));
    /* Add outer tag tpid to check if the packets are double tagged or outer tagged */
    BCM_IF_ERROR_RETURN(bcm_port_tpid_add(unit, port, outer_tpid, BCM_COLOR_PRIORITY));
    /* Set inner tag tpid to check if the packets are double tagged or inner tagged */
    BCM_IF_ERROR_RETURN(bcm_port_inner_tpid_set(unit, port, inner_tpid));
    /* Enable ingress packet translation per port */
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, port, bcmVlanTranslateIngressEnable, 1));
    return BCM_E_NONE;
}

/* This function will create vlan with specified port bitmap */
bcm_error_t create_vlan(int unit, int vlanId, bcm_pbmp_t pbm, bcm_pbmp_t upbm)
{
    bcm_vlan_t defVlan = 1;
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit,defVlan,pbm));    /* remove the ports from default vlan on device */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit,vlanId));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit,vlanId,pbm,upbm));
    return BCM_E_NONE;
}

int configure_ingress_vlan_range(int unit)
{
    bcm_error_t  rv;
	  bcm_pbmp_t pbm,upbm;
    bcm_vlan_t  vlanId1=10, vlanId2=20;
    bcm_vlan_t outer_vlan_min = 0, outer_vlan_max = 50;
    bcm_vlan_t inner_vlan_min = 51, inner_vlan_max = 100;
    bcm_vlan_t new_vlan = 10;

    BCM_PBMP_CLEAR(upbm);
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm,port1);
    BCM_PBMP_PORT_ADD(pbm,port2);
    rv = create_vlan(unit, vlanId1, pbm, upbm);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in creating VLAN:%d: %s.\n", vlanId1, bcm_errmsg(rv));
        return rv;
    }

    rv = create_vlan(unit, vlanId2, pbm, upbm);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in creating VLAN:%d: %s.\n", vlanId2, bcm_errmsg(rv));
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
    rv = configure_xlate_range_action(unit, port1, outer_vlan_min, outer_vlan_max,
                                     inner_vlan_min, inner_vlan_max, new_vlan);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in configure_xlate_range_action function : %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = add_range_exhaust(unit, vlanId2);
    return BCM_E_NONE;
}
void  verify(int unit)
{
  char   str[512];
  bshell(unit, "hm ieee");
  bshell(unit, "sleep quiet 1");
  printf("Sending test packet to port1:%d with outer vlan id 20 and inner vlan id 60\n", port1);
  snprintf(str, 512, "tx 1 pbm=%d DATA=0x000003000100000003000000810000149100003C000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B7A9AC5B0;sleep quiet 1", port1);
  bshell(unit, str);

  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port2, BCM_PORT_DISCARD_NONE));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port1, BCM_PORT_DISCARD_ALL));

  printf("\nSending test packet to port2:%d with outer vlan id 2 and inner vlan id 60\n", port2);
  snprintf(str, 512, "tx 1 pbm=%d DATA=0x000003000100000003000000810000029100003C000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B7A9AC5B0; sleep quiet 1", port2);
  bshell(unit, str);

  printf("\nSending test packet to port2:%d with outer vlan id 5\n", port2);
  snprintf(str, 512, "tx 1 pbm=%d DATA=0x00000300010000000300000081000005000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2FF42A0512; sleep quiet 1", port2);
  bshell(unit, str);

  printf("\n\nSending test packet to port2:%d with inner vlan id 25\n", port2);
  snprintf(str, 512, "tx 1 pbm=%d DATA=0x00000300011000000300001091000019000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F20A8459A; sleep quiet 1", port2);
  bshell(unit, str);
}
/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in configure_ingress_vlan_range())
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

  if (BCM_FAILURE((rv = configure_ingress_vlan_range(unit)))) {
    printf("configure_ingress_vlan_range() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing configure_ingress_vlan_range()) successfully.\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


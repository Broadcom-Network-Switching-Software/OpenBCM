/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Shared VLAN learning
 *
 *  Usage    : BCM.0> cint shared_vlan_learning.c
 *
 *  config   : BCM56275_A1-PORT.bcm
 *
 *  Log file : shared_vlan_learning_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *      ingress_port |                              |  egress_port
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *  Summary:
 *  ========
 *    This is a an example script shows how to enable shared vlan learning(SVL) feature.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Select two ports ingress_port and egress_port.
 *
 *    2) Step2 - Configuration (Done in config_svl()):
 *    ================================================
 *      a) Creates two vlans 100 and 200 add ingress_port and egress_port as members.
 *
 *      b) Enable shared VLAN learning on switch.
 *
 *      c) Set 300 as forwarding ID for both vlans 100 & 200.
 *
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) Send vlan 100 and 200 tagged packets.
 *
 *      b) Expected Result:
 *      ===================
 *        Verify that mac is learnt based on forwarding ID 300 using "l2 show".
 */

cint_reset();
bcm_port_t ingress_port, egress_port;
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
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

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

  ingress_port = port_list[0];
  egress_port = port_list[1];

  if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
    printf("ingress_port_setup() failed for port %d\n", ingress_port);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, egress_port)) {
      printf("egress_port_setup() failed for port %d\n", egress_port);
      return -1;
  }

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}
 /* Creates two vlans 100 and 200 add ingress_port and egress_port as members.
 *
 *  Enable shared VLAN learning on switch.
 *
 *  Set 300 as forwarding ID for both vlans 100 & 200.
 */
bcm_error_t config_svl(int unit)
{
  bcm_error_t rv;
  bcm_pbmp_t pbmp,ubmp;
  bcm_vlan_t vlan_100 = 100, vlan_200 = 200;
  bcm_vlan_control_vlan_t control;
  int F_ID = 300;    /* forwarding database ID */

  /* create vlan 100 */
  rv = bcm_vlan_create(unit, vlan_100);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  /* Add port ingress_port to vlan 100 */
  BCM_PBMP_CLEAR(ubmp);
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, ingress_port);
  rv = bcm_vlan_port_add(unit, vlan_100, pbmp, ubmp);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* create vlan 200 */
  rv = bcm_vlan_create(unit, vlan_200);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add port egress_port to vlan 200 */
  BCM_PBMP_CLEAR(ubmp);
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, egress_port);
  rv = bcm_vlan_port_add(unit, vlan_200, pbmp, ubmp);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Switch control to enable Shared VLAN learning feature */
  rv = bcm_switch_control_set(unit, bcmSwitchSharedVlanEnable, TRUE);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  bcm_vlan_control_vlan_t_init(&control);
  control.forwarding_vlan = F_ID;

  /* Setting the forwarding ID for both vlan 100 & 200 to be learned under */
  rv = bcm_vlan_control_vlan_set(unit, vlan_100, control);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_vlan_control_vlan_set(unit, vlan_200, control);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return BCM_E_NONE;
}
 /* Send vlan 100 and 200 tagged packets and verify that mac is
 *  learnt based on forwarding ID 300 using "l2 show".
 */
void verify(int unit)
{
  char   str[512];

  bshell(unit, "hm ieee");


 /* Send 1 VLAN 100 tagged packet.
 *      Ethernet header: DA=00:00:00:00:00:01, SA=00:00:00:00:00:02
 *
 *      000000000001 000000000002
 *      8100 0064
 *      000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F46E12242
 */
  printf("Sending VLAN 100 tagged packet on port %d\n", ingress_port);
  bshell(unit, "sleep quiet 1");
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000000100000000000281000064000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F46E12242; sleep 1", ingress_port);
  bshell(unit, str);
  printf("\nExecuting \"l2 show\" after sending VLAN 100 tagged packet\n");
  bshell(unit, "l2 show");

 /* Send 1 VLAN 200 tagged packet.
 *      Ethernet header: DA=00:00:00:00:00:03, SA=00:00:00:00:00:04
 *
 *      000000000003 000000000004
 *      8100 00c8
 *      000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F27DEDA48
 */
  printf("Sending VLAN 200 tagged packet on port %d\n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x000000000003000000000004810000C8000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F27DEDA48; sleep 1", ingress_port);
  bshell(unit, str);
  printf("\nExecuting \"l2 show\" after sending VLAN 200 tagged packet\n");
  bshell(unit, "l2 show");
}

/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in config_svl())
 * c)demonstrates the functionality (Done in verify()).
 */
bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;

  bshell(unit, "config show; a ; version");

  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }
  printf("Completed test setup successfully.\n");

  if (BCM_FAILURE((rv = config_svl(unit)))) {
    printf("config_svl() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing config_svl()) successfully.\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


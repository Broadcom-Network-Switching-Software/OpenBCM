/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Spanning Tree Group
 *
 *  Usage    : BCM.0> cint stg.c
 *
 *  config   : vlan_config.bcm
 *
 *  Log file : stg_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *      port1        |                              |  port2
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |  port3
 *                   |                              +-----------------+
 *                   |                              |
 *                   +------------------------------+
 *
 *  Summary:
 *  ========
 *    This cint example demonstrates Spanning tree group state management for
 *    physical ports using BCM APIs. This script also demonstrates how to set up
 *    default STG on the device. It displays the STGs present in the system with
 *    respect to ports and VLANs.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Select three ports port1, port2 and port3.
 *
 *    2) Step2 - Configuration (Done in config_stg()):
 *    ================================================
 *      a) Creates two vlans 10 and 20 add port1, port2 and port3 as members.
 *
 *      b) Create a new default STG and print it.
 *
 *      c) Create another STG(25) and add vlan 10 and 20.
 *
 *      d) Set STP state for vlan 10 for port1, port2, port3 as FORWARD.
 *
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) Execute "stg show" and display the VLAN_STG, EGR_VLAN_STG HW tables.
 *
 *      b) Expected Result:
 *      ===================
 *         In VLAN_STG[25], field SP_TREE_PORT<portnum> is set to 3 (i.e FORWARD)
 *         In EGR_VLAN_STG[25], field SP_TREE_PORT<portnum> is set to 3 (i.e FORWARD)
 *         "stg show" displays all the STG ids and the corresponding port's STP state.
 */
/* Reset c interpreter*/
cint_reset();
bcm_port_t port1, port2, port3;
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
 /*    Display the VLAN_STG, EGR_VLAN_STG HW tables.
 *     In VLAN_STG[25], field SP_TREE_PORT<portnum> is set to 3 (i.e FORWARD)
 *     In EGR_VLAN_STG[25], field SP_TREE_PORT<portnum> is set to 3 (i.e FORWARD)
 *     "stg show" displays all the STG ids and the corresponding port's STP state.
 */

void verify(int unit)
{
  bshell(unit,"d chg VLAN_STG");
  bshell(unit,"d chg EGR_VLAN_STG");
  bshell(unit,"stg show");
}
/*
 * This functions gets the port numbers and sets up ports.
 */
bcm_error_t test_setup(int unit)
{
  int port_list[3], i;

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  port1 = port_list[0];
  port2 = port_list[1];
  port3 = port_list[2];

  return BCM_E_NONE;
}


/* Add vlan to STG specified by id*/
bcm_error_t stg_vlan_add(int unit, int vlan_id, bcm_stg_t stg_id)
{
   bcm_error_t rv;
  rv = bcm_stg_create_id(unit, stg_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_stg_default_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_stg_vlan_add(unit, stg_id, vlan_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_stg_vlan_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return BCM_E_NONE;
}
/* Setup new default vlan using existing port bitmap of current default vlan */
bcm_error_t new_default_vlan_setup(int unit, int vid)
{
  bcm_error_t rv;
  bcm_pbmp_t pbmp;
  bcm_pbmp_t upbmp;
  bcm_vlan_t defVlan;

  rv = bcm_vlan_default_get(unit, &defVlan);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_default_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Get the port members for default VLAN */
  rv = bcm_vlan_port_get(unit,defVlan,pbmp,upbmp);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_stg_vlan_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Create a vlan with all the valid port bitmap */
  rv = bcm_vlan_create(unit, vid);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Add ports into VLAN bitmap*/
  rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Make VLAN vid as a default vlan and delete older default vlan*/
  rv = bcm_vlan_default_set(unit,vid);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_default_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_vlan_destroy(unit,defVlan);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_destroy(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  return BCM_E_NONE;
}


bcm_error_t config_stg(int unit)
{
  bcm_error_t rv;
  bcm_pbmp_t  pbmp, ubmp;
  int count;
  bcm_stg_t stg,stgDefault = 2 , newStgId = 25;
  int group_id = 2;
  bcm_vlan_t defVlan, vlanId1=10, vlanId2=20, newDefaultVlan = 2;
  /* Create the vlan with specific id if it is already present then error will be returned */
  rv = bcm_vlan_create(unit, vlanId1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  BCM_PBMP_CLEAR(ubmp);
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, port1);
  BCM_PBMP_PORT_ADD(pbmp, port2);
  BCM_PBMP_PORT_ADD(pbmp, port3);

  rv = bcm_vlan_port_add(unit, vlanId1, pbmp, ubmp);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Create the vlan with specific id if it is already present then error will be returned */
  rv = bcm_vlan_create(unit, vlanId2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  BCM_PBMP_CLEAR(ubmp);
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, port2);
  BCM_PBMP_PORT_ADD(pbmp, port3);

  rv = bcm_vlan_port_add(unit, vlanId2, pbmp, ubmp);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }


  rv = bcm_stg_count_get(unit,&count);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_stg_count_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  printf("\nThis device support %d Spanning tree groups \n", count);

  rv = bcm_vlan_default_get(unit, &defVlan);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_default_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_vlan_stg_get(unit, defVlan, &stg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_stg_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  printf("\nSpanning tree group id:%d has VLAN id:%d\n", stg,defVlan);
  rv = bcm_stg_default_get(unit, &stg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_stg_default_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  printf("\nThis device has default STG with id:%d\n", stg);
  rv = new_default_vlan_setup(unit, newDefaultVlan);
  if (BCM_FAILURE(rv)) {
    printf("Error executing new_default_vlan_setup(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = stg_vlan_add(unit, newDefaultVlan, stgDefault);
  if (BCM_FAILURE(rv)) {
    printf("Error executing stg_vlan_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_stg_default_set(unit, stgDefault);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_stg_default_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_stg_destroy(unit, stg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_stg_destroy(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_stg_default_get(unit, &stg);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_stg_default_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  printf("\nNew default STG has id:%d\n", stg);
  rv = bcm_stg_create_id(unit, newStgId);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_stg_create_id(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Multiple vlans can be associated with same STG group */
  rv = bcm_stg_vlan_add(unit, newStgId, vlanId1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_stg_vlan_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_stg_vlan_add(unit, newStgId, vlanId2);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_stg_vlan_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_vlan_stp_set(unit, vlanId1, port1, BCM_STG_STP_FORWARD);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_stp_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_vlan_stp_set(unit, vlanId1, port2, BCM_STG_STP_FORWARD);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_stp_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_vlan_stp_set(unit, vlanId1, port3, BCM_STG_STP_FORWARD);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_stp_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  return BCM_E_NONE;
}

/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in config_stg())
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

  if (BCM_FAILURE((rv = config_stg(unit)))) {
    printf("config_stg() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing config_stg()) successfully.\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


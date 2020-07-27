/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : BST
 *
 *  Usage    : BCM.0> cint bst.c
 *
 *  config   : bst_config.bcm
 *
 *  Log file : bst_log.txt
 *
 *  Test Topology : Single Helix5 device
 *
 *  Summary:
 *  ========
 *  This cint example illustrates configuration of Buffer Statistics Tracking (BST) mechanism
 *  to aid in resource monitoring and buffer allocation tuning.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one egress port on which bst has to be enabled.
 *    2) Step2 - Configuration (Done in bst_config())
 *    ===============================================
 *      a) Enable device level BST, BST tracking mode and snapshot view for
 *         THDO, THDI and CFAP using switch controls - bst_global_setup
 *      b) Set BST profile for specified mmu resource - bcmBstStatIdEgrPool in this
 *         example
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Check the Bst stats for the specified BST object - bcmBstStatIdEgrPool
 *      b) Expected Result:
 *         ================
 *         If profile threshold is exceeded we see non-zero stats in value1 and value2
 *         (It is assumed that traffic path setup between ingress and egress port and egress port is
 *         congested so that shared buffer usage exceeds the profile threshold in this example)
 */

cint_reset();

uint32 unit = 0;
int rv=0;
bcm_gport_t port_gport;	
int port;
uint64 value1 = "0x00000000:00000000";
uint64 value2 = "0x00000000:00000000";

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
 * This functions gets the port numbers and sets up ingress and
 * egress ports.
 */
bcm_error_t test_setup(int unit)
{
  int port_list[1], i;

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 1)) {
      printf("portNumbersGet() failed\n");
      return -1;
  }

  /* Get port gport */
  port = port_list[0];

  if (BCM_E_NONE != bcm_port_gport_get(unit, port, &port_gport)) {
      printf("bcm_port_gport_get() failed.\n");
      return -1;
  }
  
  return BCM_E_NONE;
}

/* Enable BST, Set BST tracking mode, Enable snapshot view for 
 * THDO, THDI and CFAP
 */
bcm_error_t bst_global_setup(int unit)
{
   bcm_switch_control_set(unit, bcmSwitchBstEnable, TRUE);
   bcm_switch_control_set(unit, bcmSwitchBstTrackingMode , 0);
   bcm_switch_control_set(unit, bcmSwitchBstSnapshotEnable, 1); /* 1 - THDO */
   bcm_switch_control_set(unit, bcmSwitchBstSnapshotEnable, 2); /* 2 - THDI */
   bcm_switch_control_set(unit, bcmSwitchBstSnapshotEnable, 4); /* 4 - CFAP */
   return BCM_E_NONE;
}

/* Set BST profile for object bcmBstStatIdEgrPool */
bcm_error_t bst_profile_set(int unit, bcm_gport_t port_gport)
{
 bcm_cosq_bst_profile_t profile;
 profile.byte=400;

 /* Set BST profile for object bcmBstStatIdEgrPool for specified port */
 rv = bcm_cosq_bst_profile_set(unit, port_gport,-1, bcmBstStatIdEgrPool, profile);
 if (rv != BCM_E_NONE) {
      printf("Error in bcm_cosq_bst_profile_set:%s.\n", bcm_errmsg(rv));
      return rv;
 }
 return BCM_E_NONE;
}

bcm_error_t bst_stat_get(int unit, bcm_gport_t port_gport)
{ 
 rv = bcm_cosq_bst_stat_get(unit, port_gport, -1, bcmBstStatIdEgrPool, 0, &value1);
 if (rv != BCM_E_NONE) {
     printf("Error in bcm_cosq_bst_stat_get:%s.\n", bcm_errmsg(rv));
     return rv;
 }

 rv = bcm_cosq_bst_stat_sync(unit, bcmBstStatIdEgrPool);
 if (rv != BCM_E_NONE) {
     printf("Error in bcm_cosq_bst_stat_sync:%s.\n", bcm_errmsg(rv));
     return rv;
 }

 rv = bcm_cosq_bst_stat_get(unit, port_gport, -1, bcmBstStatIdEgrPool, 0, &value2);
 if (rv != BCM_E_NONE) {
     printf("Error in bcm_cosq_bst_stat_sync:%s.\n", bcm_errmsg(rv));
     return rv;
 }
 print value1;
 print value2;
 return BCM_E_NONE;
}

bcm_error_t bst_config(int unit, bcm_gport_t port_gport)
{
  /* BST setup */
  if (BCM_FAILURE((rv = bst_global_setup(unit)))) {
      printf("bst_global_setup() failed. : %s. \n", bcm_errmsg(rv)); 
      return -1;
  }

  if (BCM_FAILURE((rv = bst_profile_set(unit, port_gport)))) {
      printf("bst_stat_get_on_port() failed. : %s. \n", bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}

/* Function to verify the result */
void verify(int unit)
{
  bshell(unit, "show c");
  bst_stat_get(unit, port_gport);
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit =0;
  bshell(unit, "config show; a ; version");

  if (BCM_FAILURE((rv = test_setup(unit)))) {
      printf("test_setup() failed.\n");
      return -1;
  }

  /* BST setup */
  if (BCM_FAILURE((rv = bst_config(unit, port_gport)))) {
      printf("bst_config() failed.\n");
      return -1;
  }

  /* verify the result */
  verify(unit);

  return BCM_E_NONE;
}
const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


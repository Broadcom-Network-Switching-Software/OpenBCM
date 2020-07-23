/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : Scheduler Profile
 *
 *  Usage    : BCM.0> cint sched_profile.c
 *
 *  config   : sched_profile_config.bcm
 *
 *  Log file : scheduler_profile_log.txt
 *
 *  Test Topology : None
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate creating a scheduler profile and attach it to a port
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one egress port on which new scheduler profile has to be applied
 *    2) Step2 - Configuration (Done in port_sched_config())
 *    ======================================================
 *      a) Create scheduler profile with queue to COS mapping, scheduler mode for
 *         each COS and attach created scheduler profile to port.
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify the profile attachment to port using 'dump sw cosq'
 *      b) Expected Result:
 *         ================
 *         In 'dump sw cosq' output
 *             - UCQ0-7 should be mapped to COS0-7
 *             - MCQ0-3 should be mapped to COS0-3
 */

cint_reset();

bcm_port_t port;
bcm_cosq_mapping_t cosq_mapping_arr[12];
int profile_id=1;
int unit=0;
int rv;

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

bcm_error_t test_setup(int unit)
{
  int port_list[1], i;
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 1)) {
      printf("portNumbersGet() failed\n");
      return -1;
  }

  port = port_list[0];
  return BCM_E_NONE;
}

  
bcm_error_t sched_profile_setup(int unit, int profile_id)
{

 /* Configure Scheduler profile */ 
 cosq_mapping_arr[0].cosq=0;
 cosq_mapping_arr[0].num_ucq=1;
 cosq_mapping_arr[0].num_mcq=1;
 cosq_mapping_arr[0].strict_priority=0;
 cosq_mapping_arr[0].fc_is_uc_only=0;
 cosq_mapping_arr[1].cosq=1;
 cosq_mapping_arr[1].num_ucq=1;
 cosq_mapping_arr[1].num_mcq=1;
 cosq_mapping_arr[1].strict_priority=0;
 cosq_mapping_arr[1].fc_is_uc_only=0;
 cosq_mapping_arr[2].cosq=2;
 cosq_mapping_arr[2].num_ucq=1;
 cosq_mapping_arr[2].num_mcq=1;
 cosq_mapping_arr[2].strict_priority=0;
 cosq_mapping_arr[2].fc_is_uc_only=0;
 cosq_mapping_arr[3].cosq=3;
 cosq_mapping_arr[3].num_ucq=1;
 cosq_mapping_arr[3].num_mcq=1;
 cosq_mapping_arr[3].strict_priority=0;
 cosq_mapping_arr[3].fc_is_uc_only=0;
 cosq_mapping_arr[4].cosq=4;
 cosq_mapping_arr[4].num_ucq=1;
 cosq_mapping_arr[4].num_mcq=0;
 cosq_mapping_arr[4].strict_priority=0;
 cosq_mapping_arr[4].fc_is_uc_only=0;
 cosq_mapping_arr[5].cosq=5;
 cosq_mapping_arr[5].num_ucq=1;
 cosq_mapping_arr[5].num_mcq=0;          
 cosq_mapping_arr[5].strict_priority=0;
 cosq_mapping_arr[5].fc_is_uc_only=0;
 cosq_mapping_arr[6].cosq=6;         
 cosq_mapping_arr[6].num_ucq=1;          
 cosq_mapping_arr[6].num_mcq=0;          
 cosq_mapping_arr[6].strict_priority=0;
 cosq_mapping_arr[6].fc_is_uc_only=0;
 cosq_mapping_arr[7].cosq=7;
 cosq_mapping_arr[7].num_ucq=1;
 cosq_mapping_arr[7].num_mcq=0;
 cosq_mapping_arr[7].strict_priority=0;
 cosq_mapping_arr[7].fc_is_uc_only=0;
 cosq_mapping_arr[8].cosq=8;
 cosq_mapping_arr[8].num_ucq=0;
 cosq_mapping_arr[8].num_mcq=0;
 cosq_mapping_arr[8].strict_priority=0;
 cosq_mapping_arr[8].fc_is_uc_only=0;
 cosq_mapping_arr[9].cosq=9;
 cosq_mapping_arr[9].num_ucq=0;
 cosq_mapping_arr[9].num_mcq=0;
 cosq_mapping_arr[9].strict_priority=0;
 cosq_mapping_arr[9].fc_is_uc_only=0;
 cosq_mapping_arr[10].cosq=10;
 cosq_mapping_arr[10].num_ucq=0;
 cosq_mapping_arr[10].num_mcq=0;
 cosq_mapping_arr[10].strict_priority=0;
 cosq_mapping_arr[10].fc_is_uc_only=0;
 cosq_mapping_arr[11].cosq=11;
 cosq_mapping_arr[11].num_ucq=0;
 cosq_mapping_arr[11].num_mcq=0;
 cosq_mapping_arr[11].strict_priority=0;
 cosq_mapping_arr[11].fc_is_uc_only=0;

 /* create scheduler profile */
 rv = bcm_cosq_schedq_mapping_set(unit, profile_id, 12, cosq_mapping_arr); 
 if (rv != BCM_E_NONE) {
     printf ("sched_profile_setup failed() :%s\n", bcm_errmsg(rv));
     return rv;     
 }
 return BCM_E_NONE;
}

/* Attach scheduler profile to port */
bcm_error_t attach_sched_profile_to_port(int unit, int port, int profile_id) 
{
  /* Attach sched profile to specified port */ 
  rv=bcm_cosq_port_profile_set(unit, port, bcmCosqProfilePFCAndQueueHierarchy, profile_id);
  if (rv != BCM_E_NONE) {
      printf("attach_sched_profile_to_port failed() :%s.\n", bcm_errmsg(rv));  
      return rv;
  }
  return BCM_E_NONE;
}

bcm_error_t port_sched_config(int unit, int port, int profile_id)
{
  /* create scheduler profile */
  if (BCM_FAILURE((rv = sched_profile_setup(unit, profile_id)))) {
      printf("Error in sched_profile_setup() : %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* attach created scheduler profile to port */
  if (BCM_FAILURE((rv = attach_sched_profile_to_port(unit, port, profile_id)))) {
      printf("Error in attach_sched_profile_to_port() : %s.\n", bcm_errmsg(rv)); 
      return rv;
  }
  return BCM_E_NONE;
}

/* Function to verify the result */
void verify(int unit) 
{
  /*check output for port on which new scheduler profile is attached */
  bshell(unit, "dump sw cosq");
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;
 
  bshell(unit, "config show; a ; version");

  /* select port */
  if (BCM_FAILURE((rv = test_setup(unit)))) {
     printf("test_setup() failed.\n");
     return -1;
  }

  /* configure scheduler profile and attach it to port*/
  if (BCM_FAILURE((rv = port_sched_config(unit, port, profile_id)))) {
      printf("port_sched_config() failed.\n");
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


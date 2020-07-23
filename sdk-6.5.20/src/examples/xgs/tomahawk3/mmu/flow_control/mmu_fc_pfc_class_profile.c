/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : PFC Class Profile Configuration
 *
 *  Usage    : BCM.0> cint mmu_fc_pfc_class_profile.c
 *
 *  config   : mmu_fc_pfc_class_profile_config.bcm
 *
 *  Log file : mmu_fc_pfc_class_profile_log.txt
 *
 *  Test Topology : None
 *
 *  Summary:
 *  ========
 *  This cint example demonstrates PFC Rx priority to COS Queue mapping 
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one egress port on which pfc class config profile has to be applied
 *    2) Step2 - Configuration (Done in pfc_class_config())
 *    ======================================================
 *      a) Create pfc rx priority to COS queue mapping profile
 *      b) Attach created profile in step (a) to port.
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify pfc class config profile programming in HW 
 *      b) Expected Result:
 *         ================
 *         In MMU_INTFI_PFCPRI_PROFILE pfc priority 1 should be mapped to COS queue 1,2 and 3. 
 *             - Verify that field PFCPRI_MMUQ_BMP is set to 0xe0e in MMU_INTFI_PFCPRI_PROFILE for entry 9 
 */

cint_reset();

bcm_port_t port;
int profile_id=1;
bcm_cosq_mapping_t cosq_mapping_arr[12];
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

/* PFC class and scheduler profile goes together. 
 * The cosq_port_profile_set is common for PFC and Scheduler i.e. it setups up the same profile 
 * for the port for both Scheduler and PFC. Hence scheduler profile 1 need to be created if we need to 
 * create and attach pfc profile 1 to port.
 */
 
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

/* PFC-rx test:
 * When PFC frames of priority 1 is received, traffic from COS 1, 2, 3 has to be stopped from egressing.
 */

bcm_error_t pfc_class_config(int unit, bcm_port_t port, int profile_id)
{
 int i;

 /*creating scheduler profile 1 before configuing pfc class profile */
 rv = sched_profile_setup(unit, port);
 if (rv != BCM_E_NONE) {
     printf("sched_profile_setup() failed:%s.\n", bcm_errmsg(rv));
     return rv;
 }

 bcm_cosq_pfc_class_map_config_t config_array[8];
 for(i = 0; i < 8; i++) {
   bcm_cosq_pfc_class_map_config_t_init(&config_array[i]);
 }

 config_array[1].pfc_enable = 1;
 config_array[1].pfc_optimized = 1;
 config_array[1].cos_list_bmp = (1U <<1) | (1U <<2) | (1U <<3);
 rv = bcm_cosq_pfc_class_config_profile_set(0, profile_id, 8, config_array);
 if (rv != BCM_E_NONE) {
     printf("bcm_cosq_pfc_class_config_profile_set() failed:%s.\n", bcm_errmsg(rv));
     return rv;
 }

 /* Attach PFC class profile to port */ 
 rv = bcm_cosq_port_profile_set(0, port, bcmCosqProfilePFCAndQueueHierarchy, profile_id);
 if (rv != BCM_E_NONE) {
     printf("bcm_cosq_port_profile_set() failed:%s.\n", bcm_errmsg(rv));
     return rv;
 }

 /* enable PFC-rx for logical port 1 */
 rv = bcm_port_control_set(0, port, bcmPortControlPFCReceive, 1);
 if (rv != BCM_E_NONE) {
     printf("bcm_port_control_set() for bcmPortControlPFCReceive failed:%s.\n", bcm_errmsg(rv));
     return rv;
 }
 return BCM_E_NONE;
}

/* Function to verify the result */
void verify(int unit)
{
  /*Verify PFC class config profile programming */
  bshell(unit, "g chg MMU_INTFI_PFCPRI_PROFILE");
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

  /* configure pfc class profile and attach it to port*/
  if (BCM_FAILURE((rv = pfc_class_config(unit, port, profile_id)))) {
      printf("pfc_class_config() failed.\n");
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


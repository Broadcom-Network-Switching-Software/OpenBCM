/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : Input priority to PG mapping
 *
 *  Usage    : BCM.0> cint mmu_cos_pri_to_pg.c
 *
 *  config   : mmu_cos_pri_to_pg_config.bcm
 *
 *  Log file : mmu_cos_pri_to_pg_log.txt
 *
 *  Test Topology : None
 *
 *  Summary:
 *  ========
 *  This cint example demonstrates Input priorities to PG mapping for UC and MC traffic 
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one port on which priority to PG mapping profile has to be applied
 *    2) Step2 - Configuration (Done in pri_to_pg_config())
 *    ======================================================
 *      a) Create priority to PG mapping profile for UC and MC traffic
 *      b) Attach created profile to port.
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify Input priorites to PG mapping in Profile 6 for UC
 *      b) Verify Input priorites to PG mapping in Profile 6 for MC 
 *      c) Expected Result:
 *         ================
 *         In MMU_THDI_UC_INPPRI_PG_PROFILE(6), fields INPPRIx_PG should be set to 1 where x=0..15 
 *         In MMU_THDI_NONUC_INPPRI_PG_PROFILE(6), fields INPPRIx_PG should be set to 7 where x=0..15 
 */

cint_reset();

bcm_port_t port;
int profile_id=6;
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

/*
 * Map input priorities to PGs, 
 * Map Input priorities 0-15 to PG1 for UC traffic
 * Map Input priorities 0-15 to PG7 for NON_UC traffic
 */

bcm_error_t pri_to_pg_config(int unit, bcm_port_t port, int profile_id)
{
 bcm_cosq_priority_group_mapping_profile_type_t prof_type;
 int arr_count;
 int set_pg_uc[16]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
 int set_pg_mc[16]={7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7};

 /* set port 1 to profile 6 */
 rv = bcm_cosq_port_profile_set(0, 1, bcmCosqProfileIntPriToPGMap, profile_id);
 if (rv != BCM_E_NONE) {
     printf("bcm_cosq_port_profile_set falied():%s.\n", bcm_errmsg(rv));
     return rv;
 }

 /* set profile 6 for uc and non-uc traffic*/
 arr_count=16;
 prof_type=bcmCosqInputPriPriorityGroupUcMapping;
 rv = bcm_cosq_priority_group_mapping_profile_set(0,profile_id,prof_type,arr_count,set_pg_uc);
 if (rv != BCM_E_NONE) {
     printf("bcm_cosq_priority_group_mapping_profile_set falied():%s.\n", bcm_errmsg(rv));
     return rv;
 }
 prof_type=bcmCosqInputPriPriorityGroupMcMapping;
 rv = bcm_cosq_priority_group_mapping_profile_set(0,profile_id,prof_type,arr_count,set_pg_mc);
 if (rv != BCM_E_NONE) {
     printf("bcm_cosq_priority_group_mapping_profile_set falied():%s.\n", bcm_errmsg(rv));
     return rv;
 }
 return BCM_E_NONE;
}   
  
/* Function to verify the result */
void verify(int unit)
{
  /* Verify MMU_THDI_UC_INPPRI_PG_PROFILE input priority to PG mapping for UC */
  bshell(unit, "g chg MMU_THDI_UC_INPPRI_PG_PROFILE");
  /* Verify MMU_THDI_NONUC_INPPRI_PG_PROFILE input priority to PG mapping for MC */
  bshell(unit, "g chg MMU_THDI_NONUC_INPPRI_PG_PROFILE");
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
  if (BCM_FAILURE((rv = pri_to_pg_config(unit, port, profile_id)))) {
      printf("pri_to_pg_config() failed.\n");
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


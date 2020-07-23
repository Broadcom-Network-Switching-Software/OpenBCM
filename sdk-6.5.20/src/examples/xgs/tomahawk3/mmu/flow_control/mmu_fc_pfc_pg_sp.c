/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : PG to service pool mapping and PFC-Tx priority to PG mapping 
 *
 *  Usage    : BCM.0> cint mmu_fc_pfc_pg_sp.c 
 *
 *  config   : mmu_fc_pfc_pg_sp_config.bcm
 *
 *  Log file : mmu_fc_pfc_pg_sp_log.txt
 *
 *  Test Topology : None
 *
 *  Summary:
 *  ========
 *  This cint example demonstrates PG to service pool, headroom pool mapping and PFC-Tx priority to 
 *  PG mapping.  
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one port on which pg-to-sp profile and pfc_tx priority to PG profile has to be applied. 
 *    2) Step2 - Configuration (Done in pfc_pg_sp_config())
 *    ======================================================
 *      a) Create PG to Service pool mapping profile, PG to headroom pool mapping profile and 
 *         pfc-tx priority to PG mapping profile
 *      b) Attach created profile in step (a) to port
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify all PGs are mapped to service pool 1 and headroom pool 0 in profile 1
 *      b) Verify PFC-tx priorities 1,2 and 3 are mapped to PG 7 in profile 1 
 *      c) Expected Result:
 *         ================
 *         In MMU_THDI_PG_PROFILE(1), field PG(0-7)_SPID should be set to 1 and PG(0-7)_HPID should be set to 0.  
 *         In MMU_THDI_PF CPRI_PG_PROFILE(1) field PFCPRI(1-3)_PG should be set to 7    
 */

cint_reset();

bcm_port_t port;
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

/* set PG to Service pool mapping and PG to Headroom pool mapping */ 
bcm_error_t pg_service_pool_config(int unit, int profile_id)
{
 /* set port 1 to PG profile */
 print bcm_cosq_port_profile_set(0, port, bcmCosqProfilePGProperties, profile_id);

 /* Map PG to SP and HPID */
 bcm_cosq_priority_group_mapping_profile_type_t prof_type;
 int arr_count;
 int sp_array[8]={1,1,1,1,1,1,1,1};
 int hp_array[8]={0,0,0,0,0,0,0,0};

 arr_count=8;
 prof_type=bcmCosqPriorityGroupServicePoolMapping;
 rv = bcm_cosq_priority_group_mapping_profile_set(0,profile_id,prof_type,arr_count,sp_array);
 if (rv != BCM_E_NONE) {
     printf("bcm_cosq_priority_group_mapping_profile_set() failed:%s.\n", bcm_errmsg(rv));
    return rv;
  }

 prof_type=bcmCosqPriorityGroupHeadroomPoolMapping;
 rv = bcm_cosq_priority_group_mapping_profile_set(0,profile_id,prof_type,arr_count,hp_array);
 if (rv != BCM_E_NONE) {
     printf("bcm_cosq_priority_group_mapping_profile_set() failed:%s.\n", bcm_errmsg(rv));
    return rv;
 }
 return BCM_E_NONE;
}

/* set PFC-tx priority to PG mapping */
bcm_error_t pfc_tx_prio_to_pg(int unit, int profile_id)
{ 
 int pg_array[8] = {0, 7, 7, 7, 0, 0, 0, 0};
 rv = bcm_cosq_priority_group_pfc_priority_mapping_profile_set(0, profile_id, 8, pg_array);
 if (rv != BCM_E_NONE) {
     printf("bcm_cosq_priority_group_mapping_profile_set() failed:%s.\n", bcm_errmsg(rv));
    return rv;
 }

 /* enable PFC-tx */
 rv = bcm_port_control_set(0, port, bcmPortControlPFCTransmit, 1);
 if (rv != BCM_E_NONE) {
     printf("bcm_port_control_set() for enable PFC-tx failed:%s.\n", bcm_errmsg(rv));
    return rv;
 }

 return BCM_E_NONE;
}

bcm_error_t pfc_pg_sp_config(int unit, bcm_port_t port, int profile_id)
{
  /* create PG to service pool and Headroom pool mapping profile and attach it to port */
  if (BCM_FAILURE((rv = pg_service_pool_config(unit, profile_id)))) {
      printf("Error in pg_service_pool_config(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create PFC-Tx priority to PG mapping and attach it to port */
  if (BCM_FAILURE((rv = pfc_tx_prio_to_pg(unit, profile_id)))) {
      printf("Error in pfc_tx_prio_to_pg(): %s.\n", bcm_errmsg(rv)); 
      return rv;
  }
  return BCM_E_NONE;
}

/* Function to verify the result */
void verify(int unit) 
{
  /* Verify PG to service pool and headroom pool mapping in profile 1 */
  bshell(unit, "g MMU_THDI_PG_PROFILE(1)");
  /* Verify PFC-tx priority to PG mapping in profile 1 */ 
  bshell(unit, "g MMU_THDI_PFCPRI_PG_PROFILE(1)");
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

  /* configure pg to sp profile, pfc-tx to pg profile and attach it to port*/
  if (BCM_FAILURE((rv = pfc_pg_sp_config(unit, port, profile_id)))) {
      printf("pfc_pg_sp_config() failed.\n");
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


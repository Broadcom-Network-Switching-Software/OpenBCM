/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : Shaper
 *
 *  Usage    : BCM.0> cint shaper.c
 *
 *  config   : shaper_config.bcm
 *
 *  Log file : shaper_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below cosq APIs:
 *  ========
 *   bcm_cosq_gport_bandwidth_set
 *   bcm_cosq_gport_bandwidth_get
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate configure and retrieve shaper on a certain cosq of a port
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one egress port on which new shaper will be applied
 *    2) Step2 - Configuration and check (Done in port_bandwidth_config_and_check())
 *    ======================================================
 *      a) Configure the shaper to a certain cosq of a port, and retrieve the configured shaper.
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      NULL
 */

cint_reset();

bcm_port_t port;
int unit=0;
int rv;
bcm_gport_t in_uc_gport;
bcm_cos_queue_t in_cosq = 0;
unsigned int in_mode = BCM_COSQ_BW_PACKET_MODE;
unsigned int in_kbits_min = 0;
unsigned int in_kbits_max = 100;
unsigned int out_kbits_min;
unsigned int out_kbits_max;
unsigned int out_mode;


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
  if (BCM_E_NONE != bcm_port_gport_get(unit, port, &in_uc_gport)) {
      printf("bcm_port_gport_get() failed\n");
      return -1;
  }

  return BCM_E_NONE;
}

bcm_error_t port_bandwidth_config_and_check(int unit)
{
  /* Configure the shaper */
  if (BCM_FAILURE((rv = bcm_cosq_gport_bandwidth_set(unit, in_uc_gport, in_cosq,
                                      in_kbits_min, in_kbits_max, in_mode)))) {
      printf("Error in bcm_cosq_gport_bandwidth_set() : %s.\n", bcm_errmsg(rv));
      return rv;
  }
  printf("port UCQ set: cosq=%d mode=0x%x in_kbits_min=%d in_kbits_max=%d\n",
          in_cosq, in_mode, in_kbits_min, in_kbits_max);

  /* Retrieve the shaper */
  if (BCM_FAILURE((rv = bcm_cosq_gport_bandwidth_get(unit, in_uc_gport, in_cosq,
                                    &out_kbits_min, &out_kbits_max, &out_mode)))) {
      printf("Error in bcm_cosq_gport_bandwidth_get() : %s.\n", bcm_errmsg(rv));
      return rv;
  }
  printf("port UCQ get: cosq=%d mode=0x%x out_kbits_min=%d out_kbits_max=%d\n",
          in_cosq, out_mode, out_kbits_min, out_kbits_max);

  return BCM_E_NONE;
}

/* Function to verify the result */
void verify(int unit)
{
  /*NULL */
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
  if (BCM_FAILURE((rv = port_bandwidth_config_and_check(unit)))) {
      printf("port_bandwidth_config_and_check() failed.\n");
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

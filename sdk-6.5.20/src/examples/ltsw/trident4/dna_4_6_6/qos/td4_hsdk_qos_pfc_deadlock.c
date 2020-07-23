/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/* Feature Name   : PFC(Priority Flow control) Deadlock Detection and Recovery 
 *
 * Usage          : BCM.0>cint td4_hsdk_qos_pfc_deadlock.c 
 *
 * Config         : bcm56880_a0-generic-32x400.config.yml 
 *
 * Log file       : td4_hsdk_qos_pfc_deadlock_log.txt
 *
 * Test Topology  :
 *
 * DMAC=0x1            +------------+         DMAC=0x1
 * SMAC=0x2   +-------->   56880    +-------> SMAC=0x2
 * VLAN=2,Pri=0  cd0(1)|            |cd1(2)   VLAN=2,Pri=0
 *                     | l2:mac=0x1 |
 *                     |    vlan=2  |
 *      <-------+      |    port=2  |     <-------+
 *   TX PFC FRAMES     |            |  RX PFC FRAMES
 *                     +------------+
 *
 *  Summary:
 *  ========
 *  This cint example demonstrates PFC Deadlock Detection feature on TD4
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one ingress and one egress port
 *    2) Step2 - Configuration (Done in pfc_config())
 *    ===============================================
 *      a) Sets up required data path for traffic and enables PFC Tx and Rx on
 *         ports
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) send at linerate to cd0 L2 learnt packets (vlan=1 prio=2 dmac=0x1 smac=0x2)
 *      b) Along with a), send at linerate to cd1 PFC frames with pause-control class 0 enabled
 *      c) Expected Result:
 *         ================
 *         After (a) and (b) 
 *         For the time interval for which PFC deadlock recovery is enabled, port cd1 should
 *         egress packets at configured rate ignoring the PFC XOFF received from Ixia. 
 *         Verify using show c 
 *
 *         Once Pfc deadlock detection and recovery is disabled -
 *         Port cd1 should not egress any packets as pfc xoff frames are being
 *         received. In 'show c' output, there should not be any Tx on cd1 (port-2).
 *
 *  Note: Configure MMU in LOSSLESS for this test by adding below lines to yml file under device 0:
 *        TM_THD_CONFIG:
 *                THRESHOLD_MODE: LOSSLESS
 */

cint_reset();
int rv = 0;
int unit = 0;
bcm_port_t ing_port; /*Ingress port */
bcm_port_t egr_port; /*Egress port */
int pfc_rx_profile_id = 1;

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
  int port_list[2], i;
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
      printf("portNumbersGet() failed\n");
      return -1;
  }

  ing_port = port_list[0];
  egr_port = port_list[1];

  return BCM_E_NONE;
}

bcm_error_t common_data_path_init(int unit)
{
  bcm_vlan_t vid = 2;
  bcm_mac_t dest_mac = "00:00:00:00:00:01"; /*Destination MAC */
  bcm_pbmp_t pbmp,upbmp;
  bcm_l2_addr_t l2addr;

  rv = bcm_vlan_create(unit, vid);
  if (rv != BCM_E_NONE) {
      printf("[bcm_vlan_create] returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(upbmp);
  BCM_PBMP_PORT_ADD(pbmp, ing_port);
  BCM_PBMP_PORT_ADD(pbmp, egr_port);
  rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
  if (rv != BCM_E_NONE) {
      printf("bcm_vlan_port_add returned :%s.\n", bcm_errmsg(rv));
      return rv;
  }

  bcm_l2_addr_t_init(l2addr, dest_mac, vid);
  l2addr.port = egr_port;
  l2addr.flags |= BCM_L2_STATIC;
  rv = bcm_l2_addr_add(unit, &l2addr);
  if (rv != BCM_E_NONE) {
      printf("bcm_l2_addr_add returned :'%s'\n",bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}

/* Enable PFC receive and transmit on the PFC partners */
bcm_error_t pfc_setup(int unit, bcm_port_t ing_port, bcm_port_t egr_port)
{
 uint32 limit_kbits_sec = 100000; /*Bandwidth limit on egress queue - 100 Mbps*/
 int enable = 1;
 int disable = 0;
 bcm_mac_t pause_smac = "00:11:22:33:44:55"; /*Source MAC on all PFC frames transmitted from a port */
 int cos = 2; /*Priority of incoming packet on which testing is done */
 int pri; /* Used for iterations */
 int profile_id = 0;

  /* Map priorities 0 to PG7 */
  int pri_to_pg_array[8] = {7, 1, 2, 3, 4, 5, 6, 7};

  /* set PFC-tx priority to PG mapping */
  rv = bcm_cosq_priority_group_pfc_priority_mapping_profile_set(0, profile_id, 8, pri_to_pg_array);
  if (rv != BCM_E_NONE) {
     printf("bcm_cosq_priority_group_pfc_priority_mapping_profile_set returned '%s'\n",bcm_errmsg(rv));
     return rv;
  }

  /*Create congestion by metering on PFC generate port */
    print bcm_port_rate_egress_set(0,egr_port,64,64);

  /* Disable normal pause on ingress port */
  rv = bcm_port_pause_set(unit, ing_port, disable, disable);
  if (rv != BCM_E_NONE) {
      printf("bcm_port_pause_set~ingress returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  /* Disable normal pause on egress port */
  rv = bcm_port_pause_set(unit, egr_port, disable, disable);
  if (rv != BCM_E_NONE) {
      printf("bcm_port_pause_set~egress returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  /* Enable PFC Tx on ingress port and RX on egress port */
  rv = bcm_port_control_set(unit,ing_port, bcmPortControlPFCTransmit, 1);
  if (rv != BCM_E_NONE) {
      printf("bcm_port_control_set~Tx returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_port_control_set(unit,egr_port, bcmPortControlPFCReceive, 1);
  if (rv != BCM_E_NONE) {
      printf("bcm_port_control_set~Rx returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}

bcm_error_t pfc_config(int unit, bcm_port_t ing_port, bcm_port_t egr_port)
{
  /* common data path setup */
  if (BCM_FAILURE((rv = common_data_path_init(unit)))) {
      printf("common_data_path_init() failed.: %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* pfc config */
  if (BCM_FAILURE((rv = pfc_setup(unit, ing_port, egr_port)))) {
      printf("pfc_setup() failed. : %s. \n", bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}


/* Scheduler mapping for profile profile_id */
/* Default mode with 8 UC and 4 MC */
/* Cos0-3 has 1-UC+1-MC pair, cos4-7 has resp.UC queue */
bcm_error_t sched_and_pfc_rx_profile_setup(int unit, int profile_id)
{
  bcm_cosq_mapping_t cosq_mapping_arr[12];
  cosq_mapping_arr[0].cosq=0;
  cosq_mapping_arr[0].num_ucq=1;
  cosq_mapping_arr[0].num_mcq=1;
  cosq_mapping_arr[1].cosq=1;
  cosq_mapping_arr[1].num_ucq=1;
  cosq_mapping_arr[1].num_mcq=1;
  cosq_mapping_arr[2].cosq=2;
  cosq_mapping_arr[2].num_ucq=1;
  cosq_mapping_arr[2].num_mcq=1;
  cosq_mapping_arr[3].cosq=3;
  cosq_mapping_arr[3].num_ucq=1;
  cosq_mapping_arr[3].num_mcq=1;
  cosq_mapping_arr[4].cosq=4;
  cosq_mapping_arr[4].num_ucq=1;
  cosq_mapping_arr[4].num_mcq=0;
  cosq_mapping_arr[5].cosq=5;
  cosq_mapping_arr[5].num_ucq=1;
  cosq_mapping_arr[5].num_mcq=0; 
  cosq_mapping_arr[6].cosq=6; 
  cosq_mapping_arr[6].num_ucq=1; 
  cosq_mapping_arr[6].num_mcq=0; 
  cosq_mapping_arr[7].cosq=7;
  cosq_mapping_arr[7].num_ucq=1;
  cosq_mapping_arr[7].num_mcq=0;
  cosq_mapping_arr[8].cosq=8;
  cosq_mapping_arr[8].num_ucq=0;
  cosq_mapping_arr[8].num_mcq=0;
  cosq_mapping_arr[9].cosq=9;
  cosq_mapping_arr[9].num_ucq=0;
  cosq_mapping_arr[9].num_mcq=0;
  cosq_mapping_arr[10].cosq=10;
  cosq_mapping_arr[10].num_ucq=0;
  cosq_mapping_arr[10].num_mcq=0;
  cosq_mapping_arr[11].cosq=11;
  cosq_mapping_arr[11].num_ucq=0;
  cosq_mapping_arr[11].num_mcq=0;
  
  /* create scheduler profile */
  rv =  bcm_cosq_schedq_mapping_set(0, profile_id, 12, cosq_mapping_arr);
  if (rv != BCM_E_NONE) {
      printf ("sched_and_pfc_rx_profile_setup() failed: %s\n", bcm_errmsg(rv));
      return rv;
  }

  /* Configure PFC receive for profile profile_id */
  bcm_cosq_pfc_class_map_config_t config_array1[8];
  bcm_cosq_pfc_class_map_config_t_init(&config_array1);

  config_array1[0].pfc_enable = 1;
  config_array1[0].cos_list_bmp |= 1 << 0;
  config_array1[1].pfc_enable = 1;
  config_array1[1].cos_list_bmp |= 1 << 1;
  config_array1[2].pfc_enable = 1;
  config_array1[2].cos_list_bmp |= 1 << 2;
  config_array1[3].pfc_enable = 1;
  config_array1[3].cos_list_bmp |= 1 << 3;
  config_array1[4].pfc_enable = 1;
  config_array1[4].cos_list_bmp |= 1 << 4;
  config_array1[5].pfc_enable = 1;
  config_array1[5].cos_list_bmp |= 1 << 5;
  config_array1[6].pfc_enable = 1;
  config_array1[6].cos_list_bmp |= 1 << 6;
  config_array1[7].pfc_enable = 1;
  config_array1[7].cos_list_bmp |= 1 << 7;

  rv = bcm_cosq_pfc_class_config_profile_set(0, profile_id, 8, config_array1);
  if (rv != BCM_E_NONE) {
      printf ("sched_and_pfc_rx_profile_setup() failed: %s\n", bcm_errmsg(rv));
      return rv;
  }

  /* Map egr_port to use profile 1*/
  rv= bcm_cosq_port_profile_set(0, egr_port, bcmCosqProfilePFCAndQueueHierarchy, profile_id);
  if (rv != BCM_E_NONE) {
      printf ("sched_and_pfc_rx_profile_setup() failed: %s\n", bcm_errmsg(rv)); 
      return rv;
  }
 return BCM_E_NONE;
}

/* Enable PFC deadlock detection and recovery for priority 0 */
bcm_error_t pfc_deadlock_recovery_setup(int unit, bcm_port_t egr_port) 
{
  rv =  bcm_cosq_pfc_deadlock_recovery_event_register(0, NULL, NULL);
  if (rv != BCM_E_NONE) {
     printf("pfc_deadlock_recovery_setup: event register failed: %s.\n", bcm_errmsg(rv));
     return rv;
  }

  /* Configure pfc deadlock detection time  - per (port,pfc priority) HW watchdog timer */
  /* indicates the time for Hardware to monitor a specific Cos queue of a Port */ 
  rv = bcm_cosq_pfc_deadlock_control_set(0, egr_port, 0,
          bcmCosqPFCDeadlockDetectionTimer, 10);
  if (rv != BCM_E_NONE) {
     printf("pfc_deadlock_recovery_setup: Detection timer setting failed: %s.\n", bcm_errmsg(rv));
     return rv;
  }
 
  /* Detection Timer Interval/granularity -1ms, 10ms or 100ms */    
  rv =  bcm_cosq_pfc_deadlock_control_set(0, egr_port, 0,
          bcmCosqPFCDeadlockTimerGranularity, bcmCosqPFCDeadlockTimerInterval1MiliSecond);
  if (rv != BCM_E_NONE) {
     printf("pfc_deadlock_recovery_setup: Timer granularity setting failed: %s.\n", bcm_errmsg(rv));
     return rv;
  }

  /* SW Deadlock recovery timer setting */
  rv = bcm_cosq_pfc_deadlock_control_set(0, egr_port, 0, bcmCosqPFCDeadlockRecoveryTimer, 500);
  if (rv != BCM_E_NONE) {
     printf("pfc_deadlock_recovery_setup: Deadlock revovery timer setting failed: %s.\n", bcm_errmsg(rv));
     return rv;
  }

  bshell (0, "echo 'Show c to indicate packets are not drained from queue due to PFC XOFF received on cd1'");
  bshell(0, "clear c");
  bshell(0, "show c");

  /* Enable PFC deadlock detection and recovery per-port per-PFC priority */
  rv = bcm_cosq_pfc_deadlock_control_set(0, egr_port, 0, 
         bcmCosqPFCDeadlockDetectionAndRecoveryEnable, 1);
  if (rv != BCM_E_NONE) {
     printf("pfc_deadlock_recovery_setup: Deadlock Detection-Recovery Enable failed: %s.\n", bcm_errmsg(rv));
     return rv;
  }

  /* Disable PFC deadlock detection after 2 seconds */
  bshell (0, "sleep 5");
  rv = bcm_cosq_pfc_deadlock_control_set(0, egr_port, 0,
         bcmCosqPFCDeadlockDetectionAndRecoveryEnable, 0);
  if (rv != BCM_E_NONE) {
     printf("pfc_deadlock_recovery_setup: Deadlock Detection-Recovery Disable failed: %s.\n", bcm_errmsg(rv));
     return rv;
  }

  bshell (0, "echo 'Show c to indicate packets drained of PFC queues due to deadlock recovery'");
  bshell(0, "show c cd1");
  return BCM_E_NONE;
}

/* Function to verify the result */
void verify(int unit)
{
  return;
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

  /* pfc config */
  if (BCM_FAILURE((rv = pfc_config(unit, ing_port, egr_port)))) {
      printf("pfc_config() failed.\n");
      return -1;
  }

  if (BCM_FAILURE((rv = sched_and_pfc_rx_profile_setup(unit, pfc_rx_profile_id)))) {
      printf("sched_and_pfc_rx_profile_setup() failed.\n");
      return -1;
  }

  /* configure PFC and PFC deadlock recovery on the port*/
  if (BCM_FAILURE((rv = pfc_deadlock_recovery_setup(unit, egr_port)))) {
      printf("pfc_deadlock_recovery_setup() failed.\n");
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

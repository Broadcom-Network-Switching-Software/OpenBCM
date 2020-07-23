/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : Dynamic mmu settings
 *
 *  Usage    : BCM.0> cint td4_hsdk_qos_mmu_settings.c 
 *
 *  config   : bcm56880_a0-generic-32x400.config.yml
 *
 *  Log file : td4_hsdk_qos_mmu_settings_log.txt
 *
 *  Test Topology : None  
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate dynamic mmu settings using cosq controls 
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one port on which ingress(THDI) and egress(THDO) mmu settings should be changed 
 *    2) Step2 - Configuration (Done in td4_mmu_settings())
 *    ======================================================
 *      a) Set various mmu thresholds using cosq controls 
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify mmu_settings by checking return status of cosq controls 
 *      b) Expected Result:
 *         ================
 *            All cosq control APIs should return success  
 */

 /*++++++++++++++++++++++++Important Note+++++++++++++++++++++++++++++++++++++++++++++++++++++*/ 
 /* Objective of this CINT is only to illustrate the cosq controls available in 
  * TD4 HSDK to set various MMU threholds. So threshold values used in the script are    
  * not having any significance and are used only for demonstration.
  * User is advised to check with Broadcom on their custom MMU settings before changing
  * SDK default settings. 
  *
  * Some of these settings can't be changed dynamically(fixed), while for some other settings, 
  * user is expected to stop traffic and drain before modify the settings. 
  * bcm_cosq_control_dynamic_get - can be used to check if a cosq_control_type property can be
  * changed dynamically 
  *  return type
  *    - bcmCosqDynamicTypeFlexible
  *    - bcmCosqDynamicTypeConditional
  *    - bcmCosqDynamicTypeFixed
  * Cosq control expects thresholds to be specified in terms of bytes. SDK internally converts it
  * into Cells. Cell Size for TD4 is 254 bytes.
  * Examples of threshold settings that cannot be modified dynamically
  *    - Can not dynamically change min guarantee settings from PG Min to PortSP Min for ingress  
  *    - Can not change QMin to QGrpMin for egress. 
  *    - Service pool mapping can not be modified dynamically.
  */
  /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
     
cint_reset();

bcm_port_t port;
bcm_gport_t gport;
int unit=0;
int arg=0;
int rv;
bcm_cosq_dynamic_setting_type_t dynamic;

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


/* Egress admission control - THDO settings */

bcm_error_t mmu_thdo_settings(int unit)
{
  /* Per ITM settings */

  print bcm_port_gport_get(0,port,&gport);
   
  /*Shared limit setting for per chip based egress Service Pool. */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressPoolSharedLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) { 
      printf("Cosq Setting bcmCosqControlEgressPoolSharedLimitBytes\n");
      print bcm_cosq_control_set(unit, gport, 0, bcmCosqControlEgressPoolSharedLimitBytes, 0xffff);
      print bcm_cosq_control_get(unit, gport, 0, bcmCosqControlEgressPoolSharedLimitBytes, &arg);
      print arg;
  }
 
  /* Resume limit setting for per chip based egress Service Pool. */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressPoolResumeLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) { 
      printf("Cosq Setting bcmCosqControlEgressPoolResumeLimitBytes\n");  
      print bcm_cosq_control_set(unit, gport, 0, bcmCosqControlEgressPoolResumeLimitBytes, 1780);
      print bcm_cosq_control_get(unit, gport, 0, bcmCosqControlEgressPoolResumeLimitBytes, &arg);
      print arg;
  }
 
  /* Shared limit setting for per chip based egress Service Pool for yellow packets */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressPoolYellowSharedLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressPoolYellowSharedLimitBytes\n");
      print bcm_cosq_control_set(unit, gport, 0, bcmCosqControlEgressPoolYellowSharedLimitBytes, 4580);
      print bcm_cosq_control_get(unit, gport, 0, bcmCosqControlEgressPoolYellowSharedLimitBytes, &arg);
      print arg;
  }
 
  /* Shared limit setting for per chip based egress Service Pool for red packets */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressPoolRedSharedLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressPoolRedSharedLimitBytes\n"); 
      print bcm_cosq_control_set(unit, gport, 0, bcmCosqControlEgressPoolRedSharedLimitBytes, 3412);
      print bcm_cosq_control_get(unit, gport, 0, bcmCosqControlEgressPoolRedSharedLimitBytes, &arg);
      print arg;
  }
 
  /* Resume limit setting for per chip based egress Service Pool for yellow packets. */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressPoolYellowResumeLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressPoolYellowResumeLimitBytes\n"); 
      print bcm_cosq_control_set(unit, gport, 0, bcmCosqControlEgressPoolYellowResumeLimitBytes, 2780);
      print bcm_cosq_control_get(unit, gport, 0, bcmCosqControlEgressPoolYellowResumeLimitBytes, &arg);
      print arg;
  }
  
  /* Resume limit setting for per chip based egress Service Pool for red packets */
  bcm_cosq_control_dynamic_get(unit, bcmCosqControlEgressPoolRedResumeLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressPoolRedResumeLimitBytes\n"); 
      print bcm_cosq_control_set(unit, gport, 0, bcmCosqControlEgressPoolRedResumeLimitBytes, 1780);
      print bcm_cosq_control_get(unit, gport, 0, bcmCosqControlEgressPoolRedResumeLimitBytes, &arg);
      print arg;
  }

  /* Per port */

  /* Specifies if the queue uses Qmin or Qgrpmin
   * 0: Use Qmin 1; Use QGrpMin
   * Type: static
   */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressUCQueueGroupMinEnable, &dynamic); 
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressUCQueueGroupMinEnable\n");
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressUCQueueGroupMinEnable, 0);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressUCQueueGroupMinEnable, &arg);
      print arg;
  }
 
  /* Specifies the number of DB-cells reserved for a port's group of UC queue that are configured 
   * to use QGroup-min (i.e. USE_QGROUP_MIN set to 1).
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressUCQueueGroupMinLimitBytes, &dynamic); 
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressUCQueueGroupMinLimitBytes\n"); 
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressUCQueueGroupMinLimitBytes, (10*254));
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressUCQueueGroupMinLimitBytes, &arg);
      print arg;
  }
 
  /* Specifies the number of DB-cells and MC-CQE reserved for a port's group of non-UC queue that
   * are configured to use QGroup-min (i.e. USE_QGROUP_MIN set to 1).
  */
  /* Multicast Queues are from 8-11 for TD4 as per default scheduler config */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressMCQueueGroupMinLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressMCQueueGroupMinLimitBytes\n"); 
      print bcm_cosq_control_set(0, gport, 8, bcmCosqControlEgressMCQueueGroupMinLimitBytes, (10*254));
      print bcm_cosq_control_get(0, gport, 8, bcmCosqControlEgressMCQueueGroupMinLimitBytes, &arg);
      print arg;
  }
 
  /* Per Queue */

  /* 
   *Specifies the number of cells reserved for a queue. For UC queues, DB-cells are reserved.
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressUCQueueMinLimitBytes, &dynamic); 
  if (dynamic == bcmCosqDynamicTypeFlexible) { 
      printf("Cosq Setting bcmCosqControlEgressUCQueueMinLimitBytes\n"); 
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressUCQueueMinLimitBytes, 500);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressUCQueueMinLimitBytes, &arg);
      print arg;
  }
 
  /* Enable/Disable dropping packets based on shared limit */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressUCQueueLimitEnable, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressUCQueueLimitEnable\n"); 
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressUCQueueLimitEnable, 1);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressUCQueueLimitEnable, &arg);
      print arg;
  }
 
  /* Enable/Disable dynamic threshold */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressUCSharedDynamicEnable, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) { 
      printf("Cosq Setting bcmCosqControlEgressUCSharedDynamicEnable\n");
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressUCSharedDynamicEnable, 1);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressUCSharedDynamicEnable, &arg);
      print arg;
  }
 
  /* DB cells in bytes if not limit_dynamic */
  bcm_cosq_control_dynamic_get(unit, bcmCosqControlEgressUCQueueSharedLimitBytes, &dynamic); 
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressUCQueueSharedLimitBytes\n");
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressUCQueueSharedLimitBytes, (200*254));
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressUCQueueSharedLimitBytes, &arg);
      print arg;
  }
 
  /* Red limit bytes or percentage of green limit 
           Number of bytes or 0-7
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressUCQueueRedLimit, &dynamic); 
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressUCQueueRedLimit\n"); 
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressUCQueueRedLimit, 4);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressUCQueueRedLimit, &arg);
      print arg;
  }
 
  /* Yellow limit bytes or percentage of green limit 
           Number of bytes or 0-7
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressUCQueueYellowLimit, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressUCQueueYellowLimit\n");
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressUCQueueYellowLimit, 4);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressUCQueueYellowLimit, &arg);
      print arg;
  }
 
  /* Enable color limit 0: Disable 1: Enable */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressUCQueueColorLimitEnable, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {  
      printf("Cosq Setting bcmCosqControlEgressUCQueueColorLimitEnable\n");
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressUCQueueColorLimitEnable, 1);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressUCQueueColorLimitEnable, &arg);
      print arg; 
  }
 
  /* Service pool for queue  Value: 0-3  
     Type: static 
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlUCEgressPool, &dynamic);
  if (dynamic == bcmCosqDynamicTypeConditional) {
      printf("Cosq Setting bcmCosqControlUCEgressPool\n");
      /* Disable MMU traffic on this port and Drain before changing UCQ to Service pool mapping */
      rv = bcm_port_control_set(0, gport, bcmPortControlMmuTrafficEnable, FALSE);
      if (rv == BCM_E_NONE) {
          print bcm_cosq_control_set(0, gport, 0, bcmCosqControlUCEgressPool, 1);
          print bcm_cosq_control_get(0, gport, 0, bcmCosqControlUCEgressPool, &arg);
          print arg;
      }
      /* Enable MMU traffic after changing UCQ to Service pool mapping */  
      rv = bcm_port_control_set(0, gport, bcmPortControlMmuTrafficEnable, TRUE); 
  }
 
  /* Specifies the number of cells reserved for a queue. For MC queues both DB-cells and MC-CQEs are reserved.*/
  /* MC queues 8-11 */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressMCQueueMinLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressMCQueueMinLimitBytes\n");
      print bcm_cosq_control_set(0, gport, 8, bcmCosqControlEgressMCQueueMinLimitBytes, /*10 cells*/ (10*254));
      print bcm_cosq_control_get(0, gport, 8, bcmCosqControlEgressMCQueueMinLimitBytes, &arg);
      print arg;
  }
  
  /* Enable/Disable dropping packets based on shared limit */
  /* 0: Disable 1:Enable,  MC queues 8-11 */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressMCQueueLimitEnable, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressMCQueueLimitEnable\n");
      print bcm_cosq_control_set(0, gport, 8, bcmCosqControlEgressMCQueueLimitEnable, 1);
      print bcm_cosq_control_get(0, gport, 8, bcmCosqControlEgressMCQueueLimitEnable, &arg);
      print arg;
  }
  
  /* DB cells and MC-CQE cells in bytes if not limit_dynamic, else BAF */
  /* MC queues 8-11 */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressMCQueueSharedLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressMCQueueSharedLimitBytes\n");
      print bcm_cosq_control_set(0, gport, 8, bcmCosqControlEgressMCQueueSharedLimitBytes, /*50 cells*/ (50*254));
      print bcm_cosq_control_get(0, gport, 8, bcmCosqControlEgressMCQueueSharedLimitBytes, &arg);
      print arg;
  }
 
  /* Enable/Disable dynamic thresholding for MC queues
     0: Static 1: Dynamic
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressMCSharedDynamicEnable, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressMCSharedDynamicEnable\n");
      print bcm_cosq_control_set(0, gport, 8, bcmCosqControlEgressMCSharedDynamicEnable, 1);
      print bcm_cosq_control_get(0, gport, 8, bcmCosqControlEgressMCSharedDynamicEnable, &arg);
      print arg;
  }
 
  /* Red limit bytes or percentage of green limit for RQE
     Number of bytes or 0-7
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressMCQueueRedLimit, &dynamic); 
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressMCQueueRedLimit\n");
      print bcm_cosq_control_set(0, gport, 8, bcmCosqControlEgressMCQueueRedLimit, 4);
      print bcm_cosq_control_get(0, gport, 8, bcmCosqControlEgressMCQueueRedLimit, &arg);
      print arg;
  }

  /* Yellow limit bytes or percentage of green limit for RQE
      Number of bytes or 0-7
   */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressMCQueueYellowLimit, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressMCQueueYellowLimit\n");
      print bcm_cosq_control_set(0, gport, 8, bcmCosqControlEgressMCQueueYellowLimit, 4);
      print bcm_cosq_control_get(0, gport, 8, bcmCosqControlEgressMCQueueYellowLimit, &arg);
      print arg;
  } 
  
  /* Enable color limit on MC Queue 0: Disable 1: Enable */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressMCQueueColorLimitEnable, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressMCQueueColorLimitEnable\n");
      print bcm_cosq_control_set(0, gport, 8, bcmCosqControlEgressMCQueueColorLimitEnable, 1);
      print bcm_cosq_control_get(0, gport, 8, bcmCosqControlEgressMCQueueColorLimitEnable, &arg);
      print arg;
  }
 
  /* Service pool for MC queue Value: 0-3
     Type - Static 
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlMCEgressPool, &dynamic);
  if (dynamic == bcmCosqDynamicTypeConditional) {
      printf("Cosq Setting bcmCosqControlMCEgressPool\n");
      /* Disable MMU traffic on this port and Drain before changing MCQ to Service pool mapping */ 
      rv = bcm_port_control_set(0, gport, bcmPortControlMmuTrafficEnable, FALSE);
      if (rv == BCM_E_NONE) {
          print bcm_cosq_control_set(0, gport, 8, bcmCosqControlMCEgressPool, 1);
          print bcm_cosq_control_get(0, gport, 8, bcmCosqControlMCEgressPool, &arg);
          print arg;
      }
      /* Enable MMU traffic after changing MCQ to Service pool mapping */
      rv = bcm_port_control_set(0, gport, bcmPortControlMmuTrafficEnable, TRUE); 
  } 

  /* Resume offset for unicast queue
       - Number of bytes 
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressUCQueueResumeOffsetBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) { 
      printf("Cosq Setting bcmCosqControlEgressUCQueueResumeOffsetBytes\n");
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressUCQueueResumeOffsetBytes, 1000);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressUCQueueResumeOffsetBytes, &arg);
      print arg;
  }

  /* Resume offset for multicast queue
       - Number of bytes 
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressMCQueueResumeOffsetBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressMCQueueResumeOffsetBytes\n");
      print bcm_cosq_control_set(0, gport, 8, bcmCosqControlEgressMCQueueResumeOffsetBytes,1000);
      print bcm_cosq_control_get(0, gport, 8, bcmCosqControlEgressMCQueueResumeOffsetBytes, &arg);
      print arg;
  }
 
  /*++++ Per Port Per Service Pool Enums ++++++*/

  /*
    Discard limit per Port per Service Pool
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressPortPoolSharedLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressPortPoolSharedLimitBytes\n");
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressPortPoolSharedLimitBytes, 0x4000);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressPortPoolSharedLimitBytes, &arg);
      print arg;
  }
 
  /* Red packets discard limit per port per service pool for UC and MC */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressPortPoolRedLimitBytes, &dynamic); 
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressPortPoolRedLimitBytes\n");
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressPortPoolRedLimitBytes, 0x4000);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressPortPoolRedLimitBytes, arg);
      print arg;  
  }

  /* Yellow packets discard limit per port per Service Pool for UC and MC */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressPortPoolYellowLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressPortPoolYellowLimitBytes\n");
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressPortPoolYellowLimitBytes, 0x4000);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressPortPoolYellowLimitBytes, arg);
      print arg; 
  }
 
  /* ++++ Per Queue Enums ++++*/
  
  /* Specify if Color Limit Mode setting is static or dynamic for Unicast queue. 
     When Color Mode settings are changed the Red and yellow limit values are reset
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressUCQueueColorLimitDynamicEnable, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressUCQueueColorLimitDynamicEnable\n");
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressUCQueueColorLimitDynamicEnable, 1);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressUCQueueColorLimitDynamicEnable, &arg);
      print arg;
  }
 
  /* Specify if Color Limit Mode setting is static or dynamic for multicast queue.
     When Color Mode settings are changed the Red and yellow limit values are reset
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressMCQueueColorLimitDynamicEnable, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressMCQueueColorLimitDynamicEnable\n");
      print bcm_cosq_control_set(0, gport, 8, bcmCosqControlEgressMCQueueColorLimitDynamicEnable, 1);
      print bcm_cosq_control_get(0, gport, 8, bcmCosqControlEgressMCQueueColorLimitDynamicEnable, &arg);
      print arg;
  }
 
  /* Limit for red cells for Unicast queue
      Static Color mode: Number of bytes
      Dynamic Color mode: Percentage of green cells valid values 0-7
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressUCQueueRedLimit, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressUCQueueRedLimit\n"); 
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressUCQueueRedLimit, 3);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressUCQueueRedLimit, arg);
      print arg;
  }
 
  /* Limit for red cells for Multicast queue */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressMCQueueRedLimit, &dynamic);   
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressMCQueueRedLimit\n");
      print bcm_cosq_control_set(0, gport, 8, bcmCosqControlEgressMCQueueRedLimit, 3);
      print bcm_cosq_control_get(0, gport, 8, bcmCosqControlEgressMCQueueRedLimit, arg);
      print arg;
  }

  /* Limit for yellow cells for unicast queue */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressUCQueueYellowLimit, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressUCQueueYellowLimit\n");
      print bcm_cosq_control_set(0, gport, 0, bcmCosqControlEgressUCQueueYellowLimit, 2);
      print bcm_cosq_control_get(0, gport, 0, bcmCosqControlEgressUCQueueYellowLimit, arg);
      print arg;
  }
 
  /* Limit for yellow cells for multicast queue */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlEgressMCQueueYellowLimit, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlEgressMCQueueYellowLimit\n");
      print bcm_cosq_control_set(0, gport, 8, bcmCosqControlEgressMCQueueYellowLimit, 2);
      print bcm_cosq_control_get(0, gport, 8, bcmCosqControlEgressMCQueueYellowLimit, arg);
      print arg;
  }
  return BCM_E_NONE;
}

 /*  Ingress Admission Control - THDI settings */

bcm_error_t mmu_thdi_settings(int unit) 
{ 
  bcm_gport_t port_gport;
  int pg_headroom = 10*254;
  int pg_min = 45*254;
  int PriorityGrp = 7;
  int cosq=0; /* PG for THDI */ 
  int arg;

  print bcm_port_gport_get(0, port, &port_gport);

  /* Ingress headroom pool limit settings.
     bcmCosqControlIngressHeadroomPoolLimitBytes
   */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressHeadroomPoolLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressHeadroomPoolLimitBytes\n"); 
      print bcm_cosq_control_set(0, port_gport, cosq, bcmCosqControlIngressHeadroomPoolLimitBytes, pg_headroom);
      print bcm_cosq_control_get(0, port_gport, cosq, bcmCosqControlIngressHeadroomPoolLimitBytes, &arg);
      print arg;
  }
 
  /* Ingress Shared Service Pool limit setting.
     bcmCosqControlIngressPoolLimitBytes
   */
   bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPoolLimitBytes, &dynamic);
   if (dynamic == bcmCosqDynamicTypeFlexible) {
       printf("Cosq Setting bcmCosqControlIngressPoolLimitBytes\n");
       print bcm_cosq_control_get(0, port_gport, cosq, bcmCosqControlIngressPoolLimitBytes, &arg);
       arg=arg-1;
      print bcm_cosq_control_set(0, port_gport, cosq, bcmCosqControlIngressPoolLimitBytes, arg);
      print bcm_cosq_control_get(0, port_gport, cosq, bcmCosqControlIngressPoolLimitBytes, &arg);
      print arg;
   }
 
  /* Reset Limit Offset for a service pool
     bcmCosqControlIngressPoolResetOffsetLimitBytes
   */

  /* Set Reset Limit offset */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPoolResetOffsetLimitBytes, &dynamic); 
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressPoolResetOffsetLimitBytes\n");
      print bcm_cosq_control_set(0, port_gport, cosq, bcmCosqControlIngressPoolResetOffsetLimitBytes, 52848);
      print bcm_cosq_control_get(0, port_gport, cosq, bcmCosqControlIngressPoolResetOffsetLimitBytes, &arg);
      print arg; 
  }
 
  /* Ingress Port PG Min limit setting.
     bcmCosqControlIngressPortPGMinLimitBytes
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPortPGMinLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressPortPGMinLimitBytes\n");
      print bcm_cosq_control_set (unit, port_gport, PriorityGrp, bcmCosqControlIngressPortPGMinLimitBytes,pg_min);
      print bcm_cosq_control_get (unit, port_gport, PriorityGrp, bcmCosqControlIngressPortPGMinLimitBytes,&arg);
      print arg;  
  }
 
  /* Ingress Port PG Headroom limit setting
     bcmCosqControlIngressPortPGHeadroomLimitBytes
   */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPortPGHeadroomLimitBytes, &dynamic);  
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressPortPGHeadroomLimitBytes\n");
      print bcm_cosq_control_set (unit, port_gport, PriorityGrp, bcmCosqControlIngressPortPGHeadroomLimitBytes,pg_headroom);
      print bcm_cosq_control_get (unit, port_gport, PriorityGrp, bcmCosqControlIngressPortPGHeadroomLimitBytes,&arg);
      print arg;
  }

  /* Enable Dynamic threshold limit for Ingress Port PG
     bcmCosqControlIngressPortPGSharedDynamicEnable
     0: Disable 1:Enable
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPortPGSharedDynamicEnable, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressPortPGSharedDynamicEnable\n");
      print bcm_cosq_control_set(unit, port_gport, 2, bcmCosqControlIngressPortPGSharedDynamicEnable, 0);
      print bcm_cosq_control_get(unit, port_gport, 2, bcmCosqControlIngressPortPGSharedDynamicEnable, &arg); 
      print arg;
  }
 
  /* Ingress Port PG Shared limit setting.
    bcmCosqControlIngressPortPGSharedLimitBytes
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPortPGSharedLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressPortPGSharedLimitBytes\n");
      print bcm_cosq_control_set(unit, port_gport, 2, bcmCosqControlIngressPortPGSharedLimitBytes, (200*254));
      print bcm_cosq_control_get(unit, port_gport, 2, bcmCosqControlIngressPortPGSharedLimitBytes, &arg);
      print arg;
  }
 
  /* Ingress Port PG Reset Floor settings
     bcmCosqControlIngressPortPGResetFloorBytes
   */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPortPGResetFloorBytes, &dynamic); 
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressPortPGResetFloorBytes\n");
      print bcm_cosq_control_set(0, port_gport, cosq, bcmCosqControlIngressPortPGResetFloorBytes, (100*254));
      print bcm_cosq_control_get(0, port_gport, cosq, bcmCosqControlIngressPortPGResetFloorBytes, &arg);
      print arg;
  }
 
  /* Ingress Port PG resume limit offset
     bcmCosqControlIngressPortPGResetOffsetBytes
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPortPGResetOffsetBytes, &dynamic);  
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressPortPGResetOffsetBytes\n");
      print bcm_cosq_control_set(unit, port_gport, cosq, bcmCosqControlIngressPortPGResetOffsetBytes, (100*254));
      print bcm_cosq_control_get(unit, port_gport, cosq, bcmCosqControlIngressPortPGResetOffsetBytes, &arg);
      print arg;
  }
 
  /* Ingress Port SP Min limit settings
     bcmCosqControlIngressPortPoolMinLimitBytes
  */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPortPoolMinLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressPortPoolMinLimitBytes\n");
      print bcm_cosq_control_set(0, port_gport, cosq, bcmCosqControlIngressPortPoolMinLimitBytes, 0x1040);
      print bcm_cosq_control_get(0, port_gport, cosq, bcmCosqControlIngressPortPoolMinLimitBytes, &arg);
      print arg;
  }
 
  /* Ingress Port SP Resume Limit settings
     bcmCosqControlIngressPortPoolResumeLimitBytes
   */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPortPoolResumeLimitBytes, &dynamic); 
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressPortPoolResumeLimitBytes\n");
      int  val_set = 2000;
      print bcm_cosq_control_set(unit, port_gport, cosq, bcmCosqControlIngressPortPoolResumeLimitBytes, val_set);
      print bcm_cosq_control_get(unit, port_gport, cosq, bcmCosqControlIngressPortPoolResumeLimitBytes, &arg);
      print arg;
  }

  /* Ingress Port SP Shared limit settings
     bcmCosqControlIngressPortPoolSharedLimitBytes
   */
  bcm_cosq_control_dynamic_get(unit,bcmCosqControlIngressPortPoolSharedLimitBytes, &dynamic);
  if (dynamic == bcmCosqDynamicTypeFlexible) {
      printf("Cosq Setting bcmCosqControlIngressPortPoolSharedLimitBytes\n");
      print bcm_cosq_control_set(0, 1, 0, bcmCosqControlIngressPortPoolSharedLimitBytes, 25400);
      print bcm_cosq_control_get(0, 1, 0, bcmCosqControlIngressPortPoolSharedLimitBytes, &arg);
      print arg;
  }
  return BCM_E_NONE;
}

bcm_error_t td4_mmu_settings(int unit)
{
  mmu_thdo_settings(unit);
  mmu_thdi_settings(unit);
  return BCM_E_NONE;
}
 
/* Function to verify the result */
void verify(int unit) 
{
  /* Check for error returned for cosq controls */
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

  /* MMU settings */
  if (BCM_FAILURE((rv = td4_mmu_settings(unit)))) {
     printf("td4_mmu_settings() failed\n");
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

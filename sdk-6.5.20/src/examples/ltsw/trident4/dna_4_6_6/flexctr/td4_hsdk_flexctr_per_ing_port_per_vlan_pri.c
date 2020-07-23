/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Feature  : Flex Counter 2.0
 *
 * Usage    : BCM.0> cint td4_hsdk_flexct_per_ing_port_per_vlan_pri.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : td4_hsdk_flexct_per_ing_port_per_vlan_pri_log.txt
 *
 * Test Topology :
 *                      +-------------------+
 *                      |                   |
 *                      |        TD4        |
 *         ingress port |                   |  
 *       ---->----------+                   | 
 *                      |                   |  
 *                      |                   |  
 *                      |                   |
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate per port per priority Counter implementation
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select a ingress port and configure it in Loopback mode. 
 *     b) Port is configured in the specific vlan/vfi
 *        1. Configure Flex counters with source being bcmFlexctrSourceIngPort and 
 *           object being bcmFlexctrObjectIngIntPri
 *        2. Attach flex counter to port. 
 * 
 *   2) Step2 - Verification (Done in testVerify()):
 *   ===========================================
 *     a) Check the output of bcm_flexctr_stat_get 
 *
 *     b) Expected Result:
 *     ===================
 *     The output of bcm_flexctr_stat_get should match the number of transmitted 
 *     packets
 *
 *   3) Step3 - Clean up (Done in testCleanup()):
 *   ===========================================
 *     a) The scripts detaches the flex counter of the module and destroys it. 
 */

cint_reset();

bcm_port_t ing_port1;
uint32_t stat_counter_id;
bcm_vlan_t vlan = 100;

/*
 * This function checked if given port/index is already present
 * in the list so that same ports number is not generated again
 */
int checkPortAssigned(int *port_index_list,int no_entries, int index)
{
  int i=0;

  for (i= 0; i < no_entries; i++) {
    if (port_index_list[i] == index)
      return 1;
  }

  /* no entry found */
  return 0;
}

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
int portNumbersGet(int unit, int *port_list, int num_ports)
{
  int i = 0, port = 0,rv = 0, index = 0;
  bcm_port_config_t configP;
  bcm_pbmp_t ports_pbmp;
  int tempports[BCM_PBMP_PORT_MAX];
  int port_index_list[num_ports];

  rv = bcm_port_config_get(unit,&configP);
  if(BCM_FAILURE(rv)) {
    printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
    return rv;
  }

  ports_pbmp = configP.e;
  for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
    if (BCM_PBMP_MEMBER(&ports_pbmp, i)) {
      tempports[port] = i;
      port++;
    }
  }

  if (( port == 0 ) || ( port < num_ports )) {
      printf("portNumbersGet() failed \n");
      return -1;
  }

  /* generating random ports */
  for (i= 0; i < num_ports; i++) {
    index = sal_rand()% port;
    if (checkPortAssigned(port_index_list, i, index) == 0)
    {
      port_list[i] = tempports[index];
      port_index_list[i] = index;
    } else {
      i = i - 1;
    }
  }

  return BCM_E_NONE;
}

/*
 * FLex counter configuration for  counter configuration  
 */
int per_port_pri_flex_stat_create(int unit)                 
{
    bcm_error_t rv;
    bcm_flexctr_action_t flexctr_action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;
    
    bcm_flexctr_action_t_init(&flexctr_action);
    
    flexctr_action.source = bcmFlexctrSourceIngPort;
    flexctr_action.mode = bcmFlexctrCounterModeNormal;
    /* Mode can be to count dropped packets or Non drop packets or count all packets */
    flexctr_action.drop_count_mode = bcmFlexctrDropCountModeAll; 
    /* Total number of counters (pri * ports) for now setting it 144 ports */
    flexctr_action.index_num = 16;
    
    index_op = &flexctr_action.index_operation;
    index_op->object[0] = bcmFlexctrObjectIngIntPri;
    index_op->mask_size[0] = 4;    
    index_op->shift[0] = 8;
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;

    /* Increase counter per packet. */
    value_a_op = &flexctr_action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 16;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;

    /* Increase counter per packet bytes. */
    value_b_op = &flexctr_action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;
   
    rv = bcm_flexctr_action_create(unit, options, &flexctr_action, &stat_counter_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in flex counter action create: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    
    return BCM_E_NONE;
}

/*
 * This function is test setup.
 */
int testSetup(int unit)
{
    int  i;
    bcm_error_t rv;
    bcm_field_flexctr_config_t flexctr_cfg;
    bcm_pbmp_t pbmp;
    int port_list[1];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 1)) {
      printf("portNumbersGet() failed\n");
      return -1;
    }
    ing_port1 = port_list[0];
  
    /* Create a vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, 0);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, pbmp));

    /* Create per_port_pri_flex_stat_create */
    per_port_pri_flex_stat_create(unit); 
    
      BCM_PBMP_CLEAR(pbmp);
      BCM_PBMP_PORT_ADD(pbmp, ing_port1);
      /* Add vlan member ports */
      BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, pbmp));
    
    
      /* Attach counter to port */
      bcm_gport_t  gport;
      BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, ing_port1, &gport));
      BCM_IF_ERROR_RETURN(bcm_port_stat_attach(unit, gport, stat_counter_id));

    return BCM_E_NONE;
}

/*
 * This function does the following.
 * Transmit the below unicast packet on ingress_port and verify
 */
int testVerify(int unit)
{
  char str[512];
  bcm_flexctr_counter_value_t counter_value;
  uint32 num_entries = 1;
  bcm_error_t rv;
  int i;
  /* Set port mac loopback mode */
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port1, BCM_PORT_LOOPBACK_MAC));
 
  bshell(unit, "l2 add port=cpu0 mac=0x1 vlanid=100");

   /* counter index is ingress_port + Priority */
  uint32 counter_index;
  for (i=0; i< 8 ;i++){
    counter_index=i;
    /* Send a packet to ingress port*/
    printf("\n******************************\n");
      printf("******Traffic test start******\n");
    printf("Packet with pri=%d comes into ingress port %d \n", counter_index, ing_port1);
    snprintf(str, 512, "tx 10 pbm=%d dm=0x1 vlantag=%d VlanPrio=%d; sleep 5", ing_port1, vlan, counter_index);
    printf("%s\n", str);
    bshell(unit, str);
    bshell(unit, "show c");
    printf("%s\n", str);
    printf(" !!!! Checking flex counter increment for pri= %d packets !!!!!  \n", counter_index);
    /* Collect Statistics for a given field entry counter */
    rv = bcm_flexctr_stat_get(unit, stat_counter_id, num_entries, &counter_index,
                            &counter_value);
     if (BCM_FAILURE(rv)) {
        printf("flexctr stat get failed: [%s]\n", bcm_errmsg(rv));
        return rv;
     } else {
          printf("FlexCtr Get for port %d priority %d: %d packets / %d bytes\n",
              ing_port1, 
              counter_index,
              COMPILER_64_LO(counter_value.value[0]),
              COMPILER_64_LO(counter_value.value[1]));
    }
  }
  
  return BCM_E_NONE;
}

/*
 * Cleanup test setup
 */
int testCleanup(int unit)
{
  bcm_error_t rv;
  bcm_field_flexctr_config_t flexctr_cfg;

  flexctr_cfg.flexctr_action_id = stat_counter_id;

  /* Detach counter action. */
  rv = bcm_port_stat_detach_with_id(unit, ing_port1, stat_counter_id);
  if (BCM_FAILURE(rv)) {
    printf("bcm_port_stat_detach %s\n", bcm_errmsg(rv));
    return rv;
  }
  
  /* Destroy counter action. */
  rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
  if (BCM_FAILURE(rv)) {
    printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
    return rv;
  }
  
  return BCM_E_NONE;
}

/*
 * This functions does the following
 * a)test setup *
 * b)demonstrates the functionality(done in testVerify()).
 * c)clean up
 */
int testExecute()
{
  bcm_error_t rv;
  int unit = 0;

  bshell(unit, "config show; a ; version");

  rv = testSetup(unit);
  if (BCM_FAILURE(rv)) {
    printf("testSetup() failed, %s.\n", bcm_errmsg(rv));
    return rv;
  }
  printf("Completed test setup successfully.\n");

  rv = testVerify(unit);
  if (BCM_FAILURE(rv)) {
    printf("testVerify() failed\n");
    return rv;
  }
  printf("Completed test verify successfully.\n");

  rv = testCleanup(unit);
  if (BCM_FAILURE(rv)) {
    printf("testCleanup() failed, return %s.\n", bcm_errmsg(rv));
    return rv;
  }

  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print testExecute();
}

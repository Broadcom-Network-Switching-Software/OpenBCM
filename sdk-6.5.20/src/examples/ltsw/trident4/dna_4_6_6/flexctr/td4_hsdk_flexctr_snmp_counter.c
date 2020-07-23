/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Feature  : Flex Counter 2.0
 *
 * Usage    : BCM.0> cint td4_hsdk_flexctr_snmp_counter.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : td4_hsdk_flexctr_snmp_counter_log.txt
 *
 * Test Topology :
 *                      +-------------------+
 *                      |                   |
 *                      |        TD4        |
 *         egress port  |                   |  ingress_port
 *       ----<----------+ Quals             +<----------
 *                      |   Ethertype       |  
 *                      |Actions            |  
 *                      | statwithoutctridx |
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate SNMP per port counter implementation using IFP
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select a ingress port and configure it in Loopback mode. 
 *     b) Port is configured in the specific vlan/vfi
 *     c) 1. Create Field group with Ethertype as part of QSET and 
 *           StatGroupWithoutCounterIndex as part of ASET
 *        2. Configure Flex counters with appropriate paremeters giving field
 *           group id as hint during flex counter action create
 *        3. Create field entries and attach flex counters to these entries
 * 
 *     Note: Qualifiers of IFP rule are taken for the purpose of demonstration
 *           of IPv4 and IPv6 snmp counters
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

bcm_port_t ingress_port;
uint32_t snmp_ipv4, snmp_ipv6;
int entries = 2;
bcm_vlan_t vlan = 3;

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
 * FLex counter configuration for snmp counter configuration  
 */
int snmp_stat_create(int unit, bcm_field_group_t group_id)                 
{
    bcm_error_t rv;
    bcm_flexctr_action_t flexctr_action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;
    
    bcm_flexctr_action_t_init(&flexctr_action);
    
    flexctr_action.source = bcmFlexctrSourceIfp;
    flexctr_action.mode = bcmFlexctrCounterModeNormal;
    flexctr_action.hint = group_id;
    /* Mode can be to count dropped packets or Non drop packets or count all packets */
    flexctr_action.drop_count_mode = bcmFlexctrDropCountModeAll; 
    /* Total number of counters */
    flexctr_action.index_num = 166;  
    
    index_op = &flexctr_action.index_operation;
    index_op->object[0] = bcmFlexctrObjectIngIngressPpPort;
    index_op->mask_size[0] = 13;    
    index_op->shift[0] = 0;
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
   
    rv = bcm_flexctr_action_create(unit, options, &flexctr_action, &snmp_ipv4);
    if(BCM_FAILURE(rv)) {
        printf("\nError in flex counter action create: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    
    rv = bcm_flexctr_action_create(unit, options, &flexctr_action, &snmp_ipv6);
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
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry; 
    int  i;
    uint16 ether_type[entries] = {0x0800, 0x86DD};
    uint16 ether_type_mask = 0xFFFF;
    bcm_error_t rv;
    bcm_field_flexctr_config_t flexctr_cfg;
    int port_list[1];
    bcm_pbmp_t pbmp;
    int lb_mode = BCM_PORT_LOOPBACK_MAC;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 1)) {
      printf("portNumbersGet() failed\n");
      return -1;
    }
  
    ingress_port = port_list[0];
    printf("Choosing port %d as ingress_port\n", ingress_port);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);

    /* Create a vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan));

    /* Add vlan member ports */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, pbmp));
    
    /* Set port mac loopback mode */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, lb_mode));

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyEtherType);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionStatGroupWithoutCounterIndex); 

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));
    rv = snmp_stat_create(unit, group_config.group);
    if (BCM_FAILURE(rv)) {
        printf("Error in snmp stat create %s\n", bcm_errmsg(rv));
        return rv;
    }

    for (i = 1; i <= entries; i++) {    
        BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group_config.group, i));        
        BCM_IF_ERROR_RETURN(bcm_field_qualify_EtherType(unit, i, ether_type[i-1], ether_type_mask));
        if (ether_type[i-1] == 0x0800)
          flexctr_cfg.flexctr_action_id = snmp_ipv4;
        else
          flexctr_cfg.flexctr_action_id = snmp_ipv6;
        BCM_IF_ERROR_RETURN(bcm_field_entry_flexctr_attach(unit, i, &flexctr_cfg));
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, i));
     }
     
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
   /* counter index is ingress_port are 1:1 mapping */
  uint32 counter_index = ingress_port;
  bcm_error_t rv;
 
  /* Send a packet to ingress port*/
  printf("\n******************************\n");
  printf("******Traffic test start******\n");
  printf("V4 Packet comes into ingress port %d \n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001002a1077770008004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869; sleep 5", ingress_port);
  printf("%s\n", str);
  bshell(unit, str);
  bshell(unit, "show c");
  printf("%s\n", str);
  printf(" !!!! Checking flex counter increment for SNMPV4 !!!!!  \n");
  /* Collect Statistics for a given field entry counter */
  rv = bcm_flexctr_stat_get(unit, snmp_ipv4, num_entries, &counter_index,
                            &counter_value);
  if (BCM_FAILURE(rv)) {
     printf("flexctr stat get failed: [%s]\n", bcm_errmsg(rv));
  } else {
     printf("SNMPV4 FlexCtr Get for port %d : %d packets / %d bytes\n",
            ingress_port, 
            COMPILER_64_LO(counter_value.value[0]),
            COMPILER_64_LO(counter_value.value[1]));
  }
  printf("\n******************************\n");
  printf("******Traffic test start******\n");
  printf(" V6 Packet comes into ingress port %d \n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001002a1077770086DD4500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869; sleep 5", ingress_port);
  printf("%s\n", str);
  bshell(unit, str);
  bshell(unit, "show c");
  printf("%s\n", str);
  printf(" !!!! Checking flex counter increment for SNMPV6 !!!!!  \n");
  /* Collect Statistics for a given field entry counter */
  rv = bcm_flexctr_stat_get(unit, snmp_ipv6, num_entries, &counter_index,
                            &counter_value);
  if (BCM_FAILURE(rv)) {
     printf("flexctr stat get failed: [%s]\n", bcm_errmsg(rv));
  } else {
     printf("SNMPV6 FlexCtr Get for port %d : %d packets / %d bytes\n",
            ingress_port,
            COMPILER_64_LO(counter_value.value[0]),
            COMPILER_64_LO(counter_value.value[1]));
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

  flexctr_cfg.flexctr_action_id = snmp_ipv4;

  /* Detach counter action. */
  rv = bcm_field_entry_flexctr_detach(unit, 1, &flexctr_cfg);
  if (BCM_FAILURE(rv)) {
    printf("bcm_field_stat_detach %s\n", bcm_errmsg(rv));
    return rv;
  }
  
  /* Destroy counter action. */
  rv = bcm_flexctr_action_destroy(unit, snmp_ipv4);
  if (BCM_FAILURE(rv)) {
    printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
    return rv;
  }
  
  flexctr_cfg.flexctr_action_id = snmp_ipv6;

  /* Detach counter action. */
  rv = bcm_field_entry_flexctr_detach(unit, 2, &flexctr_cfg);
  if (BCM_FAILURE(rv)) {
    printf("bcm_field_stat_detach %s\n", bcm_errmsg(rv));
    return rv;
  }
  
  /* Destroy counter action. */
  rv = bcm_flexctr_action_destroy(unit, snmp_ipv6);
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

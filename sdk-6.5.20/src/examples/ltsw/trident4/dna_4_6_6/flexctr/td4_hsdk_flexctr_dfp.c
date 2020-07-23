/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Feature  : Flex Counter 2.0
 *
 * Usage    : BCM.0> cint td4_hsdk_flexctr_dfp.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : td4_hsdk_flexctr_dfp_log.txt
 *
 * Test Topology :
 *                      +-------------------+
 *                      |                   |
 *                      |        TD4        |
 *        network port  |                   |  access_port
 *       ----<----------+                   +<----------
 *                      |                   |  
 *                      |                   |  
 *                      |                   |
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate flex counter usage for dfp
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select ports needed and configure them in Loopback mode. 
 *     b) Port is configured in the specific vlan/vfi
 *     c) 1. Configure trunk, IFP and DFP as needed
 *        2. Configure Flex counters with appropriate paremeters and attach the
 *           counter to DFP entry
 *        3. Create field entries and attach flex counters to these entries
 *
 *     Note: Qualifiers of IFP and DFP are taken for the purpose of demonstration
 *           and can be modified as needed
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

uint32_t flexctr_action_id;
bcm_port_t ingress_port;
bcm_port_t egress_port[5];
bcm_trunk_t orig_trunk_id = 1, redirect_trunk_id = 2; 
int orig_trunk_members = 3, redirect_trunk_members = 2;
bcm_gport_t redirect_trunk_gport;
bcm_vlan_t vlan = 2;
bcm_mac_t dst_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
int opaque_object = 2;

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

int trunk_config(int unit, bcm_trunk_t tgid, int members, int *port_list)
{
  int i = 0;
  bcm_trunk_member_t member_array[members];
  bcm_trunk_info_t trunk_info;
  BCM_IF_ERROR_RETURN(bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tgid));

  bcm_trunk_info_t_init(&trunk_info);
  trunk_info.psc = BCM_TRUNK_PSC_RANDOMIZED;
  trunk_info.dlf_index = BCM_TRUNK_UNSPEC_INDEX;
  trunk_info.mc_index = BCM_TRUNK_UNSPEC_INDEX;
  trunk_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;

  for (i = 0; i < members; i++) {
      bcm_trunk_member_t_init(&member_array[i]);
      BCM_GPORT_MODPORT_SET(member_array[i].gport, 0, port_list[i]);
  }
  BCM_IF_ERROR_RETURN(bcm_trunk_set(unit, tgid, &trunk_info, members,
                                    member_array));
                                    
  return BCM_E_NONE;
}

/*
 * FLex counter configuration for Destination Field Module 
 */
int dfp_stat_create(int unit)                 
{
    bcm_error_t rv;
    bcm_flexctr_action_t flexctr_action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;
    
    bcm_flexctr_action_t_init(&flexctr_action);
    
    flexctr_action.source = bcmFlexctrSourceFieldDestination;
    flexctr_action.mode = bcmFlexctrCounterModeNormal;
    /* Mode can be to count dropped packets or Non drop packets or count all packets */
    flexctr_action.drop_count_mode = bcmFlexctrDropCountModeAll; 
    /* Total number of counters */
    flexctr_action.index_num = 1;  
    
    index_op = &flexctr_action.index_operation;
    index_op->object[0] = bcmFlexctrObjectIngDstFpFlexCtrIndex;
    index_op->mask_size[0] = 16;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;


    /* Increase counter per packet. */
    value_a_op = &flexctr_action.operation_a;
    /* Increase counter per packet. */
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 15;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;

    /* Increase counter per packet bytes. */
    value_b_op = &flexctr_action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;
   
    rv = bcm_flexctr_action_create(unit, options, &flexctr_action, &flexctr_action_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in flex counter action create: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("Stat Counter Id %d\n", flexctr_action_id);
    
    return BCM_E_NONE;
}

int ifp_config(int unit)
{
    bcm_field_entry_t entry;
    bcm_field_group_config_t group_config;
  
    /* IFP Group Configuration and Creation */
    bcm_field_group_config_t_init(&group_config);

    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset,bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_ASET_INIT(group_config.aset);
    BCM_FIELD_ASET_ADD(group_config.aset,bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset,bcmFieldActionAssignOpaqueObject2);
   
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;

    /* Associating hints wth IFP Group */
    group_config.mode = bcmFieldGroupModeAuto;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));
 
    /* IFP Entry Configuration and Creation */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry)); 
   
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, ingress_port, ingress_port)); 
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstMac(unit, entry, dst_mac, dst_mac));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlan(unit, entry, vlan, vlan));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionAssignOpaqueObject2, opaque_object, 0)); 
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 1, 1)); 

    /* Installing FP Entry to FP TCAM */
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    
    return BCM_E_NONE;
}

int dfp_config(int unit)
{
  bcm_field_destination_match_t  dest_fp_match;
  bcm_field_destination_action_t  dest_fp_action;
  uint32_t flexctr_obj_val;
  bcm_error_t rv;

  bcm_field_destination_match_t_init(&dest_fp_match);
  dest_fp_match.opaque_object_2 = opaque_object;
  dest_fp_match.opaque_object_2_mask = 0xffff;
  dest_fp_match.priority = 100;

  bcm_field_destination_action_t_init(&dest_fp_action);
  dest_fp_action.flags |= BCM_FIELD_DESTINATION_ACTION_REDIRECT;
  dest_fp_action.destination_type = bcmFieldRedirectDestinationTrunk;
  dest_fp_action.gport = redirect_trunk_gport;
  dest_fp_action.copy_to_cpu = 1;
  
  rv = bcm_field_destination_entry_add(unit, 0, &dest_fp_match, &dest_fp_action);
  if (BCM_FAILURE(rv)) {
    printf("DFP entry add failed %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* Create the flex counter action instance for the field destination policy. */
  print dfp_stat_create(unit);
  printf("Stat counter id 0x%x\n", flexctr_action_id);
  
  /* Attach the flex counter ID to the field destination policy. */
  rv = bcm_field_destination_stat_attach(unit, &dest_fp_match, flexctr_action_id);
  if (BCM_FAILURE(rv)) {
    printf("DFP stat attach failed %s\n", bcm_errmsg(rv));
    return rv;
  }

  return BCM_E_NONE;
}

/*
 * This function is pre-test setup.
 */
int testSetup(int unit)
{
  bcm_error_t rv;
  int port_list[6];
  int port_list_1[3];
  int port_list_2[2];
  int i;  
  bcm_pbmp_t pbmp, ubmp;
  bcm_l2_addr_t l2_addr;
  
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 6)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }
  
  for(i=0; i<5; i++){
    egress_port[i] = port_list[i];
  }
  ingress_port = port_list[5];
  
  rv = bcm_vlan_create(unit, vlan);
  if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
      printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
      return rv;
  }
   
  BCM_PBMP_CLEAR(ubmp);
  BCM_PBMP_CLEAR(pbmp);
  for (i = 0; i < 6; i++) {
    BCM_PBMP_PORT_ADD(pbmp, port_list[i]);
  }
   
  BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));
  for (i=0; i< 6; i++) {
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_list[i], BCM_PORT_LOOPBACK_MAC));
  }
    
  for(i=0; i<3; i++){
    port_list_1[i] = egress_port[i];
  }  
  
  rv = trunk_config(unit, orig_trunk_id, orig_trunk_members, port_list_1);
  if (BCM_FAILURE(rv)) {
      printf("Error in orig trunk config %s\n", bcm_errmsg(rv));
      return rv;
  }    
  
  bcm_l2_addr_t_init(l2_addr, dst_mac, vlan);
  l2_addr.tgid = orig_trunk_id;
  l2_addr.flags = BCM_L2_STATIC | BCM_L2_TRUNK_MEMBER;
  BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
  
  for(i=0; i<2; i++){
    port_list_2[i] = egress_port[i+3];
  } 
  
  rv = trunk_config(unit, redirect_trunk_id, redirect_trunk_members, port_list_2);
  if (BCM_FAILURE(rv)) {
      printf("Error in redirect trunk config %s\n", bcm_errmsg(rv));
      return rv;
  }    
    
  BCM_GPORT_TRUNK_SET(redirect_trunk_gport, redirect_trunk_id);
    
  rv = ifp_config(unit);
  if (BCM_FAILURE(rv)) {
       printf("Error in IFP config %s\n", bcm_errmsg(rv));
       return rv;
  } 
    
  rv = dfp_config(unit);
  if (BCM_FAILURE(rv)) {
    printf("Error in DFP config %s\n", bcm_errmsg(rv));
    return rv;
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
  int rv;
  bcm_flexctr_counter_value_t counter_value;
  uint32 num_entries = 1;
  uint32 counter_idx = 0;

  /* Send a packet to ingress port*/
  printf("\n******************************\n");
  printf("******Traffic test start******\n");
  printf("Packet comes into ingress port %d\n", ingress_port);
  snprintf(str, 512, "tx 10 pbm=%d data=0x0000000000010000000000028100000208004500002E0000000040113EA00A0A0A0114141401006500C9001A7A1B000102030405060708090A0B0C0D0E0F1011;sleep 4", ingress_port);
  printf("%s\n", str);
  bshell(unit, str);
  bshell(unit, "show c");

  /* Get counter value. */
  sal_memset(&counter_value, 0, sizeof(counter_value));      
  rv = bcm_flexctr_stat_get(unit, flexctr_action_id, num_entries, &counter_idx,
                            &counter_value);
  if (BCM_FAILURE(rv)) {
      printf("flexctr stat get failed: [%s]\n", bcm_errmsg(rv));
      return rv;
      } else {
          printf("Flex Counters collected on Dest FP Packets / Bytes : 0x%08x / 0x%08x \n",
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
  bcm_field_destination_match_t dest_fp_match;

  /* Detach counter action. */
  bcm_field_destination_match_t_init(&dest_fp_match);
  dest_fp_match.opaque_object_2 = opaque_object;
  dest_fp_match.opaque_object_2_mask = 0xffff;
  dest_fp_match.priority = 100;
  rv = bcm_field_destination_stat_detach(unit, &dest_fp_match);
  if (BCM_FAILURE(rv)) {
      printf("bcm_field_destination_stat_detach %s\n", bcm_errmsg(rv));
      return rv;
  }
  
  /* Destroy counter action. */
  rv = bcm_flexctr_action_destroy(unit, flexctr_action_id);
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
    return -1;
  }
  printf("Completed test setup successfully.\n");

  rv = testVerify(unit);
  if (BCM_FAILURE(rv)) {
    printf("testVerify() failed\n");
    return -1;
  }
  printf("Completed test verify successfully.\n");

  rv = testCleanup(unit);
  if (BCM_FAILURE(rv)) {
    printf("testCleanup() failed, return %s.\n", bcm_errmsg(rv));
    return -1;
  }

  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print testExecute();
}

/*
 * Feature  : Scheduler Profile
 *
 * Usage    : BCM.0> cint th4_hsdk_mmu_sched_profile_config.c
 *
 * Config   : th4_hsdk_mmu_config.yml
 *
 * Log file : th4_hsdk_mmu_sched_profile_config_log.txt
 *
 * Test Topology :
 *                        +-------------------+
 *                        |                   |
 *                        |                   |
 *         ingress_port   |       TH4         | Egress port
 *         -------------->+                   +--------->
 *          VLAN 2        |        L2         |  VLAN 2
 *                        |                   |
 *                        |                   |
 *                        +-------------------+
 *
 * This script can verify below cosq APIs:
 * ========
 * 	bcm_cosq_schedq_mapping_set
 * 	bcm_cosq_schedq_mapping_get
 * 	bcm_cosq_profile_property_dynamic_get
 * 	bcm_cosq_port_profile_set
 * 	bcm_cosq_port_profile_get
 * 
 * Summary:
 * ========
 * This cint example demonstrate creating a scheduler profile and attach it to a port
 *
 *   Detailed steps done in the CINT script:
 *   ---------------------------------------
 *   1) Step1 - Test Setup (Done in test_setup())
 *   ---------------------------------------
 *      a) Select two ports
 *      b) Set up L2 switching path between the ingress and egress ports
 *
 *   ---------------------------------------
 *   2) Step2 - Configuration (Done in test_setup()):
 *   ---------------------------------------
 *    a) Create a scheduler profile queue to COS mapping, scheduler mode for 
 *       each COS and attach created scheduler profile to the egress port
 *    b) Set up per queue flex counter
 *
 *   ---------------------------------------
 *   3) Step3 - Verification (Done in test_verify()):
 *   ---------------------------------------
 *    Expected Result:
 *    a) Retrieve the scheduler profile of egress port, and make sure it matches the configured profile.
 *    b) Transmit packets from the ingress port to egress port with priority 0 and 3,
 *       check the per egress queue flex counter
 *    c) (optional) Verify the profile attachment to port using 'dump sw cosq', output should be:
 *        - UCQ0-7 should be mapped to COS0-7
 *        - MCQ0-3 should be mapped to COS0-3
 *
 *   ---------------------------------------
 *   4) Step4 - Cleanup (Done in test_cleanup())
 *   ---------------------------------------
 *      a) Remove the per queue flex counter
 *      b) Delete L2 forwarding path
 */

cint_reset();

int unit = 0;

int MAX_QUEUES = 12;
bcm_cosq_mapping_t cosq_mapping_arr[MAX_QUEUES];
bcm_cosq_mapping_t cosq_mapping_arr_ret[MAX_QUEUES];
int profile_id = 1;

bcm_port_t ingress_port;
bcm_port_t egress_port;
bcm_vlan_t vid = 2;
uint8 dst_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
uint32_t stat_counter_id;
bcm_flexctr_counter_value_t counter_value;
int rv;

/* This function gives required number of ports. */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if(BCM_FAILURE(rv)) {
       printf("\nError in retrieving port configuration: %s.\n", bcm_errmsg(rv));
       return rv;
    }

    ports_pbmp = configP.e;
    for (i =  1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)&& (port < num_ports)) {
            port_list[port] = i;
          port++;
        }
    }

    if (( port == 0 ) || ( port != num_ports )) {
         printf("portNumbersGet() failed \n");
         return -1;
    }

    return BCM_E_NONE;
}

/*
 * Set up L2 forwarding path
 */
int
l2_setup(int unit)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_l2_addr_t addr;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, upbmp));

    /* Set port default vlan id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port, vid));
    bcm_port_control_set(unit, ingress_port, bcmPortControlTrustIncomingVlan, 1);

    /* Add a L2 address on front panel port. */
    bcm_l2_addr_t_init(&addr, dst_mac, vid);
    addr.port = egress_port;
    addr.flags |= BCM_L2_STATIC;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &addr));

    return BCM_E_NONE;
}

int
l2_cleanup(int unit)
{
    /* Delete L2 address */
    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_vlan(unit, vid, 0));

    /* Set port default vlan id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port, 1));

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_destroy(unit, vid));

    return BCM_E_NONE;
}

int
flex_ctr_setup(int unit, bcm_vlan_t vlan)
{
    bcm_error_t rv;
    bcm_flexctr_action_t flexctr_action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;

    bcm_flexctr_action_t_init(&flexctr_action);

    flexctr_action.source = bcmFlexctrSourceEgrVlan;
    flexctr_action.mode = bcmFlexctrCounterModeNormal;
    flexctr_action.hint = 0;
    /* Total number of counters */
    flexctr_action.index_num = 16;  /* total 16 queues */

    index_op = &flexctr_action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticEgrQueueNum;
    index_op->mask_size[0] = 4;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;

    /* Increase counter per packet. */
    value_a_op = &flexctr_action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 4;
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

    /* Attach counter action to a VLAN. */
    rv = bcm_vlan_stat_attach(unit, vlan, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_stat_attach %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t
flex_ctr_cleanup(int unit)
{
    int rv;

    /* Detach counter action. */
    rv = bcm_vlan_stat_detach_with_id(unit, vid, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_stat_detach_with_id: %s\n", bcm_errmsg(rv));
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

int
flex_ctr_check(int unit, int queue)
{
    int rv;
    uint32 num_entries = 1;
    uint32 counter_index = queue;

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit, stat_counter_id, num_entries, &counter_index, &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }
  
    /* print counter value. */
    printf("FlexCtr Get : %d packets / %d bytes\n",
            COMPILER_64_LO(counter_value.value[0]), COMPILER_64_LO(counter_value.value[1]));

    if (COMPILER_64_LO(counter_value.value[0]) != 1) {
        return -1;
    }

    return BCM_E_NONE;
}

/*
 * Set up the scheduler profile map
 */
bcm_error_t
sched_profile_setup(int unit, int prf_id)
{
    /* Configure Scheduler profile */ 
    cosq_mapping_arr[0].cosq = 0;
    cosq_mapping_arr[0].num_ucq = 1;
    cosq_mapping_arr[0].num_mcq = 1;
    cosq_mapping_arr[0].strict_priority = 0;
    cosq_mapping_arr[0].fc_is_uc_only = 0;

    cosq_mapping_arr[1].cosq = 1;
    cosq_mapping_arr[1].num_ucq = 1;
    cosq_mapping_arr[1].num_mcq = 1;
    cosq_mapping_arr[1].strict_priority = 0;
    cosq_mapping_arr[1].fc_is_uc_only = 0;

    cosq_mapping_arr[2].cosq = 2;
    cosq_mapping_arr[2].num_ucq = 1;
    cosq_mapping_arr[2].num_mcq = 1;
    cosq_mapping_arr[2].strict_priority = 0;
    cosq_mapping_arr[2].fc_is_uc_only = 0;

    cosq_mapping_arr[3].cosq = 3;
    cosq_mapping_arr[3].num_ucq = 1;
    cosq_mapping_arr[3].num_mcq = 1;
    cosq_mapping_arr[3].strict_priority = 0;
    cosq_mapping_arr[3].fc_is_uc_only = 0;

    cosq_mapping_arr[4].cosq = 4;
    cosq_mapping_arr[4].num_ucq = 1;
    cosq_mapping_arr[4].num_mcq = 0;
    cosq_mapping_arr[4].strict_priority = 0;
    cosq_mapping_arr[4].fc_is_uc_only = 0;

    cosq_mapping_arr[5].cosq = 5;
    cosq_mapping_arr[5].num_ucq = 1;
    cosq_mapping_arr[5].num_mcq = 0;          
    cosq_mapping_arr[5].strict_priority = 0;
    cosq_mapping_arr[5].fc_is_uc_only = 0;

    cosq_mapping_arr[6].cosq = 6;         
    cosq_mapping_arr[6].num_ucq = 1;          
    cosq_mapping_arr[6].num_mcq = 0;          
    cosq_mapping_arr[6].strict_priority = 0;
    cosq_mapping_arr[6].fc_is_uc_only = 0;

    cosq_mapping_arr[7].cosq = 7;
    cosq_mapping_arr[7].num_ucq = 1;
    cosq_mapping_arr[7].num_mcq = 0;
    cosq_mapping_arr[7].strict_priority = 0;
    cosq_mapping_arr[7].fc_is_uc_only = 0;

    cosq_mapping_arr[8].cosq = 8;
    cosq_mapping_arr[8].num_ucq = 0;
    cosq_mapping_arr[8].num_mcq = 0;
    cosq_mapping_arr[8].strict_priority = 0;
    cosq_mapping_arr[8].fc_is_uc_only = 0;

    cosq_mapping_arr[9].cosq = 9;
    cosq_mapping_arr[9].num_ucq = 0;
    cosq_mapping_arr[9].num_mcq = 0;
    cosq_mapping_arr[9].strict_priority = 0;
    cosq_mapping_arr[9].fc_is_uc_only = 0;

    cosq_mapping_arr[10].cosq = 10;
    cosq_mapping_arr[10].num_ucq = 0;
    cosq_mapping_arr[10].num_mcq = 0;
    cosq_mapping_arr[10].strict_priority = 0;
    cosq_mapping_arr[10].fc_is_uc_only = 0;

    cosq_mapping_arr[11].cosq = 11;
    cosq_mapping_arr[11].num_ucq = 0;
    cosq_mapping_arr[11].num_mcq = 0;
    cosq_mapping_arr[11].strict_priority = 0;
    cosq_mapping_arr[11].fc_is_uc_only = 0;

    /* create scheduler profile */
    rv = bcm_cosq_schedq_mapping_set(unit, prf_id, MAX_QUEUES, cosq_mapping_arr); 
    if (rv != BCM_E_NONE) {
	printf ("sched_profile_setup failed() :%s\n", bcm_errmsg(rv));
	return rv;     
    }

    return BCM_E_NONE;
}

/*
 * Set the scheduler profile to port
 */
bcm_error_t
port_sched_config(int unit, int port, int prf_id)
{
    bcm_cosq_profile_type_t profile_type = bcmCosqProfilePFCAndQueueHierarchy;
    bcm_cosq_dynamic_setting_type_t dynamic;

    /* create scheduler profile */
    if (BCM_FAILURE((rv = sched_profile_setup(unit, prf_id)))) {
         printf("Error in sched_profile_setup() : %s.\n", bcm_errmsg(rv));
         return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_cosq_profile_property_dynamic_get(unit, profile_type, &dynamic));
    if (dynamic == bcmCosqDynamicTypeFixed) {
        /*
         * Mapping cannot be changed during run time as disable MMU
         * traffic routine cannot guarantee flush based on Source port
         * Example: bcmCosqProfilePriGroupProperties
         */
        printf("The profile %d can not be modified(bcmCosqDynamicTypeFixed)\n", prf_id);
        return -1;
    } else if (dynamic == bcmCosqDynamicTypeConditional) {
        /*
         * Mapping can be changed as long as the port has no traffic in MMU
         * Example: bcmCosqProfileQueueHierarchy, bcmCosqProfilePFCClass,
         *          bcmCosqProfilePFCAndQueueHierarchy
         */
        printf("The profile %d is bcmCosqDynamicTypeConditional\n", prf_id);
        BCM_IF_ERROR_RETURN(bcm_port_control_set (unit, port, bcmPortControlMmuTrafficEnable, 0));

        /* Attach sched profile to specified port */ 
        rv = bcm_cosq_port_profile_set(unit, port, bcmCosqProfilePFCAndQueueHierarchy, prf_id);
        if (rv != BCM_E_NONE) {
             printf("bcm_cosq_port_profile_set failed: %s.\n", bcm_errmsg(rv));  
             return rv;
        }

        BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port, bcmPortControlMmuTrafficEnable, 1));
    } else if (dynamic == bcmCosqDynamicTypeFlexible) {
        /* Mapping can be changed at run time. Example : bcmCosqProfileIntPriToPGMap */
        printf("The profile %d is bcmCosqDynamicTypeFlexible\n", prf_id);

        /* Attach sched profile to specified port */ 
        rv = bcm_cosq_port_profile_set(unit, port, bcmCosqProfilePFCAndQueueHierarchy, prf_id);
        if (rv != BCM_E_NONE) {
             printf("bcm_cosq_port_profile_set failed: %s.\n", bcm_errmsg(rv));  
             return rv;
        }
    }

    return BCM_E_NONE;
}

bcm_error_t
sched_profile_get(int unit, int prf_id)
{
    int cos, size = 0;

    for(cos = 0; cos < MAX_QUEUES; cos++) {
        bcm_cosq_mapping_t_init(cosq_mapping_arr_ret[cos]);
        if (rv != BCM_E_NONE) {
            printf ("bcm_cosq_mapping_t_init failed() for cosq_mapping_arr_ret[%d] :%s\n", 
                    cos, bcm_errmsg(rv));
            return rv;     
        }
    }

    /* get scheduler profile */
    rv = bcm_cosq_schedq_mapping_get(unit, prf_id, MAX_QUEUES, &cosq_mapping_arr_ret, &size); 
    if (rv != BCM_E_NONE) {
        printf ("sched_profile_setup failed() :%s\n", bcm_errmsg(rv));
        return rv;     
    }

    if ((size < 1) || (size > MAX_QUEUES)) {
        printf ("sched_profile_setup invalid profile entry\n");
        return rv;     
    }

    printf ("--> Detailed info for sched mapping profile id %d\n", prf_id);
    for(cos = 0; cos < size; cos++) {
        printf ("cosq_mapping_arr_ret[%d]: cosq %d, num_ucq %d, num_mcq %d, strict_priority %d, fc_is_uc_only %d \n", 
                 cos, cosq_mapping_arr_ret[cos].cosq, 
                 cosq_mapping_arr_ret[cos].num_ucq, 
                 cosq_mapping_arr_ret[cos].num_mcq, 
                 cosq_mapping_arr_ret[cos].strict_priority, 
                 cosq_mapping_arr_ret[cos].fc_is_uc_only);
    }

    return BCM_E_NONE;
}

bcm_error_t
port_sched_get(int unit, int port, int prf_id)
{
    int profile_id_ret;

    /* Get scheduler profile attached to port */
    if (BCM_FAILURE(rv = bcm_cosq_port_profile_get(unit, port, 
                                    bcmCosqProfilePFCAndQueueHierarchy, &profile_id_ret))) {
        printf("Error in bcm_cosq_port_profile_get() : %s.\n", bcm_errmsg(rv)); 
        return rv;
    }

    if(profile_id_ret != prf_id) {
        printf("Schedule profile attached to port %d does not match %d\n", port, profile_id_ret);
        return -1;
    }
    printf("==============================================================\n"); 
    printf("--> Scheduler profile attached to port %d is %d.\n", port, profile_id_ret); 

    /* Get scheduler profile */
    rv = sched_profile_get(unit, profile_id_ret);
    if (rv != BCM_E_NONE) {
      printf("Error in sched_profile_get() : %s.\n", bcm_errmsg(rv));
      return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t
test_setup(int unit)
{
     int port_list[2];

     if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
          printf("portNumbersGet() failed\n");
          return -1;
     }
     ingress_port = port_list[0];
     egress_port = port_list[1];
     printf("Ingress port : %d\n", ingress_port);
     printf("Egress port: %d\n", egress_port);

     printf("==> Build and attach profile ID %d to port %d\n", profile_id, egress_port);

     /* configure scheduler profile and attach it to port*/
     if (BCM_FAILURE((rv = port_sched_config(unit, egress_port, profile_id)))) {
          printf("port_sched_config() failed.\n");
          return -1;
     }

     printf("==> Set up L2 forwarding path:\n");

     /* Set up L2 forwarding */
     if (BCM_E_NONE != l2_setup(unit)) {
          printf("l2_setup() failed.\n");
          return -1;
     }

     /* Set up per queue flex counter */
     if (BCM_E_NONE != flex_ctr_setup(unit, vid)) {
          printf("flex_ctr_setup() failed.\n");
          return -1;
     }

     return BCM_E_NONE;
}

/* Function to verify the result */
bcm_error_t
test_verify(int unit) 
{
    char str[256];

    /* Retrieve scheduler profile information on the egress port */
    if (BCM_FAILURE((rv = port_sched_get(unit, egress_port, profile_id)))) {
        printf("Error in port_sched_get() : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("==> Verify unicast pkt from port %d forwarding to port %d\n", ingress_port, egress_port);

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port, BCM_PORT_DISCARD_ALL));

    printf("--> Packet 1 - priority 0\n");

    /* Send a packet with priority 0 to ingress port */
    snprintf(str, 256, "tx 1 pbm=%d data=0x00000000001100000000002281000002FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2DDFC5C3CB; sleep quiet 3", ingress_port);
    bshell(unit, str);

    BCM_IF_ERROR_RETURN(flex_ctr_check(unit, 0));

    printf("--> Packet 2 - priority 3\n");

    /* Send a packet with priority 3 to ingress port */
    snprintf(str, 256, "tx 1 pbm=%d data=0x00000000001100000000002281006002FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D1168282F; sleep quiet 3", ingress_port);
    bshell(unit, str);

    BCM_IF_ERROR_RETURN(flex_ctr_check(unit, 3));

    printf("Per queue packet counter check successful.\n");

    /* Check output for port on which new scheduler profile is attached */
    /* bshell(unit, "dump sw cosq"); */
}

/*
 * Clean up pre-test setup.
 */
int test_cleanup(int unit)
{
    printf("==>Cleaning up...\n");

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port, BCM_PORT_DISCARD_NONE));

    /* Remove the per queue flex counter */
    if (BCM_E_NONE != flex_ctr_cleanup(unit)) {
        printf("flex_ctr_cleanup() failed.\n");
        return -1;
    }

    /* Delete L2 forwarding */
    if (BCM_E_NONE != l2_cleanup(unit)) {
        printf("l2_cleanup() failed.\n");
        return -1;
    }

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    bshell(unit, "config show; attach; version");

    /* Select port, create and attach scheduler profile to port */
    if (BCM_FAILURE((rv = test_setup(unit)))) {
         printf("test_setup() failed.\n");
         return -1;
    }

    /* verify the result */
    if (BCM_FAILURE((rv = test_verify(unit)))) {
         printf("test_verify() failed.\n");
         return -1;
    }

    /* Cleanup */
    if (BCM_FAILURE((rv = test_cleanup(unit)))) {
         printf("test_cleanup() failed.\n");
       return -1;
    }

    return BCM_E_NONE;   
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}


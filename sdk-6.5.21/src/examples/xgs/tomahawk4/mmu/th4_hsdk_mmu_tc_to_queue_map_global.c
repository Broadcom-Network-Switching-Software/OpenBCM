/*
 * Feature  : traffic class to queue mapping
 *
 * Usage    : BCM.0> cint th4_hsdk_mmu_tc_to_queue_map_global.c
 *
 * Config   : th4_hsdk_mmu_config.yml
 *
 * Log file : th4_hsdk_mmu_tc_to_queue_map_global_log.txt
 *
 * Test Topology :
 *
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
 * This script can verify below cosq gport APIs:
 * =============================================
 * 	 bcm_cosq_mapping_set
 * 	 bcm_cosq_mapping_get
 * 	 bcm_cosq_gport_mapping_get
 *
 * Summary:
 * ========
 * This cint example demonstrate traffic class to queue mapping setting/getting globally.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup())
 *   =====================================
 *    a) Select two ports
 *    b) Set up L2 switching path between the ports
 *
 *   2) Step2 - Configuration (Done in test_setup()):
 *   ======================================================
 *    a) Save the default TC to queue mapping
 *    b) Set the mapping from TC to queue via bcm_cosq_mapping_set
 *    c) Set up per queue flex counter
 *
 *   3) Step3 - Verification (Done in test_verify()):
 *   ======================================================
 *     a) Verify the result via tc_to_q_mapping_get
 *         Retrieve the mapping via bcm_cosq_mapping_get;
 *         Retrieve the mapping via bcm_cosq_gport_mapping_get for all the valid front panel ports.
 *     b) Expected Result:
 *        ================
 *        The retrieved mapping should be aligned with the configured one as below:
 *                TC:  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}
 *           queue:  {3,2,1,0,7,6,5,4,4,3,  2,  1, 1,  2,  3, 4}
 *     c) Transmit a packet to the ingress port and create VLAN, with priority 0,
 *        expect the packet to egress on the queue mapped, verify with flex counter
 *
 *   4) Step4 - Cleanup (Done in test_cleanup())
 *   ==========================================
 *     a) Reset the TC to queue to default mapping
 *     b) Remove the per queue flex counter
 *     c) Delete L2 forwarding path
 */

cint_reset();

int unit = 0;

/* Glabal variable definition */
int MAX_PORTS = 272;
int MAX_FP_PORTS = 256;
int MAX_TC = 16;

int valid_port_number = 0;
int port_list[MAX_FP_PORTS];

/* Tc to Q mapping */
int newTcToQMap[] = {3,2,1,0,7,6,5,4,4,3,2,1,1,2,3,4};
int oldTcToQMap[MAX_TC];

bcm_port_t ingress_port;
bcm_port_t egress_port;
bcm_vlan_t vid = 2;
uint8 dst_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
uint32_t stat_counter_id;
bcm_flexctr_counter_value_t counter_value;

/* This function gives required number of ports. */
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
    for (port= 1; port < MAX_PORTS; port++) {
        port_list[i] = -1;
        if (BCM_PBMP_MEMBER(&ports_pbmp,port)&& (i < num_ports)) {
            port_list[i] = port;
            i++;
        }
    }

    if (i == 0) {
       printf("portNumbersGet() failed \n");
       return -1;
    }
    valid_port_number = i;

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
 * Save the default global TC to Q mapping
 */
int tc_to_q_mapping_save (int unit)
{
    int rc = BCM_E_NONE;
    int tc = 0;
    int queue_id = 0;
    int cosq = -1;

    for(tc = 0; tc < MAX_TC; tc ++) {
        rc = bcm_cosq_mapping_get(unit, tc, &cosq);
        if (rc != BCM_E_NONE) {
          printf("Unable to retrieve mapping via bcm_cosq_mapping_get\r\n");
          return rc;
        }
        oldTcToQMap[tc] = cosq;
    }
    return rc;
}

/*
 * Set TC to Q mapping globally for both UC and MC
 */
int tc_to_q_mapping_set (int unit)
{
    int rc = BCM_E_NONE;
    int tc = 0;
    int queue_id = 0;

    /* Set TC to Q mapping globally for both UC and MC with the same mapping */
    for(tc = 0; tc < MAX_TC; tc ++) {
        queue_id = newTcToQMap[tc];
        rc = bcm_cosq_mapping_set(unit, tc, queue_id);
        if (rc != BCM_E_NONE) {
            printf("Unicast: Unable to map TC to Egress queue (TC,Q) = (%d,%x) - %s\n",
                 tc, queue_id, bcm_errmsg(rc));
            return rc;
        }
        printf("Unicast: Map TC to Egress queue sucessfully (TC,Q) = (%d,%x) - %s\n",
                 tc, queue_id, bcm_errmsg(rc));
    }
    return rc;
}

int tc_to_q_mapping_cleanup (int unit)
{
    int rc = BCM_E_NONE;
    int tc = 0;
    int queue_id = 0;

    printf("Recover the TC to Egress queue mapping to default\n");
    for(tc = 0; tc < MAX_TC; tc ++) {
        queue_id = oldTcToQMap[tc];
        rc = bcm_cosq_mapping_set(unit, tc, queue_id);
        if (rc != BCM_E_NONE) {
            printf("Unicast: Unable to map TC to Egress queue (TC,Q) = (%d,%x) - %s\n",
                 tc, queue_id, bcm_errmsg(rc));
            return rc;
        }
    }
    return rc;
}

int port_tc_to_q_mapping_get (int unit, int port)
{
  int rc = BCM_E_NONE;
  int tc = 0;
  int cosq = -1;
  int queue_gport = 0;

  /* TC to Unicast Q mapping get */
  for(tc = 0; tc < MAX_TC; tc ++) {
      rc = bcm_cosq_gport_mapping_get(unit, port, tc,
                                      BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                      &queue_gport, &cosq);
      if (rc != BCM_E_NONE) {
          printf("Unicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                 tc, port, bcm_errmsg(rc));
          return rc;
      }
      printf("Unicast: TC %d of port %d was mapped to Egress queue %d\n",
             tc, port, cosq, bcm_errmsg(rc));
  }

  /* TC to Multicast Q mapping get */
  for(tc = 0; tc < MAX_TC; tc ++) {
      rc = bcm_cosq_gport_mapping_get(unit, port, tc,
                                      BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                      &queue_gport, &cosq);
      if (rc != BCM_E_NONE) {
          printf("Multicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                 tc, port, bcm_errmsg(rc));
          return rc;
      }
      printf("Multicast: TC %d of port %d was mapped to Egress queue %d\n",
             tc, port, cosq, bcm_errmsg(rc));
  }

  return rc;
}

int tc_to_q_mapping_get (int unit)
{
    int rc = BCM_E_NONE;
    int i = 0;
    int tc = 0;
    int cosq = -1;

    printf("\r\nMapping retrieved via bcm_cosq_mapping_get\r\n");
    printf("==========================================\r\n");
    for(tc = 0; tc < MAX_TC; tc ++) {
        rc = bcm_cosq_mapping_get(unit, tc, &cosq);
        if (rc != BCM_E_NONE) {
          printf("Unable to retrieve mapping via bcm_cosq_mapping_get\r\n");
          return rc;
        }
        printf("Mapping: TC %d was mapped to Egress queue %d global\n",
                 tc, cosq);
    }

    printf("\r\nMapping retrieved via bcm_cosq_gport_mapping_get\r\n");
    printf("==========================================\r\n");
    for (i = 0; i < valid_port_number; i++) {
      rc = port_tc_to_q_mapping_get(unit, port_list[i]);
      if (rc != BCM_E_NONE) {
          printf("Unable to retrieve mapping via bcm_cosq_gport_mapping_get\r\n");
          return rc;
      }
    }

    return rc;
}

bcm_error_t test_setup(int unit)
{
    int rv = BCM_E_NONE;

    printf("==> Set up TC to queue mapping ...\n");

    if (BCM_E_NONE != portNumbersGet(unit, port_list, MAX_FP_PORTS)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }
    ingress_port = port_list[0];
    egress_port = port_list[1];
    printf("Ingress port : %d\n", ingress_port);
    printf("Egress port: %d\n", egress_port);

    if (BCM_FAILURE((rv = tc_to_q_mapping_save(unit)))) {
       printf("tc_to_q_mapping_set() failed.\n");
       return -1;
    }

    /* Set up L2 forwarding */
    if (BCM_E_NONE != l2_setup(unit)) {
        printf("l2_setup() failed.\n");
        return -1;
    }

    /* Set TC to Q mapping globally for both UC and MC with the same mapping */
    if (BCM_FAILURE((rv = tc_to_q_mapping_set(unit)))) {
       printf("tc_to_q_mapping_set() failed.\n");
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
bcm_error_t test_verify(int unit)
{
    char str[256];
    bcm_error_t rv;

    /*
     * Retrieve the mapping via bcm_cosq_mapping_get;
     * Retrieve the mapping via bcm_cosq_gport_mapping_get for all the valid front panel ports.
     */
    if (BCM_FAILURE((rv = tc_to_q_mapping_get(unit)))) {
        printf("tc_to_q_mapping_get() failed.\n");
        return -1;
    }

    printf("==> Verify unicast pkt from port %d forwarding to port %d\n", ingress_port, egress_port);

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port, BCM_PORT_DISCARD_ALL));

    /* Send a packet with priority 0 to ingress port */
    snprintf(str, 256, "tx 1 pbm=%d data=0x00000000001100000000002281000002FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2DDFC5C3CB; sleep quiet 1", ingress_port);
    bshell(unit, str);

    BCM_IF_ERROR_RETURN(flex_ctr_check(unit, newTcToQMap[0]));

    printf("Per queue packet counter check successful.\n");

    return BCM_E_NONE;
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

    /* Restore TC to Queue mapping to default */
    if (BCM_E_NONE != tc_to_q_mapping_cleanup(unit)) {
        printf("tc_to_q_mapping_cleanup() failed.\n");
        return -1;
    }

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "show config; attach; version");

    /* Retrieve all the front panel port lists */
    if (BCM_FAILURE((rv = test_setup(unit)))) {
      printf("test_setup() failed.\n");
      return -1;
    }

    /* Verify the result via tc_to_q_mapping_get and packet test */
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


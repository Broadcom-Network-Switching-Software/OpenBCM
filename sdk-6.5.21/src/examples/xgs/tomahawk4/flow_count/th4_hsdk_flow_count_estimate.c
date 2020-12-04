/*  Feature  : Flow count estimate
 *
 *  Usage    : BCM.0> cint th4_hsdk_flow_count_estimate.c
 *
 *  config   : th4_hsdk_flow_count_estimate_config.yml
 *
 *  Log file : th4_hsdk_flow_count_estimate_log.txt
 *
 *  Test Topology :
 *
 *                      +-------------------+
 *                      |                   |
 *                      |                   |  egress_port1
 *       ingress_port   |                   +--------------
 *       ---------------+        TH4        |    VLAN 2
 *          VLAN 2      |                   |
 *                      |                   |  egress_port2
 *                      |                   +--------------
 *                      |                   |    VLAN 2
 *                      |                   |
 *                      +-------------------+
 *
 *
 *  Summary:
 *  ========
 *    This CINT configures flow count estimate using BCM APIs.
 *    256 counters are configured for stochastic averaging.
 *    For details of the hyperloglog algorithm, refer to
 *    "HyperLogLog: the analysis of a near-optimal cardinality estimation algorithm"
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      1.1) Configure a vlan with 3 ports
 *      1.2) Add a L2 entry for <vlan ,dmac> pointing to egress_port1
 *      1.3) Initialze rtag7 to use 5 tuples to generate hash
 *      1.4) Create a flex counter action for flow count estimate
 *      1.5) Initialize flow count registers:
 *           a) up to 22 bits of consective 0's in the hash are checked
 *           b) 8 bits in the hash are used as the counter index
 *           c) attach the flex counter action created in (1.4)
 *
 *    2) Step2 - Configuration (Null)
 *    ======================================================
 *      Do nothing in configuration step.
 *
 *    3) Step3 - Verification (Done in testVerify())
 *    ==============================================
 *      a) Generate 1000 pkts with randomized L4 ports
 *      b) Expected Result:
 *         ================
 *         b.1) some prefix counts in the counters are non-zero
 *         b.2) sum of packet counts in the counters equals 1000
*/

cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port1, egress_port2;
uint8 mac[6] = {0x00, 0x00, 0x00, 0xbb, 0xbb, 0xbb};
bcm_vlan_t vid = 2;
bcm_pbmp_t pbmp;
uint32_t flexctr_action_id = 0;
uint32 index_cnt = 256;
bcm_flexctr_counter_value_t counter_value[index_cnt];
int max_prefix_leng = 22;

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
bcm_error_t portNumbersGet(int unit, int *port_list_ptr, int num_ports)
{
  int i = 0, port = 0, rv = 0, index = 0;
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
      port_list_ptr[i] = tempports[index];
      port_index_list[i] = index;
    } else {
      i = i - 1;
    }
  }

  return BCM_E_NONE;
}


/* Initialze rtag7 to use 5 tuples to generate hash */
bcm_error_t
rtag7_hash_init(int unit)
{
    int arg;
    bcm_error_t rv;

    /* Enable Bin 2 and 3 for flex hash on IPv4 packets */
    arg = BCM_HASH_FIELD_PROTOCOL | BCM_HASH_FIELD_DSTL4 | BCM_HASH_FIELD_SRCL4 |
          BCM_HASH_FIELD_IP4SRC_LO | BCM_HASH_FIELD_IP4SRC_HI | BCM_HASH_FIELD_IP4DST_LO |
          BCM_HASH_FIELD_IP4DST_HI;

    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, arg);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, arg);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, arg);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, arg);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField0, arg);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField1, arg);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config,
                                BCM_HASH_FIELD_CONFIG_CRC32LO);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config1,
                                BCM_HASH_FIELD_CONFIG_CRC32HI);

    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x19364587);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x66667777);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Enable RTAG7 HASH on Non-unicast traffic */
    rv = bcm_switch_control_get(0, bcmSwitchHashControl, &arg);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_get(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    arg |= BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE;
    rv = bcm_switch_control_set(0, bcmSwitchHashControl, arg);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/* Create flex counters for flow count estimates */
bcm_error_t
flexctr_stat_create(int unit, uint32_t *flexctr_act_id)
{
    bcm_flexctr_action_t action;
    bcm_flexctr_action_t_init(&action);
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;
    bcm_error_t rv;

    action.source = bcmFlexctrSourceFlowCount;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = index_cnt;

    /* Flow count flex counters */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticIngFlowCountPortContext;
    index_op->mask_size[0] = 8; /* 2^^8 == index_cnt */
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;

    /* Estimated flow count. */
    value_a_op = &action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectStaticIngFlowCountPrefixLength;
    value_a_op->mask_size[0] = 5; /* 2^^5 >= max_prefix_leng */
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeMax;

    /* Increase counter per packet */
    value_b_op = &action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectCounterValue;
    value_b_op->object[0] = bcmFlexctrObjectConstOne;
    value_b_op->mask_size[0] = 1;
    value_b_op->shift[0] = 0;
    value_b_op->object[1] = bcmFlexctrObjectConstZero;
    value_b_op->mask_size[1] = 1;
    value_b_op->shift[1] = 0;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;

    rv = bcm_flexctr_action_create(unit, options, &action, flexctr_act_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("flex_counter_id = 0x%x\n", *flexctr_act_id);
    return BCM_E_NONE;
}

/* Clean up flex counters */
bcm_error_t
cleanup_flex_ctr(int unit, uint32_t stat_counter_id)
{
    bcm_error_t rv;

    /* Destroy counter action. */
    rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}


/*
  Flow count estimate configuration

  rtag7 hash:

  63                                  29     22 21               0
  +---------------------------------------------------------------+
  |                                  | counter |  consecutive 0's |
  |                                  |  index  |      check       |
  +---------------------------------------------------------------+
*/
bcm_error_t
flowcount_config(int unit, uint32_t stat_counter_id)
{
    bcm_error_t rv;

    /* configure up to 22 bits of 0's for prefix length */
    rv = bcm_switch_control_set(unit, bcmSwitchFlowCountHashShift, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchFlowCountHashMask, 0xffffffffffc00000);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* bit 29-22 of hash are used as counter index */
    rv = bcm_switch_control_set(unit, bcmSwitchFlowCountPortContextHashShift,
                                max_prefix_leng);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchFlowCountPortContextHashMask, 0xff);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Set the flex counter to be used for flow count estiamtes */
    rv = bcm_switch_control_set(unit, bcmSwitchFlowCountStatAttach, stat_counter_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}


/* Clean up flow count estimate configuration */
bcm_error_t
flowcount_unconfig(int unit)
{
    bcm_error_t rv;

    rv = bcm_switch_control_set(unit, bcmSwitchFlowCountStatAttach, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchFlowCountHashShift, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchFlowCountHashMask, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* bit 29-22 of hash are used as counter index */
    rv = bcm_switch_control_set(unit, bcmSwitchFlowCountPortContextHashShift, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchFlowCountPortContextHashMask, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}


/*
 * This function is pre-test setup.
 */
bcm_error_t
testSetup(int unit)
{
    bcm_l2_addr_t addr;
    int lb_mode = BCM_PORT_LOOPBACK_MAC;
    bcm_error_t rv;
    int num_ports = 3;
    int port_list[num_ports];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }
    ingress_port = port_list[0];
    egress_port1 = port_list[1];
    egress_port2 = port_list[2];

    printf("Ingress port: %d\n",ingress_port);
    printf("Egress port1: %d\n",egress_port1);
    printf("Egress port2: %d\n",egress_port2);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port1);
    BCM_PBMP_PORT_ADD(pbmp, egress_port2);

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));

    /* Add vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, pbmp));

    /* Add a L2 address on front panel port. */
    bcm_l2_addr_t_init(&addr, mac, vid);
    addr.port = egress_port1;
    addr.flags |= BCM_L2_STATIC;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &addr));

    /* Traffic test starts here - set port mac loopback mode. */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, lb_mode));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port1, lb_mode));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port2, lb_mode));

    /* Set Learning options on ingress_port. */
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, ingress_port,
                                           BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
    /* Drop egress_port received packet to avoid packet being forwarded again. */
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port1, 0));
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port2, 0));

    rv = rtag7_hash_init(unit);
    if(BCM_FAILURE(rv)) {
        printf("\nError in rtag7_hash_init(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = flexctr_stat_create(unit, &flexctr_action_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in flexctr_stat_create(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = flowcount_config(unit, flexctr_action_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in flowcount_config(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}


/*
 * Clean up pre-test setup.
 */
int testCleanup(int unit)
{
    bcm_error_t rv;

    rv = flowcount_unconfig(unit);
    if(BCM_FAILURE(rv)) {
        printf("\nError in flowcount_unconfig(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = cleanup_flex_ctr(unit, flexctr_action_id);
    if(BCM_FAILURE(rv)) {
        printf("cleanup_flex_ctr() failed.\n");
        return rv;
    }

    /* clean up learned l2 entries */
    rv = bcm_l2_addr_delete_by_vlan(unit, vid, 0);
    if(BCM_FAILURE(rv)) {
        printf("bcm_l2_addr_delete_by_vlan() failed.\n");
        return rv;
    }

    /* Delete vlan member ports. */
    rv = bcm_vlan_port_remove(unit, vid, pbmp);
    if(BCM_FAILURE(rv)) {
        printf("bcm_vlan_port_remove() failed.\n");
        return rv;
    }

    /* Delete a vlan. */
    rv = bcm_vlan_destroy(unit, vid);
    if(BCM_FAILURE(rv)) {
        printf("bcm_vlan_destroy() failed.\n");
        return rv;
    }

    /* restore port settings */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port1, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port2, BCM_PORT_LOOPBACK_NONE));

    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, ingress_port,
                                           BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port1,
                                           BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port2,
                                           BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
    return BCM_E_NONE;
}


/* Dump the flex counters */
bcm_error_t
flexctr_counter_value_get_show(int unit, uint32_t stat_counter_id)
{
    uint32 i, index[index_cnt];
    bcm_error_t rv;

    for (i = 0; i < index_cnt; i++) {
        index[i] = i;
    }
    sal_memset(counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_sync_get(unit, stat_counter_id, index_cnt, index, counter_value);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_flexctr_stat_sync_get(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    for (i = 0; i < index_cnt; i++) {
        printf("index 0x%x -- Flow count estimate + packet count:\n", i);
        print counter_value[i];
    }
    return BCM_E_NONE;
}


/*
 * Generate 1000 pkts with randomized L4 ports
 * and verify the flex counters
 */
bcm_error_t
testVerify(int unit)
{
    char   str[512];
    int i, udp_dport, udp_sport;
    uint64 total_cnt;
    bcm_flexctr_counter_value_t cvalue;
    int    pkt_cnt = 1000;
    bcm_error_t rv;

    COMPILER_64_ZERO(total_cnt);

    /* Seed the random number generator */
    sal_srand(sal_time_usecs());

    printf("Verify UC pkt from %d forwarding to port %d\n", ingress_port, egress_port1);
    for (i = 0; i < pkt_cnt; ) {
        /* generate a random udp dest port */
        udp_sport = sal_rand() % ((0x1 << 16) - 1);
        udp_dport = sal_rand() % ((0x1 << 16) - 1);
        if ((udp_sport != 0) && (udp_dport != 0))
        {
            snprintf(str, 512, "tx 1 pbm=%d data=000000BBBBBB000000AAAA0081000002080045000055000100004011F728C0A80114C0A8010A%04x%04x0041000068656C6C6F2C207468697320697320612074657374207061636B6574203120666F7220666C6F7720636F756E7420657374696D6174696F6E21", ingress_port, udp_sport, udp_dport);
            bshell(unit, str);
            i++;
        }
    }
    bshell(unit, "sleep 1");

    rv = flexctr_counter_value_get_show(unit, flexctr_action_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in flexctr_counter_value_get_show(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    for (i = 0; i < index_cnt; i++) {
        cvalue = counter_value[i];
        COMPILER_64_ADD_64(total_cnt, cvalue.value[1]);
    }
    printf("total count in counters = (%u,%u)\n",
           COMPILER_64_HI(total_cnt), COMPILER_64_LO(total_cnt));

    if (COMPILER_64_HI(total_cnt) == 0 && COMPILER_64_LO(total_cnt) == pkt_cnt)
        return BCM_E_NONE;
    else
        return BCM_E_FAIL;
}


bcm_error_t
execute()
{
    bcm_error_t rv;
    int unit =0;

    bshell(unit, "config show; a ; version");

    rv = testSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("testSetup() failed, %s.\n", bcm_errmsg(rv));
        return -1;
    }
    printf("Completed test setup successfully.\n");

    rv = testVerify(unit);
    if (BCM_FAILURE(rv)) {
        printf("testVerify(): [FAIL]\n");
        return -1;
    }
    else {
        printf("testVerify(): [SUCCESS]\n");
    }
    rv = testCleanup(unit);
    if (BCM_FAILURE(rv)) {
        printf("testCleanup() failed.\n");
        return rv;
    }
    return BCM_E_NONE;
}

print execute();
const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    printf("execute(): Start\n");
    print execute();
    printf("execute(): End\n");
}

/*  Feature  : LDH accrue mode
 *
 *  Usage    : BCM.0> cint th4_hsdk_ldh_accrue_mode.c
 *
 *  config   : TH4A/TH4B: th4_hsdk_ldh_config.yml
 *             TH4G: th4g_hsdk_ldh_config.yml
 *
 *  Log file : th4_hsdk_ldh_accrue_mode_log.txt
 *
 *  Test Topology :
 *
 *                      +-------------------+
 *                      |                   |
 *                      |                   |
 *     monitor_src_port |                   |   monitor_dst_port
 *     -----------------+   TH4A/TH4B/TH4G  +-------------------
 *                      |                   |
 *                      |                   |
 *                      |                   |
 *                      |                   |
 *                      |                   |
 *                      +-------------------+
 *
 *
 *  Summary:
 *  ========
 *    This CINT configures LDH accrue mode using BCM APIs.
 *    After configuration, packets with the specified .1p vlan tag from the monitor_src_port
 *    (ingress port) are forwarded to the monitor_dst_port (egress port).
 *    Flexible counters are set up to track the latency distribution histogram of the output queue
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      1.1) Configure L2 vlan and add L2 entry for the test
 *      1.2) Configure ToD and ingress/egress port timestamping
 *      1.3) Configure LDH accrue mode
 *         1.3.1) Create a quantizer
 *         1.3.2) Create a flexCtr action for LDH accrue mode
 *         1.3.3) Create a latency monitor for the ingress/egress port pair
 *         1.3.4) Attach the flexCtr action to the latency monitor
 *         1.3.5) Enable the monitor
 *      1.4) Save the deafult TC to cosq mapping
 *      1.5) Set up TC to cosq mapping for the ingress port
 *      1.6) Set up dot1pcfi mapping to TC for the ingress port
 *      1.7) Enable PHY loopback for the ingress port and the egress port
 *      1.8) Enable packet discard for the egress port
 *
 *    2) Step2 - Configuration (Null)
 *    ======================================================
 *      Do nothing in configuration step.
 *
 *    3) Step3 - Verification (Done in testVerify())
 *    ======================================================
 *      a) a.1) Send 100 packets from the ingress port
 *         a.2) Retrieve the LDH counters
 *         a.3) Verify the sum of the counters equals to 100
 *
 *      b) Expected Result:
 *         ================
 *         The packet counts are distrubuted across the LDH counters and the sum equals to 100
*/

cint_reset();
uint8 tc = 4;          /* internal priority */
uint8 dot1pcfi = 0xc;  /* .1p = 6, cfi = 0 */
uint8 dot1p = 0x6;
uint8 cfi = 0;

int monitor_src_port;
int monitor_dst_port;
int unit = 0;

/*
 * This function checked if given port/index is already present
 * in the list so that same ports number is not generated again
*/
int checkPortAssigned(int *port_index_list,int no_entries, int index)
{
    int i;

    for (i = 0; i < no_entries; i++) {
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

    sal_srand(sal_time_usecs());
    /* generating random ports */
    for (i = 0; i < num_ports; i++) {
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

uint16 range_max[8] = {1, 0xF, 0x7F, 0xFF, 0x7FF, 0xFFF, 0x7FFF, 0xFFFF};
uint16 range_min[8] = {0, 2, 0x10, 0x80, 0x100, 0x800, 0x1000, 0x8000};
uint32 quant_id;
int bucket_size = 8;   /* should be multiple of 2 and <= 8 */
int queue_num_shift = 3;
int queue_num = 8;     /* should be multiple of 2 */
int bucket_mask_size = 3;    /* mask_size0 */
int queue_num_mask_size = 3; /* mask_size1 */

/* Create a quantizer */
int
ldh_flexctr_quantizer_create(int unit, int bucket_size, uint16 *max, uint16 *min, uint32 *quant_id)
{
    bcm_flexctr_quantization_t quant;
    int rv, i;

    bcm_flexctr_quantization_t_init(&quant);

    if (bucket_size > 8) {
        return BCM_E_PARAM;
    }
    quant.object = bcmFlexctrObjectStaticEgrResidenceTimestampLower;
    for (i = 0; i < bucket_size; i++) {
        quant.range_check_min[i] = min[i];
        quant.range_check_max[i] = max[i];
    }
    quant.range_num = bucket_size;

    rv = bcm_flexctr_quantization_create(unit, 0, &quant, quant_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_quantization_create %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/* Destroy a quantizer */
int
ldh_flexctr_quantizer_destroy(int unit, uint32 quant_id)
{
    int rv;

    rv = bcm_flexctr_quantization_destroy(unit, quant_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_quantization_destroy %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}


/* Fill out the index parameters of a flexCtr action for LDH */
void
ldh_flexctr_index_op(int unit, bcm_flexctr_index_operation_t *index_op)
{
    index_op->object[0] = bcmFlexctrObjectQuant;
    index_op->quant_id[0] = quant_id;
    index_op->mask_size[0] = bucket_mask_size;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectStaticEgrQueueNum;
    index_op->mask_size[1] = queue_num_mask_size;
    index_op->shift[1] = queue_num_shift;
}

/* Fill out the value parameters of a flexCtr action for LDH */
void
ldh_flexctr_value_op(int unit,
             bcm_flexctr_value_operation_t *value_a_op,
             bcm_flexctr_value_operation_t *value_b_op)
{
    if (value_a_op) {
        value_a_op->type = bcmFlexctrValueOperationTypeInc;
        value_a_op->select = bcmFlexctrValueSelectCounterValue;
        value_a_op->object[0] = bcmFlexctrObjectConstOne;
        value_a_op->mask_size[0] = 1;
        value_a_op->shift[0] = 0;
        value_a_op->object[1] = bcmFlexctrObjectConstZero;
        value_a_op->mask_size[1] = 1;
        value_a_op->shift[1] = 0;
    }

    if (value_b_op) {
        value_b_op->select = bcmFlexctrValueSelectPacketLength;
        value_b_op->type = bcmFlexctrValueOperationTypeInc;
    }
}

/* Create a flexCtr action for LDH in accrue mode */
int
ldh_flexctr_action_create_accrue_mode(int unit, uint32_t *stat_counter_id)
{
    bcm_flexctr_value_operation_t *value_a_op, *value_b_op;
    bcm_flexctr_index_operation_t *index_op;
    bcm_flexctr_action_t action = {0};
    int rv;

    /* Counter index is queue_num (check 3-bit here) << 3|bucket_id (3-bit).*/
    index_op = &action.index_operation;
    ldh_flexctr_index_op(unit, index_op);

    /* For normal mode, value_a: packet counter. value_b: byte counter. */
    value_a_op = &action.operation_a;
    value_b_op = &action.operation_b;
    ldh_flexctr_value_op(unit, value_a_op, value_b_op);

    /* Assign source to bcmFlexctrSourceLatencyMonitor to create counter action */
    action.source = bcmFlexctrSourceLatencyMonitor;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = bucket_size * queue_num;
    rv = bcm_flexctr_action_create(unit, 0, &action, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}


/* Destroy a flexCtr action for LDH in accrue mode */
int
ldh_flexctr_action_destroy_accrue_mode(int unit, uint32_t stat_counter_id)
{
    int rv;

    rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}


/* Configure a latency monitor */
int
ldh_monitor_config(int unit, int monitor_id)
{
    bcm_pbmp_t temp_pbmp;
    bcm_latency_monitor_config_t config;

    bcm_latency_monitor_config_t_init(&config);

    /* Set the Source Port bitmap */
    BCM_PBMP_CLEAR(temp_pbmp);
    BCM_PBMP_PORT_ADD(temp_pbmp, monitor_src_port);
    config.src_pbmp = temp_pbmp;

    /* Set the Destination Port bitmap */
    BCM_PBMP_CLEAR(temp_pbmp);
    BCM_PBMP_PORT_ADD(temp_pbmp, monitor_dst_port);
    config.dest_pbmp = temp_pbmp;

    /* Set the mode to Flexctr mode */
    config.mode = bcmLatencyMonitorFlexCtrMode ;

    /* Set the latency monitor count mode */
    config.count_mode = bcmLatencyMonitorCountAll;

    return bcm_latency_monitor_config_set(unit, monitor_id, &config);
}


/* Unconfigure a latency monitor */
int
ldh_monitor_unconfig(int unit, int monitor_id)
{
    bcm_pbmp_t temp_pbmp;
    bcm_latency_monitor_config_t config;

    bcm_latency_monitor_config_get(unit, monitor_id, &config);

    /* Set the Source Port bitmap */
    BCM_PBMP_CLEAR(temp_pbmp);
    config.src_pbmp = temp_pbmp;

    /* Set the Destination Port bitmap */
    BCM_PBMP_CLEAR(temp_pbmp);
    config.dest_pbmp = temp_pbmp;

    return bcm_latency_monitor_config_set(unit, monitor_id, &config);
}


int mon_id = 0;
uint32_t stat_counter_id;

/* Set up LDH in accure mode */
int accrue_mode_setup(int unit)
{
    int rv;

    printf("mon_id = 0x%x\n", mon_id);

    rv = ldh_flexctr_quantizer_create(unit, bucket_size, range_max, range_min, &quant_id);
    if (BCM_FAILURE(rv)) {
        printf("ldh_flexctr_quantizer_create() %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("quant_id = 0x%x\n", quant_id);

    rv = ldh_flexctr_action_create_accrue_mode(unit, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ldh_flexctr_action_create_accrue_mode() %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("stat_counter_id = 0x%x\n", stat_counter_id);

    rv = ldh_monitor_config(unit, mon_id);
    if (BCM_FAILURE(rv)) {
        printf("ldh_monitor_config() %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_latency_monitor_stat_attach(unit, mon_id, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_latency_monitor_stat_attach() %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Enable monitor */
    rv = bcm_latency_monitor_enable(unit, mon_id, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_latency_monitor_enable() %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


/* Clean up LDH in accure mode */
int accrue_mode_cleanup(int unit)
{
    int rv;

    /* Disable monitor */
    rv = bcm_latency_monitor_enable(unit, mon_id, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_latency_monitor_enable() %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_latency_monitor_stat_detach(unit, mon_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_latency_monitor_stat_detach() %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ldh_monitor_unconfig(unit, mon_id);
    if (BCM_FAILURE(rv)) {
        printf("ldh_monitor_unconfig() %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ldh_flexctr_action_destroy_accrue_mode(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ldh_flexctr_action_destory_accrue_mode() %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ldh_flexctr_quantizer_destroy(unit, quant_id);
    if (BCM_FAILURE(rv)) {
        printf("ldh_flexctr_quantizer_destroy() %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


/* Glabal variable definition */
int CPU_PORT = 0;
int MAX_PORTS = 272;
int MAX_QUEUES = 12;
int MAX_CPU_QUEUES = 48;
int MAX_SCH_NODES = 12;
int MAX_FP_PORTS = 256;
int fp_port_list[MAX_FP_PORTS];
int prev_port = -1;
int next_ucast_index = 0;
int next_mcast_index = 0;
int next_sched_index = 0;
int valid_port_number = 0;

bcm_gport_t ucast_q_gport[MAX_PORTS][MAX_QUEUES];
bcm_gport_t mcast_q_gport[MAX_PORTS][MAX_QUEUES];
bcm_gport_t sched_gport[MAX_PORTS][MAX_SCH_NODES];
bcm_gport_t cpu_mcast_q_gport[MAX_CPU_QUEUES];
bcm_gport_t cpu_sched_gport[MAX_SCH_NODES];

/* TC (internal priority) to Q mapping */
int MAX_TC = 16;
int newUcTcToQMap[] = {7, 6, 5, 4, 3, 2, 1, 0, 4, 3, 2, 1, 1, 2, 3, 4};
int newMcTcToQMap[] = {3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* Callback function for cosq gport traversal */
static bcm_error_t
cosq_gport_traverse_callback(int unit, bcm_gport_t port,
                             int numq, uint32 flags,
                             bcm_gport_t gport, void *user_data)
{
     int local_port = 0;

     switch (flags) {
         case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
             local_port = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
             if (prev_port != local_port) {
                 next_ucast_index = 0;
                 next_mcast_index = 0;
                 next_sched_index = 0;
                 prev_port = local_port;
             }
             if (CPU_PORT == local_port) {
                 return BCM_E_PARAM;
             }
             ucast_q_gport[local_port][next_ucast_index] = gport;
             next_ucast_index++;
             break;

         case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
             local_port = BCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
             if (prev_port != local_port) {
                 next_ucast_index = 0;
                 next_mcast_index = 0;
                 next_sched_index = 0;
                 prev_port = local_port;
             }
             if (CPU_PORT == local_port) {
                 cpu_mcast_q_gport[next_mcast_index] = gport;
             } else {
                 mcast_q_gport[local_port][next_mcast_index] = gport;
             }
             next_mcast_index++;
             break;

         case BCM_COSQ_GPORT_SCHEDULER:
             if (CPU_PORT == prev_port) {
                 cpu_sched_gport[next_sched_index] = gport;
             } else {
                 sched_gport[local_port][next_sched_index] = gport;
             }
             next_sched_index++;
             break;
         default:
             return BCM_E_PARAM;
     }

    return 1;
}

/*
 * Build queue list for all cosq gports
 */
static bcm_error_t
cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb, void *user_data)
{
    int ret_val = 0;

    ret_val = bcm_cosq_gport_traverse(unit, cb, NULL);
    if (ret_val  < BCM_E_NONE) {
        printf("ERR(%d): bcm_cosq_gport_traverse function unit = %d\n", ret_val, unit);
        return -1;
    }

    next_ucast_index = 0;
    next_mcast_index = 0;
    next_sched_index = 0;
    prev_port = -1;

    return 1;
}


/*
 * Set the TC to queue map using the gport for one port using bcm_cosq_gport_mapping_set()
 */
int port_tc_to_q_mapping_setup(int unit, int port)
{
    int rv = BCM_E_NONE;
    int tci;
    int queue_id = 0;
    int queue_gport = 0;

    /* Tc to Unicast Q mapping set */
    for(tci = 0; tci < MAX_TC; tci++) {
        queue_id = newUcTcToQMap[tci];
        queue_gport = ucast_q_gport[port][queue_id];
        rv = bcm_cosq_gport_mapping_set(unit, port, tci,
                                        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                        queue_gport, 0);
        if (rv != BCM_E_NONE) {
            printf("Unicast: Unable to map TC to Egress queue (P, TC, Q) = (%d, %d, %x) - %s\n",
                   port, tci, queue_id, bcm_errmsg(rv));
            return rv;
        }
        printf("Unicast: Map TC to Egress queue (P, TC, Q) = (%d, %d, %x)\n",
                port, tci, queue_id);
    }

    /* Tc to Multicast Q mapping set */
    for(tci = 0; tci < MAX_TC; tci++) {
        queue_id = newMcTcToQMap[tci];
        queue_gport = mcast_q_gport[port][queue_id];
        rv = bcm_cosq_gport_mapping_set(unit, port, tci,
                                        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                        queue_gport, 0);
        if (rv != BCM_E_NONE) {
            printf("Multicast: Unable to map TC to Egress queue (P, TC, Q) = (%d, %d, %x) - %s\n",
                   port, tci, queue_id, bcm_errmsg(rv));
            return rv;
        }
        printf("Multicast: map TC to Egress queue (P, TC, Q) = (%d, %d, %x)\n",
                port, tci, queue_id);
    }

    return rv;
}

/*
 * Set the TC to queue map for src port
 */
int tc_to_q_mapping_setup(int unit)
{
    int rv = BCM_E_NONE;
    int i = 0;

    rv = port_tc_to_q_mapping_setup(unit, monitor_src_port);
    if (rv != BCM_E_NONE) {
        printf("port_tc_to_q_mapping_setup() failed on port %d\r\n", monitor_src_port);
        return rv;
    }
    return rv;
}


/*
 * Get the TC to queue gport map for one port
 */
int port_tc_to_q_mapping_get(int unit, int port)
{
    int rv = BCM_E_NONE;
    int tc = 0;
    int cosq = -1;
    int queue_gport = 0;

    /* Tc to Unicast Q mapping get */
    for(tc = 0; tc < MAX_TC; tc ++) {
        rv = bcm_cosq_gport_mapping_get(unit, port, tc,
                                        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                        &queue_gport, &cosq);
        if (rv != BCM_E_NONE) {
            printf("Unicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                   tc, port, bcm_errmsg(rv));
            return rv;
        }
        printf("Unicast: TC %d of port %d was mapped to Egress queue %d\n",
               tc, port, cosq, bcm_errmsg(rv));
    }

    /* Tc to Multicast Q mapping get */
    for(tc = 0; tc < MAX_TC; tc ++) {
        rv = bcm_cosq_gport_mapping_get(unit, port, tc,
                                        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                        &queue_gport, &cosq);
        if (rv != BCM_E_NONE) {
            printf("Multicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                   tc, port, bcm_errmsg(rv));
            return rv;
        }
        printf("Multicast: TC %d of port %d was mapped to Egress queue %d\n",
               tc, port, cosq, bcm_errmsg(rv));
    }

    return rv;
}

/*
 * Get the TC to queue gport map
 */
int tc_to_q_mapping_get(int unit)
{
  int rv = BCM_E_NONE;
  int i = 0;

  printf("==> tc_to_q_mapping_get:\n");

  rv = port_tc_to_q_mapping_get(unit, monitor_src_port);
  if (rv != BCM_E_NONE) {
      printf("Unable to port_tc_to_q_mapping_get on port %d\r\n", monitor_src_port);
      return rv;
  }
  return rv;
}

/*
 * Save the TC to queue gport map for one port
 */
int save_port_tc_to_q_mapping(int unit, int port)
{
    int rv = BCM_E_NONE;
    int tc = 0;
    int cosq = -1;
    int queue_gport = 0;

    /* Tc to Unicast Q mapping get */
    for(tc = 0; tc < MAX_TC; tc ++) {
        rv = bcm_cosq_gport_mapping_get(unit, port, tc,
                                        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                        &queue_gport, &cosq);
        if (rv != BCM_E_NONE) {
            printf("Unicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                   tc, port, bcm_errmsg(rv));
            return rv;
        }
        save_ucast_q_gport[port][tc] = queue_gport;
        /* printf("Unicast: TC %d of port %d was mapped to Egress queue %d\n",
               tc, port, cosq, bcm_errmsg(rv)); */
    }

    /* Tc to Multicast Q mapping get */
    for(tc = 0; tc < MAX_TC; tc ++) {
        rv = bcm_cosq_gport_mapping_get(unit, port, tc,
                                        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                        &queue_gport, &cosq);
        if (rv != BCM_E_NONE) {
            printf("Multicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                   tc, port, bcm_errmsg(rv));
            return rv;
        }
        save_mcast_q_gport[port][tc] = queue_gport;
        /* printf("Multicast: TC %d of port %d was mapped to Egress queue %d\n",
               tc, port, cosq, bcm_errmsg(rv)); */
    }

    return rv;
}

/*
 * Save the TC to queue gport map
 */
int save_tc_to_q_mapping(int unit)
{
  int rv = BCM_E_NONE;
  int i = 0;

  printf("--> save_tc_to_q_mapping:\n");

  for (i = 0; i < valid_port_number; i++) {
      rv = save_port_tc_to_q_mapping(unit, fp_port_list[i]);
      if (rv != BCM_E_NONE) {
          printf("Unable to port_tc_to_q_mapping_get on port %d\r\n", fp_port_list[i]);
          return rv;
      }
  }

  return rv;
}

/*
 * Restore the TC to queue gport map for one port
 */
int restore_port_tc_to_q_mapping(int unit, int port)
{
  int rv = BCM_E_NONE;
  int tc = 0;
  int queue_id = 0;
  int queue_gport = 0;

  /* Tc to Unicast Q mapping set */
  for(tc = 0; tc < MAX_TC; tc ++) {
      queue_gport = save_ucast_q_gport[port][tc];
      queue_id = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(queue_gport);
      rv = bcm_cosq_gport_mapping_set(unit, port, tc,
                                      BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                      queue_gport, 0);
      if (rv != BCM_E_NONE) {
          printf("Unicast: Unable to Restore TC to Egress queue (P, TC, Q) = (%d, %d, %x) - %s\n",
                 port, tc, queue_id, bcm_errmsg(rv));
          return rv;
      }
      /* printf("Unicast: Restore TC to Egress queue sucessfully (P, TC, Q) = (%d, %d, %x) - %s\n",
              port, tc, queue_id, bcm_errmsg(rv)); */
  }

  /* Tc to Multicast Q mapping set */
  for(tc = 0; tc < MAX_TC; tc ++) {
      queue_gport = save_mcast_q_gport[port][tc];
      queue_id = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(queue_gport);
      rv = bcm_cosq_gport_mapping_set(unit, port, tc,
                                      BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                      queue_gport, 0);
      if (rv != BCM_E_NONE) {
          printf("Multicast: Unable to Restore TC to Egress queue (P, TC, Q) = (%d, %d, %x) - %s\n",
                 port, tc, queue_id, bcm_errmsg(rv));
          return rv;
      }
      /* printf("Multicast: Restore TC to Egress queue sucessfully (P, TC, Q) = (%d, %d, %x) - %s\n",
              port, tc, queue_id, bcm_errmsg(rv)); */
  }

  return rv;
}

/*
 * Restore the TC to queue map for all ports
 */
int restore_tc_to_q_mapping(int unit)
{
    int rv = BCM_E_NONE;
    int i = 0;

    printf("--> restore_tc_to_q_mapping:\n");

    for (i = 0; i < valid_port_number; i++) {
        rv = restore_port_tc_to_q_mapping(unit, fp_port_list[i]);
        if (rv != BCM_E_NONE) {
            printf("Unable to restore_port_tc_to_q_mapping on port %d\r\n", fp_port_list[i]);
            return rv;
        }
    }

    return rv;
}

bcm_vlan_t vid = 10;
uint8 dst_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};

/* Create a vlan and add a L2 entry for uncast forwarding */
bcm_error_t l2_setup(int unit)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_l2_addr_t addr;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, monitor_src_port);
    BCM_PBMP_PORT_ADD(pbmp, monitor_dst_port);

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, upbmp));

    /* Set port default vlan id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, monitor_src_port, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, monitor_dst_port, vid));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, monitor_src_port,
                                             bcmPortControlTrustIncomingVlan, 1));
    /* Add a L2 address on front panel port. */
    bcm_l2_addr_t_init(&addr, dst_mac, vid);
    addr.port = monitor_dst_port;
    addr.flags |= BCM_L2_STATIC;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &addr));

    return BCM_E_NONE;
}

/* Clean up L2 entries for a vlan and remove the vlan */
bcm_error_t l2_cleanup(int unit)
{
    /* clean up learned l2 entries */
    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_vlan(unit, vid, 0));

    /* Delete a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_destroy(unit, vid));

    /* Set port default vlan id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, monitor_src_port, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, monitor_dst_port, 1));

    return BCM_E_NONE;
}


int ing_qos_map;

/* Create a qos map */
bcm_error_t
qos_map_setup(int unit, unsigned int flags, uint8 internal_pri, bcm_color_t clr, uint8 priority, uint8 cfi, int *qMapId)
{
    bcm_qos_map_t qMap;

    /* Reserve hardware resources for QoS mapping */
    BCM_IF_ERROR_RETURN(bcm_qos_map_create(unit, flags, qMapId));

    qMap.int_pri = internal_pri;
    qMap.color   = clr;
    qMap.pkt_pri = priority;
    qMap.pkt_cfi = cfi;
    BCM_IF_ERROR_RETURN(bcm_qos_map_add(unit, flags, qMap, *qMapId));

    return BCM_E_NONE;
}


/* Set up L2 qos mapping */
bcm_error_t
qos_port_setup(int unit, bcm_port_t port, unsigned int flags, uint8 internal_pri, bcm_color_t clr, uint8 priority, uint8 cfi, int *qMapId)
{
    bcm_gport_t gport;
    int rv;

    rv = qos_map_setup(unit, flags, internal_pri, clr, priority, cfi, qMapId);
    if (BCM_FAILURE(rv)) {
        printf("\nError in qos_map_setup: %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_GPORT_MODPORT_SET(gport, unit, port);
    if(flags & BCM_QOS_MAP_INGRESS)
        BCM_IF_ERROR_RETURN(bcm_qos_port_map_set(unit, gport, *qMapId, -1));
    else
        BCM_IF_ERROR_RETURN(bcm_qos_port_map_set(unit, gport, -1, *qMapId));
    return BCM_E_NONE;
}


/* Clean up L2 qos mapping */
bcm_error_t
qos_cleanup(int unit)
{
    int rv;

    rv = bcm_qos_map_destroy(unit, ing_qos_map);
    if (BCM_FAILURE(rv)) {
        printf("\nError in qos_cleanup: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


bcm_time_if_t time_if_id;               /* Time Interface Identifier */

/* Config timer */
bcm_error_t
time_config(int unit)
{
    int rv;

    bcm_time_init(unit);

    bcm_time_interface_t time_intf;
    /* Initialize CMIC Timer */
    time_intf.flags = BCM_TIME_ENABLE;
    rv = bcm_time_interface_add(unit, &time_intf);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_time_interface_add(): %s\n", bcm_errmsg(rv));
        return rv;
    }
    time_if_id = time_intf.id;

    bcm_port_phy_timesync_config_t cfg;
    /* Enable timestamping */
    cfg.flags = BCM_PORT_PHY_TIMESYNC_ENABLE |
    BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE;
    rv = bcm_port_phy_timesync_config_set(unit, monitor_src_port, cfg);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_phy_timesync_config_set(): %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_phy_timesync_config_set(unit, monitor_dst_port, cfg);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_phy_timesync_config_set(): %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

bcm_error_t
time_unconfig(int unit)
{
    int rv;

    rv = bcm_time_interface_delete(unit, time_if_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_time_interface_delete(): %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}


/* Test setup function */
bcm_error_t
test_setup(int unit)
{
    int port_list[2];
    int num_ports = 2;
    int rv;

    rv = portNumbersGet(unit, port_list, num_ports);
    if (BCM_FAILURE(rv)) {
        printf("portNumbersGet() failed\n");
        return rv;
    }
    monitor_src_port = port_list[0];
    monitor_dst_port = port_list[1];
    printf("Monitor src port : %d\n", monitor_src_port );
    printf("Monitor dst port: %d\n", monitor_dst_port);

    /* L2 related setup */
    rv = l2_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("l2_setup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Config timer */
    time_config(unit);

    /* Must use PHY loopback (instead of MAC loopback) for latency measurement */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, monitor_src_port, BCM_PORT_LOOPBACK_PHY));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, monitor_dst_port, BCM_PORT_LOOPBACK_PHY));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, monitor_dst_port, BCM_PORT_DISCARD_ALL));
    /* Sleep 1 second before testing */
    bshell(0, "sleep 1");

    /* Set up LDH in accure mode */
    rv = accrue_mode_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("accrue_mode_setup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Build queue list for all cosq gports */
    rv = cosq_gport_traverse(unit, cosq_gport_traverse_callback, NULL);
    if (BCM_FAILURE(rv)) {
        printf("cosq_gport_traverse() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Save the default TC to queue mapping */
    if (BCM_FAILURE((rv = save_tc_to_q_mapping(unit)))) {
       printf("save_tc_to_q_mapping() failed.\n");
       return rv;
    }

    /* Set the mapping from TC to queue via bcm_cosq_gport_mapping_set */
    rv = tc_to_q_mapping_setup(unit);
    if (BCM_FAILURE(rv)) {
       printf("tc_to_q_mapping_setup() failed.\n");
       return rv;
    }

    /* Set up L2 qos mapping */
    rv = qos_port_setup(unit, monitor_src_port, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_L2, tc, bcmColorYellow, dot1p, cfi, &ing_qos_map);
    if (BCM_FAILURE(rv)) {
       printf("qos_port_setup() failed.\n");
       return rv;
    }
    printf("Setup L2 qos map %d for port %d: <dot1p = %d, cfi = %d> --> TC = %d\n", ing_qos_map, monitor_src_port, dot1p, cfi, tc);

    return BCM_E_NONE;
}

/* Test cleanup function */
bcm_error_t
test_cleanup(int unit)
{
    int rv;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, monitor_src_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, monitor_dst_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, monitor_dst_port, BCM_PORT_DISCARD_NONE));

    rv = qos_cleanup(unit);
    if (BCM_FAILURE(rv)) {
       printf("qos_port_cleanup() failed.\n");
       return rv;
    }

    /* Restore TC to Queue mapping to default */
    rv = restore_tc_to_q_mapping(unit);
    if (BCM_FAILURE(rv)) {
        printf("restore_tc_to_q_mapping() failed.\n");
        return rv;
    }

    rv = accrue_mode_cleanup(unit);
    if (BCM_FAILURE(rv)) {
        printf("accrue_mode_cleanup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = time_unconfig(unit);
    if (BCM_FAILURE(rv)) {
        printf("time_unconfig() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = l2_cleanup(unit);
    if (BCM_FAILURE(rv)) {
        printf("l2_cleanup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


/* Test verification function */
bcm_error_t testVerify(int unit)
{
    uint32 num_entries = bucket_size;
    bcm_flexctr_counter_value_t counter_value[num_entries], cv;
    uint32 counter_index[num_entries];
    int queue_id;
    int i, rv, total_pkt, sum;
    char str[256];

    total_pkt = 100;
    /* send packets with the specified dot1p/cfi */
    snprintf(str, 256, "tx %d pbm=%d data=0x0000000000110000000000228100%1x00aFFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2DDFC5C3CB; sleep quiet 1", total_pkt, monitor_src_port, dot1pcfi);
    bshell(unit, str);

    /* Get the queue's LDH counters */
    queue_id = newUcTcToQMap[tc];
    printf("queue_id = %d\n", queue_id);

    sal_memset(counter_value, 0, sizeof(counter_value));
    for (i = 0; i < num_entries; i++)
        counter_index[i] = queue_id*bucket_size + i;

    rv = bcm_flexctr_stat_sync_get(unit,
                                   stat_counter_id,
                                   num_entries,
                                   counter_index,
                                   counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    for (i = 0, sum = 0; i < num_entries; i++) {
        cv = counter_value[i];
        printf("LDH flexCtr for queue_id=%d, counter[%d]: %d packets / %d bytes\n",
               queue_id, i,
               COMPILER_64_LO(cv.value[0]),
               COMPILER_64_LO(cv.value[1]));
        sum += COMPILER_64_LO(cv.value[0]);
    }

    /* clean up counters */
    sal_memset(counter_value, 0, sizeof(counter_value));
    for (i = 0; i < num_entries; i++)
        counter_index[i] = queue_id*bucket_size + i;

    rv = bcm_flexctr_stat_set(unit,
                              stat_counter_id,
                              num_entries,
                              counter_index,
                              counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_set %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* verify the sum of counters of the cosq == total_pkt */
    if (sum == total_pkt)
        return BCM_E_NONE;
    else
        return BCM_E_FAIL;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in setup_vlan_switching())
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    int unit = 0;
    int rv;

    bshell(unit, "config show; a ; version");

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) verify(): ** start **";
    if (BCM_FAILURE((rv = testVerify(unit)))) {
        printf("testVerify() [FAIL]\n");
        return -1;
    }
    else
        printf("testVerify() [PASS]\n");
    print "~~~ #2) verify(): ** end **";

    print "~~~ #3) test_cleanup(): ** start **";
    if (BCM_FAILURE((rv = test_cleanup(unit)))) {
        printf("test_cleanup() failed.\n");
        return -1;
    }
    print "~~~ #3) test_cleanup(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print "execute(): Start";
    print execute();
    print "execute(): End";
}

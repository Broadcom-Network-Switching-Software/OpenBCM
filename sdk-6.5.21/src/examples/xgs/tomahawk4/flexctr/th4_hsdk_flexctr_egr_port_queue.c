/*  Feature  : Flex Counters
 *
 *  Usage    : BCM.0> cint th4_hsdk_flexctr_egr_port_queue.c
 *
 *  config   : bcm56990_a0-generic-64x400.config.yml
 *
 *  Log file : th4_hsdk_flexctr_egr_port_queue_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *
 *  Summary:
 *  ========
 *    This CINT script demonstrates how to configure and use flex counters
 *    to count packets and bytes on egress port queues.
 *    
 *    Prerequistes:
 *    =============
 *      Build HSDK, the Flex Counter feature is built by default.
 *     
 *    =====================================================
 *    Detailed operations performed by the CINT script:
 *
 *    1) Step1 - Test Setup: The function test_setup() is used to gather
 *       test ports and set up per queue counters for each test port.
 *      a) Verify that attached device is supported by this test.
 *      b) Get list of test ports using SDK API.
 *      c) Initialize queue counter structure
 *      d) Configure queue counters for all test ports.
 *    2) Step2 - Configuration: There is no separate configuration
 *       step. Configuration operations take place during the verification step.
 *    3) Step3 - Verification (Done in verify())
 *      a) Transmit unicast packets on each test port with port in
 *         loopback. The number of packets and size of each packet is a function
 *         of the port.
 *      b) For each port, get the packet and byte counts for all queues. Verify
 *         the actual packet and byte counts against expected values.
 *
 *      Expected Result:
 *      Per queue packet and byte counts must match what was transmitted. If they
 *      don't, an error code is returned.
 */

cint_reset();

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
 *
 * SUPPORT FUNCTIONS FOR PER PORT, PER QUEUE EGRESS COUNTERS
 *
 * Use the functions defined here for applications that require per queue
 * counters.
 *
 * The 56990 (Tomahawk4) does not have dedicated per port, per queue counters
 * (typically called PERQ_PKT and PERQ_BYTE).  In place of this functionality,
 * the 56990 supports flex counters that provide equivalent functionality
 * (see bcmFlexctrObjectStaticEgrQueueNum and bcmFlexctrObjectStaticEgrEgressPort ).
 */

/* Define CONSTANTS */

/* Even though there are only 12 queues, flex counters allocate 16 */
const uint32        RESERVE_QUEUES_PER_PORT = 16;

/*
 * The 56990/Tomahawk supports a total of 12 queues, the default configuration
 * is 8 unicast queues (0..7) and 4 multicast queues (8..11).
 */
const uint32        EGR_QUEUES_PER_PORT = 12;

/* Need to reserve flex counter resources across logical port space */
const int           MAX_LOGICAL_PORT = 258;

/* Define DATA STRUCTURES */

/*
 * Define a data structure to hold the counter ID and attached ports.
 */
typedef struct queue_counter_t {
    /* Keep track of which ports have been attached to the flex counter */
    bcm_pbmp_t          attached_ports;
    /* One flex counter for all ports */
    uint32              stat_counter_id;
};

/* Pass packet and byte counts for each port in this structure. */
typedef struct per_queue_counts_t {
    bcm_flexctr_counter_value_t counter_values[EGR_QUEUES_PER_PORT];
};

/* DEFINE PER PORT/PER QUEUE SPECIFIC FUNCTIONS */

/*******************************************************************************
 * Function: queue_counter_t_init
 *
 * Helper function used to initialize a queue_counter_t structure.
 *
 * Parameters:
 *   queue_counter: Pointer to queue count structure.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
void
queue_counter_t_init(queue_counter_t * queue_counter)
{
    sal_memset(queue_counter, 0, sizeof(*queue_counter));
}

/*******************************************************************************
 * Function: egr_port_and_queue_num
 *
 * Helper function used to configure a single flex counter for
 * bcmFlexctrObjectStaticEgrQueueNum and bcmFlexctrObjectStaticEgrEgressPort.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   stat_counter_id: Pointer to receive the new counter ID.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
egr_port_and_queue_num(int unit, uint32 * stat_counter_id)
{
    const int           options = 0;

    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op;
    bcm_flexctr_value_operation_t *value_a_op;
    bcm_flexctr_value_operation_t *value_b_op;

    bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    action.source = bcmFlexctrSourceEgrPort;
    action.mode = bcmFlexctrCounterModeNormal;
    /* reserve resources */
    action.index_num = (MAX_LOGICAL_PORT + 1) * RESERVE_QUEUES_PER_PORT;

    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticEgrQueueNum;
    index_op->mask_size[0] = 4;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectStaticEgrEgressPort;
    index_op->mask_size[1] = 10;
    index_op->shift[1] = 4;

    /* Increase counter per packet. */
    value_a_op = &action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 1;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;

    /* Increase counter per packet bytes. */
    value_b_op = &action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;

    /* Create an ingress action with a time-based trigger */
    BCM_IF_ERROR_RETURN(bcm_flexctr_action_create
                        (unit, options, &action, stat_counter_id));

    printf("Created Flex Counter Action: ID=0x%X\n", *stat_counter_id);
    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: create_queue_counter_for_port
 *
 * Configure the per queue counters for specified port.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   logical_port: Logical port to configure
 *   queue_counter: Counter struct that the flex counter stat ID.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
create_queue_counter_for_port(int unit, bcm_port_t logical_port,
                              queue_counter_t * queue_counter)
{
    /*
     * Create th flex counter if this is the first port being added to the
     * flex counter.
 */
    if (queue_counter->stat_counter_id == 0) {
        BCM_IF_ERROR_RETURN(egr_port_and_queue_num
                            (unit, &queue_counter->stat_counter_id));
    }
    /* Attach logical port to flex counter */
    BCM_IF_ERROR_RETURN(bcm_port_stat_attach
                        (unit, logical_port, queue_counter->stat_counter_id));
    /* Track attached ports */
    BCM_PBMP_PORT_ADD(queue_counter->attached_ports, logical_port);
    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: get_port_queue_counts
 *
 * Get packet and byte counts for all queues on specified port.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   logical_port: Logical port to configure
 *   queue_counter: Counter struct that holds the counter stat ID.
 *   per_queue_counters: Pointer to receive packet and byte counts for
 *   all queues.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
get_port_queue_counts(int unit, bcm_port_t logical_port,
                      queue_counter_t * queue_counter,
                      per_queue_counts_t * per_queue_counts)
{
    int                 queue;
    uint32              counter_indexes[EGR_QUEUES_PER_PORT];
    bcm_error_t         rv;

    /* Compute list of indices for all queues on this logical port */
    for (queue = 0; queue < EGR_QUEUES_PER_PORT; queue++) {
        counter_indexes[queue] = (logical_port * RESERVE_QUEUES_PER_PORT) + queue;
    }

    /* Fetch packet and byte counts for all queues */
    if (BCM_FAILURE(rv = bcm_flexctr_stat_sync_get(unit,
                                                   queue_counter->stat_counter_id,
                                                   EGR_QUEUES_PER_PORT, counter_indexes,
                                                   per_queue_counts->counter_values))) {
        printf("bcm_flexctr_stat_sync_get() FAILED %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
 *
 * TEST FUNCTIONS:
 * The following functions are used to validate the per queue counter function
 * defined above.
 */

/*******************************************************************************
 * Function: portNumbersGet
 *
 * Get a list of 100G ports for use in this test.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   port_list: Pointer to receive list of test ports
 *   num_ports: Number of ports to get (and size of port_list)
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    bcm_port_t          port;
    bcm_error_t         rv;
    bcm_port_config_t   configP;
    int                 port_count;

    /* Get current port configuration */
    if (BCM_FAILURE(rv = bcm_port_config_get(unit, &configP))) {
        printf("Error in bcm_port_config_get: %s\n\n", bcm_errmsg(rv));
        return rv;
    }

    /* Collect requested number of "cd" ports */
    port_count = 0;
    printf("Ports: ");
    BCM_PBMP_ITER(configP.cd, port) {
        if (port_count == num_ports) {
            break;
        }
        port_list[port_count] = port;
        printf("%3d(cd%d) ", port, port_count);
        if ((port_count % 10) == 9) {
            printf("\n   ");
        }
        port_count++;
    }
    printf("\n");
    if (port_count != num_ports) {
        printf("portNumbersGet() Not enough ports\n");
        return BCM_E_PORT;
    }

    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: do_tx
 *
 * Using diagnostic shell commands, send packets on a single test port. The
 * number of packets and size of each packet is a function of the "port_num"
 * parameters.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   port_num: A sequential port number, "0" for "cd0" etc.
 *
 * Returns:
 * Nothing
 */
void
do_tx(int unit, int port_num)
{
    /* Packet priority, packet count and byte count are a function of port number */
    const int           tx_count = port_num + 1;
    const int           tx_len = 100 + (port_num * 4);
    const int           tx_prio = port_num % 8;

    char                command_str[185];

    /* Set port to MAC loopback */
    sprintf(command_str, "port cd%d lb=mac", port_num);
    bshell(unit, command_str);

    /* Create an L2 entry so that we're sending unicast packets */
    sprintf(command_str,
            "l2 add Port=cd%d vlan=1 mac=00:00:00:00:00:%02X static=true",
            port_num, port_num + 1);
    bshell(unit, command_str);
    printf(">> %s\n", command_str);

    /* Send packet using built in "tx" command */
    sprintf(command_str,
            "tx %d Length=%d VlanPrio=%d dm=00:00:00:00:00:%02X sm=80:80:80:80:80:%02X",
            tx_count, tx_len, tx_prio, port_num + 1, port_num + 64);
    bshell(unit, command_str);
    printf(">> %s [%d bytes]\n", command_str, tx_count * tx_len);

    /* Set port back to no loopback */
    sprintf(command_str, "port cd%d lb=none", port_num);
    bshell(unit, command_str);
}

/*******************************************************************************
 * Function: get_device_type
 *
 * Get current device type as listed on PCI bus. Used to determine if current
 * device is supported byt this test.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   dev_type: Pointer to receive device type code.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
get_device_type(int unit, uint32 * dev_type)
{
    bcm_info_t          info;

    BCM_IF_ERROR_RETURN(bcm_info_get(unit, &info));

    *dev_type = info.device & 0x0000FFF0;       /* Mask off lowest hex digit to get dev group */

    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: test_setup
 *
 * Main set up routine. Calls other setup sub-functions defined above.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   port_list: List of ports to configure
 *   num_ports: Number of ports in port_list
 *   queue_counter: Pointer to queue_counter struct, this function
 *     initializes and configures this object.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
test_setup(int unit, bcm_port_t * port_list, int num_ports,
           queue_counter_t * queue_counter)
{
    int                 idx;
    uint32              dev_type;

    /* Test test only supported for 5699X (Tomahawk4) */
    BCM_IF_ERROR_RETURN(get_device_type(unit, &dev_type));
    if (dev_type != 0xB990) {
        printf("Unsupported device type/configuration\n");
        return BCM_E_FAIL;
    }

    /* Get list of ports for this configuration */
    BCM_IF_ERROR_RETURN(portNumbersGet(unit, port_list, num_ports));

    /* Initialize queue counter structure */
    queue_counter_t_init(queue_counter);

    /* Create per queue counters for all ports in the port list */
    for (idx = 0; idx < num_ports; idx++) {
        BCM_IF_ERROR_RETURN(create_queue_counter_for_port
                            (unit, port_list[idx], queue_counter));
    }
    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: verify
 *
 * Main verification routine. Sends traffic on all test ports and verifies
 * that per queue counters match expected values.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   num_ports: Number of ports in port_list
 *   port_list: List of ports to configure
 *   queue_counter: Pointer to queue_counter struct,
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
verify(int unit, int *port_list, int num_ports, queue_counter_t * queue_counter)
{
    bcm_port_t          port_idx;
    int                 test_failed = 0;

    /* Start the packet watched, traffic ends up at CPU */
    bshell(unit, "pw start quiet");

    /* Send traffic on each test port */
    for (port_idx = 0; port_idx < num_ports; port_idx++) {
        do_tx(unit, port_idx);
        sal_sleep(1);
    }

    /* Get per queue stats for each port and confirm expected counts */
    for (port_idx = 0; port_idx < num_ports; port_idx++) {
        /* Counts should match those from "do_tx()" */
        const int           expected_queue = port_idx % 8;
        const int           expected_packets = port_idx + 1;
        const int           expected_bytes = expected_packets * (100 + (port_idx * 4));

        per_queue_counts_t  per_queue_counts;   /* Counts go here */
        int                 queue;
        int                 need_header;
        int                 fail = 0;

        /* Get current per queue counts for next port */
        BCM_IF_ERROR_RETURN(get_port_queue_counts
                            (unit, port_list[port_idx], queue_counter,
                             &per_queue_counts));
        need_header = 1;
        for (queue = 0; queue < EGR_QUEUES_PER_PORT; queue++) {
            const uint32        packets =
              COMPILER_64_LO(per_queue_counts.counter_values[queue].value[0]);
            const uint32        bytes =
              COMPILER_64_LO(per_queue_counts.counter_values[queue].value[1]);

            /* Compare actual counts against expected values */
            fail = ((queue == expected_queue)
                    && ((packets != expected_packets) || (bytes != expected_bytes)))
              || ((queue != expected_queue)
                  && ((packets != 0) || (bytes != 0)));

            /* Print non-zero and mismatched counts */
            if ((packets != 0) || (bytes != 0) || fail) {
                if (need_header) {
                    printf("Port: %3d (cd%d)\n", port_list[port_idx], port_idx);
                    need_header = 0;
                }
                printf("  %s%1d: %3u packets; %4u bytes; %s\n",
                       queue < 8 ? "UC" : "MC", queue < 8 ? queue : queue - 8,
                       packets, bytes, fail ? "FAIL" : "OK");
            }
            /* Update test status */
            test_failed = test_failed || fail;
        }
        if (need_header) {
            printf("Port: %3d: None\n", port_list[port_idx]);
        }
    }
    /* Return status of test */
    printf("Per Queue Test %s\n", test_failed ? "FAILED" : "PASSED");
    return test_failed ? BCM_E_FAIL : BCM_E_NONE;
}

/*******************************************************************************
 * Function: execute
 *
 * Test entry point. Test paramters defined here, calls "setup" and "verify"
 * functions.
 *
 * Parameters:
 *   None, all test parameters are defined by this function.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
execute()
{
    const int           unit = 0;
    const int           num_ports = 64;

    bcm_port_t          port_list[num_ports];
    queue_counter_t     queue_counter;

    /* Get test port list and configure per queue counters */
    BCM_IF_ERROR_RETURN(test_setup(unit, port_list, num_ports, &queue_counter));

    /* Send packets on specified queues and verify counts */
    return verify(unit, port_list, num_ports, &queue_counter);
}

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
 * Entry point
 */
const char         *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    if (BCM_FAILURE(execute())) {
        printf("Test Failed\n");
    }
}

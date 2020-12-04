/*
 * Feature  : cosq gport
 *
 * Usage    : BCM.0> cint th4_hsdk_mmu_cosq_gport_attach.c
 *
 * Config   : th4_hsdk_mmu_config.yml
 *
 * Log file : th4_hsdk_mmu_cosq_gport_attach_log.txt
 *
 * Test Topology :
 *
 *                         +-------------------+
 *                         |                   |
 *                         |                   |
 *          ingress_port   |       TH4         | CPU port
 *          -------------->+                   +---------> COSQ 0
 *           VLAN 2        |        L2         |  VLAN 2
 *                         |                   |
 *                         |                   |
 *                         +-------------------+
 *
 * This script can verify below cosq gport APIs:
 * ========
 *    bcm_cosq_gport_get
 *    bcm_cosq_gport_traverse
 *    bcm_cosq_gport_attach
 *    bcm_cosq_gport_attach_get
 *    bcm_cosq_gport_detach
 *    bcm_cosq_gport_child_get
 *
 * Summary:
 * ========
 * This cint example demonstrate attaching/detaching CPU queue to CPU L0 sch node
 *
 * Detailed steps done in the CINT script:
 *   =======================================
 *   1) Step1 - Test Setup (Done in test_setup())
 *   =====================================
 *    a) Set up L2 switching path from ingress port to CPU port
 *    b) Set up per queue flex counter
 *
 *   2) Step2 - Configuration (Done in test_setup())
 *   ==========================================
 *    a) Retrieve all the CPU queues and L0 scheduler nodes, save to local data structure
 *    b) Remap CPU queues to L0 sch nodes based on (queue ID --> (queue id)%(NUM_SCH_NODES-1))
 *       done in cpu_queue_to_L0_node_remap()
 *
 *   3) Step3 - Verification (Done in test_verify())
 *   ==========================================
 *    Expected Result:
 *     a) Transmit a packet with priority 0 to the ingress port, packet is L2 switched to CPU,
 *        expect the packet to egress on the queue mapped, verify with flex counter
 *        and RX callback
 *     b) (Optional) Verify the result (CPU queues to L2 sch nodes mapping) using 'dump sw cosq'
 *
 *   4) Step4 - Cleanup (Done in test_cleanup())
 *   ==========================================
 *     a) Reset the CPU queues to L0 sch nodes to default settings
 *     b) Remove the per queue flex counter
 *     c) Delete L2 forwarding path
 */

cint_reset();

int unit = 0;
int MAX_PORTS = 272;

const int CPU_PORT = 0;
int NUM_CPU_QUEUES = 48;
int NUM_SCH_NODES = 12;
int prev_port = -1;

/* Structure for each of 48 CPU queues */
typedef struct cpu_queue_t {
    bcm_gport_t gport;
    bcm_gport_t input_node;    /* The sch node CPU queue attached to */
    int         input_L0_num;  /* The number of the L0.0 - L0.11 sch node CPU queue is attached to */
};

/* Structure used to store all gports */
typedef struct cpu_queues_t {
    cpu_queue_t cpu_queues[NUM_CPU_QUEUES];
    bcm_gport_t sched_nodes[NUM_SCH_NODES];
};

cpu_queues_t save_cpu_queues;

bcm_port_t ingress_port;
bcm_vlan_t vid = 2;
uint8 dst_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
uint32_t stat_counter_id;
bcm_flexctr_counter_value_t counter_value;

/* This function gives required number of ports. */
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i=0, port=0, rv=0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if(BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (port= 1; port < MAX_PORTS; port++) {
        port_list[i] = -1;
        if (BCM_PBMP_MEMBER(&ports_pbmp, port) && (i < num_ports)) {
            port_list[i] = port;
            i++;
        }
        if (i == num_ports) {
            break;
        }
    }

    if (i == 0) {
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
    BCM_PBMP_PORT_ADD(pbmp, CPU_PORT);

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid)); 
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, upbmp));

    /* Set port default vlan id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, vid));
    bcm_port_control_set(unit, ingress_port, bcmPortControlTrustIncomingVlan, 1);

    /* Add a L2 address on front panel port. */
    bcm_l2_addr_t_init(&addr, dst_mac, vid);
    addr.port = CPU_PORT;
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
 * Callback function for bcm_cosq_gport_traverse().
 * Find and save all 48 CPU queue gports and 12 L0 gports.
 */
static bcm_error_t
cosq_gport_traverse_callback(int unit,
                             bcm_gport_t port, int numq, uint32 flags,
                             bcm_gport_t gport, void *user_data)
{
    int         local_port=prev_port;
    int         num_cos_levels;
    uint32      port_flags;
    bcm_gport_t input_port;
    bcm_cos_queue_t cosq;

    cpu_queues_t *cpu_queues = (auto) user_data;

    /* Only care about CPU queues (multicast queues for port 0) */
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        /* Only care about multicast queues */
        return BCM_E_NONE;
    }

    /*
     * Only parse the local port from multicast queues, as the local port parsed from sch node
     * can't exactly represent the local port
     */
    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_cosq_gport_get(unit, gport, &local_port,
                                               &num_cos_levels, &port_flags));
    }

    if (prev_port != local_port) {
        prev_port = local_port;
        printf("cosq_gport_traverse_callback: local_port = %d\n", local_port);
    }

    /* Only care about port 0 (the CPU port) */
    if (local_port != CPU_PORT) {
        return BCM_E_NONE;
    }

    /* Get input port and COSQ using bcm_cosq_gport_attach_get() */
    BCM_IF_ERROR_RETURN(bcm_cosq_gport_attach_get(unit, gport, &input_port, &cosq));

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        cpu_queues->sched_nodes[cosq] = gport;
        printf("cpu_queues->sched_nodes[%d]=0x%x\n", cosq, gport);
    } else {
        cpu_queues->cpu_queues[cosq].gport = gport;
        cpu_queues->cpu_queues[cosq].input_node = input_port;
        printf("cpu_queues[%d](0x%x) was attached to sched_nodes=0x%x\n",
               cosq, gport, input_port);
    }

    return BCM_E_NONE;
}

/*
 * Find all CPU queues and scheduler nodes using the gport traverse API and save them. The gport
 * values are static and only need to be obtained once.
 */
bcm_error_t
cpu_sch_gport_retrieve(int unit, cpu_queues_t * cpu_queues)
{
    int cos_idx;
    int input_idx;

    BCM_IF_ERROR_RETURN(
        bcm_cosq_gport_traverse(unit, cosq_gport_traverse_callback, cpu_queues));

    for (cos_idx = 0; cos_idx < NUM_CPU_QUEUES; cos_idx++) {
        for (input_idx = 0; input_idx < NUM_SCH_NODES; input_idx++) {
            if (cpu_queues->sched_nodes[input_idx] == cpu_queues->cpu_queues[cos_idx].input_node) {
                /* The cos_idx-th CPU queue was attached to the input_idx-th scheduler node */
                cpu_queues->cpu_queues[cos_idx].input_L0_num = input_idx;
                break;
            }
        }
    }
    prev_port = -1;

    return BCM_E_NONE;
}

/*
 * Map a CPU queue to CPU L0 scheduler node. bcm_cosq_gport_detach should be
 * used in place of bcm_rx_queue_channel_set() for Tomahawk device.
 */
bcm_error_t
rx_queue_channel_set(int unit, cpu_queues_t * cpu_queues,
                     bcm_cos_queue_t queue_id, bcm_rx_chan_t chan_id)
{
    bcm_gport_t input_port;
    int cos_num;

    /* Disconnect previous CMC RX channel */
    cos_num = cpu_queues->cpu_queues[queue_id].input_L0_num;
    BCM_IF_ERROR_RETURN(
        bcm_cosq_gport_detach(unit,
                    cpu_queues->cpu_queues[queue_id].gport,
                    cpu_queues->sched_nodes[cos_num], queue_id));
    printf("rx_queue_channel_set: Detach the %dth CPU queue 0x%x from the %dth L0 node(0x%x)\n",
           queue_id, cpu_queues->cpu_queues[queue_id].gport,
           cos_num, cpu_queues->sched_nodes[cos_num]);

    /* Update connection database
    cpu_queues->cpu_queues[queue_id].input_node = cpu_queues->sched_nodes[chan_id];
    cpu_queues->cpu_queues[queue_id].input_L0_num = chan_id; */

    /* Attach new CMC RX channel */
    BCM_IF_ERROR_RETURN(
        bcm_cosq_gport_attach(unit,
                     cpu_queues->cpu_queues[queue_id].gport,
                     cpu_queues->sched_nodes[chan_id],
                     queue_id));

    printf("rx_queue_channel_set: Attaching the %dth CPU queue 0x%x to the %dth node of L0(0x%x)\n",
            queue_id, cpu_queues->cpu_queues[queue_id].gport, chan_id,
            cpu_queues->sched_nodes[chan_id]);

    BCM_IF_ERROR_RETURN(
        bcm_cosq_gport_child_get(unit,
                     cpu_queues->sched_nodes[chan_id], queue_id, &input_port));
    printf("rx_queue_channel_set: Attach check: The %dth child of the %dth sch node 0x%x is 0x%x\n",
            queue_id, chan_id, cpu_queues->sched_nodes[chan_id], input_port);

    return BCM_E_NONE;
}

/*
 * Map the CPU queues to L0 scheduler node using : queue -> cos % 11
 */
bcm_error_t
cpu_queue_to_L0_node_remap(int unit)
{
    int cos;
    int rv = BCM_E_NONE;

    printf("==> cpu_queue_to_L0_node_remap\n");

    for (cos = 0; cos < NUM_CPU_QUEUES; cos++) {
        /*
         * Pick an arbitrary channel assignment, node 11 was reservered, queue will be attached
         * to sch node 11 when it was detached.
         * So, max value use (NUM_SCH_NODES-1) instead of NUM_SCH_NODES
         */
        int rx_queue_channel = cos % (NUM_SCH_NODES-1);
        printf("--> Map CPU COS %d to rx channel %d\n", cos, rx_queue_channel);
        if (BCM_FAILURE (rv = rx_queue_channel_set(unit, &save_cpu_queues, cos, rx_queue_channel))) {
            printf("Failed to set rx queue channel to %d on Rx cos %d: %s\n",
                   rx_queue_channel, cos, bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Set CPU queues to L0 scheduler node to default setting
 */
bcm_error_t
cpu_queue_to_L0_node_restore(int unit)
{
    int cos;
    int rv = BCM_E_NONE;
    int rx_queue_channel;

    printf("==> cpu_queue_to_L0_node_restore\n");

    for (cos = 0; cos < NUM_CPU_QUEUES; cos++) {
        rx_queue_channel = cos % (NUM_SCH_NODES-1);
        if (BCM_FAILURE(rv = bcm_cosq_gport_detach(unit,
                                 save_cpu_queues.cpu_queues[cos].gport,
                                 save_cpu_queues.sched_nodes[rx_queue_channel], cos))) {
            printf("Failed to detach cos %d from channel %d : %s\n",
                   cos, rx_queue_channel, bcm_errmsg(rv));
            return rv;
        }
        printf("Detach %dth CPU queue 0x%x from %dth L0 node(0x%x)\n",
               cos, save_cpu_queues.cpu_queues[cos].gport,
               rx_queue_channel, save_cpu_queues.sched_nodes[rx_queue_channel]);

        rx_queue_channel = save_cpu_queues.cpu_queues[cos].input_L0_num;
        if (BCM_FAILURE(rv = bcm_cosq_gport_attach(unit,
                                             save_cpu_queues.cpu_queues[cos].gport,
                                             save_cpu_queues.cpu_queues[cos].input_node, cos))) {
            printf("Failed to set rx queue channel to %d on Rx cos %d: %s\n",
                   rx_queue_channel, cos, bcm_errmsg(rv));
            return rv;
        }
        printf("Attach %dth CPU queue 0x%x to the %dth node of L0(0x%x)\n",
               cos, save_cpu_queues.cpu_queues[cos].gport,
               rx_queue_channel, save_cpu_queues.sched_nodes[rx_queue_channel]);
    }

    return BCM_E_NONE;
}

int rx_count;   /* Global received packet count */
int pkt_cb_failed = 0;
const uint8 priority = 101;
unsigned char expected_packets[68] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x81, 0x00, 0x00, 0x02,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
    0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
    0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,
    0xDF, 0xC5, 0xC3, 0xCB};

/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int    i, *count = cookie;
    void   *buffer;
    uint32 length;
    uint32 port;

    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get(unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &port));
    printf("pktioRxCallback: received from port %d\n", port);

    /* Get basic packet info */
    if (BCM_FAILURE(bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
        return BCM_PKTIO_RX_NOT_HANDLED;
    }

    if (sal_memcmp(buffer, expected_packets, length) == 0) {
        pkt_cb_failed = 0;
        printf("pktioRxCallback: PASS\n");
    } else {
        pkt_cb_failed = 1;
    }

    return BCM_PKTIO_RX_NOT_HANDLED;
}

/* Register callback function for received packets */
bcm_error_t
registerPktioRxCallback(int unit)
{
    const uint32 flags = 0;

    return bcm_pktio_rx_register(unit, "rx_cb", pktioRxCallback, priority, &rx_count, flags);
}

bcm_error_t
unregisterPktioRxCallback(int unit)
{
    return bcm_pktio_rx_unregister(unit, pktioRxCallback, priority);
}

bcm_error_t
test_setup(int unit)
{
    int rv = BCM_E_NONE;
    int port_list[1];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 1)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }
    ingress_port = port_list[0];
    printf("Ingress port : %d\n", ingress_port);

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

    /* Retrieve all the CPU scheduler gport */
    BCM_IF_ERROR_RETURN(cpu_sch_gport_retrieve(unit, &save_cpu_queues));

    /* Remap CPU queues to L0 sch nodes((queue id)% (NUM_SCH_NODES-1)) */
    if (BCM_FAILURE(rv = cpu_queue_to_L0_node_remap(unit))) {
        printf("cpu_queue_to_L0_node_remap() failed.\n");
        return -1;
    }

    /* bshell(unit, "pw start report +raw +decode"); */

    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Function to verify the result */
bcm_error_t
test_verify(int unit)
{
    char str[256];

    printf("\n======================================\n");
    printf("==> Verify unicast pkt from port %d forwarding to CPU port\n", ingress_port);
    printf("======================================\n\n");

    /* Send a packet with priority 0 to ingress port */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_MAC));
    snprintf(str, 256, "tx 1 pbm=%d data=0x00000000001100000000002281000002FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2DDFC5C3CB; sleep quiet 3", ingress_port);
    bshell(unit, str);

    BCM_IF_ERROR_RETURN(flex_ctr_check(unit, 0));

    printf("Per queue packet counter check successful.\n");

    if (pkt_cb_failed) {
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/*
 * Clean up pre-test setup.
 */
int test_cleanup(int unit)
{
    printf("==>Cleaning up...\n");

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_NONE));

    unregisterPktioRxCallback(unit);

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

    /* Restore CPU queues to L0 sch nodes mapping to default */
    if (BCM_E_NONE != cpu_queue_to_L0_node_restore(unit)) {
        printf("cpu_queue_to_L0_node_restore() failed.\n");
        return -1;
    }

    /* bshell(unit, "pw stop"); */

    /* Check output for port on which new scheduler profile is attached */
    /* bshell(unit, "dump sw cosq"); */

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    bcm_error_t rv;

    bshell(unit, "config show; attach; version");

    /* Retrieve all the CPU queues and L0 sch nodes, and save them to local data structure */
    if (BCM_FAILURE(rv = test_setup(unit))) {
        printf("test_setup() failed.\n");
        return -1;
    }
        printf("test_setup() DONE.\n");

    /* Verify the result (Check CPU queues to L2 sch nodes mapping) */
    if (BCM_FAILURE(rv = test_verify(unit))) {
         printf("test_verify() failed.\n");
         return -1;
    }

    if (BCM_FAILURE(rv = test_cleanup(unit))) {
         printf("test_cleanup() failed.\n");
         return -1;
    }

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}


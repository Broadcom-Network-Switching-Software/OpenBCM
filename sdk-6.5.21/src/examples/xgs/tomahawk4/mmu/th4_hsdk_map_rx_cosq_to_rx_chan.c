/*
 *  Feature  : CPU Queue to RX Channel Mapping
 *
 *  Usage    : BCM.0> cint th4_hsdk_map_rx_cosq_to_rx_chan.c
 *
 *  config   : bcm56990_a0-generic-64x400.config.yml
 *
 *  Log file : th4_hsdk_map_rx_cosq_to_rx_chan_log.txt
 *
 *  Test Topology :
 *                  +------------------------+
 *                  |                        |
 *    Test port     |                        |
 *    +-------------+                        |
 *                  |                        |
 *                  |                        |
 *                  |                        |
 *                  |    56990  SWITCH       |
 *                  |                        |
 *                  |                        |
 *                  |                        |
 *                 -|                        |
 *                  |                        |
 *                  |                        |
 *                  |                        |
 *                  |                        |
 *                  +------------------------+
 *
 *                  The single test port operates in loopback
 *
 *  Summary:
 *  ========
 *  The mechanism for configuring RX Queue channels on CMIC-X devices such
 *  as the 56990 (Tomahawk4) differs from many previous devices. Previous
 *  devices used bcm_rx_queue_channel_set() API which does not apply to
 *  CMIC-X devices.  For CMIC-X devices, the user needs to use a combination
 *  of cosq attach/detach operations on the CPU L0 gports. On these devices,
 *  the L0 nodes are directly wired to one of 7 RX DMA channels. The L0
 *  gports can be obtained with the bcm_cosq_gport_traverse() API. The
 *  L0 gport values do not change so they only need to be obtained once
 *  and be hardcoded. Once the L0 gports are known, the RX channels can be
 *  reconfigured as desired. The following CINT sample demonstrates how to
 *  use L0 GPORTs to reconfigure the RX channel configurations on CMIC 0 (CPU).
 *
 * bcm_error_t
 * th4_rx_queue_channel_set(int unit,
 *                          cpu_queues_t * th4_cpu_queues,
 *                          bcm_cos_queue_t queue_id,
 *                          bcm_rx_chan_t chan_id);
 *
 * The L0 nodes are mapped to CMIC-X RX DMA channels as follows:
 *   L0.0 => CMC0 CH1(RX)
 *   L0.1 => CMC0 CH2(RX)
 *   L0.2 => CMC0 CH3(RX)
 *   L0.3 => CMC0 CH4(RX)
 *   L0.4 => CMC0 CH5(RX)
 *   L0.5 => CMC0 CH6(RX)
 *   L0.6 => CMC0 CH7(RX)
 *   L0.7-L0.11 => Reserved
 *
 *   CMC0 CH0 is reserved for TX operations.
 *
 *  Detailed steps performed by this CINT script:
 *    =======================================
 *    1) Step1 - Test Setup; Done in test_setup()
 *      a) Confirm device type
 *      b) Initialize GPORT queue to channel map, verify against default
 *      c) Initialze packet counts to 0
 *      d) Register RX callback
 *      e) Configure packet priority to CPU queue mapping
 *      f) Put test port into loopback
 *      g) Configure test VLAN, add test port and cpu
 *
 *    =============================================
 *    2) Step2 - Configuration; Done in configure()
 *      Configure CPU COS to RX channel mapping
 *
 *    =======================================================
 *    3) Step3 - Verification; Done in verify()
 *      a) Display configuration information
 *      b) Transmit packets on test port
 *      c) Verify RX packet counts
 *    =======================================================
 *    Expected Result:
 *      The RX callback function maintains a count for the CPU COSQ on every
 *      received packet. During verification these counts are compared with the
 *      known queue counts for all transmitted packets. If the counts don't
 *      match a failure is logged. The BCM API does not provide a mechanism
 *      for retrieving per RX channel packet counts.  Per channel packet count
 *      information can be obtained directly from one of the CMIC registers.
 *      A function "dump_pktdma_counts()" is provided to display these counts.
 */

/* Reset CINT */
cint_reset();

/* The CPU always supports 48 queues. */
const int       MAX_CPU_QUEUES = 48;

/*
 * Each TH4 port supports 12 scheduler nodes.  For the CPU port, only the
 * first 7 are used for mapping CPU COSQs to RX DMA channels.
 */
const int       MAX_SCH_NODES = 12;

/* Structure for each of the 48 CPU queues */
typedef struct cpu_queue_t {
    bcm_gport_t     cpu_queue_gport;    /*  */
    bcm_gport_t     rx_channel_gport;
    int             rx_channel;
};

/* Structure used to store all gports needed for RX channel mapping */
typedef struct cpu_queues_t {
    cpu_queue_t     cpu_queues[MAX_CPU_QUEUES];
    bcm_gport_t     sched_nodes[MAX_SCH_NODES];
};

/*
 * For most application it is sufficient to simply include the following
 * default initial CPU GPORT mapping structure. The initial GPORT configuration
 * does not change so it really isn't necessary to rebuild it using the
 * GPORT traversal routines. This test case will verify that the map built
 * using traverse operations matches the default specified here.
 */
cpu_queues_t    th4_default_cpu_queues = {
    {
     {0x30000000, 0x37800000, 0},
     {0x30000001, 0x37800000, 0},
     {0x30000002, 0x37800000, 0},
     {0x30000003, 0x37800000, 0},
     {0x30000004, 0x37800000, 0},
     {0x30000005, 0x37800000, 0},
     {0x30000006, 0x37800000, 0},
     {0x30000007, 0x37800000, 0},
     {0x30000008, 0x37800000, 0},
     {0x30000009, 0x37800000, 0},
     {0x3000000a, 0x37800000, 0},
     {0x3000000b, 0x37800000, 0},
     {0x3000000c, 0x37800000, 0},
     {0x3000000d, 0x37800000, 0},
     {0x3000000e, 0x37800000, 0},
     {0x3000000f, 0x37800000, 0},
     {0x30000010, 0x37800000, 0},
     {0x30000011, 0x37800000, 0},
     {0x30000012, 0x37800000, 0},
     {0x30000013, 0x37800000, 0},
     {0x30000014, 0x37800000, 0},
     {0x30000015, 0x37800000, 0},
     {0x30000016, 0x37800000, 0},
     {0x30000017, 0x37800000, 0},
     {0x30000018, 0x37800000, 0},
     {0x30000019, 0x37800000, 0},
     {0x3000001a, 0x37800000, 0},
     {0x3000001b, 0x37800000, 0},
     {0x3000001c, 0x37800000, 0},
     {0x3000001d, 0x37800000, 0},
     {0x3000001e, 0x37800000, 0},
     {0x3000001f, 0x37800000, 0},
     {0x30000020, 0x37800000, 0},
     {0x30000021, 0x37800000, 0},
     {0x30000022, 0x37800000, 0},
     {0x30000023, 0x37800000, 0},
     {0x30000024, 0x37800000, 0},
     {0x30000025, 0x37800000, 0},
     {0x30000026, 0x37800000, 0},
     {0x30000027, 0x37800000, 0},
     {0x30000028, 0x37800000, 0},
     {0x30000029, 0x37800000, 0},
     {0x3000002a, 0x37800000, 0},
     {0x3000002b, 0x37800000, 0},
     {0x3000002c, 0x37800000, 0},
     {0x3000002d, 0x37800000, 0},
     {0x3000002e, 0x37800000, 0},
     {0x3000002f, 0x37800000, 0}
     },
    {0x37800000, 0x37810000, 0x37820000, 0x37830000, 0x37840000, 0x37850000,
     0x37860000, 0x37870000, 0x37880000, 0x37890000, 0x378a0000, 0x378b0000}
};

/*******************************************************************************
 * Function: th4_rx_queue_channel_set
 *
 * Configure a CPU queue to PCI RX channel connection. This routine can be
 * used in place of bcm_rx_queue_channel_set() for Tomahawk4. Mapped the
 * specified queue to the specified RX channel;
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   th4_cpu_queues: Pointer to data stucture containing all current COSQ
 *     to RX channel mapping information.
 *   queue_id: COSQ number (0..47)
 *   chan_id: RX channel (0..6);
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
th4_rx_queue_channel_set(int unit, cpu_queues_t * th4_cpu_queues,
                         bcm_cos_queue_t queue_id, bcm_rx_chan_t chan_id)
{
    /* Detach existing L0 node */
    BCM_IF_ERROR_RETURN(bcm_cosq_gport_detach
                        (unit,
                         th4_cpu_queues->cpu_queues[queue_id].cpu_queue_gport,
                         th4_cpu_queues->sched_nodes[th4_cpu_queues->cpu_queues
                                                     [queue_id].rx_channel],
                         queue_id));

    /* Update mapping structure */
    th4_cpu_queues->cpu_queues[queue_id].rx_channel = chan_id;
    th4_cpu_queues->cpu_queues[queue_id].rx_channel_gport =
      th4_cpu_queues->sched_nodes[chan_id];

    /* Attach new L0 node */
    BCM_IF_ERROR_RETURN(bcm_cosq_gport_attach
                        (unit,
                         th4_cpu_queues->cpu_queues[queue_id].cpu_queue_gport,
                         th4_cpu_queues->sched_nodes[th4_cpu_queues->cpu_queues
                                                     [queue_id].rx_channel],
                         queue_id));
    printf("Mapped COSQ %2d (GPORT 0x%08X) to RX Channel %d (GPORT 0x%08X)\n",
           queue_id, th4_cpu_queues->cpu_queues[queue_id].cpu_queue_gport,
           chan_id,
           th4_cpu_queues->sched_nodes[th4_cpu_queues->cpu_queues[queue_id].
                                       rx_channel]);

    return BCM_E_NONE;
}

/*************************************************************************
 * Function: cosq_gport_traverse_callback
 *
 * This is a callback function called via bcm_cosq_gport_traverse(). The
 * callback is used to iterate through all GPORTs. GPORTs not associated
 * with the CPU are ignored.  CPU GPORTS are collected and used to populate
 * the CPU queue GPORT data structure.
 *
 * This function must be of type bcm_cosq_gport_traverse_cb:
 * typedef int (*bcm_cosq_gport_traverse_cb)(
 *    int unit,
 *    bcm_gport_t port,
 *    int numq,
 *    uint32 flags,
 *    bcm_gport_t gport,
 *    void *user_data);
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   port: The parent port for the gport
 *   numq: Number of queues (unused)
 *   flags: Flags indicating gport type
 *   gport: The Gport for this node
 *   user_data: A generic, user defined pointer.In this case points to
 *     GPORT mapping data structure.
 *
 * Returns:
 *   BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 *   for failure.
 */
bcm_error_t
cosq_gport_traverse_callback(int unit, bcm_gport_t port, int numq, uint32 flags,
                             bcm_gport_t gport, void *user_data)
{
    const int       CPU_PORT = 0;

    /* Get pointer to CPU queue structure */
    const cpu_queues_t *th4_cpu_queues = (auto) user_data;

    /* Variables for bcm_cosq_gport_get() and bcm_cosq_gport_attach_get() */
    int             num_cos_levels;
    int             logical_port;
    int             cosq;
    bcm_gport_t     parent;
    uint32          port_flags;

    /* Get information associated with this GPORT */
    BCM_IF_ERROR_RETURN(bcm_cosq_gport_get
                        (unit, gport, &logical_port, &num_cos_levels,
                         &port_flags));

    if (logical_port != CPU_PORT) {
        /* Reject front panel ports */
        return BCM_E_NONE;
    }

    /* Get attached parent gport */
    BCM_IF_ERROR_RETURN(bcm_cosq_gport_attach_get(unit, gport, &parent, &cosq));

    if (flags == BCM_COSQ_GPORT_MCAST_QUEUE_GROUP) {
        /* Multicast queue gport is one of 48 CPU queues */
        th4_cpu_queues->cpu_queues[cosq].cpu_queue_gport = gport;
        th4_cpu_queues->cpu_queues[cosq].rx_channel_gport = parent;
        th4_cpu_queues->cpu_queues[cosq].rx_channel = -1;
    } else if (flags == BCM_COSQ_GPORT_SCHEDULER) {
        /*
         * Each CPU queue feeds one of 12 attached scheduler nodes. It should
         * be noted that only 7 of these queues are valid. By default,
         * all queues are connected to node 0 (CMIC-X CMC0, RX channel 1).
         */
        th4_cpu_queues->sched_nodes[cosq] = gport;
    }
    return BCM_E_NONE;
}

/*************************************************************************
 * Function: populate_queue_map
 *
 * Populate the GPORT CPU queue mapping data strucure using
 * bcm_cosq_gport_traverse().  See cosq_gport_traverse_callback()
 * for details.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   th4_cpu_queues: Pointer to structure containing GPORT queue mapping
 *      information.
 *
 * Returns:
 *   BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 *   for failure.
 */
bcm_error_t
populate_queue_map(int unit, cpu_queues_t * th4_cpu_queues)
{
    bcm_error_t     ret_val;

    if (BCM_FAILURE
        (ret_val =
         bcm_cosq_gport_traverse(unit, cosq_gport_traverse_callback,
                                 th4_cpu_queues))) {
        printf("bcm_cosq_gport_traverse FAILED:\n", bcm_errmsg(ret_val));
        return ret_val;
    }

    return BCM_E_NONE;
}

/*************************************************************************
 * Function: initialize_queue_map
 *
 * Populate the CPU queue data structure and then link the scheduler nodes
 * to the appropriate CPU queue notes.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   th4_cpu_queues: Pointer to structure containing GPORT queue mapping
 *      information.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
initialize_queue_map(int unit, cpu_queues_t * th4_cpu_queues)
{
    int             queue_idx;
    int             sched_idx;

    /* Initialize queue to channel map */
    sal_memset(th4_cpu_queues, 0, sizeof(*th4_cpu_queues));

    BCM_IF_ERROR_RETURN(populate_queue_map(unit, th4_cpu_queues));

    for (queue_idx = 0; queue_idx < MAX_CPU_QUEUES; queue_idx++) {
        /* Find scheduler node index (RX channel) */
        for (sched_idx = 0; sched_idx < MAX_SCH_NODES; sched_idx++) {
            if (th4_cpu_queues->cpu_queues[queue_idx].rx_channel_gport ==
                th4_cpu_queues->sched_nodes[sched_idx]) {
                break;
            }
        }
        /* Set channel mapping info. */
        th4_cpu_queues->cpu_queues[queue_idx].rx_channel = sched_idx;
    }

    return BCM_E_NONE;
}

/*************************************************************************
 * Function: dump_channel_mapping
 *
 * Print current CPU COS to channel mapping based on contents of CPU queue
 * structure.
 *
 * Parameters:
 *   th4_cpu_queues: Pointer to structure containing GPORT queue mapping
 *      information.
 *
 * Returns:
 *   N/A
 */
void
dump_channel_mapping(cpu_queues_t * th4_cpu_queues)
{
    int             cosq;

    printf("Dump RX channel map:\n");
    for (cosq = 0; cosq < MAX_CPU_QUEUES; cosq++) {
        printf("  CPU COSQ %2d is mapped to RX DMA channel %d\n",
               cosq, th4_cpu_queues->cpu_queues[cosq].rx_channel);
    }
    printf("\n");
}

/*************************************************************************
 * Function: rx_cosq_mapping
 *
 * Configure RX COSQ Mapping. In this case, packets are mapped to a particular
 * CPU COS based on their internal priority (0..15). For real applications,
 * customers can set up any mapping they choose depending on the needs of
 * the application.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   first_cos: First COS to receive "priority" packets.
 *
 * Returns:
 *   BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 *   for failure.
 */
bcm_error_t
rx_cosq_mapping(int unit, int first_cos)
{
    const int       min_priority = 8;
    const int       int_pri_count = 16;
    const uint32    options = 0;

    int             internal_priority;

    printf("RX COSQ Mapping:\n");
    for (internal_priority = 0; internal_priority < int_pri_count;
         internal_priority++) {
        bcm_rx_cosq_mapping_t rx_mapping;

        bcm_rx_cosq_mapping_t_init(&rx_mapping);
        /*
         * Set match keys:
         * Packets with matching internal priority will be directed to the
         * COSQ specified below unless the packet matches a different rule
         * with a higher mapping priority.  For example if a match rule is
         * specified for a specific "reason" code and that rule was specified
         * with a higher mapping priority, the packet will be directed to the
         * COSQ associate with the "reason" rule, not the COSQ specified here.
         */
        /* Set internal priority value */
        rx_mapping.int_prio = internal_priority;

        /* Set mask for internal priority value */
        rx_mapping.int_prio_mask = 0xF;

        /*
         * The parameter "priority" sets the precedence for this match
         * rule. Since all of the match rules for mapping interal priority to
         * CPU COS are mutually exclusive, you can set the priority for each
         * mapping to the same value and get the same result. Set the priority
         * to determine behavior when other match rules (not specified yet)
         * overlap with these.
         */
        rx_mapping.priority = 10;

        /* Index into COSQ mapping table (0 is highest match priority) */
        rx_mapping.index = int_pri_count - internal_priority + 4;

        /*
         * There are 48 COSQs for the CPU, packets that match on internal
         * priority will be mapped starting with "first_cos". This allows
         * us to save the lower mapping entries for other types of packets.
         */
        rx_mapping.cosq = internal_priority + first_cos;

        printf("  Map internal priority %2d to CPU COS %2d (priority=%2d)\n",
               internal_priority, rx_mapping.cosq, rx_mapping.priority);

        /* Add next CPU COS mapping entry */
        BCM_IF_ERROR_RETURN(bcm_rx_cosq_mapping_extended_add
                            (unit, options, &rx_mapping));
    }
    printf("\n");
    return BCM_E_NONE;
}

/* Structure used for total and per queue RX packet counts. */
typedef struct packet_counts_t {
    int             total;
    int             per_queue[MAX_CPU_QUEUES];
};

/*************************************************************************
 * Function: initialize_packet_counts
 *
 * Reset packet counts.
 *
 * Parameters:
 *   packet_counts: Pointer to structure containing total and per queue
 *     packet count information.
 *
 * Returns:
 *   N/A
 */
void
initialize_packet_counts(packet_counts_t * packet_counts)
{
    sal_memset(packet_counts, 0, sizeof(*packet_counts));
}

/*******************************************************************************
 * Function: pktioRxCallback
 *
 * RX callback function for applications using the HSDK PKTIO facility.
 * This function tracks the number of packets received by the callback
 * function and prints some basic information for each packet received.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   packet: Pointer to incoming packet and associated RX metadata.
 *   cookie: Generic pointer, in this case a pointer to current RX packet
 *     counts.
 *
 * Returns:
 *   Returns packet handled status, BCM_PKTIO_RX_HANDLED or
 *   BCM_PKTIO_RX_NOT_HANDLED.
 */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    const int       verbose = 0;        /* Be silent by default */

    packet_counts_t *packet_counts = (auto) cookie;

    uint32          src_port;
    uint32          pkt_len;
    uint32          outer_vid;
    uint32          outer_pri;
    uint32          cpu_cos;

    /* Get packet metadata and display for each received packet. */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &src_port));

    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_PKT_LENGTH, &pkt_len));

    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_OUTER_VID, &outer_vid));

    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_OUTER_PRI, &outer_pri));

    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_CPU_COS, &cpu_cos));

    /* Bump total received packet count */
    packet_counts->total++;

    /* Bump per queue received packet count */
    packet_counts->per_queue[cpu_cos]++;

    printf
      ("RX packet %2d: src_port: %2d; len: %4d; vid: %4d; pri: %d; cos: %2d\n",
       packet_counts->total, src_port, pkt_len, outer_vid, outer_pri, cpu_cos);

    /* Packet handled. Dispatcher will skip remaining callbacks. */
    return BCM_PKTIO_RX_HANDLED;
}

/*******************************************************************************
 * Function: registerPktioRxCallback
 *
 * Register callback function for received packets.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   packet_counts: A pointer to current RX packet total and per queue counts.
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
registerPktioRxCallback(int unit, packet_counts_t * packet_counts)
{
    const uint8     priority = 100;
    const uint32    flags = 0;  /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktioRxCallback, priority, packet_counts, flags);
}

/*************************************************************************
 * Function: do_tx
 *
 * Transmit test packets using the built in "tx" command. Transmit a different
 * number of packets for each VLAN priority.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   test_port: Port name to transmit packets on.
 *   vlanid: VLAN to transmit packets on.
 *
 * Returns:
 *   N/A
 */
void
do_tx(int unit, char *test_port, bcm_vlan_t vlanid)
{
    char            command[64];
    int             vlan_pri;
    int             total_packets = 0;

    /* TX some number of packets onto each of 7 RX channels */
    for (vlan_pri = 0; vlan_pri < 7; vlan_pri++) {
        int             packet_count = vlan_pri + 1;
        sprintf(command, "tx %d pbm=%s VLantag=%d VlanPrio=%d",
                vlan_pri + 1, test_port, vlanid, vlan_pri);
        bshell(unit, command);
        /*
         * A sleep is needed here to allow RX callback to print RX packet
         * information.
         */
        sal_sleep(1);
        total_packets += packet_count;
    }
    printf("Transmitted %d packets on port %s, VLAN %d\n\n", total_packets,
           test_port, vlanid);
}

/*************************************************************************
 * Function: dump_pktdma_counts
 *
 * Currently the SDK does not support a mechanism for retrieving per RX
 * channel packet counts.  The CMIC hardware track these counts in the
 * CMIC_CMC<X>_PKTDMA_CH<Y>_PKT_COUNT_RXPKT set of registers. This function
 * uses the HSDK debug shell to get the RX packet counts from these registers.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *
 * Returns:
 *   N/A
 */
void
dump_pktdma_counts(int unit)
{
    const int       cmc = 0;    /* All packet DMA occurs on CMC0 */
    char            command[80];
    int             channel;

    printf("Dump CMC/Channel RX packet counts:\n");
    printf("  For this test, expected counts are 7, 6, 5, 4, 3, 2, 1\n");
    for (channel = 1; channel < 8; channel++) {
        /* Constuct an HSDK diag shell command to get counts from register */
        sprintf(command,
                "dsh -c \"Get CMIC_CMC%d_PKTDMA_CH%d_PKT_COUNT_RXPKTr count\"",
                cmc, channel);
        bshell(unit, command);
    }
    printf("\n");
}

/*************************************************************************
 * Function: get_device_type
 *
 * Get current device type as listed on PCI bus. Used to determine if current
 * device is supported by this test.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   dev_type: Pointer to receive device type code.
 *
 * Returns:
 *   BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 *   for failure.
 */
bcm_error_t
get_device_type(int unit, uint32 * dev_type)
{
    bcm_info_t      info;

    BCM_IF_ERROR_RETURN(bcm_info_get(unit, &info));
    /*
     * The info struct takes the vendor, device and revision from the PCI
     * config space.
     */

    *dev_type = info.device & 0x0000FFF0;

    return BCM_E_NONE;
}

/*************************************************************************
 * Function: test_setup
 *
 * Perform assorted set up operations for this test, including initialization
 * and device type checks.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   th4_cpu_queues: Pointer to structure containing RX channel mapping
 *       collected during traverse operation.
 *   packet_counts: Pointer to structure used to maintain total and per
 *       queue packet counts.
 *   test_port: Port used for TX/RX operations
 *   vlan_id: VLAN used for TX/RX operations
 *   first_cos: First CPU COS to for packets mapped by priority.
 *
 * Returns:
 *   BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 *   for failure.
 */
bcm_error_t
test_setup(int unit, cpu_queues_t * th4_cpu_queues,
           packet_counts_t * packet_counts, char *test_port, bcm_vlan_t vlanid,
           int first_cos)
{
    bcm_error_t     rv;
    char            command[64];
    int             idx;
    uint32          dev_type;
    int             setup_failed = 0;

    /* Test test only supported for 5699X (Tomahawk4), reject others */
    BCM_IF_ERROR_RETURN(get_device_type(unit, &dev_type));
    if (dev_type != 0xB990) {
        printf("Unsupported device type/configuration: 0x%4X\n", dev_type);
        return BCM_E_FAIL;
    }

    /* Get current CPU queue to RX channel mapping */
    BCM_IF_ERROR_RETURN(initialize_queue_map(unit, th4_cpu_queues));

    /*
     * Compare information derived from GPORT traversal with the expected
     * default.  Typical customer applications can just use the default and
     * skip the traversal process.
     */
    for (idx = 0; idx < MAX_CPU_QUEUES; idx++) {
        int             fail =
          (th4_default_cpu_queues.cpu_queues[idx].cpu_queue_gport !=
           th4_cpu_queues->cpu_queues[idx].cpu_queue_gport) ||
          (th4_default_cpu_queues.cpu_queues[idx].rx_channel !=
           th4_cpu_queues->cpu_queues[idx].rx_channel) ||
          (th4_default_cpu_queues.cpu_queues[idx].rx_channel_gport !=
           th4_cpu_queues->cpu_queues[idx].rx_channel_gport);
        if (fail) {
            printf("Default GPORT queue mapping does not match at COS %d\n",
                   idx);
        }
        setup_failed = setup_failed || fail;
    }
    for (idx = 0; idx < MAX_SCH_NODES; idx++) {
        int             fail = (th4_cpu_queues->sched_nodes[idx] !=
                                th4_default_cpu_queues.sched_nodes[idx]);
        if (fail) {
            printf
              ("Default GPORT scheduler mapping does not match at node %d\n",
               idx);
        }
        setup_failed = setup_failed || fail;
    }
    if (setup_failed) {
        return BCM_E_FAIL;
    }

    /* Reset total and per queue packet maps */
    initialize_packet_counts(packet_counts);

    /* Register a simple RX callback function to collect packet counts. */
    if (BCM_FAILURE(rv = registerPktioRxCallback(unit, packet_counts))) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * Set up mapping for CPU queues. Map packets based on priority starting
     * with "first_cos".
     */
    if (BCM_FAILURE(rv = rx_cosq_mapping(unit, first_cos))) {
        printf("bcm_rx_cosq_mapping_extended_add() FAILED: %s\n",
               bcm_errmsg(rv));
        return rv;
    }

    /* Put test port into MAC loopback */
    sprintf(command, "port %s lb=mac", test_port);
    bshell(unit, command);

    /* Create a test VLAN and add the test port and CPU to it */
    sprintf(command, "vlan create %d pbm=%s,cpu", vlanid, test_port);
    bshell(unit, command);

    return BCM_E_NONE;
}

/*************************************************************************
 * Function: configure
 *
 * Configure CPU COS to RX channel mapping using the utility function
 * "th4_rx_queue_channel_set()". The mapping done here is arbitrary, customer
 * applications will want to assign high priority CPU traffic to a separate
 * channel to minimize RX latency when there's a lot of RX traffic.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   th4_cpu_queues: Pointer to structure containing RX channel mapping
 *       collected during traverse operation.
 *   first_cos: First CPU COS to for packets mapped by priority.
 *
 * Returns:
 *   BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 *   for failure.
 */
bcm_error_t
configure(int unit, cpu_queues_t * th4_cpu_queues, int first_cos)
{
    int             cpu_queue;  /* Range 0..47 */
    int             rx_channel; /* Range 0..6 */

    printf("Configure packet priority to CPU queue mapping:\n");
    rx_channel = 6;
    for (cpu_queue = 0; cpu_queue < MAX_CPU_QUEUES; cpu_queue++) {
        BCM_IF_ERROR_RETURN(th4_rx_queue_channel_set(unit, th4_cpu_queues,
                                                     cpu_queue, rx_channel));
        if ((cpu_queue >= first_cos) && (rx_channel > 0)) {
            rx_channel--;
        }
    }
    printf("\n");
    return BCM_E_NONE;
}

/*************************************************************************
 * Function: verify
 *
 * Print configuration information, transmit packets on test port and verify
 * RX packet counts.  It is not currently possible using the BCM API to
 * retrieve per TX channel packet counts. These packet counts will need to
 * be verifed by reading the CMIC_CMC<X>_PKTDMA_CH<Y>_PKT_COUNT_RXPKT set
 * of registers and confirming the individual count values.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   th4_cpu_queues: Pointer to structure containing RX channel mapping
 *       collected during traverse operation.
 *   packet_counts: Data struture containt total and per queue packet counts.
 *   test_port: Port used for TX/RX operations
 *   vlan_id: VLAN used for TX/RX operations
 *   first_cos: First CPU COS to for packets mapped by priority.
 *
 * Returns:
 *   BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 *   for failure.
 */
bcm_error_t
verify(int unit, cpu_queues_t * th4_cpu_queues, packet_counts_t * packet_counts,
       char *test_port, bcm_vlan_t vlanid, int first_cos)
{
    int             cpu_queue;
    int             test_failed;

    dump_channel_mapping(th4_cpu_queues);

    printf("Transmit test packets on %s\n", test_port);
    do_tx(unit, test_port, vlanid);

    /* Print RX packet per channel counts using register read operation. */
    dump_pktdma_counts(unit);

    /* Verify RX per queue packet counts against know TX counts */
    test_failed = 0;
    for (cpu_queue = 0; cpu_queue < MAX_CPU_QUEUES; cpu_queue++) {
        int             fail;

        if ((cpu_queue < first_cos) || (cpu_queue > (first_cos + 6))) {
            fail = packet_counts->per_queue[cpu_queue] != 0;
        } else {
            fail =
              packet_counts->per_queue[cpu_queue] !=
              (cpu_queue - first_cos + 1);
        }
        if (fail) {
            printf("Unexpected packet count on queue %d: %d\n", cpu_queue,
                   packet_counts->per_queue[cpu_queue]);
        }
        test_failed = test_failed || fail;
    }
    printf("Verifcation: %s\n", test_failed ? "Failed" : "Passed");
    return test_failed ? BCM_E_FAIL : BCM_E_NONE;
}

/*************************************************************************
 * Function: execute
 *
 * Top level test function. Entry point for this test, calls all other
 * functions.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 *   for failure.
 */
bcm_error_t
execute()
{
    /* Test parameters */
    const bcm_vlan_t test_vlan = 2;
    const char     *test_port = "cd0";
    const int       unit = 0;
    const int       first_cos = 10;

    bcm_error_t     rv;
    cpu_queues_t    th4_cpu_queues;
    packet_counts_t packet_counts;

    /* Initial setup */
    if (BCM_FAILURE
        ((rv =
          test_setup(unit, &th4_cpu_queues, &packet_counts, test_port,
                     test_vlan, first_cos)))) {
        printf("test_setup() failed.\n");
        return rv;
    }

    /* Configure CPU COS to RX channel mapping. */
    if (BCM_FAILURE((rv = configure(unit, &th4_cpu_queues, first_cos)))) {
        printf("configure() failed.\n");
        return rv;
    }

    /* Verify the results */
    rv =
      verify(unit, &th4_cpu_queues, &packet_counts, test_port, test_vlan,
             first_cos);
    return rv;
}

const char     *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print           execute();
}

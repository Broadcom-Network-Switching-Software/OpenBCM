/*
 *  Feature  : COSQ Debug Stat Counters
 *
 *  Usage    : BCM.0> cint th4_hsdk_cosq_debug_stat.c
 *
 *  config   : bcm56990_a0-generic-64x400.config.yml
 *
 *  Log file : th4_hsdk_cosq_debug_stat_log.txt
 *
 *  Test Topology :
 *                  +------------------------+
 *                  |                        |
 *    Test port 1   |                        |
 *    +-------------+                        |
 *                  |                        |
 *    Test port 2   |                        |
 *    +-------------+                        |
 *                  |    56990  SWITCH       |
 *                  |                        |
 *                  |                        |
 *    Test port n-1 |                        |
 *    +-------------+                        |
 *                  |                        |
 *    Test port n   |                        |
 *    +-------------+                        |
 *                  |                        |
 *                  +------------------------+
 *
 *                  All test ports operate in loopback
 *
 *  Summary:
 *  ========
 *    This CINT example configures and reads the COSQ Debug Stat Counters using
 *    BCM HSDK APIs. There are two groups of counters "Store and Forward" (SAF)
 *    and "Cut Through" (CT) in the 56990. They can be configured and read
 *    separately. It is possible to set up counters to match on the following:
 *    1) Source Port
 *    2) Source Pipe
 *    3) Destination Port
 *    4) Destination Port and Egress Queue
 *    5) Destination Pipe
 *    6) Priority Group.
 *
 *    This test uses the PKTIO API to send and receive test packets.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Create a list of test ports
 *      b) Configure each port in port list
 *      c) Configure a VLAN for each port in port list
 *      d) Add an L2 entry for each port in port list
 *      e) Register a simple RX callback
 *      f) Set up egress queue flex counters
 *
 *    2) Step2 - Configuration
 *    =======================================================
 *      There is no configuration step for this test. The COSQ debug stat
 *      counter configuration is performed as part of the verification step.
 *
 *    Step3 - Verification (Done in verify())
 *    =======================================
 *      3.1) Test packet counts based on source port:
 *           cosqDebugCounterSourcePortTest()
 *         a) Clear packet counters with bcm_cosq_stat_set()
 *         b) Configure counters to match on source ports with
 *            bcm_cosq_debug_stat_match_set()
 *         c) Transmit packets with sendPacket()
 *         d) Read back counters with bcm_cosq_stat_sync_get()
 *         e) Compare counter values with expected packet counts.
 *      3.2) Test packet counts based on egress port and queue:
 *           cosqDebugCounterDestinationPortQueueTest()
 *         a) Clear packet counters with bcm_cosq_stat_set()
 *         b) Configure counters to match on destination port and queue with
 *            bcm_cosq_debug_stat_match_set()
 *         c) Transmit packets with sendPacket()
 *         d) Read back counters with bcm_cosq_stat_sync_get()
 *         e) Compare counter values with expected packet counts.
 *      3.3) Expected Result:
 *         Compare actual counts with expect counts to determine pass/fail.
 *
 *    Important Usage Note:
 *    Internal to the 56990, to optimize performance, ingress traffic is
 *    divided into two pipe groups. Each pipe group has its own set of SAF/CT
 *    counter groups. There are 14 total counter for each of SAF and CT
 *    traffic, 7 counters for each pipe group.
 *
 *    Traffic that ingresses on pipes 0, 1, 2, 3, 12, 13, 14 and 15 goes to
 *    pipe group 0.
 *    Traffic that ingresses on pipes 4, 5, 6, 7, 8, 9, 10 and 11 goes to
 *    pipe group 1.
 *
 *    Use the "show pmap" command to see which pipe each front panel port
 *    is mapped to.
 *
 *    When configuring the counters, the "port" input parameter of
 *    bcm_cosq_debug_stat_match_set() and bcm_cosq_stat_sync_get() should be
 *    set to ANY SOURCE port from the pipe group that the user wants to
 *    match on (even when matching on destination ports). To match on ALL
 *    packets going to a particular destination port, set up TWO counters and
 *    select any one port from each pipe group for the "port" parameter.
 */

/* Reset CINT */
cint_reset();

/*
 * Define a structure that contains per port test information. This structure
 * is used to pass test information to the various functions defined for
 * this test.
 */
typedef struct port_info_t {
    /* Logical port number */
    bcm_port_t          port;

    /* Port name as listed by diagnostic shell */
    char                name[8];

    /* Default priority assigned to untagged packets that ingress on this port */
    int                 dot1p_pri;

    /* Default VLAN assigned to untagged packets that ingress on this port */
    bcm_vlan_t          vlan;
};

/*******************************************************************************
 * Function: portConfigGet
 *
 * Iterate through ports defined on this device. Assign a port name, DOT1P
 * priority and VLAN for each port. The list of test ports is distributed
 * across the range of available ports to ensure that there is at least one
 * test port for each pipe.
 *
 * Parameters:
 *   unit: Unit number for device under test
 *   port_list: Array for test port information
 *   num_ports: Number of ports to add to port_list
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
*/
bcm_error_t
portConfigGet(int unit, port_info_t * port_list, int num_ports)
{
    const bcm_vlan_t    first_test_VLAN = 1000;

    bcm_error_t         rv;
    bcm_pbmp_t          ports_pbmp;
    bcm_port_config_t   config;
    bcm_port_t          port;
    int                 idx;
    int                 port_count;

    /* Get the list of "cd" ports defined on this device */
    if (BCM_FAILURE(rv = bcm_port_config_get(unit, &config))) {
        printf("bcm_port_config_get() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    port_count = 0;
    idx = 0;
    /*
     * The expectation is that the device defines sufficient "cd" ports to
     * meet the requested number of ports "num_ports".
     */
    BCM_PBMP_ITER(config.cd, port) {
        if (port_count == num_ports) {
            /* Number of requested ports satisfied, port list is complete. */
            break;
        }
        /*
         * Try to spread the test ports across all available ports by skipping
         * adjacent ports.
         */
        /* Select every third port */
        if ((idx % 3) == 0) {
            port_info_t        *port_info = &port_list[port_count];
            /* Save logical port number */
            port_info->port = port;
            /* Construct a port name */
            sprintf(port_info->name, "cd%d", idx);
            /* Test VLANs start at first_test_VLAN */
            port_info->vlan = first_test_VLAN + port_count;
            /* Cycle through all priorities */
            port_info->dot1p_pri = port_count % 8;
            port_count++;
        }
        idx++;
    }
    if (port_count != num_ports) {
        /* If number of ports could not be satisfied, return error */
        printf("Number of requested ports could not be satisfied\n");
        return BCM_E_PORT;
    }
    printf("Test configuration:\n");
    printf("  %5s %4s %3s %4s\n", "Name", "Port", "Pri", "VLAN");
    for (idx = 0; idx < num_ports; idx++) {
        port_info_t        *port_info = &port_list[idx];

        printf("  %5s %4d %3d %4d\n", port_info->name,
               port_info->port, port_info->dot1p_pri, port_info->vlan);
    }
    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: configureVlan
 *
 * Create a VLAN and add two ports to it.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   vlan: VLAN to configure
 *   port: Test port
 *   partner: The "partner" to the test port which shares the same VLAN
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
int
configureVlan(int unit, bcm_vlan_t vlan, bcm_port_t port, bcm_port_t partner)
{
    bcm_error_t         rv;
    bcm_pbmp_t          pbmp, ubmp;

    rv = bcm_vlan_create(unit, vlan);
    /* Not an error if VLAN already exists */
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("bcm_vlan_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* Packets on VLAN should egress untagged on both ports */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(ubmp, port);
    BCM_PBMP_PORT_ADD(ubmp, partner);

    /* This test requires two ports per VLAN */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_PBMP_PORT_ADD(pbmp, partner);

    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_port_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: configureL2
 *
 * Add a single forwarding entry to the L2 table for a given port and
 * VLAN. This ensures that all test traffic is unicast and egresses on
 * unicast queues.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   port: L2 destination port
 *   vlan: L2 destination VLAN
 *   macaddr: L2 destination MAC address
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
configureL2(int unit, bcm_port_t port, bcm_vlan_t vlan, bcm_mac_t macaddr)
{
    bcm_l2_addr_t       l2addr;

    /* Set unicast forwarding for port */
    bcm_l2_addr_t_init(&l2addr, macaddr, vlan);
    l2addr.port = port;
    l2addr.flags = BCM_L2_STATIC;
    return bcm_l2_addr_add(unit, &l2addr);
}

/*******************************************************************************
 * Function: configurePort
 *
 * Configure a single test port.
 *
 * a) Put port into loopback
 * b) Set the untagged VLAN for port
 * c) Set the untagged packet priority for port
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   port_info: Test port configuration information
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
configurePort(int unit, port_info_t * port_info)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set
                        (unit, port_info->port, BCM_PORT_LOOPBACK_PHY));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set
                        (unit, port_info->port, port_info->vlan));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_priority_set
                        (unit, port_info->port, port_info->dot1p_pri));
    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: pktioRxCallback
 *
 * Rx callback function for applications using the HSDK PKTIO facility. By
 * default, just track the number of packets received by the callback function.
 * Setting the "verbose" flag causes each packet to be displayed.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   packet: Pointer to incoming packet and associated RX metadata.
 *   cookie: Generic pointer, in this case a pointer to current RX packet count.
 *
 * Returns:
 *   Returns packet handled status, BCM_PKTIO_RX_HANDLED or
 *   BCM_PKTIO_RX_NOT_HANDLED.
 */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int                *count = (auto) cookie;
    const int           verbose = 0;    /* Be silent by default */

    (*count)++; /* Bump received packet count */

    if (verbose) {
        void               *buffer;
        uint32              length;
        uint32              src_port;

        printf
          ("---RX Callback------------------------------------------------------------------\n");
        /* Get basic packet info */
        if (BCM_FAILURE
            (bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
            return BCM_PKTIO_RX_NOT_HANDLED;
        }
        /* Get source port metadata */
        BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                            (unit, packet, bcmPktioPmdTypeRx,
                             BCM_PKTIO_RXPMD_SRC_PORT_NUM, &src_port));

        printf("RX packet %d: Source port: %d; %d bytes\n", *count, src_port, length);
        printf
          ("--------------------------------------------------------------------------------\n");
    }

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
 *   count: A pointer to current RX packet count.
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
registerPktioRxCallback(int unit, int *count)
{
    const uint8         priority = 100;
    const uint32        flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktioRxCallback, priority, count, flags);
}

/*******************************************************************************
 * Function: test_setup
 *
 * Main test configuration routine. All test ports are chained together in
 * a front panel "snake" configuration using VLANs with the CPU port being
 * the last port in the chain. When packets are injected into any port in the
 * chain, they will propagate down the "snake" until they reach the CPU port.
 *
 * a) Create a list of test ports
 * b) Configure each port in port list
 * c) Configure a VLAN for each port in port list
 * d) Add an L2 entry for each port in port list
 * e) Register simple RX callback
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   port_list: List of test ports
 *   num_ports: Number of ports in port list
 *   macaddr: MAC address for L2 table entries
 *   count: Pointer to current RX packet count
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
test_setup(int unit, port_info_t * port_list, int num_ports, bcm_mac_t macaddr,
           int *count)
{
    int                 i;
    bcm_error_t         rv;

    /* Get test port list */
    BCM_IF_ERROR_RETURN(portConfigGet(unit, port_list, num_ports));

    /* Configure each port in the list */
    for (i = 0; i < num_ports; i++) {
        const bcm_port_t    port = port_list[i].port;
        const bcm_port_t    partner = (i < (num_ports - 1)) ? port_list[i + 1].port : 0;

        BCM_IF_ERROR_RETURN(configurePort(unit, &port_list[i]));
        BCM_IF_ERROR_RETURN(configureVlan(unit, port_list[i].vlan, port, partner));
        BCM_IF_ERROR_RETURN(configureL2(unit, partner, port_list[i].vlan, macaddr));
    }
    if (BCM_FAILURE(rv = registerPktioRxCallback(unit, count))) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* Wait for loopback ports to link up */
    sal_sleep(1);

    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: txSobmhPacket
 *
 * Send an SOBMH packet on specified port.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   port: Egress port for packet
 *   pkt: Pointer to packet to send
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
txSobmhPacket(int unit, bcm_port_t port, bcm_pktio_pkt_t * pkt)
{
    const int           priority = 0;
    const int           cos = 0;

    bcm_pktio_txpmd_t   pmd;
    bcm_error_t         rv;

    pmd.tx_port = port; /* TX (egress) port. */
    pmd.prio_int = priority;    /* Internal priority of the packet. */
    pmd.cos = cos;      /* The local COS queue to use. */
    pmd.flags = 0;      /* BCM_PKTIO_TX_xxx flags. */

    /*
     * Configure TX SOBMH PMD in the outgoing packet.
     */
    if (BCM_FAILURE(rv = bcm_pktio_pmd_set(unit, pkt, &pmd))) {
        /* Free the packet */
        bcm_pktio_free(unit, pkt);
        return rv;
    }

    /* Send packet and return */
    return bcm_pktio_tx(unit, pkt);
}

/*******************************************************************************
 * Utility Function: htonl()
 *
 * Convert "long" value to network order. CINT does not define the standard
 * "htonl" (host to network long) function. Ordinary "C" programs can use
 * the version defined in the networking library.
 *
 * Parameters:
 *   i: Value to convert
 *
 * Returns:
 *   Returns value converted to network order
 */
uint32
htonl(uint32 i)
{
    i = (i << 16) | (i >> 16);

    return (i & 0xff00ffff) >> 8 | (i & 0xffff00ff) << 8;
}

/*******************************************************************************
 * Function: createTxPacket()
 *
 * Allocate and initialize a simple L2/L3 packet. Uses the HSDK PKTIO
 * interface.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   pkt: Pointer to receive new packet
 *   vlan_tag: L2 header VLAN tag for new packet
 *   mac_dest: Destination MAC address for new packetfor packet
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
createTxPacket(int unit, bcm_pktio_pkt_t ** pkt, uint32 vlan_tag, bcm_mac_t mac_dest)
{
    /* Packet payload (includes L3/ICMP header). L2 header added later. */
    const uint8_t       pkt_data[] = {

/*
  00    01    02    03    04    05    06    07    08    09    10    11    12    13    14    15*/
0x08, 0x00, 0x45, 0x00, 0x00, 0x64, 0x00, 0x01, 0x00, 0x00, 0x3D, 0x01, 0x99, 0x94, 0x01, 0x01,
0x01, 0x01, 0xE1, 0x01, 0x01, 0x01, 0x08, 0x00, 0x40, 0x58, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x54,
0x65, 0x73, 0x74, 0x2D, 0x50, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x3A, 0x20, 0x49, 0x43, 0x4D, 0x50,
0x2A, 0x54, 0x65, 0x73, 0x74, 0x2D, 0x50, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x3A, 0x20, 0x49, 0x43,
0x4D, 0x50, 0x2A, 0x54, 0x65, 0x73, 0x74, 0x2D, 0x50, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x3A, 0x20,
0x49, 0x43, 0x4D, 0x50, 0x2A, 0x54, 0x65, 0x73, 0x74, 0x2D, 0x50, 0x61, 0x63, 0x6B, 0x65, 0x74,
0x3A, 0x20, 0x49, 0x43, 0x4D, 0x50
    };

    /* Compute size of packet payload (in bytes) */
    const int           DATA_BYTES = sizeof(pkt_data) / sizeof(pkt_data[0]);

    /* L2 header constants */
    const int           VLAN_TAG_BYTES = 4;
    const int           MAC_BYTES = 6;
    const int           L2_HEADER_BYTES = (MAC_BYTES * 2) + VLAN_TAG_BYTES;

    const int           TXPMD_BYTES = 32;       /* Maximum size of TX packet metadata header */
    const int           HIGIG3_BYTES = 16;      /* HiGig3 base plus extension 0 */

    /* Worse case size of header, includes potential additional header shims. */
    const int           HDR_BYTES = TXPMD_BYTES + HIGIG3_BYTES + L2_HEADER_BYTES;
    const int           PKT_BYTES = HDR_BYTES + DATA_BYTES;

    /* L2 info */
    const bcm_mac_t     mac_src = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

    uint32              temp;
    bcm_pktio_pkt_t    *new_pkt;
    uint8              *data;
    bcm_error_t         rv;

    /*
     * Allocate a packet structure and packet data.  Initially outgoing data
     * size is zero.
     */
    if (BCM_FAILURE(rv = bcm_pktio_alloc(unit, PKT_BYTES, 0, &new_pkt))) {
        return rv;
    }

    /* Reserve room at beginning of buffer for L2 header and PMD. */
    if (BCM_FAILURE(rv = bcm_pktio_reserve(unit, new_pkt, HDR_BYTES,
		    (auto) &data))) {
        /* Free the packet */
        bcm_pktio_free(unit, new_pkt);
        return rv;
    }

    /* Carve out space for payload and get pointer to data segment */
    if (BCM_FAILURE(rv = bcm_pktio_put(unit, new_pkt, DATA_BYTES,
		    (auto) &data))) {
        /* Free the packet */
        bcm_pktio_free(unit, new_pkt);
        return rv;
    }

    /* Copy payload into packet */
    sal_memcpy(data, pkt_data, DATA_BYTES);

    /* Carve out space at the beginning of the packet buffer for L2 header. */
    if (BCM_FAILURE(rv = bcm_pktio_push(unit, new_pkt, L2_HEADER_BYTES,
		    (auto) &data))) {
        /* Free the packet */
        bcm_pktio_free(unit, new_pkt);
        return rv;
    }

    /* Configure L2 header */
    sal_memcpy(data, mac_dest, MAC_BYTES);      /* Destination MAC */
    sal_memcpy(data + MAC_BYTES, mac_src, MAC_BYTES);   /* Source MAC */
    temp = htonl(vlan_tag);     /* TPID + VLAN tag (in network order) */
    sal_memcpy(data + (MAC_BYTES * 2), &temp, VLAN_TAG_BYTES);

    /* Return pointer to new base packet. */
    *pkt = new_pkt;
    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: sendPacket
 *
 * Create and send a single SOBMH type packet on specified port.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   port: Destintaion port for packet.
 *   vlan: VLAN for L2 header in outgoing packet.
 *   mac_dest: Destination MAC address for L2 header in outgoing packet.
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
sendPacket(int unit, bcm_port_t port, bcm_vlan_t vlan, bcm_mac_t mac_dest)
{
    uint32              vlan_tag;

    bcm_pktio_pkt_t    *pkt;
    bcm_error_t         rv;

    /* "DOT1P" packet priority in VLAN tag is 7 */
    vlan_tag = (0x8100 << 16) | (7 << 13) | vlan;

    /* Allocate and initialize packet */
    rv = createTxPacket(unit, &pkt, vlan_tag, mac_dest);
    if (BCM_SUCCESS(rv)) {
        /* Send packet on specific port */
        rv = txSobmhPacket(unit, port, pkt);
        /* Done with packet, free resources */
        bcm_pktio_free(unit, pkt);
    }

    return rv;
}

/*******************************************************************************
 * Function: cosqDebugCounterSourcePortTest
 *
 * Verify source port count functionality of COSQ Debug Stat Counter
 *
 * There are eight Store and Forward (SAF) and eight Cut Through (CT) debug
 * counters for each pipe group. They are programmed similarly. This test
 * will use the SAF counters.
 *
 * To demonstrate source port SAF packet counting, this test configures source
 * port counters for each test port. Because the device is limited to 8 SAF
 * counters per pipe group, the test can only test 8 ports at a time. The
 * test completes multiple test passes until all test ports are tested.
 *
 * The actual configuration and use of the COSQ debug stat counters is fairly
 * simple and straight forward:
 *
 * Step 1:
 *  Configure an SAF debug counter as follows:
 *
 *    const bcm_cosq_stat_t stat = bcmCosqStatDebugSaf0Packets;
 *    const bcm_port_t      port = 1;
 *    bcm_cosq_debug_stat_match_t match;
 *
 *    bcm_cosq_debug_stat_match_t_init(&match);
 *    match.match_flags =
 *        BCM_COSQ_DEBUG_STAT_MATCH_SRC_PORT;
 *    match.src_port = port;
 *    bcm_cosq_debug_stat_match_set (unit, port, stat, &match);
 *
 * The "port" parameter is only used to determine the pipe group to program,
 * it is not used for matching.  Traffic that ingresses on pipes 0, 1, 2, 3,
 * 12, 13, 14 and 15 goes to pipe group 0.  Traffic that ingresses on pipes
 * 4, 5, 6, 7, 8, 9, 10 and 11 goes to pipe group 1.  Use the "show pmap"
 * command to see which pipe each front panel port is mapped to.
 *
 * Step 2:
 *  Get a COSQ debug count value as follows:
 *
 *    const bcm_cosq_stat_t stat = bcmCosqStatDebugSaf0Packets;
 *    const bcm_port_t      port = 1;
 *    uint64                counter_value;
 *
 *    bcm_cosq_stat_sync_get(unit, port, -1, stat, &counter_value);
 *
 *  A couple things to note:
 *    1) The counters are not copied from the hardware as part of the
 *    normal counter collection process. To get the current counters,
 *    it is necessary to use the "sync" version of the API.
 *    2) The "cosq" parameter passed to the API must be -1.
 *    3) The port parameter is only used to determine which pipe group
 *       to access.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   port_list: Array containing test port information
 *   num_ports: Number of ports in the port_list
 *   mac_dest: Destination MAC address for transmitted packets
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
cosqDebugCounterSourcePortTest(int unit, port_info_t * port_list, int num_ports,
                               bcm_mac_t mac_dest, int *count)
{
    const char         *__func__ = "cosqDebugCounterSourcePortTest";
    bcm_error_t         rv;
    int                 start_port;
    int                 test_count = 8;
    int                 test_failed = 0;
    int                 test_id;
    uint64              reset_value;

    /* Used to reset counter to 0 */
    COMPILER_64_ZERO(reset_value);

    printf("\n%s: Begin ...\n", __func__);
    for (start_port = 0; start_port < num_ports; start_port += test_count) {
        test_count = ((num_ports - start_port) > 8) ? 8 : (num_ports - start_port);
        /*
         * There are two pipe groups. There are 8 debug counters per pipe
         * group. The pipe group is selected based on the port parameter.
         */
        for (test_id = 0; test_id < test_count; test_id++) {
            const bcm_cosq_stat_t stat = bcmCosqStatDebugSaf0Packets + test_id;
            const port_info_t  *port = &port_list[start_port + test_id];
            const port_info_t  *egr_port = &port_list[start_port + test_id + 1];

            bcm_cosq_debug_stat_match_t match;

	    /* Clear current COSQ debug counters */
            if (BCM_FAILURE
                (rv = bcm_cosq_stat_set(unit, port->port, -1, stat, reset_value))) {
                printf("%s bcm_cosq_stat_set() FAILED: %s\n", __func__, bcm_errmsg(rv));
                return rv;
            }

            /* Program slot for COSQ debug counters based on this port */
            bcm_cosq_debug_stat_match_t_init(&match);
            match.match_flags = BCM_COSQ_DEBUG_STAT_MATCH_SRC_PORT;
            match.src_port = port->port;
            if (BCM_FAILURE(rv = bcm_cosq_debug_stat_match_set
                            (unit, port->port, stat, &match))) {
                printf("%s bcm_cosq_debug_stat_match_set() FAILED: %s\n", __func__,
                       bcm_errmsg(rv));
                return rv;
            }
        }
        /* Send packets into switch */
	*count = 0;
        for (test_id = 0; test_id < test_count; test_id++) {
            port_info_t        *port = &port_list[start_port + test_id];
            if (BCM_FAILURE(rv = sendPacket(unit, port->port, port->vlan, mac_dest))) {
                printf("%s sendPacket() FAILED: %s\n", __func__, bcm_errmsg(rv));
                return rv;
            }
            /* Issues with callbacks in CINT require a short sleep. This is not needed
             * for regular applications.
             */
            sal_sleep(1);
        }

        for (test_id = 0; test_id < test_count; test_id++) {
            const bcm_cosq_stat_t stat = bcmCosqStatDebugSaf0Packets + test_id;
            const port_info_t  *egr_port = &port_list[start_port + test_id + 1];
            const port_info_t  *port = &port_list[start_port + test_id];

            int                 fail;
            int                 queue;
            uint32              counter_low;
            uint64              counter_value;

            /*
             * Need to use the "sync" version of the stat get function for
             * COSQ debug counters. The "cos" parameters must be -1.
             */
            if (BCM_FAILURE
                (rv =
                 bcm_cosq_stat_sync_get(unit, port->port, -1, stat, &counter_value))) {
                printf("%s bcm_cosq_stat_sync_get() FAILED: %s\n", __func__,
                       bcm_errmsg(rv));
                return rv;
            }
            counter_low = COMPILER_64_LO(counter_value);
            fail = counter_low != (test_id + 1);
            printf("%5s: Counter[%d]: packets=%u [%s]\n", port->name, test_id,
                   counter_low, fail ? "FAIL" : "PASS");
            test_failed = test_failed || fail;

            /* Reset counter to 0 */
            if (BCM_FAILURE
                (rv = bcm_cosq_stat_set(unit, port->port, -1, stat, reset_value))) {
                printf("%s bcm_cosq_stat_set() FAILED: %s\n", __func__, bcm_errmsg(rv));
                return rv;
            }
        }
	if (*count != test_count) {
	    printf("Unexpected RX Count: %d; expected: %d\n", *count, test_count);
	    test_failed = 1;
	}
    }

    printf("%s: %s\n", __func__, test_failed ? "FAIL" : "PASS");
    return test_failed ? BCM_E_FAIL : BCM_E_NONE;
}

/*******************************************************************************
 * Function: cosqDebugCounterDestinationPortQueueTest
 *
 * Verify destination port and queue count functionality of COSQ Debug
 * Stat Counter
 *
 * There are eight Store and Forward (SAF) and eight Cut Through (CT) debug
 * counters per pipe group. They are programmed similarly. This test will
 * use the SAF counters.
 *
 * To demonstrate destination port and queue SAF packet counting, this test
 * configures destination ports and queues counters for some number of test
 * ports. Because the device is limited to 8 SAF counters and there are 8
 * unicast queues per port, the test can only test one port at a time. The
 * test completes multiple test passes until all test ports are tested.
 *
 * The actual configuration and use of the COSQ debug stat counters is fairly
 * simple and straight forward:
 *
 * Step 1:
 *  Configure an SAF debug counter as follows:
 *
 *    const bcm_cosq_stat_t stat = bcmCosqStatDebugSaf0Packets;
 *    const bcm_port_t      port = 1;
 *    bcm_cosq_debug_stat_match_t match;
 *
 *    bcm_cosq_debug_stat_match_t_init(&match);
 *    match.match_flags =
 *        BCM_COSQ_DEBUG_STAT_MATCH_DST_PORT | BCM_COSQ_DEBUG_STAT_MATCH_QUEUE;
 *    match.dst_port = dest_port;
 *    match.cosq = test_id;
 *    bcm_cosq_debug_stat_match_set (unit, src_port, stat, &match);
 *
 * The "port" parameter is only used to determine the pipe group to program,
 * it is not used for matching.  The user should specify any SOURCE port from
 * the desired ingress pipe group. Traffic that ingresses on pipes 0, 1, 2, 3,
 * 12, 13, 14 and 15 goes to pipe group 0.  Traffic that ingresses on pipes
 * 4, 5, 6, 7, 8, 9, 10 and 11 goes to pipe group 1.  Use the "show pmap"
 * command to see which pipe each front panel port is mapped to.
 *
 * Step 2:
 *  Get a COSQ debug counter as follows:
 *
 *    const bcm_cosq_stat_t stat = bcmCosqStatDebugSaf0Packets;
 *    const bcm_port_t      port = 1;
 *    uint64                counter_value;
 *
 *    bcm_cosq_stat_sync_get(unit, port, -1, stat, &counter_value);
 *
 *  A couple things to note:
 *    1) The counters are not copied from the hardware as part of the
 *    normal counter collection process. To get the current counters,
 *    it is necessary to use the "sync" version of the API.
 *    2) The "cosq" parameter passed to the API must be -1.
 *    3) The port parameter is only used to determine which hardware to access.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   port_list: Array containing test port information
 *   num_ports: Number of ports in port_list
 *   mac_dest: Destination MAC address for transmitted packets
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
cosqDebugCounterDestinationPortQueueTest(int unit, port_info_t * port_list,
                                         int num_ports, bcm_mac_t mac_dest,
					 int *count)
{
    const char         *__func__ = "cosqDebugCounterDestinationPortQueueTest";
    const int           first_idx = 2;
    const int           max_pri = 8;
    const int           test_count = num_ports > 12 ? 12 : num_ports;

    bcm_error_t         rv;
    int                 port_idx;
    int                 test_failed = 0;
    int                 test_id;
    int                 tx_count;
    uint64              reset_value;

    printf("\n%s: Begin ...\n", __func__);

    /* Reset counter to 0 */
    COMPILER_64_ZERO(reset_value);
    for (port_idx = first_idx; port_idx < test_count; port_idx++) {
        port_info_t        *port = &port_list[port_idx];
        port_info_t        *partner = &port_list[port_idx - 1];

        for (test_id = 0; test_id < max_pri; test_id++) {
            const bcm_cosq_stat_t stat = bcmCosqStatDebugSaf0Packets + test_id;

            bcm_cosq_debug_stat_match_t match;

            if (BCM_FAILURE
                (rv = bcm_cosq_stat_set(unit, partner->port, -1, stat, reset_value))) {
                printf("%s bcm_cosq_stat_set() FAILED: %s\n", __func__, bcm_errmsg(rv));
                return rv;
            }

            /* Program slot for COSQ debug counters based on this port */
            bcm_cosq_debug_stat_match_t_init(&match);
            match.match_flags =
              BCM_COSQ_DEBUG_STAT_MATCH_DST_PORT | BCM_COSQ_DEBUG_STAT_MATCH_QUEUE;
            match.dst_port = port->port;
            match.cosq = test_id;
            /*
             * The "port" parameter is actually the source port for this
             * packet.  This is necessary to select the correct ingress
             * pipe group.
             */
            if (BCM_FAILURE(rv = bcm_cosq_debug_stat_match_set
                            (unit, partner->port, stat, &match))) {
                printf("%s bcm_cosq_debug_stat_match_set() FAILED: %s\n", __func__,
                       bcm_errmsg(rv));
                return rv;
            }
        }

        *count = 0;
        tx_count = (port_idx % 4) + 1;
        for (test_id = 0; test_id < tx_count; test_id++) {
            /*
             * Send packets to the first port in the snake and let them
             * propagate down the snake to the CPU port.
             */
            if (BCM_FAILURE
                (rv =
                 sendPacket(unit, port_list[0].port, port_list[0].vlan, mac_dest))) {
                printf("%s sendPacket() FAILED: %s\n", __func__, bcm_errmsg(rv));
                return rv;
            }
            /*
             * Issues with callbacks in CINT require a short sleep. This is
             * not needed for regular applications.
             */
            sal_sleep(1);
        }

        for (test_id = 0; test_id < max_pri; test_id++) {
            const bcm_cosq_stat_t stat = bcmCosqStatDebugSaf0Packets + test_id;

            uint64              counter_value;
            uint32              counter_low;
            int                 fail;

            /*
             * The "port" parameter is actually the source port for this
             * packet.  This is necessary to select the correct ingress
             * pipe group. Need to use the "sync" version of the API.
             */
            if (BCM_FAILURE
                (rv =
                 bcm_cosq_stat_sync_get(unit, partner->port, -1, stat,
                                        &counter_value))) {
                printf("%s bcm_cosq_stat_sync_get() FAILED: %s\n", __func__,
                       bcm_errmsg(rv));
                return rv;
            }
            counter_low = COMPILER_64_LO(counter_value);
            fail = ((counter_low == tx_count) && (test_id != partner->dot1p_pri)) ||
              ((counter_low == 0) && (test_id == partner->dot1p_pri));
            printf("%5s: COSQ Counter[%d]: packets=%u [%s]\n", port->name, test_id,
                   counter_low, fail ? "FAIL" : "PASS");
            test_failed = test_failed || fail;

            if (BCM_FAILURE
                (rv = bcm_cosq_stat_set(unit, partner->port, -1, stat, reset_value))) {
                printf("%s bcm_cosq_stat_set() FAILED: %s\n", __func__, bcm_errmsg(rv));
                return rv;
            }
        }
        if (*count != tx_count) {
            printf("Unexpected RX Count: %d; expected: %d\n", *count, tx_count);
            test_failed = 1;
        }
    }

    printf("%s: %s\n", __func__, test_failed ? "FAIL" : "PASS");
    return test_failed ? BCM_E_FAIL : BCM_E_NONE;
}

/*******************************************************************************
 * Function: verify
 *
 * Run all COSQ debug stat counter tests. See individual test descriptions
 * for details.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   port_list: Array containing test port information
 *   num_ports: Number of ports in port_list
 *   mac_dest: Destination MAC address for transmitted packets
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
verify(int unit, port_info_t * port_list, int num_ports, bcm_mac_t mac_dest,
       int * count)
{
    bcm_error_t         rv;

    printf("\nVerify ...\n");
    if (BCM_FAILURE(rv = cosqDebugCounterSourcePortTest(unit, port_list, num_ports,
                                                        mac_dest, count))) {
        return rv;
    }
    if (BCM_FAILURE
        (rv =
         cosqDebugCounterDestinationPortQueueTest(unit, port_list, num_ports, mac_dest,
                                                  count))) {
        return rv;
    }
    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: execute
 *
 * Main test entry point.
 *
 * This functions does the following
 *   a) Test setup (done in test_setup())
 *   b) Demonstrate the functionality of cosq debug stat counters (done
 *      in verify()).
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
execute()
{
    const int           unit = 0;
    const int           num_ports = 22; /* Should be at least 8 */
    const bcm_mac_t     mac_dest = { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 };

    bcm_error_t         rv;
    int                 count = 0;      /* RX packet count */
    port_info_t         port_list[num_ports];

    /* Display device and version info */
    bshell(unit, "attach ; version");

    if (BCM_FAILURE
        ((rv =
          test_setup(unit, port_list, num_ports, mac_dest, &count)))) {
        printf("test_setup() failed [%s]\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_FAILURE(rv = verify(unit, port_list, num_ports, mac_dest, &count))) {
        printf("verify() failed [%s]\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

const char         *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print               execute();
}

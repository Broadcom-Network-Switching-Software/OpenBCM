/*************************************************************************
 *  Feature  : GPORT Egress Bandwidth
 *
 *  Usage    : BCM.0> cint th4_hsdk_gport_bandwidth.c
 *
 *  config   : bcm56990_a0-lossy-64x400.config.yml
 *
 *  Log file : th4_hsdk_gport_bandwidth_log.txt
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
 *    This test validates the functionality of the 56990 device egress
 *    port bandwidth limiters (shapers).  The egress bandwidth limits are
 *    configured using the the API bcm_cosq_gport_bandwidth_set().  The test
 *    computes approximate egress rates by collecting egress byte counts over
 *    time using bcm_stat_sync_get(snmpIfOutOctets). The PKTIO API is used to
 *    send and receive test packets. The test uses an MMU configuration that
 *    is set to do egress accounting so that packets are dropped at egress.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup; Done in test_setup()
 *    =============================================
 *      a) Create a list of test ports, populate array of port_info_t with
 *         this information.
 *      b) Configure each port in port list, put port in loopback with a default
 *         untagged vlan.
 *      c) Configure a VLAN for each port in port list
 *
 *      Two ports will be set up to feedback on each other to generate
 *      high bandwidth traffic.  The remaining ports will be configured
 *      as a "snake", with egress traffic from one port being sent to the
 *      ingress of the next port. Ingress traffic on the last port in the
 *      snake is dropped
 *
 *    2) Step2 - Configuration; Done in configure()
 *    =======================================================
 *    Bandwidth limits will be set on all ports not being used to generate
 *    traffic.  The programmed bandwidth limits decrease from one port in the
 *    "snake" to the next.
 *
 *    Step3 - Verification (Done in verify())
 *    =======================================
 *      3.1) Transmit one packet into a feedback port to start a packet storm:
 *      3.2) Let traffic run for some period of time.
 *      3.3) Read byte counts for each "snake port" and compute traffic rates.
 *      3.4) Compare measured traffic rates against programmed shaper rates. If
 *           measured egress rates differ from the expected rates by an
 *           acceptable error, the test passes.
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

    /* Default VLAN assigned to untagged packets that ingress on this port */
    bcm_vlan_t          vlan;
};

/*************************************************************************
 * Function: portConfigGet
 *
 * Iterate through ports defined on this device to collect a list of test
 * ports. Assign a port name and VLAN for each port.
 *
 * Parameters:
 *   unit: Unit number for device under test
 *   port_list: Array for test port information
 *   num_ports: Number of ports to collect for port_list
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
     * The expectation is that the device configuration defines sufficient
     * "cd" ports to run the test.
     */
    BCM_PBMP_ITER(config.cd, port) {
        port_info_t        *port_info = &port_list[port_count];
        if (port_count == num_ports) {
            /* Number of requested ports satisfied, port list complete */
            break;
        }
        /* Save logical port number */
        port_info->port = port;
        /*
         * Construct a port name, should match the ones assigned by the
         * diag shell.
         */
        sprintf(port_info->name, "cd%d", port_count);
        /* Test VLANs start at first_test_VLAN specified above. */
        port_info->vlan = first_test_VLAN + port_count;
        port_count++;
    }
    if (port_count != num_ports) {
        /* If requested number of ports could not be satisfied, return error */
        printf("Number of requested ports could not be satisfied\n");
        return BCM_E_PORT;
    }
    printf("Test port/vlan configuration:\n");
    printf("  %5s %4s %4s\n", "Name", "Port", "VLAN");
    for (idx = 0; idx < num_ports; idx++) {
        port_info_t        *port_info = &port_list[idx];

        printf("%5s %4d %4d\n", port_info->name, port_info->port, port_info->vlan);
    }
    return BCM_E_NONE;
}

/*************************************************************************
 * Function: configureVlan
 *
 * Create a VLAN and add two port to it.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   vlan: VLAN to configure
 *   port: Test port
 *   partner: The "partner" to the test port which shares the same VLAN. If
 *            set to -1, don't add partner.
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
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);

    /* Two ports per VLAN */
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_PBMP_PORT_ADD(ubmp, port);
    if (partner >= 0) {
        BCM_PBMP_PORT_ADD(pbmp, partner);
        BCM_PBMP_PORT_ADD(ubmp, partner);
    }

    /* Add ports to VLAN */
    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_port_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*************************************************************************
 * Function: configurePort
 *
 * Configure a single test port.
 *
 * a) Put port into PHY loopback
 * b) Assign a default VLAN for untagged packets ingressing on port
 *
 * Parameters:
 *   unit: Unit number for device under test
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
    return BCM_E_NONE;
}

/*
 * Function: test_setup
 *
 * Main test setup routine. All test ports are chained together in a front
 * panel "snake" configuration using VLANs. When packets are injected into
 * the traffic generating ports, they will propagate down the "snake" until
 * they reach the last port where they will be dropped.
 *
 * a) Create a list of test ports
 * b) Configure each port in port list
 * c) Configure a VLAN for each port in port list
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   port_list: List of test ports
 *   num_ports: Number of ports in port list
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success

 */
bcm_error_t
test_setup(int unit, port_info_t * port_list, int num_ports)
{
    int                 i;
    bcm_error_t         rv;

    BCM_IF_ERROR_RETURN(portConfigGet(unit, port_list, num_ports));
    for (i = 0; i < num_ports; i++) {
        const bcm_port_t    port = port_list[i].port;
        const bcm_port_t    partner =
          (i < (num_ports - 1)) ? port_list[i + 1].port : -1;

        BCM_IF_ERROR_RETURN(configurePort(unit, &port_list[i]));
        BCM_IF_ERROR_RETURN(configureVlan(unit, port_list[i].vlan, port, partner));
    }
    /* Cross link first port pair to create packet storm */
    BCM_IF_ERROR_RETURN(configureVlan(unit, port_list[1].vlan, port_list[0].port, -1));
    /* Wait for loopback ports to link up */
    sal_sleep(1);

    return BCM_E_NONE;
}

/*************************************************************************
 * Function: configure()
 *
 * Main shaper configuration rountine. Iterate through the test port list
 * and configure a shaper on each port. For this test, the maximum bandwidth
 * limit on a port is set to half the limit of the previous port in the
 * snake. All queues on each port are set to the same bandwidth limit. It
 * may not be possible to configure the bandwith to the exact value specified
 * due to the inherent grandularity of the shaper hardware. In those cases,
 * the bandwidth actual setting may be slightly higher.
 *
 * a) Configure shapers for each port in port list.
 * b) Verify shapers for each port in port list.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   port_list: List of test ports
 *   num_ports: Number of ports in port list
 *   first_kbits_sec: Bandwidth limit for first test port.
 *   actual_kbits_sec: Actual bandwidth values.
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */

bcm_error_t
configure(int unit, port_info_t * shaper_port_list, int num_shaper_ports,
          uint32 first_kbits_sec, uint32 * actual_kbits_sec)
{
    const int           max_error = 6;  /* 0.06% max error */
    const int           verbose = 0;
    const int           max_cosq = 12;  /* 8 UC + 4 MC */
    const uint32        flags = 0;
    const uint32        kbits_sec_min = 0;      /* No minimum bandwidth guarantee */

    bcm_cos_queue_t     cosq;
    bcm_error_t         rv;
    int                 idx;
    uint32              kbits_sec_max = first_kbits_sec;
    uint32              out_flags;
    uint32              out_kbits_min;
    int                 error;

    for (idx = 0; idx < num_shaper_ports; idx++) {
        for (cosq = 0; cosq < max_cosq; cosq++) {
            /* Configure the same shaper value for each queue */
            if (BCM_FAILURE
                ((rv =
                  bcm_cosq_gport_bandwidth_set(unit, shaper_port_list[idx].port, cosq,
                                               kbits_sec_min, kbits_sec_max, flags)))) {
                printf("Error in bcm_cosq_gport_bandwidth_set(): %s\n", bcm_errmsg(rv));
                return rv;
            }
            if (verbose) {
                printf
                  ("Port %5s bandwidth set: cosq=%2d mode=0x%02X kbits_min=%2u kbits_max=%6u\n",
                   shaper_port_list[idx].name, cosq, flags, kbits_sec_min,
                   kbits_sec_max);
            }
        }

        for (cosq = 0; cosq < max_cosq; cosq++) {
            int                 shaper_delta;
            /* Retrieve the shaper */
            if (BCM_FAILURE
                ((rv =
                  bcm_cosq_gport_bandwidth_get(unit, shaper_port_list[idx].port, cosq,
                                               &out_kbits_min, &actual_kbits_sec[idx],
                                               &out_flags)))) {
                printf("Error in bcm_cosq_gport_bandwidth_get(): %s\n", bcm_errmsg(rv));
                return rv;
            }
            if (verbose) {
                printf
                  ("Port %5s bandwidth get: cosq=%2d mode=0x%02X kbits_min=%2u kbits_max=%6u\n",
                   shaper_port_list[idx].name, cosq, out_flags, out_kbits_min,
                   actual_kbits_sec[idx]);
            }
            shaper_delta = actual_kbits_sec[idx] - kbits_sec_max;
            if (shaper_delta < 0) {
                /* Actual should be equal to or greater than requests */
                printf("Error: Actual bandwidth (%d) less that requested (%d)\n",
                       actual_kbits_sec[idx], kbits_sec_max);
                return BCM_E_INTERNAL;
            }
            error = (shaper_delta * 10000) / kbits_sec_max;     /* LSB = 0.01% */
            if (error > max_error) {
                printf
                  ("Error: Actual bandwidth (%d) delta from requested (%d) exceeds limit\n",
                   actual_kbits_sec[idx], kbits_sec_max);
                return BCM_E_INTERNAL;
            }
        }

        kbits_sec_max = kbits_sec_max / 2;
    }
    return BCM_E_NONE;
}

/*************************************************************************
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
        bcm_pktio_free(thread_control->unit, txPkt);
        return rv;
    }

    /* Send packet and return */
    return bcm_pktio_tx(unit, pkt);
}

/*************************************************************************
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

/*************************************************************************
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
    if (BCM_FAILURE(rv = bcm_pktio_put(unit, new_pkt, DATA_BYTES, (auto) &data))) {
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

/*************************************************************************
 * Function: sendPacket
 *
 * Create and send a single SOBMH type packet on specified port.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   port: Destintaion port for packet.
 *   vlan: VLAN for L2 header in outgoing packet.
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
sendPacket(int unit, bcm_port_t port, bcm_vlan_t vlan)
{
    const bcm_mac_t     mac_dest = { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 };
    uint32              vlan_tag;

    bcm_pktio_pkt_t    *pkt;
    bcm_error_t         rv;

    /* "DOT1P" packet priority in VLAN tag is 0 */
    vlan_tag = (0x8100 << 16) | (0 << 13) | vlan;

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

/*************************************************************************
 * Function: getByteCounts
 *
 * Get RX and TX byte counts for all test ports in port list. Count
 * values are reset to 0 after being fetched.
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   shaper_port_list: Array containing test port information
 *   num_shaper_ports: Number of ports in port_list
 *   out_byte_list: Returned byte counts
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
getByteCounts(int unit, port_info_t * shaper_port_list, int num_shaper_ports,
              uint32 * out_byte_list)
{
    int                 cos;
    int                 idx;

    for (idx = 0; idx < num_shaper_ports; idx++) {
        const bcm_port_t    port = shaper_port_list[idx].port;
        uint64              out_bytes;

        /* Get SNMP port stats */
        BCM_IF_ERROR_RETURN(bcm_stat_sync_get(unit, port, snmpIfOutOctets, &out_bytes));
        out_byte_list[idx] = COMPILER_64_LO(out_bytes);

        /* Reset stats after displaying */
        BCM_IF_ERROR_RETURN(bcm_stat_clear_single(unit, port, snmpIfOutOctets));
    }
    return BCM_E_NONE;
}

/*************************************************************************
 * Function: verify
 *
 * Verify shaper behavior using actual traffic.
 *
 * 1) Start traffic
 * 2) Get byte counts after some time
 * 3) Compute measured bit rates for each port
 * 4) Compare measured bit rates against expected rates
 *
 * Parameters:
 *   unit: Unit number for device under test.
 *   port_list: Array containing test port information
 *   num_ports: Number of ports in port_list
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
verify(int unit, bcm_port_t tx_port, bcm_vlan_t tx_vlan, port_info_t * shaper_port_list,
       int num_shaper_ports, uint32 * expected_kbits_sec)
{
    const int           interval = 7;   /* In seconds */

    uint32              out_byte_list[num_shaper_ports];
    int                 idx;
    int                 test_fail = 0;

    printf("Start traffic on storm ports\n");
    BCM_IF_ERROR_RETURN(sendPacket(unit, tx_port, tx_vlan));

    /* Let counters accumulate */
    sal_sleep(interval);

    BCM_IF_ERROR_RETURN(getByteCounts
                        (unit, shaper_port_list, num_shaper_ports, out_byte_list));

    /* Compute bit rates (LSB = kbits/sec */
    for (idx = 0; idx < num_shaper_ports; idx++) {
        /* Convert bytes per sec to kbits per sec */
        out_byte_list[idx] = (out_byte_list[idx] * 8) / (1000 * interval);
    }

    /* Compare measured vs expected rates */
    printf("Check shapers:\n");
    for (idx = 0; idx < num_shaper_ports; idx++) {
        const int           max_error = 50;     /* Allow some amount of measurement error */
        int                 delta;
        int                 error;
        int                 fail;

        delta = out_byte_list[idx] - expected_kbits_sec[idx];
        if (delta < 0) {
            delta = -delta;
        }
        error = (delta * 10000) / expected_kbits_sec[idx];      /* LSB = 0.01% */
        fail = error > max_error;
        printf
          ("Port %s; Measured rate: %6u kbps; Expected rate: %6u kbps; Error: %4d kbps (%d/%d); %s\n",
           shaper_port_list[idx].name, out_byte_list[idx], expected_kbits_sec[idx],
           delta, error, max_error, fail ? "FAIL" : "PASS");
        test_fail += fail;
    }
    return test_fail ? BCM_E_FAIL : BCM_E_NONE;
}

/*************************************************************************
 * Function: execute
 *
 * Main test entry point.
 *
 * This functions does the following
 *   a) Test setup, done in test_setup()
 *   b) Shaper configuration, done in configure()
 *   c) Verify measure egress port bit rates against programmed shaper values.
 *
 * Returns:
 *   Returns propagated SDK status, BCM_E_NONE on success
 */
bcm_error_t
execute()
{
    const int           unit = 0;
    const int           num_storm_ports = 2;
    const int           num_shaper_ports = 4;
    const int           num_ports = num_storm_ports + num_shaper_ports; /* Minimum of 4 */
    const uint32        initial_kbits_sec = 500000;     /* Maximum shaper value */

    port_info_t         port_list[num_ports];
    uint32              actual_kbits_sec[num_shaper_ports];
    bcm_error_t         rv;

    /* Display device and version info */
    bshell(unit, "attach ; version");

    if (BCM_FAILURE((rv = test_setup(unit, port_list, num_ports)))) {
        printf("test_setup() failed [%s]\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_FAILURE
        (rv =
         configure(unit, &port_list[num_storm_ports], num_shaper_ports,
                   initial_kbits_sec, actual_kbits_sec))) {
        printf("configure() failed [%s]\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_FAILURE
        (rv =
         verify(unit, port_list[0].port, port_list[0].vlan, &port_list[num_storm_ports],
                num_shaper_ports, actual_kbits_sec))) {
        printf("verify() failed [%s]\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

const char         *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print               execute();
}

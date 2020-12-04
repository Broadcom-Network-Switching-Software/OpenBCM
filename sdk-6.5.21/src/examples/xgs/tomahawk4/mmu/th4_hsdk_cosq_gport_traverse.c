
/*  Feature  : COSQ gport
 *
 *  Usage    : BCM.0> cint th4_hsdk_cosq_gport_traverse.c
 *
 *  config   : bcm56990_a0-generic-64x400.config.yml
 *
 *  Log file : th4_hsdk_cosq_gport_traverse_log.txt
 *
 *  Test Topology : None
 *
 *  API tested:
 *     int bcm_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
 *       void *user_data);
 *     Traverse all GPORTs, both front panel and CPU.
 *
 *     int bcm_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *physical_port,
 *       int *num_cos_levels, uint32 *flags);
 *     Get information for a specific GPORT.
 *
 *  Summary:
 *  ========
 *  This cint example demonstrates the retrieval of gports of the system
 *  for both front panel and CPU ports.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      Check device type
 *      Initialize test variables
 *
 *    2) Step2 - Configuration (Null)
 *    ======================================================
 *      Do nothing in configuration step.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Traverse all Gports, save them in a data structure.
 *      b) Expected Result:
 *         ================
 *         Perform the following checks on the saved Gports:
 *         b.1) Verify port count.
 *         b.2) Verify MC/UC queue gports
 *         b.3) Verify scheduler gports
 */

cint_reset();

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

/*
 * Define constants for this test.
 */

/* CPU is always port 0 */
const int           CPU_PORT = 0;

/* Maximuim number of ports supported by TH4 */
const int           MAX_PORTS = 256;

/* Maximum number of multicast queues can be 6, 4 or 0 depending on configuration */
const int           MAX_MC_QUEUES = 6;

/* Maximum number of unicast queues can be 12, 8 or 6 depending on configuration */
const int           MAX_UC_QUEUES = 12;

/* CPU always supports 48 queues */
const int           MAX_CPU_QUEUES = 48;

/* Each port supports 12 scheduler nodes */
const int           MAX_SCH_NODES = 12;

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

/*
 * Define data structures for this test.
 */

/* Structure used to store all gports */
typedef struct all_gports_t {
    bcm_port_t          port_list[MAX_PORTS];
    bcm_gport_t         cpu_sched_gport[MAX_SCH_NODES];
    bcm_gport_t         cpu_mcast_q_gport[MAX_CPU_QUEUES];
    bcm_gport_t         sched_gport[MAX_PORTS][MAX_SCH_NODES];
    bcm_gport_t         mcast_q_gport[MAX_PORTS][MAX_MC_QUEUES];
    bcm_gport_t         ucast_q_gport[MAX_PORTS][MAX_UC_QUEUES];
    int                 prev_port;
    int                 port_count;
    int                 mcast_count;
    int                 sched_count;
    int                 ucast_count;
};

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

/*
 * Define functions for this test.
 */

/*
 * Function: cosq_gport_traverse_callback
 *
 * Must be of type bcm_cosq_gport_traverse_cb:
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
 *   user_data: A generic, user defined pointer.In this case points to test
 *     data structure.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
cosq_gport_traverse_callback(int unit, bcm_gport_t port, int numq, uint32 flags,
                             bcm_gport_t gport, void *user_data)
{
    /* Get pointer to test data structure */
    const all_gports_t *all_gports = (auto) user_data;

    /* Vars for bcm_cosq_gport_get */
    int                 num_cos_levels;
    int                 logical_port;
    uint32              port_flags;

    /* Index into port lists */
    int                 port_idx;

    /* Get information associated with this GPORT */
    BCM_IF_ERROR_RETURN(bcm_cosq_gport_get
                        (unit, gport, &logical_port, &num_cos_levels, &port_flags));

    /* When port changes, reset bookkeeping counters */
    if (logical_port != all_gports->prev_port) {
        /* Reset counts for each base port */
        all_gports->prev_port = logical_port;
        all_gports->port_list[all_gports->port_count++] = logical_port;
        all_gports->mcast_count = 0;
        all_gports->sched_count = 0;
        all_gports->ucast_count = 0;
    }
    /* Get index into port lists */
    port_idx = all_gports->port_count - 1;

    /* Sort gport into the appropriate list based on port type */
    switch (flags) {

          /* Unicast queue gport */
      case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
          if (all_gports->ucast_count >= MAX_UC_QUEUES) {
              return BCM_E_INTERNAL;
          }
          all_gports->ucast_q_gport[port_idx][all_gports->ucast_count++] = gport;
          break;

          /* Multicast queue gport */
      case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
          if (logical_port == CPU_PORT) {
              if (all_gports->mcast_count >= MAX_CPU_QUEUES) {
                  return BCM_E_INTERNAL;
              }
              all_gports->cpu_mcast_q_gport[all_gports->mcast_count++] = gport;
          } else {
              if (all_gports->mcast_count >= MAX_MC_QUEUES) {
                  return BCM_E_INTERNAL;
              }
              all_gports->mcast_q_gport[port_idx][all_gports->mcast_count++] = gport;
          }
          break;

          /* Scheduler gport */
      case BCM_COSQ_GPORT_SCHEDULER:
          if (all_gports->sched_count > MAX_SCH_NODES) {
              return BCM_E_INTERNAL;
          }
          if (logical_port == CPU_PORT) {
              all_gports->cpu_sched_gport[all_gports->sched_count++] = gport;
          } else {
              all_gports->sched_gport[port_idx][all_gports->sched_count++] = gport;
          }
          break;
      default:
          /* Should be no other ports types for this TH4 configuration */
          return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function: cosq_gport_traverse
 *
 * Wrapper function for bcm_cosq_gport_traverse()
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   cb: Function pointer to the traverse callback funciton.
 *   user_data: A generic, user defined pointer. In this case points to
 *     test data structure.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb, void *user_data)
{
    bcm_error_t         ret_val;

    if (BCM_FAILURE(ret_val = bcm_cosq_gport_traverse(unit, cb, user_data))) {
        printf("bcm_cosq_gport_traverse FAILED:\n", bcm_errmsg(ret_val));
        return ret_val;
    }

    return 1;
}

/*
 * Function: cosq_gport_traverse
 *
 * Compare gports collected from traverse with expected gports for this
 * port configuration.
 *
 * COSQ Queue Gports are constructed from:
 * 1) GPORT type
 * 2) System Port
 * 3) Queue
 *
 * COSQ Scheduler Gports are constructed from:
 * 1) GPORT type
 * 2) Scheduler Index
 * 3) System Port
 *
 * This routine constructs expected gports for each queue or scheduler node
 * and compares them against the values collected by the traverse function.
 * The shift and mask constants used to construct these Gports are not exposed
 * to CINT so they are redefined here.
 *
 * Parameters:
 *    all_gports: Pointer to structure containing all gports collected during
 *      traverse operation.
 *
 * Returns:
 * BCM_E_NONE if gport lists match expected values, BCM_E_PORT otherwise.
 * for failure.
 */
bcm_error_t
check_gports(all_gports_t * all_gports)
{
    /*
     * Verify that the GPORT configuration matches the gport_configuration
     * for "bcm56990_a0-generic-64x400.config.yml". 
     *
     * Total logical ports: 65 (64 front panel ports + CPU )
     * Number of front panel unicast queues: 8 (the default, could be 6, 8 or 12)
     * Number of front panel multicast queues: 6 (the default, could be 0, 4 or 6)
     */
    const int           expected_port_count = 65;
    const int           expected_mc_queues = 4;
    const int           expected_uc_queues = 8;

    /* Constants from $SDK/include/shared/gport.h */
    const int           GPORT_TYPE_SHIFT = 26;
    const int           GPORT_TYPE_MASK = 0x3f;
    const int           GPORT_SYSPORTID_SHIFT = 14;
    const int           GPORT_SYSPORTID_MASK = 0xFFF;
    const int           GPORT_TYPE_MCAST_QUEUE_GROUP = 12;
    const int           GPORT_TYPE_UCAST_QUEUE_GROUP = 9;
    const int           GPORT_TYPE_SCHEDULER = 13;
    const int           GPORT_SCHEDULER_INDEX_SHIFT = 16;       /* Made up, hard coded in source */
    const int           GPORT_SCHEDULER_NODE_SHIFT = 0;
    const int           GPORT_SCHEDULER_NODE_MASK = 0xfffff;
    const int           GPORT_SCHEDULER_CORE_SHIFT = 23;
    const int           GPORT_SCHEDULER_CORE_MASK = 0x7;

    int                 port_idx;

    /* Step 1: Verify port count */
    if (all_gports->port_count != expected_port_count) {
        printf("Wrong number of ports: %d; expected %d\n", all_gports->port_count,
               expected_port_count);
        return BCM_E_PORT;
    }
    /* Step 2: Verify MC/UC queue gports */
    for (port_idx = 0; port_idx < all_gports->port_count; port_idx++) {
        const bcm_port_t    logical_port = all_gports->port_list[port_idx];
        int                 queue_idx;

        /* Check front panel ports */
        if (logical_port != CPU_PORT) {
            /* Check front panel multicast queues */
            for (queue_idx = 0; queue_idx < MAX_MC_QUEUES; queue_idx++) {
                if (queue_idx >= expected_mc_queues) {
                    if (all_gports->mcast_q_gport[port_idx][queue_idx] != 0) {
                        printf("Extra multicast queue %d on port %d\n", queue_idx,
                               logical_port);
                        return BCM_E_PORT;
                    }
                } else {
                    const bcm_gport_t   expected =
                      (GPORT_TYPE_MCAST_QUEUE_GROUP << GPORT_TYPE_SHIFT) |
                      (logical_port << GPORT_SYSPORTID_SHIFT) | queue_idx;

                    if (expected != all_gports->mcast_q_gport[port_idx][queue_idx]) {

                        printf
                          ("MCast GPORT Error: Port: 0x%3X; Queue: 0x%X; Actual: 0x%08X; Expected: 0x%08X\n",
                           logical_port, queue_idx,
                           all_gports->mcast_q_gport[port_idx][queue_idx], expected);
                        return BCM_E_PORT;
                    }
                }
            }
            /* Check front panel unicast queues */
            for (queue_idx = 0; queue_idx < MAX_UC_QUEUES; queue_idx++) {
                if (queue_idx >= expected_uc_queues) {
                    if (all_gports->ucast_q_gport[port_idx][queue_idx] != 0) {
                        printf("Extra unicast queue %d on port %d\n", queue_idx,
                               logical_port);
                        return BCM_E_PORT;
                    }
                } else {
                    const bcm_gport_t   expected =
                      (GPORT_TYPE_UCAST_QUEUE_GROUP << GPORT_TYPE_SHIFT) |
                      (logical_port << GPORT_SYSPORTID_SHIFT)
                      | queue_idx;
                    if (expected != all_gports->ucast_q_gport[port_idx][queue_idx]) {
                        printf
                          ("UCast GPORT Error: Port: 0x%3X; Queue: 0x%X; Actual: %08X; Expected:  %08X\n",
                           logical_port, queue_idx,
                           all_gports->ucast_q_gport[port_idx][queue_idx], expected);
                        return BCM_E_PORT;
                    }
                }
            }
        } else {
            /* Check CPU port mulitcast queues */
            for (queue_idx = 0; queue_idx < MAX_CPU_QUEUES; queue_idx++) {
                const bcm_gport_t   expected =
                  (GPORT_TYPE_MCAST_QUEUE_GROUP << GPORT_TYPE_SHIFT) |
                  (CPU_PORT << GPORT_SYSPORTID_SHIFT) | queue_idx;
                if (all_gports->cpu_mcast_q_gport[queue_idx] != expected) {
                    printf("CPU GPORT ERROR: Queue: %d; Actual: %08X; Expected: %08X\n",
                           queue_idx, all_gports->cpu_mcast_q_gport[queue_idx],
                           expected);
                    return BCM_E_PORT;
                }
            }
        }
    }
    /* Step 3: Verify scheduler gports */
    /*
     * It appears that some macros for encoded/decoding scheduler nodes are
     * missing from the main code base.  The code here uses local constancts
     * for this.
     */
    for (port_idx = 0; port_idx < all_gports->port_count; port_idx++) {
        const bcm_port_t    logical_port = all_gports->port_list[port_idx];
        int                 sched_idx;

        if (logical_port == CPU_PORT) {
            for (sched_idx = 0; sched_idx < MAX_SCH_NODES; sched_idx++) {
                const bcm_gport_t   expected =
                  (GPORT_TYPE_SCHEDULER << GPORT_TYPE_SHIFT) |
                  (sched_idx << GPORT_SCHEDULER_INDEX_SHIFT) |
                  (7 << GPORT_SCHEDULER_CORE_SHIFT) | logical_port;
                if (all_gports->cpu_sched_gport[sched_idx] != expected) {
                    printf
                      ("CPU Scheduler Error; Port %3d; Index: %d; Actual: %08X; Expected: %08X\n",
                       logical_port, sched_idx,
                       all_gports->cpu_sched_gport[sched_idx], expected);
                }
            }
        } else {
            for (sched_idx = 0; sched_idx < MAX_SCH_NODES; sched_idx++) {
                const bcm_gport_t   expected =
                  (GPORT_TYPE_SCHEDULER << GPORT_TYPE_SHIFT) |
                  (sched_idx << GPORT_SCHEDULER_INDEX_SHIFT) |
                  (7 << GPORT_SCHEDULER_CORE_SHIFT) | logical_port;
                if (all_gports->sched_gport[port_idx][sched_idx] != expected) {
                    printf
                      ("Port Scheduler Error; Port %3d; Index: %d; Actual: %08X; Expected: %08X\n",
                       logical_port, sched_idx,
                       all_gports->sched_gport[port_idx][sched_idx], expected);
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
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
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
get_device_type(int unit, uint32 * dev_type)
{
    bcm_info_t          info;

    BCM_IF_ERROR_RETURN(bcm_info_get(unit, &info));
    /*
     * The info struct takes the vendor, device and revision from the PCI
     * config space.
     */

    *dev_type = info.device & 0x0000FFFF;

    return BCM_E_NONE;
}

/*
 * Function: test_setup
 *
 * Perform assorted set up operations for this test, including initialization
 * and device type checks.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   all_gports: Pointer to structure containing all gports collected during
 *      traverse operation.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
test_setup(int unit, all_gports_t * all_gports)
{
    uint32              dev_type;

    /* Initialize test structure */
    sal_memset(all_gports, 0, sizeof(*all_gports));
    all_gports->prev_port = -1;

    /* Test test only supported for 5699X (Tomahawk4), reject others */
    BCM_IF_ERROR_RETURN(get_device_type(unit, &dev_type));
    if (dev_type != 0xB990) {
        printf("Unsupported device type/configuration: 0x%4X\n", dev_type);
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function: verify
 *
 * Primary test function. Using the traverse callback, collect all cosq
 * gports. Verify collected cosq gports.
 *
 * Parameters:
 *   unit: Unit number for device under test (DUT)
 *   all_gports: Pointer to structure containing all gports collected during
 *      traverse operation.
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
verify(int unit, all_gports_t * all_gports)
{
    bcm_error_t         rv;

    /* Traverse the all the ports and print them */
    if (BCM_FAILURE
        ((rv = cosq_gport_traverse(unit, cosq_gport_traverse_callback, all_gports)))) {
        printf("cosq_gport_traverse() failed.\n");
        return rv;
    }
    if (BCM_FAILURE(rv = check_gports(all_gports))) {
        printf("GPORT configuration mismatch\n");
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * Function: execute
 *
 * Top level test function. Entry point for this test, calls all other
 * functions.
 *
 * Parameters:
 *   None
 *
 * Returns:
 * BCM_E_NONE on success, otherwise a bcm_error_t enum specifying reason
 * for failure.
 */
bcm_error_t
execute()
{
    const int           unit = 0;
    bcm_error_t         rv;
    all_gports_t        all_gports;

    /* Minimal initial setup needed */
    if (BCM_FAILURE((rv = test_setup(unit, &all_gports)))) {
        printf("test_setup() failed.\n");
        return rv;
    }

    /* No configuration needed for this case */

    /* Verify the result */
    rv = verify(unit, &all_gports);
    printf("Gport Traverse Test: %s\n", BCM_FAILURE(rv) ? "FAIL" : "PASS");

    return rv;
}

const char         *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print               execute();
}

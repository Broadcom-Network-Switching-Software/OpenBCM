/*******************************************************************************
 * Feature  : Flexport
 *
 *  Usage    : BCM.0> th4_hsdk_basic_flexport.c
 *
 *  config   : bcm56990_a0-generic-192x100_16x400.config.yml
 *
 *  Log file : th4_hsdk_basic_flexport_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              | test port groups
 *                   |                              +-----------------+
 *                   |                              |
 *                   |                              +-----------------+
 *                   |          SWITCH              |
 *                   |                              +-----------------+
 *                   |                              |
 *                   |                              +-----------------+
 *                   |                              |
 *                   +------------------------------+
 *  Summary:
 *  ========
 *    This CINT script demonstrates how to flex between one 400G port and four
 *    100G ports from an application using bcm APIs. This test validates the
 *    port flex operation by counting the number of ports of each speed and
 *    comparing this to expected values. No traffic is generated in this test.
 *
 *    Detailed steps done in the CINT script:
 *    =====================================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select a set of 100G ports for flex test using the function
 *         portInventoryGet(). Each group of 100G ports must belong to the
 *         same port macro to permit them to be flexed to a single 400G
 *         port. The algorithm used here assumes that the port configuration
 *         defines the 100G ports such that they are grouped this way. The
 *         configuration file used for this test defines the ports this
 *         way. When the user does their own port flex operation, they must
 *         ensure that the ports they select are from the same port macro. If
 *         the selected port list does not belong to the same port macro
 *         the flex operation will fail.
 *
 *    3) Step2 - Verification(Done in verify())
 *    ==========================================
 *      a) Display port information before flex operation (verbose mode only).
 *      b) Count initial 100G and 400G ports
 *      c) Iterate through 100G port set, for each port:
 *       c.1) Disable linkscan on 100G ports.
 *       c.2) Disable 100G ports.
 *       c.3) Flex four consecutive 100G ports to one 400G port.
 *       c.4) Enable linkscan on the new 400G flexed port.
 *       c.5) Enable new 400G flexed port.
 *      d) Count the number of 100G and 400G ports after first flex operation
 *      e) Iterate through new 400G ports, for each port:
 *       e.1) Disable linkscan on 400G port.
 *       e.2) Disable 400G port.
 *       e.3) Flex one 400G port to four 100G ports.
 *       e.4) Enable linkscan on the new 100G flexed ports.
 *       e.5) Enable new 100G flexed ports.
 *      f) Count 100G and 400G ports after second flex operation
 *      g) Display port information after flex operation (verbose mode only).
 *
 *      The verify routine will check to ensure that the port counts collected
 *      during the test match expected values. If counts are correct, the
 *      test will display "PASS", if not it will display "FAIL".
 */

cint_reset();

/*
 * Define a structure for storing port information used for the flex port
 * test. This information comes from bcm_port_resource_get().
 */
typedef struct port_inventory_t {
    int                 logical_port;
    int                 physical_port;
    int                 speed;
};

/* For this test, we'll flex four groups of ports */
const int           num_ports = 4;
port_inventory_t    port_list[num_ports];

/*******************************************************************************
 * Function: portInventoryGet
 *
 * Collect 100G ports for flex operations. Because it takes four 100G ports
 * to make a 400G port, the ports needs to be collected in groups of four.
 * The assumtion is that the port list is defined to ensure that the port
 * groups share the same port module and can successfully be combined to form
 * a single 400G port.  The first port from the port list is stored in the
 * global "port_list" data structure.
 *
 * Parameters:
 *   unit: Device under test.
*/
bcm_error_t
portInventoryGet(int unit)
{
    int                 port_count;
    bcm_port_t          port;
    bcm_port_t          next_port;
    bcm_error_t         rv;
    bcm_port_config_t   config;

    if (BCM_FAILURE(rv = bcm_port_config_get(unit, &config))) {
        printf("Error retrieving port configuration: %s\n", bcm_errmsg(rv));
        return rv;
    }

    next_port = 0;
    /* Interate through all Ethernet front panel ports */
    BCM_PBMP_ITER(config.e, port) {
        bcm_port_resource_t resource;
        int                 idx;

        if (port_count == num_ports) {
            /* We have enough ports */
            break;
        }
        if (port < next_port) {
            /* Skip ports sharing the same port module. */
            continue;
        }

        for (idx = 0; idx < 4; idx++) {
            /* Find 4 consecutive 100G ports, assume they share a port module.
             * This will be the case for the specified configuration file.
             */
            BCM_IF_ERROR_RETURN(bcm_port_resource_get(unit, port + idx, &resource));
            if (resource.speed != 100000) {
                break;
            }
        }

        if (idx == 4) {
            /* We found four consecutive 100G ports */
            next_port = port + 4;
            /* Save port information for flex port test */
            BCM_IF_ERROR_RETURN(bcm_port_resource_get(unit, port, &resource));
            port_list[port_count].logical_port = resource.port;
            port_list[port_count].physical_port = resource.physical_port;
            port_list[port_count].speed = resource.speed;
            port_count++;
        }
    }

    if (port_count != num_ports) {
        printf("portInventoryGet() failed; Not enough ports\n");
        return BCM_E_PORT;
    }

    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: portCountsGet
 *
 * Get the current count of 100G and 400G ports. These counts will change
 * as a result of port flex operations.
 *
 * Parameters:
 *   unit: Device under test.
 *   num100G: Gets the current 100G port count
 *   num400G: Gets the current 400G port count
*/
bcm_error_t
portCountsGet(int unit, int *num100G, int *num400G)
{
    bcm_port_t          port;
    bcm_error_t         rv;
    bcm_port_config_t   config;

    if (BCM_FAILURE(rv = bcm_port_config_get(unit, &config))) {
        printf("Error retrieving port configuration: %s\n", bcm_errmsg(rv));
        return rv;
    }
    *num100G = 0;
    *num400G = 0;

    BCM_PBMP_ITER(config.e, port) {
        bcm_port_resource_t resource;

        BCM_IF_ERROR_RETURN(bcm_port_resource_get(unit, port, &resource));
        if (resource.speed == 100000) {
            (*num100G)++;
        } else if (resource.speed == 400000) {
            (*num400G)++;
        }
    }

    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: test_setup
 *
 * Perform all test setup operations for the port flex test.
 *
 * Parameters:
 *   unit: Device under test.
*/
bcm_error_t
test_setup(int unit)
{
    bcm_error_t         rv;

    /* Assemble a list of test ports for port flex operations. */
    if (BCM_FAILURE(rv = portInventoryGet(unit))) {
        printf("portInventoryGet() failed: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: flex_4x100g_to_1x400g
 *
 * Flex four 100G ports to a single 400G port. This function assumes that
 * the 100G ports are valid candidates for port flexing. If they are not,
 * the port flex operation will fail.
 *
 * Parameters:
 *   unit: Device under test.
 *   port: First logicial port number of four ports being flexed
 *   phy_port: Physical port number resultant flex port
 *   fec_type: FEC type for resultant flex port
*/
int
flex_4x100g_to_1x400g(int unit, bcm_gport_t port, int phy_port,
                      bcm_port_phy_fec_t fec_type)
{
    /*
     * For this device, each port module has eight 50G lanes for each 400G
     * port.
 */
    const int           lanes = 8;

    int                 idx;
    bcm_port_resource_t resource_list[5];       /* 4 ports in + 1 port out */

    printf("Flex 100G ports %d-%d to 400G port %d\n", port, port + 3, port);
    /* Remove original ports */
    for (idx = 0; idx < 4; idx++) {
        bcm_port_resource_t *res_ptr = &resource_list[idx];
        bcm_port_resource_t_init(res_ptr);
        res_ptr->port = port + idx;
        res_ptr->physical_port = -1;
    }

    /* Configure new port */
    bcm_port_resource_t_init(&resource_list[4]);
    resource_list[4].port = port;
    resource_list[4].physical_port = phy_port;
    resource_list[4].speed = 400000;    /* 400G */
    resource_list[4].lanes = lanes;
    resource_list[4].fec_type = fec_type;
    resource_list[4].link_training = 0;
    resource_list[4].phy_lane_config = -1;      /* default lane config */

    return bcm_port_resource_multi_set(unit, 5, resource_list);
}

/*******************************************************************************
 * Function: flex_1x400g_to_4x100g
 *
 * Flex a single 400G port to four 100G ports.
 *
 * Parameters:
 *   unit: Device under test.
 *   port: Logicial port number of port being flexed
 *   phy_port: First physical port number resultant flex ports
 *   fec_type: FEC type for resultant flex ports
*/
int
flex_1x400g_to_4x100g(int unit, bcm_gport_t port, int phy_port,
                      bcm_port_phy_fec_t fec_type)
{
    const int           lanes = 2;      /* 2 50G lanes for each 100G port */
    int                 idx;
    bcm_port_resource_t resource_list[5];       /* 1 port in + 4 ports out */

    printf("Flex 400G port %d to 100G ports %d-%d\n", port, port, port + 3);
    /* Remove original port */
    bcm_port_resource_t_init(&resource_list[0]);
    resource_list[0].port = port;
    resource_list[0].physical_port = -1;

    /* Configure new ports */
    for (idx = 0; idx < 4; idx++) {
        bcm_port_resource_t *res_ptr = &resource_list[idx + 1];
        bcm_port_resource_t_init(res_ptr);
        res_ptr->port = port + idx;
        res_ptr->physical_port = phy_port + idx;
        res_ptr->speed = 100000;        /* 100G */
        res_ptr->lanes = lanes;
        res_ptr->fec_type = fec_type;
        res_ptr->link_training = 0;
        res_ptr->phy_lane_config = -1;  /* default lane config */
    }

    return bcm_port_resource_multi_set(unit, 5, resource_list);
}

/*******************************************************************************
 * Function: flexport_stop
 *
 * Stop linkscan and disable one or more ports. Linkscan must be stopped
 * and ports must be disabled before performing a port flex operation.
 * When combining multiple ports, they must all be stopped before flexing.
 *
 * Parameters:
 *   unit: Device under test.
 *   logical_port: Logicial port number of first port stop/disable.
 *   num_ports: Number of consecutive ports to stop/disable.
*/
bcm_error_t
flexport_stop(int unit, bcm_port_t logical_port, int num_ports)
{
    int                 idx;
    bcm_error_t         rv;

    for (idx = 0; idx < num_ports; idx++) {
        /* remove port from linkscan */
        if (BCM_FAILURE(rv = bcm_port_linkscan_set(unit, logical_port + idx, 0))) {
            printf("Error in bcm_port_linkscan_set(): %s\n", bcm_errmsg(rv));
            return rv;
        }

        /* disable the port */
        if (BCM_FAILURE(rv = bcm_port_enable_set(unit, logical_port + idx, 0))) {
            printf("Error in bcm_port_enable_set(): %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: flexport_start
 *
 * Start linkscan and enable one or more ports. This is necessary after a
 * port flex operation to be able to use the port.
 *
 * Parameters:
 *   unit: Device under test.
 *   logical_port: Logicial port number of first port start/enable.
 *   num_ports: Number of consecutive ports to start/enable.
*/
bcm_error_t
flexport_start(int unit, bcm_port_t logical_port, int num_ports)
{
    int                 idx;
    bcm_error_t         rv;

    for (idx = 0; idx < num_ports; idx++) {
        /* Add flexed ports to linkscan */
        if (BCM_FAILURE(rv = bcm_port_linkscan_set(unit, logical_port + idx, 1))) {
            printf("Error in bcm_port_linkscan_set() post flex operation: %s\n",
                   bcm_errmsg(rv));
            return rv;
        }

        /* Enable flexed ports */
        if (BCM_FAILURE(rv = bcm_port_enable_set(unit, logical_port + idx, 1))) {
            printf("Error in bcm_port_enable_set() post flex operation: %s\n",
                   bcm_errmsg(rv));
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: verify
 *
 * Main test function, see description above.  This test assumes the device
 * is configured using bcm56990_a0-generic-192x100_16x400.config.yml.
 *
 * Parameters:
 *   unit: Device under test.
*/
bcm_error_t
verify(int unit)
{
    const int           expected_100G = 192;    /* Device initially has 192 100G ports */
    const int           expected_400G = 16;     /* Device initially has 16 400G ports */

    /* When enabled, print port status before and after flex operations */
    const int           verbose = 0;

    int                 test_failed;    /* Test status */
    bcm_error_t         rv;
    int                 idx;

    /* Define port counts, these are collected a various stages of the test. */
    int                 num100G_start;
    int                 num400G_start;
    int                 num100G_flex1;
    int                 num400G_flex1;
    int                 num100G_flex2;
    int                 num400G_flex2;

    /* Get initial port counts */
    BCM_IF_ERROR_RETURN(portCountsGet(unit, &num100G_start, &num400G_start));

    if (verbose) {
        bshell(unit, "ps cd0-cd5,ce0-ce15");
    }

    /* Phase one, flex 100G ports to 400G ports */
    for (idx = 0; idx < num_ports; idx++) {
        /* First stop the old 100G ports */
        if (BCM_FAILURE(rv = flexport_stop(unit, port_list[idx].logical_port, 4))) {
            printf("Error in flexport_stop: %s.\n", bcm_errmsg(rv));
            return rv;
        }
        /* Flex 4x100G port to 1x400G ports */
        if (BCM_FAILURE
            (rv =
             flex_4x100g_to_1x400g(unit, port_list[idx].logical_port,
                                   port_list[idx].physical_port,
                                   bcmPortPhyFecRs544_2xN))) {
            printf("Error in Port Flex operation: %s.\n", bcm_errmsg(rv));
            return rv;
        }
        /* Start the new 400G ports */
        if (BCM_FAILURE(rv = flexport_start(unit, port_list[idx].logical_port, 1))) {
            printf("Error in flexport_start: %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }
    /* Get port counts after first phase. */
    BCM_IF_ERROR_RETURN(portCountsGet(unit, &num100G_flex1, &num400G_flex1));
    if (verbose) {
        bshell(unit, "ps cd0-cd5,ce0-ce15");
    }

    /* Phase one, flex 400G ports to 100G ports */
    for (idx = 0; idx < num_ports; idx++) {
        /* First stop the old 400G ports */
        if (BCM_FAILURE(rv = flexport_stop(unit, port_list[idx].logical_port, 1))) {
            printf("Error in flexport_stop: %s.\n", bcm_errmsg(rv));
            return rv;
        }
        /* Flex 1x400G port to 4x100G ports */
        if (BCM_FAILURE
            (rv =
             flex_1x400g_to_4x100g(unit, port_list[idx].logical_port,
                                   port_list[idx].physical_port, bcmPortPhyFecRs544))) {
            printf("Error in Port Flex operation: %s.\n", bcm_errmsg(rv));
            return rv;
        }
        /* Start the new 100G ports */
        if (BCM_FAILURE(rv = flexport_start(unit, port_list[idx].logical_port, 4))) {
            printf("Error in flexport_start: %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }
    /* Get port counts after first phase. */
    BCM_IF_ERROR_RETURN(portCountsGet(unit, &num100G_flex2, &num400G_flex2));
    if (verbose) {
        bshell(unit, "ps cd0-cd5,ce0-ce15");
    }

    /* Check initial port counts */
    test_failed = (num100G_start != expected_100G) || (num400G_start != expected_400G);

    printf("Initial port counts; 100G=%d; 400G=%d; %s\n",
           num100G_start, num400G_start, test_failed ? "FAIL" : "PASS");

    if (test_failed) {
        return BCM_E_FAIL;
    }

    /* Check phase one port counts */
    test_failed = (num100G_flex1 != (expected_100G - (num_ports * 4)))
      || (num400G_flex1 != (expected_400G + num_ports));
    printf("After flexing %d 100G ports to %d 400G ports; 100G=%d; 400G=%d; %s\n",
           num_ports * 4, num_ports, num100G_flex1, num400G_flex1,
           test_failed ? "FAIL" : "PASS");

    if (test_failed) {
        return BCM_E_FAIL;
    }

    /* Check phase two port counts */
    test_failed = (num100G_flex2 != expected_100G) || (num400G_flex2 != expected_400G);
    printf("After flexing %d 400G ports to %d 100G ports; 100G=%d; 400G=%d; %s\n",
           num_ports, num_ports * 4, num100G_flex2, num400G_flex2,
           test_failed ? "FAIL" : "PASS");

    if (test_failed) {
        return BCM_E_FAIL;
    }

    printf("56990 Port Flex Test 1: PASS\n");
    return BCM_E_NONE;
}

/*******************************************************************************
 * Function: execute
 *
 * Main test entry point
 */
bcm_error_t
execute()
{
    const int           unit = 0;
    bcm_error_t         rv;

    /* Dump current configuration */
    bshell(unit, "attach ; version");

    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return rv;
    }

    if (BCM_FAILURE((rv = verify(unit)))) {
        printf("Flexport verify failed\n");
        return rv;
    }

    return BCM_E_NONE;
}

const char         *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    int                 status = execute();
    printf("TEST STATUS: %s\n", bcm_errmsg(status));
}

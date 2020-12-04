/*
 * Feature  : ADT
 *
 * Usage    : BCM.0> cint th4_hsdk_mmu_adt.c
 *
 * Config   : th4_hsdk_mmu_config.yml
 *
 * Log file : th4_hsdk_mmu_adt_log.txt
 *
 * Test Topology : None
 *
 * This script can verify below cosq APIs:
 * ========
 *    bcm_cosq_control_set
 *    bcm_cosq_control_get
 *
 * Summary:
 * ========
 * This cint example demonstrate the ADT configuration steps:
 *    - Configure the ADT group for a given queue
 *    - Configure ADT alpha index value used to compute margin value for shared pool
 *    - Configure margin levels to be used to indicate which limit low priority alpha to use
 *    - Configure egress pool whose low priority packets admitted using limit alpha value
 *    - Retrieve counter of low priority packets admitted using limit alpha value
 *    - Retrieve counter of high priority accepted cells for a given service pool
 *
 *  Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *      a) Select one port
 *
 *    ==========================================
 *    2) Step2 - Configuration (Done in test_verify()):
 *      a) Configure the ADT group for a given queue
 *      b) Configure ADT alpha index value used to compute margin value for shared pool
 *      c) Configure margin levels to be used to indicate which limit low priority alpha to use
 *      d) Configure egress pool whose low priority packets admitted using limit alpha value
 *
 *    ==========================================
 *    3) Step3 - Verification (Done in test_verify()):
 *     Expected Result:
 *      a) Retrieve counter of low priority packets admitted using limit alpha value
 *      b) Retrieve counter of high priority accepted cells for a given service pool
 *
 *    ==========================================
 *    4) Step4 - Cleanup (Done in test_cleanup())
 *      a) None
 */

cint_reset();

int unit = 0;
bcm_port_t port = 1;
bcm_cosq_control_t type;
int arg, out_arg = -1;
int CELL_SIZE = 254;

bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
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
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
      if (BCM_PBMP_MEMBER(&ports_pbmp, i) && (port < num_ports)) {
          port_list[port] = i;
          port++;
      }
    }

    if ((port == 0) || (port != num_ports)) {
         printf("portNumbersGet() failed \n");
         return -1;
    }

    return BCM_E_NONE;
}

/* Configure the ADT group of a queue. */
int
adt_group_config(int unit)
{
    int adt_group[] = {BCM_COSQ_ADT_PRI_GROUP_LOW,
                       BCM_COSQ_ADT_PRI_GROUP_MIDDLE,
                       BCM_COSQ_ADT_PRI_GROUP_HIGH};
    int adt_assign[] = {0, 0, 0, 1, 1, 2, 2, 2};
    int rv = BCM_E_NONE;
    int cosq;

    type = bcmCosqControlAdtPriGroup;

    for (cosq = 0; cosq < 8; cosq++) {
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, port, cosq, type, adt_group[adt_assign[cosq]]));
    }

    for (cosq = 0; cosq < 8; cosq++) {
        BCM_IF_ERROR_RETURN(bcm_cosq_control_get(unit, port, cosq, type, &out_arg));
        if (out_arg != adt_group[adt_assign[cosq]]) {
            printf("Error: ADT group mismatch\n");
            return BCM_E_FAIL;
        }
    }

    return rv;
}

/* Configure ADT alpha index value used to compute margin value for shared pool */
int
adt_high_alpha_config(int unit)
{
    int cosq;
    int rv = BCM_E_NONE;

    type = bcmCosqControlEgressPoolAdtHighPriAlpha;
    arg = bcmCosqControlDropLimitAlpha_1_16;

    for (cosq = 0; cosq < 8; cosq++) {
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, port, cosq, type, arg));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_get(unit, port, cosq, type, &out_arg));
        if (out_arg != arg) {
            printf("Error: adt_high_alpha_config mismatch\n");
            return BCM_E_FAIL;
        }
    }

    arg = bcmCosqControlDropLimitAlpha_8;

    for (cosq = 0; cosq < 8; cosq++) {
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, port, cosq, type, arg));
        BCM_IF_ERROR_RETURN(bcm_cosq_control_get(unit, port, cosq, type, &out_arg));
        if (out_arg != arg) {
            printf("Error: adt_high_alpha_config mismatch\n");
            return BCM_E_FAIL;
        }
    }

    return rv;
}

/* Configure margin levels to be used to indicate which limit low priority alpha to use */
int
adt_margins_config(int unit)
{
    int cosq;
    int m, delta;

    int margins[] = {
        bcmCosqControlEgressPoolAdtMargin0,
        bcmCosqControlEgressPoolAdtMargin1,
        bcmCosqControlEgressPoolAdtMargin2,
        bcmCosqControlEgressPoolAdtMargin3,
        bcmCosqControlEgressPoolAdtMargin4,
        bcmCosqControlEgressPoolAdtMargin5,
        bcmCosqControlEgressPoolAdtMargin6,
        bcmCosqControlEgressPoolAdtMargin7,
        bcmCosqControlEgressPoolAdtMargin8,
        bcmCosqControlEgressPoolAdtMargin9
    };
    int margin_vals[] = {
        100000,  /* bcmCosqControlEgressPoolAdtMargin0 - 400cell/100KB */
        200000,  /* bcmCosqControlEgressPoolAdtMargin1 */
        300000,  /* bcmCosqControlEgressPoolAdtMargin2 */
        400000,  /* bcmCosqControlEgressPoolAdtMargin3 */
        500000,  /* bcmCosqControlEgressPoolAdtMargin4 */
        600000,  /* bcmCosqControlEgressPoolAdtMargin5 */
        700000,  /* bcmCosqControlEgressPoolAdtMargin6 */
        800000,  /* bcmCosqControlEgressPoolAdtMargin7 */
        900000,  /* bcmCosqControlEgressPoolAdtMargin8 */
        1000000, /* bcmCosqControlEgressPoolAdtMargin9 */
    };

    for (cosq = 0; cosq < 8; cosq++) {
        for (m = 0; m < 10; m++) {
            type = margins[m];
            arg = margin_vals[m];
            BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, port, cosq, type, arg));
            BCM_IF_ERROR_RETURN(bcm_cosq_control_get(unit, port, cosq, type, &out_arg));
            delta = out_arg > arg ? out_arg - arg : arg - out_arg;
            if (delta > CELL_SIZE) {
                printf("Error: adt_margins_config mismatch cosq=%d, arg=%d, out_arg=%d\n",
                        cosq, arg, out_arg);
                return BCM_E_FAIL;
            }
        }
    }

    return BCM_E_NONE;
}

/* Configure the egress pool on which ADT margin levels are monitored */
int
adt_monitor_pool_config(int unit)
{
    /* Accepted parameters for port and cosq are -1 only */
    type = bcmCosqControlAdtLowPriMonitorPool;
    arg = 1;  /* Service pool number 1-4 */

    BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, -1, /* port */ -1, /*cosq*/ type, 1));
    BCM_IF_ERROR_RETURN(bcm_cosq_control_get(unit, -1, /* port */ -1, /*cosq*/ type, &out_arg));
    if (out_arg != arg) {
        printf("Error: adt_monitor_pool_config mismatch\n");
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/* Retrieve counter of low priority packets admitted using limit alpha value */
int
adt_low_prio_stat(int unit)
{
    bcm_cosq_stat_t stat;
    uint64 value;
    int cosq;
    int port;

    printf("-->Retrieve counter of low priority packets admitted using limit alpha value\n");

    port = -1;
    cosq = -1;
    stat = bcmCosqStatAdtAlpha0AdmittedLowPriPackets;

    /* Read the counter */
    COMPILER_64_SET(value, 0, 0);
    BCM_IF_ERROR_RETURN(bcm_cosq_stat_get(unit, port, cosq, stat, &value));
    print value;

    /* Set the counter */
    COMPILER_64_SET(value, 0, 200);
    print value;
    BCM_IF_ERROR_RETURN(bcm_cosq_stat_set(unit, port, cosq, stat, value));

    /* Read the counter */
    COMPILER_64_SET(value, -1, -1);
    BCM_IF_ERROR_RETURN(bcm_cosq_stat_get(unit, port, cosq, stat, &value));
    print value;

    return BCM_E_NONE;
}

/* Retrieve counter of high priority accepted bytes for a given service pool */
int
adt_high_prio_stat(int unit)
{
    bcm_cosq_stat_t stat;
    uint64 value;
    int cosq;

    printf("--> Retrieve counter of high priority accepted bytes for a given service pool\n");

    stat = bcmCosqStatEgressPoolAdtAdmittedHighPriBytes;
    cosq = 2;

    COMPILER_64_SET(value, 0, 100*254);
    print value;
    BCM_IF_ERROR_RETURN(bcm_cosq_stat_set(unit, port, cosq, stat, value));

    COMPILER_64_SET(value, -1, -1);
    print value;
    BCM_IF_ERROR_RETURN(bcm_cosq_stat_get(unit, port, cosq, stat, &value));
    print value;

    return BCM_E_NONE;
}

bcm_error_t test_setup(int unit)
{
    int port_list[2];
    int rv = BCM_E_NONE;

    rv = portNumbersGet(unit, port_list, 2);
    if (BCM_E_NONE != rv) {
        printf("portNumbersGet() failed\n");
        return rv;
    }

    port = port_list[0];

    return BCM_E_NONE;
}

/* Function to verify the result */
void test_verify(int unit)
{
    int rv = BCM_E_NONE;

    rv = adt_group_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("adt_group_config() failed: %s\n", bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    printf("ADT Group config successful\n");

    rv = adt_high_alpha_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("adt_high_alpha_config() failed: %s\n", bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    printf("ADT High Alpha config successful\n");

    rv = adt_margins_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("adt_margins_config() failed: %s\n", bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    printf("ADT Margins config successful\n");

    rv = adt_monitor_pool_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("adt_monitor_pool_config() failed: %s\n", bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    printf("ADT monitored service pool config successful\n");

    rv = adt_low_prio_stat(unit);
    if (BCM_FAILURE(rv)) {
        printf("adt_low_prio_stat() failed: %s\n", bcm_errmsg(rv));
        return BCM_E_FAIL;
    }

    rv = adt_high_prio_stat(unit);
    if (BCM_FAILURE(rv)) {
        printf("adt_high_prio_stat() failed: %s\n", bcm_errmsg(rv));
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

bcm_error_t test_cleanup(int unit)
{
    int rv = BCM_E_NONE;

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; a ; version");

    /* Assign WRED profile and create discard profile */
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    if (BCM_FAILURE((rv = test_verify(unit)))) {
        printf("test_verify() failed.\n");
        return -1;
    }

    /* Clean the created discard profile */
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


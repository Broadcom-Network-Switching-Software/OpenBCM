/*
 * Feature  : scheduling policy, and weights
 *
 * Usage    : BCM.0> cint th4_hsdk_mmu_sched_config_global.c
 *
 * Config   : th4_hsdk_mmu_config.yml
 *
 * Log file : th4_hsdk_mmu_sched_config_global_log.txt
 *
 * Test Topology : None
 *
 * This script can verify below cosq APIs:
 * ========
 * 	bcm_cosq_sched_config_set
 * 	bcm_cosq_sched_config_get
 * 	bcm_cosq_sched_weight_max_get
 *
 * Summary:
 * ========
 * This cint example demonstrate configuring scheduling policy and weights per device.
 * Also will verify the max weight retrieving API.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Do nothing in this test
 *
 *    2) Step2 - Configuration (Done in sched_weight_config())
 *    ======================================================
 *      a) Configure scheduling policy, and weights per device.
 *         weights for the 12 queues are:  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ==========================================
 *      a) Verify the configured scheduling mode and weight using 'dump sw cosq';
 *      b) Expected Result:
 *         ================
 *        - Retrieve the globally configured scheduling policy, and weights for
 *          all UC, MC, and Sch belongto all valid front panel ports via bcm_cosq_sched_config_get.
 *          Check the system max weight for WRR.
 *        - Compare the retrieved result with configured policy and weight
 */

cint_reset();

int unit=0;

int MAX_QUEUES = 12;

int config_weights[MAX_QUEUES] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
int config_mode;
int get_weights[MAX_QUEUES];
int get_mode;

int rv;

bcm_error_t
sched_weight_config(int unit)
{
    int cos, weight_arr_size= MAX_QUEUES;

    config_mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;

    /* Configure scheduling policy, and weights per device */
    if (BCM_FAILURE(bcm_cosq_sched_config_set(unit, config_mode, weight_arr_size, config_weights))) {
      printf("Error in sched_profile_setup() : %s.\n", bcm_errmsg(rv));
      return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t
sched_max_weight_get(int unit)
{
    int mode, weight_max;

    mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;

    /* Get max weight of WRR */
    if (BCM_FAILURE((rv = bcm_cosq_sched_weight_max_get(unit, mode, &weight_max)))) {
        printf("Error in sched_max_weight_get() : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("==============================================================\n"); 
    printf ("sched_max_weight_get: Max weight of mode %d is %d\n", mode, weight_max);

    return BCM_E_NONE;
}

bcm_error_t
sched_weight_get(int unit)
{
    int cos, weight_arr_size = MAX_QUEUES, weight_arr_count = 0;

    for(cos = 0; cos < MAX_QUEUES; cos++) {
        get_weights[cos] = 0;
    }

    /* Retrieve scheduling policy, and weights */
    if (BCM_FAILURE((rv = bcm_cosq_sched_config_get(unit, weight_arr_size,
                                        get_weights, &weight_arr_count, &get_mode)))) {
        printf("Error in sched_weight_get() : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("==============================================================\n"); 
    printf("sched_weight_get: schedule mode %d, retrieved weight_arr_count %d\n",
            get_mode, weight_arr_count);

    for(cos = 0; cos < weight_arr_count; cos++) {
        printf("weights[%d]: weight %d \n", cos, get_weights[cos]);
    }

    return BCM_E_NONE;
}

bcm_error_t
test_setup(int unit)
{
  return BCM_E_NONE;
}

/* Function to verify the result */
bcm_error_t
test_verify(int unit) 
{
    int cos;

    /* Verify the configured scheduling mode and weight */
    /* bshell(unit, "dump sw cosq"); */

    /* Verify max weight of WRR */
    if (BCM_FAILURE((rv = sched_max_weight_get(unit)))) {
        printf("sched_max_weight_get() failed.\n");
        return -1;
    }

    /* Verify the configured scheduling mode and weight */
    if (BCM_FAILURE((rv = sched_weight_get(unit)))) {
        printf("sched_weight_get() failed.\n");
        return -1;
    }

    /* Verify the configuration */
    if(get_mode != config_mode) {
        rerurn -1;
    }

    for(cos = 0; cos < MAX_QUEUES; cos++) {
        if(get_weights[cos] != config_weights[cos]) {
            rerurn -1;
        }
    }

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    bshell(unit, "config show; attach; version");

    /* Do nothing */
    if (BCM_FAILURE((rv = test_setup(unit)))) {
       printf("test_setup() failed.\n");
       return -1;
    }

    printf("==> Configure scheduling policy, and weights per device\n");

    /* Configure scheduling policy, and weights per device */
    if (BCM_FAILURE((rv = sched_weight_config(unit)))) {
        printf("sched_weight_config() failed.\n");
        return -1;
    }

    printf("==> Verification\n");

    /* verify the result */
    if (BCM_FAILURE((rv = test_verify(unit)))) {
        printf("test_verify() failed.\n");
        return -1;
    } else {
        printf("test_verify() pass.\n");
    }

    return BCM_E_NONE;   
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


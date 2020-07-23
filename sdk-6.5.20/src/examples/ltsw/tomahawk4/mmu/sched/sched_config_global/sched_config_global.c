/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : scheduling policy, and weights
 *
 *  Usage    : BCM.0> cint sched_config_global.c
 *
 *  config   : sched_config_global.bcm
 *
 *  Log file : sched_config_global_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below cosq APIs:
 *  ========
 * 	 bcm_cosq_sched_config_set
 * 	 bcm_cosq_sched_config_get
 * 	 bcm_cosq_sched_weight_max_get
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate configuring scheduling policy, and weights per device.
 *  Also will verify the max weight retrieving API.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Do nothing in this test
 *
 *    2) Step2 - Configuration (Done in sched_weight_config())
 *    ======================================================
 *      a) Configure scheduling policy, and weights per device.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify the configured scheduling mode and weight using 'dump sw cosq';
 *          On the other hand, retrieve and print the configured scheduling policy, and weights for
 *          all UC, MC, and Sch belongto all valid front panel ports via bcm_cosq_sched_config_get.
 *          Check the system max weight for WRR.
 *      b) Expected Result:
 *         ================
 *         In 'dump sw cosq' output and printing of sched_weight_get
 *             - Weight of sch node 0-11 for all valid ports were confiugured to 0-11
 *             - Max weight of WRR is 127
 */

cint_reset();

int MAX_QUEUES = 12;

bcm_port_t port;
bcm_cosq_mapping_t cosq_mapping_arr[MAX_QUEUES];
int profile_id=1;
int unit=0;
int rv;

bcm_error_t test_setup(int unit)
{
  return BCM_E_NONE;
}

bcm_error_t sched_weight_config(int unit)
{
    int cos, mode, weight_arr_size= MAX_QUEUES;
    int weights[MAX_QUEUES];

    mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;

    for(cos = 0; cos < MAX_QUEUES; cos++) {
      weights[cos] = cos;
    }

    /* Configure scheduling policy, and weights per device */
    if (BCM_FAILURE((rv = bcm_cosq_sched_config_set(unit, mode, weight_arr_size, weights)))) {
      printf("Error in sched_profile_setup() : %s.\n", bcm_errmsg(rv));
      return rv;
    }

  return BCM_E_NONE;
}

bcm_error_t sched_weight_get(int unit)
{
    int cos, mode, weight_arr_size = MAX_QUEUES, weight_arr_count = 0;
    int weights[MAX_QUEUES];

    mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;

    for(cos = 0; cos < MAX_QUEUES; cos++) {
      weights[cos] = cos;
    }

    /* Retrieve scheduling policy, and weights */
    if (BCM_FAILURE((rv = bcm_cosq_sched_config_get(unit, weight_arr_size,
                                        weights, &weight_arr_count, &mode)))) {
        printf("Error in sched_weight_get() : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("==============================================================\n"); 
    printf ("sched_weight_get: schedule mode %d, retrieved weight_arr_count %d\n",
        mode, weight_arr_count);

    for(cos = 0; cos < weight_arr_count; cos++) {
        printf("weights[%d]: weight %d \n", cos, weights[cos]);
    }

  return BCM_E_NONE;
}

bcm_error_t sched_max_weight_get(int unit)
{
    int mode, weight_max;

    mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;

    /* Get max weight of WRR */
    if (BCM_FAILURE((rv = bcm_cosq_sched_weight_max_get(unit, mode, &weight_max)))) {
        printf("Error in sched_max_weight_get() : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("==============================================================\n"); 
    printf ("sched_max_weight_get: Max weight of mode %d is %d\n",
        mode, weight_max);

  return BCM_E_NONE;
}


/* Function to verify the result */
void verify(int unit) 
{
  /*Verify the configured scheduling mode and weight */
  bshell(unit, "dump sw cosq");

  /*Verify the configured scheduling mode and weight */
  if (BCM_FAILURE((rv = sched_weight_get(unit)))) {
      printf("sched_weight_get() failed.\n");
      return -1;
  }

  /*Verify max weight of WRR */
  if (BCM_FAILURE((rv = sched_max_weight_get(unit)))) {
      printf("sched_max_weight_get() failed.\n");
      return -1;
  }

}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;
 
  bshell(unit, "config show; a ; version");

  /* Do nothing */
  if (BCM_FAILURE((rv = test_setup(unit)))) {
     printf("test_setup() failed.\n");
     return -1;
  }

  /* Configure scheduling policy, and weights per device */
  if (BCM_FAILURE((rv = sched_weight_config(unit)))) {
      printf("sched_weight_config() failed.\n");
      return -1;
  }

  /* verify the result */
  verify(unit);
  return BCM_E_NONE;   
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}



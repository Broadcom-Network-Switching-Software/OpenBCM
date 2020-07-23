/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : scheduling policy, and weights
 *
 *  Usage    : BCM.0> cint sched_config_portmap.c
 *
 *  config   : sched_config_portmap.bcm
 *
 *  Log file : sched_config_portmap_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below cosq APIs:
 *  ========
 * 	 bcm_cosq_port_sched_config_set
 * 	 bcm_cosq_port_sched_config_get
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate configuring scheduling policy, and weights of ports in
 *  specified portmap.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Retrieved all the front panel port bitmap
 *
 *    2) Step2 - Configuration (Done in sched_weight_config())
 *    ======================================================
 *      a) Configure scheduling policy, and weights of ports in specified portmap.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify the configured scheduling mode and weight using 'dump sw cosq';
 *          On the other hand, retrieve and print the configured scheduling policy, and weights
 *          of ports in specified portmap.
 *      b) Expected Result:
 *         ================
 *         In 'dump sw cosq' output and printing of sched_weight_get
 *             - Weight of sch node 0-11 for ports in specified portmap were confiugured to 0-11
 */

cint_reset();

int MAX_QUEUES = 12;

bcm_port_t port;
bcm_cosq_mapping_t cosq_mapping_arr[MAX_QUEUES];
bcm_pbmp_t ports_pbmp;
int profile_id=1;
int unit=0;
int rv;

/* Retrieved all the front panel port bitmap. */
bcm_error_t portBitmapGet(int unit)
{
    int i=0,port=0,rv=0;
    bcm_port_config_t configP;

    rv = bcm_port_config_get(unit,&configP);
    if(BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;

    return BCM_E_NONE;
}

bcm_error_t test_setup(int unit)
{
    /* Retrieved all the front panel port bitmap */
    if (BCM_E_NONE != portBitmapGet(unit)) {
        printf("portBitmapGet() failed\n");
        return -1;
    }

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

    /* Configure scheduling policy, and weights of ports in specified portmap. */
    if (BCM_FAILURE((rv = bcm_cosq_port_sched_config_set(unit, ports_pbmp,
                                        mode, weight_arr_size, weights)))) {
      printf("Error in sched_weight_config() : %s.\n", bcm_errmsg(rv));
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

    /* Retrieve scheduling policy, and weights of ports in specified portmap. */
    if (BCM_FAILURE((rv = bcm_cosq_port_sched_config_get(unit, ports_pbmp,
                                        weight_arr_size, weights,
                                        &weight_arr_count, &mode)))) {
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


/* Function to verify the result */
void verify(int unit) 
{

  /*Check scheduling policy, and weights for sch node on ports in specified portmap. */
  bshell(unit, "dump sw cosq");

  /*Retrieve scheduling policy, and weights of ports in specified portmap.*/
  if (BCM_FAILURE((rv = sched_weight_get(unit)))) {
      printf("sched_weight_get() failed.\n");
      return -1;
  }
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;
 
  bshell(unit, "config show; a ; version");

  /* select portmap */
  if (BCM_FAILURE((rv = test_setup(unit)))) {
     printf("test_setup() failed.\n");
     return -1;
  }

  /* Configure scheduling policy, and weights of ports in specified portmap.*/
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



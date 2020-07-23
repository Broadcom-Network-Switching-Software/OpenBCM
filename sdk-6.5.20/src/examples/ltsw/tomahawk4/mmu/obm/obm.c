/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : OBM
 *
 *  Usage    : BCM.0> cint obm.c
 *
 *  config   :
 *
 *  Log file : obm_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below cosq APIs:
 *  ========
 *   bcm_obm_port_max_usage_mode_get
 *   bcm_obm_port_max_usage_mode_set
 *   bcm_obm_port_pri_traffic_class_mapping_get
 *   bcm_obm_port_pri_traffic_class_mapping_set
 *   bcm_obm_traffic_class_pfc_priority_mapping_get
 *   bcm_obm_traffic_class_pfc_priority_mapping_set
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate configure and retrieve OBM properties
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one egress port to test
 *    2) Step2 - Configuration and check (Done in obm_test())
 *    ======================================================
 *      a) Configure and retrieve the OBM properties.
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      NULL
 */

cint_reset();

bcm_port_t port;
int unit=0;
int rv;

/* This function is written so that hardcoding of port
   numbers in Cint scripts is removed. This function gives
   required number of ports
*/

bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
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
    if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
      port_list[port]=i;
      port++;
    }
  }

  if (( port == 0 ) || ( port != num_ports )) {
       printf("portNumbersGet() failed \n");
       return -1;
  }

  return BCM_E_NONE;

}

bcm_error_t test_setup(int unit)
{
  int port_list[1], i;
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 1)) {
      printf("portNumbersGet() failed\n");
      return -1;
  }

  port = port_list[0];

  return BCM_E_NONE;
}

bcm_error_t obm_traffic_class_pfc_priority_mapping_test(int unit)
{
    int pri_list[5] = {1, 2, 3, 4, 5};
    int max_pri_count = 5;
    bcm_obm_traffic_class_t obm_tc = bcmObmClassLossless0;
    int pri_get_list[5];
    int pri_get_count;
    int i;

    if (BCM_FAILURE((rv = bcm_obm_traffic_class_pfc_priority_mapping_set(unit,
                                            port, obm_tc, max_pri_count, pri_list)))) {
      printf("Error in bcm_obm_traffic_class_pfc_priority_mapping_set() : %s.\n", bcm_errmsg(rv));
      return rv;
    }

    if (BCM_FAILURE((rv = bcm_obm_traffic_class_pfc_priority_mapping_get(unit,
                                            port, obm_tc, max_pri_count, pri_get_list, &pri_get_count)))) {
      printf("Error in bcm_obm_traffic_class_pfc_priority_mapping_get() : %s.\n", bcm_errmsg(rv));
      return rv;
    }

    for (i = 0; i < pri_get_count; i++) {
        if (pri_list[i] != pri_get_list[i]) {
            printf("ERR. obm_traffic_class_pfc_priority_mapping retrieved value is not same\n");
            printf("as set one for %d priority, set: %d, get: %d\n", i, pri_list[i],
                pri_get_list[i]);
            return BCM_E_FAIL;

        }
    }

    return BCM_E_NONE;
}

void print_obm_wm_mode(bcm_obm_max_watermark_mode_t obm_wm_mode)
{
    switch (obm_wm_mode) {
        case bcmObmMaxUsageModeAllTraffic:
            printf("bcmObmMaxUsageModeAllTraffic\n");
            break;
        case bcmObmMaxUsageModeLossy:
            printf("bcmObmMaxUsageModeLossy\n");
            break;
        case bcmObmMaxUsageModeLossless0:
            printf("bcmObmMaxUsageModeLossless0\n");
            break;
        case bcmObmMaxUsageModeLossless0:
            printf("bcmObmMaxUsageModeLossless1\n");
            break;
        default:
            return BCM_E_INTERNAL;
    }

}
bcm_error_t obm_port_max_usage_mode_test(int unit)
{
    bcm_obm_max_watermark_mode_t obm_wm_mode = bcmObmMaxUsageModeLossless0;
    bcm_obm_max_watermark_mode_t obm_wm_mode_get = -1;

    if (BCM_FAILURE((rv = bcm_obm_port_max_usage_mode_get(unit,
                                            port, &obm_wm_mode_get)))) {
      printf("Error in bcm_obm_port_max_usage_mode_get() : %s.\n", bcm_errmsg(rv));
      return rv;
    }

    printf("Default OBM port max usage mode is ");

    print_obm_wm_mode(obm_wm_mode_get);

    if (BCM_FAILURE((rv = bcm_obm_port_max_usage_mode_set(unit,
                                            port, obm_wm_mode)))) {
      printf("Error in bcm_obm_port_max_usage_mode_set() : %s.\n", bcm_errmsg(rv));
      return rv;
    }

    obm_wm_mode_get = -1;
    if (BCM_FAILURE((rv = bcm_obm_port_max_usage_mode_get(unit,
                                            port, &obm_wm_mode_get)))) {
      printf("Error in bcm_obm_port_max_usage_mode_get() : %s.\n", bcm_errmsg(rv));
      return rv;
    }

    if (obm_wm_mode != obm_wm_mode_get) {
        printf("ERR. obm_port_max_usage_mode_test retrieved value is not same\n");
        printf("as set one, set: ");
        print_obm_wm_mode(obm_wm_mode);
        printf("get: ");
        print_obm_wm_mode(obm_wm_mode_get);
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

void print_obm_tc(bcm_obm_traffic_class_t obm_tc)
{
    switch (obm_tc) {
        case bcmObmClassLossyLow:
            printf("bcmObmClassLossyLow\n");
            break;
        case bcmObmClassLossyHigh:
            printf("bcmObmClassLossyHigh\n");
            break;
        case bcmObmClassLossless0:
            printf("bcmObmClassLossless0\n");
            break;
        case bcmObmClassLossless1:
            printf("bcmObmClassLossless1\n");
            break;
        default:
            return BCM_E_INTERNAL;
    }
}

bcm_error_t obm_port_pri_traffic_class_mapping_test(int unit)
{
    bcm_obm_lookup_priority_type_t pri_type = bcmObmPriorityTypeUntagged;
    bcm_obm_traffic_class_t obm_tc = bcmObmClassLossless1;
    bcm_obm_traffic_class_t obm_tc_get;
    int priority = 3;

    if (BCM_FAILURE((rv = bcm_obm_port_pri_traffic_class_mapping_get(unit,
                                            port, pri_type, priority, &obm_tc_get)))) {
      printf("Error in bcm_obm_port_pri_traffic_class_mapping_get() : %s.\n", bcm_errmsg(rv));
      return rv;
    }

    printf("Default packet priority to OBM TC for untagged packet is ");

    print_obm_tc(obm_tc_get);

    if (BCM_FAILURE((rv = bcm_obm_port_pri_traffic_class_mapping_set(unit,
                                            port, pri_type, priority, obm_tc)))) {
      printf("Error in bcm_obm_port_pri_traffic_class_mapping_set() : %s.\n", bcm_errmsg(rv));
      return rv;
    }

    obm_tc_get = -1;
    if (BCM_FAILURE((rv = bcm_obm_port_pri_traffic_class_mapping_get(unit,
                                            port, pri_type, priority, &obm_tc_get)))) {
      printf("Error in bcm_obm_port_pri_traffic_class_mapping_get() : %s.\n", bcm_errmsg(rv));
      return rv;
    }

    if (obm_tc != obm_tc_get) {
        printf("ERR. obm_port_pri_traffic_class_mapping_test retrieved value is not same\n");
        printf("as set one, set: %d, get: %d\n", obm_tc, obm_tc_get);
        print_obm_wm_mode(obm_tc_get);
        return BCM_E_FAIL;
    }

    if (obm_tc != obm_tc_get) {
        printf("ERR. obm_port_pri_traffic_class_mapping_test retrieved value is not same\n");
        printf("as set one, set: ");
        print_obm_tc(obm_tc);
        printf("get: ");
        print_obm_tc(obm_tc_get);
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

int obm_test(int unit)
{
    int rv = BCM_E_NONE;

    printf("obm_traffic_class_pfc_priority_mapping_test() started.\n");
    rv = obm_traffic_class_pfc_priority_mapping_test(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. obm_traffic_class_pfc_priority_mapping_test FAILED. rv %d\n", rv);
        return rv;
    }
    printf("obm_traffic_class_pfc_priority_mapping_test() end.\n");

    printf("obm_port_max_usage_mode_test() started.\n");
    rv = obm_port_max_usage_mode_test(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. obm_port_max_usage_mode_test FAILED. rv %d\n", rv);
        return rv;
    }
    printf("obm_port_max_usage_mode_test() end.\n");

    printf("obm_port_pri_traffic_class_mapping_test() started.\n");
    rv = obm_port_pri_traffic_class_mapping_test(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. obm_port_pri_traffic_class_mapping_test FAILED. rv %d\n", rv);
        return rv;
    }
    printf("obm_port_max_usage_mode_test() end.\n");

    return rv;
}


/* Function to verify the result */
void verify(int unit)
{
  /*NULL */
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;

  /*bshell(unit, "config show; a ; version");*/

  /* select port */
  if (BCM_FAILURE((rv = test_setup(unit)))) {
     printf("test_setup() failed.\n");
     return -1;
  }

  /* Configure and retrieve the OBM properties*/
  if (BCM_FAILURE((rv = obm_test(unit)))) {
      printf("obm_test() failed.\n");
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


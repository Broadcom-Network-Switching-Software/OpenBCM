/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : classifier
 *
 *  Usage    : BCM.0> cint cosq_control_misc.c
 *
 *  config   :
 *
 *  Log file : cosq_control_misc_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below cosq APIs:
 *  ========
 *    bcm_cosq_service_pool_set
 *    bcm_cosq_service_pool_get
 *    bcm_cosq_priority_group_mapping_profile_set
 *    bcm_cosq_priority_group_mapping_profile_get
 *    bcm_cosq_service_pool_override_set
 *    bcm_cosq_service_pool_override_get
 *    bcm_cosq_port_priority_group_property_set
 *    bcm_cosq_port_priority_group_property_get
 *    bcm_cosq_port_priority_group_property_set
 *    bcm_cosq_port_priority_group_property_get
 *    bcm_cosq_buffer_id_multi_get
 *    bcm_cosq_buffer_id_multi_get
 *    bcm_cosq_profile_info_portlist_get
 *    bcm_cosq_profile_info_inuse_get
 *    bcm_cosq_profile_property_dynamic_get
 *    bcm_cosq_cpu_cosq_enable_set
 *    bcm_cosq_cpu_cosq_enable_get
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate the usage of the misc cosq control APIs listed above.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Do nothing.
 *    2) Step2 - carry related set/get verification.
 *    ======================================================
 *    3) Step3 - verify (Done in verify())
 *    ==========================================
 *      a) Do nothing.
 */

cint_reset();

/* GLOBAL VARIABLES */
int unit = 0;

bcm_cosq_control_data_t cosq_control_bindings[] =
{
    { bcmCosqControlIngressPortPGSharedDynamicEnable, 1},
    { bcmCosqControlDropLimitAlpha, 1},
    { bcmCosqControlIngressPool, 0},
    { bcmCosqControlIngressPortPoolMinLimitBytes, 508},
    { bcmCosqControlIngressPortPoolSharedLimitBytes, 1524},
    { bcmCosqControlIngressPortPoolResumeLimitBytes, 1016},
};

bcm_cosq_control_data_t cosq_control_bindings_uc[] =
{
    { bcmCosqControlEgressUCSharedDynamicEnable, 1},
    { bcmCosqControlDropLimitAlpha, 1},
    { bcmCosqControlEgressUCQueueColorLimitEnable, 1},
    { bcmCosqControlEgressUCQueueYellowLimit, 4},
};

bcm_cosq_control_data_t cosq_control_bindings_mc[] =
{
    { bcmCosqControlEgressMCSharedDynamicEnable, 1},
    { bcmCosqControlDropLimitAlpha, 1},
};

bcm_port_t port;
bcm_gport_t gport;
bcm_cos_queue_t cosq = 2;
bcm_cos_queue_t cosq_mc = 8;
bcm_cosq_control_t type;
int arg, out_arg;
int MAX_NUM_PORT = 272;
/* This function is written so that hardcoding of port
   numbers in Cint scripts is removed. This function gives
   required number of ports
*/

bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{

  int i=0, port=0, rv=0;
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

int service_pool_test(int unit)
{
    int rv = BCM_E_NONE;
    bcm_cosq_service_pool_t cosq_service_pool, out_cosq_service_pool;
    bcm_service_pool_id_t service_pool_id;

    service_pool_id = 0;
    bcm_cosq_service_pool_t_init(&cosq_service_pool);
    cosq_service_pool.type = bcmCosqServicePoolColorAware;
    cosq_service_pool.enabled = 1;

    rv = bcm_cosq_service_pool_set(unit, service_pool_id, cosq_service_pool);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_service_pool_set FAILED. rv %d\n", rv);
        return rv;
    }

    bcm_cosq_service_pool_t_init(&out_cosq_service_pool);
    out_cosq_service_pool.type = bcmCosqServicePoolColorAware;
    rv = bcm_cosq_service_pool_get(unit, service_pool_id, &out_cosq_service_pool);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_service_pool_get FAILED. rv %d\n", rv);
        return rv;
    }

    if (cosq_service_pool.enabled != out_cosq_service_pool.enabled)
    {
        printf("ERR. bcm_cosq_service_pool_get retrieved enable status not same\n");
        printf("as set one, set: %d, get: %d\n", cosq_service_pool.enabled,
                out_cosq_service_pool.enabled);
        return rv;
    }

    return rv;
}


/* Pri to PG mapping */
int pri_2_PG_map[] = {3,2,1,0,7,6,5,4,4,3,2,1,1,2,3,4};
int pri_2_PG_map_out[16] = {{0}};

int priority_group_mapping_profile_test(int unit)
{
    int rv = BCM_E_NONE;
    int profile_index;
    bcm_cosq_priority_group_mapping_profile_type_t type;
    int array_count;
    int array_max;
    int i;

    profile_index = 1;
    type = bcmCosqInputPriPriorityGroupUcMapping;
    array_count = 16;

    rv = bcm_cosq_priority_group_mapping_profile_set(unit, profile_index, type,
                                                     array_count, &pri_2_PG_map);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_priority_group_mapping_profile_set FAILED. rv %d\n", rv);
        return rv;
    }

    array_max = 16;
    rv = bcm_cosq_priority_group_mapping_profile_get(unit, profile_index, type,
                                                     array_max, &pri_2_PG_map_out,
                                                     &array_count);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_priority_group_mapping_profile_get FAILED. rv %d\n", rv);
        return rv;
    }

    for (i = 0; i < array_count; i++) {
        if (pri_2_PG_map[i] != pri_2_PG_map_out[i]) {
            printf("ERR. priority_group_mapping_profile_test retrieved value is not same\n");
            printf("as set one for entry %d, set: %d, get: %d\n", i, pri_2_PG_map[i],
                pri_2_PG_map_out[i]);
            return BCM_E_FAIL;

        }
    }

    return rv;
}

int service_pool_override_test(int unit)
{
    int rv = BCM_E_NONE;
    bcm_cosq_service_pool_override_type_t type;
    bcm_service_pool_id_t service_pool, service_pool_out;
    int enable, enable_out;

    service_pool = 1;
    type = bcmCosqServicePoolOverrideCpu;
    enable = 1;

    rv = bcm_cosq_service_pool_override_set(unit, type, service_pool, enable);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_service_pool_override_set FAILED. rv %d\n", rv);
        return rv;
    }

    enable_out= -1;
    rv = bcm_cosq_service_pool_override_get(unit, type, &service_pool_out, &enable_out);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_service_pool_override_get FAILED. rv %d\n", rv);
        return rv;
    }

    if (service_pool != service_pool_out) {
        printf("ERR. service_pool_override_test retrieved value of service_pool is not same\n");
        printf("as set one , set: %d, get: %d\n", service_pool, service_pool_out);
        return BCM_E_FAIL;
    }
    if (enable != enable_out) {
        printf("ERR. service_pool_override_test retrieved value of enable is not same\n");
        printf("as set one , set: %d, get: %d\n", enable, enable_out);
        return BCM_E_FAIL;
    }

    return rv;
}

int port_priority_group_property_test(int unit)
{
    int rv = BCM_E_NONE;
    bcm_cosq_port_prigroup_control_t type;
    int enable, enable_out;
    int priority_group_id;

    enable = 1;
    priority_group_id = 1;

    printf("***port_priority_group_property_test for type: bcmCosqPriorityGroupLossless=***\n");

    type = bcmCosqPriorityGroupLossless;

    rv = bcm_cosq_port_priority_group_property_set(unit, port, priority_group_id, type, enable);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_port_priority_group_property_set FAILED. rv %d\n", rv);
        return rv;
    }

    enable_out= -1;
    rv = bcm_cosq_port_priority_group_property_get(unit, port, priority_group_id, type, &enable_out);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_port_priority_group_property_get FAILED. rv %d\n", rv);
        return rv;
    }

    if (enable != enable_out) {
        printf("ERR. port_priority_group_property_test retrieved value of enable is not same\n");
        printf("as set one , set: %d, get: %d\n", enable, enable_out);
        return BCM_E_FAIL;
    }

    printf("***port_priority_group_property_test for type: bcmCosqPauseEnable=***\n");

    type = bcmCosqPauseEnable;

    rv = bcm_cosq_port_priority_group_property_set(unit, port, priority_group_id, type, enable);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_port_priority_group_property_set FAILED. rv %d\n", rv);
        return rv;
    }

    enable_out= -1;
    rv = bcm_cosq_port_priority_group_property_get(unit, port, priority_group_id, type, &enable_out);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_port_priority_group_property_get FAILED. rv %d\n", rv);
        return rv;
    }

    if (enable != enable_out) {
        printf("ERR. port_priority_group_property_test retrieved value of enable is not same\n");
        printf("as set one , set: %d, get: %d\n", enable, enable_out);
        return BCM_E_FAIL;
    }

    return rv;
}

int buffer_id_get_test(int unit)
{
    int rv = BCM_E_NONE;
    bcm_cosq_dir_t direction;
    int array_max = 2;
    bcm_cosq_buffer_id_t buf_id_array[2] = {-1, -1};
    int array_count = -1;

    printf("***buffer_id_get_test for direction: bcmCosqIngress=***\n");

    direction = bcmCosqIngress;
    rv = bcm_cosq_buffer_id_multi_get(unit, port, cosq, direction, array_max,
                                      &buf_id_array, &array_count);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_buffer_id_multi_get FAILED. rv %d\n", rv);
        return rv;
    }
    printf("Ingress buffer id of port %d, cosq %d is: %d, %d\n", port, cosq,
            buf_id_array[0], buf_id_array[1]);

    printf("***buffer_id_get_test for direction: bcmCosqEgress=***\n");

    direction = bcmCosqEgress;
    buf_id_array[0] = -1;
    buf_id_array[1] = -1;
    array_count = -1;
    rv = bcm_cosq_buffer_id_multi_get(unit, port, cosq, direction, array_max,
                                      &buf_id_array, &array_count);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_buffer_id_multi_get FAILED. rv %d\n", rv);
        return rv;
    }
    printf("Egress buffer id of port %d, cosq %d is: %d, %d\n", port, cosq,
           buf_id_array[0], buf_id_array[1]);

    return rv;
}

int profile_info_portlist_get_test(int unit)
{
    int rv = BCM_E_NONE;
    bcm_cosq_profile_type_t profile_type;
    bcm_cosq_profile_type_t profile_type_arr[] =
        {bcmCosqProfilePFCAndQueueHierarchy, bcmCosqProfilePGProperties,
         bcmCosqProfilePGProperties, bcmCosqProfileIntPriToPGMap};
    int profile_id;
    int max_port_count;
    bcm_port_t port_list[MAX_NUM_PORT];
    int port_count;
    int i, j;

    profile_id = 0;
    max_port_count= MAX_NUM_PORT;

    for (j = 0; j < bcmCosqProfileCount; j++) {
        profile_type = profile_type_arr[j];
        printf("***profile_info_portlist_get_test for type: ");

        switch (profile_type) {
            case bcmCosqProfilePFCAndQueueHierarchy:
                printf("bcmCosqProfilePFCAndQueueHierarchy***\n");
                break;
            case bcmCosqProfilePGProperties:
                printf("bcmCosqProfilePGProperties***\n");
                break;
            case bcmCosqProfilePGProperties:
                printf("bcmCosqProfilePGProperties***\n");
                break;
            case bcmCosqProfileIntPriToPGMap:
                printf("bcmCosqProfileIntPriToPGMap***\n");
                break;
            default:
                return BCM_E_INTERNAL;
        }

        for (i = 0; i < MAX_NUM_PORT; i++) {
            port_list[i] = -1;
        }

        port_count = -1;

        rv = bcm_cosq_profile_info_portlist_get(unit, profile_type, profile_id, max_port_count, &port_list,
                                          &port_count);
        if (rv != BCM_E_NONE)
        {
            printf("ERR. bcm_cosq_profile_info_portlist_get FAILED. rv %d\n", rv);
            return rv;
        }
        printf("port_count %d\n", port_count);
        printf("port list:");
        for (i = 0; i < port_count; i++) {
            if (!(i%20)) {
                printf("\n");
            }
            printf("%4d,", port_list[i]);
        }
        printf("\n\n");
    }

    return rv;
}

int profile_info_inuse_get_test(int unit)
{
    int rv = BCM_E_NONE;
    bcm_cosq_profile_type_t profile_type;
    bcm_cosq_profile_type_t profile_type_arr[] =
        {bcmCosqProfilePFCAndQueueHierarchy, bcmCosqProfilePGProperties,
         bcmCosqProfilePGProperties, bcmCosqProfileIntPriToPGMap};
    int profile_id;
    int j;
    int in_use;

    profile_id = 0;

    for (j = 0; j < bcmCosqProfileCount; j++) {
        profile_type = profile_type_arr[j];
        printf("***profile_info_inuse_get_test for type: ");

        switch (profile_type) {
            case bcmCosqProfilePFCAndQueueHierarchy:
                printf("bcmCosqProfilePFCAndQueueHierarchy***\n");
                break;
            case bcmCosqProfilePGProperties:
                printf("bcmCosqProfilePGProperties***\n");
                break;
            case bcmCosqProfilePGProperties:
                printf("bcmCosqProfilePGProperties***\n");
                break;
            case bcmCosqProfileIntPriToPGMap:
                printf("bcmCosqProfileIntPriToPGMap***\n");
                break;
            default:
                return BCM_E_INTERNAL;
        }

        rv = bcm_cosq_profile_info_inuse_get(unit, profile_type, profile_id, &in_use);
        if (rv != BCM_E_NONE)
        {
            printf("ERR. bcm_cosq_profile_info_inuse_get FAILED. rv %d\n", rv);
            return rv;
        }
        if (in_use)
        {
            printf("Profile %d is in use\n", profile_id);
        } else {
            printf("Profile %d is not in use\n", profile_id);
        }

    }

    return rv;
}

int profile_property_dynamic_get_test(int unit)
{
    int rv = BCM_E_NONE;
    bcm_cosq_profile_type_t profile_type;
    bcm_cosq_profile_type_t profile_type_arr[] =
        {bcmCosqProfilePFCAndQueueHierarchy, bcmCosqProfilePGProperties,
         bcmCosqProfilePGProperties, bcmCosqProfileIntPriToPGMap};
    int j;
    bcm_cosq_dynamic_setting_type_t dynamic;

    for (j = 0; j < bcmCosqProfileCount; j++) {
        profile_type = profile_type_arr[j];
        printf("***profile_property_dynamic_get_test for type: ");

        switch (profile_type) {
            case bcmCosqProfilePFCAndQueueHierarchy:
                printf("bcmCosqProfilePFCAndQueueHierarchy***\n");
                break;
            case bcmCosqProfilePGProperties:
                printf("bcmCosqProfilePGProperties***\n");
                break;
            case bcmCosqProfilePGProperties:
                printf("bcmCosqProfilePGProperties***\n");
                break;
            case bcmCosqProfileIntPriToPGMap:
                printf("bcmCosqProfileIntPriToPGMap***\n");
                break;
            default:
                return BCM_E_INTERNAL;
        }

        rv = bcm_cosq_profile_property_dynamic_get(unit, profile_type, &dynamic);
        if (rv != BCM_E_NONE)
        {
            printf("ERR. bcm_cosq_profile_property_dynamic_get FAILED. rv %d\n", rv);
            return rv;
        }

        printf("Profile type is: ");
        switch (dynamic) {
            case bcmCosqDynamicTypeFixed:
                printf("bcmCosqDynamicTypeFixed\n");
                break;
            case bcmCosqDynamicTypeFlexible:
                printf("bcmCosqDynamicTypeFlexible\n");
                break;
            case bcmCosqDynamicTypeConditional:
                printf("bcmCosqDynamicTypeConditional\n");
                break;
            default:
                return BCM_E_INTERNAL;
        }
    }

    return rv;
}

int cpu_cosq_enable_test(int unit)
{
    int rv = BCM_E_NONE;
    int enable, enable_out;

    enable = 1;

    rv = bcm_cosq_cpu_cosq_enable_set(unit, cosq, enable);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_cpu_cosq_enable_set FAILED. rv %d\n", rv);
        return rv;
    }

    enable_out= -1;
    rv = bcm_cosq_cpu_cosq_enable_get(unit, cosq, &enable_out);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. bcm_cosq_cpu_cosq_enable_get FAILED. rv %d\n", rv);
        return rv;
    }

    if (enable != enable_out) {
        printf("ERR. cpu_cosq_enable_test retrieved value of enable is not same\n");
        printf("as set one , set: %d, get: %d\n", enable, enable_out);
        return BCM_E_FAIL;
    }

    return rv;
}

int cosq_control_misc_test(int unit)
{
    int rv = BCM_E_NONE;

    printf("service_pool_test() started.\n");
    rv = service_pool_test(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERR. service_pool_test FAILED. rv %d\n", rv);
        return rv;
    }
    printf("service_pool_test() end.\n");

    printf("priority_group_mapping_profile_test() started.\n");
    if (BCM_FAILURE((rv = priority_group_mapping_profile_test(unit)))) {
        printf("priority_group_mapping_profile_test() failed.\n");
        return -1;
    }
    printf("priority_group_mapping_profile_test() end.\n");

    printf("service_pool_override_test() started.\n");
    if (BCM_FAILURE((rv = service_pool_override_test(unit)))) {
        printf("service_pool_override_test() failed.\n");
        return -1;
    }
    printf("service_pool_override_test() end.\n");

    printf("port_priority_group_property_test() started.\n");
    if (BCM_FAILURE((rv = port_priority_group_property_test(unit)))) {
        printf("port_priority_group_property_test() failed.\n");
        return -1;
    }
    printf("port_priority_group_property_test() end.\n");

    printf("buffer_id_get_test() started.\n");
    if (BCM_FAILURE((rv = buffer_id_get_test(unit)))) {
        printf("buffer_id_get_test() failed.\n");
        return -1;
    }
    printf("buffer_id_get_test() end.\n");

    printf("profile_info_portlist_get_test() started.\n");
    if (BCM_FAILURE((rv = profile_info_portlist_get_test(unit)))) {
        printf("profile_info_portlist_get_test() failed.\n");
        return -1;
    }
    printf("profile_info_portlist_get_test() end.\n");

    printf("profile_info_inuse_get_test() started.\n");
    if (BCM_FAILURE((rv = profile_info_inuse_get_test(unit)))) {
        printf("profile_info_inuse_get_test() failed.\n");
        return -1;
    }
    printf("profile_info_inuse_get_test() end.\n");

    printf("profile_property_dynamic_get_test() started.\n");
    if (BCM_FAILURE((rv = profile_property_dynamic_get_test(unit)))) {
        printf("profile_property_dynamic_get_test() failed.\n");
        return -1;
    }
    printf("profile_property_dynamic_get_test() end.\n");

    printf("cpu_cosq_enable_test() started.\n");
    if (BCM_FAILURE((rv = cpu_cosq_enable_test(unit)))) {
        printf("cpu_cosq_enable_test() failed.\n");
        return -1;
    }
    printf("cpu_cosq_enable_test() end.\n");

    return rv;
}

/* Function to verify the result */
void verify(int unit)
{

/* Check already done in cosq_control_misc_test */
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

  /* Carry related set/get verification. */
  printf("cosq_control_misc_test() started.\n");
  if (BCM_FAILURE((rv = cosq_control_misc_test(unit)))) {
      printf("cosq_control_misc_test() failed.\n");
      return -1;
  }
  printf("cosq_control_misc_test() end.\n");

  /* NULL function here as check already done in cosq_control_misc_test */
  verify(unit);

  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}




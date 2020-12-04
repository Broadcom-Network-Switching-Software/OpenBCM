/*
 * Feature  : OBM
 *
 * Usage    : BCM.0> cint th4_hsdk_mmu_obm.c
 *
 * Config   : th4_hsdk_mmu_config.yml
 *
 * Log file : th4_hsdk_mmu_obm_log.txt
 *
 * Test Topology :
 *            None
 *
 * This script can verify below cosq APIs:
 * ========
 *     bcm_obm_traffic_class_pfc_priority_mapping_get
 *     bcm_obm_traffic_class_pfc_priority_mapping_set
 *
 *     bcm_obm_port_max_usage_mode_get
 *     bcm_obm_port_max_usage_mode_set
 *
 *     bcm_switch_obm_classifier_mapping_set
 *     bcm_switch_obm_classifier_mapping_get
 *     bcm_switch_obm_classifier_mapping_multi_set
 *     bcm_switch_obm_classifier_mapping_multi_get
 *
 *     bcm_cosq_stat_get
 *     bcm_cosq_stat_set
 *
 * Summary:
 * ========
 * This cint example demonstrate configure and retrieve OBM properties
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *      a) Select one egress port to test
 *
 *    2) Step2 - Configuration (Done in test_verify())
 *      a) Map loseless OBM traffic class to PFC priority
 *      b) Set the mapping from code point, Dest MAC and EtherType to OBM priority
 *      c) Max watermark mode
 *
 *    3) Step3 - Verification (Done in test_verify())
 *      a) OBM watermark counters
 *     Expected Result:
 *        None
 *
 *    4) Step4 - Cleanup (Done in test_cleanup())
 *       None
 */

cint_reset();

int unit=0;

bcm_port_t test_port;
int rv;

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i=0, port=0, rv=0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if(BCM_FAILURE(rv)) {
       printf("\nError in retrieving port configuration: %s.\n", bcm_errmsg(rv));
       return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)&& (port < num_ports)) {
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

/*
 * Map loseless OBM traffic class to PFC priority for a port
 */
bcm_error_t
obm_traffic_class_pfc_priority_mapping_test(int unit)
{
    int pri_list[5] = {1, 2, 3, 4, 5};
    int max_pri_count = 5;
    bcm_obm_traffic_class_t obm_tc = bcmObmClassLossless0;
    int pri_get_list[5];
    int pri_get_count;
    int i;

    rv = bcm_obm_traffic_class_pfc_priority_mapping_set(unit, 
                                  test_port, obm_tc, max_pri_count, pri_list);
    if(BCM_FAILURE(rv)) {
        printf("Error in bcm_obm_traffic_class_pfc_priority_mapping_set() : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_obm_traffic_class_pfc_priority_mapping_get(unit, 
                          test_port, obm_tc, max_pri_count, pri_get_list, &pri_get_count);
    if(BCM_FAILURE(rv)) {
        printf("Error in bcm_obm_traffic_class_pfc_priority_mapping_get() : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    for (i = 0; i < pri_get_count; i++) {
        if (pri_list[i] != pri_get_list[i]) {
            printf("ERR. obm_traffic_class_pfc_priority_mapping retrieved value is not same\n");
            printf("as set one for %d priority, set: %d, get: %d\n", i, pri_list[i], pri_get_list[i]);
            return BCM_E_FAIL;

        }
    }

    return BCM_E_NONE;
}

/* Print the OBM class */
void print_obm_priority(bcm_switch_obm_priority_t obm_tc)
{
    switch (obm_tc) {
        case 0:
            printf("BCM_SWITCH_OBM_PRIORITY_LOSSY_LOW\n");
            break;
        case 1:
            printf("BCM_SWITCH_OBM_PRIORITY_LOSSY_HIGH\n");
            break;
        case 2:
            printf("BCM_SWITCH_OBM_PRIORITY_LOSSLESS0\n");
            break;
        case 3:
            printf("BCM_SWITCH_OBM_PRIORITY_LOSSLESS1\n");
            break;
        case 4:
            printf("BCM_SWITCH_OBM_PRIORITY_EXPRESS\n");
            break;
        default:
            return BCM_E_INTERNAL;
    }
}

/*
 * Set the mapping from code_point, Dest MAC and EtherType to a
 * OBM priority based on the specified OBM classifier type (VLAN tag, MPLS EXP, DSCP, etc)
 */
bcm_error_t port_codepoint_obm_class_mapping_test(int unit)
{
    uint8 dest_mac[6] = {0x00, 0x00, 0x00, 0x02, 0x01, 0x02};
    uint8 dest_mac_mask[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xf0};
    uint8 dest_mac1[6] = {0x00, 0x00, 0x00, 0x02, 0x01, 0x01};
    uint8 dest_mac_mask1[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xf0};
    bcm_error_t rv;
    bcm_switch_obm_classifier_type_t classifier_type;
    bcm_switch_obm_classifier_t obm_classifier;
    bcm_switch_obm_classifier_t obm_classifier_get;

    /*
     * Test OBM classifier of type VLAN
     */ 
    classifier_type = bcmSwitchObmClassifierVlan;

    bcm_switch_obm_classifier_t_init(&obm_classifier);
    obm_classifier.obm_code_point = 2;
    obm_classifier.obm_priority = BCM_SWITCH_OBM_PRIORITY_LOSSY_HIGH;
    rv = bcm_switch_obm_classifier_mapping_set(unit, test_port, classifier_type, &obm_classifier);
    if(BCM_FAILURE(rv)) {
       printf("\nError in bcm_switch_obm_classifier_mapping_set: %s.\n", bcm_errmsg(rv));
       return rv;
    }

    bcm_switch_obm_classifier_t_init(&obm_classifier_get);
    obm_classifier_get.obm_code_point = 2;
    rv = bcm_switch_obm_classifier_mapping_get(unit, test_port, classifier_type, &obm_classifier_get);
    if(BCM_FAILURE(rv)) {
       printf("\nError in bcm_switch_obm_classifier_mapping_get: %s.\n", bcm_errmsg(rv));
       return rv;
    }

    if (obm_classifier_get.obm_priority != obm_classifier.obm_priority) {
        printf("Error: ODM classifier does not match\n");
        return BCM_E_FAIL;
    }

    /*
     * Test OBM classifier of type HiGig3
     * Expect to return BCM_E_UNAVAIL as bcmSwitchObmClassifierHigig3 is not supported on TH4
     */ 
    classifier_type = bcmSwitchObmClassifierHigig3;

    bcm_switch_obm_classifier_t_init(&obm_classifier);
    obm_classifier.obm_code_point = 8;
    obm_classifier.obm_priority = BCM_SWITCH_OBM_PRIORITY_LOSSY_HIGH;
    rv = bcm_switch_obm_classifier_mapping_set(unit, test_port, classifier_type, &obm_classifier);
    if(rv == BCM_E_UNAVAIL) {
       printf("\nOBM classifier type bcmSwitchObmClassifierHigig3 not supported on TH4\n");
    } else if (BCM_FAILURE(rv)) {
       printf("\nError in bcm_switch_obm_classifier_mapping_set: %s.\n", bcm_errmsg(rv));
       return rv;
    }

    /*
     * Test OBM classifier of type FIELD
     */ 
    classifier_type = bcmSwitchObmClassifierField;

    bcm_switch_obm_classifier_t_init(&obm_classifier);
    obm_classifier.obm_ethertype = 0x08bb;
    obm_classifier.obm_ethertype_mask = 0xffff;
    obm_classifier.obm_destination_mac = dest_mac;
    obm_classifier.obm_destination_mac_mask = dest_mac_mask;
    obm_classifier.obm_code_point = 2;
    obm_classifier.obm_priority = BCM_SWITCH_OBM_PRIORITY_LOSSY_HIGH;
    rv = bcm_switch_obm_classifier_mapping_set(unit, test_port, classifier_type, &obm_classifier);
    if(BCM_FAILURE(rv)) {
       printf("\nError in bcm_switch_obm_classifier_mapping_set: %s.\n", bcm_errmsg(rv));
       return rv;
    }

    bcm_switch_obm_classifier_t_init(&obm_classifier_get);
    obm_classifier_get.obm_code_point = 2;
    rv = bcm_switch_obm_classifier_mapping_get(unit, test_port, classifier_type, &obm_classifier_get);
    if(BCM_FAILURE(rv)) {
       printf("\nError in bcm_switch_obm_classifier_mapping_get: %s.\n", bcm_errmsg(rv));
       return rv;
    }

    print obm_classifier_get;
    if (obm_classifier_get.obm_priority != obm_classifier.obm_priority) {
        printf("Error: ODM classifier does not match\n");
        return BCM_E_FAIL;
    }

    bcm_switch_obm_classifier_t obm_classifier_arr[2];
    
    bcm_switch_obm_classifier_t_init(&obm_classifier_arr[0]);
    bcm_switch_obm_classifier_t_init(&obm_classifier_arr[1]);

    classifier_type = bcmSwitchObmClassifierField;

    obm_classifier_arr[0].obm_ethertype = 0x08bb;
    obm_classifier_arr[0].obm_ethertype_mask = 0xffff;
    obm_classifier_arr[0].obm_destination_mac = dest_mac;
    obm_classifier_arr[0].obm_destination_mac_mask = dest_mac_mask;
    obm_classifier_arr[0].obm_code_point = 2;
    obm_classifier_arr[0].obm_priority = BCM_SWITCH_OBM_PRIORITY_LOSSY_HIGH;

    obm_classifier_arr[1].obm_ethertype = 0x08bb;
    obm_classifier_arr[1].obm_ethertype_mask = 0xffff;
    obm_classifier_arr[1].obm_destination_mac = dest_mac1;
    obm_classifier_arr[1].obm_destination_mac_mask = dest_mac_mask1;
    obm_classifier_arr[1].obm_code_point = 2;
    obm_classifier_arr[1].obm_priority = BCM_SWITCH_OBM_PRIORITY_LOSSY_HIGH;

    rv = bcm_switch_obm_classifier_mapping_multi_set(unit, test_port,
                         classifier_type, 2, &obm_classifier_arr);
    if(BCM_FAILURE(rv)) {
       printf("\nError in bcm_switch_obm_classifier_mapping_multi_set: %s.\n", bcm_errmsg(rv));
       return rv;
    }

    bcm_switch_obm_classifier_t obm_classifier_get_arr[2];
    bcm_switch_obm_classifier_t_init(&obm_classifier_get_arr[0]);
    bcm_switch_obm_classifier_t_init(&obm_classifier_get_arr[1]);

    int count = 0;

    rv = bcm_switch_obm_classifier_mapping_multi_get(unit, test_port,
                         classifier_type, 2, &obm_classifier_get_arr, &count);
    if(BCM_FAILURE(rv)) {
       printf("\nError in bcm_switch_obm_classifier_mapping_multi_get: %s.\n", bcm_errmsg(rv));
       return rv;
    }

    print obm_classifier_get_arr;
    print count;

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

/*
 * Max watermark mode for a port
 */
bcm_error_t
obm_port_max_usage_mode_test(int unit)
{
    bcm_obm_max_watermark_mode_t obm_wm_mode;
    bcm_obm_max_watermark_mode_t obm_wm_mode_get = -1;

    rv = bcm_obm_port_max_usage_mode_get(unit, test_port, &obm_wm_mode_get);
    if(BCM_FAILURE(rv)) {
        printf("Error in bcm_obm_port_max_usage_mode_get() : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Default OBM port max usage mode is ");
    print_obm_wm_mode(obm_wm_mode_get);

    obm_wm_mode = bcmObmMaxUsageModeLossless0;
    rv = bcm_obm_port_max_usage_mode_set(unit, test_port, obm_wm_mode);
    if(BCM_FAILURE(rv)) {
        printf("Error in bcm_obm_port_max_usage_mode_set() : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    obm_wm_mode_get = -1;
    rv = bcm_obm_port_max_usage_mode_get(unit, test_port, &obm_wm_mode_get);
    if(BCM_FAILURE(rv)) {
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

/*
 * Get High Watermark statistics counter of a port
 */
bcm_error_t
obm_max_watermark_stat_get (int unit)
{
    bcm_gport_t test_gport;
    uint64 cell_count;
    uint64 byte_count;

    bcm_port_gport_get(unit, test_port, &test_gport);

    /* Get the per port OBM cell max usage */
    rv = bcm_cosq_stat_get(unit, test_gport, BCM_COS_INVALID, bcmCosqStatOBMHighWatermark, &cell_count);
    if(BCM_FAILURE(rv)) {
        printf("Error in bcm_cosq_stat_get() : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf(" OBM cell max usage : %d-%d\n", COMPILER_64_HI(cell_count), COMPILER_64_LO(cell_count));

    /* Get the per port OBM cell max usage */
    rv = bcm_cosq_stat_get(unit, test_gport, BCM_COS_INVALID, bcmCosqStatOBMLossyBufferBytes, &byte_count);
    if(BCM_FAILURE(rv)) {
        printf("Error in bcm_cosq_stat_get() : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf(" OBM max byte usage : %d-%d\n", COMPILER_64_HI(byte_count), COMPILER_64_LO(byte_count));

    COMPILER_64_SET(cell_count, 0, 0);
    rv = bcm_cosq_stat_set(unit, test_gport, BCM_COS_INVALID, bcmCosqStatOBMHighWatermark, cell_count);
    if(BCM_FAILURE(rv)) {
        printf("Error in bcm_cosq_stat_set() : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Register a OBM callback function
 */
/*
int obm_count;

void obm_callback_fn(int unit, bcm_obm_interrupt_info_t *obm_info, void *userdata)
{
    int *count = (auto) userdata;

    (*count)++;

    printf("port %d, interrupt_type %d\n", obm_info->port, obm_info->interrupt_type);
}

bcm_error_t register_obm_intr_callback(int unit)
{
    return bcm_obm_cb_register (unit, obm_callback_fn, &obm_count);
}

bcm_error_t unregister_obm_intr_callback(int unit)
{
    return bcm_obm_cb_unregister(unit, obm_callback_fn, NULL);
}
*/

/*
 * Map OBM traffic class to different prioity types such as
 * bcmObmPriorityTypeVLAN, bcmObmPriorityTypeDCSP
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
    bcm_obm_lookup_priority_type_t pri_type;
    bcm_obm_traffic_class_t obm_tc;
    bcm_obm_traffic_class_t obm_tc_get;
    int priority = 3;

    pri_type = bcmObmPriorityTypeUntagged;
    rv = bcm_obm_port_pri_traffic_class_mapping_get(unit, test_port, pri_type, priority, &obm_tc_get);
    if(BCM_FAILURE(rv)) {
        printf("Error in bcm_obm_port_pri_traffic_class_mapping_get() : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Default packet priority to OBM TC for untagged packet is ");
    print_obm_tc(obm_tc_get);

    pri_type = bcmObmPriorityTypeUntagged;
    obm_tc = bcmObmClassLossless1;
    rv = bcm_obm_port_pri_traffic_class_mapping_set(unit, test_port, pri_type, priority, obm_tc);
    if(BCM_FAILURE(rv)) {
        printf("Error in bcm_obm_port_pri_traffic_class_mapping_set() : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    obm_tc_get = -1;
    rv = bcm_obm_port_pri_traffic_class_mapping_get(unit, test_port, pri_type, priority, &obm_tc_get);
    if(BCM_FAILURE(rv)) {
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
 */

int obm_test(int unit)
{
    int rv = BCM_E_NONE;

    printf("--> PFC priorities mapped to OBM traffic class\n");
    rv = obm_traffic_class_pfc_priority_mapping_test(unit);
    if (rv != BCM_E_NONE) {
        printf("ERR. obm_traffic_class_pfc_priority_mapping_test FAILED. rv %d\n", rv);
        return rv;
    }
    printf("DONE\n");

    printf("--> port_codepoint_obm_class_mapping_test\n");
    rv = port_codepoint_obm_class_mapping_test(unit);
    if (rv != BCM_E_NONE) {
        printf("ERR. port_codepoint_obm_class_mapping_test FAILED. rv %d\n", rv);
        return rv;
    }
    printf("DONE\n");

    printf("--> Max watermark mode test\n");
    rv = obm_port_max_usage_mode_test(unit);
    if (rv != BCM_E_NONE) {
        printf("ERR. obm_port_max_usage_mode_test FAILED. rv %d\n", rv);
        return rv;
    }
    printf("DONE\n");

    printf("--> Max watermark counter test\n");
    rv = obm_max_watermark_stat_get(unit);
    if (rv != BCM_E_NONE) {
        printf("ERR. obm_max_watermark_stat_get FAILED. rv %d\n", rv);
        return rv;
    }
    printf("DONE\n");

/*
    printf("--> bcm_obm_port_pri_traffic_class_mapping\n");
    rv = obm_port_pri_traffic_class_mapping_test(unit);
    if (rv != BCM_E_NONE) {
        printf("ERR. obm_port_pri_traffic_class_mapping_test FAILED. rv %d\n", rv);
        return rv;
    }
    printf("DONE\n");
*/

    return rv;
}

bcm_error_t test_setup(int unit)
{
    int port_list[1];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 1)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }
    test_port = port_list[0];

    /* register_obm_intr_callback(unit); */

    return BCM_E_NONE;
}

/* Function to verify the result */
bcm_error_t test_verify(int unit)
{
    /* Configure and retrieve the OBM properties*/
    if (BCM_FAILURE((rv = obm_test(unit)))) {
        printf("obm_test() failed.\n");
        return -1;
    }

    return BCM_E_NONE;
}

/* Clean up pre-test setup */
int test_cleanup(int unit)
{
    printf("==>Cleaning up...\n");

    /* unregister_obm_intr_callback(unit); */

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; attach; version");

    /* select port */
    if (BCM_FAILURE((rv = test_setup(unit)))) {
       printf("test_setup() failed.\n");
       return -1;
    }

    /* verify the result */
    if (BCM_FAILURE((rv = test_verify(unit)))) {
       printf("test_verify() failed.\n");
       return -1;
    }

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


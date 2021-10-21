/* $Id: cint_elephant_flow.c
*/

/**
 *
 * This cint calls all the required APIs to configure a basic 
 * Elephant Flow. 
 *
 * How to run: 
 *  cint ../../../../src/examples/sand/cint_ip_route_basic.c
 *  cint ../../../../src/examples/dnx/field/cint_field_utils.c
 *  cint../../../../src/examples/dnx/elephant_flow/cint_elephant_flow_field.c
 *  cint../../../../src/examples/dnx/elephant_flow/cint_elephant_flow.c
 *  cint
 *  cint_elephant_flow_main(unit, inPort, routeOutPort, cpuPort);
 *  cint_elephant_flow_set_age_period( unit, age_scan_period_usec);
 *  cint_elephant_flow_set_probability(unit, inPort);
 */

/* An indication of skipping L3 setting or not */
int elephant_flow_skip_l3_cfg = 0;

struct cint_elephant_flow_info_s {
 
   uint32 kaps_result;                   /* kaps result for the basic route. */
   uint32 flush_profile_id;
   bcm_field_layer_type_t fwd_layer_type;
   int high_threshold_value;
   int low_threshold_value;
   int increment_value;
   int decrement_value;
   int maximal_value;
   int out_value;
   int init_value;
   int trace_probability;
};

cint_elephant_flow_info_s cint_elephant_flow_info = {
 
   /* kaps result */
   0xA711,
   /* flush_profile_id */
   5,
   /* fwd_layer_type */
   bcmFieldLayerTypeIp4,
   /* high_threshold_value */
   20,
   /* low_threshold_value */
   5,
   /* increment_value */
   2,
   /* decrement_value */
   2,
   /* maximal_value */
   50,
   /* out_value */
   0,
   /* init_value */
   10,
   /* trace_probability */
   100
};


/*
 * Elephant Flow initialization function: 
 *    - enables trace probability.
 */
int
cint_elephant_flow_set_probability(int unit, int in_port)
{
    int rv;
    bcm_switch_control_key_t control_key;
    bcm_switch_control_info_t control_info;
    int trace_prob;

    /*
     * Enable Trace probability. 
     *  
     * percentage - percentage in units of one-tenth. 
     * Meaning that, for example, probability of 40% is achieved by setting this value to 400. 
     *  
     * For basic test - set it to 100% - that is trace every packet from in_port
     */
    trace_prob = cint_elephant_flow_info.trace_probability;
    
    rv = bcm_instru_gport_control_set(unit, in_port, 0, bcmInstruGportControlTraceProbability, trace_prob * 10);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_gport_control_set\n");
        return rv;
    }
    
    printf("cint_elephant_flow_set_probability: Set port = %d Trace probability to %d\n", in_port, trace_prob);

    return BCM_E_NONE;
}

/*
 * This function creates Elephant Flow setup: 
 *    - create Router setup.
 *    - create FG (Field Group) and add flush-machine entries (rule and action).
 *    - create flush profile.
 *    - attach the FG to the created Flush profile.
 */
int
cint_elephant_flow_main(int unit, int in_port, int route_out_port, int cpu_port)
{
    bcm_error_t rv;
    int fec_id;
    bcm_switch_control_key_t control_key;
    bcm_switch_control_info_t control_info;
    int state_action_offset;
    int snoop_action_offset;
    int age_action_offset;
    bcm_field_flush_profile_info_t flush_profile_info;
    int snoop_cmd;
    int snoop_str;

    /*
     * Create traffic flow - ip route basic.
     */
    if (!elephant_flow_skip_l3_cfg)
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit,0, &fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        cint_elephant_flow_info.kaps_result = fec_id;
        
        rv = dnx_basic_example(unit, in_port, route_out_port, cint_elephant_flow_info.kaps_result);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in dnx_basic_example\n");
            return rv;
        }
    }

    /*
     * Create SNOOP to CPU
     */
    rv = cint_field_ele_flow_utils_mirror_create(unit, cpu_port, &snoop_cmd);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_field_ele_flow_utils_mirror_create)\n");
        return rv;
    }

    /*
     * Create FG and add flush-machine entries (rule and action)
     */
    snoop_str = 7;
    rv = cint_field_ele_flow_main(unit, cint_elephant_flow_info.fwd_layer_type, snoop_cmd, snoop_str, cint_elephant_flow_info.flush_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_field_ele_flow_main\n");
        return rv;
    }

    /*
     * Set EXEM flush profile payload offset 
     *   bcmSwitchExemFlushProfilePayloadOffset
     *  
     * Take the created FG age offset and sets LBP
     */
    rv = cint_field_ele_flow_get_flush_profile_offset( unit,  &age_action_offset);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in cint_field_ele_flow_get_flush_profile_offset\n", rv);
      return rv;
    }
    printf(" age_action_offset = %d\n", age_action_offset);

    control_key.type = bcmSwitchExemFlushProfilePayloadOffset;
    control_key.index = 0; /* EXEM-3 */
    control_info.value = age_action_offset;

    rv = bcm_switch_control_indexed_set(unit, control_key, control_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_switch_control_indexed_set(bcmSwitchExemFlushProfilePayloadOffset)\n");
        return rv;
    }

    /*
     * Create Flush profile 
     *  
     * Note: 
     * The user can use the flag "WITH ID" to create the profile ID he choose, 
     * or to set the flag to 0 and let the SDK choose a vacant profile ID for him. 
     */
    bcm_field_flush_profile_info_t_init(&flush_profile_info);

    flush_profile_info.high_threshold_value = cint_elephant_flow_info.high_threshold_value;
    flush_profile_info.low_threshold_value = cint_elephant_flow_info.low_threshold_value;
    flush_profile_info.increment_value = cint_elephant_flow_info.increment_value;
    flush_profile_info.decrement_value = cint_elephant_flow_info.decrement_value;
    flush_profile_info.maximal_value = cint_elephant_flow_info.maximal_value;
    flush_profile_info.out_value = cint_elephant_flow_info.out_value;
    flush_profile_info.init_value = cint_elephant_flow_info.init_value;

    rv = bcm_field_flush_profile_create(unit, BCM_FIELD_FLAG_WITH_ID, &flush_profile_info, &cint_elephant_flow_info.flush_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_field_flush_profile_create)\n");
        return rv;
    }
    printf("cint_elephant_flow_main: flush profile (%d) is created.\n", cint_elephant_flow_info.flush_profile_id);

    /*
     * Attach the FG to the created Flush profile
     */
    rv = bcm_field_flush_profile_attach(unit, 0, cint_field_ele_flow_exem_fg_id, cint_elephant_flow_info.flush_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_field_flush_profile_attach)\n");
        return rv;
    }
    printf("cint_elephant_flow_main: fg_id (%d) is attached to flush profile (%d).\n", cint_field_ele_flow_exem_fg_id, cint_elephant_flow_info.flush_profile_id);
 
    return BCM_E_NONE;
}

/*
 * Elephant Flow initialization function: 
 *    - sets EXEM age scan period.
 *  
 *   Note:
 *   For functional testing -
 *   the scan period should be set to a large scan period so signals can be verified in between periods.
 *   Set the scan period to 5 sec.
 *  
 *   For opertional mode - set is to 500 uSec (0.5 mSec0
 */
int
cint_elephant_flow_set_age_period(int unit, int age_scan_period_usec)
{
    int rv;
    bcm_switch_control_key_t control_key;
    bcm_switch_control_info_t control_info;


    /*
     * Set EXEM flush-machine scan period (uSec)
     *   bcmSwitchExemScanPeriod
     */
    control_key.type = bcmSwitchExemScanPeriod;
    control_key.index = 0; /* EXEM-3 */
    control_info.value = age_scan_period_usec;
  
    rv = bcm_switch_control_indexed_set(unit, control_key, control_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_switch_control_indexed_set(bcmSwitchExemScanPeriod)\n");
        return rv;
    }

    printf("cint_elephant_flow_set_age_period: Set EXEM age scan period to %d uSec\n", age_scan_period_usec);

    return BCM_E_NONE;
}


/*
 * Elephant Flow delete function: 
 *    - disable trace probability.
 *    - disable EXEM flush-machine scan period.
 *    - delete Field Group setup.
 *    - destroy Flush profile.
 */
int
cint_elephant_flow_delete(int unit, int in_port)
{
    int rv;
    bcm_switch_control_key_t control_key;
    bcm_switch_control_info_t control_info;
    int trace_prob;
    int scan_period;

    /*
     * Disable Trace probability. 
     *  
     * percentage - percentage in units of one-tenth. 
     * Meaning that, for example, probability of 40% is achieved by setting this value to 400. 
 */
    trace_prob = 0;
    
    rv = bcm_instru_gport_control_set(unit, in_port, 0, bcmInstruGportControlTraceProbability, trace_prob * 10);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_gport_control_set\n");
        return rv;
    }
    
    printf("cint_elephant_flow_delete: Set port = %d Trace probability to %d\n", in_port, trace_prob);

    /*
     * clear EXEM flush-machine scan period
     *   bcmSwitchExemScanPeriod
     */
    scan_period = 0;

    control_key.type = bcmSwitchExemScanPeriod;
    control_key.index = 0; /* EXEM-3 */
    control_info.value = scan_period;
  
    rv = bcm_switch_control_indexed_set(unit, control_key, control_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_switch_control_indexed_set(bcmSwitchExemScanPeriod)\n");
        return rv;
    }

    printf("cint_elephant_flow_delete: Set EXEM scan period to %d uSec\n", scan_period);

   /*
   *  Destroy all Field Group configuration.
   */
    rv = cint_field_ele_flow_destroy(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_field_ele_flow_destroy\n");
        return rv;
    }

    rv = bcm_field_flush_profile_destroy(unit, 0, cint_elephant_flow_info.flush_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_field_flubcm_field_flush_profile_destroysh_profile_create)\n");
        return rv;
    }
    printf("cint_elephant_flow_delete: flush profile (%d) is destroyed.\n", cint_elephant_flow_info.flush_profile_id);

    return BCM_E_NONE;
}


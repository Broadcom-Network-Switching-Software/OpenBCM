/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * FT Group C file to put flow tracker group related api calls
 */

/* No of entries in key double view */
uint32 ftv2_key_double_entry_count = ift_key_double_entry_count;

/* FT Group Related Configuration Variables */
uint8 ftv2_group_test_config = 0;
bcm_flowtracker_ipfix_observation_domain_t ipfix_observation_domain_id1 = 0;
bcm_flowtracker_group_t flow_group_id1 = -1;

/* FT Group Flow Limit Value */
uint8 ftv2_group_flow_limit_test_config = 0;
uint32 flow_limit_set1 = 1;

/* FT Group Ageing Timer */
uint8 ftv2_group_ageing_timer_test_config = 0;
uint32 ageing_interval_ms1 = 10000;

/* FT Group Collector Copy Info */
uint8 ftv2_group_collector_copy_test_config = 0;
uint32 initial_samples1 = 3;

/* CPU Notification Info Variables */
uint8 ftv2_group_cpu_notification_test_config = 0;

/* Group Control Variables */
uint8 ftv2_group_control_test_config = 0;

/* Meter Profile Variables */
uint8 ftv2_group_meter_test_config = 0;

/* Tracking param variables, make sure number of elements and array are same */
uint8 ftv2_group_tracking_param_test_config = 0;
int max_num_tracking_params = 18;
bcm_flowtracker_tracking_param_info_t tracking_info1[max_num_tracking_params];
int actual_num_tracking_params1 = 0;

/* Group Stat Variables */
bcm_flowtracker_group_stat_t group_stats1;

/* Expected Group flow count */
uint16 expected_group_property_flow_count = 0;

/* Expected packet count value for
 * param bcmFlowtrackerTrackingParamTypePktCount */
uint32 expected_tparam_pkt_count_value = 0;

/* Expected packet count value for
 * param bcmFlowtrackerTrackingParamTypeByteCount */
uint32 expected_tparam_byte_count_value = 0;

/* Expected chip delay value for
 * param bcmFlowtrackerTrackingParamTypeChipDelay */
uint32 expected_tparam_chip_delay_value = 0;

/* Expected ipat delay value for
 * param bcmFlowtrackerTrackingParamTypeIPATDelay */
uint32 expected_tparam_ipat_delay_value = 0;

/* Expected ipdt delay value for
 * param bcmFlowtrackerTrackingParamTypeIPDTDelay */
uint32 expected_tparam_ipdt_delay_value = 0;

/*
 * Create FT Group as per ftv2_group_test_config
 * - d chg BSC_KG_GROUP_TABLE
 * - d chg BSC_DG_GROUP_TABLE
 * - d chg BSC_KG_FLOW_EXCEED_COUNTER
 * - d chg BSC_KG_FLOW_MISSED_COUNTER
 * - d chg BSC_KG_FLOW_AGE_OUT_COUNTER
 * - d chg BSC_KG_FLOW_LEARNED_COUNTER
 * - d chg BSC_DG_FLOW_METER_EXCEED_COUNTER
 */
int
ftv2_group_create_main(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_group_info_t flow_group_info;

    /* Skip this FT group create if no test config set  */
    if (!ftv2_group_test_config) {
        if (!skip_log) {
            printf("[CINT] Step4 - Skip FT Group Create ......\n");
        }
        return rv;
    }

    /* Create FT Group as per test config set */
    switch(ftv2_group_test_config) {
        case 1:
            /* ftv2_group_test_config = 1 */
            /* Create FT Group */
            bcm_flowtracker_group_info_t_init(&flow_group_info);
            flow_group_info.observation_domain_id = ipfix_observation_domain_id1;
            rv = bcm_flowtracker_group_create (unit, 0, &flow_group_id1, &flow_group_info);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_group_create => rv %d(%s)\n",
                                                           tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step4 - FT Group created: %d\n", flow_group_id1);
            }
            break;
        default:
            printf("[CINT] TC %2d - invalid group test config %d\n",tc_id,ftv2_group_test_config);
            return BCM_E_FAIL;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        printf("Step4 - Dump relevant memories & registers .. ..\n");
        bshell(unit, "d chg BSC_KG_GROUP_TABLE");
        bshell(unit, "d chg BSC_DG_GROUP_TABLE");
        bshell(unit, "d chg BSC_KG_FLOW_EXCEED_COUNTER");
        bshell(unit, "d chg BSC_KG_FLOW_MISSED_COUNTER");
        bshell(unit, "d chg BSC_KG_FLOW_AGE_OUT_COUNTER");
        bshell(unit, "d chg BSC_KG_FLOW_LEARNED_COUNTER");
        bshell(unit, "d chg BSC_DG_FLOW_METER_EXCEED_COUNTER");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step4 - FT Group Created Succesfully.\n");
    }

    return BCM_E_NONE;
}

/* Destroy FT Groups */
int
ftv2_group_destroy_main(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip this FT group Destroy if no test config set */
    if (!ftv2_group_test_config) {
        if (!skip_log) {
            printf("[CINT] Cleanup - Skip FT Group Destroy ......\n");
        }
        return rv;
    }

    /* Destroy FT Group as per test config set */
    if (flow_group_id1 != -1) {

        /* Set Flow Limit to 0 first before destroying */
        rv = bcm_flowtracker_group_flow_limit_set(unit, flow_group_id1, 0);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_flowtracker_group_flow_limit_set => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - FT Group %d flow limit set to 0\n", flow_group_id1);
        }

        /* Destroy FT Group */
        rv = bcm_flowtracker_group_destroy(unit, flow_group_id1);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_flowtracker_group_destroy => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - FT Group Destroyed: %d\n", flow_group_id1);
        }
        flow_group_id1 = -1;
        actual_num_tracking_params1 = 0;
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Cleanup - FT Group Destroyed Succesfully.\n");
    }

    return BCM_E_NONE;
}

/*
 * Set Flow Limit to a FT Group
 * BSC_KG_FLOW_EXCEED_PROFILE - 64 profile entries - indexed by profile idx in group table.
 *                            - DO_NOT_LEARN_WHEN_FIFO_FULL (16) - Do not Learn when FIFO is full
 *                            - MAX_FLOW_LIMIT (0-15) - Max Flow Limit
 * BSC_KG_GROUP_TABLE - FLOW_EXCEED_PROFILE_IDXf (11-6)
 * Software - FT_FLOW_LIMIT - _bcm_common_bk_info.flow_limit_profile
 *          - Profiles created in _bcm_common_init during init time
 */
int
ftv2_group_flow_limit_set(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip this flow limit set if no test config set */
    if (!ftv2_group_flow_limit_test_config) {
        if (!skip_log) {
            printf("[CINT] Step5 - Skip Flow Limit Set ......\n");
        }
        return rv;
    }

    /* Set FT Group flow limit as per test config set */
    switch(ftv2_group_flow_limit_test_config) {
        case 1:
            /* ftv2_group_flow_limit_test_config = 1 */
            /* Set FT Group flow limit */
            rv = bcm_flowtracker_group_flow_limit_set(unit, flow_group_id1, flow_limit_set1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_group_flow_limit_set => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step5 - FT Group Flow Limit set to: %d\n", flow_limit_set1);
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid flow limit test config %d\n", tc_id, ftv2_group_flow_limit_test_config);
            return BCM_E_FAIL;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        bshell(unit, "d chg BSC_KG_FLOW_EXCEED_PROFILE");
        bshell(unit, "d chg BSC_KG_GROUP_TABLE");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step5 - Flow Limit Set Succesfully.\n");
    }

    return BCM_E_NONE;
}

/* Set Age Timer to a FT Group
 * BSC_KG_AGE_OUT_PROFILE - 64 profile entries - indexed by profile idx in group table.
 *                        - EXPORT_FLUSH_EN (7) - Enables the exporting of the records that are flushed by HW using FT_FLUSH register 0 = Disable, 1 = Enable
 *                        - EXPORT_AGE_OUT (6) - Enable Export to Export FIFO when a Flow ages out 0 = Disable, 1 = Enable
 *                        - AGE_OUT_ENABLE (5) - Maximum Age Out Time. Each bit is worth 100 ms
 *                        - DO_NOT_AGE_WHEN_FIFO_FULL (4)  - Do not Age when FIFO is full
 *                        - AGE_OUT_INTERVAL (3-0) - Age Out Interval
 * BSC_KG_GROUP_TABLE - AGE_OUT_PROFILE_IDXf (5-0)
 * Software - FT_AGE_OUT - _bcm_common_bk_info.age_out_profile
 *          - Profiles created in _bcm_common_init during init time
 */
int
ftv2_group_age_timer_set(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip age timer set if no test config set */
    if (!ftv2_group_ageing_timer_test_config) {
        if (!skip_log) {
            printf("[CINT] Step6 - Skip Age Timer Set ......\n");
        }
        return rv;
    }

    /* Set Ageing Timer as per test config set */
    switch(ftv2_group_ageing_timer_test_config) {
        case 1:
            /* ftv2_group_ageing_timer_test_config = 1 */
            /* set ageing timer */
            rv = bcm_flowtracker_group_age_timer_set(unit, flow_group_id1, ageing_interval_ms1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_group_age_timer_set => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step6 - FT Group Ageing Time set to (in ms): %d\n", ageing_interval_ms1);
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid ageing timer test config %d\n", tc_id, ftv2_group_ageing_timer_test_config);
            return BCM_E_FAIL;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        bshell(unit, "d chg BSC_KG_AGE_OUT_PROFILE");
        bshell(unit, "d chg BSC_KG_GROUP_TABLE");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step6 - FT Group Age Timer Set Succesfully.\n");
    }

    return BCM_E_NONE;
}

/*
 * Set Flow Tracker collector copy profile to a FT Group
 * BSC_DG_GROUP_COPY_TO_COLLECTOR_PROFILE - 64 profile entries - indexed by profile idx in group table.
 *                                        - SAMPLE_COPY_COUNT_ENABLE (28) - Initial Copy Count Enable
 *                                        - INITIAL_COPY_COUNT_ENABLE (27)  - Initial Copy Count Enable
 *                                        - MAX_COUNT (26-17)  - Configurable Maximum number of initial packets to be forwarded to CPU or remote CPU
 *                                        - SAMPLE_UPPER_LIMIT (16-1) - If PACKET_SAMPLING_METHOD = 1, gives Upper Limit range for the random number used to sample packet as follows:
 *                                                                    - Lower 6b of 16b are Count and Next 3b are Shift amount to be applied to the count.
 *                                                                    - Else number represents the frequency of sampling packets. Lower 6b Count cannot be zero.
 *                                        - PACKET_SAMPLING_METHOD (0) - RANDOM = 1 , FIXED = 0
 * BSC_DG_GROUP_TABLE - COPY_TO_COLLECTOR_PROFILE_IDXf (5-0) - Index to Initial Copy to Collector Profile Table for this Group
 */
int
ftv2_group_collector_copy_info_set(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_collector_copy_info_t cinfo1;

    /* Skip this collector copy info set if no test config set */
    if (!ftv2_group_collector_copy_test_config) {
        if (!skip_log) {
            printf("[CINT] Step7 - Skip Collector Copy Info Set ......\n");
        }
        return rv;
    }

    /* Set Collector Copy info as per test config set */
    switch(ftv2_group_collector_copy_test_config) {
        case 1:
            /* ftv2_group_collector_copy_test_config = 1 */
            /* Set Collector Copy Info */
            bcm_flowtracker_collector_copy_info_t_init(&cinfo1);
            cinfo1.num_pkts_initial_samples = initial_samples1;
            rv = bcm_flowtracker_group_collector_copy_info_set(unit, flow_group_id1, cinfo1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_group_collector_copy_info_set => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step7 - FT Group Collector Copy Info set for FT Group %d\n", flow_group_id1);
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid collector copy test config %d\n", tc_id, ftv2_group_collector_copy_test_config);
            return BCM_E_FAIL;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        bshell(unit, "d chg BSC_DG_GROUP_COPY_TO_COLLECTOR_PROFILE");
        bshell(unit, "d chg BSC_DG_GROUP_TABLE");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step7 - FT Group Collector Copy Info Set Succesfully.\n");
    }

    return BCM_E_NONE;
}

/*
 * Set Flow Tracker CPU notification to a FT group
 */
int
ftv2_group_cpu_notification_set(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_cpu_notification_info_t cpu_notification_info1;

    /* Skip cpu notification if no test config set */
    if (!ftv2_group_cpu_notification_test_config ) {
        if (!skip_log) {
            printf("[CINT] Step8 - Skip CPU Notification Set ......\n");
        }
        return rv;
    }

    /* Create CPU notification as per test config set */
    switch(ftv2_group_cpu_notification_test_config) {
        case 1:
            /* ftv2_group_cpu_notification_test_config = 1 */
            /* Set CPU notification */
            bcm_flowtracker_cpu_notification_info_t_init(&cpu_notification_info1);
            BCM_FLOWTRACKER_CPU_NOTIFICATION_SET(cpu_notification_info1, bcmFlowtrackerCpuNotificationFlowExceeded);
            rv = bcm_flowtracker_group_cpu_notification_set(unit, flow_group_id1, &cpu_notification_info1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_group_cpu_notification_set => rv %d(%s)\n",
                                                                         tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step8 - FT Group CPU notification for FT group %d\n",flow_group_id1);
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid cpu notification test config %d\n", tc_id, ftv2_group_cpu_notification_test_config);
            return BCM_E_FAIL;
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step8 - FT Group CPU Notification Set Succesfully.\n");
    }

    return BCM_E_NONE;
}

/*
 * Set Flow Tracker control to a FT group
 * bcmFlowtrackerGroupControlNewLearn
 * bcmFlowtrackerGroupControlFlowTrack
 * bcmFlowtrackerGroupControlFlowDirection
 * bcmFlowtrackerGroupControlPeriodicExport
 */
int
ftv2_group_control_set(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip this group control if no test config set */
    if (!ftv2_group_control_test_config) {
        if (!skip_log) {
            printf("[CINT] Step9 - Skip Group Control Set ......\n");
        }
        return rv;
    }

    /* Set group control as per test config set */
    switch(ftv2_group_control_test_config) {
        case 1:
            /* ftv2_group_control_test_config = 1 */
            /* Set FT Group control */
            rv = bcm_flowtracker_group_control_set(unit, flow_group_id1, bcmFlowtrackerGroupControlNewLearnEnable, 1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_group_control_set => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step9 - FT Group Control <NewLearn> set for FT group %d\n",flow_group_id1);
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid group control test config %d\n", tc_id, ftv2_group_control_test_config);
            return BCM_E_FAIL;
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step9 - FT Group Control Set Succesfully.\n");
    }

    return BCM_E_NONE;
}

/*
 * Set Flow Group Meter Profile info
 * BSC_DG_GROUP_METER_PROFILE - 64 profile entries - indexed by profile idx in group table.
 *                                        - REFRESHMAX (39-35) - Maximum number of tokens that can be added to a bucket.
 *                                        - METER_GRAN (34-32)  - Defines metering rate and granularity
 *                                        - BUCKETSIZE (31-20)  - Bucket size of meter
 *                                        - PKTS_BYTES (19) - This bit controls whether packet quanta or bytes are subtracted from the bucket count
 *                                        - REFRESHCOUNT (18-0) - Refresh count for meter.The number of tokens added to the bucket at each referesh intreval
 * BSC_DG_GROUP_TABLE - GROUP_METER_PROFILE_IDXf (18-23) - Index to Group Meter Profile Table for this Group
 */
int
ftv2_group_meter_set(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_meter_info_t meter_profile_info1;

    /* Skip this meter set if no test config set */
    if (!ftv2_group_meter_test_config) {
        if (!skip_log) {
            printf("[CINT] Step10 - Skip Group Meter Set ......\n");
        }
        return rv;
    }

    /* Set meter as per test config set */
    switch(ftv2_group_meter_test_config) {
        case 1:
            /* ftv2_group_meter_test_config = 1 */
            /* Set Meter info on FT Group */
            bcm_flowtracker_meter_info_t_init(&meter_profile_info1);
            rv = bcm_flowtracker_group_meter_info_set(unit, flow_group_id1, meter_profile_info1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_group_meter_info_set => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step10 - FT Group Meter Profile set for FT group %d\n",flow_group_id1);
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid group meter test config %d\n", tc_id, ftv2_group_meter_test_config);
            return BCM_E_FAIL;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        bshell(unit, "d chg BSC_DG_GROUP_METER_PROFILE");
        bshell(unit, "d chg BSC_DG_GROUP_TABLE");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step10 - FT Group Meter Set Succesfully.\n");
    }

    return BCM_E_NONE;
}

/*
 * Set Flow Tracker tracking params to a FT Group
 */
int
ftv2_group_tracking_params_set(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    int i = 0, j = 0, num_key_params = 0, num_normal_params = 0;
    bcm_flowtracker_tracking_param_type_t key_param[max_num_tracking_params] = { 0 };
    bcm_flowtracker_tracking_param_type_t normal_param[max_num_tracking_params] = { 0 };
    bcm_flowtracker_timestamp_source_t src_ts[max_num_tracking_params] = { 0 };
    bcm_flowtracker_check_t check_id[max_num_tracking_params] = { 0 };

    /* Skip this tracking params set if no test config set */
    if (!ftv2_group_tracking_param_test_config) {
        if (!skip_log) {
            printf("[CINT] Step14 - Skip Tracking Param Set ......\n");
        }
        return rv;
    }

    /* Initialise tracking info structure */
    for (i = 0; i < max_num_tracking_params; i++)
    {
        tracking_info1[i].flags = 0;
        tracking_info1[i].param = 0;
        tracking_info1[i].src_ts = 0;
        tracking_info1[i].check_id = 0;
    }

    /* First fill key param as per test config */
    /* Remember Me - Modify key tracking param as per config */
    switch(ftv2_group_tracking_param_test_config) {
        case 2:
            /* ftv2_group_tracking_param_test_config = 2 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 3:
            /* ftv2_group_tracking_param_test_config = 3 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 4:
            /* ftv2_group_tracking_param_test_config = 4 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 5:
            /* ftv2_group_tracking_param_test_config = 5 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 6:
            /* ftv2_group_tracking_param_test_config = 6 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 7:
            /* ftv2_group_tracking_param_test_config = 7 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 8:
            /* ftv2_group_tracking_param_test_config = 8 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 9:
            /* ftv2_group_tracking_param_test_config = 9 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 10:
            /* ftv2_group_tracking_param_test_config = 10 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 11:
            /* ftv2_group_tracking_param_test_config = 11 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 12:
            /* ftv2_group_tracking_param_test_config = 12 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 13:
            /* ftv2_group_tracking_param_test_config = 13 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 14:
            /* ftv2_group_tracking_param_test_config = 14 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 15:
            /* ftv2_group_tracking_param_test_config = 15 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 16:
            /* ftv2_group_tracking_param_test_config = 16 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 17:
            /* ftv2_group_tracking_param_test_config = 17 */
            /* IPv4 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 80:
            /* ftv2_group_tracking_param_test_config = 80 */
            /* IPv6 TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv6;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv6;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        case 90:
            /* ftv2_group_tracking_param_test_config = 90 */
            /* VXLAN TC's Key */
            num_key_params = 5;
            key_param[0] = bcmFlowtrackerTrackingParamTypeSrcIPv4;
            key_param[1] = bcmFlowtrackerTrackingParamTypeDstIPv4;
            key_param[2] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            key_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            key_param[4] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            break;

        default:
            printf("[CINT] TC %2d - invalid tracking param test config %d\n", tc_id, ftv2_group_tracking_param_test_config);
            return BCM_E_FAIL;
    }

    /* Now fill normal params as per test config */
    /* Remember Me - Modify normal param as per config */
    switch(ftv2_group_tracking_param_test_config) {
        case 2:
            /* ftv2_group_tracking_param_test_config = 2 */
            num_normal_params = 5;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeTTL;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeByteCount;
            break;

        case 3:
            /* ftv2_group_tracking_param_test_config = 3 */
            num_normal_params = 6;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeTTL;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
            break;

        case 4:
            /* ftv2_group_tracking_param_test_config = 4 */
            num_normal_params = 8;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeTTL;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
            normal_param[6] = bcmFlowtrackerTrackingParamTypeIngPort;
            normal_param[7] = bcmFlowtrackerTrackingParamTypeEgrPort;
            break;

        case 5:
            /* ftv2_group_tracking_param_test_config = 5 */
            num_normal_params = 8;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeTTL;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
            normal_param[6] = bcmFlowtrackerTrackingParamTypeIngPort;
            normal_param[7] = bcmFlowtrackerTrackingParamTypeEgrPort;
            break;

        case 6:
            /* ftv2_group_tracking_param_test_config = 6 */
            num_normal_params = 5;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeTTL;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeByteCount;
            break;

        case 7:
            /* ftv2_group_tracking_param_test_config = 7 */
            num_normal_params = 9;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeTTL;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
            normal_param[6] = bcmFlowtrackerTrackingParamTypeIngPort;
            normal_param[7] = bcmFlowtrackerTrackingParamTypeEgrPort;
            normal_param[8] = bcmFlowtrackerTrackingParamTypeChipDelay;
            src_ts[8] = bcmFlowtrackerTimestampSourceCMIC;
            break;

        case 8:
            /* ftv2_group_tracking_param_test_config = 8 */
            num_normal_params = 9;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeTTL;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
            normal_param[6] = bcmFlowtrackerTrackingParamTypeIngPort;
            normal_param[7] = bcmFlowtrackerTrackingParamTypeEgrPort;
            normal_param[8] = bcmFlowtrackerTrackingParamTypeIPATDelay;
            src_ts[8] = bcmFlowtrackerTimestampSourceCMIC;
            break;

        case 9:
            /* ftv2_group_tracking_param_test_config = 9 */
            num_normal_params = 8;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeTTL;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeIngPort;
            normal_param[6] = bcmFlowtrackerTrackingParamTypeEgrPort;
            normal_param[7] = bcmFlowtrackerTrackingParamTypeFlowtrackerCheck;
            check_id[7] = pri_check_id1[0];
            break;

        case 10:
            /* ftv2_group_tracking_param_test_config = 10 */
            num_normal_params = 7;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeTTL;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeL4DstPort;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2;
            src_ts[5] = bcmFlowtrackerTimestampSourcePTP;
            normal_param[6] = bcmFlowtrackerTrackingParamTypeFlowtrackerCheck;
            check_id[6] = pri_check_id1[0];
            break;

        case 11:
            /* ftv2_group_tracking_param_test_config = 11 */
            num_normal_params = 6;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeIPLength;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3;
            src_ts[4] = bcmFlowtrackerTimestampSourceNTP;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeFlowtrackerCheck;
            check_id[5] = pri_check_id1[0];
            break;

        case 12:
            /* ftv2_group_tracking_param_test_config = 12 */
            num_normal_params = 8;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeTTL;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeIngPort;
            normal_param[6] = bcmFlowtrackerTrackingParamTypeEgrPort;
            normal_param[7] = bcmFlowtrackerTrackingParamTypeIPDTDelay;
            src_ts[7] = bcmFlowtrackerTimestampSourceCMIC;
            break;

        case 13:
            /* ftv2_group_tracking_param_test_config = 13 */
            num_normal_params = 7;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeIPLength;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
            src_ts[5] = bcmFlowtrackerTimestampSourceNTP;
            normal_param[6] = bcmFlowtrackerTrackingParamTypeFlowtrackerCheck;
            check_id[6] = pri_check_id1[0];
            break;

        case 14:
            /* ftv2_group_tracking_param_test_config = 14 */
            num_normal_params = 7;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeIPLength;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
            src_ts[5] = bcmFlowtrackerTimestampSourceNTP;
            normal_param[6] = bcmFlowtrackerTrackingParamTypeFlowtrackerCheck;
            check_id[6] = pri_check_id1[0];
            break;

        case 15:
            /* ftv2_group_tracking_param_test_config = 15 */
            num_normal_params = 6;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeIPLength;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
            src_ts[4] = bcmFlowtrackerTimestampSourceCMIC;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeFlowtrackerCheck;
            check_id[5] = pri_check_id1[0];
            break;

        case 16:
            /* ftv2_group_tracking_param_test_config = 16 */
            num_normal_params = 7;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeIPLength;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
            src_ts[5] = bcmFlowtrackerTimestampSourcePTP;
            normal_param[6] = bcmFlowtrackerTrackingParamTypeFlowtrackerCheck;
            check_id[6] = pri_check_id1[0];
            break;

        case 17:
            /* ftv2_group_tracking_param_test_config = 17 */
            num_normal_params = 7;
            normal_param[0] = bcmFlowtrackerTrackingParamTypeIPProtocol;
            normal_param[1] = bcmFlowtrackerTrackingParamTypePktCount;
            normal_param[2] = bcmFlowtrackerTrackingParamTypeByteCount;
            normal_param[3] = bcmFlowtrackerTrackingParamTypeIPLength;
            normal_param[4] = bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector;
            normal_param[5] = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
            src_ts[5] = bcmFlowtrackerTimestampSourcePTP;
            normal_param[6] = bcmFlowtrackerTrackingParamTypeFlowtrackerCheck;
            check_id[6] = pri_check_id1[0];
            break;

        case 80:
            /* ftv2_group_tracking_param_test_config = 80 */
            /* IPv6 TC Config */
            num_normal_params = 1;
            normal_param[0] = bcmFlowtrackerTrackingParamTypePktCount;
            break;

        case 90:
            /* ftv2_group_tracking_param_test_config = 90 */
            /* Vxlan TC Config */
            num_normal_params = 1;
            normal_param[0] = bcmFlowtrackerTrackingParamTypePktCount;
            break;

        default:
            printf("[CINT] TC %2d - invalid tracking param test config %d\n", tc_id, ftv2_group_tracking_param_test_config);
            return BCM_E_FAIL;
    }

    /* Fill Tracking info in local structure */
    for (i = 0; i < num_key_params; i++) {
        tracking_info1[i].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
        tracking_info1[i].param = key_param[i];
    }
    for (; i < (num_key_params + num_normal_params); i++) {
        tracking_info1[i].flags = 0;
        tracking_info1[i].param = normal_param[i - num_key_params];
        tracking_info1[i].src_ts = src_ts[i - num_key_params];
        tracking_info1[i].check_id = check_id[i - num_key_params];
    }
    actual_num_tracking_params1 = num_key_params + num_normal_params;

    /* Set Tracking info to FT Group */
    rv = bcm_flowtracker_group_tracking_params_set(unit, flow_group_id1, actual_num_tracking_params1, tracking_info1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - bcm_flowtracker_group_tracking_params_set => rv %d(%s)\n",
                                                                tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    if (!skip_log) {
        printf("[CINT] Step14 - FT Group tracking params %d actual added to FT group %d.\n",
                                                  actual_num_tracking_params1,flow_group_id1);
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step14 - FT Group Tracking Params Set Succesfully.\n");
    }

    return BCM_E_NONE;
}

/*
 * Clear all flows in a FT Group
 */
int
ftv2_group_flows_clear(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    char str[256];

    /* Skip this FT group flow clear if no test config set  */
    if (!ftv2_group_test_config) {
        if (!skip_log) {
            printf("[CINT] Cleanup - Skip group flow clear ......\n");
        }
        return rv;
    }

    /* Clear all flows in a FT Group */
    if (flow_group_id1 != -1) {

        /* Use hardware to clear flows in a group */
        rv = bcm_flowtracker_group_clear(unit, flow_group_id1, BCM_FLOWTRACKER_GROUP_CLEAR_ALL);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_flowtracker_group_clear => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
            return rv;
        }

        /* In Sim hardware flow clear does not work , hence clear memory by writing 0 */
        if (bcmsim_ftv2_enabled) {
            sprintf(str, "w ft_key_double 0 %d 0 0 0 0 0 0 0 0 0 0 0 0", ftv2_key_double_entry_count);
            bshell(unit, str);
            sprintf(str, "w bsc_dt_flex_session_data_double 0 %d 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0", ftv2_key_double_entry_count);
            bshell(unit, str);
        }

        if (!skip_log) {
            printf("[CINT] Cleanup - Clear all flows in FT Group id %d.\n",flow_group_id1);
        }
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        bshell(unit, "d chg FT_KEY_SINGLE");
        bshell(unit, "d chg FT_KEY_DOUBLE");
        bshell(unit, "d chg BSC_DT_FLEX_SESSION_DATA_SINGLE");
        bshell(unit, "d chg BSC_DT_FLEX_SESSION_DATA_DOUBLE");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Cleanup - All Flows in Group Cleared Succesfully.\n");
    }

    return BCM_E_NONE;
}

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * C file to validate results of test
 */

/* Validate Variables */
uint8 ftv2_validate_test_config = 0;

/* delay before validation */
uint8 sleep_before_validation = 45;

/*
 *   Get FT Group Stat
 * - d chg BSC_KG_FLOW_EXCEED_COUNTER
 * - d chg BSC_KG_FLOW_MISSED_COUNTER
 * - d chg BSC_KG_FLOW_AGE_OUT_COUNTER
 * - d chg BSC_KG_FLOW_LEARNED_COUNTER
 * - d chg BSC_DG_FLOW_METER_EXCEED_COUNTER
 */
int ftv2_group_stat_get(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    uint8 flow_cnt_check_pass = 0;

    /* Get Group Stat */
    bcm_flowtracker_group_stat_t_init(&group_stats1);
    rv = bcm_flowtracker_group_stat_get(unit, flow_group_id1, &group_stats1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - bcm_flowtracker_group_stat_get => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    if (COMPILER_64_LO(group_stats1.flow_learnt_count) == expected_group_property_flow_count) {
        flow_cnt_check_pass = 1;
    }

    /* If group count fails then dump log always and return fail */
    if (!flow_cnt_check_pass) {
        printf("[CINT] Validation - Flow Learnt Count is %d whereas expected is %d. Validation failed !\n",
                                                            COMPILER_64_LO(group_stats1.flow_learnt_count),
                                                                        expected_group_property_flow_count);
        return (BCM_E_FAIL);
    }

    /* This validation can be run for both ftrmon as well as callback utility monitoring */
    if (!skip_log) {
        printf("\n[CINT] Validation - Observe FT Group Counters should increment\n");
        printf("###############################################################################################################\n");
        printf("[CINT] Validation - Dumping Group Counters ......\n");
        printf("[CINT] Validation - flow_count - exceeded 0x%x%x, missed 0x%x%x, aged_out 0x%x%x, learnt 0x%x%x, meter_exceeded 0x%x%x\n",
                COMPILER_64_HI(group_stats1.flow_exceeded_count),COMPILER_64_LO(group_stats1.flow_exceeded_count),
                COMPILER_64_HI(group_stats1.flow_missed_count),COMPILER_64_LO(group_stats1.flow_missed_count),
                COMPILER_64_HI(group_stats1.flow_aged_out_count),COMPILER_64_LO(group_stats1.flow_aged_out_count),
                COMPILER_64_HI(group_stats1.flow_learnt_count), COMPILER_64_LO(group_stats1.flow_learnt_count),
                COMPILER_64_HI(group_stats1.flow_meter_exceeded_count), COMPILER_64_LO(group_stats1.flow_meter_exceeded_count));
        printf("[CINT] Validation - Flow Learnt Count is %d as expected.\n",
                                                                COMPILER_64_LO(group_stats1.flow_learnt_count));
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        printf("[CINT] Validation - Dumping relevant memories & registers ..\n");
        bshell(unit, "d chg BSC_KG_FLOW_EXCEED_COUNTER");
        bshell(unit, "d chg BSC_KG_FLOW_MISSED_COUNTER");
        bshell(unit, "d chg BSC_KG_FLOW_AGE_OUT_COUNTER");
        bshell(unit, "d chg BSC_KG_FLOW_LEARNED_COUNTER");
        bshell(unit, "d chg BSC_DG_FLOW_METER_EXCEED_COUNTER");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf("###############################################################################################################\n");
    }

    return BCM_E_NONE;
}

int ftv2_group_ipfix_stat_get(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    if (use_ftrmon_thread) {
        if (!skip_log) {
            /* Dump ipfix stat */
            printf("\n[CINT] Validation - Observe IPFIX Stats should increment\n");
            printf("##################################################################################################################################\n");
            bshell(unit, "d chg bsc_ipfix_stats");
            printf("##################################################################################################################################\n");
        }
    }

    /* Needed to send export packets in BCMSIM, run always */
    if (bcmsim_ftv2_enabled) {
        if (!skip_log) {
            printf("[CINT] Validation - For SIM, trigger export packet by setting EXPORT_ENABLE in register BSC_EX_EXPORT_TRIGGER\n");
        }
        bshell(unit, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
    }

    return BCM_E_NONE;
}

int ftv2_group_export_reason_get(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /*  Dump This only of ftrmon task is used for monitoring, otherwise packet monitoring call back will validate */
    if (use_ftrmon_thread) {
        if (!skip_log) {
            /* Dump export reasons */
            printf("\n[CINT] Validation - Validate Export Reason Coming in Packet.\n");
            printf("##################################################################################################################################\n");
            printf("Bit0(0x01) - Export Periodic,                      Bit1(0x02) - Export Group Flush,                  Bit2(0x04) - Export Age Out,\n");
            printf("Bit3(0x08) - Export New Flow Learnt,               Bit4(0x10) - Export when state transition occur,\n");
            printf("Bit5(0x20) - Export ALU32 thresold check triggers, Bit6(0x40) - Export ALU16 thresold check triggers\n");
            printf("###################################################################################################################\n");
        }
    }

    return BCM_E_NONE;
}

int ftv2_group_pw_validation(int tc_id, uint32 debug_flags)
{
    int rv_validate = BCM_E_NONE;

    /*  Dump this only if packet monitoring call back is used to validate */
    if (!use_ftrmon_thread) {

        /* Sleeping as packet watcher utility will receive export packet, parse and validate it
         * and set variable packet_validation_failed accordingly. This delay is to allow it
         * do so */
        if (!skip_log) {
            printf("[CINT] Validation - sleeping for %d seconds before validation ..\n",sleep_before_validation);
        }
        sal_sleep(sleep_before_validation);

        if (packet_validation_failed) {
            rv_validate = BCM_E_FAIL;
        }
    }

    return rv_validate;
}

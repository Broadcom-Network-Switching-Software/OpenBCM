/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * C file to put timestamp related api calls
 */

/* Time Related Global Variables */
uint8 ftv2_system_time_test_config = 0;

/* Expected TimeOfDay Value */
uint8 expected_tod_value[6] = {0};

/* Expected Export Packet TimeOfDay Value */
uint8 expected_export_tod_value[6] = {0};

/* Set time in system.
 * source_type - 0 : legacy
 * source_type - 1: NTP
 * source_type - 2: PTP
 * source_type - 3: CMIC
 * source_type - 4: MAC
 * Calls dummy registers for bcmsim time settings.
 */
int
ftv2_time_set(int tc_id, uint8 source_type, uint32 val_lo, uint32 val_hi)
{
    int rv = BCM_E_NONE;
    bcm_time_tod_t tod;
    unsigned int stages;
    char str[200];

    if (source_type == 0) {

        /* Legacy Time Setting - Using bcm_time_tod_set */
        bcm_time_tod_t_init(&tod);
        stages = BCM_TIME_STAGE_ALL;
        tod.time_format = bcmTimeFormatPTP;
        tod.ts.nanoseconds = val_lo;
        COMPILER_64_SET(tod.ts.seconds, 0, val_hi);
        COMPILER_64_SET(tod.ts_adjustment_counter_ns, 0, 0);
        rv = bcm_time_tod_set(unit, stages, &tod);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_time_tod_set => rv %d(%s)\n",
                                           tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Step18 - Legacy Sytem time set to val_hi 0x%08x, val_lo 0x%08x\n", val_hi, val_lo);
        }

    } else if (source_type == 1) {

        /* NTP Time */
        if (bcmsim_ftv2_enabled) {
            sprintf(str, "s NS_NTP_TOD_VALUE_0 VAL=%d", val_lo);
            bshell(unit, str);
            sprintf(str, "s NS_NTP_TOD_VALUE_1 VAL=%d", val_hi);
            bshell(unit, str);
            if (!skip_log) {
                printf("[CINT] Step18 - NTP Time set in dummy sim registers to val_hi 0x%08x, val_lo 0x%08x\n", val_hi, val_lo);
            }

        } else {
            /* In actual hw, set values using time api */
        }

    } else if (source_type == 2) {

        /* PTP Time */
        if (bcmsim_ftv2_enabled) {
            sprintf(str, "s NS_PTP_TOD_A_VALUE_0 VAL=%d", val_lo);
            bshell(unit, str);
            sprintf(str, "s NS_PTP_TOD_A_VALUE_1 VAL=%d", val_hi);
            bshell(unit, str);
            if (!skip_log) {
                printf("[CINT] Step18 - PTP Time set in dummy sim registers to val_hi 0x%08x, val_lo 0x%08x\n", val_hi, val_lo);
            }

        } else {
            /* In actual hw, set values using time api */
        }

    } else if (source_type == 3) {

        /* CMIC Time */
        if (bcmsim_ftv2_enabled) {
            sprintf(str, "s NS_TIMESYNC_INPUT_TIME_FIFO1_TS_0 TS0_L=%d", val_lo);
            bshell(unit, str);
            sprintf(str, "s NS_TIMESYNC_INPUT_TIME_FIFO1_TS_1 TS0_U=%d", val_hi);
            bshell(unit, str);
            if (!skip_log) {
                printf("[CINT] Step18 - CMIC Time set in dummy sim registers to val_hi 0x%08x, val_lo 0x%08x\n", val_hi, val_lo);
            }

        } else {
            /* In actual hw, set values using time api */
        }

    } else if (source_type == 4) {

        /* MAC Time */
        if (bcmsim_ftv2_enabled) {
            sprintf(str, "s NS_TIMESYNC_GPIO_0_PHASE_ADJUST_LOWER PHASE_ADJUST=%d", val_lo);
            bshell(unit, str);
            sprintf(str, "s NS_TIMESYNC_INPUT_TIME_FIFO1_TS_1 TS0_U=%d", val_hi);
            bshell(unit, str);
            if (!skip_log) {
                printf("[CINT] Step18 - MAC Time set in dummy sim registers to val_hi 0x%08x, val_lo 0x%08x\n", val_hi, val_lo);
            }

        } else {
            /* In actual hw, set values using time api */
        }

    } else {

        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Set Time Config as per ftv2_system_time_test_config
 */
int
ftv2_system_time_set_main(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip this trigger set if no test config set */
    if (!ftv2_system_time_test_config) {
        if (!skip_log) {
            printf("[CINT] Step18 - Skip System Time Set ......\n");
        }
        return rv;
    }

    /* Set Time */
    switch(ftv2_system_time_test_config) {

        case 3:
            /* ftv2_system_time_test_config = 3 */

            /* Legacy Time */
            rv = ftv2_time_set(tc_id, 0, 0x0a0b0c0d, 0x00201122);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            expected_tod_value[0] = 0x11;
            expected_tod_value[1] = 0x22;
            expected_tod_value[2] = 0x0a;
            expected_tod_value[3] = 0x0b;
            expected_tod_value[4] = 0x0c;
            expected_tod_value[5] = 0x0d;

            expected_export_tod_value[0] = 0x11;
            expected_export_tod_value[1] = 0x22;
            expected_export_tod_value[2] = 0x0a;
            expected_export_tod_value[3] = 0x0b;
            expected_export_tod_value[4] = 0x0c;
            expected_export_tod_value[5] = 0x0d;
            break;

        case 4:
            /* ftv2_system_time_test_config = 4 */

            /* Legacy time */
            rv = ftv2_time_set(tc_id, 0, 0x11223344, 0x00220a0b);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            expected_tod_value[0] = 0x0a;
            expected_tod_value[1] = 0x0b;
            expected_tod_value[2] = 0x11;
            expected_tod_value[3] = 0x22;
            expected_tod_value[4] = 0x33;
            expected_tod_value[5] = 0x44;

            expected_export_tod_value[0] = 0x0a;
            expected_export_tod_value[1] = 0x0b;
            expected_export_tod_value[2] = 0x11;
            expected_export_tod_value[3] = 0x22;
            expected_export_tod_value[4] = 0x33;
            expected_export_tod_value[5] = 0x44;
            break;

        case 5:
            /* ftv2_system_time_test_config = 5 */

            /* Legacy time */
            rv = ftv2_time_set(tc_id, 0, 0x01020304, 0xFFFFFFFF);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            expected_tod_value[0] = 0xFF;
            expected_tod_value[1] = 0xFF;
            expected_tod_value[2] = 0x01;
            expected_tod_value[3] = 0x02;
            expected_tod_value[4] = 0x03;
            expected_tod_value[5] = 0x04;

            expected_export_tod_value[0] = 0xFF;
            expected_export_tod_value[1] = 0xFF;
            expected_export_tod_value[2] = 0x01;
            expected_export_tod_value[3] = 0x02;
            expected_export_tod_value[4] = 0x03;
            expected_export_tod_value[5] = 0x04;
            break;

        case 7:
            /* ftv2_system_time_test_config = 7 */

            /* ChipDelay in simulation should be set in below registers.
             * ChipDelay = (Current Time - MAC Time) + offset) >> gran */
            /* CMIC Time - Current Time */
            rv = ftv2_time_set(tc_id, 3, 0x07070707, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            /* MAC Time - Time captured at packet ingress in MAC */
            rv = ftv2_time_set(tc_id, 4, 0x01230606, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            /* Legacy Time = ToD Set (Sec,Nsec) + Current Time (in Nsec) */
            /* Legacy Time = 0 + 0x07070707 */
            expected_tod_value[0] = 0x00;
            expected_tod_value[1] = 0x00;
            expected_tod_value[2] = 0x07;
            expected_tod_value[3] = 0x07;
            expected_tod_value[4] = 0x07;
            expected_tod_value[5] = 0x07;

            /* Update chip delay expected value if at all added */
            /* ((0x07070707 - 0x01230606) + 768) >> 8 */
            expected_tparam_chip_delay_value = 0x0005E404;

            expected_export_tod_value[0] = 0x00;
            expected_export_tod_value[1] = 0x00;
            expected_export_tod_value[2] = 0x07;
            expected_export_tod_value[3] = 0x07;
            expected_export_tod_value[4] = 0x07;
            expected_export_tod_value[5] = 0x07;
            break;

        case 8:
            /* ftv2_system_time_test_config = 8 */
            /* IPATDelay in simulation should be set in below registers */
            /* IPAT Delay = (Current Time - Previous packet stored timestamp) + offset) >> gran */

            /* CMIC Time - Current Time */
            rv = ftv2_time_set(tc_id, 3, 0x08070705, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            /* timeset is updated before sending last packet in tx step,
             * hence updating tod expected to latest value. */
            expected_tod_value[0] = 0x00;
            expected_tod_value[1] = 0x00;
            expected_tod_value[2] = 0x09;
            expected_tod_value[3] = 0x08;
            expected_tod_value[4] = 0x08;
            expected_tod_value[5] = 0x06;

            /* Update ipat delay expected value if at all added */
            /* ((0x09080806 - 0x08070705) + 7) >> 0 */
            expected_tparam_ipat_delay_value = 0x01010108;

            expected_export_tod_value[0] = 0x00;
            expected_export_tod_value[1] = 0x00;
            expected_export_tod_value[2] = 0x09;
            expected_export_tod_value[3] = 0x08;
            expected_export_tod_value[4] = 0x08;
            expected_export_tod_value[5] = 0x06;
            break;

        case 9:
            /* ftv2_system_time_test_config = 9 */
            /* Egress Delay to calculate chip delay equivalent */

            /* CMIC Time - Current Time */
            rv = ftv2_time_set(tc_id, 3, 0x0a0a0a0a, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                     tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            /* MAC Time - Time captured at packet ingress in MAC */
            rv = ftv2_time_set(tc_id, 4, 0x01230606, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                     tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            expected_tod_value[0] = 0x00;
            expected_tod_value[1] = 0x00;
            expected_tod_value[2] = 0x00;
            expected_tod_value[3] = 0x00;
            expected_tod_value[4] = 0x00;
            expected_tod_value[5] = 0x00;

            expected_export_tod_value[0] = 0x00;
            expected_export_tod_value[1] = 0x00;
            expected_export_tod_value[2] = 0x0a;
            expected_export_tod_value[3] = 0x0a;
            expected_export_tod_value[4] = 0x0a;
            expected_export_tod_value[5] = 0x0a;
            break;

        case 10:
            /* ftv2_system_time_test_config = 10 */

            /* PTP Time - Current Time */
            rv = ftv2_time_set(tc_id, 2, 0x010a020b, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                     tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            expected_tod_value[0] = 0x00;
            expected_tod_value[1] = 0x00;
            expected_tod_value[2] = 0x01;
            expected_tod_value[3] = 0x0a;
            expected_tod_value[4] = 0x02;
            expected_tod_value[5] = 0x0b;

            /* Collector Src TS is set to PTP */
            expected_export_tod_value[0] = 0x00;
            expected_export_tod_value[1] = 0x00;
            expected_export_tod_value[2] = 0x01;
            expected_export_tod_value[3] = 0x0a;
            expected_export_tod_value[4] = 0x02;
            expected_export_tod_value[5] = 0x0b;
            break;

        case 11:
            /* ftv2_system_time_test_config = 11 */

            /* NTP Time - Current Time */
            rv = ftv2_time_set(tc_id, 1, 0x33456789, 0x00001122);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            expected_tod_value[0] = 0x11;
            expected_tod_value[1] = 0x22;
            expected_tod_value[2] = 0x33;
            expected_tod_value[3] = 0x45;
            expected_tod_value[4] = 0x67;
            expected_tod_value[5] = 0x89;

            /* Collector Src TS is set to NTP */
            expected_export_tod_value[0] = 0x11;
            expected_export_tod_value[1] = 0x22;
            expected_export_tod_value[2] = 0x33;
            expected_export_tod_value[3] = 0x45;
            expected_export_tod_value[4] = 0x67;
            expected_export_tod_value[5] = 0x89;
            break;

        case 12:
            /* ftv2_system_time_test_config = 12 */
            /* IPDTDelay in simulation should be set in below registers */
            /* IPDT Delay = (Current Time - Previous packet stored timestamp) + offset) >> gran */

            /* CMIC Time - Current Time */
            rv = ftv2_time_set(tc_id, 3, 0x08070705, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            /* timeset is updated before sending last packet in tx step,
             * hence updating tod expected to latest value. */
            expected_tod_value[0] = 0x00;
            expected_tod_value[1] = 0x00;
            expected_tod_value[2] = 0x09;
            expected_tod_value[3] = 0x08;
            expected_tod_value[4] = 0x08;
            expected_tod_value[5] = 0x06;

            /* Update ipdt delay expected value if at all added */
            /* ((0x09080806 - 0x08070705) + 15) >> 8 */
            expected_tparam_ipdt_delay_value = 0x00010101;

            expected_export_tod_value[0] = 0x00;
            expected_export_tod_value[1] = 0x00;
            expected_export_tod_value[2] = 0x09;
            expected_export_tod_value[3] = 0x08;
            expected_export_tod_value[4] = 0x08;
            expected_export_tod_value[5] = 0x06;
            break;

        case 13:
            /* ftv2_system_time_test_config = 13 */

            /* NTP Time - Current Time */
            rv = ftv2_time_set(tc_id, 1, 0x03406889, 0x00001002);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                     tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            expected_tod_value[0] = 0x10;
            expected_tod_value[1] = 0x02;
            expected_tod_value[2] = 0x03;
            expected_tod_value[3] = 0x40;
            expected_tod_value[4] = 0x68;
            expected_tod_value[5] = 0x89;

            expected_export_tod_value[0] = 0x00;
            expected_export_tod_value[1] = 0x00;
            expected_export_tod_value[2] = 0x00;
            expected_export_tod_value[3] = 0x00;
            expected_export_tod_value[4] = 0x00;
            expected_export_tod_value[5] = 0x00;
            break;

        case 14:
            /* ftv2_system_time_test_config = 14 */

            /* NTP Time - Current Time */
            rv = ftv2_time_set(tc_id, 1, 0x22223333, 0x00001111);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            expected_tod_value[0] = 0x11;
            expected_tod_value[1] = 0x11;
            expected_tod_value[2] = 0x22;
            expected_tod_value[3] = 0x22;
            expected_tod_value[4] = 0x33;
            expected_tod_value[5] = 0x33;

            expected_export_tod_value[0] = 0x00;
            expected_export_tod_value[1] = 0x00;
            expected_export_tod_value[2] = 0x00;
            expected_export_tod_value[3] = 0x00;
            expected_export_tod_value[4] = 0x00;
            expected_export_tod_value[5] = 0x00;
            break;

        case 15:
            /* ftv2_system_time_test_config = 15 */

            /* CMIC Time - Current Time */
            rv = ftv2_time_set(tc_id, 3, 0x02020311, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            /* Legacy Time = ToD Set (Sec,Nsec) + Current Time (in Nsec) */
            /* Legacy Time = 0 + 0x07070707 */
            expected_tod_value[0] = 0x00;
            expected_tod_value[1] = 0x00;
            expected_tod_value[2] = 0x02;
            expected_tod_value[3] = 0x02;
            expected_tod_value[4] = 0x03;
            expected_tod_value[5] = 0x11;

            expected_export_tod_value[0] = 0x00;
            expected_export_tod_value[1] = 0x00;
            expected_export_tod_value[2] = 0x02;
            expected_export_tod_value[3] = 0x02;
            expected_export_tod_value[4] = 0x03;
            expected_export_tod_value[5] = 0x11;
            break;

        case 16:
            /* ftv2_system_time_test_config = 16 */

            /* PTP Time - Current Time */
            rv = ftv2_time_set(tc_id, 2, 0x03030404, 0x00000505);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            expected_tod_value[0] = 0x05;
            expected_tod_value[1] = 0x05;
            expected_tod_value[2] = 0x03;
            expected_tod_value[3] = 0x03;
            expected_tod_value[4] = 0x04;
            expected_tod_value[5] = 0x04;

            expected_export_tod_value[0] = 0x00;
            expected_export_tod_value[1] = 0x00;
            expected_export_tod_value[2] = 0x00;
            expected_export_tod_value[3] = 0x00;
            expected_export_tod_value[4] = 0x00;
            expected_export_tod_value[5] = 0x00;
            break;

        case 17:
            /* ftv2_system_time_test_config = 17 */

            /* PTP Time - Current Time */
            rv = ftv2_time_set(tc_id, 2, 0x30034004, 0x00000550);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            expected_tod_value[0] = 0x05;
            expected_tod_value[1] = 0x50;
            expected_tod_value[2] = 0x30;
            expected_tod_value[3] = 0x03;
            expected_tod_value[4] = 0x40;
            expected_tod_value[5] = 0x04;

            expected_export_tod_value[0] = 0x00;
            expected_export_tod_value[1] = 0x00;
            expected_export_tod_value[2] = 0x00;
            expected_export_tod_value[3] = 0x00;
            expected_export_tod_value[4] = 0x00;
            expected_export_tod_value[5] = 0x00;
            break;

        default:
            printf("[CINT] TC %2d - invalid system time test config %d\n",tc_id, ftv2_system_time_test_config);
            return BCM_E_FAIL;
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step18 - System Time Setup done succesfully.\n");
    }

    return BCM_E_NONE;
}

/* Destroy System Time Setting */
int
ftv2_system_time_destroy_main(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip this FT system time setting cleanup if no test config set */
    if (!ftv2_system_time_test_config) {
        if (!skip_log) {
            printf("[CINT] Cleanup - Skip FT System Timesetting destroy ......\n");
        }
        return rv;
    }

    /* Destroy FT system time setting */

    /* Clear Legacy Time */
    rv = ftv2_time_set(tc_id, 0, 0, 0);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                            tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    /* Clear NTP Time */
    rv = ftv2_time_set(tc_id, 1, 0, 0);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                             tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    /* Clear PTP Time */
    rv = ftv2_time_set(tc_id, 2, 0, 0);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                             tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    /* Clear CMIC Time */
    rv = ftv2_time_set(tc_id, 3, 0, 0);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                             tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    /* Clear MAC Time */
    rv = ftv2_time_set(tc_id, 4, 0, 0);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                             tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    expected_tod_value[0] = 0x00;
    expected_tod_value[1] = 0x00;
    expected_tod_value[2] = 0x00;
    expected_tod_value[3] = 0x00;
    expected_tod_value[4] = 0x00;
    expected_tod_value[5] = 0x00;

    expected_export_tod_value[0] = 0x00;
    expected_export_tod_value[1] = 0x00;
    expected_export_tod_value[2] = 0x00;
    expected_export_tod_value[3] = 0x00;
    expected_export_tod_value[4] = 0x00;
    expected_export_tod_value[5] = 0x00;

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Cleanup - FT System Time setting destroyed Succesfully.\n");
    }

    return BCM_E_NONE;
}

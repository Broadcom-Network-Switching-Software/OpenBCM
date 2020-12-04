/*  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        credit_request_thresholds.c
 *
 * Purpose:
 * Define default credit request profiles for DPP diag init (appl).
 *
 * Focal function:
 * appl_dpp_cosq_diag_credit_request_thresholds_set()
 */

#include <bcm/cosq.h>
#include <soc/drv.h>
#include <appl/diag/dcmn/credit_request_thresholds.h>

#define SLOW_LEVEL_FACTOR (5)

/*
 * --------------------------------------------------------------------------------------------
 * Credit request thresholds set by various functions in this file
 * assume credit size of 1K.
 *
 * When used with different credit size the following adjustments should be applied:
 * - credit_request_satisfied_empty_queue_max_balance_thresh should be equal to Credit Size
 * - the following thresholds should be rounded up to integral number of Credit Size
 *   * credit_request_hungry_off_to_normal_thresh
 *   * credit_request_hungry_slow_to_normal_thresh
 *   * credit_request_hungry_normal_to_slow_thresh
 *   * credit_request_hungry_off_to_slow_thresh (only if slow is not enabled)
 *
 * Note that the system can work with up to two different credit sizes.
 * In case there are two credit sizes, the credit size considered above is the credit size used
 * by the destination device.
 * --------------------------------------------------------------------------------------------
 */


/* 
 * Devide some of the IPS thresholds by a factor.
 * Needed for local switch devices, where thresholds should be lower.
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_divide_by_factor(bcm_cosq_delay_tolerance_t *delay_tolerance, int factor)
{
    int slow_level = 0;

    delay_tolerance->credit_request_hungry_off_to_slow_thresh                /= factor;
    delay_tolerance->credit_request_hungry_off_to_normal_thresh              /= factor;
    delay_tolerance->credit_request_hungry_slow_to_normal_thresh             /= factor;
    delay_tolerance->credit_request_hungry_normal_to_slow_thresh             /= factor;
    delay_tolerance->credit_request_satisfied_backoff_enter_thresh           /= factor;
    delay_tolerance->credit_request_satisfied_backoff_exit_thresh            /= factor;
    delay_tolerance->credit_request_satisfied_backlog_enter_thresh           /= factor;
    delay_tolerance->credit_request_satisfied_backlog_exit_thresh            /= factor;
    delay_tolerance->credit_request_satisfied_backslow_enter_thresh          /= factor;
    delay_tolerance->credit_request_satisfied_backslow_exit_thresh           /= factor;
    delay_tolerance->credit_request_satisfied_empty_queue_thresh             /= factor;
    if (delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh / factor < 1024) {
        /* Min value is 1024 */
        delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh = 1024;
    } else {
        delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh /= factor;
    }
    for (slow_level = 0; slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; ++slow_level) {
        delay_tolerance->slow_level_thresh_up[slow_level]        /= factor;
        delay_tolerance->slow_level_thresh_down[slow_level]      /= factor;
    }
}

/* 
 * credit request profile (delay tolerance) for (any) 1Gb ports 
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_1g(bcm_cosq_delay_tolerance_t *delay_tolerance, int *delay_tolerance_level)
{
    int slow_level = 0;

    delay_tolerance->credit_request_hungry_off_to_slow_thresh                = -1024;
    delay_tolerance->credit_request_hungry_off_to_normal_thresh              = -1024;
    delay_tolerance->credit_request_hungry_slow_to_normal_thresh             = -1024;
    delay_tolerance->credit_request_hungry_normal_to_slow_thresh             = -30 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_enter_thresh           = 25 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_exit_thresh            = 25 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_enter_thresh           = 1024;
    delay_tolerance->credit_request_satisfied_backlog_exit_thresh            = 1024;
    delay_tolerance->credit_request_satisfied_backslow_enter_thresh          = 25 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_exit_thresh           = 25 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_thresh             = 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh = 2 * 1024;
    for (slow_level = 0; slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; ++slow_level) {
        delay_tolerance->slow_level_thresh_up[slow_level]                    = 0;
        delay_tolerance->slow_level_thresh_down[slow_level]                  = 0;
    }

    *delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_1G;
}

/* 
 * credit request profile (delay tolerance) for slow enabled 10Gb ports 
 * The thresholds essentially are (100G threshold / 10), but we round them to 
 * 1K resolution.
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_10g_slow_enabled(bcm_cosq_delay_tolerance_t *delay_tolerance, int *delay_tolerance_level)
{
    int slow_level = 0;
    int default_slow_level_thresh_up[BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS] = {1500, 3000, 6000, 10000, 15000, 21000, 28000};
    int default_slow_level_thresh_down[BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS] = {500, 1500, 3000, 6000, 10000, 15000, 21000};

    delay_tolerance->credit_request_hungry_off_to_slow_thresh                = 0;
    delay_tolerance->credit_request_hungry_off_to_normal_thresh              = 4 * 1024;
    delay_tolerance->credit_request_hungry_slow_to_normal_thresh             = 4 * 1024;
    delay_tolerance->credit_request_hungry_normal_to_slow_thresh             = 4 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_enter_thresh           = 4 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_exit_thresh            = 4 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_enter_thresh           = 4 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_exit_thresh            = 4 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_enter_thresh          = 4 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_exit_thresh           = 4 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_thresh             = 0;
    delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh = 1024;
    for (slow_level = 0; slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; ++slow_level) {
        delay_tolerance->slow_level_thresh_up[slow_level]                    = default_slow_level_thresh_up[slow_level];
        delay_tolerance->slow_level_thresh_down[slow_level]                  = default_slow_level_thresh_down[slow_level];
    }

    *delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED;
}

/* 
 * credit request profile (delay tolerance) for slow enabled 10Gb ports on local switch device 
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_10g_slow_enabled_local_switch(bcm_cosq_delay_tolerance_t *delay_tolerance, int *delay_tolerance_level)
{
    appl_dpp_cosq_diag_credit_request_thresholds_10g_slow_enabled(delay_tolerance, delay_tolerance_level);
    delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh *= 2;
}

/* 
 * credit request profile (delay tolerance) for delay sensitive 10Gb ports 
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_10g_low_delay(bcm_cosq_delay_tolerance_t *delay_tolerance, int *delay_tolerance_level)
{
    int slow_level = 0;

    delay_tolerance->credit_request_hungry_off_to_slow_thresh                = -38 * 1024 / 10;
    delay_tolerance->credit_request_hungry_off_to_normal_thresh              = -38 * 1024 / 10;
    delay_tolerance->credit_request_hungry_slow_to_normal_thresh             = -38 * 1024 / 10;
    delay_tolerance->credit_request_hungry_normal_to_slow_thresh             = -30 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_enter_thresh           = 25 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_exit_thresh            = 25 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_enter_thresh           = 38 * 1024 / 10;
    delay_tolerance->credit_request_satisfied_backlog_exit_thresh            = 38 * 1024 / 10;
    delay_tolerance->credit_request_satisfied_backslow_enter_thresh          = 25 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_exit_thresh           = 25 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_thresh             = 38 * 1024 / 10;
    delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh = 76 * 1024 / 10;
    delay_tolerance->flags                                                  |= BCM_COSQ_DELAY_TOLERANCE_IS_LOW_LATENCY;
    for (slow_level = 0; slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; ++slow_level) {
        delay_tolerance->slow_level_thresh_up[slow_level]                    = 0;
        delay_tolerance->slow_level_thresh_down[slow_level]                  = 0;
    }

    *delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY;
}

/* 
 * credit request profile (delay tolerance) for slow enabled 40Gb ports 
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_40g_slow_enabled(bcm_cosq_delay_tolerance_t *delay_tolerance, int *delay_tolerance_level)
{
    int slow_level = 0;
    int default_slow_level_thresh_up[BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS] = {1500, 3000, 6000, 10000, 15000, 21000, 28000};
    int default_slow_level_thresh_down[BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS] = {500, 1500, 3000, 6000, 10000, 15000, 21000};

    delay_tolerance->credit_request_hungry_off_to_slow_thresh                = 0;
    delay_tolerance->credit_request_hungry_off_to_normal_thresh              = 15 * 1024;
    delay_tolerance->credit_request_hungry_slow_to_normal_thresh             = 15 * 1024;
    delay_tolerance->credit_request_hungry_normal_to_slow_thresh             = 15 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_enter_thresh           = 15 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_exit_thresh            = 15 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_enter_thresh           = 15 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_exit_thresh            = 15 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_enter_thresh          = 15 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_exit_thresh           = 15 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_thresh             = 0;
    delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh = 1024;
    for (slow_level = 0; slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; ++slow_level) {
        delay_tolerance->slow_level_thresh_up[slow_level]                    = default_slow_level_thresh_up[slow_level];
        delay_tolerance->slow_level_thresh_down[slow_level]                  = default_slow_level_thresh_down[slow_level];
    }

    *delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_40G_SLOW_ENABLED;
}

/* 
 * credit request profile (delay tolerance) for slow enabled 40Gb ports on local switch device 
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_40g_slow_enabled_local_switch(bcm_cosq_delay_tolerance_t *delay_tolerance, int *delay_tolerance_level)
{
    int factor = SLOW_LEVEL_FACTOR;

    appl_dpp_cosq_diag_credit_request_thresholds_40g_slow_enabled(delay_tolerance, delay_tolerance_level);
    appl_dpp_cosq_diag_credit_request_thresholds_divide_by_factor(delay_tolerance, factor);
}

/* 
 * credit request profile (delay tolerance) for delay sensitive 40Gb ports 
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_40g_low_delay(bcm_cosq_delay_tolerance_t *delay_tolerance, int *delay_tolerance_level)
{
    int slow_level = 0;

    delay_tolerance->credit_request_hungry_off_to_slow_thresh                = -15 * 1024;
    delay_tolerance->credit_request_hungry_off_to_normal_thresh              = -15 * 1024;
    delay_tolerance->credit_request_hungry_slow_to_normal_thresh             = -15 * 1024;
    delay_tolerance->credit_request_hungry_normal_to_slow_thresh             = -30 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_enter_thresh           = 30 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_exit_thresh            = 30 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_enter_thresh           = 15 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_exit_thresh            = 15 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_thresh             = 15 * 1024 / 2;
    delay_tolerance->credit_request_satisfied_backslow_enter_thresh          = 30 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_exit_thresh           = 30 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh = 15 * 1024;
    delay_tolerance->flags                                                  |= BCM_COSQ_DELAY_TOLERANCE_IS_LOW_LATENCY;
    for (slow_level = 0; slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; ++slow_level) {
        delay_tolerance->slow_level_thresh_up[slow_level]                    = 0;
        delay_tolerance->slow_level_thresh_down[slow_level]                  = 0;
    }

    *delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_40G_LOW_DELAY;
}

/* 
 * credit request profile (delay tolerance) for slow enabled 100Gb ports 
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_100g_slow_enabled(bcm_cosq_delay_tolerance_t *delay_tolerance, int *delay_tolerance_level)
{
    int slow_level = 0;
    int default_slow_level_thresh_up[BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS] = {1500, 3000, 6000, 10000, 15000, 21000, 28000};
    int default_slow_level_thresh_down[BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS] = {500, 1500, 3000, 6000, 10000, 15000, 21000};

    delay_tolerance->credit_request_hungry_off_to_slow_thresh                = 0;
    delay_tolerance->credit_request_hungry_off_to_normal_thresh              = 38 * 1024;
    delay_tolerance->credit_request_hungry_slow_to_normal_thresh             = 38 * 1024;
    delay_tolerance->credit_request_hungry_normal_to_slow_thresh             = 38 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_enter_thresh           = 38 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_exit_thresh            = 38 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_enter_thresh           = 38 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_exit_thresh            = 38 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_enter_thresh          = 38 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_exit_thresh           = 38 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_thresh             = 0;
    delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh = 1024;
    for (slow_level = 0; slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; ++slow_level) {
        delay_tolerance->slow_level_thresh_up[slow_level]                    = default_slow_level_thresh_up[slow_level];
        delay_tolerance->slow_level_thresh_down[slow_level]                  = default_slow_level_thresh_down[slow_level];
    }

    *delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_100G_SLOW_ENABLED;
}

/* 
 * credit request profile (delay tolerance) for slow enabled 100Gb ports on local switch device 
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_100g_slow_enabled_local_switch(bcm_cosq_delay_tolerance_t *delay_tolerance, int *delay_tolerance_level)
{
    int factor = SLOW_LEVEL_FACTOR;

    appl_dpp_cosq_diag_credit_request_thresholds_100g_slow_enabled(delay_tolerance, delay_tolerance_level);
    appl_dpp_cosq_diag_credit_request_thresholds_divide_by_factor(delay_tolerance, factor);
}

/* 
 * credit request profile (delay tolerance) for delay sensitive 100Gb ports 
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_100g_low_delay(bcm_cosq_delay_tolerance_t *delay_tolerance, int *delay_tolerance_level)
{
    int slow_level = 0;

    delay_tolerance->credit_request_hungry_off_to_slow_thresh                = -38 * 1024;
    delay_tolerance->credit_request_hungry_off_to_normal_thresh              = -38 * 1024;
    delay_tolerance->credit_request_hungry_slow_to_normal_thresh             = -38 * 1024;
    delay_tolerance->credit_request_hungry_normal_to_slow_thresh             = -100 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_enter_thresh           = 90 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_exit_thresh            = 90 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_enter_thresh           = 38 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_exit_thresh            = 38 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_enter_thresh          = 90 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_exit_thresh           = 90 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_thresh             = 38 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh = 76 * 1024;
    delay_tolerance->flags                                                  |= BCM_COSQ_DELAY_TOLERANCE_IS_LOW_LATENCY;
    for (slow_level = 0; slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; ++slow_level) {
        delay_tolerance->slow_level_thresh_up[slow_level]                    = 0;
        delay_tolerance->slow_level_thresh_down[slow_level]                  = 0;
    }

    *delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_100G_LOW_DELAY;
}

/* 
 * credit request profile (delay tolerance) for slow enabled 200Gb ports 
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_200g_slow_enabled(bcm_cosq_delay_tolerance_t *delay_tolerance, int *delay_tolerance_level)
{
    int slow_level = 0;
    int default_slow_level_thresh_up[BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS] = {1500, 3000, 6000, 10000, 15000, 21000, 28000};
    int default_slow_level_thresh_down[BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS] = {500, 1500, 3000, 6000, 10000, 15000, 21000};

    delay_tolerance->credit_request_hungry_off_to_slow_thresh                = 0;
    delay_tolerance->credit_request_hungry_off_to_normal_thresh              = 76 * 1024;
    delay_tolerance->credit_request_hungry_slow_to_normal_thresh             = 76 * 1024;
    delay_tolerance->credit_request_hungry_normal_to_slow_thresh             = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_enter_thresh           = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_exit_thresh            = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_enter_thresh           = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_exit_thresh            = 76 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_thresh             = 0;
    delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh = 1024;
    for (slow_level = 0; slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; ++slow_level) {
        delay_tolerance->slow_level_thresh_up[slow_level]                    = default_slow_level_thresh_up[slow_level];
        delay_tolerance->slow_level_thresh_down[slow_level]                  = default_slow_level_thresh_down[slow_level];
    }

    *delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_200G_SLOW_ENABLED;
}

/* 
 * credit request profile (delay tolerance) for delay sensitive 200Gb ports 
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_200g_low_delay(bcm_cosq_delay_tolerance_t *delay_tolerance, int *delay_tolerance_level)
{
    int slow_level = 0;

    delay_tolerance->credit_request_hungry_off_to_slow_thresh                = -76 * 1024;
    delay_tolerance->credit_request_hungry_off_to_normal_thresh              = -76 * 1024;
    delay_tolerance->credit_request_hungry_slow_to_normal_thresh             = -76 * 1024;
    delay_tolerance->credit_request_hungry_normal_to_slow_thresh             = -114688; /* max absolute value supported */
    delay_tolerance->credit_request_satisfied_backoff_enter_thresh           = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_exit_thresh            = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_enter_thresh           = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_exit_thresh            = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_enter_thresh          = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_exit_thresh           = 76 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_thresh             = 38 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh = 64 * 1024;
    delay_tolerance->flags                                                  |= BCM_COSQ_DELAY_TOLERANCE_IS_LOW_LATENCY;
    for (slow_level = 0; slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; ++slow_level) {
        delay_tolerance->slow_level_thresh_up[slow_level]                    = 0;
        delay_tolerance->slow_level_thresh_down[slow_level]                  = 0;
    }

    *delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY;
}

/* 
 * credit request profile (delay tolerance) for delay sensitive 200Gb ports 
 */
STATIC void
appl_dpp_cosq_diag_credit_request_thresholds_200g_high_prio(bcm_cosq_delay_tolerance_t *delay_tolerance, int *delay_tolerance_level)
{
    int slow_level = 0;

    delay_tolerance->credit_request_hungry_off_to_slow_thresh                = -76 * 1024;
    delay_tolerance->credit_request_hungry_off_to_normal_thresh              = -76 * 1024;
    delay_tolerance->credit_request_hungry_slow_to_normal_thresh             = -76 * 1024;
    delay_tolerance->credit_request_hungry_normal_to_slow_thresh             = -114688; /* max absolute value supported */
    delay_tolerance->credit_request_satisfied_backoff_enter_thresh           = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backoff_exit_thresh            = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_enter_thresh           = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backlog_exit_thresh            = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_enter_thresh          = 76 * 1024;
    delay_tolerance->credit_request_satisfied_backslow_exit_thresh           = 76 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_thresh             = 38 * 1024;
    delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh = 64 * 1024; /* using backoff value, higher is not allowed */
    delay_tolerance->flags                                                  |= BCM_COSQ_DELAY_TOLERANCE_HIGH_Q_PRIORITY;
    for (slow_level = 0; slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; ++slow_level) {
        delay_tolerance->slow_level_thresh_up[slow_level]                    = 0;
        delay_tolerance->slow_level_thresh_down[slow_level]                  = 0;
    }

    *delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_15;
}

/*
 * Define credit request profiles.
 *
 * Params:
 *  port_speed  - the speed of the port.
 *  is_slow     - 0/1 to apply low_delay/slow thresholds.
 *  mode        - additional parameter to choose appropriate thresholds.
 *                Can be one of:
 *                APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_SINGLE_FAP - profile match the case where there is only 1 fap in the system.
 *                APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_MGMT - profile match the case where there are 2 faps in the system.
 *                APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_HIGH_PRIO - high priority credit request profile.
 */
int appl_dpp_cosq_diag_credit_request_thresholds_set(int unit, uint32 port_speed, int is_slow, APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE mode)
{
    int rv;
    int credit_watchdog_mode, nof_drams, delay_tolerance_level;
    bcm_cosq_delay_tolerance_t delay_tolerance;

    sal_memset(&delay_tolerance, 0x0,sizeof(bcm_cosq_delay_tolerance_t));

    /* Get credit watchdog mode */
    rv = bcm_fabric_control_get(unit, bcmFabricWatchdogQueueEnable, &credit_watchdog_mode);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_cosq_control_range_get failed. Error:%d (%s) \n"), 
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /* Watchdog thresholds */
    delay_tolerance.credit_request_watchdog_status_msg_gen = (credit_watchdog_mode >= BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE) ?
       0 : 33; /* default status msg threshold */
    delay_tolerance.credit_request_watchdog_delete_queue_thresh = 500; /*default delete queue threshold */

    /* Get OCB only flags */
    nof_drams = soc_property_get(unit, spn_EXT_RAM_PRESENT, 0);
    delay_tolerance.flags = nof_drams ? 0 : BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY;

    /* Common thresholds */
    delay_tolerance.credit_request_hungry_multiplier = 2048;
    delay_tolerance.credit_request_satisfied_empty_queue_exceed_thresh = 1;

    /* Specific thresholds per port, slow, mode */
    if (port_speed <= 1000) {
        appl_dpp_cosq_diag_credit_request_thresholds_1g(&delay_tolerance, &delay_tolerance_level);
    } else if (port_speed <= 10000) {
        if (is_slow) {
            appl_dpp_cosq_diag_credit_request_thresholds_10g_slow_enabled(&delay_tolerance, &delay_tolerance_level);
            if (mode == APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_MGMT) {
                appl_dpp_cosq_diag_credit_request_thresholds_10g_slow_enabled(&delay_tolerance, &delay_tolerance_level);
            } else { /* mode == APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_SINGLE_FAP */
                appl_dpp_cosq_diag_credit_request_thresholds_10g_slow_enabled_local_switch(&delay_tolerance, &delay_tolerance_level);
            }
        } else {
            appl_dpp_cosq_diag_credit_request_thresholds_10g_low_delay(&delay_tolerance, &delay_tolerance_level);
        }
    } else if (port_speed <= 40000) {
        if (is_slow) {
            if (mode == APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_MGMT) {
                appl_dpp_cosq_diag_credit_request_thresholds_40g_slow_enabled(&delay_tolerance, &delay_tolerance_level);
            } else { /* mode == APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_SINGLE_FAP */
                appl_dpp_cosq_diag_credit_request_thresholds_40g_slow_enabled_local_switch(&delay_tolerance, &delay_tolerance_level);
            }
        } else {
            appl_dpp_cosq_diag_credit_request_thresholds_40g_low_delay(&delay_tolerance, &delay_tolerance_level);
        }
    } else if (port_speed <= 100000) {
        if (is_slow) {
            if (mode == APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_MGMT) {
                appl_dpp_cosq_diag_credit_request_thresholds_100g_slow_enabled(&delay_tolerance, &delay_tolerance_level);
            } else { /* mode == APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_SINGLE_FAP */
                appl_dpp_cosq_diag_credit_request_thresholds_100g_slow_enabled_local_switch(&delay_tolerance, &delay_tolerance_level);
            }
        } else {
            appl_dpp_cosq_diag_credit_request_thresholds_100g_low_delay(&delay_tolerance, &delay_tolerance_level);
        }
    } else {
        if (is_slow) {
            appl_dpp_cosq_diag_credit_request_thresholds_200g_slow_enabled(&delay_tolerance, &delay_tolerance_level);
        } else {
            if (mode == APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_HIGH_PRIO) {
                appl_dpp_cosq_diag_credit_request_thresholds_200g_high_prio(&delay_tolerance, &delay_tolerance_level);
            } else {
                appl_dpp_cosq_diag_credit_request_thresholds_200g_low_delay(&delay_tolerance, &delay_tolerance_level);
            }
        }
    }

    /* Define credit request profile */
    rv = bcm_cosq_delay_tolerance_level_set(unit, delay_tolerance_level, &delay_tolerance);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return BCM_E_NONE;
}

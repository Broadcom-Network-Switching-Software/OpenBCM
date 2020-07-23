/** \file iqs_tune.c
 *  Tuning for ingress queue scheduler.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/tune/iqs_tune.h>
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx_dispatch.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tune.h>

/*
 * }
 * Include files.
 */

static shr_error_e dnx_tune_iqs_rate_to_bw_level_get(
    int unit,
    uint32 rate,
    int *bw_level);

static shr_error_e dnx_tune_iqs_rate_to_credit_resolution_get(
    int unit,
    int rate,
    int fabric_delay,
    int credit_size,
    int slow_enable,
    int *credit_resolution);

static shr_error_e dnx_tune_iqs_slow_to_normal_th_get(
    int unit,
    int credit_resolution,
    int *slow_to_normal_th);

/*
 * See .h
 */
shr_error_e
dnx_tune_iqs_delay_tolerance_preset_get(
    int unit,
    bcm_cosq_delay_tolerance_preset_attr_t * preset_attr,
    bcm_cosq_delay_tolerance_t * delay_tolerance)
{
    int credit_resolution, bw_level;
    int slow_enable, max_inflight_credits, max_inflight_bytes;
    int fabric_delay;
    int slow_to_normal_th;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(delay_tolerance, 0, sizeof(bcm_cosq_delay_tolerance_t));

    /** push queue */
    if (preset_attr->flags & BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_PUSH_QUEUE)
    {
        delay_tolerance->flags |= BCM_COSQ_DELAY_TOLERANCE_PUSH_QUEUE;
    }
    /** high priority */
    if (preset_attr->flags & BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_HIGH_PRIORITY)
    {
        delay_tolerance->flags |= BCM_COSQ_DELAY_TOLERANCE_HIGH_Q_PRIORITY;
    }

    /** OCB only */
    if (preset_attr->flags & BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_OCB_ONLY)
    {
        delay_tolerance->flags |= BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY;
    }

    /** slow enable */
    slow_enable = (preset_attr->flags & BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_SLOW_ENABLED) ? 1 : 0;

    /** Low latency */
    if (!slow_enable)
    {
        delay_tolerance->flags |= BCM_COSQ_DELAY_TOLERANCE_IS_LOW_LATENCY;
    }

    if (!(preset_attr->flags & BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_PUSH_QUEUE))
    {
        /** bandwidth level */
        SHR_IF_ERR_EXIT(dnx_tune_iqs_rate_to_bw_level_get(unit, preset_attr->rate, &bw_level));
        delay_tolerance->bandwidth_level = bw_level;

        /** delayed size */
        if (preset_attr->flags & BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_MULTICAST)
        {
            fabric_delay = dnx_data_tune.iqs.fabric_multicast_delay_get(unit);
        }
        else
        {
            fabric_delay = dnx_data_tune.iqs.fabric_delay_get(unit);
        }

        /*
         * calculate the required credit resolution for the rate
         */
        SHR_IF_ERR_EXIT(dnx_tune_iqs_rate_to_credit_resolution_get(unit, preset_attr->rate, fabric_delay,
                                                                   preset_attr->credit_size, slow_enable,
                                                                   &credit_resolution));

        /** maximal amount of in-flight credits (for a single direction) */
        max_inflight_credits = UTILEX_DIV_ROUND_UP(preset_attr->rate * fabric_delay, preset_attr->credit_size * 8);
        max_inflight_credits = UTILEX_MAX(2, max_inflight_credits);

        /** maximal amount of in-flight bytes (for a single direction) */
        max_inflight_bytes = max_inflight_credits * preset_attr->credit_size;

        /** IF(Slow Enable,0, CreditSize*(MaxInflight[Credits]+1)) */
        delay_tolerance->credit_request_satisfied_empty_queue_thresh = slow_enable ? 0 : preset_attr->credit_size * (max_inflight_credits + 1);  /** bytes */

        /** IF(Slow Enable,2*CreditSize, MIN(500*1000, 3*MaxInflight[Credits]*CreditSize-CreditSize)) */
        delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh = slow_enable ? 2 * preset_attr->credit_size : 3 * max_inflight_bytes - preset_attr->credit_size;   /** bytes */
        delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh =
            UTILEX_MIN(500 * 1000, delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh);

        delay_tolerance->credit_request_satisfied_empty_queue_exceed_thresh = 1;

        /** satisfied thresholds */
        /** IF((CreditResolution=0),128-64,IF((CreditResolution=1),256-128,IF((CreditResolution=2),512-256))) */
        /** credit_resolution variable hold 1, 2 or 4 */
        delay_tolerance->credit_request_satisfied_backoff_enter_thresh = ((128 - 64) * credit_resolution) * 1024;
                                                                                                                /** bytes */
        delay_tolerance->credit_request_satisfied_backoff_exit_thresh = ((128 - 64) * credit_resolution) * 1024;
                                                                                                               /** bytes */
        delay_tolerance->credit_request_satisfied_backlog_enter_thresh = slow_enable ? delay_tolerance->credit_request_satisfied_backoff_enter_thresh : delay_tolerance->credit_request_satisfied_empty_queue_thresh; /** bytes */
        delay_tolerance->credit_request_satisfied_backlog_exit_thresh = slow_enable ? delay_tolerance->credit_request_satisfied_backoff_enter_thresh : delay_tolerance->credit_request_satisfied_empty_queue_thresh; /** bytes */

        delay_tolerance->credit_request_satisfied_backslow_enter_thresh = slow_enable ? delay_tolerance->credit_request_satisfied_backoff_enter_thresh * 75 / 100 : delay_tolerance->credit_request_satisfied_backoff_enter_thresh + 6 * 4096; /** bytes */
        delay_tolerance->credit_request_satisfied_backslow_exit_thresh = slow_enable ? delay_tolerance->credit_request_satisfied_backoff_exit_thresh * 75 / 100 : delay_tolerance->credit_request_satisfied_backoff_exit_thresh + 6 * 4096; /** bytes */

        /** hungry thresholds - all in bytes*/
        delay_tolerance->credit_request_hungry_off_to_normal_thresh =
            slow_enable ? (2 * (max_inflight_credits + 1) * preset_attr->credit_size) : (-1 *
                                                                                         delay_tolerance->
                                                                                         credit_request_satisfied_empty_queue_thresh);
                                                                                                                                                             /** bytes */
        delay_tolerance->credit_request_hungry_off_to_normal_thresh =
            UTILEX_RANGE_BOUND(delay_tolerance->credit_request_hungry_off_to_normal_thresh, -500 * 1024, 500 * 1024);

        delay_tolerance->credit_request_hungry_off_to_slow_thresh = slow_enable ? 0 /** enable slow on the first packet */ : delay_tolerance->credit_request_hungry_off_to_normal_thresh;  /** bytes */
        delay_tolerance->credit_request_hungry_off_to_slow_thresh =
            UTILEX_RANGE_BOUND(delay_tolerance->credit_request_hungry_off_to_slow_thresh, -500 * 1024, 500 * 1024);

        SHR_IF_ERR_EXIT(dnx_tune_iqs_slow_to_normal_th_get(unit, credit_resolution, &slow_to_normal_th));

        delay_tolerance->credit_request_hungry_slow_to_normal_thresh =
            slow_enable ? (2 * (max_inflight_credits + 1) * preset_attr->credit_size) : -1 * slow_to_normal_th;
                                                                                                                /** bytes */
        delay_tolerance->credit_request_hungry_slow_to_normal_thresh =
            UTILEX_RANGE_BOUND(delay_tolerance->credit_request_hungry_slow_to_normal_thresh, -500 * 1024, 500 * 1024);

        delay_tolerance->credit_request_hungry_normal_to_slow_thresh =
            slow_enable ? (2 * (max_inflight_credits + 1) * preset_attr->credit_size) : -1 * slow_to_normal_th;
                                                                                                                /** bytes */
        delay_tolerance->credit_request_hungry_normal_to_slow_thresh =
            UTILEX_RANGE_BOUND(delay_tolerance->credit_request_hungry_normal_to_slow_thresh, -500 * 1024, 500 * 1024);

        delay_tolerance->credit_request_hungry_multiplier = 2048;   /** bytes */

        /** slow levels - all in bytes*/
        if (slow_enable)
        {
            int slow_level_index, slow_level_th;
            int off_to_norm_th;

            off_to_norm_th = delay_tolerance->credit_request_hungry_off_to_normal_thresh;

            for (slow_level_index = 0; slow_level_index < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; slow_level_index++)
            {
                slow_level_th = off_to_norm_th * (slow_level_index + 1) / 8;

                /** same threshold for up and down */
                delay_tolerance->slow_level_thresh_down[slow_level_index] = slow_level_th;
                delay_tolerance->slow_level_thresh_up[slow_level_index] = slow_level_th;
            }
        }

        /** credit watchdog delete threshold */
        if (dnx_data_iqs.credit.feature_get(unit, dnx_data_iqs_credit_watchdog_fixed_period))
        {
            /*
             * Increasing the threshold - allows to avoid from an issue occurs while a queue existing from delete state
             */
            /** 10 sec = 10000 mili seconds */
            delay_tolerance->credit_request_watchdog_delete_queue_thresh = 10000;
        }
        else
        {
            /** 500 mili seconds */
            delay_tolerance->credit_request_watchdog_delete_queue_thresh = 500;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Setting all voq priorities to low
 */
static shr_error_e
dnx_tune_iqs_dqcq_priority_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities))
    {
        /*
         * all priorities are set to LOW
         */
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_set(unit, 0, -1, bcmCosqControlIngressMinLowPriority, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Tune credit watchdog
 * Configure range of queues to be all queues and set credit retransmit threshold.
 * Delete threshold configured per credit request profile in dnx_tune_iqs_delay_tolerance_preset_get()
 */
static shr_error_e
dnx_tune_iqs_credit_watchdog_init(
    int unit)
{
    int min_queue, max_queue, retransmit_threshold, enable;
    bcm_cosq_delay_tolerance_t delay_tolerance;
    bcm_cosq_range_t range;
    SHR_FUNC_INIT_VARS(unit);

    min_queue = 0;
    max_queue = dnx_data_ipq.queues.nof_queues_get(unit) - 1;   /* max queue */
    enable = 1;
    retransmit_threshold = 500; /* micro seconds */

    /** Set retransmit threshold */
    sal_memset(&delay_tolerance, 0, sizeof(bcm_cosq_delay_tolerance_t));
    delay_tolerance.credit_request_watchdog_status_msg_gen = retransmit_threshold;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_delay_tolerance_level_set
                    (unit, BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG, &delay_tolerance));

    /** Set queue range and enable */
    sal_memset(&range, 0, sizeof(bcm_cosq_range_t));
    range.range_start = min_queue;
    range.range_end = max_queue;
    range.is_enabled = enable;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_range_set(unit, 0, 0, bcmCosqWatchdogQueue, &range));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Convert rate to predefined bandwidth levels
 *
 * \param [in] unit -  Unit-ID
 * \param [in] rate -  port rate - Gbps.
 * \param [out] bw_level -  level according to rate.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_tune_iqs_rate_to_bw_level_get(
    int unit,
    uint32 rate,
    int *bw_level)
{
    uint32 rate_min, rate_max;
    SHR_FUNC_INIT_VARS(unit);

    for (*bw_level = 0; *bw_level < dnx_data_iqs.credit.nof_bw_levels_get(unit); (*bw_level)++)
    {
         /** credit resolution */
        rate_min = dnx_data_tune.iqs.bw_level_rate_get(unit, *bw_level)->min;
        rate_max = dnx_data_tune.iqs.bw_level_rate_get(unit, *bw_level)->max;
         /** for the highest group */
        if (rate >= rate_min && rate <= rate_max)
        {
            break;
        }
    }

    /*
     * exit:
     */
    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate credit resolution from rate
 */
static shr_error_e
dnx_tune_iqs_rate_to_credit_resolution_get(
    int unit,
    int rate,
    int fabric_delay,
    int credit_size,
    int slow_enable,
    int *credit_resolution)
{
    int delayed_size_credits, delayed_size_bytes;
    int i;
    int factor;

    SHR_FUNC_INIT_VARS(unit);

    /** fabric delay - nano second */

    factor = (slow_enable ? 2 : 3);
    /** rate - Gbit/sec */
    delayed_size_credits = UTILEX_DIV_ROUND_UP((fabric_delay * factor) * rate / 8, credit_size);        /* credits */
    delayed_size_bytes = delayed_size_credits * credit_size;    /* bytes */

    *credit_resolution = 4;
    for (i = 0; i < 3; i++)
    {
        if (delayed_size_bytes < dnx_data_tune.iqs.credit_resolution_up_th_get(unit) * (1 << i))
        {
            *credit_resolution = (1 << i);
            break;
        }
    }

/*exit:*/
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tune_iqs_bw_level_to_credit_resolution_get(
    int unit,
    int bw_level,
    int *credit_resolution)
{
    int fabric_delay;
    int credit_worth;
    int rate;

    SHR_FUNC_INIT_VARS(unit);

    fabric_delay = dnx_data_tune.iqs.fabric_delay_get(unit);
    credit_worth = dnx_data_iqs.credit.worth_get(unit);

    /** rate - Gbit/sec */
    rate = dnx_data_tune.iqs.bw_level_rate_get(unit, bw_level)->max;
    /** for the highest group */
    if (rate == -1)
    {
        rate = dnx_data_tune.iqs.bw_level_rate_get(unit, bw_level)->min;
    }

    SHR_IF_ERR_EXIT(dnx_tune_iqs_rate_to_credit_resolution_get(unit, rate, fabric_delay, credit_worth, FALSE,
                                                               credit_resolution));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get slow to normal threshold (slow disabled)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] credit_resolution - credit resolution (1,2,4)
 * \param [out] slow_to_normal_th - slow to normal threshold
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_tune_iqs_slow_to_normal_th_get(
    int unit,
    int credit_resolution,
    int *slow_to_normal_th)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (credit_resolution)
    {
        case 1:
            *slow_to_normal_th = (128 - 6) * 1024;
            break;
        case 2:
            *slow_to_normal_th = (256 - 40) * 1024;
            break;
        case 4:
            *slow_to_normal_th = (512 - 80) * 1024;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid credit resolution %d", credit_resolution);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_tune_iqs_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Enable and config thresholds for credit watchdog */
    SHR_IF_ERR_EXIT(dnx_tune_iqs_credit_watchdog_init(unit));

    /** setting all priorities to be low */
    SHR_IF_ERR_EXIT(dnx_tune_iqs_dqcq_priority_init(unit));

exit:
    SHR_FUNC_EXIT;
}

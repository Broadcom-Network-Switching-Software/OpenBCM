/** \file iqs_api.c
 * 
 *
 * BCM APIs and High level functions
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm/cosq.h>
#include <shared/gport.h>

#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx/cosq/ingress/iqs_api.h>
#include <bcm_int/dnx/cosq/cosq.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/tune/iqs_tune.h>
#include <sal/core/thread.h>

#include "iqs.h"

/*
 * Defines
 * {
 */

 /*
  * }
  */

/*
 * See .h
 */
shr_error_e
dnx_iqs_api_wd_range_set(
    int unit,
    bcm_cosq_range_t * range)
{
    int enable;
    SHR_FUNC_INIT_VARS(unit);

    /** High Level Verify */
    SHR_NULL_CHECK(range, _SHR_E_PARAM, "range");

    /** set se state */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_min_sw_set(unit, /* queue-id */ range->range_start));
    SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_max_sw_set(unit, /* queue-id */ range->range_end));
    enable = range->is_enabled;
    SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_enable_sw_set(unit, enable));
    /** verify */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_verify(unit));
    /** set hw accordingly */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_hw_set(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_iqs_api_wd_range_get(
    int unit,
    bcm_cosq_range_t * range)
{
    int enable;
    SHR_FUNC_INIT_VARS(unit);

    /** High Level Verify */
    SHR_NULL_CHECK(range, _SHR_E_PARAM, "range");

    /** Clear structure */
    sal_memset(range, 0, sizeof(bcm_cosq_range_t));

    SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_min_sw_get(unit, /* queue-id */ &range->range_start));
    SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_max_sw_get(unit, /* queue-id */ &range->range_end));
    SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_enable_sw_get(unit, /* enable/disable */ &enable));
    range->is_enabled = enable;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_iqs_api_queue_to_request_profile_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int delay_tolerance_id)
{
    int profile_id;
    int queue_id;
    int core_id;
    SHR_FUNC_INIT_VARS(unit);

    /** Extract parameters */
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        queue_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
        core_id = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport);
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        queue_id = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
        core_id = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "expect just ucast / mast queue group gport.\n");
    }
    /** convert mode to actual profile_id */
    SHR_IF_ERR_EXIT(dnx_iqs_api_delay_tolerance_level_to_profile_id_get(unit, delay_tolerance_id, &profile_id));
    /** verify profile_id valid */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_request_profile_valid_verify(unit, profile_id));

    /** map queue to profile */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_request_profile_map_set(unit, core_id, queue_id, cosq, profile_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_iqs_api_queue_to_request_profile_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *delay_tolerance_id)
{
    int queue_id;
    int core_id;
    SHR_FUNC_INIT_VARS(unit);

    /** Extract parameters */
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        queue_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
        core_id = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport);
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        queue_id = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
        core_id = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "expect just ucast / mast queue group gport.\n");
    }
    SHR_IF_ERR_EXIT(dnx_iqs_credit_request_profile_map_get(unit, core_id, queue_id, cosq, delay_tolerance_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_iqs_api_delay_tolerance_level_to_profile_id_get(
    int unit,
    int delay_tolerance_level,
    int *profile_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * extract actual profile id from delay_tolerance_level
     */
    if (delay_tolerance_level >= 0 && delay_tolerance_level < dnx_data_iqs.credit.nof_profiles_get(unit))
    {
        /** one to one mapping */
        *profile_id = delay_tolerance_level;
    }
    else
    {
        /** for backward compatibility map delay tolerance defines to profiles */
        switch (delay_tolerance_level)
        {
            case BCM_COSQ_DELAY_TOLERANCE_NORMAL:
                *profile_id = 0;
                break;
            case BCM_COSQ_DELAY_TOLERANCE_1G:
                *profile_id = 1;
                break;
            case BCM_COSQ_DELAY_TOLERANCE_02:
            case BCM_COSQ_DELAY_TOLERANCE_03:
            case BCM_COSQ_DELAY_TOLERANCE_04:
            case BCM_COSQ_DELAY_TOLERANCE_05:
            case BCM_COSQ_DELAY_TOLERANCE_06:
            case BCM_COSQ_DELAY_TOLERANCE_07:
            case BCM_COSQ_DELAY_TOLERANCE_08:
            case BCM_COSQ_DELAY_TOLERANCE_09:
            case BCM_COSQ_DELAY_TOLERANCE_10:
            case BCM_COSQ_DELAY_TOLERANCE_11:
            case BCM_COSQ_DELAY_TOLERANCE_12:
            case BCM_COSQ_DELAY_TOLERANCE_13:
            case BCM_COSQ_DELAY_TOLERANCE_14:
            case BCM_COSQ_DELAY_TOLERANCE_15:
                *profile_id = delay_tolerance_level - BCM_COSQ_DELAY_TOLERANCE_02 + 2;
                break;
            case BCM_COSQ_DELAY_TOLERANCE_16:
            case BCM_COSQ_DELAY_TOLERANCE_17:
            case BCM_COSQ_DELAY_TOLERANCE_18:
            case BCM_COSQ_DELAY_TOLERANCE_19:
            case BCM_COSQ_DELAY_TOLERANCE_20:
            case BCM_COSQ_DELAY_TOLERANCE_21:
            case BCM_COSQ_DELAY_TOLERANCE_22:
            case BCM_COSQ_DELAY_TOLERANCE_23:
                *profile_id = delay_tolerance_level - BCM_COSQ_DELAY_TOLERANCE_16 + 16;
                break;
            case BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED:
                *profile_id = 24;
                break;
            case BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY:
                *profile_id = 25;
                break;
            case BCM_COSQ_DELAY_TOLERANCE_40G_SLOW_ENABLED:
                *profile_id = 26;
                break;
            case BCM_COSQ_DELAY_TOLERANCE_40G_LOW_DELAY:
                *profile_id = 27;
                break;
            case BCM_COSQ_DELAY_TOLERANCE_100G_SLOW_ENABLED:
                *profile_id = 28;
                break;
            case BCM_COSQ_DELAY_TOLERANCE_100G_LOW_DELAY:
                *profile_id = 29;
                break;
            case BCM_COSQ_DELAY_TOLERANCE_200G_SLOW_ENABLED:
                *profile_id = 30;
                break;
            case BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY:
                *profile_id = 31;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected delay_tolerance_level %d.\n", delay_tolerance_level);
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * Verify Input parameters of bcm_dnx_cosq_delay_tolerance_level_set().
 * Refer to bcm_dnx_cosq_delay_tolerance_level_set() for additional details
 */
static int
dnx_cosq_delay_tolerance_level_set_verify(
    int unit,
    int delay_tolerance_level,
    bcm_cosq_delay_tolerance_t * delay_tolerance)
{
    int profile_id;
    uint32 supported_flags;
    int max_credit_balance_resolution = 4;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(delay_tolerance, _SHR_E_PARAM, "delay_tolerance");

    /** Verify per use case */
    /*
     * Use Case 1 - per credit request profile thresholds
     */
    if (delay_tolerance_level != BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG)
    {
        /*
         * profile id
         */
        SHR_IF_ERR_EXIT(dnx_iqs_api_delay_tolerance_level_to_profile_id_get(unit, delay_tolerance_level, &profile_id));

        /*
         * Verify thresholds
         */
        /** flags */
        /** backward compatibility error*/
        if (delay_tolerance->flags & BCM_COSQ_DELAY_TOLERANCE_BANDWIDTH_PROFILE_HIGH)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "high bandwidth flag is not supported, instead use bandwidth_level field");
        }
        supported_flags = BCM_COSQ_DELAY_TOLERANCE_IS_LOW_LATENCY | BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY |
            BCM_COSQ_DELAY_TOLERANCE_PUSH_QUEUE;
        if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == 0)
        {
            supported_flags |= BCM_COSQ_DELAY_TOLERANCE_HIGH_Q_PRIORITY;
        }
        SHR_MASK_VERIFY(delay_tolerance->flags, supported_flags, _SHR_E_PARAM,
                        "some of the flags are not supported.\n");

        if (delay_tolerance->flags & BCM_COSQ_DELAY_TOLERANCE_PUSH_QUEUE)
        {
            uint32 push_queue_profile, push_queue_enable;
            SHR_IF_ERR_EXIT(dnx_iqs_push_queue_get(unit, &push_queue_profile, &push_queue_enable));
            if (push_queue_enable && push_queue_profile != profile_id)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "push queue profile already exists\n");
            }
        }

        /** SATISFIED BACK THRESHOLDS */
        SHR_RANGE_VERIFY(delay_tolerance->credit_request_satisfied_backoff_enter_thresh,
                         dnx_data_iqs.credit.satisified_back_min_get(unit) * max_credit_balance_resolution,
                         dnx_data_iqs.credit.satisified_back_max_get(unit) * max_credit_balance_resolution,
                         _SHR_E_PARAM, "satisfied backoff enter is out of bound.\n");
        SHR_RANGE_VERIFY(delay_tolerance->credit_request_satisfied_backoff_exit_thresh,
                         dnx_data_iqs.credit.satisified_back_min_get(unit) * max_credit_balance_resolution,
                         dnx_data_iqs.credit.satisified_back_max_get(unit) * max_credit_balance_resolution,
                         _SHR_E_PARAM, "satisfied backoff exit is out of bound.\n");
        SHR_RANGE_VERIFY(delay_tolerance->credit_request_satisfied_backlog_enter_thresh,
                         dnx_data_iqs.credit.satisified_back_min_get(unit) * max_credit_balance_resolution,
                         dnx_data_iqs.credit.satisified_back_max_get(unit) * max_credit_balance_resolution,
                         _SHR_E_PARAM, "satisfied backlog enter is out of bound.\n");
        SHR_RANGE_VERIFY(delay_tolerance->credit_request_satisfied_backlog_exit_thresh,
                         dnx_data_iqs.credit.satisified_back_min_get(unit) * max_credit_balance_resolution,
                         dnx_data_iqs.credit.satisified_back_max_get(unit) * max_credit_balance_resolution,
                         _SHR_E_PARAM, "satisfied backlog exit is out of bound.\n");
        SHR_RANGE_VERIFY(delay_tolerance->credit_request_satisfied_backslow_enter_thresh,
                         dnx_data_iqs.credit.satisified_back_min_get(unit) * max_credit_balance_resolution,
                         dnx_data_iqs.credit.satisified_back_max_get(unit) * max_credit_balance_resolution,
                         _SHR_E_PARAM, "satisfied backslow enter is out of bound.\n");
        SHR_RANGE_VERIFY(delay_tolerance->credit_request_satisfied_backslow_exit_thresh,
                         dnx_data_iqs.credit.satisified_back_min_get(unit) * max_credit_balance_resolution,
                         dnx_data_iqs.credit.satisified_back_max_get(unit) * max_credit_balance_resolution,
                         _SHR_E_PARAM, "satisfied backslow exit is out of bound.\n");

        /** SATISFIED EMPTY THRESHOLDS */
        SHR_RANGE_VERIFY(delay_tolerance->credit_request_satisfied_empty_queue_thresh,
                         dnx_data_iqs.credit.satisified_empty_min_get(unit) * max_credit_balance_resolution,
                         dnx_data_iqs.credit.satisified_empty_max_get(unit) * max_credit_balance_resolution,
                         _SHR_E_PARAM, "satisfied empty queue is out of bound.\n");
        SHR_RANGE_VERIFY(delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh,
                         dnx_data_iqs.credit.satisified_empty_min_get(unit) * max_credit_balance_resolution,
                         dnx_data_iqs.credit.satisified_empty_max_get(unit) * max_credit_balance_resolution,
                         _SHR_E_PARAM, "satisfied empty queue max balance is out of bound.\n");
        /** empty exceed is true / false nothing to verify */

        /** HUNGRY THRESHOLDS */
        SHR_RANGE_VERIFY(delay_tolerance->credit_request_hungry_off_to_slow_thresh,
                         dnx_data_iqs.credit.hungry_min_get(unit) * max_credit_balance_resolution,
                         dnx_data_iqs.credit.hungry_max_get(unit) * max_credit_balance_resolution,
                         _SHR_E_PARAM, "hungry off to slow is out of bound.\n");
        SHR_RANGE_VERIFY(delay_tolerance->credit_request_hungry_off_to_normal_thresh,
                         dnx_data_iqs.credit.hungry_min_get(unit) * max_credit_balance_resolution,
                         dnx_data_iqs.credit.hungry_max_get(unit) * max_credit_balance_resolution,
                         _SHR_E_PARAM, "hungry off to normal is out of bound.\n");
        SHR_RANGE_VERIFY(delay_tolerance->credit_request_hungry_slow_to_normal_thresh,
                         dnx_data_iqs.credit.hungry_min_get(unit) * max_credit_balance_resolution,
                         dnx_data_iqs.credit.hungry_max_get(unit) * max_credit_balance_resolution,
                         _SHR_E_PARAM, "hungry slow to normal is out of bound.\n");
        SHR_RANGE_VERIFY(delay_tolerance->credit_request_hungry_normal_to_slow_thresh,
                         dnx_data_iqs.credit.hungry_min_get(unit) * max_credit_balance_resolution,
                         dnx_data_iqs.credit.hungry_max_get(unit) * max_credit_balance_resolution,
                         _SHR_E_PARAM, "hungry normal to slow is out of bound.\n");
        SHR_RANGE_VERIFY(delay_tolerance->credit_request_hungry_multiplier,
                         (delay_tolerance->flags & BCM_COSQ_DELAY_TOLERANCE_PUSH_QUEUE ? 0 :
                          dnx_data_iqs.credit.hungry_mult_min_get(unit)) * max_credit_balance_resolution,
                         dnx_data_iqs.credit.hungry_mult_max_get(unit) * max_credit_balance_resolution,
                         _SHR_E_PARAM, "hungry mult is out of bound.\n");

        SHR_RANGE_VERIFY(delay_tolerance->bandwidth_level,
                         0, dnx_data_iqs.credit.nof_bw_levels_get(unit) - 1,
                         _SHR_E_PARAM, "bandwidth level is out of bound.\n");

        /** CREDIT WATCHDOG - DELETE */
        if (dnx_data_iqs.credit.feature_get(unit, dnx_data_iqs_credit_watchdog_fixed_period))
        {
            if (delay_tolerance->flags & BCM_COSQ_DELAY_TOLERANCE_PUSH_QUEUE)
            {
                if (delay_tolerance->credit_request_watchdog_delete_queue_thresh != 10000 &&
                    delay_tolerance->credit_request_watchdog_delete_queue_thresh != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Credit watchdog delete threshold should either be disabled (0) or be fixed to 10000\n");

                }
            }

            else
            {
                /*
                 * Only fixed value allowed - used to avoid from an issue occurs while a queue existing from delete state
                 */
                SHR_VAL_VERIFY(delay_tolerance->credit_request_watchdog_delete_queue_thresh,
                               10000, _SHR_E_PARAM, "Credit watchdog delete threshold should be fixed to 10000.\n");
            }
        }

    }
    /*
     * Use case 2 - common (for all profiles) credit watchdog thresholds
     */
    else
    {
        /** Verify retransmit threshold */
        switch (delay_tolerance->credit_request_watchdog_status_msg_gen)
        {
            case 0:
            case 125:
            case 250:
            case 500:
            case 1000:
            case 2000:
            case 4000:
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid common status message time %d, must be one of 0, 125, 250, 500, 1000, 2000, 4000",
                             delay_tolerance->credit_request_watchdog_status_msg_gen);
                break;
        }

    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * Use cases:
 *
 * 1.
 * Set a set of thresholds of credit request profile (which includes also delete credit watchdog thresholds).
 * See Parameters for details about the threshold.
 * A credit request profile is disable by default and must be set before assigning a queue to the profile,
 * A set of recommended thresholds can be retrieved using bcm_dnx_cosq_delay_tolerance_preset_get().

 * 2.
 * Special use case of this function:
 * Getting retransmit credit watchdog threshold common to all profile IDs.
 * Parameters:
 * - delay tolerance level - Set to BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG.
 * - delay_tolerance.credit_request_watchdog_status_msg_gen - relevant field in output structure.
 *
 *
 * \param [in] unit -  Unit-ID
 * \param [in] delay_tolerance_level - credit request profile id
 * \param [in] delay_tolerance - Set of thresholds - see description of structure for details about the thresholds.
 *             delay_tolerance.flags - supported flags:
 *             BCM_COSQ_DELAY_TOLERANCE_HIGH_Q_PRIORITY - set for high priority queues (otherwise low priority queues)
 *             BCM_COSQ_DELAY_TOLERANCE_BANDWIDTH_PROFILE_HIGH -  set for high bandwidth queues (used for congested scenarios).
 *             BCM_COSQ_DELAY_TOLERANCE_IS_LOW_LATENCY - set for low latency queues.
 *             BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY - SRAM only queues.
 *             BCM_COSQ_DELAY_TOLERANCE_PUSH_QUEUES - Used for TDM queues (no credits are needed nor requested for the queue). Note that only one profile can be marked as push queue.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_delay_tolerance_level_set(
    int unit,
    int delay_tolerance_level,
    bcm_cosq_delay_tolerance_t * delay_tolerance)
{
    int profile_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * High Level Verify
     * Additional verification in functionals APIs.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_delay_tolerance_level_set_verify(unit, delay_tolerance_level, delay_tolerance));

    /** Implementation per use case */

    /*
     * Use case 1: Credit request profile - set of thresholds per profile
     */
    if (delay_tolerance_level != BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG)
    {
        /** Extract parameters */
        SHR_IF_ERR_EXIT(dnx_iqs_api_delay_tolerance_level_to_profile_id_get(unit, delay_tolerance_level, &profile_id));

        if (delay_tolerance->flags & BCM_COSQ_DELAY_TOLERANCE_PUSH_QUEUE)
        {
            SHR_IF_ERR_EXIT(dnx_iqs_push_queue_set(unit, profile_id, TRUE));
        }
        else
        {
            uint32 push_queue_profile, push_queue_enable;
            SHR_IF_ERR_EXIT(dnx_iqs_push_queue_get(unit, &push_queue_profile, &push_queue_enable));
            if (profile_id == push_queue_profile && push_queue_enable == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_iqs_push_queue_set(unit, 0, FALSE));
            }
        }

        /** Call to functional API to configure credit request thresholds*/
        SHR_IF_ERR_EXIT(dnx_iqs_credit_request_profile_set(unit, profile_id, delay_tolerance));

        /** Call to functional APIs to configure credit watchdog thresholds*/
        SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_delete_sw_set
                        (unit, profile_id, delay_tolerance->credit_request_watchdog_delete_queue_thresh));
        SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_hw_set(unit));
    }

    /*
     * Use case 2: Credit watchdog - common threshold for all profiles
     */
    else
    {
        /** update data base */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_retransmit_sw_set
                        (unit, delay_tolerance->credit_request_watchdog_status_msg_gen));

        /** set hw accordingly */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_hw_set(unit));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * Verify Input parameters of bcm_dnx_cosq_delay_tolerance_level_get().
 * Refer to bcm_dnx_cosq_delay_tolerance_level_get() for additional details
 */
static int
dnx_cosq_delay_tolerance_level_get_verify(
    int unit,
    int delay_tolerance_level,
    bcm_cosq_delay_tolerance_t * delay_tolerance)
{
    int profile_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(delay_tolerance, _SHR_E_PARAM, "delay_tolerance");

    /** Verify per use case */
    /*
     * Use Case 1 - per credit request profile thresholds
     */
    if (delay_tolerance_level != BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG)
    {
        /*
         * profile id
         */
        SHR_IF_ERR_EXIT(dnx_iqs_api_delay_tolerance_level_to_profile_id_get(unit, delay_tolerance_level, &profile_id));
        /** validate profile_id */

        SHR_IF_ERR_EXIT(dnx_iqs_credit_request_profile_valid_verify(unit, profile_id));
    }
    /*
     * Use case 2 - common (for all profiles) credit watchdog thresholds
     */
    else
    {
        /** Nothing to verify */
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get a set of thresholds of credit request profile.
 * See Parameters for details about the threshold.
 * A credit request profile is disable by default and must be set before assigning a queue to the profile,
 * A set of recommended thresholds can be retrieved using bcm_dnx_cosq_delay_tolerance_preset_get().
 *
 * \param [in] unit -  Unit-ID
 * \param [in] delay_tolerance_level - credit request profile id
 * \param [in] delay_tolerance - Set of thresholds - see description of structure for details about the thresholds.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_delay_tolerance_level_get(
    int unit,
    int delay_tolerance_level,
    bcm_cosq_delay_tolerance_t * delay_tolerance)
{
    int profile_id;
    uint32 push_queue_profile, push_queue_enable;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * High Level Verify
     * Additional verification in functionals APIs.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_delay_tolerance_level_get_verify(unit, delay_tolerance_level, delay_tolerance));

    /** Clear structure */
    sal_memset(delay_tolerance, 0, sizeof(bcm_cosq_delay_tolerance_t));

    /*
     * Use case 1: Credit request profile - set of thresholds per profile
     */
    if (delay_tolerance_level != BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG)
    {
        /** Extract parameters */
        SHR_IF_ERR_EXIT(dnx_iqs_api_delay_tolerance_level_to_profile_id_get(unit, delay_tolerance_level, &profile_id));

        /** Call to functional API to configure credit request thresholds*/
        SHR_IF_ERR_EXIT(dnx_iqs_credit_request_profile_get(unit, profile_id, delay_tolerance));

        /** Call to functional APIs to configure credit watchdog thresholds*/
        SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_delete_sw_get
                        (unit, profile_id, &delay_tolerance->credit_request_watchdog_delete_queue_thresh));

        SHR_IF_ERR_EXIT(dnx_iqs_push_queue_get(unit, &push_queue_profile, &push_queue_enable));
        if (profile_id == push_queue_profile && push_queue_enable == TRUE)
        {
            delay_tolerance->flags |= BCM_COSQ_DELAY_TOLERANCE_PUSH_QUEUE;
        }
    }

    /*
     * Use case 2: Credit watchdog - common threshold for all profiles
     */
    else
    {
        /** update data base */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_retransmit_sw_get
                        (unit, &delay_tolerance->credit_request_watchdog_status_msg_gen));

    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_cosq_delay_tolerance_preset_get_verify(
    int unit,
    bcm_cosq_delay_tolerance_preset_attr_t * preset_attr,
    bcm_cosq_delay_tolerance_t * delay_tolerance)
{
    uint32 push_queue_supported_flags = BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_PUSH_QUEUE |
        BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_OCB_ONLY | BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_HIGH_PRIORITY;
    uint32 all_supported_flags = push_queue_supported_flags |
        BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_SLOW_ENABLED | BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_MULTICAST;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(preset_attr, _SHR_E_PARAM, "preset_attr");
    SHR_NULL_CHECK(delay_tolerance, _SHR_E_PARAM, "delay_tolerance");

    if (preset_attr->flags & BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_PUSH_QUEUE)
    {

        SHR_MASK_VERIFY(preset_attr->flags, push_queue_supported_flags, _SHR_E_PARAM,
                        "Only BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_OCB_ONLY and BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_HIGH_PRIORITY can be provided with BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_PUSH_QUEUE.\n");
        /** rate */
        SHR_VAL_VERIFY(preset_attr->rate, 0, _SHR_E_PARAM,
                       "Rate should be 0 when BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_PUSH_QUEUE is provided.\n");
    }
    else
    {
        /** rate */
        if ((preset_attr->rate <= 0)
            || (preset_attr->rate > dnx_data_iqs.credit.max_rate_profile_preset_gbps_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid rate value  %d.\n", preset_attr->rate);
        }

        /** credit size */
        SHR_RANGE_VERIFY(preset_attr->credit_size, 1, dnx_data_iqs.credit.max_worth_get(unit), _SHR_E_PARAM,
                         "credit size out of bound.\n");

        /** flags */
        SHR_MASK_VERIFY(preset_attr->flags, all_supported_flags, _SHR_E_PARAM, "flags are not supported.\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *  Get a set of recommended thresholds.
 *  This set of thresholds could be assign to credit request profile by API bcm_cosq_delay_tolerance_level_set().
 *  This API might be optimized in SDK release (and therefore implemented in "tune" module).
 *
 * \param [in] unit -  Unit-ID
 * \param [in] preset_attr - required queue attributes
 * \param [in] delay_tolerance - recommended threholds for the required attributes
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_delay_tolerance_preset_get(
    int unit,
    bcm_cosq_delay_tolerance_preset_attr_t * preset_attr,
    bcm_cosq_delay_tolerance_t * delay_tolerance)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_delay_tolerance_preset_get_verify(unit, preset_attr, delay_tolerance));

    /** get the values from tune function */
    SHR_IF_ERR_EXIT(dnx_tune_iqs_delay_tolerance_preset_get(unit, preset_attr, delay_tolerance));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify Input parameters of bcm_dnx_cosq_dest_credit_size_get().
 * Refer to bcm_dnx_cosq_dest_credit_size_get() for additional details
 */
static int
dnx_cosq_dest_credit_size_set_verify(
    int unit,
    bcm_module_t dest_modid,
    uint32 credit_size)
{
    int is_local;

    SHR_FUNC_INIT_VARS(unit);
    /** Verify dest id */
    SHR_RANGE_VERIFY(dest_modid, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM,
                     "dest_modid out of bound.\n");

    /** Configuring local credit size is not supported */
    SHR_IF_ERR_EXIT(dnx_stk_sys_modid_is_local_get(unit, dest_modid, &is_local));
    if (is_local)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Configuring local credit size is not supported.\n");
    }

    /** verify credit size */
    SHR_RANGE_VERIFY(credit_size, 1, dnx_data_iqs.credit.max_worth_get(unit), _SHR_E_PARAM,
                     "credit size out of bound.\n");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * Set credit worth per each remote modid (FAP-ID).
 * Credit worth can be either local credit worth or default remote credit worth (configured by API bcm_fabric_control_set())
 *
 * \param [in] unit -  Unit-ID
 * \param [in] dest_modid - remote modid - up to 2k - 1
 * \param [in] credit_size - Credit worth in bytes.
 *                           Must be equal to local credit worth or default remote credit worth.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_dest_credit_size_set(
    int unit,
    bcm_module_t dest_modid,
    uint32 credit_size)
{

    uint32 local_credit_size;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_dest_credit_size_set_verify(unit, dest_modid, credit_size));

    /** in case required credit size is identical to local credit size - assign FAP-ID to local group */
    local_credit_size = dnx_data_iqs.credit.worth_get(unit);
    if (credit_size == local_credit_size)
    {
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_map_set(unit, dest_modid, 0));
    }
    /** Otherwise reset the other group and assign FAP-ID to this group */
    else
    {
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_map_set(unit, dest_modid, 1));
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_set(unit, 1, credit_size));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * Verify Input parameters of bcm_dnx_cosq_dest_credit_size_get().
 * Refer to bcm_dnx_cosq_dest_credit_size_get() for additional details
 */
static int
dnx_cosq_dest_credit_size_get_verify(
    int unit,
    bcm_module_t dest_modid,
    uint32 *credit_size)
{
    SHR_FUNC_INIT_VARS(unit);
    /** Verify dest id */
    SHR_RANGE_VERIFY(dest_modid, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM,
                     "dest_modid out of bound");

    /** verify credit size */
    SHR_NULL_CHECK(credit_size, _SHR_E_PARAM, "credit_size");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * Get credit worth per each remote modid (FAP-ID).
 *
 * \param [in] unit -  Unit-ID
 * \param [in] dest_modid - remote modid - up to 2k - 1
 * \param [out] credit_size - Credit worth in bytes.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_dest_credit_size_get(
    int unit,
    bcm_module_t dest_modid,
    uint32 *credit_size)
{
    uint32 group_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_dest_credit_size_get_verify(unit, dest_modid, credit_size));

    /** Get mapping FAP_ID -> GROUP_ID -> CREDIT_WORTH */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_map_get(unit, dest_modid, &group_id));
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, group_id, credit_size));

exit:
    SHR_FUNC_EXIT;
}
/*
 * brief - verify function for dnx_iqs_queue_priority_set
 */
static shr_error_e
dnx_iqs_queue_priority_set_verify(
    int unit,
    int priority)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is not supported for this device type.\n");
    }

    SHR_RANGE_VERIFY(priority, 0, dnx_data_iqs.dqcq.nof_priorities_get(unit) - 1, _SHR_E_PARAM,
                     "Priority out of bound");
exit:
    SHR_FUNC_EXIT;

}

/*
 * see .h file
 */
shr_error_e
dnx_iqs_queue_priority_set(
    int unit,
    bcm_gport_t voq_gport,
    bcm_cos_queue_t cosq,
    int priority)
{
    int core, force_core_all = FALSE;
    int voq;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify gport and cosq, get VOQ
     */
    SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, voq_gport, cosq, force_core_all, &core, &voq));

    /*
     * Verify priority
     */
    SHR_INVOKE_VERIFY_DNX(dnx_iqs_queue_priority_set_verify(unit, priority));

    /*
     * Configure priority in HW
     */
    SHR_IF_ERR_EXIT(dnx_iqs_queue_to_priority_map_set(unit, core, voq, priority));

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_iqs_queue_priority_get
 */
static shr_error_e
dnx_iqs_queue_priority_get_verify(
    int unit,
    int *priority)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is not supported for this device type.\n");
    }

    SHR_NULL_CHECK(priority, _SHR_E_PARAM, "priority");
exit:
    SHR_FUNC_EXIT;

}

/*
 * see .h file
 */
shr_error_e
dnx_iqs_queue_priority_get(
    int unit,
    bcm_gport_t voq_gport,
    bcm_cos_queue_t cosq,
    int *priority)
{
    int core, force_core_all = FALSE;
    int voq;
    uint32 hw_priority;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify gport and cosq, get VOQ
     */
    SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, voq_gport, cosq, force_core_all, &core, &voq));

    /*
     * Verify priority
     */
    SHR_INVOKE_VERIFY_DNX(dnx_iqs_queue_priority_get_verify(unit, priority));

    /*
     * Get priority from HW
     */
    SHR_IF_ERR_EXIT(dnx_iqs_queue_to_priority_map_get(unit, core, voq, &hw_priority));
    *priority = hw_priority;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_iqs_queue_flush_timeout_verify(
    int unit,
    int timeout)
{
    SHR_FUNC_INIT_VARS(unit);

    if (timeout < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Timeout %d is invalid.\n", timeout);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_iqs_queue_flush_set(
    int unit,
    bcm_gport_t voq_gport,
    bcm_cos_queue_t cosq,
    int timeout)
{
    int voq, core, force_core_all = FALSE;
    uint32 flush_trigger, is_del_dqcq, byte_units_to_send;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify gport and cosq, get VOQ
     */
    SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, voq_gport, cosq, force_core_all, &core, &voq));

    /*
     * Verify timeout
     */
    SHR_INVOKE_VERIFY_DNX(dnx_iqs_queue_flush_timeout_verify(unit, timeout));

    /*
     * Set queue id to flush
     */
    SHR_IF_ERR_EXIT(dnx_iqs_queue_flush_queue_id_set(unit, core, voq));

    /*
     * Enable flush
     */
    is_del_dqcq = TRUE;
    flush_trigger = TRUE;
    byte_units_to_send = dnx_data_iqs.flush.bytes_units_to_send_get(unit);
    SHR_IF_ERR_EXIT(dnx_iqs_queue_flush_attributes_set(unit, core, is_del_dqcq, byte_units_to_send, flush_trigger));

    if (timeout)
    {
        SHR_IF_ERR_EXIT(dnx_iqs_queue_flush_trigger_down_polling(unit, core, timeout));
    }

exit:
    SHR_FUNC_EXIT;
}

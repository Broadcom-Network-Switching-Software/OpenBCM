/** \file meter_utils.c
 * 
 *  policer module gather all functionality that related to the policer/meter.
 *  meter_utils.c holds all common functinality for the generic and global meter. (most likely in the profile elements calculations)
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_POLICER

/*
 * Include files.
 * {
 */
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/policer.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>
#include "meter_utils.h"
#include <bcm_int/dnx/policer/policer_mgmt.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/meter_access.h>
#include <bcm_int/dnx/policer/policer_mgmt.h>

/*
 * }
 */

/*
 * DEFINEs/MACROs
 * {
 */
#define DNX_METER_UTILS_RATE_ERROR_MAX (0xFFFFFFFF)
/*
 * }
 */

/*
 * GLOBALS
 * {
 */

/*
 * }
 */

/** see .h file */
shr_error_e
dnx_meter_utils_bucket_size_adjust(
    int unit,
    uint32 rate1_man,
    uint32 rate1_exp,
    uint32 rate2_man,
    uint32 rate2_exp,
    uint32 *bucket_man,
    uint32 *bucket_exp)
{
    uint32 bucket_size, bucket_size_new, bucket_size_min, bucket_size_max;
    uint32 chunk1, chunk2, chunk_size;
    uint32 bucket_exp_internal;
    uint32 max_cbs_exp, max_cbs_mant;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_cbs_mant.get(unit, &max_cbs_mant));
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_cbs_exp.get(unit, &max_cbs_exp));

    bucket_exp_internal = *bucket_exp;
    chunk1 = (rate1_man) << rate1_exp;
    chunk2 = (rate2_man) << rate2_exp;

    /** chunk1 represent the own rate of the bucket */
    /** chunk2 represent the max rate of the bucket (own+others) */
    /** use the max chunk for calculation */
    chunk_size = UTILEX_MAX(chunk1, chunk2);
    bucket_size = (*bucket_man) << (bucket_exp_internal);
    if (bucket_size < chunk_size)
    {
        *bucket_man = 0;
        bucket_exp_internal = 0;
        /** calculate the exp */
        while (bucket_exp_internal <= max_cbs_exp)
        {
            bucket_size_min = (1) << (bucket_exp_internal);
            bucket_size_max = (max_cbs_mant) << (bucket_exp_internal);
            if ((bucket_size_min <= chunk_size) && (bucket_size_max >= chunk_size))
            {
                break;
            }
            bucket_exp_internal = bucket_exp_internal + 1;
        }
        /** if the chunk is too high (theoretical case), set the max value for bucket size*/
        if (bucket_exp_internal > max_cbs_exp)
        {
            *bucket_man = max_cbs_mant;
            *bucket_exp = max_cbs_exp;
        }
        else
        {
            /** set the exp part */
            *bucket_exp = bucket_exp_internal;
            /** calc the mantissa */
            *bucket_man = chunk_size >> bucket_exp_internal;
            bucket_size_new = (*bucket_man) << (bucket_exp_internal);
            if (bucket_size_new < chunk_size)
            {
                *bucket_man = *bucket_man + 1;
            }
        }

        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "WARNING: bucket size has changed!!!"
                              "Orginal value: %d [bytes]\n" "Actual value: %d [bytes]\n"), bucket_size, chunk_size));
    }
exit:
    SHR_FUNC_EXIT;
}

/** see.h file */
shr_error_e
dnx_meter_utils_profile_burst_to_exp_mnt(
    int unit,
    uint32 burst,
    int is_large_bucket_mode,
    uint32 *mnt,
    uint32 *exp)
{
    uint32 max_cbs_exp, max_cbs_mant;
    uint32 max_burst_normal_mode;
    int devision;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_burst_normal_mode.get(unit, &max_burst_normal_mode));
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_cbs_exp.get(unit, &max_cbs_exp));
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_cbs_mant.get(unit, &max_cbs_mant));

    if (is_large_bucket_mode == TRUE)
    {
        max_cbs_exp = max_cbs_exp + dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit);
    }

    SHR_IF_ERR_EXIT(utilex_break_complex_to_mnt_exp_round_down(burst,
                                                               max_cbs_mant,
                                                               max_cbs_exp,
                                                               DNX_METER_UTILS_BURST_MULTI_CONST_VAL,
                                                               DNX_METER_UTILS_BURST_MANT_CONST_VAL, mnt, exp));

    /** align back the exp value if the burst size match to large_bucket mode */
    if (is_large_bucket_mode == TRUE)
    {
        if (*exp >= dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit))
        {
            *exp = (*exp - dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit));
        }
        /** handle a case of force large bucket mode, and the burst required is not in large bucket mode.
            In such case, need to used the closest burst value in large bucket mode. It may be not the same as the user asked */
        else
        {
            /** the exp is the min value, and the mnt will compensate it as much as possible */
            devision = utilex_power_of_2((dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit) - *exp));
            if (devision == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  devision value(=%d)\n", devision);
            }
            *mnt = *mnt / devision;
            *exp = 0;
            if (*mnt == 0)
            {
                *mnt = 1;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see.h file */
shr_error_e
dnx_meter_utils_min_max_rate_per_rev_exp_get(
    int unit,
    uint32 rev_exp,
    uint32 *min_rate,
    uint32 *max_rate)
{
    uint32 core_clock_freq;
    uint32 max_ir_mant, max_ir_exp;
    SHR_FUNC_INIT_VARS(unit);

    core_clock_freq = dnx_data_device.general.core_clock_khz_get(unit) * 1000;

    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_ir_mant.get(unit, &max_ir_mant));
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_ir_exp.get(unit, &max_ir_exp));

    /** calculate min_rate and max_rate for this res (rev_exp) */
    SHR_IF_ERR_EXIT(dnx_meter_utils_compute_complex_from_mnt_exp_reverse_exp(unit, max_ir_mant,
                                                                             max_ir_exp,
                                                                             rev_exp,
                                                                             core_clock_freq,
                                                                             DNX_METER_UTILS_RATE_MANT_CONST_VAL,
                                                                             FALSE, TRUE, max_rate));

    SHR_IF_ERR_EXIT(dnx_meter_utils_compute_complex_from_mnt_exp_reverse_exp(unit, DNX_METER_UTILS_RATE_MANT_MIN_VAL,
                                                                             DNX_METER_UTILS_RATE_EXP_MIN_VAL,
                                                                             rev_exp,
                                                                             core_clock_freq,
                                                                             DNX_METER_UTILS_RATE_MANT_CONST_VAL,
                                                                             FALSE, TRUE, min_rate));

exit:
    SHR_FUNC_EXIT;
}

 /** see .h file*/
shr_error_e
dnx_utils_limits_profile_info_init(
    int unit)
{
    dbal_table_field_info_t field_info;
    uint32 core_clock_freq = dnx_data_device.general.core_clock_khz_get(unit) * 1000;
    uint32 max_cbs_mant, max_cbs_exp, max_burst_normal_mode, max_burst_large_mode;
    uint32 min_rate, max_rate, max_rate_verify = 0xFFFFFFFF;
    uint32 max_ir_mant, max_ir_exp;
    int temp_exp;
    shr_error_e retval;
    SHR_FUNC_INIT_VARS(unit);

    /** calculate max burst size and max cbs mantissa and exponent */
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_ING_PROFILE_CONFIG, DBAL_FIELD_CBS_MANT,
                                               FALSE, 0, 0, &field_info));
    max_cbs_mant = field_info.max_value;
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_cbs_mant.set(unit, max_cbs_mant));
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_ING_PROFILE_CONFIG, DBAL_FIELD_CBS_EXP, FALSE,
                                               0, 0, &field_info));
    max_cbs_exp = field_info.max_value;
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_cbs_exp.set(unit, max_cbs_exp));

    SHR_IF_ERR_EXIT(utilex_compute_complex_to_mnt_exp(max_cbs_mant,
                                                      max_cbs_exp,
                                                      DNX_METER_UTILS_BURST_MULTI_CONST_VAL,
                                                      DNX_METER_UTILS_BURST_MANT_CONST_VAL, &max_burst_normal_mode));

    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_burst_normal_mode.set(unit, max_burst_normal_mode));

    SHR_IF_ERR_EXIT(utilex_compute_complex_to_mnt_exp(max_cbs_mant,
                                                      (max_cbs_exp +
                                                       dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit)),
                                                      DNX_METER_UTILS_BURST_MULTI_CONST_VAL,
                                                      DNX_METER_UTILS_BURST_MANT_CONST_VAL, &max_burst_large_mode));

    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_burst_large_mode.set(unit, max_burst_large_mode));

    /** calculate max rate size and max rate mantissa and exponent and rev_exp*/
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_ING_PROFILE_CONFIG, DBAL_FIELD_CIR_MANT,
                                               FALSE, 0, 0, &field_info));
    max_ir_mant = field_info.max_value;
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_ir_mant.set(unit, max_ir_mant));

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_ING_PROFILE_CONFIG, DBAL_FIELD_CIR_MANT_EXP,
                                               FALSE, 0, 0, &field_info));
    max_ir_exp = field_info.max_value;
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_ir_exp.set(unit, max_ir_exp));

    /** calc the max rate allowed. the max rate determine by sw limitation: 
              max_rate is uint32 number, thefore, the limitation is the max value that the function will not return overflow of uint32.
              For clock_freq=1Ghz, the max should be 0xF23BB800 (close to 4Tbps) 
       */
    for (temp_exp = (int) max_ir_exp; temp_exp >= 0; temp_exp--)
    {
        retval = dnx_meter_utils_compute_complex_from_mnt_exp_reverse_exp(unit, max_ir_mant,
                                                                          (uint32) temp_exp,
                                                                          dnx_data_meter.profile.rev_exp_min_get(unit),
                                                                          core_clock_freq,
                                                                          DNX_METER_UTILS_RATE_MANT_CONST_VAL, FALSE,
                                                                          FALSE, &max_rate);
        if (retval == _SHR_E_NONE)
        {
            break;
        }
    }

    if (temp_exp < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "max rate calculation failed\n");
    }
    /** verify max_rate can be achieved with max_exp, and bigger rev_exp */
    /** This is order not to get to situation when we cross 2^32, when trying to calc best rev_exp */
    SHR_IF_ERR_EXIT(dnx_meter_utils_compute_complex_from_mnt_exp_reverse_exp(unit, max_ir_mant,
                                                                             (uint32) max_ir_exp,
                                                                             (dnx_data_meter.
                                                                              profile.rev_exp_min_get(unit) +
                                                                              max_ir_exp - temp_exp), core_clock_freq,
                                                                             DNX_METER_UTILS_RATE_MANT_CONST_VAL, FALSE,
                                                                             TRUE, &max_rate_verify));
    if (max_rate_verify < max_rate)
    {
        max_rate = max_rate_verify;
    }
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_rate.set(unit, max_rate));

     /** set min burst size */
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.min_burst.set
                    (unit, dnx_data_meter.profile.burst_size_min_get(unit)));

     /** calc min rate */
    SHR_IF_ERR_EXIT(dnx_meter_utils_compute_complex_from_mnt_exp_reverse_exp(unit, DNX_METER_UTILS_RATE_MANT_MIN_VAL,
                                                                             DNX_METER_UTILS_RATE_EXP_MIN_VAL,
                                                                             dnx_data_meter.
                                                                             profile.rev_exp_max_get(unit),
                                                                             core_clock_freq,
                                                                             DNX_METER_UTILS_RATE_MANT_CONST_VAL, FALSE,
                                                                             TRUE, &min_rate));
    /** rate zero is not an option. if user set rate zero, 
        driver enable disable_rate in the HW, the rate calculation is irelevant (except max_ir) */
    min_rate = (min_rate == 0) ? 1 : min_rate;
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.min_rate.set(unit, min_rate));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Calculate the highest shared rev_exp for: 
*   1) CIR and max CIR (when sharing is not enabled)
*   2) EIR and max EIR (when sharing is not enabled)
*   3) CIR, EIR, max CIR and max EIR (when sharing is enabled)  
* \param [in] unit -unit id
* \param [in] min_rate - min CIR/EIR 
* \param [in] max_rate - max CIR/EIR 
* \param [out] rev_exp -calculated reverse exponent
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_meter_utils_ir_val_to_max_rev_exp(
    int unit,
    uint32 min_rate,
    uint32 max_rate,
    uint32 *rev_exp)
{
    uint32 calc_min_rate, calc_max_rate;
    int tmp_rev_exp;
    uint32 max_rev_exp, min_rev_exp;
    SHR_FUNC_INIT_VARS(unit);

    max_rev_exp = dnx_data_meter.profile.rev_exp_max_get(unit);
    min_rev_exp = dnx_data_meter.profile.rev_exp_min_get(unit);

    for (tmp_rev_exp = max_rev_exp; tmp_rev_exp >= (int) min_rev_exp; tmp_rev_exp--)
    {
        /** calculate min_rate and max_rate for this res (rev_exp) */
        SHR_IF_ERR_EXIT(dnx_meter_utils_min_max_rate_per_rev_exp_get
                        (unit, tmp_rev_exp, &calc_min_rate, &calc_max_rate));

        /** if the max rate and min rate is in the boundries for this rev_exp - break */
        if (calc_max_rate >= max_rate)
        {
            if (calc_min_rate > min_rate)
            {
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                     "Metering warning- Large difference between parameters sharing rev_exp, this leads to high error in the smaller value.\n\r Original low value: %d, Configured low value: %d\n\r "),
                                          min_rate, calc_min_rate));
            }

            /** found max rev_exp */
            break;
        }
    }

    if (tmp_rev_exp < (int) min_rev_exp)
    {
        /** no shared rev_exp could be found for CIR and EIR */
        SHR_ERR_EXIT(_SHR_E_PARAM, " no shared rev_exp could be found \n");
    }

    *rev_exp = tmp_rev_exp;

exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*   calculate the min and max values in a given array
* \param [in] vals -pointer to array
* \param [in] nof_vals - array size
* \param [out] min_val - min val in the array
* \param [out] max_val -max val in the array
* \return
*   void
* \remark
*   NONE
* \see
*   NONE
*/
static void
dnx_meter_utils_max_min_vals_for_calc_get(
    uint32 *vals,
    uint32 nof_vals,
    uint32 *min_val,
    uint32 *max_val)
{
    int i = 0;
    *min_val = vals[i];
    *max_val = vals[i];
    for (i = 1; i < nof_vals; i++)
    {
        if (*min_val > vals[i])
        {
            *min_val = vals[i];
        }
        else if (*max_val < vals[i])
        {
            *max_val = vals[i];
        }
    }
}

/**
* \brief
*   calculate the error rate in 0.1% units
* \param [in] unit - unit number
* \param [in] expected_rate - expected_rate
* \param [in] actual_rate - actual rate
* \param [out] error_rate - error rate in 0.1%
* \param [out] valid_error_rate - TRUE, if the error rate is less than 1%
* \return
*   void
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_meter_utils_error_rate_calc_get(
    int unit,
    uint32 expected_rate,
    uint64 actual_rate,
    uint32 *error_rate,
    uint32 *valid_error_rate)
{
    uint64 expected_rate_u64, temp_u64;
    SHR_FUNC_INIT_VARS(unit);

    if (expected_rate == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " expected_rate(=%d)\n", expected_rate);
    }
    COMPILER_64_SET(expected_rate_u64, 0x0, expected_rate);
    COMPILER_64_UMUL_32(expected_rate_u64, DNX_METER_UTILS_KBITS_TO_BYTES);

    if (COMPILER_64_GT(expected_rate_u64, actual_rate))
    {
        COMPILER_64_COPY(temp_u64, expected_rate_u64);
        COMPILER_64_SUB_64(temp_u64, actual_rate);
    }
    else
    {
        COMPILER_64_COPY(temp_u64, actual_rate);
        COMPILER_64_SUB_64(temp_u64, expected_rate_u64);
    }
    /** calc error in 0.1 % units */
    COMPILER_64_UMUL_32(temp_u64, 1000);
    COMPILER_64_UDIV_64(temp_u64, expected_rate_u64);

    COMPILER_64_TO_32_LO((*error_rate), temp_u64);
    *valid_error_rate = ((*error_rate) <= DNX_METER_UTILS_ACCURACY_PERCENTAGE * 10) ? TRUE : FALSE;
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_utils_rev_exp_optimized_for_bucket_rate(
    int unit,
    uint32 *rates,
    uint32 nof_rates,
    uint32 *rev_exp)
{
    uint32 min_rate;
    uint32 max_rate;
    int tmp_rev_exp;
    uint32 lowest_available_rev_exp = dnx_data_meter.profile.rev_exp_min_get(unit);
    uint32 highest_available_rev_exp = 0;
    uint32 rate_idx = 0;
    uint32 temp_rate_errors[DNX_METER_UTILS_NOF_METER_CASCADE_MODE * DNX_METER_UTILS_PROFILE_MAX_RATES_FOR_CALC];
    uint32 selected_rev_exp = -1;
    uint32 max_ir_mant, max_ir_exp;
    uint32 core_clock_freq;
    uint32 error_average_prev, error_average_current; /** in 0.1% percantage */
    uint32 valid_error_for_all_prev_rates, valid_error_for_all_current_rates, valid_error;
    SHR_FUNC_INIT_VARS(unit);

    core_clock_freq = dnx_data_device.general.core_clock_khz_get(unit) * 1000;

    /** check the size of the input array */
    if (nof_rates <= 0 ||
        (DNX_METER_UTILS_NOF_METER_CASCADE_MODE * DNX_METER_UTILS_PROFILE_MAX_RATES_FOR_CALC) < nof_rates)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " invalid nof_rates(=%d)\n", nof_rates);
    }

    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_ir_mant.get(unit, &max_ir_mant));
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_ir_exp.get(unit, &max_ir_exp));

    /** Find min and max rates. */
    dnx_meter_utils_max_min_vals_for_calc_get(rates, nof_rates, &min_rate, &max_rate);

    /** First step is to calculate the smaller rev_exp where all rates are in range of this rev_exp. */
    SHR_IF_ERR_EXIT(dnx_meter_utils_ir_val_to_max_rev_exp(unit, min_rate, max_rate, &highest_available_rev_exp));

    selected_rev_exp = lowest_available_rev_exp;
    valid_error_for_all_prev_rates = FALSE;
    error_average_prev = DNX_METER_UTILS_RATE_ERROR_MAX;
    for (tmp_rev_exp = lowest_available_rev_exp; tmp_rev_exp <= highest_available_rev_exp; tmp_rev_exp++)
    {
        valid_error_for_all_current_rates = TRUE;
        for (rate_idx = 0; rate_idx < nof_rates; rate_idx++)
        {
            uint32 mant;
            uint32 exp;
            uint64 cfg_rate;

            /** Calculate the optimal (mantissa, exponent) for all rates with this rev_exp. */
            SHR_IF_ERR_EXIT(dnx_meter_utils_break_complex_to_mnt_exp(unit, rates[rate_idx],
                                                                     max_ir_mant,
                                                                     DNX_METER_UTILS_RATE_MANT_MIN_VAL,
                                                                     max_ir_exp,
                                                                     core_clock_freq,
                                                                     tmp_rev_exp,
                                                                     DNX_METER_UTILS_RATE_MANT_CONST_VAL, &mant, &exp));

            /** Calculate the configured rate. */
            SHR_IF_ERR_EXIT(dnx_meter_utils_compute_rate_u64_bytes(unit, mant,
                                                                   exp, tmp_rev_exp, core_clock_freq, &cfg_rate));
            /** save the error per rate. error is in 0.1% units */
            SHR_IF_ERR_EXIT(dnx_meter_utils_error_rate_calc_get
                            (unit, rates[rate_idx], cfg_rate, &temp_rate_errors[rate_idx], &valid_error));
            if (valid_error == FALSE)
            {
                valid_error_for_all_current_rates = FALSE;
            }
        }

        if (valid_error_for_all_current_rates == valid_error_for_all_prev_rates)
        {
            error_average_current = utilex_unsigned_integer_array_average_get(temp_rate_errors, nof_rates);
            /** if prev average error is better (or equal) then current - select it*/
            if (error_average_prev <= error_average_current)
            {
                continue;
            }
        }
        else if (valid_error_for_all_prev_rates == TRUE)
        {
            continue;
        }
        else
        {
            error_average_current = utilex_unsigned_integer_array_average_get(temp_rate_errors, nof_rates);
        }

        /** this rev_exp is the best so far, save it and its errors data */
        valid_error_for_all_prev_rates = valid_error_for_all_current_rates;
        /** save the average */
        error_average_prev = error_average_current;
        selected_rev_exp = tmp_rev_exp;
    }

    *rev_exp = selected_rev_exp;

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_utils_rate_from_mnt_exp_rev_exp_get(
    int unit,
    uint32 mnt,
    uint32 exp,
    uint32 rev_exp,
    uint32 *rate)
{
    uint32 core_clock_freq = dnx_data_device.general.core_clock_khz_get(unit) * 1000;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_meter_utils_compute_complex_from_mnt_exp_reverse_exp(unit, mnt, exp, rev_exp, core_clock_freq,
                                                                             DNX_METER_UTILS_RATE_MANT_CONST_VAL, TRUE,
                                                                             TRUE, rate));
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
meter_utils_large_bucket_mode_get(
    int unit,
    uint32 cbs,
    uint32 ebs,
    uint32 *is_large_bucket_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_large_bucket_mode = FALSE;

    if ((cbs > dnx_data_meter.profile.max_regular_bucket_mode_burst_get(unit)) ||
        (ebs > dnx_data_meter.profile.max_regular_bucket_mode_burst_get(unit)))
    {
        *is_large_bucket_mode = TRUE;
    }

/** exit: */
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_utils_break_complex_to_mnt_exp(
    int unit,
    uint32 rate,
    uint32 max_mnt,
    uint32 min_mnt,
    uint32 max_exp,
    uint32 clock_rate,
    uint32 exp_div,
    uint32 eq_const_mnt_inc,
    uint32 *mnt,
    uint32 *exp)
{
    uint64 tmp_64, div_u64, divisor_half, rate_u64;
    uint32 temp_exp, tmp, min_rate;
    SHR_FUNC_INIT_VARS(unit);

    /** Update-Rate = Global-Time >> Profile.Rev-Exp */
    /** Credit-Per-Update = (Profile.Mant << Profile.Mant-Exp) */
    /** Meter-Rate (bytes) = Update-Rate * Credit-Per-Update */

    if ((clock_rate == 0))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " invalid inputs parameters\n");

    }
    /** the function cannot handle exp_div>=32, because it will cause U64*U64 operation, which cannot be made. */
    if (exp_div >= 32)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " input parameter exp_div=%d. max allowed is 32\n", exp_div);
    }
    COMPILER_64_SET(rate_u64, 0x0, rate);
    COMPILER_64_UMUL_32(rate_u64, DNX_METER_UTILS_KBITS_TO_BYTES);

    for (temp_exp = 0; temp_exp <= max_exp; temp_exp++)
    {
        COMPILER_64_COPY(tmp_64, rate_u64);
        COMPILER_64_UMUL_32(tmp_64, (1 << exp_div));

        COMPILER_64_SET(div_u64, 0x0, (1 << temp_exp));
        /** global time is clock_rate / 2 */
        COMPILER_64_UMUL_32(div_u64, (clock_rate / 2));
        COMPILER_64_COPY(divisor_half, div_u64);
        COMPILER_64_UDIV_32(divisor_half, 2);
        COMPILER_64_ADD_64(tmp_64, divisor_half);
        COMPILER_64_UDIV_64(tmp_64, div_u64);
        if (COMPILER_64_HI(tmp_64) > 0) /* overflow */
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, " overflow during calculation \n");
        }
        COMPILER_64_TO_32_LO(tmp, tmp_64);

        if (eq_const_mnt_inc > tmp)
        {
            continue;
        }
        *mnt = tmp - eq_const_mnt_inc;
        if ((*mnt >= min_mnt) && (*mnt <= max_mnt))
        {
            *exp = temp_exp;
            break;
        }
    }

    /** if couldn't find exp and mant for the given rate, we will set the limits exp and mnt. */
    if (temp_exp > max_exp)
    {
        /** calc rate with the min exp and min mnt */
        SHR_IF_ERR_EXIT(dnx_meter_utils_compute_complex_from_mnt_exp_reverse_exp
                        (unit, min_mnt, 0, exp_div, clock_rate, eq_const_mnt_inc, TRUE, TRUE, &min_rate));
        /** if rate is smaller then minimum, set the min. else set the max */
        if (min_rate > rate)
        {
            *exp = 0;
            *mnt = 0;
        }
        else
        {
            *exp = max_exp;
            *mnt = max_mnt;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_utils_compute_complex_from_mnt_exp_reverse_exp(
    int unit,
    uint32 mnt,
    uint32 exp,
    uint32 rev_exp,
    uint32 clock_rate,
    uint32 eq_const_mnt_inc,
    uint32 round_to_closest,
    int print_error,
    uint32 *rate)
{
    uint64 rate_u64_bytes;

    SHR_FUNC_INIT_VARS(unit);

    if (rate == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " input parameter x is NULL \n");
    }
    SHR_IF_ERR_EXIT(dnx_meter_utils_compute_rate_u64_bytes(unit, mnt, exp, rev_exp, clock_rate, &rate_u64_bytes));
    if (round_to_closest)
    {
        COMPILER_64_ADD_32(rate_u64_bytes, (DNX_METER_UTILS_KBITS_TO_BYTES / 2));
    }
    COMPILER_64_UDIV_32(rate_u64_bytes, DNX_METER_UTILS_KBITS_TO_BYTES);
    if (COMPILER_64_HI(rate_u64_bytes) > 0)     /* overflow */
    {
        if (print_error)
        {
            SHR_ERR_EXIT(_SHR_E_LIMIT, "overflow during calculation \n");
        }
        else
        {
            _func_rv = _SHR_E_LIMIT;
            SHR_EXIT();
        }
    }
    COMPILER_64_TO_32_LO((*rate), rate_u64_bytes);

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_utils_compute_rate_u64_bytes(
    int unit,
    uint32 mnt,
    uint32 exp,
    uint32 rev_exp,
    uint32 clock_rate,
    uint64 *rate)
{
    uint64 numerator;
    uint32 credits_per_update, update_rate, global_time = clock_rate / 2;

    SHR_FUNC_INIT_VARS(unit);

    if (rate == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " input parameter x is NULL \n");
    }
    /** Update-Rate = Global-Time >> Profile.Rev-Exp */
    update_rate = global_time >> rev_exp;
    /** Credit-Per-Update = (Profile.Mant << Profile.Mant-Exp) */
    credits_per_update = mnt << exp;
    /** Meter-Rate (bytes) = Update-Rate * Credit-Per-Update */
    COMPILER_64_SET(numerator, 0x0, update_rate);
    COMPILER_64_UMUL_32(numerator, credits_per_update);
    COMPILER_64_COPY((*rate), numerator);

exit:
    SHR_FUNC_EXIT;
}

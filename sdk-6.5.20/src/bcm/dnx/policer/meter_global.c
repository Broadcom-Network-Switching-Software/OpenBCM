/** \file meter_global.c
 * 
 *  policer module gather all functionality that related to the policer/meter.
 *  meter_global.c holds all the global meter implemantation for DNX
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
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/policer.h>
#include <soc/dnx/dbal/dbal.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include "meter_utils.h"
#include "meter_global.h"
#include <bcm_int/dnx/policer/policer_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/meter_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/** see .h file */
shr_error_e
dnx_meter_global_profile_set(
    int unit,
    int core_id,
    int glbl_idx,
    dnx_meter_profile_info_t * profile_info)
{
    uint32 entry_handle_id;
    uint32 reverse_exponent = 0;
    uint32 rate_mantissa;
    uint32 rate_exp;
    uint32 burst_mantissa;
    uint32 burst_exp;
    uint32 cir = profile_info->cir;
    uint32 max_ir_mant, max_ir_exp;
    uint32 core_clock_freq;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** burst mantissa and exponent calculation */
    SHR_IF_ERR_EXIT(dnx_meter_utils_profile_burst_to_exp_mnt
                    (unit, profile_info->cbs, profile_info->is_large_bucket_mode, &burst_mantissa, &burst_exp));

    if (profile_info->is_large_bucket_mode == TRUE)
    {
        /** if large_bucket_mode_exp_add_get>32, utilex_power_of_2 will return zero and will cause zero division */
        /** but this is not the case */
         /* coverity[Division or modulo by zero :FALSE]  */
        cir = UTILEX_DIV_ROUND_UP
            (profile_info->cir, utilex_power_of_2(dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit)));
    }
    /** transfer user values (cir, cbs) to mnt, exp and res */
    core_clock_freq = dnx_data_device.general.core_clock_khz_get(unit) * 1000;
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_ir_mant.get(unit, &max_ir_mant));
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_ir_exp.get(unit, &max_ir_exp));
    if (profile_info->disable_cir == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_meter_utils_rev_exp_optimized_for_bucket_rate(unit, &cir, 1, &reverse_exponent));
    }
    /** calculate mnt and exp, based on res */
    SHR_IF_ERR_EXIT(dnx_meter_utils_break_complex_to_mnt_exp(unit, cir,
                                                             max_ir_mant,
                                                             DNX_METER_UTILS_RATE_MANT_MIN_VAL,
                                                             max_ir_exp,
                                                             core_clock_freq,
                                                             reverse_exponent,
                                                             DNX_METER_UTILS_RATE_MANT_CONST_VAL, &rate_mantissa,
                                                             &rate_exp));

    /** if the bucket size is too  small to get the credits of one chunk, increase it */
    SHR_IF_ERR_EXIT(dnx_meter_utils_bucket_size_adjust
                    (unit, rate_mantissa, rate_exp, 0, 0, &burst_mantissa, &burst_exp));
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_PROFILE_CONFIG, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_INDEX, glbl_idx);
    /** set the table values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COLOR_AWARE, INST_SINGLE, profile_info->color_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_MODE, INST_SINGLE,
                                 profile_info->is_packet_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUPLING_FLAG, INST_SINGLE,
                                 profile_info->is_coupling_enabled);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHARING_FLAG, INST_SINGLE,
                                 profile_info->is_sharing_enabled);
    /** for global, no way of disable cir, because the bucket will be always empty */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CIR_RESET, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CBS_MANT, INST_SINGLE, burst_mantissa);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CBS_EXP, INST_SINGLE, burst_exp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CIR_MANT, INST_SINGLE, rate_mantissa);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CIR_MANY_EXP, INST_SINGLE, rate_exp);
    /** althought max rate not relevant if sharing is disable, we set it as the cir rate */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_CIR_MANT, INST_SINGLE, rate_mantissa);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_CIR_MANT_EXP, INST_SINGLE, rate_exp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CIR_REV_EXP, INST_SINGLE, reverse_exponent);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COLOR_RSLV_PROFILE, INST_SINGLE,
                                 profile_info->profile_data);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_COMPENSATION_PROFILE, INST_SINGLE, 0);

    /** Add the value of in_pp_port  and header_delta to the packet size. */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_COMPENSATION_IRPP_HDR_ENABLE, INST_SINGLE,
                                 TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_COMPENSATION_IN_PORT_ENABLE, INST_SINGLE,
                                 TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_COMPENSATION_TRUNCATE_ENABLE, INST_SINGLE,
                                 profile_info->is_pkt_truncate ? TRUE : FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUCKET_LEVEL_RESOLUTION, INST_SINGLE,
                                 profile_info->is_large_bucket_mode);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_global_profile_get(
    int unit,
    int core_id,
    int glbl_idx,
    dnx_meter_profile_info_t * profile_info)
{
    uint32 entry_handle_id;
    uint32 reverse_exponent;
    uint32 rate_mantissa;
    uint32 rate_exp;
    uint32 burst_mantissa;
    uint32 burst_exp;
    uint32 max_rate_mantissa;
    uint32 max_rate_exp;
    uint32 core_clock_freq = dnx_data_device.general.core_clock_khz_get(unit) * 1000;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_PROFILE_CONFIG, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_INDEX, glbl_idx);
    /** get the table values */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COLOR_AWARE, INST_SINGLE, &profile_info->color_mode);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PACKET_MODE, INST_SINGLE,
                               &profile_info->is_packet_mode);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUPLING_FLAG, INST_SINGLE,
                               &profile_info->is_coupling_enabled);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SHARING_FLAG, INST_SINGLE,
                               &profile_info->is_sharing_enabled);
    /** for global, no way of disable cir, because the bucket will be always empty */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CIR_RESET, INST_SINGLE, &profile_info->disable_cir);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CBS_MANT, INST_SINGLE, &burst_mantissa);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CBS_EXP, INST_SINGLE, &burst_exp);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CIR_MANT, INST_SINGLE, &rate_mantissa);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CIR_MANY_EXP, INST_SINGLE, &rate_exp);
    /** althought max rate not relevant if sharing is disable, we get it as is */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_CIR_MANT, INST_SINGLE, &max_rate_mantissa);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_CIR_MANT_EXP, INST_SINGLE, &max_rate_exp);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CIR_REV_EXP, INST_SINGLE, &reverse_exponent);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COLOR_RSLV_PROFILE, INST_SINGLE,
                               &profile_info->profile_data);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HEADER_COMPENSATION_TRUNCATE_ENABLE, INST_SINGLE,
                               &profile_info->is_pkt_truncate);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BUCKET_LEVEL_RESOLUTION, INST_SINGLE,
                               &profile_info->is_large_bucket_mode);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** calc burst size */
    /** for large bucket mode the burst size is multiple in 64 */
    burst_exp =
        profile_info->is_large_bucket_mode ? (burst_exp +
                                              dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit)) : (burst_exp);
    SHR_IF_ERR_EXIT(utilex_compute_complex_to_mnt_exp
                    (burst_mantissa, burst_exp, DNX_METER_UTILS_BURST_MULTI_CONST_VAL,
                     DNX_METER_UTILS_BURST_MANT_CONST_VAL, &profile_info->cbs));

    /** calc cir */
    SHR_IF_ERR_EXIT(dnx_meter_utils_compute_complex_from_mnt_exp_reverse_exp(unit, rate_mantissa,
                                                                             rate_exp,
                                                                             reverse_exponent,
                                                                             core_clock_freq,
                                                                             DNX_METER_UTILS_RATE_MANT_CONST_VAL,
                                                                             TRUE, TRUE, &profile_info->cir));
    /** calc max_cir. max_cir should be the same as cir, but we get it from HW as is */
    SHR_IF_ERR_EXIT(dnx_meter_utils_compute_complex_from_mnt_exp_reverse_exp(unit, max_rate_mantissa,
                                                                             max_rate_exp,
                                                                             reverse_exponent,
                                                                             core_clock_freq,
                                                                             DNX_METER_UTILS_RATE_MANT_CONST_VAL,
                                                                             TRUE, TRUE, &profile_info->max_cir));

    if (profile_info->is_large_bucket_mode == TRUE)
    {
        profile_info->cir = profile_info->cir << dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit);
        profile_info->max_cir = profile_info->max_cir << dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   mapping table configuration for the fwd-type that is used for the ingress metering. It goes over statistic_object_10. 
* \param [in] unit - unit id
* \return
*   shr_error_e 
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_meter_global_fwd_type_mapping(
    int unit)
{
    uint32 entry_handle_id, entry_handle_id1;
    int is_unknown, is_mc, is_bc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_STATISTIC_10_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    for (is_unknown = 0; is_unknown <= 1; is_unknown++)
    {
        for (is_mc = 0; is_mc <= 1; is_mc++)
        {
            for (is_bc = 0; is_bc <= 1; is_bc++)
            {
                /** Setting key fields */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_UNKNOWN, is_unknown);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_BC, is_bc);

                /** Setting vlaue field */
                if (is_bc)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_FWD_TYPE, INST_SINGLE,
                                                 bcmPolicerFwdTypeBc);
                }
                else if (is_mc)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_FWD_TYPE, INST_SINGLE,
                                                 (is_unknown ? bcmPolicerFwdTypeUnkownMc : bcmPolicerFwdTypeMc));
                }
                else
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_FWD_TYPE, INST_SINGLE,
                                                 (is_unknown ? bcmPolicerFwdTypeUnkownUc : bcmPolicerFwdTypeUc));
                }

                /** Preforming the action */
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

    /** set the eth protocol type */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_PROTOCOL_TYPE_IS_ETH, &entry_handle_id1));
    dbal_entry_key_field32_set(unit, entry_handle_id1, DBAL_FIELD_PROT_TYPE, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET);
    dbal_entry_value_field32_set(unit, entry_handle_id1, DBAL_FIELD_IS_ETH, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id1, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_global_init(
    int unit,
    dnx_meter_profile_info_t * profile_info)
{
    int global_meter_idx;
    uint32 entry_handle_id;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_GENERAL_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_INIT, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, FALSE);
    /** filter all Fwd-Type*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_TYPE_FILTER_BITMAP, INST_SINGLE, 0xFF);
    /** don't filter DP-CMD */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_CMD_FILTER_BITMAP, INST_SINGLE, 0);
    /** disable random mode */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RANDOM_MODE_ENABLE, INST_SINGLE, FALSE);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** configure statistic object 10, which holds the forwarding-type*/
    SHR_IF_ERR_EXIT(dnx_meter_global_fwd_type_mapping(unit));

    /** set all global meters to the max rate */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
    {
        for (global_meter_idx = bcmPolicerFwdTypeUc; global_meter_idx < bcmPolicerFwdTypeMax; global_meter_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_meter_global_profile_set(unit, core_idx, global_meter_idx, profile_info));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_global_ctr_mode_set(
    int unit,
    int policer_database_handle,
    uint32 is_bytes)
{
    int core_id;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_ING_GENERAL_SETTINGS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_CTR_MODE, INST_SINGLE, is_bytes);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_global_ctr_mode_get(
    int unit,
    int policer_database_handle,
    uint32 *is_bytes)
{
    int core_id;
    uint32 entry_handle_id, fld_value;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_ING_GENERAL_SETTINGS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GLOBAL_CTR_MODE, INST_SINGLE, &fld_value);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *is_bytes = fld_value;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_global_stat_get(
    int unit,
    bcm_policer_t policer_id,
    bcm_policer_stat_t stat,
    uint64 *value)
{
    uint32 entry_handle_id;
    uint64 fld_value;
    int meter_idx, database_handle, core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    database_handle = _SHR_POLICER_ID_DATABASE_HANDLE_GET(policer_id);
    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(database_handle);
    meter_idx = _SHR_POLICER_ID_METER_INDEX_GET(policer_id);

     /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_COUNTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_ID, meter_idx);

    switch (stat)
    {
        case bcmPolicerStatGlobalInRedColorPackets:
        case bcmPolicerStatGlobalInRedColorBytes:
            dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_RED_COLOR_COUNTER, INST_SINGLE, &fld_value);
            break;
        case bcmPolicerStatGlobalInBlackColorPackets:
        case bcmPolicerStatGlobalInBlackColorBytes:
            dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_BLACK_COLOR_COUNTER, INST_SINGLE, &fld_value);
            break;
        case bcmPolicerStatGlobalRejectPackets:
        case bcmPolicerStatGlobalRejectBytes:
            dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_REJECT_COUNTER, INST_SINGLE, &fld_value);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "stat=%d, incorrect statistic \n", stat);
    }
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *value = fld_value;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_meter_global_fairness_set(
    int unit,
    bcm_core_t core_id,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_GENERAL_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RANDOM_MODE_ENABLE, INST_SINGLE, enable);
    /** set the range of random values add/substract to the bucket level as the highest posssible range (-8K..8K) */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CBL_RANDOM_RANGE, INST_SINGLE, 0);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_global_fairness_get(
    int unit,
    bcm_core_t core_id,
    uint32 *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_GENERAL_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RANDOM_MODE_ENABLE, INST_SINGLE, (uint32 *) enable);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

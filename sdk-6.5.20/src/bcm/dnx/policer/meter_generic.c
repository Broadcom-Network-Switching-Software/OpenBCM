/** \file meter_generic.c
 * 
 *  policer module gather all functionality that related to the policer/meter.
 *  meter_generic.c holds all the generic meter (ingress+egress) implemantation for DNX
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>
#include "meter_utils.h"
#include "meter_generic.h"
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/policer/policer_mgmt.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/meter_access.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>

/*
 * }
 */

/*
 * MACROs/Defines
 * {
 */

/*
 * }
 */

/** Brief: HW meter profile structure */
typedef struct
{
    uint32 color_aware;
    uint32 coupling_flag;
    uint32 sharing_flag;
    uint32 cir_mantissa;
    uint32 cir_mantissa_exponent;
    uint32 reset_cir;
    uint32 cbs_mantissa;
    uint32 cbs_exponent;
    uint32 eir_mantissa;
    uint32 eir_mantissa_exponent;
    uint32 max_cir_mantissa;
    uint32 max_cir_mantissa_exponent;
    uint32 max_eir_mantissa;
    uint32 max_eir_mantissa_exponent;
    uint32 reset_eir;
    uint32 ebs_mantissa;
    uint32 cir_reverse_exponent;
    uint32 eir_reverse_exponent;
    uint32 is_large_bucket_mode;
    uint32 ebs_exponent;
    uint32 packet_mode;
    uint32 color_resolved_profile_data;
    uint32 is_pkt_truncate;
} dnx_meter_generic_hw_profile_data_t;

/**
* \brief
*      first initialization of the generic ingress meter database
* \param [in] unit -unit id
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_meter_generic_hw_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * ingress config 
     */

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_ING_GENERAL_SETTINGS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHICAL_MODE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEF_10_DOT_3_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_DELTA_NEGATE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRUNCATE_SIZE_NEGATE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESOLVE_TABLE_MODE_SELECT_PROFILE, INST_SINGLE,
                                 TRUE);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * egress config 
     */

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGR_GENERAL_SETTINGS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHICAL_MODE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEF_10_DOT_3_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESOLVE_TABLE_MODE_SELECT_PROFILE, INST_SINGLE,
                                 TRUE);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_ingress_db_hw_init(
    int unit,
    int database_id,
    int init_stage)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_INIT, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_TYPE_FILTER_BITMAP, INST_SINGLE, 0x0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_CMD_FILTER_BITMAP, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SINGLE_RATE_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXPANSION_SIZE, INST_SINGLE, 0);
    /** enable random/fairness mode. It is made just in the first initialization and not every database destroy. */
    /** The API which control the fairness is bcm_switch_control_set, which change the mode for all  */
    /** databases and global meter. The API is like higher level from the databases APIs level. */
    /** So, even if database destroyed, the fairness mode remian the same. This way all databases has the same value */
    if (init_stage)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CBL_RANDOM_MODE_ENABLE, INST_SINGLE, FALSE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EBL_RANDOM_MODE_ENABLE, INST_SINGLE, FALSE);
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_egress_db_hw_init(
    int unit,
    int database_id,
    int init_stage)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGRESS_DB_GENERAL_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_INIT, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_CMD_FILTER_BITMAP, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SINGLE_RATE_ENABLE, INST_SINGLE, 0);
    /** enable random/fairness mode. It is made just in the first initialization and not every database destroy. */
    /** The API which control the fairness is bcm_switch_control_set, which change the mode for all  */
    /** databases and global meter. The API is like higher level from the databases APIs level. */
    /** So, even if database destroyed, the fairness mode remian the same. This way all databases has the same value */
    if (init_stage)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CBL_RANDOM_MODE_ENABLE, INST_SINGLE, FALSE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EBL_RANDOM_MODE_ENABLE, INST_SINGLE, FALSE);
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*      hw configuration of the meter profile
* \param [in] unit -unit id
* \param [in] core_id -core id
* \param [in] is_ingress -indicates ingress or egress
* \param [in] database_id -database id
* \param [in] profile_idx -profile  index
* \param [in] hw_data -structure of the hw profile configuration
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_meter_generic_profile_hw_set(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int profile_idx,
    dnx_meter_generic_hw_profile_data_t * hw_data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      (is_ingress ==
                                       TRUE ? DBAL_TABLE_METER_ING_PROFILE_CONFIG :
                                       DBAL_TABLE_METER_EGR_PROFILE_CONFIG), &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_INDEX, profile_idx);
    /** set the table values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COLOR_AWARE, INST_SINGLE, hw_data->color_aware);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_MODE, INST_SINGLE, hw_data->packet_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUPLING_FLAG, INST_SINGLE, hw_data->coupling_flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHARING_FLAG, INST_SINGLE, hw_data->sharing_flag);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CIR_RESET, INST_SINGLE, hw_data->reset_cir);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EIR_RESET, INST_SINGLE, hw_data->reset_eir);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CBS_MANT, INST_SINGLE, hw_data->cbs_mantissa);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CBS_EXP, INST_SINGLE, hw_data->cbs_exponent);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EBS_MANT, INST_SINGLE, hw_data->ebs_mantissa);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EBS_EXP, INST_SINGLE, hw_data->ebs_exponent);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CIR_MANT, INST_SINGLE, hw_data->cir_mantissa);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EIR_MANT, INST_SINGLE, hw_data->eir_mantissa);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CIR_MANT_EXP, INST_SINGLE,
                                 hw_data->cir_mantissa_exponent);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EIR_MANT_EXP, INST_SINGLE,
                                 hw_data->eir_mantissa_exponent);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_CIR_MANT, INST_SINGLE,
                                 hw_data->max_cir_mantissa);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_CIR_MANT_EXP, INST_SINGLE,
                                 hw_data->max_cir_mantissa_exponent);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_EIR_MANT, INST_SINGLE,
                                 hw_data->max_eir_mantissa);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_EIR_MANT_EXP, INST_SINGLE,
                                 hw_data->max_eir_mantissa_exponent);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CIR_REV_EXP, INST_SINGLE,
                                 hw_data->cir_reverse_exponent);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EIR_REV_EXP, INST_SINGLE,
                                 hw_data->eir_reverse_exponent);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COLOR_RSLV_PROFILE, INST_SINGLE,
                                 hw_data->color_resolved_profile_data);

    /** compensation parameters relevant only for ingres meter */
    if (is_ingress == TRUE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_COMPENSATION_PROFILE, INST_SINGLE, 0);

        /** Add the value of in_pp_port  and header_delta to the packet size. */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_COMPENSATION_IRPP_HDR_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_COMPENSATION_IN_PORT_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_COMPENSATION_TRUNCATE_ENABLE, INST_SINGLE,
                                     hw_data->is_pkt_truncate ? TRUE : FALSE);

    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUCKET_LEVEL_RESOLUTION, INST_SINGLE,
                                 hw_data->is_large_bucket_mode);

 /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*      calculate the burst parameters (mant, exp, large_bucket mode) for the given configuration
* \param [in] unit -unit id
* \param [in] profile_info -profile information (tm structure)
* \param [in] cir_valid_rate -is cir is valid
* \param [in] max_cir_valid_rate -is max cir is valid
* \param [in] eir_valid_rate -is eir is valid
* \param [in] max_eir_valid_rate -is max eir is valid
* \param [out] hw_data -profile information as the hw see it. update the burst mant/exp params based on other rate params
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
meter_generic_burst_mant_exp_calc(
    int unit,
    dnx_meter_profile_info_t * profile_info,
    uint32 cir_valid_rate,
    uint32 max_cir_valid_rate,
    uint32 eir_valid_rate,
    uint32 max_eir_valid_rate,
    dnx_meter_generic_hw_profile_data_t * hw_data)
{

    SHR_FUNC_INIT_VARS(unit);

    /** calculate cbs and ebs. there is one configuration for large_mode, therefore, 
         if one of them is in large mode also the second calculation should be in large bucket mode */

    /** calc the green bucket size */
    SHR_IF_ERR_EXIT(dnx_meter_utils_profile_burst_to_exp_mnt(unit,
                                                             profile_info->cbs,
                                                             profile_info->is_large_bucket_mode,
                                                             &hw_data->cbs_mantissa, &hw_data->cbs_exponent));
    /** adjust the green bucket size, extend the bucket size if one rate chunk is bigger than the burst size */
    SHR_IF_ERR_EXIT(dnx_meter_utils_bucket_size_adjust(unit,
                                                       hw_data->cir_mantissa * cir_valid_rate,
                                                       hw_data->cir_mantissa_exponent * cir_valid_rate,
                                                       hw_data->max_cir_mantissa * max_cir_valid_rate,
                                                       hw_data->max_cir_mantissa_exponent * max_cir_valid_rate,
                                                       &hw_data->cbs_mantissa, &hw_data->cbs_exponent));

    /** calc the yellow bucket size */
    SHR_IF_ERR_EXIT(dnx_meter_utils_profile_burst_to_exp_mnt(unit,
                                                             profile_info->ebs,
                                                             profile_info->is_large_bucket_mode,
                                                             &hw_data->ebs_mantissa, &hw_data->ebs_exponent));

    /** adjust the yellow bucket size, extend the bucket size if one rate chunk is bigger than the burst size */
    SHR_IF_ERR_EXIT(dnx_meter_utils_bucket_size_adjust(unit,
                                                       hw_data->eir_mantissa * eir_valid_rate,
                                                       hw_data->eir_mantissa_exponent * eir_valid_rate,
                                                       hw_data->max_eir_mantissa * max_eir_valid_rate,
                                                       hw_data->max_eir_mantissa_exponent * max_eir_valid_rate,
                                                       &hw_data->ebs_mantissa, &hw_data->ebs_exponent));

    hw_data->is_large_bucket_mode = profile_info->is_large_bucket_mode;
exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_meter_generic_profile_set(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int profile_idx,
    dnx_meter_profile_info_t * profile_info)
{
    dnx_meter_generic_hw_profile_data_t hw_profile_data;
    uint32 core_clock_freq = dnx_data_device.general.core_clock_khz_get(unit) * 1000;
    uint32 max_ir_mant, max_ir_exp;
    uint32 max_eir_for_calc;
    uint32 eir_valid_rate = 0, max_eir_valid_rate = 0, cir_valid_rate = 0, max_cir_valid_rate = 0;
    uint32 rates[DNX_METER_UTILS_PROFILE_MAX_RATES_FOR_CALC];
    uint32 nof_rates = -1;
    uint32 min_rate;
    int is_large_bucket_mode = profile_info->is_large_bucket_mode;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&hw_profile_data, 0x0, sizeof(dnx_meter_generic_hw_profile_data_t));

    /** get sw state parameters */
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_ir_mant.get(unit, &max_ir_mant));
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.max_ir_exp.get(unit, &max_ir_exp));
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.min_rate.get(unit, &min_rate));

    if (is_large_bucket_mode)
    {
        profile_info->cir = UTILEX_DIV_ROUND_UP
            (profile_info->cir, utilex_power_of_2(dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit)));
        profile_info->eir = UTILEX_DIV_ROUND_UP
            (profile_info->eir, utilex_power_of_2(dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit)));
    }

    /** max eir unlimited */
    if (profile_info->max_eir == DNX_METER_UTILS_PROFILE_IR_MAX_UNLIMITED)
    {
        /** max_eir is not used in calculations */
        max_eir_for_calc = profile_info->eir;

        /** -1 is written to HW */
        hw_profile_data.max_eir_mantissa = max_ir_mant;
        hw_profile_data.max_eir_mantissa_exponent = max_ir_exp;
    }
    else
    {

        max_eir_for_calc = (is_large_bucket_mode) ?
            UTILEX_DIV_ROUND_UP
            (profile_info->max_eir, utilex_power_of_2(dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit))) :
            profile_info->max_eir;
        max_eir_valid_rate = 1;
    }

    /** transfer user values (cir, eir, cbs, ebs) to mant and exp */
    if (profile_info->is_sharing_enabled)
    {
        if (profile_info->max_cir == DNX_METER_UTILS_PROFILE_IR_MAX_UNLIMITED)
        {
             /** -1 is written to HW */
            hw_profile_data.max_cir_mantissa = max_ir_mant;
            hw_profile_data.max_cir_mantissa_exponent = max_ir_exp;
        }
        else
        {
            max_cir_valid_rate = 1;
        }

         /** for sharing enabled mode (cascade), the rev_exp should have been calc before */
        if (profile_info->ir_rev_exp == DNX_METER_UTILS_INVALID_IR_REV_EXP)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid ir reverse exponent\n");
        }
        else
        {
            hw_profile_data.cir_reverse_exponent = profile_info->ir_rev_exp;
            hw_profile_data.eir_reverse_exponent = profile_info->ir_rev_exp;
        }

         /** find min_val and max_val of: CIR, EIR, max CIR and max EIR (only ones actualy used) */
        if (profile_info->disable_cir && profile_info->disable_eir &&
            (0 == profile_info->max_cir) && (0 == profile_info->max_eir))
        {
            hw_profile_data.max_cir_mantissa = 0;
            hw_profile_data.max_cir_mantissa_exponent = 0;
            hw_profile_data.max_eir_mantissa = 0;
            hw_profile_data.max_eir_mantissa_exponent = 0;
            hw_profile_data.cir_mantissa = 0;
            hw_profile_data.cir_mantissa_exponent = 0;
            hw_profile_data.eir_mantissa = 0;
            hw_profile_data.eir_mantissa_exponent = 0;
        }
        else
        {
            /** not max cir unlimited calc the mant and exp */
            if (profile_info->max_cir != DNX_METER_UTILS_MAX_RATE_UNLIMITED)
            {
                if (is_large_bucket_mode)
                {
                    /** if large_bucket_mode_exp_add_get>32,  */
                    /** utilex_power_of_2 will return zero and will cause zero division, but this is not the case */
                     /* coverity[Division or modulo by zero :FALSE]  */
                    profile_info->max_cir = UTILEX_DIV_ROUND_UP
                        (profile_info->max_cir,
                         utilex_power_of_2(dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit)));
                }
                SHR_IF_ERR_EXIT(dnx_meter_utils_break_complex_to_mnt_exp(unit, profile_info->max_cir,
                                                                         max_ir_mant,
                                                                         DNX_METER_UTILS_RATE_MANT_MIN_VAL,
                                                                         max_ir_exp,
                                                                         core_clock_freq,
                                                                         hw_profile_data.cir_reverse_exponent,
                                                                         DNX_METER_UTILS_RATE_MANT_CONST_VAL,
                                                                         &hw_profile_data.max_cir_mantissa,
                                                                         &hw_profile_data.max_cir_mantissa_exponent));
            }
        }
    }
    /** sharing=FALSE*/
    else
    {
        max_cir_valid_rate = 0;

        /*
         * if coupling is set: eir_rev_exp must be equal to cir_rev_exp because Committed bucket share its credits with 
         * the Excess bucket otherwise: cir-rev-exp and eir-rev-exp are calculated seperatly 
         */
        if (profile_info->is_coupling_enabled)
        {

            /** find min_val and max_val of: CIR, EIR and max EIR (no max CIR) */
            if (profile_info->disable_cir && profile_info->disable_eir)
            {
                /** both are disabled, no need to calculate anything */
                hw_profile_data.cir_reverse_exponent = dnx_data_meter.profile.rev_exp_min_get(unit);
                hw_profile_data.eir_reverse_exponent = dnx_data_meter.profile.rev_exp_min_get(unit);;
                max_eir_valid_rate = 0;
            }
            else
            {
                if (profile_info->disable_cir)
                {
                    /** cir not used in calculations */
                    rates[0] = profile_info->eir;
                    rates[1] = max_eir_for_calc;
                    nof_rates = 2;
                    eir_valid_rate = 1;
                    max_eir_valid_rate = 0;
                }
                else if (profile_info->disable_eir)
                {
                    /** eir not used in calculations */
                    rates[0] = profile_info->cir;
                    nof_rates = 1;
                    if (max_eir_for_calc >= min_rate)
                    {
                        rates[1] = max_eir_for_calc;
                        nof_rates = 2;
                    }
                    cir_valid_rate = 1;
                }
                else
                {
                    /** use all */
                    rates[0] = profile_info->eir;
                    rates[1] = max_eir_for_calc;
                    rates[2] = profile_info->cir;
                    nof_rates = 3;
                    cir_valid_rate = 1;
                    eir_valid_rate = 1;
                }

                /** Calculate the best rev_exp (with respect to both rate accuracy and bucket update) for: CIR, EIR and max EIR */
                SHR_IF_ERR_EXIT(dnx_meter_utils_rev_exp_optimized_for_bucket_rate(unit,
                                                                                  rates,
                                                                                  nof_rates,
                                                                                  &hw_profile_data.eir_reverse_exponent));

                hw_profile_data.cir_reverse_exponent = hw_profile_data.eir_reverse_exponent;
            }
        }
        else
        { /** no coupling */
            max_eir_valid_rate = 0;
            eir_valid_rate = 1;
            cir_valid_rate = 1;
            rates[0] = profile_info->cir;
            nof_rates = 1;

            /*
             * Calculate the best rev_exp (with respect to both rate accuracy and bucket update) for: * 1) CIR * 2)
             * EIR and max EIR 
             */
            if (profile_info->disable_cir == FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_meter_utils_rev_exp_optimized_for_bucket_rate(unit, rates, nof_rates,
                                                                                  &hw_profile_data.cir_reverse_exponent));
            }

            if (profile_info->disable_eir == FALSE)
            {
                rates[0] = profile_info->eir;
                rates[1] = max_eir_for_calc;
                nof_rates = 2;
                SHR_IF_ERR_EXIT(dnx_meter_utils_rev_exp_optimized_for_bucket_rate(unit, rates, nof_rates,
                                                                                  &hw_profile_data.eir_reverse_exponent));
            }

        }
    }

       /** calculate  mnt and exp for: CIR, EIR and max EIR, based on their rev_exps */
    SHR_IF_ERR_EXIT(dnx_meter_utils_break_complex_to_mnt_exp(unit, profile_info->cir,
                                                             max_ir_mant,
                                                             DNX_METER_UTILS_RATE_MANT_MIN_VAL,
                                                             max_ir_exp,
                                                             core_clock_freq,
                                                             hw_profile_data.cir_reverse_exponent,
                                                             DNX_METER_UTILS_RATE_MANT_CONST_VAL,
                                                             &hw_profile_data.cir_mantissa,
                                                             &hw_profile_data.cir_mantissa_exponent));

    SHR_IF_ERR_EXIT(dnx_meter_utils_break_complex_to_mnt_exp(unit, profile_info->eir,
                                                             max_ir_mant,
                                                             DNX_METER_UTILS_RATE_MANT_MIN_VAL,
                                                             max_ir_exp,
                                                             core_clock_freq,
                                                             hw_profile_data.eir_reverse_exponent,
                                                             DNX_METER_UTILS_RATE_MANT_CONST_VAL,
                                                             &hw_profile_data.eir_mantissa,
                                                             &hw_profile_data.eir_mantissa_exponent));

    if (profile_info->max_eir != DNX_METER_UTILS_MAX_RATE_UNLIMITED)
    {         /** not max eir unlimited */
        SHR_IF_ERR_EXIT(dnx_meter_utils_break_complex_to_mnt_exp(unit, max_eir_for_calc,
                                                                 max_ir_mant,
                                                                 DNX_METER_UTILS_RATE_MANT_MIN_VAL,
                                                                 max_ir_exp,
                                                                 core_clock_freq,
                                                                 hw_profile_data.eir_reverse_exponent,
                                                                 DNX_METER_UTILS_RATE_MANT_CONST_VAL,
                                                                 &hw_profile_data.max_eir_mantissa,
                                                                 &hw_profile_data.max_eir_mantissa_exponent));
    }

    /** calculate cbs and ebs. there is one configuration for large_mode, therefore, 
         if one of them is in large mode also the second calculation should be in large bucket mode */
    SHR_IF_ERR_EXIT(meter_generic_burst_mant_exp_calc(unit, profile_info, cir_valid_rate, max_cir_valid_rate,
                                                      eir_valid_rate, max_eir_valid_rate, &hw_profile_data));

    hw_profile_data.color_resolved_profile_data = profile_info->profile_data;
    hw_profile_data.color_aware = profile_info->color_mode;
    hw_profile_data.coupling_flag = profile_info->is_coupling_enabled;
    hw_profile_data.sharing_flag = profile_info->is_sharing_enabled;
    hw_profile_data.reset_cir = profile_info->disable_cir;
    hw_profile_data.reset_eir = profile_info->disable_eir;
    hw_profile_data.packet_mode = profile_info->is_packet_mode;
    hw_profile_data.is_pkt_truncate = profile_info->is_pkt_truncate;
    SHR_IF_ERR_EXIT(dnx_meter_generic_profile_hw_set(unit, core_id, is_ingress, database_id, profile_idx,
                                                     &hw_profile_data));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*      get hw configuration of the meter profile
* \param [in] unit -unit id
* \param [in] core_id -core id
* \param [in] is_ingress -indicates ingress or egress
* \param [in] database_id -database id
* \param [in] profile_idx -profile  index
* \param [out] hw_data -structure of the hw profile configuration
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_meter_generic_profile_hw_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int profile_idx,
    dnx_meter_generic_hw_profile_data_t * hw_data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      (is_ingress ==
                                       TRUE ? DBAL_TABLE_METER_ING_PROFILE_CONFIG :
                                       DBAL_TABLE_METER_EGR_PROFILE_CONFIG), &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_INDEX, profile_idx);
    /** set the table values */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COLOR_AWARE, INST_SINGLE, &hw_data->color_aware);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PACKET_MODE, INST_SINGLE, &hw_data->packet_mode);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUPLING_FLAG, INST_SINGLE, &hw_data->coupling_flag);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SHARING_FLAG, INST_SINGLE, &hw_data->sharing_flag);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CIR_RESET, INST_SINGLE, &hw_data->reset_cir);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EIR_RESET, INST_SINGLE, &hw_data->reset_eir);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CBS_MANT, INST_SINGLE, &hw_data->cbs_mantissa);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CBS_EXP, INST_SINGLE, &hw_data->cbs_exponent);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EBS_MANT, INST_SINGLE, &hw_data->ebs_mantissa);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EBS_EXP, INST_SINGLE, &hw_data->ebs_exponent);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CIR_MANT, INST_SINGLE, &hw_data->cir_mantissa);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EIR_MANT, INST_SINGLE, &hw_data->eir_mantissa);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CIR_MANT_EXP, INST_SINGLE,
                               &hw_data->cir_mantissa_exponent);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EIR_MANT_EXP, INST_SINGLE,
                               &hw_data->eir_mantissa_exponent);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_CIR_MANT, INST_SINGLE, &hw_data->max_cir_mantissa);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_CIR_MANT_EXP, INST_SINGLE,
                               &hw_data->max_cir_mantissa_exponent);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_EIR_MANT, INST_SINGLE, &hw_data->max_eir_mantissa);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_EIR_MANT_EXP, INST_SINGLE,
                               &hw_data->max_eir_mantissa_exponent);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CIR_REV_EXP, INST_SINGLE,
                               &hw_data->cir_reverse_exponent);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EIR_REV_EXP, INST_SINGLE,
                               &hw_data->eir_reverse_exponent);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COLOR_RSLV_PROFILE, INST_SINGLE,
                               &hw_data->color_resolved_profile_data);

    /** compensation parameters relevant only for ingres meter */
    if (is_ingress == TRUE)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HEADER_COMPENSATION_TRUNCATE_ENABLE, INST_SINGLE,
                                   &hw_data->is_pkt_truncate);
    }
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BUCKET_LEVEL_RESOLUTION, INST_SINGLE,
                               &hw_data->is_large_bucket_mode);

   /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_profile_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int profile_idx,
    dnx_meter_profile_info_t * profile_info)
{
    dnx_meter_generic_hw_profile_data_t hw_profile_data;
    dbal_table_field_info_t field_info;
    uint32 max_mant, max_exp;
    uint32 burst_exp;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&hw_profile_data, 0x0, sizeof(hw_profile_data));
    SHR_IF_ERR_EXIT(dnx_meter_generic_profile_hw_get(unit, core_id, is_ingress, database_id, profile_idx,
                                                     &hw_profile_data));

    profile_info->profile_data = hw_profile_data.color_resolved_profile_data;
    profile_info->color_mode = hw_profile_data.color_aware;
    profile_info->is_coupling_enabled = hw_profile_data.coupling_flag;
    profile_info->is_sharing_enabled = hw_profile_data.sharing_flag;
    profile_info->disable_cir = hw_profile_data.reset_cir;
    profile_info->disable_eir = hw_profile_data.reset_eir;
    profile_info->is_packet_mode = hw_profile_data.packet_mode;
    profile_info->is_pkt_truncate = hw_profile_data.is_pkt_truncate;

    /** Calculate the configured rates. */
    SHR_IF_ERR_EXIT(dnx_meter_utils_rate_from_mnt_exp_rev_exp_get(unit, hw_profile_data.cir_mantissa,
                                                                  hw_profile_data.cir_mantissa_exponent,
                                                                  hw_profile_data.cir_reverse_exponent,
                                                                  &profile_info->cir));

    profile_info->cir = (hw_profile_data.is_large_bucket_mode) ?
        (profile_info->cir << dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit)) : profile_info->cir;

    SHR_IF_ERR_EXIT(dnx_meter_utils_rate_from_mnt_exp_rev_exp_get(unit, hw_profile_data.eir_mantissa,
                                                                  hw_profile_data.eir_mantissa_exponent,
                                                                  hw_profile_data.eir_reverse_exponent,
                                                                  &profile_info->eir));
    profile_info->eir = (hw_profile_data.is_large_bucket_mode) ?
        (profile_info->eir << dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit)) : profile_info->eir;

    /** get from dbal the max_eir/cir mantissa and exponent values */
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_ING_PROFILE_CONFIG, DBAL_FIELD_MAX_EIR_MANT,
                                               FALSE, 0, 0, &field_info));
    max_mant = field_info.max_value;
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_ING_PROFILE_CONFIG, DBAL_FIELD_MAX_EIR_MANT_EXP,
                                               FALSE, 0, 0, &field_info));
    max_exp = field_info.max_value;

    if ((hw_profile_data.max_eir_mantissa_exponent == max_exp) && (hw_profile_data.max_eir_mantissa == max_mant))
    {
        /** unlimited max rate */
        profile_info->max_eir = DNX_METER_UTILS_MAX_RATE_UNLIMITED;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_meter_utils_rate_from_mnt_exp_rev_exp_get(unit, hw_profile_data.max_eir_mantissa,
                                                                      hw_profile_data.max_eir_mantissa_exponent,
                                                                      hw_profile_data.eir_reverse_exponent,
                                                                      &profile_info->max_eir));
        profile_info->max_eir = (hw_profile_data.is_large_bucket_mode) ?
            (profile_info->max_eir << dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit)) :
            profile_info->max_eir;
    }

    if (profile_info->is_sharing_enabled)
    {
        if ((hw_profile_data.max_cir_mantissa_exponent == max_exp) && (hw_profile_data.max_cir_mantissa == max_mant))
        {
            /** unlimited max rate */
            profile_info->max_cir = DNX_METER_UTILS_MAX_RATE_UNLIMITED;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_meter_utils_rate_from_mnt_exp_rev_exp_get(unit, hw_profile_data.max_cir_mantissa,
                                                                          hw_profile_data.max_cir_mantissa_exponent,
                                                                          hw_profile_data.cir_reverse_exponent,
                                                                          &profile_info->max_cir));
            profile_info->max_cir = (hw_profile_data.is_large_bucket_mode) ?
                (profile_info->max_cir << dnx_data_meter.profile.
                 large_bucket_mode_exp_add_get(unit)) : profile_info->max_cir;
        }
    }

    /** Transfer mnt and exp to burst values */
    /** for large bucket mode the burst size is multiple in 64 */
    burst_exp = hw_profile_data.is_large_bucket_mode ?
        (hw_profile_data.cbs_exponent + dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit)) :
        (hw_profile_data.cbs_exponent);
    SHR_IF_ERR_EXIT(utilex_compute_complex_to_mnt_exp(hw_profile_data.cbs_mantissa,
                                                      burst_exp,
                                                      DNX_METER_UTILS_BURST_MULTI_CONST_VAL,
                                                      DNX_METER_UTILS_BURST_MANT_CONST_VAL, &profile_info->cbs));

    burst_exp = hw_profile_data.is_large_bucket_mode ?
        (hw_profile_data.ebs_exponent + dnx_data_meter.profile.large_bucket_mode_exp_add_get(unit)) :
        (hw_profile_data.ebs_exponent);
    SHR_IF_ERR_EXIT(utilex_compute_complex_to_mnt_exp(hw_profile_data.ebs_mantissa,
                                                      burst_exp,
                                                      DNX_METER_UTILS_BURST_MULTI_CONST_VAL,
                                                      DNX_METER_UTILS_BURST_MANT_CONST_VAL, &profile_info->ebs));

    profile_info->is_large_bucket_mode = hw_profile_data.is_large_bucket_mode;
exit:
    SHR_FUNC_EXIT;
}

 /** see .h file */
shr_error_e
dnx_meter_generic_init(
    int unit)
{
    int database_idx;
    dbal_table_field_info_t field_info;
    uint32 nof_ingress_db, nof_egress_db;

    SHR_FUNC_INIT_VARS(unit);

    /** general setting configuration for ingress and egress meter */
    SHR_IF_ERR_EXIT(dnx_meter_generic_hw_init(unit));

    /** get from dbal nuber of databases for ingress and egress*/
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_ING_PROFILE_CONFIG, DBAL_FIELD_DATABASE_ID,
                                               TRUE, 0, 0, &field_info));
    nof_ingress_db = field_info.max_value + 1;
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_EGR_PROFILE_CONFIG, DBAL_FIELD_DATABASE_ID,
                                               TRUE, 0, 0, &field_info));
    nof_egress_db = field_info.max_value + 1;

    /** init ingress database settings */
    /** set default meter profile for each database */
    for (database_idx = 0; database_idx < nof_ingress_db; database_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_meter_generic_ingress_db_hw_init(unit, database_idx, TRUE));
        /** set the ptr map table to invalid banks */
        SHR_IF_ERR_EXIT(dnx_meter_generic_ptr_map_hw_set
                        (unit, BCM_CORE_ALL, TRUE, database_idx, DNX_METER_PTR_MAP_INDEX_ALL,
                         dnx_data_meter.mem_mgmt.invalid_bank_id_get(unit)));
    }
    /** init egress database settings */
    /** set default meter profile for each database */
    for (database_idx = 0; database_idx < nof_egress_db; database_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_meter_generic_egress_db_hw_init(unit, database_idx, TRUE));
        /** set the ptr map table to invalid banks */
        SHR_IF_ERR_EXIT(dnx_meter_generic_ptr_map_hw_set
                        (unit, BCM_CORE_ALL, FALSE, database_idx, DNX_METER_PTR_MAP_INDEX_ALL,
                         dnx_data_meter.mem_mgmt.invalid_bank_id_get(unit)));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*      enable/disable the refresh mechanism of the meter
* \param [in] unit -unit id
* \param [in] core_id -core id
* \param [in] is_ingress -indicates ingress or egress
* \param [in] database_id -database id
* \param [in] enable -TRUE to enable, FALSE to disable
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_meter_generic_refresh_mechanism_enable(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_meter.meter_db.feature_get(unit, dnx_data_meter_meter_db_refresh_disable_required))
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                          (is_ingress == TRUE ? DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG :
                                           DBAL_TABLE_METER_EGRESS_DB_GENERAL_CONFIG), &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
        /** set the table values */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_REFRESH_ENABLE, INST_SINGLE, enable);
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_profile_id_set(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_idx,
    int profile_idx)
{
    uint32 entry_handle_id;
    int use_single_bucket_bank;
    int hw_index;
    uint32 bank_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.use_single_bucket_bank.get
                    (unit, core_id, is_ingress, database_id, &use_single_bucket_bank));

    /** disable refresh mechanism in order to get access to the HW table */
    SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, core_id, is_ingress, database_id, FALSE));

    if (use_single_bucket_bank)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_PROFILE_SET_FOR_SMALL_ENGINE, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_EGRESS, !(is_ingress));
        /** for small engines, meter_idx is the real hw meter index */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_ID, meter_idx);
        /** set the table values */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, profile_idx);
    }
    else
    {
        /** extract from ptr_map table the bank id it is being used */
        SHR_IF_ERR_EXIT(dnx_meter_generic_ptr_map_hw_get(unit, core_id, is_ingress, database_id,
                                                         (meter_idx /
                                                          dnx_data_meter.mem_mgmt.ptr_map_table_resolution_get(unit)),
                                                         &bank_id));

        /** set the hw index as the meter_idx%resolution. */
        hw_index = meter_idx % dnx_data_meter.mem_mgmt.object_stat_pointer_base_resolution_get(unit);

        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_PROFILE_SET_FOR_BIG_ENGINE, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_ID, bank_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_ID, hw_index);
        /** set the table values */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, profile_idx);
    }
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** enable back refresh mechanism */
    SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, core_id, is_ingress, database_id, TRUE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_profile_id_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_idx,
    int *profile_idx)
{
    uint32 entry_handle_id;
    int use_single_bucket_bank;
    int hw_index;
    uint32 bank_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.use_single_bucket_bank.get
                    (unit, core_id, is_ingress, database_id, &use_single_bucket_bank));

    /** disable refresh mechanism in order to get access to the HW table */
    SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, core_id, is_ingress, database_id, FALSE));

    if (use_single_bucket_bank)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_PROFILE_SET_FOR_SMALL_ENGINE, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_EGRESS, !(is_ingress));
        /** for small engines, meter_idx is the real hw meter index */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_ID, meter_idx);
    }
    else
    {
        /** extract from ptr_map table the bank id it is being used */
        SHR_IF_ERR_EXIT(dnx_meter_generic_ptr_map_hw_get(unit, core_id, is_ingress, database_id,
                                                         (meter_idx /
                                                          dnx_data_meter.mem_mgmt.ptr_map_table_resolution_get(unit)),
                                                         &bank_id));

        /** set the hw index as the meter_idx%resolution. */
        hw_index = meter_idx % dnx_data_meter.mem_mgmt.object_stat_pointer_base_resolution_get(unit);

        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_PROFILE_SET_FOR_BIG_ENGINE, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_ID, bank_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_ID, hw_index);
    }
    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, (uint32 *) profile_idx);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** enable back refresh mechanism */
    SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, core_id, is_ingress, database_id, TRUE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_global_sharing_set(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_idx,
    int global_sharing)
{
    uint32 entry_handle_id;
    int use_single_bucket_bank;
    int hw_index;
    uint32 bank_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.use_single_bucket_bank.get
                    (unit, core_id, is_ingress, database_id, &use_single_bucket_bank));

    /** disable refresh mechanism in order to get access to the HW table */
    SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, core_id, is_ingress, database_id, FALSE));

    if (use_single_bucket_bank)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_SMALL_ENGINES, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_EGRESS, !(is_ingress));
        /** for small engines, meter_idx is the real hw meter index */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX,
                                   meter_idx / dnx_data_meter.meter_db.meter_ids_in_group_get(unit));
    }
    else
    {
        /** extract from ptr_map table the bank id it is being used */
        SHR_IF_ERR_EXIT(dnx_meter_generic_ptr_map_hw_get(unit, core_id, is_ingress, database_id,
                                                         (meter_idx /
                                                          dnx_data_meter.mem_mgmt.ptr_map_table_resolution_get(unit)),
                                                         &bank_id));

        /** set the hw index as the meter_idx%resolution. */
        hw_index = meter_idx % dnx_data_meter.mem_mgmt.object_stat_pointer_base_resolution_get(unit);

        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_BIG_ENGINES, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_ID, bank_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX,
                                   hw_index / dnx_data_meter.meter_db.meter_ids_in_group_get(unit));
    }
    /** set the table values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_SHARING_FLAG, INST_SINGLE, global_sharing);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** enable back refresh mechanism */
    SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, core_id, is_ingress, database_id, TRUE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_global_sharing_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_idx,
    int *global_sharing)
{
    uint32 entry_handle_id;
    int use_single_bucket_bank;
    int hw_index;
    uint32 bank_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.use_single_bucket_bank.get
                    (unit, core_id, is_ingress, database_id, &use_single_bucket_bank));

    /** disable refresh mechanism in order to get access to the HW table */
    SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, core_id, is_ingress, database_id, FALSE));

    if (use_single_bucket_bank)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_SMALL_ENGINES, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_EGRESS, !(is_ingress));
        /** for small engines, meter_idx is the real hw meter index */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX,
                                   meter_idx / dnx_data_meter.meter_db.meter_ids_in_group_get(unit));
    }
    else
    {
        /** extract from ptr_map table the bank id it is being used */
        SHR_IF_ERR_EXIT(dnx_meter_generic_ptr_map_hw_get(unit, core_id, is_ingress, database_id,
                                                         (meter_idx /
                                                          dnx_data_meter.mem_mgmt.ptr_map_table_resolution_get(unit)),
                                                         &bank_id));

        /** set the hw index as the meter_idx%resolution. */
        hw_index = meter_idx % dnx_data_meter.mem_mgmt.object_stat_pointer_base_resolution_get(unit);

        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_BIG_ENGINES, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_ID, bank_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX,
                                   hw_index / dnx_data_meter.meter_db.meter_ids_in_group_get(unit));
    }
    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GLOBAL_SHARING_FLAG, INST_SINGLE,
                               (uint32 *) global_sharing);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** enable back refresh mechanism */
    SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, core_id, is_ingress, database_id, TRUE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_ptr_map_hw_set(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int ptr_base,
    uint32 bank_id)
{
    uint32 entry_handle_id;
    uint8 index;
    uint32 value;
    uint32 nof_indexes, nof_bits;
    int bank_id_first_bit, i;
    dbal_table_field_info_t bank_field_info, index_field_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** nof indexes to update the the ratio between the object_stat_pointer resolution and the table resolution */
    nof_indexes = dnx_data_meter.mem_mgmt.object_stat_pointer_base_resolution_get(unit) /
        dnx_data_meter.mem_mgmt.ptr_map_table_resolution_get(unit);
    nof_bits = nof_indexes - 1;
    SHR_BITCOUNT_RANGE(&nof_bits, bank_id_first_bit, 0, UTILEX_NOF_BITS_IN_CHAR);

    /** get from dbal the size of bank id*/
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_ING_DATABASE_MEMORY_MAP, DBAL_FIELD_BANK_ID,
                                               FALSE, 0, 0, &bank_field_info));
    /** get from dbal the size of the table index */
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_ING_PTR_MAP, DBAL_FIELD_INDEX,
                                               TRUE, 0, 0, &index_field_info));

    /** during init stage, we set all indexes to invalid bank_id. therefore nof_indexes will be change and represent all indexes in the table*/
    if (ptr_base == DNX_METER_PTR_MAP_INDEX_ALL)
    {
        nof_indexes = index_field_info.max_value + 1;
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      (is_ingress ? DBAL_TABLE_METER_ING_PTR_MAP : DBAL_TABLE_METER_EGR_PTR_MAP),
                                      &entry_handle_id));

    /** need to update several consecutive indexes */
    for (i = 0; i < nof_indexes; i++)
    {
        /** calc the index. the index is the Msb bits of the pointer */
        /** if the ptr_base is invalid, so need to run ovel all table, so the index is i */
        index = (ptr_base == DNX_METER_PTR_MAP_INDEX_ALL) ? i :
            (ptr_base / dnx_data_meter.mem_mgmt.ptr_map_table_resolution_get(unit)) + i;

        /** the value of the table holds the bank_id. update it accordingly */
        value = index;
        SHR_BITCOPY_RANGE(&value, bank_id_first_bit, &bank_id, 0, bank_field_info.field_nof_bits);

        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA, INST_SINGLE, value);

        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_ptr_map_hw_data_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int index,
    uint32 *data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      ((is_ingress ==
                                        TRUE) ? DBAL_TABLE_METER_ING_PTR_MAP : DBAL_TABLE_METER_EGR_PTR_MAP),
                                      &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);
    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DATA, INST_SINGLE, data);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_ptr_map_hw_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int index,
    uint32 *bank_id)
{
    uint32 data;
    dbal_table_field_info_t bank_field_info;
    uint32 nof_indexes, nof_bits;
    int bank_id_first_bit;
    uint32 bank_arr[1] = { 0 };
                              /** coverity */

    SHR_FUNC_INIT_VARS(unit);

    /** calc the first bit of the bank id */
    nof_indexes = dnx_data_meter.mem_mgmt.object_stat_pointer_base_resolution_get(unit) /
        dnx_data_meter.mem_mgmt.ptr_map_table_resolution_get(unit);
    nof_bits = nof_indexes - 1;
    SHR_BITCOUNT_RANGE(&nof_bits, bank_id_first_bit, 0, UTILEX_NOF_BITS_IN_CHAR);

    /** get from dbal the size of bank id*/
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_ING_DATABASE_MEMORY_MAP, DBAL_FIELD_BANK_ID,
                                               FALSE, 0, 0, &bank_field_info));

    SHR_IF_ERR_EXIT(dnx_meter_generic_ptr_map_hw_data_get(unit, core_id, is_ingress, database_id, index, &data));
    /** extruct the bank id */
    SHR_BITCOPY_RANGE(bank_arr, 0, &data, bank_id_first_bit, bank_field_info.field_nof_bits);
    *bank_id = bank_arr[0];

exit:
    SHR_FUNC_EXIT;
}

/** see .h file  */
shr_error_e
dnx_meter_generic_address_range_hw_set(
    int unit,
    int database_core,
    int is_ingress,
    int database_id,
    int nof_meters)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      (is_ingress ? DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG :
                                       DBAL_TABLE_METER_EGRESS_DB_GENERAL_CONFIG), &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, database_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_METERS, INST_SINGLE,
                                 (nof_meters / dnx_data_meter.mem_mgmt.adrress_range_resolution_get(unit)));

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_address_map_hw_set(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int map_index,
    int bank_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      (is_ingress ? DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG :
                                       DBAL_TABLE_METER_EGRESS_DB_GENERAL_CONFIG), &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    /** Setting value fields */
    /** if nof_banks = -1, it mean that the maping should be disabled */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEMORY_BANKS_MAPPING_ENABLE, INST_SINGLE,
                                 (bank_id != -1));

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (bank_id != -1)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                          (is_ingress ? DBAL_TABLE_METER_ING_DATABASE_MEMORY_MAP :
                                           DBAL_TABLE_METER_EGR_DATABASE_MEMORY_MAP), &entry_handle_id));

        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, map_index);
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_ID, INST_SINGLE, bank_id);
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_clear_all(
    int unit)
{
    int database_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** disable refresh mechanism in order to get access to the HW table */
    for (database_id = 0; database_id < dnx_data_meter.meter_db.nof_ingress_db_get(unit); database_id++)
    {
        SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, BCM_CORE_ALL, 1, database_id, FALSE));
    }
    for (database_id = 0; database_id < dnx_data_meter.meter_db.nof_egress_db_get(unit); database_id++)
    {
        SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, BCM_CORE_ALL, 0, database_id, FALSE));
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_METER_PROFILE_SET_FOR_SMALL_ENGINE));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_SMALL_ENGINES));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_METER_PROFILE_SET_FOR_BIG_ENGINE));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_BIG_ENGINES));

    /** enable back refresh mechanism */
    for (database_id = 0; database_id < dnx_data_meter.meter_db.nof_ingress_db_get(unit); database_id++)
    {
        SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, BCM_CORE_ALL, 1, database_id, TRUE));
    }
    for (database_id = 0; database_id < dnx_data_meter.meter_db.nof_egress_db_get(unit); database_id++)
    {
        SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, BCM_CORE_ALL, 0, database_id, TRUE));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/** see .h file */
shr_error_e
dnx_meter_generic_engine_clear(
    int unit,
    int core_id,
    bcm_policer_engine_t * engine,
    int is_ingress,
    int database_id,
    int single_bucket_bank)
{
    dnx_crps_mgmt_shmem_section_t section_arr[2];
    int nof_sections, section_idx;
    int bank_id;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, core_id, is_ingress, database_id, FALSE));

    if (single_bucket_bank)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_PROFILE_SET_FOR_SMALL_ENGINE, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_EGRESS, !(is_ingress));
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_METER_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        /** set the table values */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, 0);
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_SMALL_ENGINES, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_EGRESS, !(is_ingress));
        /** all meters range */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INDEX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        /** set the table values - clear it*/
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_SHARING_FLAG, INST_SINGLE, 0);
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        if (engine->section == bcmPolicerEngineSectionAll)
        {
            section_arr[0] = dnx_crps_mgmt_shmem_section_low;
            section_arr[1] = dnx_crps_mgmt_shmem_section_high;
            nof_sections = 2;
        }
        else
        {
            section_arr[0] = (bcm_policer_engine_section_t) engine->section;
            nof_sections = 1;
        }
        for (section_idx = 0; section_idx < nof_sections; section_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_meter_shmem_bank_id_get
                            (unit, engine->engine_id, section_arr[section_idx], &bank_id));
            /** Taking a handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_PROFILE_SET_FOR_BIG_ENGINE, &entry_handle_id));
            /** Setting key fields */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_ID, bank_id);
            dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_METER_ID, DBAL_RANGE_ALL,
                                             DBAL_RANGE_ALL);
            /** clear the table */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, 0);
            /** Preforming the action */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Taking a handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_BIG_ENGINES, &entry_handle_id));
            /** Setting key fields */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_ID, bank_id);
            dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INDEX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
            /** set the table values - clear it */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_SHARING_FLAG, INST_SINGLE, 0);
            /** Preforming the action */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

    SHR_IF_ERR_EXIT(dnx_meter_generic_refresh_mechanism_enable(unit, core_id, is_ingress, database_id, TRUE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_fairness_set(
    int unit,
    bcm_core_t core_id,
    int is_ingress,
    int database_id,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** ingress settings */
    if (is_ingress)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG, &entry_handle_id));
    }
    /** egress settings */
    else
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGRESS_DB_GENERAL_CONFIG, &entry_handle_id));
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CBL_RANDOM_MODE_ENABLE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EBL_RANDOM_MODE_ENABLE, INST_SINGLE, enable);
    /** set the range of random values add/substract to the bucket level as the highest posssible range (-8K..8K) */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RANDOM_RANGE, INST_SINGLE, 0);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_fairness_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    uint32 *enabled)
{
    uint32 hw_enabled;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      ((is_ingress ==
                                        TRUE) ? DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG :
                                       DBAL_TABLE_METER_EGRESS_DB_GENERAL_CONFIG), &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    /** get value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CBL_RANDOM_MODE_ENABLE, INST_SINGLE, &hw_enabled);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *enabled = hw_enabled;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_meter_generic_index_verify(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_idx,
    int *valid)
{
    int core_idx;
    int single_bucket_bank;
    uint32 nof_banks;
    int bank_idx, nof_meters;
    int base_meter_index;

    SHR_FUNC_INIT_VARS(unit);

    *valid = TRUE;

    /** check that the meter_idx that user gave belongs to the given database. */
    /** meter_idx is the meter pointer (after expansion) that is used by the meter processor.*/
    /** need to go over all the banks in the database and verify that meter idx is in one of the banks range */
    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.use_single_bucket_bank.get
                        (unit, core_idx, is_ingress, database_id, &single_bucket_bank));

        if (single_bucket_bank == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.nof_generic_banks.get
                            (unit, core_idx, is_ingress, database_id, &nof_banks));
            for (bank_idx = 0; bank_idx < nof_banks; bank_idx++)
            {
                SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.banks_arr.base_meter_index.get
                                (unit, core_idx, is_ingress, database_id, bank_idx, &base_meter_index));
                if ((meter_idx >= base_meter_index) &&
                    (meter_idx <
                     (base_meter_index + dnx_data_meter.mem_mgmt.object_stat_pointer_base_resolution_get(unit))))
                {
                    break;
                }
            }
            /** if bank not found in database, return an error */
            if (bank_idx == nof_banks)
            {
                *valid = FALSE;
            }
        }
        else
        {
            /** for single bucket database, the base_meter is zero and the meter_idx should 
                be smaller than the size of the engine */
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_nof_meter_in_database_get
                            (unit, core_idx, is_ingress, database_id, &nof_meters));
            if (meter_idx >= nof_meters)
            {
                *valid = FALSE;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

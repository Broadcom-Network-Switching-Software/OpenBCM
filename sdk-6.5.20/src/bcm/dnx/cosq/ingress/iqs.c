/** \file iqs.c
 * 
 *
 * Module IQS - Ingress Queue Scheduling
 *
 * Configuration of Ingress TM queue scheduling, inclues:
 *  * Credit request profiles
 *  * Credit worth
 *  * FSM reorder mechanism
 *
 *  Not including:
 *  * FMQ Credit generation(implementation in fmq.c)
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

/*
 * Include files.
 * {
 */
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/rate/algo_rates.h>
#include <bcm_int/dnx/tune/iqs_tune.h>
#include <bcm_int/dnx/fabric/fabric_transmit.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_iqs_access.h>

#include <shared/shrextend/shrextend_error.h>

#include "iqs.h"
#include "fmq.h"


#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

/*
 * }
 */

/*
 * Defines
 * {
 */

/**
 * \brief - resolution of credit request slow levels - 256B
 */
#define DNX_IQS_CREDIT_REQURST_SLOW_LEVEL_RESOLUTION        256
/**
 * \brief - longest credit watchdog cycle allowed (micro second)
 * Use the same limitation as in legacy devices.
 */
#define DNX_IQS_CREDIT_WATCHDOG_LONGEST_CYCLE_US            (2000)

/**
 * \brief - convert cycle in clocks to period units to set the device.
 */
#define DNX_IQS_CREDIT_WATCHDOG_CYCLE_PERIOD_UNITS(_cycle_clocks)        (_cycle_clocks / 2048)

/**
 * \brief - max values for credit watchdog delete thresholds (for exponent and mant)
 */
#define DNX_IQS_DELETE_TH_EXP_MAX (0xf)
#define DNX_IQS_DELETE_TH_MANT_MAX (0xf)
/*
 * }
 */
/*
 * Credit Request Profile
 * {
 */

/**
 * \brief
 *   Initialize Credit Request
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_iqs_credit_request_init(
    int unit)
{
    uint32 entry_handle_id;
    const dnxc_data_table_info_t *table_info;
    int profile_id, max_deq_cmd;
    int bw_level, credit_balance, credit_balance_max, deq_credit_balance_resolution, credit_resolution;
    int threshold_index;
    uint32 sram_read_weight_profile = 0, s2d_read_weight_profile = 0, dram_read_weight_profile = 0;
    uint32 sram_extra_credits, s2d_extra_credits;
    uint32 sram_extra_credits_lfsr, s2d_extra_credits_lfsr;
    const dnx_data_iqs_deq_default_params_t *deq_params;
    const dnx_data_iqs_deq_default_dram_params_t *dram_deq_params;
    const dnxc_data_table_info_t *dram_params_table_info;
    dbal_enum_value_field_iqs_active_queues_e nof_active_queues;
    uint32 lfsr_default_thr = dnx_data_iqs.deq_default.sram_to_fabric_credit_lfsr_thr_get(unit);
    uint32 lfsr_default_mask = dnx_data_iqs.deq_default.sram_to_fabric_credit_lfsr_mask_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Allocate sw state
     */
    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_request_profile.alloc(unit));

    /*
     * Set low delay thresholds
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_REQUEST_LOW_DELAY, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEQUEUE_BYTES, INST_SINGLE,
                                 dnx_data_iqs.deq_default.low_delay_deq_bytes_get(unit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /*
     * Set queue read weight profiles.
     * Both sqm and dqm have the same amount of profiles
     */
    table_info = dnx_data_iqs.deq_default.sqm_read_weight_profiles_info_get(unit);
    for (profile_id = 0; profile_id < table_info->key_size[0]; profile_id++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_IQS_CREDIT_REQUEST_QUEUE_READ_WEIGHT_PROFILE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

        /** SQM */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SQM_READ_WEIGHT, INST_SINGLE,
                                     dnx_data_iqs.deq_default.sqm_read_weight_profiles_get(unit, profile_id)->weight);

        /** DQM */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DQM_READ_WEIGHT, INST_SINGLE,
                                     dnx_data_iqs.deq_default.dqm_read_weight_profiles_get(unit, profile_id)->weight);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }

    /*
     *  SRAM and DRAM DEQ parameters
     */
    credit_balance_max = dnx_data_iqs.deq_default.credit_balance_max_get(unit);
    deq_credit_balance_resolution = dnx_data_iqs.deq_default.credit_balance_resolution_get(unit);

    for (bw_level = 0; bw_level < dnx_data_iqs.credit.nof_bw_levels_get(unit); bw_level++)
    {
        SHR_IF_ERR_EXIT(dnx_tune_iqs_bw_level_to_credit_resolution_get(unit, bw_level, &credit_resolution));
        deq_params = dnx_data_iqs.deq_default.params_get(unit, bw_level);
        max_deq_cmd =
            dnx_data_iqs.deq_default.max_deq_cmd_get(unit, bw_level, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE)->value;

        for (credit_balance = 0; credit_balance < credit_balance_max;
             credit_balance += deq_credit_balance_resolution * credit_resolution)
        {
            /** SRAM */
            {
                sram_read_weight_profile = 0;
                sram_extra_credits = 0;
                sram_extra_credits_lfsr = 0;

                if (credit_balance < max_deq_cmd)
                {
                    threshold_index = DNX_IQS_DEQ_PARAM_TYPE_MAX_DEQ_CMD_BELOW;
                }
                else if ((bw_level == 0) || (bw_level == 1))
                {
                    threshold_index = DNX_IQS_DEQ_PARAM_TYPE_MAX_DEQ_CMD_ABOVE;
                }
                else if (credit_balance < max_deq_cmd * 150 / 100)
                {
                    threshold_index = DNX_IQS_DEQ_PARAM_TYPE_MAX_DEQ_CMD_ABOVE;
                }
                else
                {
                    threshold_index = DNX_IQS_DEQ_PARAM_TYPE_MAX_DEQ_CMD_ABOVE_EXTREME;
                }

                sram_read_weight_profile = deq_params->sram_read_weight_profile_val[threshold_index];
                sram_extra_credits = deq_params->sram_extra_credits_val[threshold_index];
                sram_extra_credits_lfsr = deq_params->sram_extra_credits_lfsr_val[threshold_index];
            }

            /** DRAM - bw level is irrelevant (althought HW is per bw level) */
            {
                dram_params_table_info = dnx_data_iqs.deq_default.dram_params_info_get(unit);

                for (threshold_index = 0; threshold_index < dram_params_table_info->key_size[0]; threshold_index++)
                {
                    dram_deq_params = dnx_data_iqs.deq_default.dram_params_get(unit, threshold_index);
                    dram_read_weight_profile = 0;

                    if (credit_balance <= dram_deq_params->read_weight_profile_th)
                    {
                        dram_read_weight_profile = dram_deq_params->read_weight_profile_val;
                        break;
                    }
                }
            }

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_DEQ_PARAMS, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BW_LEVEL, bw_level);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_BALANCE_MSB,
                                       credit_balance / (deq_credit_balance_resolution * credit_resolution));

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_READ_WEIGHT_PROFILE, INST_SINGLE,
                                         sram_read_weight_profile);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_READ_WEIGHT_PROFILE, INST_SINGLE,
                                         dram_read_weight_profile);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_DEQ_EXTRA_CREDITS, INST_SINGLE,
                                         sram_extra_credits);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_DEQ_EXTRA_CREDITS_LFSR, INST_SINGLE,
                                         sram_extra_credits_lfsr);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_id);
        }
    }

    /*
     * S2D dequeue commands paramaters
     */
    for (bw_level = 0; bw_level < dnx_data_iqs.credit.nof_bw_levels_get(unit); bw_level++)
    {
        SHR_IF_ERR_EXIT(dnx_tune_iqs_bw_level_to_credit_resolution_get(unit, bw_level, &credit_resolution));
        deq_params = dnx_data_iqs.deq_default.params_get(unit, bw_level);
        max_deq_cmd =
            dnx_data_iqs.deq_default.s2d_max_deq_cmd_get(unit, bw_level,
                                                         DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE)->value;

        for (credit_balance = 0; credit_balance < credit_balance_max;
             credit_balance += deq_credit_balance_resolution * credit_resolution)
        {
            s2d_read_weight_profile = 0;
            s2d_extra_credits = 0;
            s2d_extra_credits_lfsr = 0;

            if (credit_balance < max_deq_cmd)
            {
                threshold_index = DNX_IQS_DEQ_PARAM_TYPE_MAX_DEQ_CMD_BELOW;
            }
            else if ((bw_level == 0) || (bw_level == 1))
            {
                threshold_index = DNX_IQS_DEQ_PARAM_TYPE_MAX_DEQ_CMD_ABOVE;
            }
            else if (credit_balance < max_deq_cmd * 150 / 100)
            {
                threshold_index = DNX_IQS_DEQ_PARAM_TYPE_MAX_DEQ_CMD_ABOVE;
            }
            else
            {
                threshold_index = DNX_IQS_DEQ_PARAM_TYPE_MAX_DEQ_CMD_ABOVE_EXTREME;
            }

            s2d_read_weight_profile = deq_params->s2d_read_weight_profile_val[threshold_index];
            s2d_extra_credits = deq_params->s2d_extra_credits_val[threshold_index];
            s2d_extra_credits_lfsr = deq_params->s2d_extra_credits_lfsr_val[threshold_index];

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_DEQ_PARAMS, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BW_LEVEL, bw_level);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_BALANCE_MSB,
                                       credit_balance / (deq_credit_balance_resolution * credit_resolution));

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_S2D_READ_WEIGHT_PROFILE, INST_SINGLE,
                                         s2d_read_weight_profile);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_S2D_DEQ_EXTRA_CREDITS, INST_SINGLE,
                                         s2d_extra_credits);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_S2D_DEQ_EXTRA_CREDITS_LFSR, INST_SINGLE,
                                         s2d_extra_credits_lfsr);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_id);
        }
    }

    /*
     * Set max dequeue command parameters (sram/dram to fabric)
     */
    for (bw_level = 0; bw_level < dnx_data_iqs.credit.nof_bw_levels_get(unit); bw_level++)
    {
        for (nof_active_queues = DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE;
             nof_active_queues < DBAL_NOF_ENUM_IQS_ACTIVE_QUEUES_VALUES; nof_active_queues++)
        {

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_MAX_DEQ_CMDS, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BW_LEVEL, bw_level);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTIVE_QUEUES, nof_active_queues);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_DEQ_CMD, INST_SINGLE,
                                         dnx_data_iqs.deq_default.max_deq_cmd_get(unit, bw_level,
                                                                                  nof_active_queues)->value);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_id);
        }

        /** set SRAM to Fabric LFSR thresholds */
        /** same configurations should apply even if the system is loaded without DRAM */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_SRAM_TO_FABRIC_CRDT_LFSR_THR, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BW_LEVEL, bw_level);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LFSR_THR, INST_SINGLE, lfsr_default_thr);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LFSR_MASK, INST_SINGLE, lfsr_default_mask);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }

    /*
     * Set max dequeue command parameters (sram to dram)
     */
    for (bw_level = 0; bw_level < dnx_data_iqs.credit.nof_bw_levels_get(unit); bw_level++)
    {
        for (nof_active_queues = DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE;
             nof_active_queues < DBAL_NOF_ENUM_IQS_ACTIVE_QUEUES_VALUES; nof_active_queues++)
        {

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_S2D_MAX_DEQ_CMDS, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BW_LEVEL, bw_level);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTIVE_QUEUES, nof_active_queues);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_DEQ_CMD, INST_SINGLE,
                                         dnx_data_iqs.deq_default.s2d_max_deq_cmd_get(unit, bw_level,
                                                                                      nof_active_queues)->value);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_id);
        }
    }

    /*
     * FMQ thresholds
     */
    if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == 0)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_REQUEST_FMQ_TH, &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_ON_TH, INST_SINGLE,
                                     dnx_data_iqs.credit.fmq_credit_fc_on_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_OFF_TH, INST_SINGLE,
                                     dnx_data_iqs.credit.fmq_credit_fc_off_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYTE_ON_TH, INST_SINGLE,
                                     dnx_data_iqs.credit.fmq_byte_fc_on_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYTE_OFF_TH, INST_SINGLE,
                                     dnx_data_iqs.credit.fmq_byte_fc_off_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EIR_CREDIT_ON_TH, INST_SINGLE,
                                     dnx_data_iqs.credit.fmq_eir_credit_fc_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EIR_CREDIT_OFF_TH, INST_SINGLE,
                                     dnx_data_iqs.credit.fmq_eir_credit_fc_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EIR_BYTE_ON_TH, INST_SINGLE,
                                     dnx_data_iqs.credit.fmq_eir_byte_fc_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EIR_BYTE_OFF_TH, INST_SINGLE,
                                     dnx_data_iqs.credit.fmq_eir_byte_fc_th_get(unit));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get lowest credit balance resolution possible according to the highet threshold written in credit balance resolution
 */
static shr_error_e
dnx_iqs_credit_request_credit_balance_resolution_get(
    int unit,
    bcm_cosq_delay_tolerance_t * thresholds,
    int *credit_balance_resolution,
    dbal_enum_value_field_iqs_credit_balance_resolution_e * credit_balance_resolution_dbal)
{
    uint32 max_value = 0;
    uint32 max_threshold;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * find max threshold
     */
    /** satisfied */
    max_value = UTILEX_MAX(max_value, thresholds->credit_request_satisfied_backoff_enter_thresh);
    max_value = UTILEX_MAX(max_value, thresholds->credit_request_satisfied_backoff_exit_thresh);
    max_value = UTILEX_MAX(max_value, thresholds->credit_request_satisfied_backlog_enter_thresh);
    max_value = UTILEX_MAX(max_value, thresholds->credit_request_satisfied_backlog_exit_thresh);
    max_value = UTILEX_MAX(max_value, thresholds->credit_request_satisfied_backslow_enter_thresh);
    max_value = UTILEX_MAX(max_value, thresholds->credit_request_satisfied_backslow_exit_thresh);

    /** satisfied empty */
    max_value = UTILEX_MAX(max_value, utilex_abs(thresholds->credit_request_satisfied_empty_queue_thresh));
    max_value = UTILEX_MAX(max_value, utilex_abs(thresholds->credit_request_satisfied_empty_queue_max_balance_thresh));

    /** hungry */
    max_value = UTILEX_MAX(max_value, utilex_abs(thresholds->credit_request_hungry_off_to_slow_thresh));
    max_value = UTILEX_MAX(max_value, utilex_abs(thresholds->credit_request_hungry_off_to_normal_thresh));
    max_value = UTILEX_MAX(max_value, utilex_abs(thresholds->credit_request_hungry_slow_to_normal_thresh));
    max_value = UTILEX_MAX(max_value, utilex_abs(thresholds->credit_request_hungry_normal_to_slow_thresh));

    /*
     * max value to credit balance resolution
     */
    max_threshold = dnx_data_iqs.credit.max_credit_balance_threshold_get(unit);
    /** 1B resolution */
    *credit_balance_resolution = 1;
    *credit_balance_resolution_dbal = DBAL_ENUM_FVAL_IQS_CREDIT_BALANCE_RESOLUTION_1B;

    /** 2B resolution */
    if (max_value / *credit_balance_resolution > max_threshold)
    {
        *credit_balance_resolution = 2;
        *credit_balance_resolution_dbal = DBAL_ENUM_FVAL_IQS_CREDIT_BALANCE_RESOLUTION_2B;

        /** 4B resolution */
        if (max_value / *credit_balance_resolution > max_threshold)
        {
            *credit_balance_resolution = 4;
            *credit_balance_resolution_dbal = DBAL_ENUM_FVAL_IQS_CREDIT_BALANCE_RESOLUTION_4B;
        }
    }

    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_request_profile_set(
    int unit,
    int profile_id,
    bcm_cosq_delay_tolerance_t * thresholds)
{
    int src_slow_level;
    int dst_slow_level;
    uint32 entry_handle_id;
    int credit_balance_reolution = 1;
    dbal_enum_value_field_iqs_credit_balance_resolution_e credit_balance_resolution_dbal =
        DBAL_ENUM_FVAL_IQS_CREDIT_BALANCE_RESOLUTION_1B;
    uint32 is_high_priority, is_low_delay, is_ocb_only;
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Store the original values in sw state - resolution might be changed */
    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_request_profile.thresholds.set(unit, profile_id, *thresholds));

    /** calc required credit balance resolution */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_request_credit_balance_resolution_get
                    (unit, thresholds, &credit_balance_reolution, &credit_balance_resolution_dbal));

    /** satisfied thresholds */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_REQUEST_SATISFIED_THRESHOLDS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    value = thresholds->credit_request_satisfied_backoff_enter_thresh / credit_balance_reolution;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BACKOFF_ENTER, INST_SINGLE, value);
    value = thresholds->credit_request_satisfied_backoff_exit_thresh / credit_balance_reolution;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BACKOFF_EXIT, INST_SINGLE, value);
    value = thresholds->credit_request_satisfied_backlog_enter_thresh / credit_balance_reolution;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BACKLOG_ENTER, INST_SINGLE, value);
    value = thresholds->credit_request_satisfied_backlog_exit_thresh / credit_balance_reolution;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BACKLOG_EXIT, INST_SINGLE, value);
    value = thresholds->credit_request_satisfied_backslow_enter_thresh / credit_balance_reolution;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BACKSLOW_ENTER, INST_SINGLE, value);
    value = thresholds->credit_request_satisfied_backslow_exit_thresh / credit_balance_reolution;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BACKSLOW_EXIT, INST_SINGLE, value);
    value = utilex_abs(thresholds->credit_request_satisfied_empty_queue_thresh) / credit_balance_reolution;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EMPTY_QUEUE, INST_SINGLE, value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EMPTY_QUEUE_SIGN, INST_SINGLE,
                                 utilex_is_neg(thresholds->credit_request_satisfied_empty_queue_thresh));
    value = utilex_abs(thresholds->credit_request_satisfied_empty_queue_max_balance_thresh) / credit_balance_reolution;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EMPTY_QUEUE_MAX, INST_SINGLE, value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EMPTY_QUEUE_MAX_SIGN, INST_SINGLE,
                                 utilex_is_neg(thresholds->credit_request_satisfied_empty_queue_max_balance_thresh));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EMPTY_QUEUE_EXCEED, INST_SINGLE,
                                 UTILEX_NUM2BOOL(thresholds->credit_request_satisfied_empty_queue_exceed_thresh));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /** hungry thresholds */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_REQUEST_HUNGRY_THRESHOLDS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MULT, INST_SINGLE,
                                 utilex_log2_round_up(thresholds->credit_request_hungry_multiplier));
    value = utilex_abs(thresholds->credit_request_hungry_off_to_slow_thresh) / credit_balance_reolution;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFF_TO_SLOW, INST_SINGLE, value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFF_TO_SLOW_SIGN, INST_SINGLE,
                                 utilex_is_neg(thresholds->credit_request_hungry_off_to_slow_thresh));
    value = utilex_abs(thresholds->credit_request_hungry_off_to_normal_thresh) / credit_balance_reolution;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFF_TO_NORM, INST_SINGLE, value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFF_TO_NORM_SIGN, INST_SINGLE,
                                 utilex_is_neg(thresholds->credit_request_hungry_off_to_normal_thresh));
    value = utilex_abs(thresholds->credit_request_hungry_slow_to_normal_thresh) / credit_balance_reolution;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOW_TO_NORM, INST_SINGLE, value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOW_TO_NORM_SIGN, INST_SINGLE,
                                 utilex_is_neg(thresholds->credit_request_hungry_slow_to_normal_thresh));
    value = utilex_abs(thresholds->credit_request_hungry_normal_to_slow_thresh) / credit_balance_reolution;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NORM_TO_SLOW, INST_SINGLE, value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NORM_TO_SLOW_SIGN, INST_SINGLE,
                                 utilex_is_neg(thresholds->credit_request_hungry_normal_to_slow_thresh));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /** slow thresholds*/
    for (dst_slow_level = 0; dst_slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; dst_slow_level++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_REQUEST_SLOW_FACTOR_DOWN, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DEST_SLOW_LEVEL, dst_slow_level);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOW_FACTOR_DOWN, INST_SINGLE,
                                     UTILEX_DIV_ROUND_DOWN(thresholds->slow_level_thresh_down[dst_slow_level],
                                                           DNX_IQS_CREDIT_REQURST_SLOW_LEVEL_RESOLUTION));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }
    for (src_slow_level = 0; src_slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; src_slow_level++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_REQUEST_SLOW_FACTOR_UP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SRC_SLOW_LEVEL, src_slow_level);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOW_FACTOR_UP, INST_SINGLE,
                                     UTILEX_DIV_ROUND_UP(thresholds->slow_level_thresh_up[src_slow_level],
                                                         DNX_IQS_CREDIT_REQURST_SLOW_LEVEL_RESOLUTION));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }

    /** attributes */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_REQUEST_PROFILE_GENERAL_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    is_low_delay = UTILEX_NUM2BOOL(thresholds->flags & BCM_COSQ_DELAY_TOLERANCE_IS_LOW_LATENCY);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LOW_DELAY, INST_SINGLE, is_low_delay);
    is_high_priority = UTILEX_NUM2BOOL(thresholds->flags & BCM_COSQ_DELAY_TOLERANCE_HIGH_Q_PRIORITY);
    if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities) == 0)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HIGH_PRIORITY, INST_SINGLE, is_high_priority);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BW_LEVEL, INST_SINGLE, thresholds->bandwidth_level);
    is_ocb_only = UTILEX_NUM2BOOL(thresholds->flags & BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OCB_ONLY, INST_SINGLE, is_ocb_only);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_BALANCE_RESOLUTION, INST_SINGLE,
                                 credit_balance_resolution_dbal);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /*
     * Mark as valid profile
     */
    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_request_profile.valid.set(unit, profile_id, 1));

    /*
     * Set SW state copy - to be used by get
     */
    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_request_profile.thresholds.set(unit, profile_id, *thresholds));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get credit request profile credit balance resolution value
 *
 * \param [in] unit -  Unit-ID
 * \param [in] profile_id - credit request profile_id
 * \param [out] credit_balance_resolution -  credit balance resolution value
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_iqs_credit_request_profile_credit_balance_resolution_get(
    int unit,
    int profile_id,
    uint32 *credit_balance_resolution)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_iqs_credit_balance_resolution_e credit_balance_resolution_dbal =
        DBAL_ENUM_FVAL_IQS_CREDIT_BALANCE_RESOLUTION_1B;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_REQUEST_PROFILE_GENERAL_CONFIG, &entry_handle_id));

    /**
     * Set profile id key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
    /*
     * Get
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDIT_BALANCE_RESOLUTION, INST_SINGLE,
                               &credit_balance_resolution_dbal);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * Parse dbal enum to actual credit balance resolution
     */
    switch (credit_balance_resolution_dbal)
    {
        case DBAL_ENUM_FVAL_IQS_CREDIT_BALANCE_RESOLUTION_1B:
            *credit_balance_resolution = 1;
            break;
        case DBAL_ENUM_FVAL_IQS_CREDIT_BALANCE_RESOLUTION_2B:
            *credit_balance_resolution = 2;
            break;
        case DBAL_ENUM_FVAL_IQS_CREDIT_BALANCE_RESOLUTION_4B:
            *credit_balance_resolution = 4;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid dbal credit balance resolution value received %d",
                         credit_balance_resolution_dbal);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to get queue state signals and credit request info
 */
shr_error_e
dnx_iqs_dbal_voq_state_info_get(
    int unit,
    int core,
    int queue_id,
    dnx_iqs_voq_state_info_t * voq_state_info)
{
    uint32 entry_handle_id;
    uint32 credit_balance_field;
    int credit_balance_bits;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_VOQ_STATE_INFO, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Select the queue */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ, queue_id);

    /**
     * Request the queue credit request state
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QUEUE_CREDIT_REQUEST_STATE, INST_SINGLE,
                               (uint32 *) &voq_state_info->credit_request_state);

    /**
     * Request the queue credit balance
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QUEUE_CREDIT_BALANCE, INST_SINGLE,
                               &credit_balance_field);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_IQS_VOQ_STATE_INFO, DBAL_FIELD_QUEUE_CREDIT_BALANCE,
                                               0, 0, 0, &credit_balance_bits));

    voq_state_info->credit_balance = UTILEX_TWO_COMPLEMENT_INTO_SIGNED_NUM(credit_balance_field, credit_balance_bits);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to get queue state signals and credit request info
 */
shr_error_e
dnx_iqs_voq_state_info_get(
    int unit,
    bcm_gport_t voq_gport,
    int cos,
    dnx_iqs_voq_state_info_t * voq_state_info)
{
    int profile, weight, queue, core;
    uint32 credit_balance_resolution = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Get queue id */
    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(voq_gport))
    {
        queue = BCM_GPORT_MCAST_QUEUE_GROUP_QUEUE_GET(voq_gport);
        core = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(voq_gport);
    }
    else
    {
        queue = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(voq_gport);
        core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(voq_gport);
    }
    /**
     * Take cos offset into consideration
     */
    queue += cos;

    SHR_IF_ERR_EXIT(dnx_iqs_dbal_voq_state_info_get(unit, core, queue, voq_state_info));
    /**
     * Get credit request profile
     */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, voq_gport, cos, &profile, &weight));
    /**
     * Get profile's credit_balance_resolution
     */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_request_profile_credit_balance_resolution_get
                    (unit, profile, &credit_balance_resolution));
    /**
     * Normalize the credit balance taking into account the credit balance resolution
     */
    voq_state_info->credit_balance *= credit_balance_resolution;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_request_profile_get(
    int unit,
    int profile_id,
    bcm_cosq_delay_tolerance_t * thresholds)
{
    SHR_FUNC_INIT_VARS(unit);

    /** get data from sw state */
    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_request_profile.thresholds.get(unit, profile_id, thresholds));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_request_profile_map_get(
    int unit,
    int core,
    int queue_id,
    int queue_offset,
    int *profile_id)
{
    uint32 entry_handle_id;
    int num_cos;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verfiy queue id */
    SHR_RANGE_VERIFY(queue_id + queue_offset, 0, dnx_data_ipq.queues.nof_queues_get(unit) - 1, _SHR_E_PARAM,
                     "queue_id out of bound %d.\n", queue_id + queue_offset);
    /** Verifty*/
    SHR_IF_ERR_EXIT(dnx_ipq_queue_num_cos_get(unit, core, queue_id, &num_cos));
    SHR_RANGE_VERIFY(queue_offset, 0, num_cos - 1, _SHR_E_PARAM, "queue offset out of bound %d (max is %d).\n",
                     queue_offset, num_cos - 1);

    /*
     * Get
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_REQUEST_PROFILE_MAP, &entry_handle_id));

    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_ID, queue_id + queue_offset);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, (uint32 *) profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_request_profile_map_set(
    int unit,
    int core,
    int queue_id,
    int queue_offset,
    int profile_id)
{
    uint32 entry_handle_id;
    int num_cos;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verfiy queue id */
    SHR_RANGE_VERIFY(queue_id + queue_offset, 0, dnx_data_ipq.queues.nof_queues_get(unit) - 1, _SHR_E_PARAM,
                     "queue_id out of bound %d.\n", queue_id + queue_offset);
    /** Verifty*/
    SHR_IF_ERR_EXIT(dnx_ipq_queue_num_cos_get(unit, core, queue_id, &num_cos));
    SHR_RANGE_VERIFY(queue_offset, 0, num_cos - 1, _SHR_E_PARAM, "queue offset out of bound %d (num_cos = %d).\n",
                     queue_offset, num_cos);

    /*
     * Set
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_REQUEST_PROFILE_MAP, &entry_handle_id));

    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_ID, queue_id + queue_offset);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_request_profile_map_clear(
    int unit,
    int core,
    int queue_id,
    int queue_offset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Clear
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_REQUEST_PROFILE_MAP, &entry_handle_id));

    /** Select the core and the voq */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_ID, queue_id + queue_offset);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_request_profile_valid_verify(
    int unit,
    int profile_id)
{
    int valid;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_request_profile.valid.get(unit, profile_id, &valid));

    if (!valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "profile_id %d should be set first.\n", profile_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * DQCQ FC
 * {
 */
/**
 * \brief
 *   Initialize DQCQ FC
 *   This FC is sent from IPT to IPS (stopping IPS dqcq's), IPS also forwards it to QM (DQM and SQM) in order to stop
 *   proccessing dqcq's that were already sent from IPS.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_iqs_dqcq_fc_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_DQCQ_FLOW_CONTROL_MASK, &entry_handle_id));

    /** IPS to SQM - enable by setting all bits */
    {
        /** If set, flow control from S2D DQCQs will be sent to the SQM. Note: bit-0 HP S2D DQCQ, bit-1 LP S2D DQCQ */
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_SRAM_TO_DRAM, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);

        /** If set, flow control from SRAM Delete DQCQ will be sent to the SQM */
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_SRAM_DELETE, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);

        /** If set, flow control from OCB-Only DQCQs will be sent to the SQM (bit per context). */
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_OCB_ONLY, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);

        /** If set, flow control from SRAM DQCQs will be sent to the SQM (bit per context) */
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_SRAM, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);

        /** If set, flow control from MIX-S DQCQs will be sent to the SQM (bit per context) */
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_MIXED_SRAM, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);
    }

    /** IPS to DQM - enable by setting all bits (if feature is enabled) */
    if (dnx_data_iqs.deq_default.feature_get(unit, dnx_data_iqs_deq_default_dqcq_fc_to_dram) == TRUE)
    {
        /** If set, flow control from DRAM Delete DQCQ will be sent to the DQM */
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_DRAM_DELETE, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);

        /** If set, flow control from MIX-D DQCQs will be sent to the DQM (bit per context) */
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_MIXED_DRAM, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * Credit Watchdog
 * {
 */
/**
 * \brief
 *   Initialize Credit Watchdog
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_iqs_credit_watchdog_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** SW state alloc */
    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_watchdog.delete_th.alloc(unit));

    /** enable mantissa-exp mode for delete threshold configuration */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_WATCHDOG_COMMON, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_EXP_MODE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_watchdog_queue_min_sw_set(
    int unit,
    int min_queue_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_watchdog.queue_min.set(unit, min_queue_id));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_watchdog_queue_min_sw_get(
    int unit,
    int *min_queue_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_watchdog.queue_min.get(unit, min_queue_id));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_watchdog_queue_max_sw_set(
    int unit,
    int min_queue_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_watchdog.queue_max.set(unit, min_queue_id));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_watchdog_queue_max_sw_get(
    int unit,
    int *min_queue_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_watchdog.queue_max.get(unit, min_queue_id));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_watchdog_retransmit_sw_set(
    int unit,
    int retransmit_msec)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_watchdog.retransmit_th.set(unit, retransmit_msec));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_watchdog_retransmit_sw_get(
    int unit,
    int *retransmit_msec)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_watchdog.retransmit_th.get(unit, retransmit_msec));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_watchdog_delete_sw_set(
    int unit,
    int profile_id,
    int delete_msec)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_watchdog.delete_th.set(unit, profile_id, delete_msec));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_watchdog_delete_sw_get(
    int unit,
    int profile_id,
    int *delete_msec)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_watchdog.delete_th.get(unit, profile_id, delete_msec));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_watchdog_enable_sw_set(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_watchdog.enable.set(unit, enable));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_watchdog_enable_sw_get(
    int unit,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_watchdog.enable.get(unit, enable));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_watchdog_hw_set(
    int unit)
{
    int enable;
    int nof_queues;
    int first_queue, last_queue;
    int nof_credit_request_profiles;
    int profile_id;
    int retransmit_th;
    int delete_th_nof_cycles, delete_th;
    uint32 delete_th_exp, delete_th_mant;
    dnxcmn_time_t time;
    uint32 cycle_clocks, cycle_period;
    int queue_scan_clocks;
    dbal_table_field_info_t mant_field_info, exp_field_info;
    uint32 fsm_shaper;
    uint32 entry_handle_id;
    dbal_enum_value_field_iqs_watchdog_cycles_e nof_cycles_for_retransmit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_watchdog.enable.get(unit, &enable));

    if (enable)
    {
        /*
         * queue range
         */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_min_sw_get(unit, &first_queue));
        SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_max_sw_get(unit, &last_queue));
        nof_queues = last_queue - first_queue + 1;

        /*
         * scan cycle configuration
         */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_retransmit_sw_get(unit, &retransmit_th));
        /*
         * number of cycles for retransmit
         * assuming retransmit_th is either smaller than DNX_IQS_CREDIT_WATCHDOG_LONGEST_CYCLE_US
         * or multiplication of DNX_IQS_CREDIT_WATCHDOG_LONGEST_CYCLE_MS. In case of multiplication,
         * scan cycle is set to maximal value - DNX_IQS_CREDIT_WATCHDOG_LONGEST_CYCLE_MS, otherwise
         * set to the requested value
         */
        if (retransmit_th == 0)
        {
            nof_cycles_for_retransmit = DBAL_ENUM_FVAL_IQS_WATCHDOG_CYCLES_NONE;
            cycle_period = DNX_IQS_CREDIT_WATCHDOG_LONGEST_CYCLE_US;
        }
        else if (retransmit_th <= DNX_IQS_CREDIT_WATCHDOG_LONGEST_CYCLE_US)
        {
            nof_cycles_for_retransmit = DBAL_ENUM_FVAL_IQS_WATCHDOG_CYCLES_EVERY_CYCLE;
            cycle_period = retransmit_th; /** set to requested time */
        }
        else
        {
            nof_cycles_for_retransmit = DBAL_ENUM_FVAL_IQS_WATCHDOG_CYCLES_EVERY_SECOND_CYCLE;
            cycle_period = DNX_IQS_CREDIT_WATCHDOG_LONGEST_CYCLE_US; /** set to multiplication of maximal allowed time */

        }

        /** microsec to clocks */
        COMPILER_64_SET(time.time, 0, cycle_period);
        time.time_units = DNXCMN_TIME_UNIT_USEC;
        SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &cycle_clocks));

        queue_scan_clocks = cycle_clocks / nof_queues;

        /*
         * reuse same equation as in jericho1 -
         * role: not have scans longer than the declared scan time, and not have burstiness.
         */
        fsm_shaper = UTILEX_MAX(queue_scan_clocks / 2, queue_scan_clocks - 20);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_WATCHDOG_COMMON, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_QUEUE, INST_SINGLE, first_queue);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_QUEUE, INST_SINGLE, last_queue);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FSM_SHAPER, INST_SINGLE, fsm_shaper);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_CYCLE_PERIOD, INST_SINGLE,
                                     DNX_IQS_CREDIT_WATCHDOG_CYCLE_PERIOD_UNITS(cycle_clocks));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CYCLES_FOR_RETRANSMIT, INST_SINGLE,
                                     nof_cycles_for_retransmit);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);

        /*
         * Update delete threshold per profile
         */
        nof_credit_request_profiles = dnx_data_iqs.credit.nof_profiles_get(unit);
        for (profile_id = 0; profile_id < nof_credit_request_profiles; profile_id++)
        {
            /** get delete period */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_delete_sw_get(unit, profile_id, &delete_th));
            /** convert to watchdog cycles units */
            delete_th_nof_cycles = UTILEX_DIV_ROUND_UP(delete_th * 1000 /* mili to micro */ , cycle_period);    /* both
                                                                                                                 * in
                                                                                                                 * micro
                                                                                                                 * second 
                                                                                                                 */

            /** break to mant-exp*/
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                            (unit, DBAL_TABLE_IQS_CREDIT_WATCHDOG_PROFILE, DBAL_FIELD_DELETE_MANT, FALSE, 0, 0,
                             &mant_field_info));
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                            (unit, DBAL_TABLE_IQS_CREDIT_WATCHDOG_PROFILE, DBAL_FIELD_DELETE_EXP, FALSE, 0, 0,
                             &exp_field_info));
            SHR_IF_ERR_EXIT(utilex_break_to_mnt_exp_round_up
                            (delete_th_nof_cycles, mant_field_info.field_nof_bits, exp_field_info.field_nof_bits, 0,
                             &delete_th_mant, &delete_th_exp));

            /** write to dbal */
            if (delete_th_exp > DNX_IQS_DELETE_TH_EXP_MAX || delete_th_mant > DNX_IQS_DELETE_TH_MANT_MAX)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Setting credit watchdog delete threshold failed (resolution).");
            }
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_WATCHDOG_PROFILE, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_EXP, INST_SINGLE, delete_th_exp);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_MANT, INST_SINGLE, delete_th_mant);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_id);
        }
    }
    else
    {
        /** Disable watchdog */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_WATCHDOG_COMMON, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CYCLES_FOR_RETRANSMIT, INST_SINGLE,
                                     DBAL_ENUM_FVAL_IQS_WATCHDOG_CYCLES_NONE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_CYCLE_PERIOD, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_watchdog_verify(
    int unit)
{
    int enable;
    int first_queue, last_queue, nof_queues;
    int retransmit_th;
    int period;
    uint32 fsm_shaper_period_max, fsm_shaper_period;
    dnxcmn_time_t time;
    uint32 period_clocks;
    int queue_scan_clocks;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_iqs_db.credit_watchdog.enable.get(unit, &enable));

    if (enable)
    {
        /*
         * queue range
         */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_min_sw_get(unit, &first_queue));
        SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_max_sw_get(unit, &last_queue));
        SHR_RANGE_VERIFY(last_queue, first_queue, dnx_data_ipq.queues.nof_queues_get(unit) - 1, _SHR_E_PARAM,
                         "queue_max out of bound");

        /*
         * Make sure the ratio (period / nof_queues) is supported
         */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_retransmit_sw_get(unit, &retransmit_th));
        if (retransmit_th == 0)
        {
            period = DNX_IQS_CREDIT_WATCHDOG_LONGEST_CYCLE_US;

        }
        else
        {
            period = UTILEX_MIN(retransmit_th, DNX_IQS_CREDIT_WATCHDOG_LONGEST_CYCLE_US);
        }

        /**
         * The period and number of queues are later used to clculate the period in clocks in which a token bucket is incremented.
         * There is a limitation for the ratio of the period and number of queues. This limitation is due to the size of the HW field in which this value(in clocks) need so be written.
         */
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                        (unit, DBAL_TABLE_IQS_CREDIT_WATCHDOG_COMMON, DBAL_FIELD_FSM_SHAPER, FALSE, 0, 0,
                         DBAL_PREDEF_VAL_MAX_VALUE, &fsm_shaper_period_max));
        COMPILER_64_SET(time.time, 0, period);
        time.time_units = DNXCMN_TIME_UNIT_USEC;
        SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &period_clocks));
        nof_queues = last_queue - first_queue + 1;
        queue_scan_clocks = period_clocks / nof_queues;
        fsm_shaper_period = UTILEX_MAX(queue_scan_clocks / 2, queue_scan_clocks - 20);
        if (fsm_shaper_period > fsm_shaper_period_max)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Out of bound ratio on the (credit_request_watchdog_status_msg_gen / queue_range to scan)");
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
/*
 * Credit Worth
 * {
 */
/**
 * \brief
 *   Initialize Credit Worth
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_iqs_credit_worth_init(
    int unit)
{
    uint32 credit_worth;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Enable two groups of credit worth - by default all groups mapped to group 1
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_WORTH_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /*
     * Set local credit worth
     */
    credit_worth = dnx_data_iqs.credit.worth_get(unit);
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_set(unit, 0, credit_worth));

    /*
     * Set second group to the same value by default - will be override by the user
     */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_set(unit, 1, credit_worth));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_worth_group_set(
    int unit,
    int group_id,
    uint32 credit_worth_bytes)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_WORTH, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_WORTH_GROUP_ID, group_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_WORTH, INST_SINGLE, credit_worth_bytes);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_worth_group_get(
    int unit,
    int group_id,
    uint32 *credit_worth_bytes)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_WORTH, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_WORTH_GROUP_ID, group_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDIT_WORTH, INST_SINGLE, credit_worth_bytes);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_worth_map_set(
    int unit,
    uint32 fap_id,
    uint32 group_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_WORTH_GROUPS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, fap_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_WORTH_GROUP_ID, INST_SINGLE, group_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_credit_worth_map_get(
    int unit,
    uint32 fap_id,
    uint32 *group_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_WORTH_GROUPS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, fap_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDIT_WORTH_GROUP_ID, INST_SINGLE, group_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * }
 */
/*
 * FSM
 * {
 */
/**
 * \brief
 *   Initialize FSM
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_iqs_fsm_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Enable FSM selection per FAP and set fixed aging threshold
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_FSM_REORDER_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PER_FAP_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGING_TIMER, INST_SINGLE, 4);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_fsm_mode_set(
    int unit,
    uint32 fap_id,
    dnx_iqs_fsm_mode_e fsm_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_FSM_REORDER_FAP_MODE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, fap_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_SEQ, INST_SINGLE,
                                 (fsm_mode == dnx_iqs_fsm_mode_sequential ? 1 : 0));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_iqs_fsm_mode_get(
    int unit,
    uint32 fap_id,
    dnx_iqs_fsm_mode_e * fsm_mode)
{
    uint32 entry_handle_id;
    uint32 is_seq;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_FSM_REORDER_FAP_MODE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, fap_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_SEQ, INST_SINGLE, &is_seq);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *fsm_mode = is_seq ? dnx_iqs_fsm_mode_sequential : dnx_iqs_fsm_mode_delayed;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * }
 */
/*
 * See .h
 */
shr_error_e
dnx_iqs_debug_autocredit_rate_set(
    int unit,
    uint32 queue_min,
    uint32 queue_max,
    uint32 rate)
{
    uint32 entry_handle_id;
    uint32 credit_worth;
    uint32 clocks;
    uint32 rate_field;
    uint32 auto_credit_rate_max;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (rate)
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, DBAL_TABLE_IQS_DEBUG_AUTO_CREDIT,
                                                              DBAL_FIELD_RATE, 0, 0, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE, &auto_credit_rate_max));

        credit_worth = dnx_data_iqs.credit.worth_get(unit);
        SHR_IF_ERR_EXIT(dnx_algo_rates_kbits_per_sec_to_clocks(unit, rate * 1000, credit_worth, &clocks));
        rate_field = utilex_log2_round_down(clocks);
        rate_field = UTILEX_RANGE_BOUND(rate_field, 1, auto_credit_rate_max);
    }
    else
    {
        rate_field = 0;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_DEBUG_AUTO_CREDIT, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_MIN, INST_SINGLE, rate ? queue_min : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_MAX, INST_SINGLE, rate ? queue_max : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE, INST_SINGLE, rate_field);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_iqs_debug_autocredit_rate_get(
    int unit,
    uint32 *queue_min,
    uint32 *queue_max,
    uint32 *rate)
{
    uint32 entry_handle_id;
    uint32 rate_field;
    uint32 credit_worth;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_DEBUG_AUTO_CREDIT, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QUEUE_MIN, INST_SINGLE, queue_min);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QUEUE_MAX, INST_SINGLE, queue_max);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RATE, INST_SINGLE, &rate_field);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (rate_field)
    {
        credit_worth = dnx_data_iqs.credit.worth_get(unit);
        rate_field = utilex_power_of_2(rate_field);
        SHR_IF_ERR_EXIT(dnx_algo_rates_clocks_to_kbits_per_sec(unit, rate_field, credit_worth, rate));
        /** Kbps to Mbps */
        *rate /= 1000;
    }
    else
    {
        *rate = 0;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_iqs_defaults_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_GENERAL_CONFIGURATION, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISCARD_CGM_REPORTS, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISCARD_CREDITS, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_STATUS_MSG_GEN, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_DEQ_CMDS, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_MAX_LOCAL_RED_IDX, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEND_FSM_ON_MAX_LOCAL_RED_IDX, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_queue_to_priority_map_set(
    int unit,
    int core,
    uint32 voq,
    uint32 priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_QUEUE_TO_PRIORITY_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ, voq);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_queue_to_priority_map_get(
    int unit,
    int core,
    uint32 voq,
    uint32 *priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_QUEUE_TO_PRIORITY_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ, voq);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_min_low_priority_set(
    int unit,
    uint32 min_low_priority)
{
    uint32 entry_handle_id;
    uint32 prio, num_of_prio, is_high;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_PRIORITY_IS_HIGH, &entry_handle_id));

    num_of_prio = dnx_data_iqs.dqcq.nof_priorities_get(unit);

    is_high = 1;
    for (prio = 0; prio < min_low_priority; prio++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, prio);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HIGH, INST_SINGLE, is_high);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    is_high = 0;
    for (prio = min_low_priority; prio < num_of_prio; prio++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, prio);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HIGH, INST_SINGLE, is_high);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    if (!soc_is(unit, J2P_DEVICE))
    {
        
        SHR_IF_ERR_EXIT(dnx_fabric_transmit_dqcq_priority_update(unit, min_low_priority));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_min_low_priority_get(
    int unit,
    uint32 *min_low_prio)
{
    uint32 entry_handle_id;
    int prio, num_of_prio;
    uint32 is_high;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_PRIORITY_IS_HIGH, &entry_handle_id));

    num_of_prio = dnx_data_iqs.dqcq.nof_priorities_get(unit);

    for (prio = 0; prio < num_of_prio; prio++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, prio);

        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_HIGH, INST_SINGLE, &is_high);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (is_high == 0)
        {
            *min_low_prio = prio;
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_push_queue_set(
    int unit,
    uint32 profile_id,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_REQUEST_PUSH_QUEUE, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, profile_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_push_queue_get(
    int unit,
    uint32 *profile_id,
    uint32 *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_REQUEST_PUSH_QUEUE, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, profile_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_queue_flush_queue_id_set(
    int unit,
    int core,
    uint32 voq)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_QUEUE_FLUSH_ATTRIBUTES, &entry_handle_id));

    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ, INST_SINGLE, voq);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_queue_flush_queue_id_get(
    int unit,
    int core,
    uint32 *voq)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_QUEUE_FLUSH_ATTRIBUTES, &entry_handle_id));

    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DNXCMN_CORE_ID2INDEX(unit, core));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VOQ, INST_SINGLE, voq);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_queue_flush_attributes_set(
    int unit,
    int core,
    uint32 is_del_dqcq,
    uint32 bytes_units_to_send,
    uint32 flush_en)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_QUEUE_FLUSH_ATTRIBUTES, &entry_handle_id));

    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DEL_DQCQ, INST_SINGLE, is_del_dqcq);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYTE_UNITS_TO_SEND, INST_SINGLE,
                                 bytes_units_to_send);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH_TRIGGER, INST_SINGLE, flush_en);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_queue_flush_attributes_get(
    int unit,
    int core,
    uint32 *is_del_dqcq,
    uint32 *bytes_units_to_send,
    uint32 *flush_en)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_QUEUE_FLUSH_ATTRIBUTES, &entry_handle_id));

    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DNXCMN_CORE_ID2INDEX(unit, core));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_DEL_DQCQ, INST_SINGLE, is_del_dqcq);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BYTE_UNITS_TO_SEND, INST_SINGLE, bytes_units_to_send);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FLUSH_TRIGGER, INST_SINGLE, flush_en);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_iqs_queue_flush_trigger_down_polling(
    int unit,
    int core,
    sal_usecs_t timeout)
{
    uint32 entry_handle_id;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_QUEUE_FLUSH_ATTRIBUTES, &entry_handle_id));

    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        /** Select the core */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_idx);
        SHR_IF_ERR_EXIT(dnxcmn_polling(unit, timeout, 100, entry_handle_id, DBAL_FIELD_FLUSH_TRIGGER, 0));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_iqs_dram_delete_fix_hw(
    int unit)
{
    int fix_exists = dnx_data_iqs.credit.feature_get(unit, dnx_data_iqs_credit_dram_delete_fix_hw);
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (fix_exists == 1)
    {
        /*
         * write to dbal the fix exists.
         */

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_CREDIT_WATCHDOG_COMMON, &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_DRAM_DELETE_FIX_HW, INST_SINGLE, 1);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Module Init /Deinit
 * {
 */
/** See .h file */
shr_error_e
dnx_iqs_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Alloc SW state
     */
    SHR_IF_ERR_EXIT(dnx_iqs_db.init(unit));

    /*
     * Default values to HW
     */
    SHR_IF_ERR_EXIT(dnx_iqs_defaults_init(unit));

    /*
     * Credit request init
     */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_request_init(unit));

    /*
     * Credit watchdog init
     */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_init(unit));

    /*
     * Credit worth init
     */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_init(unit));

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        /*
         * FSM init
         */
        SHR_IF_ERR_EXIT(dnx_iqs_fsm_init(unit));
    }

    /*
     * FMQ (Fabric Multicast) init
     */
    SHR_IF_ERR_EXIT(dnx_fmq_init(unit));

    /*
     * DQCQ FC (IPS to DQM/SQM FC) init
     */
    SHR_IF_ERR_EXIT(dnx_iqs_dqcq_fc_init(unit));

    SHR_IF_ERR_EXIT(dnx_iqs_dram_delete_fix_hw(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

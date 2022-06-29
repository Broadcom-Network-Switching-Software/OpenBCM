/** \file iqs_dequeue_params.c
 * 
 *
 * Module IQS - Ingress Queue Scheduling
 *
 * Configurations of dequeue command parameters
 *
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ
/*
 * Include files
 */
#include <soc/sand/shrextend/shrextend_debug.h>
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

#include "iqs.h"
#include "iqs_dequeue_params.h"

/*
 * \ brief - init dequeue command parameters
 */
shr_error_e
dnx_iqs_dequeue_params_init(
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
    uint32 lfsr_default_thr, lfsr_default_mask;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

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
     * SRAM and DRAM DEQ parameters
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

    /** set SRAM to Fabric LFSR thresholds */
    /** same configurations should apply even if the system is loaded without DRAM */
    for (bw_level = 0; bw_level < dnx_data_iqs.credit.nof_bw_levels_get(unit); bw_level++)
    {
        lfsr_default_thr = dnx_data_iqs.deq_default.sram_to_fabric_credit_lfsr_thr_get(unit);
        lfsr_default_mask = dnx_data_iqs.deq_default.sram_to_fabric_credit_lfsr_mask_get(unit);
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IQS_SRAM_TO_FABRIC_CRDT_LFSR_THR, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BW_LEVEL, bw_level);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LFSR_THR, INST_SINGLE, lfsr_default_thr);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LFSR_MASK, INST_SINGLE, lfsr_default_mask);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

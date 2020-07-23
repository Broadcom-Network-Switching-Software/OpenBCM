/*
 * system_red_dbal.c
 *
 *  Created on: Sep 12, 2018
 *      Author: si888124
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_error.h>

#include <bcm/cosq.h>

#include <bcm_int/dnx/cosq/cosq.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>

#include "system_red.h"
#include "system_red_dbal.h"

/*
 * System RED resources DBAL tables array - for unified HW access function
 */
static const dbal_tables_e dnx_system_red_free_res_thr_table_id[] = {
    /** PBBs */
    DBAL_TABLE_SYSTEM_RED_FREE_RES_THR_PDBS,
    /** SRAM buffers */
    DBAL_TABLE_SYSTEM_RED_FREE_RES_THR_SRAM_BUFFERS,
    /** DRAM BDBs */
    DBAL_TABLE_SYSTEM_RED_FREE_RES_THR_DRAM_BDBS
};

/**
 * \brief - Set system RED free resource threshold to HW
 *
 * \param [in] unit - The unit number.
 * \param [in] table_id - DBAL table id. see dnx_system_red_free_res_thr_table_id
 * \param [in] thr_index - (0-2) there are three thresholds for each resource, to define 4 ranges of free resources. each range is mapped to a RED-Q-Size value.
 * \param [in] thr_value - threshold value in HW granularity.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_system_red_dbal_free_res_thr_hw_set(
    int unit,
    dbal_tables_e table_id,
    uint32 thr_index,
    uint32 thr_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_THR_INDEX, thr_index);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THR_VALUE, INST_SINGLE, thr_value);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get system RED free resource threshold from HW.
 * see dnx_system_red_dbal_free_res_thr_hw_set
 */
static shr_error_e
dnx_system_red_dbal_free_res_thr_hw_get(
    int unit,
    dbal_tables_e table_id,
    uint32 thr_index,
    uint32 *thr_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_THR_INDEX, thr_index);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_THR_VALUE, INST_SINGLE, thr_value);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_free_res_thr_set(
    int unit,
    dnx_system_red_resource_type_e resource,
    uint32 thr_index,
    int thr_value)
{
    dbal_tables_e table_id;
    uint32 granularity, hw_threshold;
    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_system_red_free_res_thr_table_id[resource];
    granularity = dnx_data_ingr_congestion.info.threshold_granularity_get(unit);

    hw_threshold = thr_value / granularity;

    SHR_IF_ERR_EXIT(dnx_system_red_dbal_free_res_thr_hw_set(unit, table_id, thr_index, hw_threshold));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_free_res_thr_get(
    int unit,
    dnx_system_red_resource_type_e resource,
    uint32 thr_index,
    int *thr_value)
{
    dbal_tables_e table_id;
    uint32 granularity, hw_threshold;
    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_system_red_free_res_thr_table_id[resource];
    granularity = dnx_data_ingr_congestion.info.threshold_granularity_get(unit);

    SHR_IF_ERR_EXIT(dnx_system_red_dbal_free_res_thr_hw_get(unit, table_id, thr_index, &hw_threshold));

    *thr_value = hw_threshold * granularity;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -translate system RED resource to DBAL type system RED resource.
 */
static shr_error_e
dnx_system_red_resource_to_dbal_resource(
    int unit,
    dnx_system_red_resource_type_e resource,
    dbal_enum_value_field_system_red_resource_type_e * dbal_sys_red_resource)
{
    SHR_FUNC_INIT_VARS(unit);

    *dbal_sys_red_resource = DBAL_NOF_ENUM_SYSTEM_RED_RESOURCE_TYPE_VALUES;

    switch (resource)
    {
        case DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS:
            *dbal_sys_red_resource = DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_SRAM_PDBS;
            break;
        case DNX_SYSTEM_RED_RESOURCE_SRAM_BUFFERS:
            *dbal_sys_red_resource = DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_SRAM_BUFFERS;
            break;
        case DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS:
            *dbal_sys_red_resource = DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_DRAM_BDBS;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid resource %d\n", resource);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_free_res_range_to_red_q_size_map_set(
    int unit,
    dnx_system_red_resource_type_e resource,
    uint32 range_index,
    uint32 red_q_size_index)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_system_red_resource_type_e dbal_resource;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_FREE_RES_Q_SIZE_INDEX, &entry_handle_id));

    /** Convert resource to DBAL enum resource */
    SHR_IF_ERR_EXIT(dnx_system_red_resource_to_dbal_resource(unit, resource, &dbal_resource));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RES_TYPE, dbal_resource);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RANGE_INDEX, range_index);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RED_Q_SIZE_INDEX, INST_SINGLE, red_q_size_index);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_free_res_range_to_red_q_size_map_get(
    int unit,
    dnx_system_red_resource_type_e resource,
    uint32 range_index,
    uint32 *red_q_size_index)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_system_red_resource_type_e dbal_resource;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_FREE_RES_Q_SIZE_INDEX, &entry_handle_id));

    /** Convert resource to DBAL enum resource */
    SHR_IF_ERR_EXIT(dnx_system_red_resource_to_dbal_resource(unit, resource, &dbal_resource));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RES_TYPE, dbal_resource);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RANGE_INDEX, range_index);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RED_Q_SIZE_INDEX, INST_SINGLE, red_q_size_index);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_voq_size_range_to_red_q_size_map_set(
    int unit,
    uint32 rate_class,
    uint32 red_q_size_index,
    uint32 voq_range_max_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_Q_SIZE_TO_VOQ_SIZE_RANGE_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RED_Q_SIZE_INDEX, red_q_size_index);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_SIZE_RANGE_MAX, INST_SINGLE, voq_range_max_size);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_voq_size_range_to_red_q_size_map_get(
    int unit,
    uint32 rate_class,
    uint32 red_q_size_index,
    uint32 *voq_range_max_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_Q_SIZE_TO_VOQ_SIZE_RANGE_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RED_Q_SIZE_INDEX, red_q_size_index);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VOQ_SIZE_RANGE_MAX, INST_SINGLE, voq_range_max_size);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_discard_prob_low_set(
    int unit,
    uint32 rate_class,
    uint32 dp,
    dnx_system_red_dbal_discard_params_t * discard_params)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_Q_SIZE_DISCARD_PARAMS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, discard_params->enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_THR, INST_SINGLE, discard_params->admit_thr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROB_THR, INST_SINGLE, discard_params->prob_thr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_PROB_LOW, INST_SINGLE,
                                 discard_params->drop_prob_low);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_discard_prob_high_set(
    int unit,
    uint32 rate_class,
    uint32 dp,
    dnx_system_red_dbal_discard_params_t * discard_params)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_Q_SIZE_DISCARD_PARAMS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, discard_params->enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROB_THR, INST_SINGLE, discard_params->prob_thr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REJECT_THR, INST_SINGLE, discard_params->reject_thr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_PROB_HIGH, INST_SINGLE,
                                 discard_params->drop_prob_high);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_discard_params_get(
    int unit,
    uint32 rate_class,
    uint32 dp,
    dnx_system_red_dbal_discard_params_t * discard_params)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_Q_SIZE_DISCARD_PARAMS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &discard_params->enable);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ADMIT_THR, INST_SINGLE, &discard_params->admit_thr);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROB_THR, INST_SINGLE, &discard_params->prob_thr);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_REJECT_THR, INST_SINGLE, &discard_params->reject_thr);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DROP_PROB_LOW, INST_SINGLE,
                               &discard_params->drop_prob_low);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DROP_PROB_HIGH, INST_SINGLE,
                               &discard_params->drop_prob_high);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_sch_aging_params_set(
    int unit,
    dnx_system_red_dbal_aging_t * aging_params)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_SCH_CONFIG, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, aging_params->enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGING_PERIOD, INST_SINGLE,
                                 aging_params->sch_aging_period);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGING_MODE, INST_SINGLE, aging_params->aging_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ONLY_AGING_CAN_DECREMENT, INST_SINGLE,
                                 aging_params->is_only_aging_dec);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_sch_aging_params_get(
    int unit,
    dnx_system_red_dbal_aging_t * aging_params)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_SCH_CONFIG, &entry_handle_id));

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &aging_params->enable);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_AGING_PERIOD, INST_SINGLE,
                               &aging_params->sch_aging_period);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_AGING_MODE, INST_SINGLE, &aging_params->aging_mode);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ONLY_AGING_CAN_DECREMENT, INST_SINGLE,
                               &aging_params->is_only_aging_dec);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_ing_params_set(
    int unit,
    uint32 aging_period)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_ING_CONFIG, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGING_PERIOD, INST_SINGLE, aging_period);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_ing_params_get(
    int unit,
    uint32 *aging_period)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_ING_CONFIG, &entry_handle_id));

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_AGING_PERIOD, INST_SINGLE, aging_period);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_enable_set(
    int unit,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_ING_CONFIG, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_RED_ENABLE, INST_SINGLE, enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_enable_get(
    int unit,
    uint32 *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_ING_CONFIG, &entry_handle_id));

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYS_RED_ENABLE, INST_SINGLE, enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_global_red_status_get(
    int unit,
    int core,
    uint32 *current_red_index,
    uint32 *max_red_index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_GLOBAL_STATUS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CURRENT_RED_INDEX, INST_SINGLE, current_red_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_RED_INDEX, INST_SINGLE, max_red_index);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_system_red_dbal_sysport_red_status_get(
    int unit,
    int core,
    uint32 system_port,
    uint32 *red_index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_RED_MAX_RED_Q_SIZE_PER_SYSPORT, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT, system_port);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_RED_INDEX, INST_SINGLE, red_index);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * phantom_queues_dbal.c
 *
 *  Created on: Dec 11, 2018
 *      Author: si888124
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
#include <soc/dnx/dbal/dbal.h>

/*
 * see .h file
 */
shr_error_e
dnx_phantom_queues_dbal_egq_if_enable_set(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 enable,
    uint32 threshold_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_PHANTOM_QUEUES_IF_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egq_if);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_PHANTOM_QUEUE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_PROFILE, INST_SINGLE, threshold_profile);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_phantom_queues_dbal_egq_if_enable_get(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 *enable,
    uint32 *threshold_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_PHANTOM_QUEUES_IF_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egq_if);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE_PHANTOM_QUEUE, INST_SINGLE, enable);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_PROFILE, INST_SINGLE, threshold_profile);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_phantom_queues_dbal_threshold_set(
    uint32 unit,
    bcm_core_t core_id,
    uint32 threshold_profile,
    uint32 threshold_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_PHANTOM_QUEUES_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_PROFILE, threshold_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_VALUE, INST_SINGLE, threshold_value);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_phantom_queues_dbal_threshold_get(
    uint32 unit,
    bcm_core_t core_id,
    uint32 threshold_profile,
    uint32 *threshold_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_PHANTOM_QUEUES_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_PROFILE, threshold_profile);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_VALUE, INST_SINGLE, threshold_value);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_phantom_queues_dbal_global_enable_set(
    uint32 unit,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_PHANTOM_QUEUES_GLOBAL_CONFIGURATION, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_phantom_queues_dbal_global_enable_get(
    uint32 unit,
    uint32 *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_PHANTOM_QUEUES_GLOBAL_CONFIGURATION, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * rate_measurement_dbal.c
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
dnx_rate_measurement_dbal_interval_set(
    uint32 unit,
    uint32 interval)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_RATE_MEASUREMENT_GLOBAL, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERVAL, INST_SINGLE, interval);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_dbal_interval_get(
    uint32 unit,
    uint32 *interval)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_RATE_MEASUREMENT_GLOBAL, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INTERVAL, INST_SINGLE, interval);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_dbal_rate_weight_set(
    uint32 unit,
    dbal_enum_value_field_egress_rate_weight_e rate_weight)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_RATE_MEASUREMENT_GLOBAL, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_WEIGHT, INST_SINGLE, rate_weight);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_dbal_rate_weight_get(
    uint32 unit,
    dbal_enum_value_field_egress_rate_weight_e * rate_weight)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_RATE_MEASUREMENT_GLOBAL, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RATE_WEIGHT, INST_SINGLE, rate_weight);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_dbal_compensation_set(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 compensation_abs,
    uint32 compensation_sign)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_RATE_MEASUREMENT_IF_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egq_if);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMPENSATION_ABS, INST_SINGLE, compensation_abs);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMPENSATION_SIGN, INST_SINGLE, compensation_sign);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_dbal_compensation_get(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 *compensation_abs,
    uint32 *compensation_sign)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_RATE_MEASUREMENT_IF_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egq_if);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COMPENSATION_ABS, INST_SINGLE, compensation_abs);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COMPENSATION_SIGN, INST_SINGLE, compensation_sign);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_dbal_rate_promile_set(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 rate_promile,
    uint32 granularity)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_RATE_MEASUREMENT_IF_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egq_if);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_PROMILE, INST_SINGLE, rate_promile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GRANULARITY, INST_SINGLE, granularity);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_dbal_rate_promile_get(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 *rate_promile,
    uint32 *granularity)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_RATE_MEASUREMENT_IF_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egq_if);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RATE_PROMILE, INST_SINGLE, rate_promile);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GRANULARITY, INST_SINGLE, granularity);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_dbal_rate_counter_config_set(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_RATE_MEASUREMENT_COUNTER, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, INST_SINGLE, egq_if);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_dbal_rate_counter_get(
    uint32 unit,
    bcm_core_t core_id,
    uint32 *rate)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_RATE_MEASUREMENT_COUNTER, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROMILE_RATE, INST_SINGLE, rate);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

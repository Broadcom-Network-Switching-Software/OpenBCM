/*
 * 
 * fmq_dbal.c
 *
 *  Created on: Nov 1, 2018
 *      Author: si888124
 *
 *  DBAL access functions for FMQ (Ingress Queue Scheduling module).
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

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <sal/types.h>
#include "fmq_dbal.h"
/*
 * }
 */

shr_error_e
dnx_cosq_fmq_dbal_credit_weight_mode_set(
    int unit,
    uint32 is_wfq)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FMQ_CREDIT_WEIGHT_CONFIG, &entry_handle_id));

    /** Set Credit distribution mode (WFQ / SP) */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WFQ_EN, INST_SINGLE, is_wfq);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_fmq_dbal_credit_weight_mode_get(
    int unit,
    uint32 *is_wfq)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FMQ_CREDIT_WEIGHT_CONFIG, &entry_handle_id));

    /** Set Credit distribution mode (WFQ / SP) */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_WFQ_EN, INST_SINGLE, is_wfq);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_fmq_dbal_credit_weight_set(
    int unit,
    dbal_fields_e fmq_class_field_id,
    uint32 weight)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FMQ_CREDIT_WEIGHT_CONFIG, &entry_handle_id));

    /** Set the weight for the specified class */
    dbal_entry_value_field32_set(unit, entry_handle_id, fmq_class_field_id, INST_SINGLE, weight);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_fmq_dbal_credit_weight_get(
    int unit,
    dbal_fields_e fmq_class_field_id,
    uint32 *weight)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FMQ_CREDIT_WEIGHT_CONFIG, &entry_handle_id));

    /** Get the weight for the specified class */
    dbal_value_field32_request(unit, entry_handle_id, fmq_class_field_id, INST_SINGLE, weight);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_fmq_dbal_shaper_credit_rate_set(
    int unit,
    dbal_enum_value_field_fmq_shaper_type_e shaper_type,
    uint32 credit_rate)
{
    uint32 entry_handle_id;
    uint32 maximal_credit;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get maximal value of the credit rate field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, DBAL_TABLE_FMQ_SHAPER_CONFIG, DBAL_FIELD_CREDIT_RATE,
                                                          0, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &maximal_credit));

    if (credit_rate > maximal_credit)
    {
        credit_rate = maximal_credit;
    }

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FMQ_SHAPER_CONFIG, &entry_handle_id));

    /** Set shaper type */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMQ_SHAPER_TYPE, shaper_type);

    /** Set the weight for the specified class */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_RATE, INST_SINGLE, credit_rate);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_fmq_dbal_shaper_credit_rate_get(
    int unit,
    dbal_enum_value_field_fmq_shaper_type_e shaper_type,
    uint32 *credit_rate)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FMQ_SHAPER_CONFIG, &entry_handle_id));

    /** Set shaper type */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMQ_SHAPER_TYPE, shaper_type);

    /** Set the weight for the specified class */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDIT_RATE, INST_SINGLE, credit_rate);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_fmq_dbal_shaper_max_burst_set(
    int unit,
    dbal_enum_value_field_fmq_shaper_type_e shaper_type,
    uint32 max_burst)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FMQ_SHAPER_CONFIG, &entry_handle_id));

    /** Set shaper type */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMQ_SHAPER_TYPE, shaper_type);

    /** Set the weight for the specified class */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_BURST, INST_SINGLE, max_burst);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_fmq_dbal_shaper_max_burst_get(
    int unit,
    dbal_enum_value_field_fmq_shaper_type_e shaper_type,
    uint32 *max_burst)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FMQ_SHAPER_CONFIG, &entry_handle_id));

    /** Set shaper type */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMQ_SHAPER_TYPE, shaper_type);

    /** Set the weight for the specified class */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_BURST, INST_SINGLE, max_burst);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_fmq_dbal_enhanced_mode_set(
    int unit,
    uint32 is_enhanced_mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FMQ_GENERAL_CONFIG, &entry_handle_id));

    /** Set enhance mode */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ENHANCED_MODE, INST_SINGLE, is_enhanced_mode);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_fmq_dbal_enhanced_mode_get(
    int unit,
    uint32 *is_enhanced_mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FMQ_GENERAL_CONFIG, &entry_handle_id));

    /** Set enhance mode */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_ENHANCED_MODE, INST_SINGLE, is_enhanced_mode);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_fmq_dbal_enhanced_mode_hr_id_set(
    int unit,
    int core,
    uint32 mc_port_id,
    uint32 hr_id,
    uint32 is_hr_valid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FMQ_ENHANCED_MODE_CONFIG, &entry_handle_id));

    /** Set Core*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Set MC port index (guaranteed/BE class0/1/2) */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMQ_ENHANCED_PORT_IDX, mc_port_id);

    /** Set MC id for the port */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, INST_SINGLE, hr_id);

    /** Set MC is valid */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HR_ID_VALID, INST_SINGLE, is_hr_valid);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_fmq_dbal_enhanced_mode_hr_id_get(
    int unit,
    int core,
    uint32 mc_port_id,
    uint32 *hr_id,
    uint32 *is_hr_valid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FMQ_ENHANCED_MODE_CONFIG, &entry_handle_id));

    /** Set Core*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Set MC port index (guaranteed/BE class0/1/2) */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMQ_ENHANCED_PORT_IDX, mc_port_id);

    /** Get MC id for the port */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HR_ID, INST_SINGLE, hr_id);

    /** Get MC is valid */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_HR_ID_VALID, INST_SINGLE, is_hr_valid);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

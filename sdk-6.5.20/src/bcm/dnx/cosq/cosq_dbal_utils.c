/** \file cosq_dbal_utils.c
 *
 * Generic DBAL access functionality
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

#include <bcm_int/dnx/cosq/cosq_dbal_utils.h>

/**
 * \brief - set a value to dbal table entry
 */
shr_error_e
dnx_cosq_dbal_entry_set(
    int unit,
    dbal_tables_e table_id,
    int nof_keys,
    dnx_cosq_dbal_field_t key[],
    dnx_cosq_dbal_field_t * result)
{
    uint32 entry_handle_id;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    for (i = 0; i < nof_keys; i++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, key[i].id, key[i].value);
    }

    /** Setting value field */
    dbal_entry_value_field32_set(unit, entry_handle_id, result->id, INST_SINGLE, result->value);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get a value from dbal table entry
 */
shr_error_e
dnx_cosq_dbal_entry_get(
    int unit,
    dbal_tables_e table_id,
    int nof_keys,
    dnx_cosq_dbal_field_t key[],
    dnx_cosq_dbal_field_t * result)
{
    uint32 entry_handle_id;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    for (i = 0; i < nof_keys; i++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, key[i].id, key[i].value);
    }

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, result->id, INST_SINGLE, &result->value);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - return TRUE iff a key exists in the table
 */
shr_error_e
dnx_cosq_dbal_key_exists_in_table(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e key_field_id,
    int *exists)
{
    dbal_table_field_info_t field_info;
    int result;

    SHR_FUNC_INIT_VARS(unit);

    result = dbal_tables_field_info_get_no_err(unit, table_id, key_field_id, TRUE, 0, INST_SINGLE, &field_info);

    *exists = (result == _SHR_E_NONE);

    SHR_FUNC_EXIT;
}

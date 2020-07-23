/** \file field_utils.c
 * 
 *
 * Field Processor utilities - may be in use by any field_* module
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field_map.h>
/*
 * }Include files
 */

shr_error_e
dnx_field_dbal_entry_set(
    int unit,
    dnx_field_dbal_entry_t * field_dbal_entry)
{
    uint32 entry_handle_id;
    int i_field;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, field_dbal_entry->table_id, &entry_handle_id));

    for (i_field = 0; i_field < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_DBAL_PAIRS; i_field++)
    {
        /*
         * We may change paradigm in future - not to break, but to go through all fields and check whether there were
         * no empty ones in the list
         */
        if (field_dbal_entry->key_dbal_pair[i_field].field_id == DBAL_FIELD_EMPTY)
            break;
        dbal_entry_key_field32_set(unit, entry_handle_id, field_dbal_entry->key_dbal_pair[i_field].field_id,
                                   field_dbal_entry->key_dbal_pair[i_field].value);
    }

    for (i_field = 0; i_field < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_DBAL_PAIRS; i_field++)
    {
        /*
         * We may change paradigm in future - not to break, but to go through all fields and check whether there were
         * no empty ones in the list
         */
        if (field_dbal_entry->res_dbal_pair[i_field].field_id == DBAL_FIELD_EMPTY)
            break;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_dbal_entry->res_dbal_pair[i_field].field_id,
                                     INST_SINGLE, field_dbal_entry->res_dbal_pair[i_field].value);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

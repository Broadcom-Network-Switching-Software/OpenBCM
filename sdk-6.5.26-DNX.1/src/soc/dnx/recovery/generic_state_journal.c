/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * INFO: This module is a generic side effect / state rollback journal,
 * saving the old values of global state or side effects that were overridden
 * by access calls since the beginning of the last transaction.
 * This journal should be used only in places where the existing DBAL or SWSTATE journal fail short.
 */

#include <shared/bsl.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/core/sync.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
#include <soc/dnx/recovery/generic_state_journal.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_dynamic.h>
#endif

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

#define DNX_GENERIC_ROLLBACK_JOURNAL_NUM 0
#define DNX_GENERIC_COMPARISON_JOURNAL_NUM 1

int
dnx_is_generic_state_journal_is_on(
    int unit,
    uint8 is_comparison)
{
    dnx_er_db_t *dnx_er_db = NULL;

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        return FALSE;
    }

    if (is_comparison)
    {
        return dnx_rollback_journal_is_on(unit, dnx_er_db, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_COMPARISON);
    }
    else
    {
        return dnx_rollback_journal_is_on(unit, dnx_er_db, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_ROLLBACK);
    }
}

static inline shr_error_e
dnx_generic_state_journal_get_if_on(
    int unit,
    dnx_er_db_t ** dnx_er_db,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, dnx_er_db) != _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INTERNAL);
    }

    if (is_comparison)
    {
        if (!dnx_rollback_journal_is_on(unit, *dnx_er_db, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_COMPARISON))
        {
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INTERNAL);
        }
    }
    else
    {
        if (!dnx_rollback_journal_is_on(unit, *dnx_er_db, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_ROLLBACK))
        {
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INTERNAL);
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_generic_state_journal_rollback_memcpy_entry(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnx_generic_state_journal_entry_t *entry = (dnx_generic_state_journal_entry_t *) (metadata);

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_err_recovery_is_validation_enabled(unit))
    {
        if ((NULL == entry->ptr) || (NULL == payload))
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                        "state journal ERROR: invalid entry data detected.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
    }

    sal_memcpy(entry->ptr, payload, entry->size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_generic_state_journal_entry_new(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint32 metadata_size,
    uint8 *payload,
    uint32 payload_size,
    dnx_generic_state_journal_rollback_handler rollback_handler,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_new(unit,
                                                       dnx_er_db,
                                                       DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_COMPARISON,
                                                       metadata,
                                                       metadata_size,
                                                       payload,
                                                       payload_size,
                                                       DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID,
                                                       rollback_handler, NULL, NULL));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_new(unit,
                                                       dnx_er_db,
                                                       DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_ROLLBACK,
                                                       metadata,
                                                       metadata_size,
                                                       payload,
                                                       payload_size,
                                                       DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID,
                                                       rollback_handler, NULL, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX_SUPPORT

static shr_error_e
dnx_generic_state_journal_rollback_dbal_table_info_change(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnx_generic_state_journal_dbal_table_entry_t *entry = (dnx_generic_state_journal_dbal_table_entry_t *) (metadata);

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_err_recovery_is_validation_enabled(unit))
    {
        if (NULL == metadata)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                        "state journal ERROR: invalid entry data detected.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_restore(unit, entry->table_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_generic_state_journal_log_dbal_table_info_change(
    int unit,
    dbal_tables_e table_id)
{
    dnx_er_db_t *dnx_er_db = NULL;
    dnx_generic_state_journal_dbal_table_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_generic_state_journal_get_if_on(unit, &dnx_er_db, FALSE) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    if (table_id < DBAL_NOF_TABLES)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                    "state journal ERROR: Attempted to log change to non dynamic DBAL table %d.\n%s%s",
                                                    table_id, EMPTY, EMPTY);
    }

    entry.table_id = table_id;

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_entry_new(unit,
                                                        dnx_er_db,
                                                        (uint8 *) (&entry),
                                                        sizeof(dnx_generic_state_journal_dbal_table_entry_t),
                                                        (uint8 *) (&entry),
                                                        sizeof(dnx_generic_state_journal_dbal_table_entry_t),
                                                        &dnx_generic_state_journal_rollback_dbal_table_info_change,
                                                        FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_generic_state_journal_rollback_result_type_add(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnx_generic_state_journal_dbal_table_entry_t *entry = (dnx_generic_state_journal_dbal_table_entry_t *) (metadata);

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_err_recovery_is_validation_enabled(unit))
    {
        if (NULL == metadata)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "state journal ERROR: invalid entry data detected.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }
    }

    SHR_IF_ERR_EXIT(dbal_tables_dynamic_result_type_delete(unit, entry->table_id, entry->res_type_idx));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_generic_state_journal_log_result_type_add(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx)
{
    dnx_er_db_t *dnx_er_db = NULL;
    dnx_generic_state_journal_dbal_table_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_generic_state_journal_get_if_on(unit, &dnx_er_db, FALSE) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    entry.table_id = table_id;
    entry.res_type_idx = res_type_idx;

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_entry_new(unit,
                                                        dnx_er_db,
                                                        (uint8 *) (&entry),
                                                        sizeof(dnx_generic_state_journal_dbal_table_entry_t),
                                                        (uint8 *) (&entry),
                                                        sizeof(dnx_generic_state_journal_dbal_table_entry_t),
                                                        &dnx_generic_state_journal_rollback_result_type_add, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_generic_state_journal_rollback_result_type_delete(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnx_generic_state_journal_dbal_table_entry_t *entry = (dnx_generic_state_journal_dbal_table_entry_t *) (metadata);

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_err_recovery_is_validation_enabled(unit))
    {
        if (NULL == metadata)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "state journal ERROR: invalid entry data detected.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }
    }

    SHR_IF_ERR_EXIT(dbal_tables_result_type_restore_rollback(unit, entry->table_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_generic_state_journal_log_result_type_delete(
    int unit,
    dbal_tables_e table_id)
{
    dnx_er_db_t *dnx_er_db = NULL;
    dnx_generic_state_journal_dbal_table_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_generic_state_journal_get_if_on(unit, &dnx_er_db, FALSE) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    entry.table_id = table_id;

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_entry_new(unit,
                                                        dnx_er_db,
                                                        (uint8 *) (&entry),
                                                        sizeof(dnx_generic_state_journal_dbal_table_entry_t),
                                                        (uint8 *) (&entry),
                                                        sizeof(dnx_generic_state_journal_dbal_table_entry_t),
                                                        &dnx_generic_state_journal_rollback_result_type_delete, FALSE));

exit:
    SHR_FUNC_EXIT;
}

#endif

shr_error_e
dnx_generic_state_journal_log_memcpy(
    int unit,
    uint32 size,
    uint8 *ptr)
{
    dnx_er_db_t *dnx_er_db = NULL;
    dnx_generic_state_journal_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_generic_state_journal_get_if_on(unit, &dnx_er_db, FALSE) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    if (dnx_err_recovery_is_validation_enabled(unit))
    {
        if (NULL == ptr)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                        "state journal ERROR: NULL data attempted to be inserted to journal.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        if (0 == size)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                        "state journal ERROR: data with size zero attempted to be inserted to journal.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
    }

    entry.size = size;
    entry.ptr = ptr;

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_entry_new(unit,
                                                        dnx_er_db,
                                                        (uint8 *) (&entry),
                                                        sizeof(dnx_generic_state_journal_entry_t),
                                                        ptr, size, &dnx_generic_state_journal_rollback_memcpy_entry,
                                                        FALSE));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_generic_state_journal_log_entry(
    int unit,
    uint8 *metadata,
    uint32 metadata_size,
    uint8 *payload,
    uint32 payload_size,
    dnx_generic_state_journal_rollback_handler rollback_handler,
    uint8 is_comparison)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_generic_state_journal_get_if_on(unit, &dnx_er_db, is_comparison) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_entry_new(unit,
                                                        dnx_er_db,
                                                        metadata,
                                                        metadata_size,
                                                        payload, payload_size, rollback_handler, is_comparison));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_generic_state_journal_clear(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_clear(unit, dnx_er_db, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_COMPARISON));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_clear(unit, dnx_er_db, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_ROLLBACK));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_generic_state_journal_initialize(
    int unit,
    dnx_er_db_t * dnx_er_db,
    dnx_rollback_journal_cbs_t rollback_journal_cbs,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_err_recovery_is_validation_enabled(unit))
    {
        if (NULL == rollback_journal_cbs.is_on)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                        "state journal ERROR: journal is on callback must be set.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
    }

    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_new
                        (unit, dnx_er_db, TRUE, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                         DNX_ROLLBACK_JOURNAL_SUBTYPE_GENERIC, rollback_journal_cbs,
                         DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_COMPARISON));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_new
                        (unit, dnx_er_db, FALSE, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                         DNX_ROLLBACK_JOURNAL_SUBTYPE_GENERIC, rollback_journal_cbs,
                         DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_ROLLBACK));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_generic_state_journal_destroy(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_destroy
                        (unit, dnx_er_db, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_COMPARISON));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_destroy(unit, dnx_er_db, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_ROLLBACK));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_generic_state_journal_get_head_seq_id(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_comparison,
    uint32 *seq_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_get_head_seq_id
                        (unit, dnx_er_db, seq_id, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_COMPARISON));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_get_head_seq_id
                        (unit, dnx_er_db, seq_id, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_ROLLBACK));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_generic_state_journal_all_tmp_suppress_unsafe(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_disable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_disabled_counter_change
                    (unit, dnx_er_db, is_disable, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_ROLLBACK, TRUE));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_disabled_counter_change
                    (unit, dnx_er_db, is_disable, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_COMPARISON, TRUE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_generic_state_journal_roll_back(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 only_head_rollback,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_rollback
                        (unit, dnx_er_db, only_head_rollback, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_COMPARISON));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_rollback
                        (unit, dnx_er_db, only_head_rollback, DNX_ER_DB_JOURNAL_HANDLE_GENERIC_STATE_ROLLBACK));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME


/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/error.h>
#include <sal/core/alloc.h>
#include <soc/dnx/dnx_er_db.h>

#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnx/recovery/rollback_journal_diag.h>
#include <soc/dnx/recovery/generic_state_journal.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnx/swstate/dnx_sw_state_journal.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

sal_tls_key_t *dnx_er_db_key[SOC_MAX_NUM_DEVICES] = { 0 };

static shr_error_e
dnx_er_db_init(
    int unit,
    dnx_er_db_t ** dnx_er_db)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_IGNORE_COUNTERS_ERR_EXIT(*dnx_er_db, sizeof(dnx_er_db_t), "dnx_er_db", "%s%s%s\n", EMPTY, EMPTY,
                                                EMPTY);

    (*dnx_er_db)->unit = unit;

    SHR_ALLOC_SET_ZERO_IGNORE_COUNTERS_ERR_EXIT((*dnx_er_db)->dnx_err_recovery_manager, sizeof(err_recovery_manager_t),
                                                "dnx_err_recovery_manager", "%s%s%s\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO_IGNORE_COUNTERS_ERR_EXIT((*dnx_er_db)->dnx_rollback_journal_pool,
                                                sizeof(dnx_rollback_journal_pool_t), "dnx_rollback_journal_pool",
                                                "%s%s%s\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO_IGNORE_COUNTERS_ERR_EXIT((*dnx_er_db)->dnx_dbal_journal_manager,
                                                sizeof(dnx_dbal_journal_manager_t), "dnx_dbal_journal_manager",
                                                "%s%s%s\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO_IGNORE_COUNTERS_ERR_EXIT((*dnx_er_db)->dnx_sw_state_manager,
                                                sizeof(dnx_sw_state_journal_manager_t), "dnx_sw_state_manager",
                                                "%s%s%s\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO_IGNORE_COUNTERS_ERR_EXIT((*dnx_er_db)->dnx_state_snapshot_manager,
                                                sizeof(dnx_state_snapshot_manager_t), "dnx_state_snapshot_manager",
                                                "%s%s%s\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_err_recovery_initialize(unit, (*dnx_er_db)));
    SHR_IF_ERR_EXIT(dnx_state_snapshot_manager_init(unit, (*dnx_er_db)));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static void
dnx_er_db_deinit(
    dnx_er_db_t * dnx_er_db)
{
#ifdef DNX_ROLLBACK_JOURNAL_DIAGNOSTICS
    dnx_rollback_journal_diag_clean(dnx_er_db->unit);
#endif

    if (dnx_er_db->dnx_state_snapshot_manager != NULL)
    {

        dnx_state_snapshot_manager_deinit(dnx_er_db->unit, dnx_er_db);

        SHR_FREE_IGNORE_COUNTERS(dnx_er_db->dnx_state_snapshot_manager);
        dnx_er_db->dnx_state_snapshot_manager = NULL;
    }

    if (dnx_er_db->dnx_err_recovery_manager != NULL)
    {

        dnx_err_recovery_deinitialize(dnx_er_db->unit, dnx_er_db);

        SHR_FREE_IGNORE_COUNTERS(dnx_er_db->dnx_err_recovery_manager);
        dnx_er_db->dnx_err_recovery_manager = NULL;
    }

    if (dnx_er_db->dnx_rollback_journal_pool != NULL)
    {
        SHR_FREE_IGNORE_COUNTERS(dnx_er_db->dnx_rollback_journal_pool);
        dnx_er_db->dnx_rollback_journal_pool = NULL;
    }

    if (dnx_er_db->dnx_dbal_journal_manager != NULL)
    {
        SHR_FREE_IGNORE_COUNTERS(dnx_er_db->dnx_dbal_journal_manager);
        dnx_er_db->dnx_dbal_journal_manager = NULL;
    }

    if (dnx_er_db->dnx_sw_state_manager != NULL)
    {
        SHR_FREE_IGNORE_COUNTERS(dnx_er_db->dnx_sw_state_manager);
        dnx_er_db->dnx_sw_state_manager = NULL;
    }

    SHR_FREE_IGNORE_COUNTERS(dnx_er_db);
    dnx_er_db = NULL;
}

void
dnx_er_db_free(
    void *dnx_er_db)
{
    if (dnx_er_db != NULL)
    {
        dnx_er_db_deinit((dnx_er_db_t *) dnx_er_db);
    }
}

shr_error_e
dnx_er_db_get(
    int unit,
    dnx_er_db_t ** dnx_er_db)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_key[unit] != NULL)
    {
        *dnx_er_db = (dnx_er_db_t *) sal_tls_key_get(dnx_er_db_key[unit]);

        if (*dnx_er_db == NULL)
        {
            SHR_IF_ERR_EXIT(dnx_er_db_init(unit, dnx_er_db));

            sal_tls_key_set(dnx_er_db_key[unit], (void *) *dnx_er_db);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "Cannot retrieve ER DB when the TLS key is NULL.\n");
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_er_db_get_if_initialized(
    int unit,
    dnx_er_db_t ** dnx_er_db)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_key[unit] != NULL)
    {
        *dnx_er_db = (dnx_er_db_t *) sal_tls_key_get(dnx_er_db_key[unit]);
    }

    if (*dnx_er_db == NULL)
    {

        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INIT);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

void
dnx_er_db_key_init(
    int unit)
{

    if (dnx_er_db_key[unit] == NULL)
    {
        dnx_er_db_key[unit] = sal_tls_key_create(dnx_er_db_free);
    }
}

shr_error_e
dnx_er_db_key_deinit(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) == _SHR_E_NONE)
    {
        dnx_er_db_deinit(dnx_er_db);
    }

    if (dnx_er_db_key[unit] != NULL)
    {
        sal_tls_key_delete(dnx_er_db_key[unit]);
        dnx_er_db_key[unit] = NULL;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME

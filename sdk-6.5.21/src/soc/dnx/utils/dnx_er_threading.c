
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

dnx_er_threading_t dnx_er_threading[SOC_MAX_NUM_DEVICES] = { {0}
};

shr_error_e
dnx_er_threading_initialize(
    int unit)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; i++)
    {
        dnx_er_threading[unit].transaction_to_thread[i] = SAL_THREAD_ERROR;
        dnx_er_threading[unit].counter[i] = 0;
        dnx_er_threading[unit].tmp_excluded_counter[i] = 0;
    }

    if ((dnx_er_threading[unit].mtx = sal_mutex_create("ErrorRecovery")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "error recovery threading ERROR: failed to allocate lock");
    }

    dnx_er_threading[unit].is_initialized = TRUE;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_er_threading_deinitialize(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_mutex_destroy(dnx_er_threading[unit].mtx);
    dnx_er_threading[unit].is_initialized = FALSE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_er_threading_transaction_register_internal(
    int unit,
    int is_tmp_expluded_thread)
{
    sal_thread_t tid = sal_thread_self();
    int i = 0;
    int first_empty_id = -1;
    int mtx_taken = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; i++)
    {
        if (dnx_er_threading[unit].transaction_to_thread[i] == tid)
        {
            dnx_er_threading[unit].counter[i]++;
            if (is_tmp_expluded_thread)
            {
                dnx_er_threading[unit].tmp_excluded_counter[i]++;
            }
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT(sal_mutex_take(dnx_er_threading[unit].mtx, sal_mutex_FOREVER));
    mtx_taken = 1;

    for (i = 0; i < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; i++)
    {
        if (dnx_er_threading[unit].transaction_to_thread[i] == tid)
        {
            dnx_er_threading[unit].counter[i]++;
            if (is_tmp_expluded_thread)
            {
                dnx_er_threading[unit].tmp_excluded_counter[i]++;
            }
            SHR_EXIT();
        }
        if (first_empty_id == -1 && dnx_er_threading[unit].transaction_to_thread[i] == SAL_THREAD_ERROR)
        {
            first_empty_id = i;
        }
    }

    if (first_empty_id == -1)
    {
    SHR_ERR_EXIT(_SHR_E_RESOURCE, "the amount of active transactions exceed the allowed amount of %d",
                     DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION)}
    else
    {
        assert(dnx_er_threading[unit].counter[first_empty_id] == 0);
        assert(dnx_er_threading[unit].tmp_excluded_counter[first_empty_id] == 0);
        dnx_er_threading[unit].counter[first_empty_id]++;
        dnx_er_threading[unit].transaction_to_thread[first_empty_id] = tid;
        if (is_tmp_expluded_thread)
        {
            dnx_er_threading[unit].tmp_excluded_counter[first_empty_id]++;
        }
    }

    SHR_EXIT();

exit:
    if (mtx_taken)
    {
        sal_mutex_give(dnx_er_threading[unit].mtx);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_er_threading_transaction_unregister_internal(
    int unit,
    int is_tmp_expluded_thread)
{
    sal_thread_t tid = sal_thread_self();
    int i = 0;
    int mtx_taken = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; i++)
    {
        if (dnx_er_threading[unit].transaction_to_thread[i] == tid)
        {
            if (dnx_er_threading[unit].counter[i] <= 1)
            {
                break;
            }
            dnx_er_threading[unit].counter[i]--;
            if (is_tmp_expluded_thread)
            {
                dnx_er_threading[unit].tmp_excluded_counter[i]--;
            }
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT(sal_mutex_take(dnx_er_threading[unit].mtx, sal_mutex_FOREVER));
    mtx_taken = 1;

    for (i = 0; i < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; i++)
    {
        if (dnx_er_threading[unit].transaction_to_thread[i] == tid)
        {
            dnx_er_threading[unit].counter[i]--;
            if (is_tmp_expluded_thread)
            {
                dnx_er_threading[unit].tmp_excluded_counter[i]--;
            }
            if (dnx_er_threading[unit].counter[i] == 0)
            {
                dnx_er_threading[unit].transaction_to_thread[i] = SAL_THREAD_ERROR;
                assert(dnx_er_threading[unit].tmp_excluded_counter[i] == 0);
            }
            else
            {

                assert(dnx_er_threading[unit].counter[i] > 0);
            }
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_RESOURCE,
                 "transaction_unregister could not find an active transaction registered under the caller thread");

exit:
    if (mtx_taken)
    {
        sal_mutex_give(dnx_er_threading[unit].mtx);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_er_threading_transaction_register(
    int unit)
{
    int rv;

    if (!dnx_err_recovery_are_prerequisites_met(unit))
    {
        return _SHR_E_NONE;
    }

    rv = dnx_er_threading_transaction_register_internal(unit, FALSE);

    assert(rv == _SHR_E_NONE);

    return rv;
}

shr_error_e
dnx_er_threading_transaction_unregister(
    int unit)
{
    int rv;

    if (!dnx_err_recovery_are_prerequisites_met(unit))
    {
        return _SHR_E_NONE;
    }

    rv = dnx_er_threading_transaction_unregister_internal(unit, FALSE);

    assert(rv == _SHR_E_NONE);

    return rv;

}

int
dnx_er_threading_transaction_get(
    int unit)
{
    sal_thread_t tid = sal_thread_self();
    int i = 0;

    if (dnx_err_recovery_are_prerequisites_met(unit))
    {
        for (i = 0; i < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; i++)
        {
            if (dnx_er_threading[unit].transaction_to_thread[i] == tid)
            {
                return i;
            }
        }
    }

    return -1;
}

shr_error_e
dnx_er_threading_tmp_excluded_add(
    int unit)
{
    if (!dnx_err_recovery_are_prerequisites_met(unit))
    {
        return _SHR_E_NONE;
    }
    return dnx_er_threading_transaction_register_internal(unit, TRUE);
}

shr_error_e
dnx_er_threading_tmp_excluded_remove(
    int unit)
{
    if (!dnx_err_recovery_are_prerequisites_met(unit))
    {
        return _SHR_E_NONE;
    }
    return dnx_er_threading_transaction_unregister_internal(unit, TRUE);

}

int
dnx_er_threading_is_tmp_excluded(
    int unit)
{
    sal_thread_t tid = sal_thread_self();
    int i = 0;

    if (!dnx_err_recovery_are_prerequisites_met(unit))
    {
        return TRUE;
    }

    for (i = 0; i < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; i++)
    {
        if (dnx_er_threading[unit].transaction_to_thread[i] == tid)
        {
            return (dnx_er_threading[unit].tmp_excluded_counter[i] > 0);
        }
    }

    return FALSE;
}

#undef _ERR_MSG_MODULE_NAME

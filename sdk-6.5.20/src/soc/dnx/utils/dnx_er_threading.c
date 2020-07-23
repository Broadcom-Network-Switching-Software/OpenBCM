
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
#include <soc/drv.h>
#include <sal/core/dpc.h>

#include <soc/dnx/dnx_er_threading.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

dnx_er_threading_t dnx_er_threading[SOC_MAX_NUM_DEVICES] = { {0}
};


static shr_error_e
dnx_er_threading_mutex_take(
    int unit,
    uint8 *mtx_taken)
{
    SHR_FUNC_INIT_VARS(unit);

    *mtx_taken = FALSE;

    if (dnx_er_threading[unit].is_initialized)
    {
        SHR_IF_ERR_EXIT(sal_mutex_take(dnx_er_threading[unit].mtx, sal_mutex_FOREVER));
        *mtx_taken = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_er_threading_mutex_give(
    int unit,
    uint8 mtx_taken)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_threading[unit].is_initialized && mtx_taken)
    {
        SHR_IF_ERR_EXIT(sal_mutex_give(dnx_er_threading[unit].mtx));
    }

exit:
    SHR_FUNC_EXIT;
}


void
dnx_er_threading_perm_excluded_periodic_event_thread_add(
    int unit)
{
    uint8 mtx_taken = FALSE;
    uint32 idx = 0;
    sal_thread_t tid = sal_thread_self();

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (SAL_THREAD_ERROR == tid)
    {
        LOG_CLI((BSL_META_U(unit, "error recovery threading ERROR: Invalid thread attempted to be excluded\n")));
        assert(0);
    }

    if (dnx_er_threading[unit].config_thread.tid == tid)
    {
        LOG_CLI((BSL_META_U(unit, "error recovery threading ERROR: Config thread attempted to be excluded\n")));
        assert(0);
    }

    if (dnx_er_threading[unit].main_thread.tid == tid)
    {
        LOG_CLI((BSL_META_U(unit, "error recovery threading ERROR: Main thread attempted to be excluded\n")));
        assert(0);
    }
#endif 

    
    dnx_er_threading_mutex_take(unit, &mtx_taken);
    for (idx = 0; idx < dnx_er_threading[unit].internal_periodic_thread_count; idx++)
    {
        if (dnx_er_threading[unit].internal_periodic_event_threads[idx].disabled_counter == 0)
        {
            break;
        }
    }

    dnx_er_threading[unit].internal_periodic_event_threads[idx].disabled_counter = 1;
    dnx_er_threading[unit].internal_periodic_event_threads[idx].tid = tid;

    if (idx == dnx_er_threading[unit].internal_periodic_thread_count)
    {
        dnx_er_threading[unit].internal_periodic_thread_count++;
    }
    dnx_er_threading_mutex_give(unit, mtx_taken);
}


void
dnx_er_threading_perm_excluded_thread_add(
    int unit,
    dnx_err_recovery_thread_internal_ids_e id)
{
    sal_thread_t tid = sal_thread_self();

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (SAL_THREAD_ERROR == tid)
    {
        LOG_CLI((BSL_META_U(unit, "error recovery threading ERROR: Invalid thread attempted to be excluded\n")));
        assert(0);
    }

    if (dnx_er_threading[unit].config_thread.tid == tid)
    {
        LOG_CLI((BSL_META_U(unit, "error recovery threading ERROR: Config thread attempted to be excluded\n")));
        assert(0);
    }

    if (dnx_er_threading[unit].main_thread.tid == tid)
    {
        LOG_CLI((BSL_META_U(unit, "error recovery threading ERROR: Main thread attempted to be excluded\n")));
        assert(0);
    }

    if (dnx_er_threading[unit].internal_threads[id].disabled_counter > 0
        && dnx_er_threading[unit].internal_threads[id].tid == tid)
    {
        LOG_CLI((BSL_META_U
                 (unit,
                  "error recovery threading ERROR: Thread attempted to be registered twice with a different thread id\n")));
        assert(0);
    }
#endif 

    dnx_er_threading[unit].internal_threads[id].disabled_counter = 1;
    dnx_er_threading[unit].internal_threads[id].tid = tid;
}


void
dnx_er_threading_perm_excluded_threads_add_all_units(
    dnx_err_recovery_thread_internal_ids_e id)
{
    int unit = 0;

    for (unit = 0; unit < SOC_MAX_NUM_DEVICES; unit++)
    {
        if (SOC_CONTROL(unit) != NULL && SOC_IS_DNX(unit))
        {
            dnx_er_threading_perm_excluded_thread_add(unit, id);
        }
    }
}


void
dnx_er_threading_perm_excluded_periodic_event_thread_remove(
    int unit)
{
    uint8 mtx_taken = FALSE;
    uint8 found = FALSE;
    uint32 idx = 0;
    sal_thread_t tid = sal_thread_self();

    dnx_er_threading_mutex_take(unit, &mtx_taken);
    for (idx = 0; idx < dnx_er_threading[unit].internal_periodic_thread_count; idx++)
    {
        if (dnx_er_threading[unit].internal_periodic_event_threads[idx].tid == tid)
        {
            found = TRUE;
            break;
        }
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (!found || dnx_er_threading[unit].internal_periodic_event_threads[idx].disabled_counter == 0)
    {
        LOG_CLI((BSL_META_U
                 (unit,
                  "error recovery threading ERROR: Periodic event thread attempted to be removed without being registered\n")));
        assert(0);
    }
#endif 

    if (found)
    {
        dnx_er_threading[unit].internal_periodic_event_threads[idx].disabled_counter = 0;
        dnx_er_threading[unit].internal_periodic_event_threads[idx].tid = SAL_THREAD_ERROR;
    }
    dnx_er_threading_mutex_give(unit, mtx_taken);
}


void
dnx_er_threading_perm_excluded_thread_remove(
    int unit,
    dnx_err_recovery_thread_internal_ids_e id)
{

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (dnx_er_threading[unit].internal_threads[id].disabled_counter == 0)
    {
        LOG_CLI((BSL_META_U
                 (unit, "error recovery threading ERROR: Cannot remove thread from permanently excluded list\n")));
        assert(0);
    }
#endif 

    dnx_er_threading[unit].internal_threads[id].disabled_counter = 0;
    dnx_er_threading[unit].internal_threads[id].tid = SAL_THREAD_ERROR;
}


void
dnx_er_threading_perm_excluded_threads_remove_all_units(
    dnx_err_recovery_thread_internal_ids_e id)
{
    int unit;

    for (unit = 0; unit < SOC_MAX_NUM_DEVICES; unit++)
    {
        if (SOC_CONTROL(unit) != NULL && SOC_IS_DNX(unit))
        {
            dnx_er_threading_perm_excluded_thread_remove(unit, id);
        }
    }
}


static uint8
dnx_er_threading_is_tmp_excluded(
    int unit,
    sal_thread_t tid)
{
    uint8 mtx_taken = FALSE;
    uint8 result = FALSE;
    uint32 idx = 0;

    
    if (dnx_er_threading[unit].excluded_external_thread_count == 0)
    {
        return FALSE;
    }

    
    dnx_er_threading_mutex_take(unit, &mtx_taken);

    for (idx = 0; idx < dnx_er_threading[unit].excluded_external_thread_count; idx++)
    {
        
        if (dnx_er_threading[unit].excluded_external_threads[idx].tid == tid)
        {
            result = TRUE;
            break;
        }
    }

    dnx_er_threading_mutex_give(unit, mtx_taken);

    return result;
}


uint8
dnx_er_threading_is_excluded_thread(
    int unit)
{
    uint8 found = FALSE;
    uint32 idx = 0;
    sal_thread_t tid = sal_thread_self();

    
    for (idx = 0; idx < DNX_ERR_RECOVERY_INTERNAL_THREAD_NOF; idx++)
    {
        
        if (dnx_er_threading[unit].internal_threads[idx].tid == tid)
        {
            return TRUE;
        }
    }

    
    for (idx = 0; idx < dnx_er_threading[unit].internal_periodic_thread_count; idx++)
    {
        if (dnx_er_threading[unit].internal_periodic_event_threads[idx].tid == tid)
        {
            found = TRUE;
            break;
        }
    }

    if (found)
    {
        return found;
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (dnx_er_threading[unit].excluded_external_thread_count != 0
        && dnx_er_threading[unit].main_thread.tid != tid
        && dnx_er_threading[unit].config_thread.tid != tid && dnx_er_threading[unit].skip_validation == 0)
    {
        LOG_CLI((BSL_META_U
                 (unit,
                  "error recovery threading ERROR: every non-main thread SDK thread must utilize DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_ADD/REMOVE(unit, tid)\n")));
        assert(0);
    }
#endif 

    return dnx_er_threading_is_tmp_excluded(unit, tid);
}


static void
dnx_er_threading_tmp_excluded_add_internal(
    int unit,
    sal_thread_t tid)
{
    uint8 mtx_taken = FALSE;
    uint8 found = FALSE;
    uint32 idx = 0;

    
    dnx_er_threading_mutex_take(unit, &mtx_taken);

    
    for (idx = 0; idx < dnx_er_threading[unit].excluded_external_thread_count; idx++)
    {
        if (dnx_er_threading[unit].excluded_external_threads[idx].tid == tid)
        {
            found = TRUE;
            break;
        }
    }

    if (!found)
    {
        dnx_er_threading[unit].excluded_external_threads[idx].disabled_counter = 0;
        dnx_er_threading[unit].excluded_external_threads[idx].tid = tid;

        dnx_er_threading[unit].excluded_external_thread_count++;
    }

    dnx_er_threading[unit].excluded_external_threads[idx].disabled_counter++;

    dnx_er_threading_mutex_give(unit, mtx_taken);
}

void
dnx_er_threading_tmp_excluded_add(
    int unit)
{
    uint32 idx = 0;
    sal_thread_t tid = sal_thread_self();

    
    for (idx = 0; idx < DNX_ERR_RECOVERY_INTERNAL_THREAD_NOF; idx++)
    {
        
        if (dnx_er_threading[unit].internal_threads[idx].tid == tid)
        {
            return;
        }
    }

    
    dnx_er_threading_tmp_excluded_add_internal(unit, tid);
}


static void
dnx_er_threading_tmp_excluded_remove_internal(
    int unit,
    sal_thread_t tid)
{
    uint8 mtx_taken = FALSE;
    uint8 found = FALSE;
    uint32 idx = 0;
    uint32 last = 0;

    
    dnx_er_threading_mutex_take(unit, &mtx_taken);

    for (idx = 0; idx < dnx_er_threading[unit].excluded_external_thread_count; idx++)
    {
        if (dnx_er_threading[unit].excluded_external_threads[idx].tid == tid)
        {
            found = TRUE;
            break;
        }
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (!found || dnx_er_threading[unit].excluded_external_threads[idx].disabled_counter == 0)
    {
        LOG_CLI((BSL_META_U(unit, "error recovery threading ERROR: No support counter end called without start\n")));
        assert(0);
    }
#endif 

    if (found)
    {
        dnx_er_threading[unit].excluded_external_threads[idx].disabled_counter--;

        if (dnx_er_threading[unit].excluded_external_threads[idx].disabled_counter == 0)
        {
            last = dnx_er_threading[unit].excluded_external_thread_count - 1;
            dnx_er_threading[unit].excluded_external_threads[idx] =
                dnx_er_threading[unit].excluded_external_threads[last];
            dnx_er_threading[unit].excluded_external_threads[last].tid = SAL_THREAD_ERROR;
            dnx_er_threading[unit].excluded_external_threads[last].disabled_counter = 0;
            dnx_er_threading[unit].excluded_external_thread_count--;
        }
    }

    dnx_er_threading_mutex_give(unit, mtx_taken);
}

void
dnx_er_threading_tmp_excluded_remove(
    int unit)
{
    uint32 idx = 0;
    sal_thread_t tid = sal_thread_self();

    
    for (idx = 0; idx < DNX_ERR_RECOVERY_INTERNAL_THREAD_NOF; idx++)
    {
        
        if (dnx_er_threading[unit].internal_threads[idx].tid == tid)
        {
            return;
        }
    }

    
    dnx_er_threading_tmp_excluded_remove_internal(unit, tid);
}


uint8
dnx_er_threading_is_main_thread(
    int unit)
{
    return (dnx_er_threading[unit].is_initialized
            && dnx_er_threading[unit].main_thread.tid != SAL_THREAD_ERROR
            && dnx_er_threading[unit].main_thread.tid == sal_thread_self());
}

#ifdef DNX_ERR_RECOVERY_VALIDATION

uint32
dnx_er_threading_test_skip_validation(
    int unit,
    uint8 is_on)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_er_threading[unit].skip_validation = is_on;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#endif 


uint8
dnx_er_threading_is_journaling_thread(
    int unit)
{
    
    return (dnx_er_threading[unit].is_initialized
            && dnx_er_threading[unit].journaling_thread.tid != SAL_THREAD_ERROR
            && dnx_er_threading[unit].journaling_thread.tid == sal_thread_self());
}


shr_error_e
dnx_er_threading_journaling_thread_set(
    int unit)
{
    sal_thread_t tid = sal_thread_self();

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (dnx_er_threading[unit].journaling_thread.tid == tid)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Error Recovery threading ERROR: Attempted to reset the thread id while transaction was active!\n");
    }
#endif 

    
    if (dnx_er_threading[unit].is_initialized && dnx_er_threading[unit].journaling_thread.tid == SAL_THREAD_ERROR)
    {
        dnx_er_threading[unit].journaling_thread.tid = tid;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_er_threading_journaling_thread_clear(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (dnx_er_threading[unit].is_initialized && dnx_er_threading[unit].journaling_thread.tid != SAL_THREAD_ERROR)
    {
        dnx_er_threading[unit].journaling_thread.tid = SAL_THREAD_ERROR;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static void
dnx_er_threading_dpc_thread_cb(
    void *owner_ptr,
    void *dont_care_0,
    void *dont_care_1,
    void *dont_care_2,
    void *dont_care_3)
{
    dnx_er_threading_perm_excluded_thread_add(PTR_TO_INT(owner_ptr), DNX_ERR_RECOVERY_INTERNAL_THREAD_DPC);
}


shr_error_e
dnx_er_threading_initialize(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_er_threading[unit].journaling_thread.tid = SAL_THREAD_ERROR;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    dnx_er_threading[unit].config_thread.tid = sal_thread_self();
#endif 

    dnx_er_threading[unit].main_thread.tid = sal_thread_main_get();

    
    sal_dpc(dnx_er_threading_dpc_thread_cb, INT_TO_PTR(unit), 0, 0, 0, 0);

    if ((dnx_er_threading[unit].mtx = sal_mutex_create("er thread cnt mtx")) == NULL)
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

    dnx_er_threading[unit].journaling_thread.tid = SAL_THREAD_ERROR;
    dnx_er_threading[unit].excluded_external_thread_count = 0;

    dnx_er_threading[unit].is_initialized = FALSE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_er_threading_no_parallel_transactions_verify(
    int unit)
{
    sal_thread_t tid = sal_thread_self();

    SHR_FUNC_INIT_VARS(unit);

    
    if (dnx_er_threading[unit].journaling_thread.tid != SAL_THREAD_ERROR
        && dnx_er_threading[unit].journaling_thread.tid != tid)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "error recovery threading ERROR: Transaction attempted to be started in parallel to an existing transaction on another thread!\n");
    }

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME

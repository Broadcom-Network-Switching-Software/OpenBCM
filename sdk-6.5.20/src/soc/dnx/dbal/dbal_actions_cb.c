/** \file dbal_actions_cb.c
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * cb for other modules to use before and after accessing the HW
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include "dbal_internal.h"


#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/multithread_analyzer.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnx/dbal/dbal_journal.h>


shr_error_e
dbal_action_access_error_recovery_invalidate(
    int unit,
    dbal_action_access_func_e action_access_func)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (action_access_func)
    {
        case DBAL_ACTION_ACCESS_FUNC_TABLE_CLEAR:
            if (_SHR_E_NONE != dnx_err_recovery_transaction_invalidate(unit))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal table clear not supported in Error Recovery transactions!\n");
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Function cannot invalidate the Error Recovery transaction!\n");
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_action_access_pre_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_action_access_type_e action_access_type,
    dbal_action_access_func_e action_access_func)
{
    SHR_FUNC_INIT_VARS(unit);

    if (action_access_type == DBAL_ACTION_ACCESS_TYPE_WRITE)
    {
        
        SHR_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_TABLE_DIRTY);
    }

    if (entry_handle->er_flags & DNX_DBAL_JOURNAL_ROLLBACK_FLAG
        || entry_handle->er_flags & DNX_DBAL_JOURNAL_COMPARE_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_err_recovery_common_dbal_access_region_start(unit));
    }

    if (dnxc_ha_is_access_disabled(unit, UTILEX_SEQ_ALLOW_DBAL))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "It is not allowed to access DBAL during deinit or warmboot\n");
    }

    DNXC_MTA(dnxc_multithread_analyzer_mark_dbal_region(unit, TRUE));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_action_access_post_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_entry_handle,
    dbal_action_access_type_e action_access_type,
    dbal_action_access_func_e action_access_func,
    int rv)
{

    SHR_FUNC_INIT_VARS(unit);

    DNXC_MTA(dnxc_multithread_analyzer_mark_dbal_region(unit, FALSE));

    switch (action_access_func)
    {
        case DBAL_ACTION_ACCESS_FUNC_ENTRY_COMMIT:
            
            if (rv != _SHR_E_FULL)
            {
                DNX_DBAL_JRNL(SHR_IF_ERR_EXIT
                              (dnx_dbal_journal_log_add(unit, get_entry_handle, entry_handle, _SHR_E_NONE != rv)));
            }
            DNXC_MTA(SHR_IF_ERR_EXIT
                     (dnxc_multithread_analyzer_log_resource_use
                      (unit, MTA_RESOURCE_DBAL, entry_handle->table_id, TRUE)));

            break;
        case DBAL_ACTION_ACCESS_FUNC_ENTRY_GET:
            DNXC_MTA(SHR_IF_ERR_EXIT
                     (dnxc_multithread_analyzer_log_resource_use
                      (unit, MTA_RESOURCE_DBAL, entry_handle->table_id, FALSE)));
            break;
        case DBAL_ACTION_ACCESS_FUNC_ENTRY_CLEAR:
            DNX_DBAL_JRNL(SHR_IF_ERR_EXIT(dnx_dbal_journal_log_clear(unit, get_entry_handle, _SHR_E_NONE != rv)));
            DNXC_MTA(SHR_IF_ERR_EXIT
                     (dnxc_multithread_analyzer_log_resource_use
                      (unit, MTA_RESOURCE_DBAL, entry_handle->table_id, TRUE)));
            break;
        case DBAL_ACTION_ACCESS_FUNC_TABLE_CLEAR:
            DNX_DBAL_JRNL(SHR_IF_ERR_EXIT
                          (dbal_action_access_error_recovery_invalidate(unit, DBAL_ACTION_ACCESS_FUNC_TABLE_CLEAR)));
            break;
        default:
            break;
    }

    SHR_EXIT();
exit:
    if (entry_handle->er_flags & DNX_DBAL_JOURNAL_ROLLBACK_FLAG
        || entry_handle->er_flags & DNX_DBAL_JOURNAL_COMPARE_FLAG)
    {
        dnx_err_recovery_common_dbal_access_region_end(unit);
    }

    SHR_FUNC_EXIT;
}

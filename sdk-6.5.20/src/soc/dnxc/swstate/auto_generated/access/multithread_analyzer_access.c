
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnxc/swstate/auto_generated/access/multithread_analyzer_access.h>
#include <soc/dnxc/swstate/auto_generated/diagnostic/multithread_analyzer_diagnostic.h>



multithread_analyzer_t* multithread_analyzer_dummy = NULL;
multithread_analyzer_t* multithread_analyzer_root[SOC_MAX_NUM_DEVICES] = {NULL};



int
multithread_analyzer_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    *is_init = (NULL != ((multithread_analyzer_t*)multithread_analyzer_root[unit]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]),
        "multithread_analyzer[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
multithread_analyzer_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    DNX_SW_STATE_NO_WB_MODULE_INIT(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_root[unit],
        DNXC_SW_STATE_IMPLEMENTATION_PLAIN,
        multithread_analyzer_t,
        DNXC_SW_STATE_ALLOC_AFTER_INIT_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "multithread_analyzer_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]),
        "multithread_analyzer[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_info,
        MULTITHREAD_ANALYZER_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(multithread_analyzer_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
multithread_analyzer_deinit(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        SW_STATE_FUNC_DEINIT,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_info,
        MULTITHREAD_ANALYZER_INFO,
        DNX_SW_STATE_DIAG_FREE,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]));

    DNX_SW_STATE_NO_WB_MODULE_DEINIT(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_root[unit],
        0,
        "multithread_analyzer_deinit");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_DEINIT_LOGGING,
        BSL_LS_SWSTATEDNX_DEINIT,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]),
        "multithread_analyzer[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
multithread_analyzer_htb_create(int unit, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID);

    SW_STATE_HTB_CREATE(
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        init_info,
        thread_access_log_entry_t,
        uint32,
        TRUE,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        MULTITHREAD_ANALYZER_MODULE_ID,
        &((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        "multithread_analyzer[%d]->->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_info,
        MULTITHREAD_ANALYZER_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
multithread_analyzer_htb_destroy(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        SW_STATE_FUNC_DESTROY,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID);

    SW_STATE_HTB_DESTROY(
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DESTROY_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDESTROY,
        MULTITHREAD_ANALYZER_MODULE_ID,
        &((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        "multithread_analyzer[%d]->->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_info,
        MULTITHREAD_ANALYZER_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
multithread_analyzer_htb_find(int unit, const thread_access_log_entry_t *key, uint32 *data_index, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID);

    SW_STATE_HTB_FIND(
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        key,
        data_index,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        MULTITHREAD_ANALYZER_MODULE_ID,
        &((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        "multithread_analyzer[%d]->->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_info,
        MULTITHREAD_ANALYZER_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
multithread_analyzer_htb_insert(int unit, const thread_access_log_entry_t *key, const uint32 *data_index, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID);

    SW_STATE_HTB_INSERT(
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        key,
        data_index,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        MULTITHREAD_ANALYZER_MODULE_ID,
        &((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        "multithread_analyzer[%d]->->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_info,
        MULTITHREAD_ANALYZER_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
multithread_analyzer_htb_get_next(int unit, SW_STATE_HASH_TABLE_ITER *iter, const thread_access_log_entry_t *key, const uint32 *data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID);

    SW_STATE_HTB_GET_NEXT(
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        iter,
        key,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        MULTITHREAD_ANALYZER_MODULE_ID,
        &((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        "multithread_analyzer[%d]->->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_info,
        MULTITHREAD_ANALYZER_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
multithread_analyzer_htb_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID);

    SW_STATE_HTB_CLEAR(
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        MULTITHREAD_ANALYZER_MODULE_ID,
        &((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        "multithread_analyzer[%d]->->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_info,
        MULTITHREAD_ANALYZER_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
multithread_analyzer_htb_delete(int unit, const thread_access_log_entry_t *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID);

    SW_STATE_HTB_DELETE(
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        MULTITHREAD_ANALYZER_MODULE_ID,
        &((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        "multithread_analyzer[%d]->->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_info,
        MULTITHREAD_ANALYZER_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
multithread_analyzer_htb_insert_at_index(int unit, const thread_access_log_entry_t *key, uint32 data_idx, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        SW_STATE_FUNC_INSERT_AT_INDEX,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID);

    SW_STATE_HTB_INSERT_AT_INDEX(
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        key,
        data_idx,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_AT_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT_AT_INDEX,
        MULTITHREAD_ANALYZER_MODULE_ID,
        &((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        "multithread_analyzer[%d]->->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_info,
        MULTITHREAD_ANALYZER_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
multithread_analyzer_htb_delete_by_index(int unit, uint32 data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        SW_STATE_FUNC_DELETE_BY_INDEX,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID);

    SW_STATE_HTB_DELETE_BY_INDEX(
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_BY_INDEX,
        MULTITHREAD_ANALYZER_MODULE_ID,
        &((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        "multithread_analyzer[%d]->->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_info,
        MULTITHREAD_ANALYZER_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
multithread_analyzer_htb_get_by_index(int unit, uint32 data_index, thread_access_log_entry_t *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        SW_STATE_FUNC_GET_BY_INDEX,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit]));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID);

    SW_STATE_HTB_GET_BY_INDEX(
        MULTITHREAD_ANALYZER_MODULE_ID,
        ((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        data_index,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_BY_INDEX,
        MULTITHREAD_ANALYZER_MODULE_ID,
        &((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb,
        "multithread_analyzer[%d]->->htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MULTITHREAD_ANALYZER_MODULE_ID,
        multithread_analyzer_info,
        MULTITHREAD_ANALYZER_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}





multithread_analyzer_cbs multithread_analyzer = 	{
	
	multithread_analyzer_is_init,
	multithread_analyzer_init,
	multithread_analyzer_deinit,
		{
		
		multithread_analyzer_htb_create,
		multithread_analyzer_htb_destroy,
		multithread_analyzer_htb_find,
		multithread_analyzer_htb_insert,
		multithread_analyzer_htb_get_next,
		multithread_analyzer_htb_clear,
		multithread_analyzer_htb_delete,
		multithread_analyzer_htb_insert_at_index,
		multithread_analyzer_htb_delete_by_index,
		multithread_analyzer_htb_get_by_index}
	}
;
#undef BSL_LOG_MODULE

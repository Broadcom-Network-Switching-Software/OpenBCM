
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
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_diagnostic.h>



dnx_field_tcam_access_t* dnx_field_tcam_access_sw_dummy = NULL;



int
dnx_field_tcam_access_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]),
        "dnx_field_tcam_access_sw[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_tcam_access_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]),
        "dnx_field_tcam_access_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_field_tcam_access_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_location_hash_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        sw_state_htbl_t,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_entry_location_hash_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        "dnx_field_tcam_access_sw[%d]->entry_location_hash",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(sw_state_htbl_t) / sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_location_hash_create(int unit, uint32 entry_location_hash_idx_0, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    SW_STATE_HTB_CREATE(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        init_info,
        uint32,
        uint32,
        TRUE,
        dnx_data_field.tcam.tcam_banks_size_get(unit),
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_location_hash_find(int unit, uint32 entry_location_hash_idx_0, const uint32 *key, uint32 *data_index, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    SW_STATE_HTB_FIND(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        key,
        data_index,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_location_hash_insert(int unit, uint32 entry_location_hash_idx_0, const uint32 *key, const uint32 *data_index, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    SW_STATE_HTB_INSERT(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        key,
        data_index,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_location_hash_get_next(int unit, uint32 entry_location_hash_idx_0, SW_STATE_HASH_TABLE_ITER *iter, const uint32 *key, const uint32 *data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    SW_STATE_HTB_GET_NEXT(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        iter,
        key,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_location_hash_clear(int unit, uint32 entry_location_hash_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    SW_STATE_HTB_CLEAR(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_location_hash_delete(int unit, uint32 entry_location_hash_idx_0, const uint32 *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    SW_STATE_HTB_DELETE(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_location_hash_insert_at_index(int unit, uint32 entry_location_hash_idx_0, const uint32 *key, uint32 data_idx, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_INSERT_AT_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    SW_STATE_HTB_INSERT_AT_INDEX(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        key,
        data_idx,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_AT_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT_AT_INDEX,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_location_hash_delete_by_index(int unit, uint32 entry_location_hash_idx_0, uint32 data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_DELETE_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    SW_STATE_HTB_DELETE_BY_INDEX(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_BY_INDEX,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_location_hash_get_by_index(int unit, uint32 entry_location_hash_idx_0, uint32 data_index, uint32 *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_GET_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    SW_STATE_HTB_GET_BY_INDEX(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        data_index,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_BY_INDEX,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        dnx_field_tcam_access_v_bit_t,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_valid_bmp_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        "dnx_field_tcam_access_sw[%d]->valid_bmp",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(dnx_field_tcam_access_v_bit_t) / sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_alloc_bitmap(int unit, uint32 valid_bmp_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        dnx_data_field.tcam.tcam_banks_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_valid_bmp_v_bit_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0,
        dnx_data_field.tcam.tcam_banks_size_get(unit));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_set(int unit, uint32 valid_bmp_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_clear(int unit, uint32 valid_bmp_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_get(int unit, uint32 valid_bmp_idx_0, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_read(int unit, uint32 valid_bmp_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_write(int unit, uint32 valid_bmp_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_and(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_or(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_xor(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_remove(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_negate(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_clear(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_set(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_null(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_test(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_eq(int unit, uint32 valid_bmp_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _first,
         _count,
         input_bmp,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEEQ,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_count(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        dnx_field_tcam_access_v_bit_t,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_entry_in_use_bmp_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(dnx_field_tcam_access_v_bit_t) / sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_alloc_bitmap(int unit, uint32 entry_in_use_bmp_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        dnx_data_field.tcam.tcam_banks_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0,
        dnx_data_field.tcam.tcam_banks_size_get(unit));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_set(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_clear(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_get(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_read(int unit, uint32 entry_in_use_bmp_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_write(int unit, uint32 entry_in_use_bmp_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_and(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_or(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_xor(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_remove(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_negate(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_clear(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_set(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_null(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_test(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_eq(int unit, uint32 entry_in_use_bmp_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _first,
         _count,
         input_bmp,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEEQ,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_count(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_FIELD_TCAM_ACCESS_MODULE_ID,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_set(int unit, uint32 fg_params_idx_0, CONST dnx_field_tcam_access_fg_params_t *fg_params)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0],
        fg_params,
        dnx_field_tcam_access_fg_params_t,
        0,
        "dnx_field_tcam_access_sw_fg_params_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        fg_params,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d]",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_get(int unit, uint32 fg_params_idx_0, dnx_field_tcam_access_fg_params_t *fg_params)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        fg_params);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    *fg_params = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0],
        "dnx_field_tcam_access_sw[%d]->fg_params[%d]",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_alloc(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        dnx_field_tcam_access_fg_params_t,
        dnx_data_field.group.nof_fgs_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_fg_params_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        "dnx_field_tcam_access_sw[%d]->fg_params",
        unit,
        dnx_data_field.group.nof_fgs_get(unit) * sizeof(dnx_field_tcam_access_fg_params_t) / sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[def_val_idx[0]].stage,
        DNX_FIELD_TCAM_STAGE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[def_val_idx[0]].dt_bank_id,
        DNX_FIELD_TCAM_BANK_ID_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[def_val_idx[0]].bank_allocation_mode,
        DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[def_val_idx[0]].context_sharing_handlers_cb,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_key_size_set(int unit, uint32 fg_params_idx_0, uint32 key_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].key_size,
        key_size,
        uint32,
        0,
        "dnx_field_tcam_access_sw_fg_params_key_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &key_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].key_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_KEY_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_key_size_get(int unit, uint32 fg_params_idx_0, uint32 *key_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        key_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    *key_size = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].key_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].key_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].key_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_KEY_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_action_size_set(int unit, uint32 fg_params_idx_0, uint32 action_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].action_size,
        action_size,
        uint32,
        0,
        "dnx_field_tcam_access_sw_fg_params_action_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &action_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].action_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_ACTION_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_action_size_get(int unit, uint32 fg_params_idx_0, uint32 *action_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        action_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    *action_size = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].action_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].action_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].action_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_ACTION_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_actual_action_size_set(int unit, uint32 fg_params_idx_0, uint32 actual_action_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].actual_action_size,
        actual_action_size,
        uint32,
        0,
        "dnx_field_tcam_access_sw_fg_params_actual_action_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &actual_action_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].actual_action_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_ACTUAL_ACTION_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_actual_action_size_get(int unit, uint32 fg_params_idx_0, uint32 *actual_action_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        actual_action_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    *actual_action_size = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].actual_action_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].actual_action_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].actual_action_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_ACTUAL_ACTION_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_stage_set(int unit, uint32 fg_params_idx_0, dnx_field_tcam_stage_e stage)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].stage,
        stage,
        dnx_field_tcam_stage_e,
        0,
        "dnx_field_tcam_access_sw_fg_params_stage_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &stage,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].stage",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_STAGE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_stage_get(int unit, uint32 fg_params_idx_0, dnx_field_tcam_stage_e *stage)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        stage);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    *stage = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].stage;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].stage,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].stage",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_STAGE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_prefix_size_set(int unit, uint32 fg_params_idx_0, uint32 prefix_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].prefix_size,
        prefix_size,
        uint32,
        0,
        "dnx_field_tcam_access_sw_fg_params_prefix_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &prefix_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].prefix_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_PREFIX_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_prefix_size_get(int unit, uint32 fg_params_idx_0, uint32 *prefix_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        prefix_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    *prefix_size = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].prefix_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].prefix_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].prefix_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_PREFIX_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_prefix_value_set(int unit, uint32 fg_params_idx_0, uint32 prefix_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].prefix_value,
        prefix_value,
        uint32,
        0,
        "dnx_field_tcam_access_sw_fg_params_prefix_value_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &prefix_value,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].prefix_value",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_PREFIX_VALUE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_prefix_value_get(int unit, uint32 fg_params_idx_0, uint32 *prefix_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        prefix_value);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    *prefix_value = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].prefix_value;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].prefix_value,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].prefix_value",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_PREFIX_VALUE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_direct_table_set(int unit, uint32 fg_params_idx_0, uint8 direct_table)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].direct_table,
        direct_table,
        uint8,
        0,
        "dnx_field_tcam_access_sw_fg_params_direct_table_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &direct_table,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].direct_table",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_DIRECT_TABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_direct_table_get(int unit, uint32 fg_params_idx_0, uint8 *direct_table)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        direct_table);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    *direct_table = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].direct_table;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].direct_table,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].direct_table",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_DIRECT_TABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_dt_bank_id_set(int unit, uint32 fg_params_idx_0, int dt_bank_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].dt_bank_id,
        dt_bank_id,
        int,
        0,
        "dnx_field_tcam_access_sw_fg_params_dt_bank_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &dt_bank_id,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].dt_bank_id",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_DT_BANK_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_dt_bank_id_get(int unit, uint32 fg_params_idx_0, int *dt_bank_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dt_bank_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    *dt_bank_id = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].dt_bank_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].dt_bank_id,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].dt_bank_id",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_DT_BANK_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_set(int unit, uint32 fg_params_idx_0, dnx_field_tcam_bank_allocation_mode_e bank_allocation_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].bank_allocation_mode,
        bank_allocation_mode,
        dnx_field_tcam_bank_allocation_mode_e,
        0,
        "dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &bank_allocation_mode,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].bank_allocation_mode",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_BANK_ALLOCATION_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_get(int unit, uint32 fg_params_idx_0, dnx_field_tcam_bank_allocation_mode_e *bank_allocation_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        bank_allocation_mode);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    *bank_allocation_mode = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].bank_allocation_mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].bank_allocation_mode,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].bank_allocation_mode",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_BANK_ALLOCATION_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_set(int unit, uint32 fg_params_idx_0, dnx_field_tcam_context_sharing_handlers_get_p context_sharing_handlers_cb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].context_sharing_handlers_cb,
        context_sharing_handlers_cb,
        dnx_field_tcam_context_sharing_handlers_get_p,
        0,
        "dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &context_sharing_handlers_cb,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].context_sharing_handlers_cb",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_CONTEXT_SHARING_HANDLERS_CB_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_get(int unit, uint32 fg_params_idx_0, dnx_field_tcam_context_sharing_handlers_get_p *context_sharing_handlers_cb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        context_sharing_handlers_cb);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID);

    *context_sharing_handlers_cb = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].context_sharing_handlers_cb;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].context_sharing_handlers_cb,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].context_sharing_handlers_cb",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MODULE_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_CONTEXT_SHARING_HANDLERS_CB_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




const char *
dnx_field_tcam_stage_e_get_name(dnx_field_tcam_stage_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_INVALID", value, DNX_FIELD_TCAM_STAGE_INVALID);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_FIRST", value, DNX_FIELD_TCAM_STAGE_FIRST);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_IPMF1", value, DNX_FIELD_TCAM_STAGE_IPMF1);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_IPMF2", value, DNX_FIELD_TCAM_STAGE_IPMF2);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_IPMF3", value, DNX_FIELD_TCAM_STAGE_IPMF3);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_EPMF", value, DNX_FIELD_TCAM_STAGE_EPMF);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_VTT1", value, DNX_FIELD_TCAM_STAGE_VTT1);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_VTT2", value, DNX_FIELD_TCAM_STAGE_VTT2);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_VTT3", value, DNX_FIELD_TCAM_STAGE_VTT3);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_VTT4", value, DNX_FIELD_TCAM_STAGE_VTT4);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_VTT5", value, DNX_FIELD_TCAM_STAGE_VTT5);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_FWD1", value, DNX_FIELD_TCAM_STAGE_FWD1);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_FWD2", value, DNX_FIELD_TCAM_STAGE_FWD2);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_NOF", value, DNX_FIELD_TCAM_STAGE_NOF);

    return NULL;
}




const char *
dnx_field_tcam_bank_allocation_mode_e_get_name(dnx_field_tcam_bank_allocation_mode_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_INVALID", value, DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_INVALID);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_FIRST", value, DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_FIRST);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO", value, DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT", value, DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION", value, DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_NOF", value, DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_NOF);

    return NULL;
}




const char *
dnx_field_tcam_entry_size_e_get_name(dnx_field_tcam_entry_size_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_ENTRY_SIZE_INVALID", value, DNX_FIELD_TCAM_ENTRY_SIZE_INVALID);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_ENTRY_SIZE_FIRST", value, DNX_FIELD_TCAM_ENTRY_SIZE_FIRST);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_ENTRY_SIZE_HALF", value, DNX_FIELD_TCAM_ENTRY_SIZE_HALF);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE", value, DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_ENTRY_SIZE_DOUBLE", value, DNX_FIELD_TCAM_ENTRY_SIZE_DOUBLE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_ENTRY_SIZE_COUNT", value, DNX_FIELD_TCAM_ENTRY_SIZE_COUNT);

    return NULL;
}




const char *
dnx_field_tcam_core_e_get_name(dnx_field_tcam_core_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_CORE_INVALID", value, DNX_FIELD_TCAM_CORE_INVALID);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_CORE_FIRST", value, DNX_FIELD_TCAM_CORE_FIRST);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_CORE_0", value, DNX_FIELD_TCAM_CORE_0);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_CORE_1", value, DNX_FIELD_TCAM_CORE_1);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_CORE_ALL", value, DNX_FIELD_TCAM_CORE_ALL);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_CORE_COUNT", value, DNX_FIELD_TCAM_CORE_COUNT);

    return NULL;
}





dnx_field_tcam_access_sw_cbs dnx_field_tcam_access_sw = 	{
	
	dnx_field_tcam_access_sw_is_init,
	dnx_field_tcam_access_sw_init,
		{
		
		dnx_field_tcam_access_sw_entry_location_hash_alloc,
		dnx_field_tcam_access_sw_entry_location_hash_create,
		dnx_field_tcam_access_sw_entry_location_hash_find,
		dnx_field_tcam_access_sw_entry_location_hash_insert,
		dnx_field_tcam_access_sw_entry_location_hash_get_next,
		dnx_field_tcam_access_sw_entry_location_hash_clear,
		dnx_field_tcam_access_sw_entry_location_hash_delete,
		dnx_field_tcam_access_sw_entry_location_hash_insert_at_index,
		dnx_field_tcam_access_sw_entry_location_hash_delete_by_index,
		dnx_field_tcam_access_sw_entry_location_hash_get_by_index}
	,
		{
		
		dnx_field_tcam_access_sw_valid_bmp_alloc,
			{
			
			dnx_field_tcam_access_sw_valid_bmp_v_bit_alloc_bitmap,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_set,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_clear,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_get,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_read,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_write,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_and,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_or,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_xor,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_remove,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_negate,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_clear,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_set,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_null,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_test,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_eq,
			dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_count}
		}
	,
		{
		
		dnx_field_tcam_access_sw_entry_in_use_bmp_alloc,
			{
			
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_alloc_bitmap,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_set,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_clear,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_get,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_read,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_write,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_and,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_or,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_xor,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_remove,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_negate,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_clear,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_set,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_null,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_test,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_eq,
			dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_count}
		}
	,
		{
		
		dnx_field_tcam_access_sw_fg_params_set,
		dnx_field_tcam_access_sw_fg_params_get,
		dnx_field_tcam_access_sw_fg_params_alloc,
			{
			
			dnx_field_tcam_access_sw_fg_params_key_size_set,
			dnx_field_tcam_access_sw_fg_params_key_size_get}
		,
			{
			
			dnx_field_tcam_access_sw_fg_params_action_size_set,
			dnx_field_tcam_access_sw_fg_params_action_size_get}
		,
			{
			
			dnx_field_tcam_access_sw_fg_params_actual_action_size_set,
			dnx_field_tcam_access_sw_fg_params_actual_action_size_get}
		,
			{
			
			dnx_field_tcam_access_sw_fg_params_stage_set,
			dnx_field_tcam_access_sw_fg_params_stage_get}
		,
			{
			
			dnx_field_tcam_access_sw_fg_params_prefix_size_set,
			dnx_field_tcam_access_sw_fg_params_prefix_size_get}
		,
			{
			
			dnx_field_tcam_access_sw_fg_params_prefix_value_set,
			dnx_field_tcam_access_sw_fg_params_prefix_value_get}
		,
			{
			
			dnx_field_tcam_access_sw_fg_params_direct_table_set,
			dnx_field_tcam_access_sw_fg_params_direct_table_get}
		,
			{
			
			dnx_field_tcam_access_sw_fg_params_dt_bank_id_set,
			dnx_field_tcam_access_sw_fg_params_dt_bank_id_get}
		,
			{
			
			dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_set,
			dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_get}
		,
			{
			
			dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_set,
			dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_get}
		}
	}
;
#undef BSL_LOG_MODULE

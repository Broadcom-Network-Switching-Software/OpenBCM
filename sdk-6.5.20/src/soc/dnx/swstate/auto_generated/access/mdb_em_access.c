
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
#include <soc/dnx/swstate/auto_generated/access/mdb_em_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/mdb_em_diagnostic.h>



mdb_em_sw_state_t* mdb_em_db_dummy = NULL;



int
mdb_em_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID]),
        "mdb_em_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_init(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        MDB_EM_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        mdb_em_sw_state_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_em_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID]),
        "mdb_em_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(mdb_em_sw_state_t),
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DBAL_NOF_PHYSICAL_TABLES)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_MDB_EM_VMV_NOF_VALUES)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[def_val_idx[1]].encoding_map[def_val_idx[2]].encoding,
        DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_shadow_em_db_create(int unit, uint32 shadow_em_db_idx_0, sw_state_htbl_init_info_t *init_info, int key_size, int value_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    SW_STATE_HTB_CREATE_WITH_SIZE(
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0],
        init_info,
        key_size,
        value_size,
        FALSE,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0],
        "mdb_em_db[%d]->shadow_em_db[%d]",
        unit, shadow_em_db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_SHADOW_EM_DB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_shadow_em_db_find(int unit, uint32 shadow_em_db_idx_0, const void *key, void *value, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    SW_STATE_HTB_FIND(
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0],
        key,
        value,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0],
        "mdb_em_db[%d]->shadow_em_db[%d]",
        unit, shadow_em_db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_SHADOW_EM_DB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_shadow_em_db_insert(int unit, uint32 shadow_em_db_idx_0, const void *key, const void *value, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    SW_STATE_HTB_INSERT(
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0],
        key,
        value,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0],
        "mdb_em_db[%d]->shadow_em_db[%d]",
        unit, shadow_em_db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_SHADOW_EM_DB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_shadow_em_db_get_next(int unit, uint32 shadow_em_db_idx_0, SW_STATE_HASH_TABLE_ITER *iter, const void *key, const void *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    SW_STATE_HTB_GET_NEXT(
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0],
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0],
        "mdb_em_db[%d]->shadow_em_db[%d]",
        unit, shadow_em_db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_SHADOW_EM_DB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_shadow_em_db_clear(int unit, uint32 shadow_em_db_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    SW_STATE_HTB_CLEAR(
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0],
        "mdb_em_db[%d]->shadow_em_db[%d]",
        unit, shadow_em_db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_SHADOW_EM_DB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_shadow_em_db_delete(int unit, uint32 shadow_em_db_idx_0, const void *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    SW_STATE_HTB_DELETE(
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0],
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0],
        "mdb_em_db[%d]->shadow_em_db[%d]",
        unit, shadow_em_db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_SHADOW_EM_DB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_shadow_em_db_delete_all(int unit, uint32 shadow_em_db_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_DELETE_ALL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    SW_STATE_HTB_DELETE_ALL(
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_ALL_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_ALL,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[shadow_em_db_idx_0],
        "mdb_em_db[%d]->shadow_em_db[%d]",
        unit, shadow_em_db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_SHADOW_EM_DB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_value_set(int unit, uint32 vmv_info_idx_0, uint32 value_idx_0, uint8 value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vmv_info_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        value_idx_0,
        DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].value[value_idx_0],
        value,
        uint8,
        0,
        "mdb_em_db_vmv_info_value_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        MDB_EM_MODULE_ID,
        &value,
        "mdb_em_db[%d]->vmv_info[%d].value[%d]",
        unit, vmv_info_idx_0, value_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_VMV_INFO_VALUE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_value_get(int unit, uint32 vmv_info_idx_0, uint32 value_idx_0, uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vmv_info_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        MDB_EM_MODULE_ID,
        value);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        value_idx_0,
        DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    *value = ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].value[value_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].value[value_idx_0],
        "mdb_em_db[%d]->vmv_info[%d].value[%d]",
        unit, vmv_info_idx_0, value_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_VMV_INFO_VALUE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_size_set(int unit, uint32 vmv_info_idx_0, uint32 size_idx_0, uint8 size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vmv_info_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        size_idx_0,
        DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].size[size_idx_0],
        size,
        uint8,
        0,
        "mdb_em_db_vmv_info_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        MDB_EM_MODULE_ID,
        &size,
        "mdb_em_db[%d]->vmv_info[%d].size[%d]",
        unit, vmv_info_idx_0, size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_VMV_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_size_get(int unit, uint32 vmv_info_idx_0, uint32 size_idx_0, uint8 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vmv_info_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        MDB_EM_MODULE_ID,
        size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        size_idx_0,
        DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    *size = ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].size[size_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].size[size_idx_0],
        "mdb_em_db[%d]->vmv_info[%d].size[%d]",
        unit, vmv_info_idx_0, size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_VMV_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_encoding_map_encoding_set(int unit, uint32 vmv_info_idx_0, uint32 encoding_map_idx_0, uint8 encoding)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vmv_info_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        encoding_map_idx_0,
        DNX_DATA_MAX_MDB_EM_VMV_NOF_VALUES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].encoding_map[encoding_map_idx_0].encoding,
        encoding,
        uint8,
        0,
        "mdb_em_db_vmv_info_encoding_map_encoding_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        MDB_EM_MODULE_ID,
        &encoding,
        "mdb_em_db[%d]->vmv_info[%d].encoding_map[%d].encoding",
        unit, vmv_info_idx_0, encoding_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_VMV_INFO_ENCODING_MAP_ENCODING_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_encoding_map_encoding_get(int unit, uint32 vmv_info_idx_0, uint32 encoding_map_idx_0, uint8 *encoding)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vmv_info_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        MDB_EM_MODULE_ID,
        encoding);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        encoding_map_idx_0,
        DNX_DATA_MAX_MDB_EM_VMV_NOF_VALUES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    *encoding = ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].encoding_map[encoding_map_idx_0].encoding;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].encoding_map[encoding_map_idx_0].encoding,
        "mdb_em_db[%d]->vmv_info[%d].encoding_map[%d].encoding",
        unit, vmv_info_idx_0, encoding_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_VMV_INFO_ENCODING_MAP_ENCODING_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_encoding_map_size_set(int unit, uint32 vmv_info_idx_0, uint32 encoding_map_idx_0, uint8 size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vmv_info_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        encoding_map_idx_0,
        DNX_DATA_MAX_MDB_EM_VMV_NOF_VALUES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].encoding_map[encoding_map_idx_0].size,
        size,
        uint8,
        0,
        "mdb_em_db_vmv_info_encoding_map_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        MDB_EM_MODULE_ID,
        &size,
        "mdb_em_db[%d]->vmv_info[%d].encoding_map[%d].size",
        unit, vmv_info_idx_0, encoding_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_VMV_INFO_ENCODING_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_encoding_map_size_get(int unit, uint32 vmv_info_idx_0, uint32 encoding_map_idx_0, uint8 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vmv_info_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        MDB_EM_MODULE_ID,
        size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        encoding_map_idx_0,
        DNX_DATA_MAX_MDB_EM_VMV_NOF_VALUES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    *size = ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].encoding_map[encoding_map_idx_0].size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].encoding_map[encoding_map_idx_0].size,
        "mdb_em_db[%d]->vmv_info[%d].encoding_map[%d].size",
        unit, vmv_info_idx_0, encoding_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_VMV_INFO_ENCODING_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_app_id_size_set(int unit, uint32 vmv_info_idx_0, uint32 app_id_size_idx_0, uint8 app_id_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vmv_info_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        app_id_size_idx_0,
        DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].app_id_size[app_id_size_idx_0],
        app_id_size,
        uint8,
        0,
        "mdb_em_db_vmv_info_app_id_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        MDB_EM_MODULE_ID,
        &app_id_size,
        "mdb_em_db[%d]->vmv_info[%d].app_id_size[%d]",
        unit, vmv_info_idx_0, app_id_size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_VMV_INFO_APP_ID_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_app_id_size_get(int unit, uint32 vmv_info_idx_0, uint32 app_id_size_idx_0, uint8 *app_id_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vmv_info_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        MDB_EM_MODULE_ID,
        app_id_size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        app_id_size_idx_0,
        DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    *app_id_size = ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].app_id_size[app_id_size_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].app_id_size[app_id_size_idx_0],
        "mdb_em_db[%d]->vmv_info[%d].app_id_size[%d]",
        unit, vmv_info_idx_0, app_id_size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_VMV_INFO_APP_ID_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_max_payload_size_set(int unit, uint32 vmv_info_idx_0, uint32 max_payload_size_idx_0, int max_payload_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vmv_info_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        max_payload_size_idx_0,
        DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].max_payload_size[max_payload_size_idx_0],
        max_payload_size,
        int,
        0,
        "mdb_em_db_vmv_info_max_payload_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        MDB_EM_MODULE_ID,
        &max_payload_size,
        "mdb_em_db[%d]->vmv_info[%d].max_payload_size[%d]",
        unit, vmv_info_idx_0, max_payload_size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_VMV_INFO_MAX_PAYLOAD_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_max_payload_size_get(int unit, uint32 vmv_info_idx_0, uint32 max_payload_size_idx_0, int *max_payload_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        vmv_info_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        MDB_EM_MODULE_ID,
        max_payload_size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        max_payload_size_idx_0,
        DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    *max_payload_size = ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].max_payload_size[max_payload_size_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[vmv_info_idx_0].max_payload_size[max_payload_size_idx_0],
        "mdb_em_db[%d]->vmv_info[%d].max_payload_size[%d]",
        unit, vmv_info_idx_0, max_payload_size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_VMV_INFO_MAX_PAYLOAD_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_mact_stamp_set(int unit, uint32 mact_stamp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        MDB_EM_MODULE_ID,
        ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->mact_stamp,
        mact_stamp,
        uint32,
        0,
        "mdb_em_db_mact_stamp_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        MDB_EM_MODULE_ID,
        &mact_stamp,
        "mdb_em_db[%d]->mact_stamp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_MACT_STAMP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_mact_stamp_get(int unit, uint32 *mact_stamp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_EM_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_EM_MODULE_ID,
        sw_state_roots_array[unit][MDB_EM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        MDB_EM_MODULE_ID,
        mact_stamp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_EM_MODULE_ID);

    *mact_stamp = ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->mact_stamp;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        MDB_EM_MODULE_ID,
        &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->mact_stamp,
        "mdb_em_db[%d]->mact_stamp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_EM_MODULE_ID,
        mdb_em_db_info,
        MDB_EM_DB_MACT_STAMP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





mdb_em_db_cbs mdb_em_db = 	{
	
	mdb_em_db_is_init,
	mdb_em_db_init,
		{
		
		mdb_em_db_shadow_em_db_create,
		mdb_em_db_shadow_em_db_find,
		mdb_em_db_shadow_em_db_insert,
		mdb_em_db_shadow_em_db_get_next,
		mdb_em_db_shadow_em_db_clear,
		mdb_em_db_shadow_em_db_delete,
		mdb_em_db_shadow_em_db_delete_all}
	,
		{
		
			{
			
			mdb_em_db_vmv_info_value_set,
			mdb_em_db_vmv_info_value_get}
		,
			{
			
			mdb_em_db_vmv_info_size_set,
			mdb_em_db_vmv_info_size_get}
		,
			{
			
				{
				
				mdb_em_db_vmv_info_encoding_map_encoding_set,
				mdb_em_db_vmv_info_encoding_map_encoding_get}
			,
				{
				
				mdb_em_db_vmv_info_encoding_map_size_set,
				mdb_em_db_vmv_info_encoding_map_size_get}
			}
		,
			{
			
			mdb_em_db_vmv_info_app_id_size_set,
			mdb_em_db_vmv_info_app_id_size_get}
		,
			{
			
			mdb_em_db_vmv_info_max_payload_size_set,
			mdb_em_db_vmv_info_max_payload_size_get}
		}
	,
		{
		
		mdb_em_db_mact_stamp_set,
		mdb_em_db_mact_stamp_get}
	}
;
#undef BSL_LOG_MODULE


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
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_mapper_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_mapper_diagnostic.h>



dnx_field_tcam_access_mapper_t* dnx_field_tcam_access_mapper_sw_dummy = NULL;



int
dnx_field_tcam_access_mapper_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]),
        "dnx_field_tcam_access_mapper_sw[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_tcam_access_mapper_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_mapper_sw_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]),
        "dnx_field_tcam_access_mapper_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_field_tcam_access_mapper_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash,
        sw_state_htbl_t*,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_alloc");

    for(uint32 key_2_entry_id_hash_idx_0 = 0;
         key_2_entry_id_hash_idx_0 < dnx_data_device.general.nof_cores_get(unit);
         key_2_entry_id_hash_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0],
        sw_state_htbl_t,
        dnx_data_field.tcam.nof_banks_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash,
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_entry_id_hash",
        unit,
        dnx_data_field.tcam.nof_banks_get(unit) * sizeof(sw_state_htbl_t) / sizeof(*((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash)+(dnx_data_field.tcam.nof_banks_get(unit) * sizeof(sw_state_htbl_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_ENTRY_ID_HASH_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_create(int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    SW_STATE_HTB_CREATE(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        init_info,
        dnx_field_tcam_access_mapper_hash_key,
        uint32,
        TRUE,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_entry_id_hash[%d][%d]",
        unit, key_2_entry_id_hash_idx_0, key_2_entry_id_hash_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_ENTRY_ID_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_find(int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, const dnx_field_tcam_access_mapper_hash_key *key, uint32 *data_index, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    SW_STATE_HTB_FIND(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        key,
        data_index,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_entry_id_hash[%d][%d]",
        unit, key_2_entry_id_hash_idx_0, key_2_entry_id_hash_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_ENTRY_ID_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_insert(int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, const dnx_field_tcam_access_mapper_hash_key *key, const uint32 *data_index, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    SW_STATE_HTB_INSERT(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        key,
        data_index,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_entry_id_hash[%d][%d]",
        unit, key_2_entry_id_hash_idx_0, key_2_entry_id_hash_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_ENTRY_ID_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_get_next(int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, SW_STATE_HASH_TABLE_ITER *iter, const dnx_field_tcam_access_mapper_hash_key *key, const uint32 *data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    SW_STATE_HTB_GET_NEXT(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        iter,
        key,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_entry_id_hash[%d][%d]",
        unit, key_2_entry_id_hash_idx_0, key_2_entry_id_hash_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_ENTRY_ID_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_clear(int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    SW_STATE_HTB_CLEAR(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_entry_id_hash[%d][%d]",
        unit, key_2_entry_id_hash_idx_0, key_2_entry_id_hash_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_ENTRY_ID_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_delete(int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, const dnx_field_tcam_access_mapper_hash_key *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    SW_STATE_HTB_DELETE(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_entry_id_hash[%d][%d]",
        unit, key_2_entry_id_hash_idx_0, key_2_entry_id_hash_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_ENTRY_ID_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_insert_at_index(int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, const dnx_field_tcam_access_mapper_hash_key *key, uint32 data_idx, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_INSERT_AT_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    SW_STATE_HTB_INSERT_AT_INDEX(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        key,
        data_idx,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_AT_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT_AT_INDEX,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_entry_id_hash[%d][%d]",
        unit, key_2_entry_id_hash_idx_0, key_2_entry_id_hash_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_ENTRY_ID_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_delete_by_index(int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, uint32 data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_DELETE_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    SW_STATE_HTB_DELETE_BY_INDEX(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE_BY_INDEX,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_entry_id_hash[%d][%d]",
        unit, key_2_entry_id_hash_idx_0, key_2_entry_id_hash_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_ENTRY_ID_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_get_by_index(int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, uint32 data_index, dnx_field_tcam_access_mapper_hash_key *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_GET_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    SW_STATE_HTB_GET_BY_INDEX(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        data_index,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_BY_INDEX,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[key_2_entry_id_hash_idx_0][key_2_entry_id_hash_idx_1],
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_entry_id_hash[%d][%d]",
        unit, key_2_entry_id_hash_idx_0, key_2_entry_id_hash_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_ENTRY_ID_HASH_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_field_tcam_access_mapper_sw_cbs dnx_field_tcam_access_mapper_sw = 	{
	
	dnx_field_tcam_access_mapper_sw_is_init,
	dnx_field_tcam_access_mapper_sw_init,
		{
		
		dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_alloc,
		dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_create,
		dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_find,
		dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_insert,
		dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_get_next,
		dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_clear,
		dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_delete,
		dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_insert_at_index,
		dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_delete_by_index,
		dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_get_by_index}
	}
;
#undef BSL_LOG_MODULE

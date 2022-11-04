
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_access_layout.h>





int
dnx_field_tcam_access_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW,
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
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_tcam_access_t,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_field_tcam_access_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]),
        "dnx_field_tcam_access_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]),
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_location_hash_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        sw_state_index_htb_t,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_entry_location_hash_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        "dnx_field_tcam_access_sw[%d]->entry_location_hash",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(sw_state_index_htb_t) / sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_location_hash_create(int unit, uint32 entry_location_hash_idx_0, sw_state_htb_create_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        entry_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH_CREATE);

    SW_STATE_INDEX_HTB_RH_CREATE(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        init_info,
        sizeof(uint32),
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_RH_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTB_RHCREATE,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_location_hash_find(int unit, uint32 entry_location_hash_idx_0, const uint32 *key, uint32 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        entry_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH);

    SW_STATE_INDEX_HTB_RH_FIND(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_RH_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTB_RHFIND,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_location_hash_get_next(int unit, uint32 entry_location_hash_idx_0, uint32 *iter, uint32 *key, uint32 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        entry_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH);

    SW_STATE_INDEX_HTB_RH_GET_NEXT(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_RH_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTB_RHGET_NEXT,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_location_hash_insert(int unit, uint32 entry_location_hash_idx_0, const uint32 *key, uint32 *data_idx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        entry_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH);

    SW_STATE_INDEX_HTB_RH_INSERT(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        key,
        data_idx);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_RH_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTB_RHINSERT,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_location_hash_delete(int unit, uint32 entry_location_hash_idx_0, const uint32 *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        entry_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH);

    SW_STATE_INDEX_HTB_RH_DELETE(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_RH_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTB_RHDELETE,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_location_hash_print(int unit, uint32 entry_location_hash_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        entry_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH);

    SW_STATE_INDEX_HTB_RH_PRINT(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_RH_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTB_RHPRINT,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_location_hash_traverse(int unit, uint32 entry_location_hash_idx_0, sw_state_htb_traverse_cb traverse_func, void *user_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        SW_STATE_FUNC_TRAVERSE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        entry_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH);

    SW_STATE_INDEX_HTB_RH_TRAVERSE(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        traverse_func,
        user_data);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_RH_TRAVERSE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTB_RHTRAVERSE,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_location_hash_insert_at_index(int unit, uint32 entry_location_hash_idx_0, const uint32 *key, uint32 data_idx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        SW_STATE_FUNC_INSERT_AT_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        entry_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH);

    SW_STATE_INDEX_HTB_RH_INSERT_AT_INDEX(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        key,
        data_idx);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_RH_INSERT_AT_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTB_RHINSERT_AT_INDEX,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_location_hash_delete_by_index(int unit, uint32 entry_location_hash_idx_0, uint32 data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        SW_STATE_FUNC_DELETE_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        entry_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH);

    SW_STATE_INDEX_HTB_RH_DELETE_BY_INDEX(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_RH_DELETE_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTB_RHDELETE_BY_INDEX,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_location_hash_get_by_index(int unit, uint32 entry_location_hash_idx_0, uint32 data_index, uint32 *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        SW_STATE_FUNC_GET_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash,
        entry_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH);

    SW_STATE_INDEX_HTB_RH_GET_BY_INDEX(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        data_index,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_RH_GET_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTB_RHGET_BY_INDEX,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_location_hash[entry_location_hash_idx_0],
        "dnx_field_tcam_access_sw[%d]->entry_location_hash[%d]",
        unit, entry_location_hash_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_RH,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        dnx_field_tcam_access_v_bit_t,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_valid_bmp_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        "dnx_field_tcam_access_sw[%d]->valid_bmp",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(dnx_field_tcam_access_v_bit_t) / sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_alloc_bitmap(int unit, uint32 valid_bmp_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        dnx_data_field.tcam.tcam_banks_size_get(unit),
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_valid_bmp_v_bit_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0,
        dnx_data_field.tcam.tcam_banks_size_get(unit));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_set(int unit, uint32 valid_bmp_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_clear(int unit, uint32 valid_bmp_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_get(int unit, uint32 valid_bmp_idx_0, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_read(int unit, uint32 valid_bmp_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
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
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_write(int unit, uint32 valid_bmp_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
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
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_and(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_or(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_xor(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_remove(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_negate(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_clear(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_set(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_null(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_test(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_eq(int unit, uint32 valid_bmp_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
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
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_count(int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp,
        valid_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->valid_bmp[valid_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->valid_bmp[%d].v_bit",
        unit, valid_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        dnx_field_tcam_access_v_bit_t,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_entry_in_use_bmp_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(dnx_field_tcam_access_v_bit_t) / sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_alloc_bitmap(int unit, uint32 entry_in_use_bmp_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        dnx_data_field.tcam.tcam_banks_size_get(unit),
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0,
        dnx_data_field.tcam.tcam_banks_size_get(unit));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_set(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_clear(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_get(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_read(int unit, uint32 entry_in_use_bmp_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
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
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_write(int unit, uint32 entry_in_use_bmp_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
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
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_and(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_or(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_xor(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_remove(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_negate(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_clear(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_set(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_null(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_test(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_eq(int unit, uint32 entry_in_use_bmp_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
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
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_count(int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp,
        entry_in_use_bmp_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
         0,
         ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->entry_in_use_bmp[entry_in_use_bmp_idx_0].v_bit,
        "dnx_field_tcam_access_sw[%d]->entry_in_use_bmp[%d].v_bit",
        unit, entry_in_use_bmp_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_set(int unit, uint32 fg_params_idx_0, CONST dnx_field_tcam_access_fg_params_t *fg_params)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0],
        fg_params,
        dnx_field_tcam_access_fg_params_t,
        0,
        "dnx_field_tcam_access_sw_fg_params_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        fg_params,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d]",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_get(int unit, uint32 fg_params_idx_0, dnx_field_tcam_access_fg_params_t *fg_params)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        fg_params);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        DNX_SW_STATE_DIAG_READ);

    *fg_params = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0],
        "dnx_field_tcam_access_sw[%d]->fg_params[%d]",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_alloc(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        dnx_field_tcam_access_fg_params_t,
        dnx_data_field.group.nof_fgs_get(unit),
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_fg_params_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        "dnx_field_tcam_access_sw[%d]->fg_params",
        unit,
        dnx_data_field.group.nof_fgs_get(unit) * sizeof(dnx_field_tcam_access_fg_params_t) / sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[def_val_idx[0]].dt_bank_id,
        DNX_FIELD_TCAM_BANK_ID_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[def_val_idx[0]].stage,
        DNX_FIELD_TCAM_STAGE_INVALID);

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
dnx_field_tcam_access_sw_fg_params_dt_bank_id_set(int unit, uint32 fg_params_idx_0, int dt_bank_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].dt_bank_id,
        dt_bank_id,
        int,
        0,
        "dnx_field_tcam_access_sw_fg_params_dt_bank_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        &dt_bank_id,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].dt_bank_id",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_DT_BANK_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_dt_bank_id_get(int unit, uint32 fg_params_idx_0, int *dt_bank_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        dt_bank_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        DNX_SW_STATE_DIAG_READ);

    *dt_bank_id = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].dt_bank_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].dt_bank_id,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].dt_bank_id",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_DT_BANK_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_stage_set(int unit, uint32 fg_params_idx_0, dnx_field_tcam_stage_e stage)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].stage,
        stage,
        dnx_field_tcam_stage_e,
        0,
        "dnx_field_tcam_access_sw_fg_params_stage_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        &stage,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].stage",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_STAGE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_stage_get(int unit, uint32 fg_params_idx_0, dnx_field_tcam_stage_e *stage)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        stage);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        DNX_SW_STATE_DIAG_READ);

    *stage = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].stage;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].stage,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].stage",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_STAGE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_set(int unit, uint32 fg_params_idx_0, dnx_field_tcam_bank_allocation_mode_e bank_allocation_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].bank_allocation_mode,
        bank_allocation_mode,
        dnx_field_tcam_bank_allocation_mode_e,
        0,
        "dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        &bank_allocation_mode,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].bank_allocation_mode",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_BANK_ALLOCATION_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_get(int unit, uint32 fg_params_idx_0, dnx_field_tcam_bank_allocation_mode_e *bank_allocation_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        bank_allocation_mode);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        DNX_SW_STATE_DIAG_READ);

    *bank_allocation_mode = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].bank_allocation_mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].bank_allocation_mode,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].bank_allocation_mode",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_BANK_ALLOCATION_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_key_size_set(int unit, uint32 fg_params_idx_0, uint32 key_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].key_size,
        key_size,
        uint32,
        0,
        "dnx_field_tcam_access_sw_fg_params_key_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        &key_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].key_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_KEY_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_key_size_get(int unit, uint32 fg_params_idx_0, uint32 *key_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        key_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        DNX_SW_STATE_DIAG_READ);

    *key_size = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].key_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].key_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].key_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_KEY_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_action_size_set(int unit, uint32 fg_params_idx_0, uint32 action_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].action_size,
        action_size,
        uint32,
        0,
        "dnx_field_tcam_access_sw_fg_params_action_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        &action_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].action_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_ACTION_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_action_size_get(int unit, uint32 fg_params_idx_0, uint32 *action_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        action_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        DNX_SW_STATE_DIAG_READ);

    *action_size = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].action_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].action_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].action_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_ACTION_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_actual_action_size_set(int unit, uint32 fg_params_idx_0, uint32 actual_action_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].actual_action_size,
        actual_action_size,
        uint32,
        0,
        "dnx_field_tcam_access_sw_fg_params_actual_action_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        &actual_action_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].actual_action_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_ACTUAL_ACTION_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_actual_action_size_get(int unit, uint32 fg_params_idx_0, uint32 *actual_action_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        actual_action_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        DNX_SW_STATE_DIAG_READ);

    *actual_action_size = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].actual_action_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].actual_action_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].actual_action_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_ACTUAL_ACTION_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_prefix_size_set(int unit, uint32 fg_params_idx_0, uint32 prefix_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].prefix_size,
        prefix_size,
        uint32,
        0,
        "dnx_field_tcam_access_sw_fg_params_prefix_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        &prefix_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].prefix_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_PREFIX_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_prefix_size_get(int unit, uint32 fg_params_idx_0, uint32 *prefix_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        prefix_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        DNX_SW_STATE_DIAG_READ);

    *prefix_size = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].prefix_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].prefix_size,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].prefix_size",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_PREFIX_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_direct_table_set(int unit, uint32 fg_params_idx_0, uint8 direct_table)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].direct_table,
        direct_table,
        uint8,
        0,
        "dnx_field_tcam_access_sw_fg_params_direct_table_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        &direct_table,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].direct_table",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_DIRECT_TABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_direct_table_get(int unit, uint32 fg_params_idx_0, uint8 *direct_table)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        direct_table);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        DNX_SW_STATE_DIAG_READ);

    *direct_table = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].direct_table;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].direct_table,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].direct_table",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_DIRECT_TABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_set(int unit, uint32 fg_params_idx_0, dnx_field_tcam_context_sharing_handlers_get_p context_sharing_handlers_cb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].context_sharing_handlers_cb,
        context_sharing_handlers_cb,
        dnx_field_tcam_context_sharing_handlers_get_p,
        0,
        "dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        &context_sharing_handlers_cb,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].context_sharing_handlers_cb",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_CONTEXT_SHARING_HANDLERS_CB_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_get(int unit, uint32 fg_params_idx_0, dnx_field_tcam_context_sharing_handlers_get_p *context_sharing_handlers_cb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params,
        fg_params_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        context_sharing_handlers_cb);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        DNX_SW_STATE_DIAG_READ);

    *context_sharing_handlers_cb = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].context_sharing_handlers_cb;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_params[fg_params_idx_0].context_sharing_handlers_cb,
        "dnx_field_tcam_access_sw[%d]->fg_params[%d].context_sharing_handlers_cb",
        unit, fg_params_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_CONTEXT_SHARING_HANDLERS_CB_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_prefix_per_core_set(int unit, uint32 fg_prefix_per_core_idx_0, uint32 fg_prefix_per_core_idx_1, uint32 fg_prefix_per_core)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core,
        fg_prefix_per_core_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core[fg_prefix_per_core_idx_0],
        fg_prefix_per_core_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core[fg_prefix_per_core_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core[fg_prefix_per_core_idx_0][fg_prefix_per_core_idx_1],
        fg_prefix_per_core,
        uint32,
        0,
        "dnx_field_tcam_access_sw_fg_prefix_per_core_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        &fg_prefix_per_core,
        "dnx_field_tcam_access_sw[%d]->fg_prefix_per_core[%d][%d]",
        unit, fg_prefix_per_core_idx_0, fg_prefix_per_core_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PREFIX_PER_CORE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_prefix_per_core_get(int unit, uint32 fg_prefix_per_core_idx_0, uint32 fg_prefix_per_core_idx_1, uint32 *fg_prefix_per_core)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core,
        fg_prefix_per_core_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        fg_prefix_per_core);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core[fg_prefix_per_core_idx_0],
        fg_prefix_per_core_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core[fg_prefix_per_core_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        DNX_SW_STATE_DIAG_READ);

    *fg_prefix_per_core = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core[fg_prefix_per_core_idx_0][fg_prefix_per_core_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core[fg_prefix_per_core_idx_0][fg_prefix_per_core_idx_1],
        "dnx_field_tcam_access_sw[%d]->fg_prefix_per_core[%d][%d]",
        unit, fg_prefix_per_core_idx_0, fg_prefix_per_core_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PREFIX_PER_CORE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_fg_prefix_per_core_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        dnx_data_field.group.nof_fgs_get(unit),
        dnx_data_device.general.nof_cores_get(unit),
        sizeof(uint32*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core,
        uint32*,
        dnx_data_field.group.nof_fgs_get(unit),
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_fg_prefix_per_core_alloc");

    for(uint32 fg_prefix_per_core_idx_0 = 0;
         fg_prefix_per_core_idx_0 < dnx_data_field.group.nof_fgs_get(unit);
         fg_prefix_per_core_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core[fg_prefix_per_core_idx_0],
        uint32,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_sw_fg_prefix_per_core_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core,
        "dnx_field_tcam_access_sw[%d]->fg_prefix_per_core",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(uint32) / sizeof(*((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->fg_prefix_per_core)+(dnx_data_device.general.nof_cores_get(unit) * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_FG_PREFIX_PER_CORE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_cache_install_in_progress_set(int unit, uint8 cache_install_in_progress)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__CACHE_INSTALL_IN_PROGRESS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__CACHE_INSTALL_IN_PROGRESS,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__CACHE_INSTALL_IN_PROGRESS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__CACHE_INSTALL_IN_PROGRESS,
        ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->cache_install_in_progress,
        cache_install_in_progress,
        uint8,
        0,
        "dnx_field_tcam_access_sw_cache_install_in_progress_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__CACHE_INSTALL_IN_PROGRESS,
        &cache_install_in_progress,
        "dnx_field_tcam_access_sw[%d]->cache_install_in_progress",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__CACHE_INSTALL_IN_PROGRESS,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_CACHE_INSTALL_IN_PROGRESS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_access_sw_cache_install_in_progress_get(int unit, uint8 *cache_install_in_progress)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__CACHE_INSTALL_IN_PROGRESS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__CACHE_INSTALL_IN_PROGRESS,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__CACHE_INSTALL_IN_PROGRESS,
        cache_install_in_progress);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__CACHE_INSTALL_IN_PROGRESS,
        DNX_SW_STATE_DIAG_READ);

    *cache_install_in_progress = ((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->cache_install_in_progress;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__CACHE_INSTALL_IN_PROGRESS,
        &((dnx_field_tcam_access_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID])->cache_install_in_progress,
        "dnx_field_tcam_access_sw[%d]->cache_install_in_progress",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__CACHE_INSTALL_IN_PROGRESS,
        dnx_field_tcam_access_sw_info,
        DNX_FIELD_TCAM_ACCESS_SW_CACHE_INSTALL_IN_PROGRESS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



const char *
dnx_field_tcam_stage_e_get_name(dnx_field_tcam_stage_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_TCAM_STAGE_INVALID", value, DNX_FIELD_TCAM_STAGE_INVALID);

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
		dnx_field_tcam_access_sw_entry_location_hash_get_next,
		dnx_field_tcam_access_sw_entry_location_hash_insert,
		dnx_field_tcam_access_sw_entry_location_hash_delete,
		dnx_field_tcam_access_sw_entry_location_hash_print,
		dnx_field_tcam_access_sw_entry_location_hash_traverse,
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
			
			dnx_field_tcam_access_sw_fg_params_dt_bank_id_set,
			dnx_field_tcam_access_sw_fg_params_dt_bank_id_get}
		,
			{
			
			dnx_field_tcam_access_sw_fg_params_stage_set,
			dnx_field_tcam_access_sw_fg_params_stage_get}
		,
			{
			
			dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_set,
			dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_get}
		,
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
			
			dnx_field_tcam_access_sw_fg_params_prefix_size_set,
			dnx_field_tcam_access_sw_fg_params_prefix_size_get}
		,
			{
			
			dnx_field_tcam_access_sw_fg_params_direct_table_set,
			dnx_field_tcam_access_sw_fg_params_direct_table_get}
		,
			{
			
			dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_set,
			dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_get}
		}
	,
		{
		
		dnx_field_tcam_access_sw_fg_prefix_per_core_set,
		dnx_field_tcam_access_sw_fg_prefix_per_core_get,
		dnx_field_tcam_access_sw_fg_prefix_per_core_alloc}
	,
		{
		
		dnx_field_tcam_access_sw_cache_install_in_progress_set,
		dnx_field_tcam_access_sw_cache_install_in_progress_get}
	}
;
#undef BSL_LOG_MODULE

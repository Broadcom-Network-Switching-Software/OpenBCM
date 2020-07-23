
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
#include <soc/dnx/swstate/auto_generated/access/dnx_fifodma_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_fifodma_diagnostic.h>



fifodma_db_t* dnx_fifodma_info_dummy = NULL;



int
dnx_fifodma_info_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]),
        "dnx_fifodma_info[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_FIFODMA_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        fifodma_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_fifodma_info_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]),
        "dnx_fifodma_info[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(fifodma_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        fifodma_info_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_fifodma_info_entry_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        "dnx_fifodma_info[%d]->entry",
        unit,
        nof_instances_to_alloc_0 * sizeof(fifodma_info_t) / sizeof(*((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_max_entries_set(int unit, uint32 entry_idx_0, uint32 max_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].max_entries,
        max_entries,
        uint32,
        0,
        "dnx_fifodma_info_entry_max_entries_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &max_entries,
        "dnx_fifodma_info[%d]->entry[%d].max_entries",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_MAX_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_max_entries_get(int unit, uint32 entry_idx_0, uint32 *max_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        max_entries);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *max_entries = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].max_entries;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].max_entries,
        "dnx_fifodma_info[%d]->entry[%d].max_entries",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_MAX_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_mem_set(int unit, uint32 entry_idx_0, int is_mem)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_mem,
        is_mem,
        int,
        0,
        "dnx_fifodma_info_entry_is_mem_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &is_mem,
        "dnx_fifodma_info[%d]->entry[%d].is_mem",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_MEM_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_mem_get(int unit, uint32 entry_idx_0, int *is_mem)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        is_mem);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *is_mem = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_mem;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_mem,
        "dnx_fifodma_info[%d]->entry[%d].is_mem",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_MEM_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_mem_set(int unit, uint32 entry_idx_0, int mem)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].mem,
        mem,
        int,
        0,
        "dnx_fifodma_info_entry_mem_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &mem,
        "dnx_fifodma_info[%d]->entry[%d].mem",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_MEM_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_mem_get(int unit, uint32 entry_idx_0, int *mem)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        mem);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *mem = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].mem;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].mem,
        "dnx_fifodma_info[%d]->entry[%d].mem",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_MEM_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_reg_set(int unit, uint32 entry_idx_0, int reg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].reg,
        reg,
        int,
        0,
        "dnx_fifodma_info_entry_reg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &reg,
        "dnx_fifodma_info[%d]->entry[%d].reg",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_REG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_reg_get(int unit, uint32 entry_idx_0, int *reg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        reg);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *reg = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].reg;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].reg,
        "dnx_fifodma_info[%d]->entry[%d].reg",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_REG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_array_index_set(int unit, uint32 entry_idx_0, int array_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].array_index,
        array_index,
        int,
        0,
        "dnx_fifodma_info_entry_array_index_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &array_index,
        "dnx_fifodma_info[%d]->entry[%d].array_index",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_ARRAY_INDEX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_array_index_get(int unit, uint32 entry_idx_0, int *array_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        array_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *array_index = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].array_index;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].array_index,
        "dnx_fifodma_info[%d]->entry[%d].array_index",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_ARRAY_INDEX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_copyno_set(int unit, uint32 entry_idx_0, int copyno)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].copyno,
        copyno,
        int,
        0,
        "dnx_fifodma_info_entry_copyno_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &copyno,
        "dnx_fifodma_info[%d]->entry[%d].copyno",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_COPYNO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_copyno_get(int unit, uint32 entry_idx_0, int *copyno)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        copyno);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *copyno = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].copyno;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].copyno,
        "dnx_fifodma_info[%d]->entry[%d].copyno",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_COPYNO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_force_entry_size_set(int unit, uint32 entry_idx_0, int force_entry_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].force_entry_size,
        force_entry_size,
        int,
        0,
        "dnx_fifodma_info_entry_force_entry_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &force_entry_size,
        "dnx_fifodma_info[%d]->entry[%d].force_entry_size",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_FORCE_ENTRY_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_force_entry_size_get(int unit, uint32 entry_idx_0, int *force_entry_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        force_entry_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *force_entry_size = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].force_entry_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].force_entry_size,
        "dnx_fifodma_info[%d]->entry[%d].force_entry_size",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_FORCE_ENTRY_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_threshold_set(int unit, uint32 entry_idx_0, int threshold)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].threshold,
        threshold,
        int,
        0,
        "dnx_fifodma_info_entry_threshold_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &threshold,
        "dnx_fifodma_info[%d]->entry[%d].threshold",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_THRESHOLD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_threshold_get(int unit, uint32 entry_idx_0, int *threshold)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        threshold);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *threshold = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].threshold;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].threshold,
        "dnx_fifodma_info[%d]->entry[%d].threshold",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_THRESHOLD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_timeout_set(int unit, uint32 entry_idx_0, int timeout)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].timeout,
        timeout,
        int,
        0,
        "dnx_fifodma_info_entry_timeout_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &timeout,
        "dnx_fifodma_info[%d]->entry[%d].timeout",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_TIMEOUT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_timeout_get(int unit, uint32 entry_idx_0, int *timeout)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        timeout);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *timeout = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].timeout;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].timeout,
        "dnx_fifodma_info[%d]->entry[%d].timeout",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_TIMEOUT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_poll_set(int unit, uint32 entry_idx_0, int is_poll)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_poll,
        is_poll,
        int,
        0,
        "dnx_fifodma_info_entry_is_poll_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &is_poll,
        "dnx_fifodma_info[%d]->entry[%d].is_poll",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_POLL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_poll_get(int unit, uint32 entry_idx_0, int *is_poll)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        is_poll);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *is_poll = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_poll;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_poll,
        "dnx_fifodma_info[%d]->entry[%d].is_poll",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_POLL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_entry_size_set(int unit, uint32 entry_idx_0, uint32 entry_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].entry_size,
        entry_size,
        uint32,
        0,
        "dnx_fifodma_info_entry_entry_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &entry_size,
        "dnx_fifodma_info[%d]->entry[%d].entry_size",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_ENTRY_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_entry_size_get(int unit, uint32 entry_idx_0, uint32 *entry_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        entry_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *entry_size = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].entry_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].entry_size,
        "dnx_fifodma_info[%d]->entry[%d].entry_size",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_ENTRY_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_alloced_set(int unit, uint32 entry_idx_0, int is_alloced)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_alloced,
        is_alloced,
        int,
        0,
        "dnx_fifodma_info_entry_is_alloced_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &is_alloced,
        "dnx_fifodma_info[%d]->entry[%d].is_alloced",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_ALLOCED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_alloced_get(int unit, uint32 entry_idx_0, int *is_alloced)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        is_alloced);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *is_alloced = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_alloced;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_alloced,
        "dnx_fifodma_info[%d]->entry[%d].is_alloced",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_ALLOCED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_started_set(int unit, uint32 entry_idx_0, int is_started)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_started,
        is_started,
        int,
        0,
        "dnx_fifodma_info_entry_is_started_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &is_started,
        "dnx_fifodma_info[%d]->entry[%d].is_started",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_STARTED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_started_get(int unit, uint32 entry_idx_0, int *is_started)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        is_started);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *is_started = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_started;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_started,
        "dnx_fifodma_info[%d]->entry[%d].is_started",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_STARTED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_paused_set(int unit, uint32 entry_idx_0, int is_paused)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_paused,
        is_paused,
        int,
        0,
        "dnx_fifodma_info_entry_is_paused_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &is_paused,
        "dnx_fifodma_info[%d]->entry[%d].is_paused",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_PAUSED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_paused_get(int unit, uint32 entry_idx_0, int *is_paused)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        is_paused);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *is_paused = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_paused;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_paused,
        "dnx_fifodma_info[%d]->entry[%d].is_paused",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_PAUSED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_handler_get_cb(int unit, uint32 entry_idx_0, soc_dnx_fifodma_interrupt_handler* cb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET_CB,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    SW_STATE_CB_DB_GET_CB(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].handler,
        cb,
        soc_dnx_fifodma_interrupt_handler_get_cb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_CB_DB_GET_CB_LOGGING,
        BSL_LS_SWSTATEDNX_CB_DBGET_CB,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].handler,
        "dnx_fifodma_info[%d]->entry[%d].handler",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HANDLER_INFO,
        DNX_SW_STATE_DIAG_CB_DB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_handler_register_cb(int unit, uint32 entry_idx_0, char *name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_REGISTER_CB,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    SW_STATE_CB_DB_REGISTER_CB(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].handler,
        name,
        soc_dnx_fifodma_interrupt_handler_get_cb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_CB_DB_REGISTER_CB_LOGGING,
        BSL_LS_SWSTATEDNX_CB_DBREGISTER_CB,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].handler,
        "dnx_fifodma_info[%d]->entry[%d].handler",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HANDLER_INFO,
        DNX_SW_STATE_DIAG_CB_DB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_handler_unregister_cb(int unit, uint32 entry_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_UNREGISTER_CB,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    SW_STATE_CB_DB_UNREGISTER_CB(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].handler);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_CB_DB_UNREGISTER_CB_LOGGING,
        BSL_LS_SWSTATEDNX_CB_DBUNREGISTER_CB,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].handler,
        "dnx_fifodma_info[%d]->entry[%d].handler",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HANDLER_INFO,
        DNX_SW_STATE_DIAG_CB_DB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_last_entry_id_set(int unit, uint32 entry_idx_0, int last_entry_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].last_entry_id,
        last_entry_id,
        int,
        0,
        "dnx_fifodma_info_entry_last_entry_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &last_entry_id,
        "dnx_fifodma_info[%d]->entry[%d].last_entry_id",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_LAST_ENTRY_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_last_entry_id_get(int unit, uint32 entry_idx_0, int *last_entry_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        last_entry_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *last_entry_id = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].last_entry_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].last_entry_id,
        "dnx_fifodma_info[%d]->entry[%d].last_entry_id",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_LAST_ENTRY_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_host_buff_lo_set(int unit, uint32 entry_idx_0, uint32 host_buff_lo)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].host_buff_lo,
        host_buff_lo,
        uint32,
        0,
        "dnx_fifodma_info_entry_host_buff_lo_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &host_buff_lo,
        "dnx_fifodma_info[%d]->entry[%d].host_buff_lo",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HOST_BUFF_LO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_host_buff_lo_get(int unit, uint32 entry_idx_0, uint32 *host_buff_lo)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        host_buff_lo);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *host_buff_lo = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].host_buff_lo;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].host_buff_lo,
        "dnx_fifodma_info[%d]->entry[%d].host_buff_lo",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HOST_BUFF_LO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_host_buff_hi_set(int unit, uint32 entry_idx_0, uint32 host_buff_hi)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].host_buff_hi,
        host_buff_hi,
        uint32,
        0,
        "dnx_fifodma_info_entry_host_buff_hi_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIFODMA_MODULE_ID,
        &host_buff_hi,
        "dnx_fifodma_info[%d]->entry[%d].host_buff_hi",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HOST_BUFF_HI_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_host_buff_hi_get(int unit, uint32 entry_idx_0, uint32 *host_buff_hi)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIFODMA_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        host_buff_hi);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIFODMA_MODULE_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIFODMA_MODULE_ID);

    *host_buff_hi = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].host_buff_hi;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIFODMA_MODULE_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].host_buff_hi,
        "dnx_fifodma_info[%d]->entry[%d].host_buff_hi",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIFODMA_MODULE_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HOST_BUFF_HI_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




shr_error_e
soc_dnx_fifodma_interrupt_handler_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, soc_dnx_fifodma_interrupt_handler * cb)
{
    if (!strcmp(cb_db->function_name,"dnx_fifodma_interrupt_handler_example"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_fifodma_interrupt_handler_example;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"dnx_olp_dma_handler"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_olp_dma_handler;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"dnx_oam_oamp_dma_event_handler"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_oam_oamp_dma_event_handler;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"dnx_oam_oamp_stat_dma_event_handler"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_oam_oamp_stat_dma_event_handler;
        }
        return _SHR_E_NONE;
    }

    return _SHR_E_NOT_FOUND;
}





dnx_fifodma_info_cbs dnx_fifodma_info = 	{
	
	dnx_fifodma_info_is_init,
	dnx_fifodma_info_init,
		{
		
		dnx_fifodma_info_entry_alloc,
			{
			
			dnx_fifodma_info_entry_max_entries_set,
			dnx_fifodma_info_entry_max_entries_get}
		,
			{
			
			dnx_fifodma_info_entry_is_mem_set,
			dnx_fifodma_info_entry_is_mem_get}
		,
			{
			
			dnx_fifodma_info_entry_mem_set,
			dnx_fifodma_info_entry_mem_get}
		,
			{
			
			dnx_fifodma_info_entry_reg_set,
			dnx_fifodma_info_entry_reg_get}
		,
			{
			
			dnx_fifodma_info_entry_array_index_set,
			dnx_fifodma_info_entry_array_index_get}
		,
			{
			
			dnx_fifodma_info_entry_copyno_set,
			dnx_fifodma_info_entry_copyno_get}
		,
			{
			
			dnx_fifodma_info_entry_force_entry_size_set,
			dnx_fifodma_info_entry_force_entry_size_get}
		,
			{
			
			dnx_fifodma_info_entry_threshold_set,
			dnx_fifodma_info_entry_threshold_get}
		,
			{
			
			dnx_fifodma_info_entry_timeout_set,
			dnx_fifodma_info_entry_timeout_get}
		,
			{
			
			dnx_fifodma_info_entry_is_poll_set,
			dnx_fifodma_info_entry_is_poll_get}
		,
			{
			
			dnx_fifodma_info_entry_entry_size_set,
			dnx_fifodma_info_entry_entry_size_get}
		,
			{
			
			dnx_fifodma_info_entry_is_alloced_set,
			dnx_fifodma_info_entry_is_alloced_get}
		,
			{
			
			dnx_fifodma_info_entry_is_started_set,
			dnx_fifodma_info_entry_is_started_get}
		,
			{
			
			dnx_fifodma_info_entry_is_paused_set,
			dnx_fifodma_info_entry_is_paused_get}
		,
			{
			
			dnx_fifodma_info_entry_handler_get_cb,
			dnx_fifodma_info_entry_handler_register_cb,
			dnx_fifodma_info_entry_handler_unregister_cb}
		,
			{
			
			dnx_fifodma_info_entry_last_entry_id_set,
			dnx_fifodma_info_entry_last_entry_id_get}
		,
			{
			
			dnx_fifodma_info_entry_host_buff_lo_set,
			dnx_fifodma_info_entry_host_buff_lo_get}
		,
			{
			
			dnx_fifodma_info_entry_host_buff_hi_set,
			dnx_fifodma_info_entry_host_buff_hi_get}
		}
	}
;
#undef BSL_LOG_MODULE

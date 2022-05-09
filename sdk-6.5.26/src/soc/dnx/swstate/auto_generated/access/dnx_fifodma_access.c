
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_fifodma_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_fifodma_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_fifodma_layout.h>





int
dnx_fifodma_info_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_FIFODMA_INFO,
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
        DNX_SW_STATE_DNX_FIFODMA_INFO,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        fifodma_db_t,
        DNX_SW_STATE_DNX_FIFODMA_INFO_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_fifodma_info_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_fifodma_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_FIFODMA_INFO,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]),
        "dnx_fifodma_info[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]),
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->fifodma_thread_shr_thread_manager_handler,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        fifodma_info_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_fifodma_info_entry_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        "dnx_fifodma_info[%d]->entry",
        unit,
        nof_instances_to_alloc_0 * sizeof(fifodma_info_t) / sizeof(*((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_max_entries_set(int unit, uint32 entry_idx_0, uint32 max_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].max_entries,
        max_entries,
        uint32,
        0,
        "dnx_fifodma_info_entry_max_entries_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        &max_entries,
        "dnx_fifodma_info[%d]->entry[%d].max_entries",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_MAX_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_max_entries_get(int unit, uint32 entry_idx_0, uint32 *max_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        max_entries);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        DNX_SW_STATE_DIAG_READ);

    *max_entries = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].max_entries;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].max_entries,
        "dnx_fifodma_info[%d]->entry[%d].max_entries",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_MAX_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_entry_size_set(int unit, uint32 entry_idx_0, uint32 entry_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].entry_size,
        entry_size,
        uint32,
        0,
        "dnx_fifodma_info_entry_entry_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        &entry_size,
        "dnx_fifodma_info[%d]->entry[%d].entry_size",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_ENTRY_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_entry_size_get(int unit, uint32 entry_idx_0, uint32 *entry_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        entry_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        DNX_SW_STATE_DIAG_READ);

    *entry_size = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].entry_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].entry_size,
        "dnx_fifodma_info[%d]->entry[%d].entry_size",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_ENTRY_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_host_buff_lo_set(int unit, uint32 entry_idx_0, uint32 host_buff_lo)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].host_buff_lo,
        host_buff_lo,
        uint32,
        0,
        "dnx_fifodma_info_entry_host_buff_lo_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        &host_buff_lo,
        "dnx_fifodma_info[%d]->entry[%d].host_buff_lo",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HOST_BUFF_LO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_host_buff_lo_get(int unit, uint32 entry_idx_0, uint32 *host_buff_lo)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        host_buff_lo);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        DNX_SW_STATE_DIAG_READ);

    *host_buff_lo = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].host_buff_lo;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].host_buff_lo,
        "dnx_fifodma_info[%d]->entry[%d].host_buff_lo",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HOST_BUFF_LO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_host_buff_hi_set(int unit, uint32 entry_idx_0, uint32 host_buff_hi)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].host_buff_hi,
        host_buff_hi,
        uint32,
        0,
        "dnx_fifodma_info_entry_host_buff_hi_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        &host_buff_hi,
        "dnx_fifodma_info[%d]->entry[%d].host_buff_hi",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HOST_BUFF_HI_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_host_buff_hi_get(int unit, uint32 entry_idx_0, uint32 *host_buff_hi)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        host_buff_hi);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        DNX_SW_STATE_DIAG_READ);

    *host_buff_hi = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].host_buff_hi;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].host_buff_hi,
        "dnx_fifodma_info[%d]->entry[%d].host_buff_hi",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HOST_BUFF_HI_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_is_mem_set(int unit, uint32 entry_idx_0, int is_mem)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_mem,
        is_mem,
        int,
        0,
        "dnx_fifodma_info_entry_is_mem_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        &is_mem,
        "dnx_fifodma_info[%d]->entry[%d].is_mem",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_MEM_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_is_mem_get(int unit, uint32 entry_idx_0, int *is_mem)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        is_mem);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        DNX_SW_STATE_DIAG_READ);

    *is_mem = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_mem;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_mem,
        "dnx_fifodma_info[%d]->entry[%d].is_mem",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_MEM_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_mem_set(int unit, uint32 entry_idx_0, int mem)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].mem,
        mem,
        int,
        0,
        "dnx_fifodma_info_entry_mem_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        &mem,
        "dnx_fifodma_info[%d]->entry[%d].mem",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_MEM_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_mem_get(int unit, uint32 entry_idx_0, int *mem)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        mem);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        DNX_SW_STATE_DIAG_READ);

    *mem = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].mem;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].mem,
        "dnx_fifodma_info[%d]->entry[%d].mem",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_MEM_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_reg_set(int unit, uint32 entry_idx_0, int reg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].reg,
        reg,
        int,
        0,
        "dnx_fifodma_info_entry_reg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        &reg,
        "dnx_fifodma_info[%d]->entry[%d].reg",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_REG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_reg_get(int unit, uint32 entry_idx_0, int *reg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        reg);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        DNX_SW_STATE_DIAG_READ);

    *reg = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].reg;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].reg,
        "dnx_fifodma_info[%d]->entry[%d].reg",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_REG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_array_index_set(int unit, uint32 entry_idx_0, int array_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].array_index,
        array_index,
        int,
        0,
        "dnx_fifodma_info_entry_array_index_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        &array_index,
        "dnx_fifodma_info[%d]->entry[%d].array_index",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_ARRAY_INDEX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_array_index_get(int unit, uint32 entry_idx_0, int *array_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        array_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        DNX_SW_STATE_DIAG_READ);

    *array_index = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].array_index;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].array_index,
        "dnx_fifodma_info[%d]->entry[%d].array_index",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_ARRAY_INDEX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_copyno_set(int unit, uint32 entry_idx_0, int copyno)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].copyno,
        copyno,
        int,
        0,
        "dnx_fifodma_info_entry_copyno_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        &copyno,
        "dnx_fifodma_info[%d]->entry[%d].copyno",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_COPYNO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_copyno_get(int unit, uint32 entry_idx_0, int *copyno)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        copyno);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        DNX_SW_STATE_DIAG_READ);

    *copyno = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].copyno;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].copyno,
        "dnx_fifodma_info[%d]->entry[%d].copyno",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_COPYNO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_force_entry_size_set(int unit, uint32 entry_idx_0, int force_entry_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].force_entry_size,
        force_entry_size,
        int,
        0,
        "dnx_fifodma_info_entry_force_entry_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        &force_entry_size,
        "dnx_fifodma_info[%d]->entry[%d].force_entry_size",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_FORCE_ENTRY_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_force_entry_size_get(int unit, uint32 entry_idx_0, int *force_entry_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        force_entry_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        DNX_SW_STATE_DIAG_READ);

    *force_entry_size = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].force_entry_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].force_entry_size,
        "dnx_fifodma_info[%d]->entry[%d].force_entry_size",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_FORCE_ENTRY_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_threshold_set(int unit, uint32 entry_idx_0, int threshold)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].threshold,
        threshold,
        int,
        0,
        "dnx_fifodma_info_entry_threshold_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        &threshold,
        "dnx_fifodma_info[%d]->entry[%d].threshold",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_THRESHOLD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_threshold_get(int unit, uint32 entry_idx_0, int *threshold)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        threshold);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        DNX_SW_STATE_DIAG_READ);

    *threshold = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].threshold;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].threshold,
        "dnx_fifodma_info[%d]->entry[%d].threshold",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_THRESHOLD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_timeout_set(int unit, uint32 entry_idx_0, int timeout)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].timeout,
        timeout,
        int,
        0,
        "dnx_fifodma_info_entry_timeout_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        &timeout,
        "dnx_fifodma_info[%d]->entry[%d].timeout",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_TIMEOUT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_timeout_get(int unit, uint32 entry_idx_0, int *timeout)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        timeout);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        DNX_SW_STATE_DIAG_READ);

    *timeout = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].timeout;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].timeout,
        "dnx_fifodma_info[%d]->entry[%d].timeout",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_TIMEOUT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_is_poll_set(int unit, uint32 entry_idx_0, int is_poll)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_poll,
        is_poll,
        int,
        0,
        "dnx_fifodma_info_entry_is_poll_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        &is_poll,
        "dnx_fifodma_info[%d]->entry[%d].is_poll",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_POLL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_is_poll_get(int unit, uint32 entry_idx_0, int *is_poll)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        is_poll);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        DNX_SW_STATE_DIAG_READ);

    *is_poll = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_poll;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_poll,
        "dnx_fifodma_info[%d]->entry[%d].is_poll",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_POLL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_is_alloced_set(int unit, uint32 entry_idx_0, int is_alloced)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_alloced,
        is_alloced,
        int,
        0,
        "dnx_fifodma_info_entry_is_alloced_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        &is_alloced,
        "dnx_fifodma_info[%d]->entry[%d].is_alloced",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_ALLOCED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_is_alloced_get(int unit, uint32 entry_idx_0, int *is_alloced)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        is_alloced);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        DNX_SW_STATE_DIAG_READ);

    *is_alloced = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_alloced;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_alloced,
        "dnx_fifodma_info[%d]->entry[%d].is_alloced",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_ALLOCED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_is_started_set(int unit, uint32 entry_idx_0, int is_started)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_started,
        is_started,
        int,
        0,
        "dnx_fifodma_info_entry_is_started_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        &is_started,
        "dnx_fifodma_info[%d]->entry[%d].is_started",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_STARTED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_is_started_get(int unit, uint32 entry_idx_0, int *is_started)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        is_started);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        DNX_SW_STATE_DIAG_READ);

    *is_started = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_started;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_started,
        "dnx_fifodma_info[%d]->entry[%d].is_started",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_STARTED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_is_paused_set(int unit, uint32 entry_idx_0, int is_paused)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_paused,
        is_paused,
        int,
        0,
        "dnx_fifodma_info_entry_is_paused_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        &is_paused,
        "dnx_fifodma_info[%d]->entry[%d].is_paused",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_PAUSED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_is_paused_get(int unit, uint32 entry_idx_0, int *is_paused)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        is_paused);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        DNX_SW_STATE_DIAG_READ);

    *is_paused = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_paused;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].is_paused,
        "dnx_fifodma_info[%d]->entry[%d].is_paused",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_IS_PAUSED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_last_entry_id_set(int unit, uint32 entry_idx_0, int last_entry_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].last_entry_id,
        last_entry_id,
        int,
        0,
        "dnx_fifodma_info_entry_last_entry_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        &last_entry_id,
        "dnx_fifodma_info[%d]->entry[%d].last_entry_id",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_LAST_ENTRY_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_last_entry_id_get(int unit, uint32 entry_idx_0, int *last_entry_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        last_entry_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        DNX_SW_STATE_DIAG_READ);

    *last_entry_id = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].last_entry_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].last_entry_id,
        "dnx_fifodma_info[%d]->entry[%d].last_entry_id",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_LAST_ENTRY_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_handler_get_cb(int unit, uint32 entry_idx_0, soc_dnx_fifodma_interrupt_handler* cb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        SW_STATE_FUNC_GET_CB,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        DNX_SW_STATE_DIAG_CB_DB);

    SW_STATE_CB_DB_GET_CB(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].handler,
        cb,
        soc_dnx_fifodma_interrupt_handler_get_cb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_CB_DB_GET_CB_LOGGING,
        BSL_LS_SWSTATEDNX_CB_DBGET_CB,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].handler,
        "dnx_fifodma_info[%d]->entry[%d].handler",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HANDLER_INFO,
        DNX_SW_STATE_DIAG_CB_DB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_handler_register_cb(int unit, uint32 entry_idx_0, char *name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        SW_STATE_FUNC_REGISTER_CB,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        DNX_SW_STATE_DIAG_CB_DB);

    SW_STATE_CB_DB_REGISTER_CB(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].handler,
        name,
        soc_dnx_fifodma_interrupt_handler_get_cb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_CB_DB_REGISTER_CB_LOGGING,
        BSL_LS_SWSTATEDNX_CB_DBREGISTER_CB,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].handler,
        "dnx_fifodma_info[%d]->entry[%d].handler",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HANDLER_INFO,
        DNX_SW_STATE_DIAG_CB_DB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_entry_handler_unregister_cb(int unit, uint32 entry_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        SW_STATE_FUNC_UNREGISTER_CB,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry,
        entry_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        DNX_SW_STATE_DIAG_CB_DB);

    SW_STATE_CB_DB_UNREGISTER_CB(
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].handler);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_CB_DB_UNREGISTER_CB_LOGGING,
        BSL_LS_SWSTATEDNX_CB_DBUNREGISTER_CB,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->entry[entry_idx_0].handler,
        "dnx_fifodma_info[%d]->entry[%d].handler",
        unit, entry_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_ENTRY_HANDLER_INFO,
        DNX_SW_STATE_DIAG_CB_DB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_fifodma_thread_shr_thread_manager_handler_set(int unit, shr_thread_manager_handler_t fifodma_thread_shr_thread_manager_handler)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_WB);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER,
        ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->fifodma_thread_shr_thread_manager_handler,
        fifodma_thread_shr_thread_manager_handler,
        shr_thread_manager_handler_t,
        0,
        "dnx_fifodma_info_fifodma_thread_shr_thread_manager_handler_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER,
        &fifodma_thread_shr_thread_manager_handler,
        "dnx_fifodma_info[%d]->fifodma_thread_shr_thread_manager_handler",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fifodma_info_fifodma_thread_shr_thread_manager_handler_get(int unit, shr_thread_manager_handler_t *fifodma_thread_shr_thread_manager_handler)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_WB);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER,
        sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER,
        fifodma_thread_shr_thread_manager_handler);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER,
        DNX_SW_STATE_DIAG_READ);

    *fifodma_thread_shr_thread_manager_handler = ((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->fifodma_thread_shr_thread_manager_handler;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIFODMA_INFO__FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER,
        &((fifodma_db_t*)sw_state_roots_array[unit][DNX_FIFODMA_MODULE_ID])->fifodma_thread_shr_thread_manager_handler,
        "dnx_fifodma_info[%d]->fifodma_thread_shr_thread_manager_handler",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIFODMA_INFO__FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER,
        dnx_fifodma_info_info,
        DNX_FIFODMA_INFO_FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



shr_error_e
soc_dnx_fifodma_interrupt_handler_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, soc_dnx_fifodma_interrupt_handler * cb)
{
    if (!sal_strncmp(cb_db->function_name,"dnx_fifodma_interrupt_handler_example", 37))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_fifodma_interrupt_handler_example;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"dnx_oam_oamp_stat_dma_event_handler", 35))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_oam_oamp_stat_dma_event_handler;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"dnx_oam_oamp_v1_dma_event_handler", 33))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_oam_oamp_v1_dma_event_handler;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"dnx_olp_dma_handler", 19))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_olp_dma_handler;
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
			
			dnx_fifodma_info_entry_entry_size_set,
			dnx_fifodma_info_entry_entry_size_get}
		,
			{
			
			dnx_fifodma_info_entry_host_buff_lo_set,
			dnx_fifodma_info_entry_host_buff_lo_get}
		,
			{
			
			dnx_fifodma_info_entry_host_buff_hi_set,
			dnx_fifodma_info_entry_host_buff_hi_get}
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
			
			dnx_fifodma_info_entry_last_entry_id_set,
			dnx_fifodma_info_entry_last_entry_id_get}
		,
			{
			
			dnx_fifodma_info_entry_handler_get_cb,
			dnx_fifodma_info_entry_handler_register_cb,
			dnx_fifodma_info_entry_handler_unregister_cb}
		}
	,
		{
		
		dnx_fifodma_info_fifodma_thread_shr_thread_manager_handler_set,
		dnx_fifodma_info_fifodma_thread_shr_thread_manager_handler_get}
	}
;
#undef BSL_LOG_MODULE


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
#include <soc/dnx/swstate/auto_generated/access/algo_lif_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/algo_lif_diagnostic.h>



local_outlif_info_t* local_outlif_info_dummy = NULL;



int
local_outlif_info_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]),
        "local_outlif_info[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        ALGO_LIF_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        local_outlif_info_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "local_outlif_info_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]),
        "local_outlif_info[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(local_outlif_info_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_disable_mdb_clusters_set(int unit, uint8 disable_mdb_clusters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->disable_mdb_clusters,
        disable_mdb_clusters,
        uint8,
        0,
        "local_outlif_info_disable_mdb_clusters_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &disable_mdb_clusters,
        "local_outlif_info[%d]->disable_mdb_clusters",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_DISABLE_MDB_CLUSTERS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_disable_mdb_clusters_get(int unit, uint8 *disable_mdb_clusters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        disable_mdb_clusters);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *disable_mdb_clusters = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->disable_mdb_clusters;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->disable_mdb_clusters,
        "local_outlif_info[%d]->disable_mdb_clusters",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_DISABLE_MDB_CLUSTERS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_disable_eedb_data_banks_set(int unit, uint8 disable_eedb_data_banks)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->disable_eedb_data_banks,
        disable_eedb_data_banks,
        uint8,
        0,
        "local_outlif_info_disable_eedb_data_banks_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &disable_eedb_data_banks,
        "local_outlif_info[%d]->disable_eedb_data_banks",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_DISABLE_EEDB_DATA_BANKS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_disable_eedb_data_banks_get(int unit, uint8 *disable_eedb_data_banks)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        disable_eedb_data_banks);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *disable_eedb_data_banks = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->disable_eedb_data_banks;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->disable_eedb_data_banks,
        "local_outlif_info[%d]->disable_eedb_data_banks",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_DISABLE_EEDB_DATA_BANKS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SWSTATE_ALLOC_SIZE_VERIFY(
        unit,
        sizeof(uint8),
        MAX_EEDB_BANKS_PER_OUTLIF_BANK,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        local_outlif_bank_info_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "local_outlif_info_outlif_bank_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        "local_outlif_info[%d]->outlif_bank_info",
        unit,
        nof_instances_to_alloc_0 * sizeof(local_outlif_bank_info_t) / sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_assigned_set(int unit, uint32 outlif_bank_info_idx_0, uint8 assigned)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        outlif_bank_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].assigned,
        assigned,
        uint8,
        0,
        "local_outlif_info_outlif_bank_info_assigned_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &assigned,
        "local_outlif_info[%d]->outlif_bank_info[%d].assigned",
        unit, outlif_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_ASSIGNED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_assigned_get(int unit, uint32 outlif_bank_info_idx_0, uint8 *assigned)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        outlif_bank_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        assigned);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *assigned = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].assigned;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].assigned,
        "local_outlif_info[%d]->outlif_bank_info[%d].assigned",
        unit, outlif_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_ASSIGNED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_in_use_set(int unit, uint32 outlif_bank_info_idx_0, uint8 in_use)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        outlif_bank_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].in_use,
        in_use,
        uint8,
        0,
        "local_outlif_info_outlif_bank_info_in_use_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &in_use,
        "local_outlif_info[%d]->outlif_bank_info[%d].in_use",
        unit, outlif_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_IN_USE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_in_use_get(int unit, uint32 outlif_bank_info_idx_0, uint8 *in_use)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        outlif_bank_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        in_use);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *in_use = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].in_use;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].in_use,
        "local_outlif_info[%d]->outlif_bank_info[%d].in_use",
        unit, outlif_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_IN_USE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_used_logical_phase_set(int unit, uint32 outlif_bank_info_idx_0, uint8 used_logical_phase)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        outlif_bank_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].used_logical_phase,
        used_logical_phase,
        uint8,
        0,
        "local_outlif_info_outlif_bank_info_used_logical_phase_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &used_logical_phase,
        "local_outlif_info[%d]->outlif_bank_info[%d].used_logical_phase",
        unit, outlif_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_USED_LOGICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_used_logical_phase_get(int unit, uint32 outlif_bank_info_idx_0, uint8 *used_logical_phase)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        outlif_bank_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        used_logical_phase);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *used_logical_phase = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].used_logical_phase;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].used_logical_phase,
        "local_outlif_info[%d]->outlif_bank_info[%d].used_logical_phase",
        unit, outlif_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_USED_LOGICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_ll_in_use_set(int unit, uint32 outlif_bank_info_idx_0, uint8 ll_in_use)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        outlif_bank_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].ll_in_use,
        ll_in_use,
        uint8,
        0,
        "local_outlif_info_outlif_bank_info_ll_in_use_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &ll_in_use,
        "local_outlif_info[%d]->outlif_bank_info[%d].ll_in_use",
        unit, outlif_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_LL_IN_USE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_ll_in_use_get(int unit, uint32 outlif_bank_info_idx_0, uint8 *ll_in_use)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        outlif_bank_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ll_in_use);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *ll_in_use = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].ll_in_use;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].ll_in_use,
        "local_outlif_info[%d]->outlif_bank_info[%d].ll_in_use",
        unit, outlif_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_LL_IN_USE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_ll_index_set(int unit, uint32 outlif_bank_info_idx_0, uint8 ll_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        outlif_bank_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].ll_index,
        ll_index,
        uint8,
        0,
        "local_outlif_info_outlif_bank_info_ll_index_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &ll_index,
        "local_outlif_info[%d]->outlif_bank_info[%d].ll_index",
        unit, outlif_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_LL_INDEX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_ll_index_get(int unit, uint32 outlif_bank_info_idx_0, uint8 *ll_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        outlif_bank_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ll_index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *ll_index = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].ll_index;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].ll_index,
        "local_outlif_info[%d]->outlif_bank_info[%d].ll_index",
        unit, outlif_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_LL_INDEX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_eedb_data_banks_set(int unit, uint32 outlif_bank_info_idx_0, uint32 eedb_data_banks_idx_0, uint8 eedb_data_banks)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        outlif_bank_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        eedb_data_banks_idx_0,
        MAX_EEDB_BANKS_PER_OUTLIF_BANK);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].eedb_data_banks[eedb_data_banks_idx_0],
        eedb_data_banks,
        uint8,
        0,
        "local_outlif_info_outlif_bank_info_eedb_data_banks_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &eedb_data_banks,
        "local_outlif_info[%d]->outlif_bank_info[%d].eedb_data_banks[%d]",
        unit, outlif_bank_info_idx_0, eedb_data_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_EEDB_DATA_BANKS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_outlif_bank_info_eedb_data_banks_get(int unit, uint32 outlif_bank_info_idx_0, uint32 eedb_data_banks_idx_0, uint8 *eedb_data_banks)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info,
        outlif_bank_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        eedb_data_banks);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        eedb_data_banks_idx_0,
        MAX_EEDB_BANKS_PER_OUTLIF_BANK);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *eedb_data_banks = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].eedb_data_banks[eedb_data_banks_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->outlif_bank_info[outlif_bank_info_idx_0].eedb_data_banks[eedb_data_banks_idx_0],
        "local_outlif_info[%d]->outlif_bank_info[%d].eedb_data_banks[%d]",
        unit, outlif_bank_info_idx_0, eedb_data_banks_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_EEDB_DATA_BANKS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        outlif_logical_phase_info_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "local_outlif_info_logical_phase_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        "local_outlif_info[%d]->logical_phase_info",
        unit,
        nof_instances_to_alloc_0 * sizeof(outlif_logical_phase_info_t) / sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_physical_phase_set(int unit, uint32 logical_phase_info_idx_0, uint8 physical_phase)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        logical_phase_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].physical_phase,
        physical_phase,
        uint8,
        0,
        "local_outlif_info_logical_phase_info_physical_phase_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &physical_phase,
        "local_outlif_info[%d]->logical_phase_info[%d].physical_phase",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_PHYSICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_physical_phase_get(int unit, uint32 logical_phase_info_idx_0, uint8 *physical_phase)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        logical_phase_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        physical_phase);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *physical_phase = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].physical_phase;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].physical_phase,
        "local_outlif_info[%d]->logical_phase_info[%d].physical_phase",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_PHYSICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_address_granularity_set(int unit, uint32 logical_phase_info_idx_0, uint8 address_granularity)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        logical_phase_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].address_granularity,
        address_granularity,
        uint8,
        0,
        "local_outlif_info_logical_phase_info_address_granularity_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &address_granularity,
        "local_outlif_info[%d]->logical_phase_info[%d].address_granularity",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_ADDRESS_GRANULARITY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_address_granularity_get(int unit, uint32 logical_phase_info_idx_0, uint8 *address_granularity)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        logical_phase_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        address_granularity);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *address_granularity = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].address_granularity;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].address_granularity,
        "local_outlif_info[%d]->logical_phase_info[%d].address_granularity",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_ADDRESS_GRANULARITY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_first_bank_set(int unit, uint32 logical_phase_info_idx_0, uint32 first_bank)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        logical_phase_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].first_bank,
        first_bank,
        uint32,
        0,
        "local_outlif_info_logical_phase_info_first_bank_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &first_bank,
        "local_outlif_info[%d]->logical_phase_info[%d].first_bank",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_FIRST_BANK_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_first_bank_get(int unit, uint32 logical_phase_info_idx_0, uint32 *first_bank)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        logical_phase_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        first_bank);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *first_bank = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].first_bank;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].first_bank,
        "local_outlif_info[%d]->logical_phase_info[%d].first_bank",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_FIRST_BANK_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_last_bank_set(int unit, uint32 logical_phase_info_idx_0, uint32 last_bank)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        logical_phase_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].last_bank,
        last_bank,
        uint32,
        0,
        "local_outlif_info_logical_phase_info_last_bank_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &last_bank,
        "local_outlif_info[%d]->logical_phase_info[%d].last_bank",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_LAST_BANK_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_last_bank_get(int unit, uint32 logical_phase_info_idx_0, uint32 *last_bank)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        logical_phase_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        last_bank);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *last_bank = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].last_bank;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].last_bank,
        "local_outlif_info[%d]->logical_phase_info[%d].last_bank",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_LAST_BANK_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_first_ll_bank_set(int unit, uint32 logical_phase_info_idx_0, uint8 first_ll_bank)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        logical_phase_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].first_ll_bank,
        first_ll_bank,
        uint8,
        0,
        "local_outlif_info_logical_phase_info_first_ll_bank_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &first_ll_bank,
        "local_outlif_info[%d]->logical_phase_info[%d].first_ll_bank",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_FIRST_LL_BANK_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_first_ll_bank_get(int unit, uint32 logical_phase_info_idx_0, uint8 *first_ll_bank)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        logical_phase_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        first_ll_bank);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *first_ll_bank = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].first_ll_bank;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].first_ll_bank,
        "local_outlif_info[%d]->logical_phase_info[%d].first_ll_bank",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_FIRST_LL_BANK_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_last_ll_bank_set(int unit, uint32 logical_phase_info_idx_0, uint8 last_ll_bank)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        logical_phase_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].last_ll_bank,
        last_ll_bank,
        uint8,
        0,
        "local_outlif_info_logical_phase_info_last_ll_bank_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &last_ll_bank,
        "local_outlif_info[%d]->logical_phase_info[%d].last_ll_bank",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_LAST_LL_BANK_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_last_ll_bank_get(int unit, uint32 logical_phase_info_idx_0, uint8 *last_ll_bank)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info,
        logical_phase_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        last_ll_bank);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *last_ll_bank = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].last_ll_bank;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].last_ll_bank,
        "local_outlif_info[%d]->logical_phase_info[%d].last_ll_bank",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_LAST_LL_BANK_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_dbal_valid_combinations_create(int unit, uint32 logical_phase_info_idx_0, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_HTB_CREATE(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].dbal_valid_combinations,
        init_info,
        dbal_to_phase_info_t,
        dnx_local_outlif_ll_indication_e,
        FALSE,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].dbal_valid_combinations,
        "local_outlif_info[%d]->logical_phase_info[%d].dbal_valid_combinations",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_DBAL_VALID_COMBINATIONS_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_dbal_valid_combinations_find(int unit, uint32 logical_phase_info_idx_0, const dbal_to_phase_info_t *key, dnx_local_outlif_ll_indication_e *value, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_HTB_FIND(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].dbal_valid_combinations,
        key,
        value,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].dbal_valid_combinations,
        "local_outlif_info[%d]->logical_phase_info[%d].dbal_valid_combinations",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_DBAL_VALID_COMBINATIONS_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_dbal_valid_combinations_insert(int unit, uint32 logical_phase_info_idx_0, const dbal_to_phase_info_t *key, const dnx_local_outlif_ll_indication_e *value, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_HTB_INSERT(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].dbal_valid_combinations,
        key,
        value,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].dbal_valid_combinations,
        "local_outlif_info[%d]->logical_phase_info[%d].dbal_valid_combinations",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_DBAL_VALID_COMBINATIONS_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_dbal_valid_combinations_get_next(int unit, uint32 logical_phase_info_idx_0, SW_STATE_HASH_TABLE_ITER *iter, const dbal_to_phase_info_t *key, const dnx_local_outlif_ll_indication_e *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_HTB_GET_NEXT(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].dbal_valid_combinations,
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].dbal_valid_combinations,
        "local_outlif_info[%d]->logical_phase_info[%d].dbal_valid_combinations",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_DBAL_VALID_COMBINATIONS_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_dbal_valid_combinations_clear(int unit, uint32 logical_phase_info_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_HTB_CLEAR(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].dbal_valid_combinations);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].dbal_valid_combinations,
        "local_outlif_info[%d]->logical_phase_info[%d].dbal_valid_combinations",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_DBAL_VALID_COMBINATIONS_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_logical_phase_info_dbal_valid_combinations_delete(int unit, uint32 logical_phase_info_idx_0, const dbal_to_phase_info_t *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_HTB_DELETE(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].dbal_valid_combinations,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->logical_phase_info[logical_phase_info_idx_0].dbal_valid_combinations,
        "local_outlif_info[%d]->logical_phase_info[%d].dbal_valid_combinations",
        unit, logical_phase_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_DBAL_VALID_COMBINATIONS_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_physical_phase_to_logical_phase_map_set(int unit, uint32 physical_phase_to_logical_phase_map_idx_0, uint8 physical_phase_to_logical_phase_map)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map,
        physical_phase_to_logical_phase_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map[physical_phase_to_logical_phase_map_idx_0],
        physical_phase_to_logical_phase_map,
        uint8,
        0,
        "local_outlif_info_physical_phase_to_logical_phase_map_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        ALGO_LIF_MODULE_ID,
        &physical_phase_to_logical_phase_map,
        "local_outlif_info[%d]->physical_phase_to_logical_phase_map[%d]",
        unit, physical_phase_to_logical_phase_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_PHYSICAL_PHASE_TO_LOGICAL_PHASE_MAP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_physical_phase_to_logical_phase_map_get(int unit, uint32 physical_phase_to_logical_phase_map_idx_0, uint8 *physical_phase_to_logical_phase_map)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map,
        physical_phase_to_logical_phase_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        physical_phase_to_logical_phase_map);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    *physical_phase_to_logical_phase_map = ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map[physical_phase_to_logical_phase_map_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map[physical_phase_to_logical_phase_map_idx_0],
        "local_outlif_info[%d]->physical_phase_to_logical_phase_map[%d]",
        unit, physical_phase_to_logical_phase_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_PHYSICAL_PHASE_TO_LOGICAL_PHASE_MAP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_physical_phase_to_logical_phase_map_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map,
        uint8,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "local_outlif_info_physical_phase_to_logical_phase_map_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map,
        "local_outlif_info[%d]->physical_phase_to_logical_phase_map",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint8) / sizeof(*((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->physical_phase_to_logical_phase_map));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_PHYSICAL_PHASE_TO_LOGICAL_PHASE_MAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_bank_occupation_bitmap_alloc_bitmap(int unit, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
        _nof_bits_to_alloc,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "local_outlif_info_eedb_banks_info_bank_occupation_bitmap_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
        "local_outlif_info[%d]->eedb_banks_info.bank_occupation_bitmap",
        unit,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_BANK_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_BIT_SET(
        unit,
         ALGO_LIF_MODULE_ID,
         0,
         ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
        "local_outlif_info[%d]->eedb_banks_info.bank_occupation_bitmap",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_BANK_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_clear(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         ALGO_LIF_MODULE_ID,
         0,
         ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
        "local_outlif_info[%d]->eedb_banks_info.bank_occupation_bitmap",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_BANK_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_BIT_GET(
        unit,
         ALGO_LIF_MODULE_ID,
         0,
         ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
        "local_outlif_info[%d]->eedb_banks_info.bank_occupation_bitmap",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_BANK_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_range_count(int unit, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         ALGO_LIF_MODULE_ID,
         0,
         ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.bank_occupation_bitmap,
        "local_outlif_info[%d]->eedb_banks_info.bank_occupation_bitmap",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_BANK_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_alloc_bitmap(int unit, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap,
        _nof_bits_to_alloc,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap,
        "local_outlif_info[%d]->eedb_banks_info.outlif_prefix_occupation_bitmap",
        unit,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_OUTLIF_PREFIX_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_BIT_SET(
        unit,
         ALGO_LIF_MODULE_ID,
         0,
         ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap,
        "local_outlif_info[%d]->eedb_banks_info.outlif_prefix_occupation_bitmap",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_OUTLIF_PREFIX_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_bit_clear(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         ALGO_LIF_MODULE_ID,
         0,
         ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap,
        "local_outlif_info[%d]->eedb_banks_info.outlif_prefix_occupation_bitmap",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_OUTLIF_PREFIX_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    DNX_SW_STATE_BIT_GET(
        unit,
         ALGO_LIF_MODULE_ID,
         0,
         ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.outlif_prefix_occupation_bitmap,
        "local_outlif_info[%d]->eedb_banks_info.outlif_prefix_occupation_bitmap",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_OUTLIF_PREFIX_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_create_empty(int unit, sw_state_ll_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_CREATE_EMPTY(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        init_info,
        SW_STATE_LL_MULTIHEAD,
        non_existing_dummy_type_t,
        uint8,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_CREATE_EMPTY_LOGGING,
        BSL_LS_SWSTATEDNX_LLCREATE_EMPTY,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        "local_outlif_info[%d]->eedb_banks_info.used_data_bank_per_logical_phase",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_USED_DATA_BANK_PER_LOGICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_nof_elements(int unit, uint32 ll_head_index, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_NOF_ELEMENTS(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        ll_head_index,
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_LLNOF_ELEMENTS,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        "local_outlif_info[%d]->eedb_banks_info.used_data_bank_per_logical_phase",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_USED_DATA_BANK_PER_LOGICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_node_value(int unit, sw_state_ll_node_t node, uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_NODE_VALUE(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_VALUE,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        "local_outlif_info[%d]->eedb_banks_info.used_data_bank_per_logical_phase",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_USED_DATA_BANK_PER_LOGICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_node_update(int unit, sw_state_ll_node_t node, const uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_NODE_UPDATE(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_UPDATE,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        "local_outlif_info[%d]->eedb_banks_info.used_data_bank_per_logical_phase",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_USED_DATA_BANK_PER_LOGICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_next_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_NEXT_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_NEXT_NODE(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        ll_head_index,
        node,
        next_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NEXT_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNEXT_NODE,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        "local_outlif_info[%d]->eedb_banks_info.used_data_bank_per_logical_phase",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_USED_DATA_BANK_PER_LOGICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_previous_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_PREVIOUS_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_PREVIOUS_NODE(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        ll_head_index,
        node,
        prev_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLPREVIOUS_NODE,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        "local_outlif_info[%d]->eedb_banks_info.used_data_bank_per_logical_phase",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_USED_DATA_BANK_PER_LOGICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_add_first(int unit, uint32 ll_head_index, const uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_ADD_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_ADD_FIRST(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        ll_head_index,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_FIRST,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        "local_outlif_info[%d]->eedb_banks_info.used_data_bank_per_logical_phase",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_USED_DATA_BANK_PER_LOGICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_add_last(int unit, uint32 ll_head_index, const uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_ADD_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_ADD_LAST(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        ll_head_index,
        NULL,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_LAST,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        "local_outlif_info[%d]->eedb_banks_info.used_data_bank_per_logical_phase",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_USED_DATA_BANK_PER_LOGICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_remove_node(int unit, uint32 ll_head_index, sw_state_ll_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_REMOVE_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_REMOVE_NODE(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_REMOVE_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLREMOVE_NODE,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        "local_outlif_info[%d]->eedb_banks_info.used_data_bank_per_logical_phase",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_USED_DATA_BANK_PER_LOGICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_get_first(int unit, uint32 ll_head_index, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        ALGO_LIF_MODULE_ID,
        SW_STATE_FUNC_GET_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        ALGO_LIF_MODULE_ID,
        sw_state_roots_array[unit][ALGO_LIF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        ALGO_LIF_MODULE_ID);

    SW_STATE_MULTIHEAD_LL_GET_FIRST(
        ALGO_LIF_MODULE_ID,
        ((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        ll_head_index,
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_FIRST,
        ALGO_LIF_MODULE_ID,
        &((local_outlif_info_t*)sw_state_roots_array[unit][ALGO_LIF_MODULE_ID])->eedb_banks_info.used_data_bank_per_logical_phase,
        "local_outlif_info[%d]->eedb_banks_info.used_data_bank_per_logical_phase",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        ALGO_LIF_MODULE_ID,
        local_outlif_info_info,
        LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_USED_DATA_BANK_PER_LOGICAL_PHASE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




const char *
dnx_local_outlif_ll_indication_e_get_name(dnx_local_outlif_ll_indication_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_LOCAL_OUTLIF_LL_ALWAYS", value, DNX_LOCAL_OUTLIF_LL_ALWAYS);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_LOCAL_OUTLIF_LL_OPTIONAL", value, DNX_LOCAL_OUTLIF_LL_OPTIONAL);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_LOCAL_OUTLIF_LL_NEVER", value, DNX_LOCAL_OUTLIF_LL_NEVER);

    return NULL;
}





local_outlif_info_cbs local_outlif_info = 	{
	
	local_outlif_info_is_init,
	local_outlif_info_init,
		{
		
		local_outlif_info_disable_mdb_clusters_set,
		local_outlif_info_disable_mdb_clusters_get}
	,
		{
		
		local_outlif_info_disable_eedb_data_banks_set,
		local_outlif_info_disable_eedb_data_banks_get}
	,
		{
		
		local_outlif_info_outlif_bank_info_alloc,
			{
			
			local_outlif_info_outlif_bank_info_assigned_set,
			local_outlif_info_outlif_bank_info_assigned_get}
		,
			{
			
			local_outlif_info_outlif_bank_info_in_use_set,
			local_outlif_info_outlif_bank_info_in_use_get}
		,
			{
			
			local_outlif_info_outlif_bank_info_used_logical_phase_set,
			local_outlif_info_outlif_bank_info_used_logical_phase_get}
		,
			{
			
			local_outlif_info_outlif_bank_info_ll_in_use_set,
			local_outlif_info_outlif_bank_info_ll_in_use_get}
		,
			{
			
			local_outlif_info_outlif_bank_info_ll_index_set,
			local_outlif_info_outlif_bank_info_ll_index_get}
		,
			{
			
			local_outlif_info_outlif_bank_info_eedb_data_banks_set,
			local_outlif_info_outlif_bank_info_eedb_data_banks_get}
		}
	,
		{
		
		local_outlif_info_logical_phase_info_alloc,
			{
			
			local_outlif_info_logical_phase_info_physical_phase_set,
			local_outlif_info_logical_phase_info_physical_phase_get}
		,
			{
			
			local_outlif_info_logical_phase_info_address_granularity_set,
			local_outlif_info_logical_phase_info_address_granularity_get}
		,
			{
			
			local_outlif_info_logical_phase_info_first_bank_set,
			local_outlif_info_logical_phase_info_first_bank_get}
		,
			{
			
			local_outlif_info_logical_phase_info_last_bank_set,
			local_outlif_info_logical_phase_info_last_bank_get}
		,
			{
			
			local_outlif_info_logical_phase_info_first_ll_bank_set,
			local_outlif_info_logical_phase_info_first_ll_bank_get}
		,
			{
			
			local_outlif_info_logical_phase_info_last_ll_bank_set,
			local_outlif_info_logical_phase_info_last_ll_bank_get}
		,
			{
			
			local_outlif_info_logical_phase_info_dbal_valid_combinations_create,
			local_outlif_info_logical_phase_info_dbal_valid_combinations_find,
			local_outlif_info_logical_phase_info_dbal_valid_combinations_insert,
			local_outlif_info_logical_phase_info_dbal_valid_combinations_get_next,
			local_outlif_info_logical_phase_info_dbal_valid_combinations_clear,
			local_outlif_info_logical_phase_info_dbal_valid_combinations_delete}
		}
	,
		{
		
		local_outlif_info_physical_phase_to_logical_phase_map_set,
		local_outlif_info_physical_phase_to_logical_phase_map_get,
		local_outlif_info_physical_phase_to_logical_phase_map_alloc}
	,
		{
		
			{
			
			local_outlif_info_eedb_banks_info_bank_occupation_bitmap_alloc_bitmap,
			local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_set,
			local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_clear,
			local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_get,
			local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_range_count}
		,
			{
			
			local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_alloc_bitmap,
			local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_bit_set,
			local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_bit_clear,
			local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_bit_get}
		,
			{
			
			local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_create_empty,
			local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_nof_elements,
			local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_node_value,
			local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_node_update,
			local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_next_node,
			local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_previous_node,
			local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_add_first,
			local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_add_last,
			local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_remove_node,
			local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_get_first}
		}
	}
;
#undef BSL_LOG_MODULE

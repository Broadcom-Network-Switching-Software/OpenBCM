
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/mdb_kaps_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/mdb_kaps_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/mdb_kaps_layout.h>





int
mdb_kaps_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_MDB_KAPS_DB,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]),
        "mdb_kaps_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        mdb_kaps_sw_state_t,
        DNX_SW_STATE_MDB_KAPS_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_kaps_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        mdb_kaps_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_MDB_KAPS_DB,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]),
        "mdb_kaps_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB,
        mdb_kaps_db_info,
        MDB_KAPS_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]),
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_db_info_set(int unit, uint32 db_info_idx_0, CONST mdb_kaps_db_handles_t *db_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info,
        db_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info[db_info_idx_0],
        db_info,
        mdb_kaps_db_handles_t,
        0,
        "mdb_kaps_db_db_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        db_info,
        "mdb_kaps_db[%d]->db_info[%d]",
        unit, db_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        mdb_kaps_db_info,
        MDB_KAPS_DB_DB_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_db_info_get(int unit, uint32 db_info_idx_0, mdb_kaps_db_handles_t *db_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info,
        db_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        db_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        DNX_SW_STATE_DIAG_READ);

    *db_info = ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info[db_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        &((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info[db_info_idx_0],
        "mdb_kaps_db[%d]->db_info[%d]",
        unit, db_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        mdb_kaps_db_info,
        MDB_KAPS_DB_DB_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_db_info_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info,
        mdb_kaps_db_handles_t,
        dnx_data_mdb.kaps.nof_dbs_get(unit),
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_kaps_db_db_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info,
        "mdb_kaps_db[%d]->db_info",
        unit,
        dnx_data_mdb.kaps.nof_dbs_get(unit) * sizeof(mdb_kaps_db_handles_t) / sizeof(*((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        mdb_kaps_db_info,
        MDB_KAPS_DB_DB_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_search_instruction_info_set(int unit, uint32 search_instruction_info_idx_0, CONST mdb_kaps_instruction_handles_t *search_instruction_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info,
        search_instruction_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info[search_instruction_info_idx_0],
        search_instruction_info,
        mdb_kaps_instruction_handles_t,
        0,
        "mdb_kaps_db_search_instruction_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        search_instruction_info,
        "mdb_kaps_db[%d]->search_instruction_info[%d]",
        unit, search_instruction_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        mdb_kaps_db_info,
        MDB_KAPS_DB_SEARCH_INSTRUCTION_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_search_instruction_info_get(int unit, uint32 search_instruction_info_idx_0, mdb_kaps_instruction_handles_t *search_instruction_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info,
        search_instruction_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        search_instruction_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        DNX_SW_STATE_DIAG_READ);

    *search_instruction_info = ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info[search_instruction_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        &((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info[search_instruction_info_idx_0],
        "mdb_kaps_db[%d]->search_instruction_info[%d]",
        unit, search_instruction_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        mdb_kaps_db_info,
        MDB_KAPS_DB_SEARCH_INSTRUCTION_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_search_instruction_info_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info,
        mdb_kaps_instruction_handles_t,
        dnx_data_mdb.kaps.nof_dbs_get(unit)/MDB_LPM_DBS_IN_PAIR,
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_kaps_db_search_instruction_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info,
        "mdb_kaps_db[%d]->search_instruction_info",
        unit,
        dnx_data_mdb.kaps.nof_dbs_get(unit)/MDB_LPM_DBS_IN_PAIR * sizeof(mdb_kaps_instruction_handles_t) / sizeof(*((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        mdb_kaps_db_info,
        MDB_KAPS_DB_SEARCH_INSTRUCTION_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_big_kaps_large_bb_mode_set(int unit, uint32 big_kaps_large_bb_mode_idx_0, uint32 big_kaps_large_bb_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__BIG_KAPS_LARGE_BB_MODE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__BIG_KAPS_LARGE_BB_MODE,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        big_kaps_large_bb_mode_idx_0,
        DNX_DATA_MAX_MDB_KAPS_NOF_BIG_BBS_BLK_IDS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__BIG_KAPS_LARGE_BB_MODE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__BIG_KAPS_LARGE_BB_MODE,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->big_kaps_large_bb_mode[big_kaps_large_bb_mode_idx_0],
        big_kaps_large_bb_mode,
        uint32,
        0,
        "mdb_kaps_db_big_kaps_large_bb_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_KAPS_DB__BIG_KAPS_LARGE_BB_MODE,
        &big_kaps_large_bb_mode,
        "mdb_kaps_db[%d]->big_kaps_large_bb_mode[%d]",
        unit, big_kaps_large_bb_mode_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__BIG_KAPS_LARGE_BB_MODE,
        mdb_kaps_db_info,
        MDB_KAPS_DB_BIG_KAPS_LARGE_BB_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_big_kaps_large_bb_mode_get(int unit, uint32 big_kaps_large_bb_mode_idx_0, uint32 *big_kaps_large_bb_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__BIG_KAPS_LARGE_BB_MODE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__BIG_KAPS_LARGE_BB_MODE,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        big_kaps_large_bb_mode_idx_0,
        DNX_DATA_MAX_MDB_KAPS_NOF_BIG_BBS_BLK_IDS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__BIG_KAPS_LARGE_BB_MODE,
        big_kaps_large_bb_mode);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__BIG_KAPS_LARGE_BB_MODE,
        DNX_SW_STATE_DIAG_READ);

    *big_kaps_large_bb_mode = ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->big_kaps_large_bb_mode[big_kaps_large_bb_mode_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_KAPS_DB__BIG_KAPS_LARGE_BB_MODE,
        &((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->big_kaps_large_bb_mode[big_kaps_large_bb_mode_idx_0],
        "mdb_kaps_db[%d]->big_kaps_large_bb_mode[%d]",
        unit, big_kaps_large_bb_mode_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__BIG_KAPS_LARGE_BB_MODE,
        mdb_kaps_db_info,
        MDB_KAPS_DB_BIG_KAPS_LARGE_BB_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_ad_size_supported_set(int unit, uint32 ad_size_supported_idx_0, uint32 ad_size_supported_idx_1, uint8 ad_size_supported)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported,
        ad_size_supported_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ad_size_supported_idx_1,
        MDB_NOF_LPM_AD_SIZES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported[ad_size_supported_idx_0][ad_size_supported_idx_1],
        ad_size_supported,
        uint8,
        0,
        "mdb_kaps_db_ad_size_supported_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        &ad_size_supported,
        "mdb_kaps_db[%d]->ad_size_supported[%d][%d]",
        unit, ad_size_supported_idx_0, ad_size_supported_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        mdb_kaps_db_info,
        MDB_KAPS_DB_AD_SIZE_SUPPORTED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_ad_size_supported_get(int unit, uint32 ad_size_supported_idx_0, uint32 ad_size_supported_idx_1, uint8 *ad_size_supported)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported,
        ad_size_supported_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        ad_size_supported);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ad_size_supported_idx_1,
        MDB_NOF_LPM_AD_SIZES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        DNX_SW_STATE_DIAG_READ);

    *ad_size_supported = ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported[ad_size_supported_idx_0][ad_size_supported_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        &((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported[ad_size_supported_idx_0][ad_size_supported_idx_1],
        "mdb_kaps_db[%d]->ad_size_supported[%d][%d]",
        unit, ad_size_supported_idx_0, ad_size_supported_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        mdb_kaps_db_info,
        MDB_KAPS_DB_AD_SIZE_SUPPORTED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_ad_size_supported_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported,
        uint8[MDB_NOF_LPM_AD_SIZES],
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_kaps_db_ad_size_supported_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported,
        "mdb_kaps_db[%d]->ad_size_supported",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint8[MDB_NOF_LPM_AD_SIZES]) / sizeof(*((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
        mdb_kaps_db_info,
        MDB_KAPS_DB_AD_SIZE_SUPPORTED_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], MDB_NOF_LPM_AD_SIZES)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported[def_val_idx[0]][def_val_idx[1]],
        FALSE);

    DNXC_SW_STATE_FUNC_RETURN;
}




mdb_kaps_db_cbs mdb_kaps_db = 	{
	
	mdb_kaps_db_is_init,
	mdb_kaps_db_init,
		{
		
		mdb_kaps_db_db_info_set,
		mdb_kaps_db_db_info_get,
		mdb_kaps_db_db_info_alloc}
	,
		{
		
		mdb_kaps_db_search_instruction_info_set,
		mdb_kaps_db_search_instruction_info_get,
		mdb_kaps_db_search_instruction_info_alloc}
	,
		{
		
		mdb_kaps_db_big_kaps_large_bb_mode_set,
		mdb_kaps_db_big_kaps_large_bb_mode_get}
	,
		{
		
		mdb_kaps_db_ad_size_supported_set,
		mdb_kaps_db_ad_size_supported_get,
		mdb_kaps_db_ad_size_supported_alloc}
	}
;
#undef BSL_LOG_MODULE

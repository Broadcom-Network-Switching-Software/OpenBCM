
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
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]));

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
        NULL);

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
        NULL);

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

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__DB_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info,
        mdb_kaps_db_handles_t,
        dnx_data_mdb.kaps.nof_dbs_get(unit),
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
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_kaps_db_search_instruction_info_set(int unit, CONST mdb_kaps_instruction_handles_t *search_instruction_info)
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

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info,
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
        "mdb_kaps_db[%d]->search_instruction_info",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        mdb_kaps_db_info,
        MDB_KAPS_DB_SEARCH_INSTRUCTION_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_kaps_db_search_instruction_info_get(int unit, mdb_kaps_instruction_handles_t *search_instruction_info)
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

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        search_instruction_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        DNX_SW_STATE_DIAG_READ);

    *search_instruction_info = ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        &((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info,
        "mdb_kaps_db[%d]->search_instruction_info",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO,
        mdb_kaps_db_info,
        MDB_KAPS_DB_SEARCH_INSTRUCTION_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

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
        NULL);

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
        NULL);

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
		mdb_kaps_db_search_instruction_info_get}
	,
		{
		
		mdb_kaps_db_big_kaps_large_bb_mode_set,
		mdb_kaps_db_big_kaps_large_bb_mode_get}
	}
;
#undef BSL_LOG_MODULE

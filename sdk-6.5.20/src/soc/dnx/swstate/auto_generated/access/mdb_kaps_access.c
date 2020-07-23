
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
#include <soc/dnx/swstate/auto_generated/access/mdb_kaps_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/mdb_kaps_diagnostic.h>



mdb_kaps_sw_state_t* mdb_kaps_db_dummy = NULL;



int
mdb_kaps_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_KAPS_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        MDB_KAPS_MODULE_ID,
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
        MDB_KAPS_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        MDB_KAPS_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        mdb_kaps_sw_state_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_kaps_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        MDB_KAPS_MODULE_ID,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]),
        "mdb_kaps_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        MDB_KAPS_MODULE_ID,
        mdb_kaps_db_info,
        MDB_KAPS_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(mdb_kaps_sw_state_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_kaps_db_db_info_set(int unit, uint32 db_info_idx_0, CONST mdb_kaps_db_handles_t *db_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_KAPS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_KAPS_MODULE_ID,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_KAPS_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        MDB_KAPS_MODULE_ID,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info[db_info_idx_0],
        db_info,
        mdb_kaps_db_handles_t,
        0,
        "mdb_kaps_db_db_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        MDB_KAPS_MODULE_ID,
        db_info,
        "mdb_kaps_db[%d]->db_info[%d]",
        unit, db_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_KAPS_MODULE_ID,
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
        MDB_KAPS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_KAPS_MODULE_ID,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_info_idx_0,
        MDB_KAPS_IP_NOF_DB);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        MDB_KAPS_MODULE_ID,
        db_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_KAPS_MODULE_ID);

    *db_info = ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info[db_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        MDB_KAPS_MODULE_ID,
        &((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info[db_info_idx_0],
        "mdb_kaps_db[%d]->db_info[%d]",
        unit, db_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_KAPS_MODULE_ID,
        mdb_kaps_db_info,
        MDB_KAPS_DB_DB_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_kaps_db_search_instruction_info_set(int unit, uint32 search_instruction_info_idx_0, CONST mdb_kaps_instruction_handles_t *search_instruction_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_KAPS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_KAPS_MODULE_ID,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_KAPS_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        MDB_KAPS_MODULE_ID,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info[search_instruction_info_idx_0],
        search_instruction_info,
        mdb_kaps_instruction_handles_t,
        0,
        "mdb_kaps_db_search_instruction_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        MDB_KAPS_MODULE_ID,
        search_instruction_info,
        "mdb_kaps_db[%d]->search_instruction_info[%d]",
        unit, search_instruction_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_KAPS_MODULE_ID,
        mdb_kaps_db_info,
        MDB_KAPS_DB_SEARCH_INSTRUCTION_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_kaps_db_search_instruction_info_get(int unit, uint32 search_instruction_info_idx_0, mdb_kaps_instruction_handles_t *search_instruction_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        MDB_KAPS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_KAPS_MODULE_ID,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        search_instruction_info_idx_0,
        MDB_LPM_NOF_INSTRUCTIONS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        MDB_KAPS_MODULE_ID,
        search_instruction_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_KAPS_MODULE_ID);

    *search_instruction_info = ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info[search_instruction_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        MDB_KAPS_MODULE_ID,
        &((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->search_instruction_info[search_instruction_info_idx_0],
        "mdb_kaps_db[%d]->search_instruction_info[%d]",
        unit, search_instruction_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_KAPS_MODULE_ID,
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
        MDB_KAPS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_KAPS_MODULE_ID,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        big_kaps_large_bb_mode_idx_0,
        MDB_LPM_MAX_NOF_BIG_KAPS_BB_BLK_IDS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_KAPS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        MDB_KAPS_MODULE_ID,
        ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->big_kaps_large_bb_mode[big_kaps_large_bb_mode_idx_0],
        big_kaps_large_bb_mode,
        uint32,
        0,
        "mdb_kaps_db_big_kaps_large_bb_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        MDB_KAPS_MODULE_ID,
        &big_kaps_large_bb_mode,
        "mdb_kaps_db[%d]->big_kaps_large_bb_mode[%d]",
        unit, big_kaps_large_bb_mode_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_KAPS_MODULE_ID,
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
        MDB_KAPS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        MDB_KAPS_MODULE_ID,
        sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        big_kaps_large_bb_mode_idx_0,
        MDB_LPM_MAX_NOF_BIG_KAPS_BB_BLK_IDS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        MDB_KAPS_MODULE_ID,
        big_kaps_large_bb_mode);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        MDB_KAPS_MODULE_ID);

    *big_kaps_large_bb_mode = ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->big_kaps_large_bb_mode[big_kaps_large_bb_mode_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        MDB_KAPS_MODULE_ID,
        &((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->big_kaps_large_bb_mode[big_kaps_large_bb_mode_idx_0],
        "mdb_kaps_db[%d]->big_kaps_large_bb_mode[%d]",
        unit, big_kaps_large_bb_mode_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        MDB_KAPS_MODULE_ID,
        mdb_kaps_db_info,
        MDB_KAPS_DB_BIG_KAPS_LARGE_BB_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




const char *
mdb_kaps_ip_db_id_e_get_name(mdb_kaps_ip_db_id_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("MDB_KAPS_IP_PRIVATE_DB_ID", value, MDB_KAPS_IP_PRIVATE_DB_ID);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("MDB_KAPS_IP_PUBLIC_DB_ID", value, MDB_KAPS_IP_PUBLIC_DB_ID);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("MDB_KAPS_IP_NOF_DB", value, MDB_KAPS_IP_NOF_DB);

    return NULL;
}





mdb_kaps_db_cbs mdb_kaps_db = 	{
	
	mdb_kaps_db_is_init,
	mdb_kaps_db_init,
		{
		
		mdb_kaps_db_db_info_set,
		mdb_kaps_db_db_info_get}
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

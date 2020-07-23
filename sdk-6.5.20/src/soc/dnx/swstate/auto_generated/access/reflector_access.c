
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
#include <soc/dnx/swstate/auto_generated/access/reflector_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/reflector_diagnostic.h>



reflector_sw_db_info_t* reflector_sw_db_info_dummy = NULL;



int
reflector_sw_db_info_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        REFLECTOR_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        REFLECTOR_MODULE_ID,
        ((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID]),
        "reflector_sw_db_info[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
reflector_sw_db_info_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        REFLECTOR_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        REFLECTOR_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        reflector_sw_db_info_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "reflector_sw_db_info_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        REFLECTOR_MODULE_ID,
        ((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID]),
        "reflector_sw_db_info[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        REFLECTOR_MODULE_ID,
        reflector_sw_db_info_info,
        REFLECTOR_SW_DB_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(reflector_sw_db_info_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
reflector_sw_db_info_uc_encap_id_set(int unit, int encap_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        REFLECTOR_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        REFLECTOR_MODULE_ID,
        sw_state_roots_array[unit][REFLECTOR_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        REFLECTOR_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        REFLECTOR_MODULE_ID,
        ((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID])->uc.encap_id,
        encap_id,
        int,
        0,
        "reflector_sw_db_info_uc_encap_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        REFLECTOR_MODULE_ID,
        &encap_id,
        "reflector_sw_db_info[%d]->uc.encap_id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        REFLECTOR_MODULE_ID,
        reflector_sw_db_info_info,
        REFLECTOR_SW_DB_INFO_UC_ENCAP_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
reflector_sw_db_info_uc_encap_id_get(int unit, int *encap_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        REFLECTOR_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        REFLECTOR_MODULE_ID,
        sw_state_roots_array[unit][REFLECTOR_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        REFLECTOR_MODULE_ID,
        encap_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        REFLECTOR_MODULE_ID);

    *encap_id = ((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID])->uc.encap_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        REFLECTOR_MODULE_ID,
        &((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID])->uc.encap_id,
        "reflector_sw_db_info[%d]->uc.encap_id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        REFLECTOR_MODULE_ID,
        reflector_sw_db_info_info,
        REFLECTOR_SW_DB_INFO_UC_ENCAP_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
reflector_sw_db_info_uc_is_allocated_set(int unit, int is_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        REFLECTOR_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        REFLECTOR_MODULE_ID,
        sw_state_roots_array[unit][REFLECTOR_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        REFLECTOR_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        REFLECTOR_MODULE_ID,
        ((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID])->uc.is_allocated,
        is_allocated,
        int,
        0,
        "reflector_sw_db_info_uc_is_allocated_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        REFLECTOR_MODULE_ID,
        &is_allocated,
        "reflector_sw_db_info[%d]->uc.is_allocated",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        REFLECTOR_MODULE_ID,
        reflector_sw_db_info_info,
        REFLECTOR_SW_DB_INFO_UC_IS_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
reflector_sw_db_info_uc_is_allocated_get(int unit, int *is_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        REFLECTOR_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        REFLECTOR_MODULE_ID,
        sw_state_roots_array[unit][REFLECTOR_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        REFLECTOR_MODULE_ID,
        is_allocated);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        REFLECTOR_MODULE_ID);

    *is_allocated = ((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID])->uc.is_allocated;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        REFLECTOR_MODULE_ID,
        &((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID])->uc.is_allocated,
        "reflector_sw_db_info[%d]->uc.is_allocated",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        REFLECTOR_MODULE_ID,
        reflector_sw_db_info_info,
        REFLECTOR_SW_DB_INFO_UC_IS_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
reflector_sw_db_info_sbfd_reflector_cnt_set(int unit, uint32 sbfd_reflector_cnt)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        REFLECTOR_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        REFLECTOR_MODULE_ID,
        sw_state_roots_array[unit][REFLECTOR_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        REFLECTOR_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        REFLECTOR_MODULE_ID,
        ((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID])->sbfd_reflector_cnt,
        sbfd_reflector_cnt,
        uint32,
        0,
        "reflector_sw_db_info_sbfd_reflector_cnt_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        REFLECTOR_MODULE_ID,
        &sbfd_reflector_cnt,
        "reflector_sw_db_info[%d]->sbfd_reflector_cnt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        REFLECTOR_MODULE_ID,
        reflector_sw_db_info_info,
        REFLECTOR_SW_DB_INFO_SBFD_REFLECTOR_CNT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
reflector_sw_db_info_sbfd_reflector_cnt_get(int unit, uint32 *sbfd_reflector_cnt)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        REFLECTOR_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        REFLECTOR_MODULE_ID,
        sw_state_roots_array[unit][REFLECTOR_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        REFLECTOR_MODULE_ID,
        sbfd_reflector_cnt);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        REFLECTOR_MODULE_ID);

    *sbfd_reflector_cnt = ((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID])->sbfd_reflector_cnt;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        REFLECTOR_MODULE_ID,
        &((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID])->sbfd_reflector_cnt,
        "reflector_sw_db_info[%d]->sbfd_reflector_cnt",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        REFLECTOR_MODULE_ID,
        reflector_sw_db_info_info,
        REFLECTOR_SW_DB_INFO_SBFD_REFLECTOR_CNT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





reflector_sw_db_info_cbs reflector_sw_db_info = 	{
	
	reflector_sw_db_info_is_init,
	reflector_sw_db_info_init,
		{
		
			{
			
			reflector_sw_db_info_uc_encap_id_set,
			reflector_sw_db_info_uc_encap_id_get}
		,
			{
			
			reflector_sw_db_info_uc_is_allocated_set,
			reflector_sw_db_info_uc_is_allocated_get}
		}
	,
		{
		
		reflector_sw_db_info_sbfd_reflector_cnt_set,
		reflector_sw_db_info_sbfd_reflector_cnt_get}
	}
;
#undef BSL_LOG_MODULE

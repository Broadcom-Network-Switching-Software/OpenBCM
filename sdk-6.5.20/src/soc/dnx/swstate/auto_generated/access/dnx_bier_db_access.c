
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
#include <soc/dnx/swstate/auto_generated/access/dnx_bier_db_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_bier_db_diagnostic.h>



dnx_bier_db_t* dnx_bier_db_dummy = NULL;



int
dnx_bier_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_BIER_DB_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_bier_db_t*)sw_state_roots_array[unit][DNX_BIER_DB_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_BIER_DB_MODULE_ID,
        ((dnx_bier_db_t*)sw_state_roots_array[unit][DNX_BIER_DB_MODULE_ID]),
        "dnx_bier_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_bier_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_BIER_DB_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_BIER_DB_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_bier_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_bier_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_BIER_DB_MODULE_ID,
        ((dnx_bier_db_t*)sw_state_roots_array[unit][DNX_BIER_DB_MODULE_ID]),
        "dnx_bier_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_BIER_DB_MODULE_ID,
        dnx_bier_db_info,
        DNX_BIER_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_bier_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_bier_db_set_size_set(int unit, uint32 set_size_idx_0, int set_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_BIER_DB_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_BIER_DB_MODULE_ID,
        sw_state_roots_array[unit][DNX_BIER_DB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        set_size_idx_0,
        DNX_DATA_MAX_MULTICAST_PARAMS_NOF_MCDB_ENTRIES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_BIER_DB_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_BIER_DB_MODULE_ID,
        ((dnx_bier_db_t*)sw_state_roots_array[unit][DNX_BIER_DB_MODULE_ID])->set_size[set_size_idx_0],
        set_size,
        int,
        0,
        "dnx_bier_db_set_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_BIER_DB_MODULE_ID,
        &set_size,
        "dnx_bier_db[%d]->set_size[%d]",
        unit, set_size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_BIER_DB_MODULE_ID,
        dnx_bier_db_info,
        DNX_BIER_DB_SET_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_bier_db_set_size_get(int unit, uint32 set_size_idx_0, int *set_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_BIER_DB_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_BIER_DB_MODULE_ID,
        sw_state_roots_array[unit][DNX_BIER_DB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        set_size_idx_0,
        DNX_DATA_MAX_MULTICAST_PARAMS_NOF_MCDB_ENTRIES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_BIER_DB_MODULE_ID,
        set_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_BIER_DB_MODULE_ID);

    *set_size = ((dnx_bier_db_t*)sw_state_roots_array[unit][DNX_BIER_DB_MODULE_ID])->set_size[set_size_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_BIER_DB_MODULE_ID,
        &((dnx_bier_db_t*)sw_state_roots_array[unit][DNX_BIER_DB_MODULE_ID])->set_size[set_size_idx_0],
        "dnx_bier_db[%d]->set_size[%d]",
        unit, set_size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_BIER_DB_MODULE_ID,
        dnx_bier_db_info,
        DNX_BIER_DB_SET_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_bier_db_cbs dnx_bier_db = 	{
	
	dnx_bier_db_is_init,
	dnx_bier_db_init,
		{
		
		dnx_bier_db_set_size_set,
		dnx_bier_db_set_size_get}
	}
;
#undef BSL_LOG_MODULE

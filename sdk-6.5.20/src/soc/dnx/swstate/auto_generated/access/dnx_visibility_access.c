
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
#include <soc/dnx/swstate/auto_generated/access/dnx_visibility_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_visibility_diagnostic.h>



visibility_params* visibility_info_db_dummy = NULL;



int
visibility_info_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_VISIBILITY_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((visibility_params*)sw_state_roots_array[unit][DNX_VISIBILITY_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_VISIBILITY_MODULE_ID,
        ((visibility_params*)sw_state_roots_array[unit][DNX_VISIBILITY_MODULE_ID]),
        "visibility_info_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
visibility_info_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_VISIBILITY_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_VISIBILITY_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        visibility_params,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "visibility_info_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_VISIBILITY_MODULE_ID,
        ((visibility_params*)sw_state_roots_array[unit][DNX_VISIBILITY_MODULE_ID]),
        "visibility_info_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_VISIBILITY_MODULE_ID,
        visibility_info_db_info,
        VISIBILITY_INFO_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(visibility_params),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
visibility_info_db_visib_mode_set(int unit, int visib_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_VISIBILITY_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_VISIBILITY_MODULE_ID,
        sw_state_roots_array[unit][DNX_VISIBILITY_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_VISIBILITY_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_VISIBILITY_MODULE_ID,
        ((visibility_params*)sw_state_roots_array[unit][DNX_VISIBILITY_MODULE_ID])->visib_mode,
        visib_mode,
        int,
        0,
        "visibility_info_db_visib_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_VISIBILITY_MODULE_ID,
        &visib_mode,
        "visibility_info_db[%d]->visib_mode",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_VISIBILITY_MODULE_ID,
        visibility_info_db_info,
        VISIBILITY_INFO_DB_VISIB_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
visibility_info_db_visib_mode_get(int unit, int *visib_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_VISIBILITY_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_VISIBILITY_MODULE_ID,
        sw_state_roots_array[unit][DNX_VISIBILITY_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_VISIBILITY_MODULE_ID,
        visib_mode);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_VISIBILITY_MODULE_ID);

    *visib_mode = ((visibility_params*)sw_state_roots_array[unit][DNX_VISIBILITY_MODULE_ID])->visib_mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_VISIBILITY_MODULE_ID,
        &((visibility_params*)sw_state_roots_array[unit][DNX_VISIBILITY_MODULE_ID])->visib_mode,
        "visibility_info_db[%d]->visib_mode",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_VISIBILITY_MODULE_ID,
        visibility_info_db_info,
        VISIBILITY_INFO_DB_VISIB_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





visibility_info_db_cbs visibility_info_db = 	{
	
	visibility_info_db_is_init,
	visibility_info_db_init,
		{
		
		visibility_info_db_visib_mode_set,
		visibility_info_db_visib_mode_get}
	}
;
#undef BSL_LOG_MODULE

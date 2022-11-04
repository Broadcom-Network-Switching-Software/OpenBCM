
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/rx_trap_access.h>
#include <include/bcm/rx.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/rx_trap_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/rx_trap_layout.h>





int
rx_trap_info_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_RX_TRAP_INFO,
        ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID]),
        "rx_trap_info[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
rx_trap_info_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        rx_trap_info_t,
        DNX_SW_STATE_RX_TRAP_INFO_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "rx_trap_info_init",
        DNX_SW_STATE_DIAG_ALLOC,
        rx_trap_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_RX_TRAP_INFO,
        ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID]),
        "rx_trap_info[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO,
        rx_trap_info_info,
        RX_TRAP_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID]),
        sw_state_layout_array[unit][RX_TRAP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
rx_trap_info_user_trap_info_set(int unit, uint32 user_trap_info_idx_0, CONST rx_trap_user_info_t *user_trap_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        sw_state_roots_array[unit][RX_TRAP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info,
        user_trap_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info[user_trap_info_idx_0],
        user_trap_info,
        rx_trap_user_info_t,
        0,
        "rx_trap_info_user_trap_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        user_trap_info,
        "rx_trap_info[%d]->user_trap_info[%d]",
        unit, user_trap_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        rx_trap_info_info,
        RX_TRAP_INFO_USER_TRAP_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][RX_TRAP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
rx_trap_info_user_trap_info_get(int unit, uint32 user_trap_info_idx_0, rx_trap_user_info_t *user_trap_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        sw_state_roots_array[unit][RX_TRAP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info,
        user_trap_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        user_trap_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        DNX_SW_STATE_DIAG_READ);

    *user_trap_info = ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info[user_trap_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        &((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info[user_trap_info_idx_0],
        "rx_trap_info[%d]->user_trap_info[%d]",
        unit, user_trap_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        rx_trap_info_info,
        RX_TRAP_INFO_USER_TRAP_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][RX_TRAP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
rx_trap_info_user_trap_info_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        sw_state_roots_array[unit][RX_TRAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info,
        rx_trap_user_info_t,
        dnx_data_trap.ingress.nof_user_defined_traps_get(unit),
        sw_state_layout_array[unit][RX_TRAP_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "rx_trap_info_user_trap_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        ((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info,
        "rx_trap_info[%d]->user_trap_info",
        unit,
        dnx_data_trap.ingress.nof_user_defined_traps_get(unit) * sizeof(rx_trap_user_info_t) / sizeof(*((rx_trap_info_t*)sw_state_roots_array[unit][RX_TRAP_MODULE_ID])->user_trap_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO,
        rx_trap_info_info,
        RX_TRAP_INFO_USER_TRAP_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][RX_TRAP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




rx_trap_info_cbs rx_trap_info = 	{
	
	rx_trap_info_is_init,
	rx_trap_info_init,
		{
		
		rx_trap_info_user_trap_info_set,
		rx_trap_info_user_trap_info_get,
		rx_trap_info_user_trap_info_alloc}
	}
;
#undef BSL_LOG_MODULE

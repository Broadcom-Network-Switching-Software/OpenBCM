
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
#include <soc/dnx/swstate/auto_generated/access/dnx_ire_packet_generator_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_ire_packet_generator_diagnostic.h>



dnx_ire_packet_generator_info_struct* ire_packet_generator_info_dummy = NULL;



int
ire_packet_generator_info_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_ire_packet_generator_info_struct*)sw_state_roots_array[unit][DNX_IRE_PACKET_GENERATOR_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        ((dnx_ire_packet_generator_info_struct*)sw_state_roots_array[unit][DNX_IRE_PACKET_GENERATOR_MODULE_ID]),
        "ire_packet_generator_info[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
ire_packet_generator_info_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_ire_packet_generator_info_struct,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "ire_packet_generator_info_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        ((dnx_ire_packet_generator_info_struct*)sw_state_roots_array[unit][DNX_IRE_PACKET_GENERATOR_MODULE_ID]),
        "ire_packet_generator_info[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        ire_packet_generator_info_info,
        IRE_PACKET_GENERATOR_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_ire_packet_generator_info_struct),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
ire_packet_generator_info_ire_datapath_enable_state_set(int unit, uint32 ire_datapath_enable_state)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        sw_state_roots_array[unit][DNX_IRE_PACKET_GENERATOR_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        ((dnx_ire_packet_generator_info_struct*)sw_state_roots_array[unit][DNX_IRE_PACKET_GENERATOR_MODULE_ID])->ire_datapath_enable_state,
        ire_datapath_enable_state,
        uint32,
        0,
        "ire_packet_generator_info_ire_datapath_enable_state_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        &ire_datapath_enable_state,
        "ire_packet_generator_info[%d]->ire_datapath_enable_state",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        ire_packet_generator_info_info,
        IRE_PACKET_GENERATOR_INFO_IRE_DATAPATH_ENABLE_STATE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
ire_packet_generator_info_ire_datapath_enable_state_get(int unit, uint32 *ire_datapath_enable_state)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        sw_state_roots_array[unit][DNX_IRE_PACKET_GENERATOR_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        ire_datapath_enable_state);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID);

    *ire_datapath_enable_state = ((dnx_ire_packet_generator_info_struct*)sw_state_roots_array[unit][DNX_IRE_PACKET_GENERATOR_MODULE_ID])->ire_datapath_enable_state;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        &((dnx_ire_packet_generator_info_struct*)sw_state_roots_array[unit][DNX_IRE_PACKET_GENERATOR_MODULE_ID])->ire_datapath_enable_state,
        "ire_packet_generator_info[%d]->ire_datapath_enable_state",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IRE_PACKET_GENERATOR_MODULE_ID,
        ire_packet_generator_info_info,
        IRE_PACKET_GENERATOR_INFO_IRE_DATAPATH_ENABLE_STATE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





ire_packet_generator_info_cbs ire_packet_generator_info = 	{
	
	ire_packet_generator_info_is_init,
	ire_packet_generator_info_init,
		{
		
		ire_packet_generator_info_ire_datapath_enable_state_set,
		ire_packet_generator_info_ire_datapath_enable_state_get}
	}
;
#undef BSL_LOG_MODULE

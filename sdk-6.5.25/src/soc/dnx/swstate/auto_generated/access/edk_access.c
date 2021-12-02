
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/edk_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/edk_diagnostic.h>





int
edk_state_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EDK_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        EDK_MODULE_ID,
        ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID]),
        "edk_state[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
edk_state_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EDK_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        EDK_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        edk_sw_state,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "edk_state_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        EDK_MODULE_ID,
        ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID]),
        "edk_state[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EDK_MODULE_ID,
        edk_state_info,
        EDK_STATE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID]));

    DNXC_SW_STATE_FUNC_RETURN;
}




int
edk_state_edk_seq_set(int unit, uint32 edk_seq_idx_0, uint32 edk_seq_idx_1, int edk_seq)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EDK_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_WB);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EDK_MODULE_ID,
        sw_state_roots_array[unit][EDK_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        edk_seq_idx_0,
        EDK_SEQ_MAX);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EDK_MODULE_ID,
        ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[edk_seq_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EDK_MODULE_ID,
        ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[edk_seq_idx_0],
        edk_seq_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EDK_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        EDK_MODULE_ID,
        ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[edk_seq_idx_0][edk_seq_idx_1],
        edk_seq,
        int,
        0,
        "edk_state_edk_seq_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        EDK_MODULE_ID,
        &edk_seq,
        "edk_state[%d]->edk_seq[%d][%d]",
        unit, edk_seq_idx_0, edk_seq_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EDK_MODULE_ID,
        edk_state_info,
        EDK_STATE_EDK_SEQ_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
edk_state_edk_seq_get(int unit, uint32 edk_seq_idx_0, uint32 edk_seq_idx_1, int *edk_seq)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EDK_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_WB);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EDK_MODULE_ID,
        sw_state_roots_array[unit][EDK_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        edk_seq_idx_0,
        EDK_SEQ_MAX);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EDK_MODULE_ID,
        edk_seq);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        EDK_MODULE_ID,
        ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[edk_seq_idx_0],
        edk_seq_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        EDK_MODULE_ID,
        ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[edk_seq_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EDK_MODULE_ID);

    *edk_seq = ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[edk_seq_idx_0][edk_seq_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        EDK_MODULE_ID,
        &((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[edk_seq_idx_0][edk_seq_idx_1],
        "edk_state[%d]->edk_seq[%d][%d]",
        unit, edk_seq_idx_0, edk_seq_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EDK_MODULE_ID,
        edk_state_info,
        EDK_STATE_EDK_SEQ_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
edk_state_edk_seq_alloc(int unit, uint32 edk_seq_idx_0, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        EDK_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_ALLOW_WRITES_DURING_WB);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        EDK_MODULE_ID,
        sw_state_roots_array[unit][EDK_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        edk_seq_idx_0,
        EDK_SEQ_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        EDK_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        EDK_MODULE_ID,
        ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[edk_seq_idx_0],
        int,
        nof_instances_to_alloc_1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "edk_state_edk_seq_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        EDK_MODULE_ID,
        ((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[edk_seq_idx_0],
        "edk_state[%d]->edk_seq[%d]",
        unit, edk_seq_idx_0,
        nof_instances_to_alloc_1 * sizeof(int) / sizeof(*((edk_sw_state*)sw_state_roots_array[unit][EDK_MODULE_ID])->edk_seq[edk_seq_idx_0]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        EDK_MODULE_ID,
        edk_state_info,
        EDK_STATE_EDK_SEQ_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





edk_state_cbs edk_state = 	{
	
	edk_state_is_init,
	edk_state_init,
		{
		
		edk_state_edk_seq_set,
		edk_state_edk_seq_get,
		edk_state_edk_seq_alloc}
	}
;
#undef BSL_LOG_MODULE

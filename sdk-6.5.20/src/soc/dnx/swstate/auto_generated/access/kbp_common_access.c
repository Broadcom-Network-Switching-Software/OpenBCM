
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/access/kbp_common_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/kbp_common_diagnostic.h>



kbp_common_sw_state_t* kbp_common_sw_state_dummy = NULL;



int
kbp_common_sw_state_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_COMMON_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((kbp_common_sw_state_t*)sw_state_roots_array[unit][KBP_COMMON_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        KBP_COMMON_MODULE_ID,
        ((kbp_common_sw_state_t*)sw_state_roots_array[unit][KBP_COMMON_MODULE_ID]),
        "kbp_common_sw_state[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_common_sw_state_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_COMMON_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        KBP_COMMON_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        kbp_common_sw_state_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "kbp_common_sw_state_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        KBP_COMMON_MODULE_ID,
        ((kbp_common_sw_state_t*)sw_state_roots_array[unit][KBP_COMMON_MODULE_ID]),
        "kbp_common_sw_state[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        KBP_COMMON_MODULE_ID,
        kbp_common_sw_state_info,
        KBP_COMMON_SW_STATE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(kbp_common_sw_state_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_common_sw_state_kbp_fwd_caching_enabled_set(int unit, int kbp_fwd_caching_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_COMMON_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_COMMON_MODULE_ID,
        sw_state_roots_array[unit][KBP_COMMON_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_COMMON_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        KBP_COMMON_MODULE_ID,
        ((kbp_common_sw_state_t*)sw_state_roots_array[unit][KBP_COMMON_MODULE_ID])->kbp_fwd_caching_enabled,
        kbp_fwd_caching_enabled,
        int,
        0,
        "kbp_common_sw_state_kbp_fwd_caching_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_COMMON_MODULE_ID,
        &kbp_fwd_caching_enabled,
        "kbp_common_sw_state[%d]->kbp_fwd_caching_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_COMMON_MODULE_ID,
        kbp_common_sw_state_info,
        KBP_COMMON_SW_STATE_KBP_FWD_CACHING_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_common_sw_state_kbp_fwd_caching_enabled_get(int unit, int *kbp_fwd_caching_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_COMMON_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_COMMON_MODULE_ID,
        sw_state_roots_array[unit][KBP_COMMON_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_COMMON_MODULE_ID,
        kbp_fwd_caching_enabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_COMMON_MODULE_ID);

    *kbp_fwd_caching_enabled = ((kbp_common_sw_state_t*)sw_state_roots_array[unit][KBP_COMMON_MODULE_ID])->kbp_fwd_caching_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_COMMON_MODULE_ID,
        &((kbp_common_sw_state_t*)sw_state_roots_array[unit][KBP_COMMON_MODULE_ID])->kbp_fwd_caching_enabled,
        "kbp_common_sw_state[%d]->kbp_fwd_caching_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_COMMON_MODULE_ID,
        kbp_common_sw_state_info,
        KBP_COMMON_SW_STATE_KBP_FWD_CACHING_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_common_sw_state_kbp_acl_caching_enabled_set(int unit, int kbp_acl_caching_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_COMMON_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_COMMON_MODULE_ID,
        sw_state_roots_array[unit][KBP_COMMON_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_COMMON_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        KBP_COMMON_MODULE_ID,
        ((kbp_common_sw_state_t*)sw_state_roots_array[unit][KBP_COMMON_MODULE_ID])->kbp_acl_caching_enabled,
        kbp_acl_caching_enabled,
        int,
        0,
        "kbp_common_sw_state_kbp_acl_caching_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_COMMON_MODULE_ID,
        &kbp_acl_caching_enabled,
        "kbp_common_sw_state[%d]->kbp_acl_caching_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_COMMON_MODULE_ID,
        kbp_common_sw_state_info,
        KBP_COMMON_SW_STATE_KBP_ACL_CACHING_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_common_sw_state_kbp_acl_caching_enabled_get(int unit, int *kbp_acl_caching_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_COMMON_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_COMMON_MODULE_ID,
        sw_state_roots_array[unit][KBP_COMMON_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_COMMON_MODULE_ID,
        kbp_acl_caching_enabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_COMMON_MODULE_ID);

    *kbp_acl_caching_enabled = ((kbp_common_sw_state_t*)sw_state_roots_array[unit][KBP_COMMON_MODULE_ID])->kbp_acl_caching_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_COMMON_MODULE_ID,
        &((kbp_common_sw_state_t*)sw_state_roots_array[unit][KBP_COMMON_MODULE_ID])->kbp_acl_caching_enabled,
        "kbp_common_sw_state[%d]->kbp_acl_caching_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_COMMON_MODULE_ID,
        kbp_common_sw_state_info,
        KBP_COMMON_SW_STATE_KBP_ACL_CACHING_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





kbp_common_sw_state_cbs kbp_common_sw_state = 	{
	
	kbp_common_sw_state_is_init,
	kbp_common_sw_state_init,
		{
		
		kbp_common_sw_state_kbp_fwd_caching_enabled_set,
		kbp_common_sw_state_kbp_fwd_caching_enabled_get}
	,
		{
		
		kbp_common_sw_state_kbp_acl_caching_enabled_set,
		kbp_common_sw_state_kbp_acl_caching_enabled_get}
	}
;
#endif  
#endif  
#undef BSL_LOG_MODULE

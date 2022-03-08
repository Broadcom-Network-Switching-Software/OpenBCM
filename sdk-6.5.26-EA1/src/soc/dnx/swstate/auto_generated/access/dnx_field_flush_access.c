
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_field_flush_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_flush_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_flush_layout.h>





int
dnx_field_flush_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID]),
        "dnx_field_flush_sw[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_flush_sw_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_flush_sw_t,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_flush_sw_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_field_flush_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID]),
        "dnx_field_flush_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW,
        dnx_field_flush_sw_info,
        DNX_FIELD_FLUSH_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID]));

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_flush_sw_flush_profile_alloc(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE,
        sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile,
        dnx_field_flush_profile_info_t,
        dnx_data_field.exem_learn_flush_machine.nof_flush_profiles_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_flush_sw_flush_profile_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile,
        "dnx_field_flush_sw[%d]->flush_profile",
        unit,
        dnx_data_field.exem_learn_flush_machine.nof_flush_profiles_get(unit) * sizeof(dnx_field_flush_profile_info_t) / sizeof(*((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE,
        dnx_field_flush_sw_info,
        DNX_FIELD_FLUSH_SW_FLUSH_PROFILE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.exem_learn_flush_machine.nof_flush_profiles_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile[def_val_idx[0]].mapped_fg,
        DNX_FIELD_GROUP_INVALID);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_flush_sw_flush_profile_is_mapped_set(int unit, uint32 flush_profile_idx_0, uint8 is_mapped)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile,
        flush_profile_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile[flush_profile_idx_0].is_mapped,
        is_mapped,
        uint8,
        0,
        "dnx_field_flush_sw_flush_profile_is_mapped_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        &is_mapped,
        "dnx_field_flush_sw[%d]->flush_profile[%d].is_mapped",
        unit, flush_profile_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        dnx_field_flush_sw_info,
        DNX_FIELD_FLUSH_SW_FLUSH_PROFILE_IS_MAPPED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_flush_sw_flush_profile_is_mapped_get(int unit, uint32 flush_profile_idx_0, uint8 *is_mapped)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile,
        flush_profile_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        is_mapped);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        DNX_SW_STATE_DIAG_READ);

    *is_mapped = ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile[flush_profile_idx_0].is_mapped;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        &((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile[flush_profile_idx_0].is_mapped,
        "dnx_field_flush_sw[%d]->flush_profile[%d].is_mapped",
        unit, flush_profile_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
        dnx_field_flush_sw_info,
        DNX_FIELD_FLUSH_SW_FLUSH_PROFILE_IS_MAPPED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_flush_sw_flush_profile_mapped_fg_set(int unit, uint32 flush_profile_idx_0, dnx_field_group_t mapped_fg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile,
        flush_profile_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile[flush_profile_idx_0].mapped_fg,
        mapped_fg,
        dnx_field_group_t,
        0,
        "dnx_field_flush_sw_flush_profile_mapped_fg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        &mapped_fg,
        "dnx_field_flush_sw[%d]->flush_profile[%d].mapped_fg",
        unit, flush_profile_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        dnx_field_flush_sw_info,
        DNX_FIELD_FLUSH_SW_FLUSH_PROFILE_MAPPED_FG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_flush_sw_flush_profile_mapped_fg_get(int unit, uint32 flush_profile_idx_0, dnx_field_group_t *mapped_fg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile,
        flush_profile_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        mapped_fg);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        DNX_SW_STATE_DIAG_READ);

    *mapped_fg = ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile[flush_profile_idx_0].mapped_fg;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        &((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile[flush_profile_idx_0].mapped_fg,
        "dnx_field_flush_sw[%d]->flush_profile[%d].mapped_fg",
        unit, flush_profile_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
        dnx_field_flush_sw_info,
        DNX_FIELD_FLUSH_SW_FLUSH_PROFILE_MAPPED_FG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_field_flush_sw_cbs dnx_field_flush_sw = 	{
	
	dnx_field_flush_sw_is_init,
	dnx_field_flush_sw_init,
		{
		
		dnx_field_flush_sw_flush_profile_alloc,
			{
			
			dnx_field_flush_sw_flush_profile_is_mapped_set,
			dnx_field_flush_sw_flush_profile_is_mapped_get}
		,
			{
			
			dnx_field_flush_sw_flush_profile_mapped_fg_set,
			dnx_field_flush_sw_flush_profile_mapped_fg_get}
		}
	}
;
#undef BSL_LOG_MODULE

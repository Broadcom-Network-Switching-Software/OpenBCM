
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
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_profile_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_profile_diagnostic.h>



dnx_field_tcam_access_profile_sw_t* dnx_field_tcam_access_profile_sw_dummy = NULL;



int
dnx_field_tcam_access_profile_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]),
        "dnx_field_tcam_access_profile_sw[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_tcam_access_profile_sw_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_profile_sw_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]),
        "dnx_field_tcam_access_profile_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_field_tcam_access_profile_sw_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_set(int unit, uint32 access_profiles_idx_0, CONST dnx_field_tcam_access_profile_t *access_profiles)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0],
        access_profiles,
        dnx_field_tcam_access_profile_t,
        0,
        "dnx_field_tcam_access_profile_sw_access_profiles_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        access_profiles,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d]",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_get(int unit, uint32 access_profiles_idx_0, dnx_field_tcam_access_profile_t *access_profiles)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        access_profiles);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    *access_profiles = ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0],
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d]",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_alloc(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        dnx_field_tcam_access_profile_t,
        dnx_data_field.tcam.nof_access_profiles_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_profile_sw_access_profiles_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles",
        unit,
        dnx_data_field.tcam.nof_access_profiles_get(unit) * sizeof(dnx_field_tcam_access_profile_t) / sizeof(*((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.tcam.nof_access_profiles_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[def_val_idx[0]].key_size,
        DNX_FIELD_KEY_LENGTH_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.tcam.nof_access_profiles_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[def_val_idx[0]].action_size,
        DNX_FIELD_ACTION_LENGTH_TYPE_INVALID);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_occupied_set(int unit, uint32 access_profiles_idx_0, uint8 occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].occupied,
        occupied,
        uint8,
        0,
        "dnx_field_tcam_access_profile_sw_access_profiles_occupied_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &occupied,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].occupied",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_OCCUPIED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_occupied_get(int unit, uint32 access_profiles_idx_0, uint8 *occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        occupied);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    *occupied = ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].occupied;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].occupied,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].occupied",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_OCCUPIED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_key_size_set(int unit, uint32 access_profiles_idx_0, dnx_field_key_length_type_e key_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].key_size,
        key_size,
        dnx_field_key_length_type_e,
        0,
        "dnx_field_tcam_access_profile_sw_access_profiles_key_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &key_size,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].key_size",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_KEY_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_key_size_get(int unit, uint32 access_profiles_idx_0, dnx_field_key_length_type_e *key_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        key_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    *key_size = ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].key_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].key_size,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].key_size",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_KEY_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_action_size_set(int unit, uint32 access_profiles_idx_0, dnx_field_action_length_type_e action_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].action_size,
        action_size,
        dnx_field_action_length_type_e,
        0,
        "dnx_field_tcam_access_profile_sw_access_profiles_action_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &action_size,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].action_size",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_ACTION_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_action_size_get(int unit, uint32 access_profiles_idx_0, dnx_field_action_length_type_e *action_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        action_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    *action_size = ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].action_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].action_size,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].action_size",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_ACTION_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_bank_ids_bmp_set(int unit, uint32 access_profiles_idx_0, uint16 bank_ids_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].bank_ids_bmp,
        bank_ids_bmp,
        uint16,
        0,
        "dnx_field_tcam_access_profile_sw_access_profiles_bank_ids_bmp_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &bank_ids_bmp,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].bank_ids_bmp",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_BANK_IDS_BMP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_bank_ids_bmp_get(int unit, uint32 access_profiles_idx_0, uint16 *bank_ids_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        bank_ids_bmp);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    *bank_ids_bmp = ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].bank_ids_bmp;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].bank_ids_bmp,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].bank_ids_bmp",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_BANK_IDS_BMP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_is_direct_set(int unit, uint32 access_profiles_idx_0, uint8 is_direct)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].is_direct,
        is_direct,
        uint8,
        0,
        "dnx_field_tcam_access_profile_sw_access_profiles_is_direct_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &is_direct,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].is_direct",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_IS_DIRECT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_is_direct_get(int unit, uint32 access_profiles_idx_0, uint8 *is_direct)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        is_direct);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    *is_direct = ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].is_direct;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].is_direct,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].is_direct",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_IS_DIRECT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_prefix_set(int unit, uint32 access_profiles_idx_0, uint8 prefix)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].prefix,
        prefix,
        uint8,
        0,
        "dnx_field_tcam_access_profile_sw_access_profiles_prefix_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &prefix,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].prefix",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_PREFIX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_prefix_get(int unit, uint32 access_profiles_idx_0, uint8 *prefix)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        prefix);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    *prefix = ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].prefix;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].prefix,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].prefix",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_PREFIX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_handler_id_set(int unit, uint32 access_profiles_idx_0, uint32 handler_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].handler_id,
        handler_id,
        uint32,
        0,
        "dnx_field_tcam_access_profile_sw_access_profiles_handler_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &handler_id,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].handler_id",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_HANDLER_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_handler_id_get(int unit, uint32 access_profiles_idx_0, uint32 *handler_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles,
        access_profiles_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        handler_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID);

    *handler_id = ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].handler_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        &((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[access_profiles_idx_0].handler_id,
        "dnx_field_tcam_access_profile_sw[%d]->access_profiles[%d].handler_id",
        unit, access_profiles_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
        dnx_field_tcam_access_profile_sw_info,
        DNX_FIELD_TCAM_ACCESS_PROFILE_SW_ACCESS_PROFILES_HANDLER_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_field_tcam_access_profile_sw_cbs dnx_field_tcam_access_profile_sw = 	{
	
	dnx_field_tcam_access_profile_sw_is_init,
	dnx_field_tcam_access_profile_sw_init,
		{
		
		dnx_field_tcam_access_profile_sw_access_profiles_set,
		dnx_field_tcam_access_profile_sw_access_profiles_get,
		dnx_field_tcam_access_profile_sw_access_profiles_alloc,
			{
			
			dnx_field_tcam_access_profile_sw_access_profiles_occupied_set,
			dnx_field_tcam_access_profile_sw_access_profiles_occupied_get}
		,
			{
			
			dnx_field_tcam_access_profile_sw_access_profiles_key_size_set,
			dnx_field_tcam_access_profile_sw_access_profiles_key_size_get}
		,
			{
			
			dnx_field_tcam_access_profile_sw_access_profiles_action_size_set,
			dnx_field_tcam_access_profile_sw_access_profiles_action_size_get}
		,
			{
			
			dnx_field_tcam_access_profile_sw_access_profiles_bank_ids_bmp_set,
			dnx_field_tcam_access_profile_sw_access_profiles_bank_ids_bmp_get}
		,
			{
			
			dnx_field_tcam_access_profile_sw_access_profiles_is_direct_set,
			dnx_field_tcam_access_profile_sw_access_profiles_is_direct_get}
		,
			{
			
			dnx_field_tcam_access_profile_sw_access_profiles_prefix_set,
			dnx_field_tcam_access_profile_sw_access_profiles_prefix_get}
		,
			{
			
			dnx_field_tcam_access_profile_sw_access_profiles_handler_id_set,
			dnx_field_tcam_access_profile_sw_access_profiles_handler_id_get}
		}
	}
;
#undef BSL_LOG_MODULE

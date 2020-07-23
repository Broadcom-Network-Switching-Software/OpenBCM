
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
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_field_action_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_algo_field_action_diagnostic.h>



dnx_algo_field_action_sw_t* dnx_algo_field_action_sw_dummy = NULL;



int
dnx_algo_field_action_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]),
        "dnx_algo_field_action_sw[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_init(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_algo_field_action_sw_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_algo_field_action_sw_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]),
        "dnx_algo_field_action_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_algo_field_action_sw_t),
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[def_val_idx[1]].field_group[def_val_idx[2]],
        DNX_FIELD_GROUP_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.context_state[def_val_idx[1]].priority[def_val_idx[2]],
        DNX_FIELD_ACTION_PRIORITY_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[def_val_idx[1]].field_group[def_val_idx[2]],
        DNX_FIELD_GROUP_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_CONTEXTS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.context_state[def_val_idx[1]].priority[def_val_idx[2]],
        DNX_FIELD_ACTION_PRIORITY_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[def_val_idx[1]].field_group[def_val_idx[2]],
        DNX_FIELD_GROUP_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_CONTEXTS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.context_state[def_val_idx[1]].priority[def_val_idx[2]],
        DNX_FIELD_ACTION_PRIORITY_INVALID);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_set(int unit, CONST dnx_algo_field_action_ipmf2_t *ipmf2)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2,
        ipmf2,
        dnx_algo_field_action_ipmf2_t,
        0,
        "dnx_algo_field_action_sw_ipmf2_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ipmf2,
        "dnx_algo_field_action_sw[%d]->ipmf2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF2_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_get(int unit, dnx_algo_field_action_ipmf2_t *ipmf2)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ipmf2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *ipmf2 = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2,
        "dnx_algo_field_action_sw[%d]->ipmf2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF2_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_fes_state_set(int unit, uint32 fes_state_idx_0, CONST dnx_algo_field_action_fes_state_t *fes_state)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[fes_state_idx_0],
        fes_state,
        dnx_algo_field_action_fes_state_t,
        0,
        "dnx_algo_field_action_sw_ipmf2_fes_state_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        fes_state,
        "dnx_algo_field_action_sw[%d]->ipmf2.fes_state[%d]",
        unit, fes_state_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF2_FES_STATE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_fes_state_get(int unit, uint32 fes_state_idx_0, dnx_algo_field_action_fes_state_t *fes_state)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        fes_state);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *fes_state = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[fes_state_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[fes_state_idx_0],
        "dnx_algo_field_action_sw[%d]->ipmf2.fes_state[%d]",
        unit, fes_state_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF2_FES_STATE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_fes_state_field_group_set(int unit, uint32 fes_state_idx_0, uint32 field_group_idx_0, dnx_field_group_t field_group)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        field_group_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[fes_state_idx_0].field_group[field_group_idx_0],
        field_group,
        dnx_field_group_t,
        0,
        "dnx_algo_field_action_sw_ipmf2_fes_state_field_group_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &field_group,
        "dnx_algo_field_action_sw[%d]->ipmf2.fes_state[%d].field_group[%d]",
        unit, fes_state_idx_0, field_group_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF2_FES_STATE_FIELD_GROUP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_fes_state_field_group_get(int unit, uint32 fes_state_idx_0, uint32 field_group_idx_0, dnx_field_group_t *field_group)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        field_group);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        field_group_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *field_group = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[fes_state_idx_0].field_group[field_group_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[fes_state_idx_0].field_group[field_group_idx_0],
        "dnx_algo_field_action_sw[%d]->ipmf2.fes_state[%d].field_group[%d]",
        unit, fes_state_idx_0, field_group_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF2_FES_STATE_FIELD_GROUP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_set(int unit, uint32 fes_state_idx_0, uint32 place_in_fg_idx_0, uint8 place_in_fg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        place_in_fg_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[fes_state_idx_0].place_in_fg[place_in_fg_idx_0],
        place_in_fg,
        uint8,
        0,
        "dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &place_in_fg,
        "dnx_algo_field_action_sw[%d]->ipmf2.fes_state[%d].place_in_fg[%d]",
        unit, fes_state_idx_0, place_in_fg_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF2_FES_STATE_PLACE_IN_FG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_get(int unit, uint32 fes_state_idx_0, uint32 place_in_fg_idx_0, uint8 *place_in_fg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        place_in_fg);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        place_in_fg_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *place_in_fg = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[fes_state_idx_0].place_in_fg[place_in_fg_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[fes_state_idx_0].place_in_fg[place_in_fg_idx_0],
        "dnx_algo_field_action_sw[%d]->ipmf2.fes_state[%d].place_in_fg[%d]",
        unit, fes_state_idx_0, place_in_fg_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF2_FES_STATE_PLACE_IN_FG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_set(int unit, uint32 fes_state_idx_0, uint32 mask_is_alloc_idx_0, uint8 mask_is_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        mask_is_alloc_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[fes_state_idx_0].mask_is_alloc[mask_is_alloc_idx_0],
        mask_is_alloc,
        uint8,
        0,
        "dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &mask_is_alloc,
        "dnx_algo_field_action_sw[%d]->ipmf2.fes_state[%d].mask_is_alloc[%d]",
        unit, fes_state_idx_0, mask_is_alloc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF2_FES_STATE_MASK_IS_ALLOC_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_get(int unit, uint32 fes_state_idx_0, uint32 mask_is_alloc_idx_0, uint8 *mask_is_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        mask_is_alloc);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        mask_is_alloc_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *mask_is_alloc = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[fes_state_idx_0].mask_is_alloc[mask_is_alloc_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[fes_state_idx_0].mask_is_alloc[mask_is_alloc_idx_0],
        "dnx_algo_field_action_sw[%d]->ipmf2.fes_state[%d].mask_is_alloc[%d]",
        unit, fes_state_idx_0, mask_is_alloc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF2_FES_STATE_MASK_IS_ALLOC_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_context_state_priority_set(int unit, uint32 context_state_idx_0, uint32 priority_idx_0, dnx_field_action_priority_t priority)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        priority_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.context_state[context_state_idx_0].priority[priority_idx_0],
        priority,
        dnx_field_action_priority_t,
        0,
        "dnx_algo_field_action_sw_ipmf2_context_state_priority_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &priority,
        "dnx_algo_field_action_sw[%d]->ipmf2.context_state[%d].priority[%d]",
        unit, context_state_idx_0, priority_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF2_CONTEXT_STATE_PRIORITY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_context_state_priority_get(int unit, uint32 context_state_idx_0, uint32 priority_idx_0, dnx_field_action_priority_t *priority)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        priority);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        priority_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *priority = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.context_state[context_state_idx_0].priority[priority_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.context_state[context_state_idx_0].priority[priority_idx_0],
        "dnx_algo_field_action_sw[%d]->ipmf2.context_state[%d].priority[%d]",
        unit, context_state_idx_0, priority_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF2_CONTEXT_STATE_PRIORITY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_set(int unit, CONST dnx_algo_field_action_ipmf3_t *ipmf3)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3,
        ipmf3,
        dnx_algo_field_action_ipmf3_t,
        0,
        "dnx_algo_field_action_sw_ipmf3_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ipmf3,
        "dnx_algo_field_action_sw[%d]->ipmf3",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF3_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_get(int unit, dnx_algo_field_action_ipmf3_t *ipmf3)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ipmf3);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *ipmf3 = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3,
        "dnx_algo_field_action_sw[%d]->ipmf3",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF3_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_fes_state_set(int unit, uint32 fes_state_idx_0, CONST dnx_algo_field_action_fes_state_t *fes_state)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[fes_state_idx_0],
        fes_state,
        dnx_algo_field_action_fes_state_t,
        0,
        "dnx_algo_field_action_sw_ipmf3_fes_state_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        fes_state,
        "dnx_algo_field_action_sw[%d]->ipmf3.fes_state[%d]",
        unit, fes_state_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF3_FES_STATE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_fes_state_get(int unit, uint32 fes_state_idx_0, dnx_algo_field_action_fes_state_t *fes_state)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        fes_state);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *fes_state = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[fes_state_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[fes_state_idx_0],
        "dnx_algo_field_action_sw[%d]->ipmf3.fes_state[%d]",
        unit, fes_state_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF3_FES_STATE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_fes_state_field_group_set(int unit, uint32 fes_state_idx_0, uint32 field_group_idx_0, dnx_field_group_t field_group)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        field_group_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[fes_state_idx_0].field_group[field_group_idx_0],
        field_group,
        dnx_field_group_t,
        0,
        "dnx_algo_field_action_sw_ipmf3_fes_state_field_group_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &field_group,
        "dnx_algo_field_action_sw[%d]->ipmf3.fes_state[%d].field_group[%d]",
        unit, fes_state_idx_0, field_group_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF3_FES_STATE_FIELD_GROUP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_fes_state_field_group_get(int unit, uint32 fes_state_idx_0, uint32 field_group_idx_0, dnx_field_group_t *field_group)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        field_group);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        field_group_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *field_group = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[fes_state_idx_0].field_group[field_group_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[fes_state_idx_0].field_group[field_group_idx_0],
        "dnx_algo_field_action_sw[%d]->ipmf3.fes_state[%d].field_group[%d]",
        unit, fes_state_idx_0, field_group_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF3_FES_STATE_FIELD_GROUP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_set(int unit, uint32 fes_state_idx_0, uint32 place_in_fg_idx_0, uint8 place_in_fg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        place_in_fg_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[fes_state_idx_0].place_in_fg[place_in_fg_idx_0],
        place_in_fg,
        uint8,
        0,
        "dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &place_in_fg,
        "dnx_algo_field_action_sw[%d]->ipmf3.fes_state[%d].place_in_fg[%d]",
        unit, fes_state_idx_0, place_in_fg_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF3_FES_STATE_PLACE_IN_FG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_get(int unit, uint32 fes_state_idx_0, uint32 place_in_fg_idx_0, uint8 *place_in_fg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        place_in_fg);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        place_in_fg_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *place_in_fg = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[fes_state_idx_0].place_in_fg[place_in_fg_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[fes_state_idx_0].place_in_fg[place_in_fg_idx_0],
        "dnx_algo_field_action_sw[%d]->ipmf3.fes_state[%d].place_in_fg[%d]",
        unit, fes_state_idx_0, place_in_fg_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF3_FES_STATE_PLACE_IN_FG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_set(int unit, uint32 fes_state_idx_0, uint32 mask_is_alloc_idx_0, uint8 mask_is_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        mask_is_alloc_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[fes_state_idx_0].mask_is_alloc[mask_is_alloc_idx_0],
        mask_is_alloc,
        uint8,
        0,
        "dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &mask_is_alloc,
        "dnx_algo_field_action_sw[%d]->ipmf3.fes_state[%d].mask_is_alloc[%d]",
        unit, fes_state_idx_0, mask_is_alloc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF3_FES_STATE_MASK_IS_ALLOC_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_get(int unit, uint32 fes_state_idx_0, uint32 mask_is_alloc_idx_0, uint8 *mask_is_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        mask_is_alloc);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        mask_is_alloc_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *mask_is_alloc = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[fes_state_idx_0].mask_is_alloc[mask_is_alloc_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[fes_state_idx_0].mask_is_alloc[mask_is_alloc_idx_0],
        "dnx_algo_field_action_sw[%d]->ipmf3.fes_state[%d].mask_is_alloc[%d]",
        unit, fes_state_idx_0, mask_is_alloc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF3_FES_STATE_MASK_IS_ALLOC_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_context_state_priority_set(int unit, uint32 context_state_idx_0, uint32 priority_idx_0, dnx_field_action_priority_t priority)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_CONTEXTS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        priority_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.context_state[context_state_idx_0].priority[priority_idx_0],
        priority,
        dnx_field_action_priority_t,
        0,
        "dnx_algo_field_action_sw_ipmf3_context_state_priority_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &priority,
        "dnx_algo_field_action_sw[%d]->ipmf3.context_state[%d].priority[%d]",
        unit, context_state_idx_0, priority_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF3_CONTEXT_STATE_PRIORITY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_context_state_priority_get(int unit, uint32 context_state_idx_0, uint32 priority_idx_0, dnx_field_action_priority_t *priority)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        priority);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        priority_idx_0,
        DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *priority = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.context_state[context_state_idx_0].priority[priority_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.context_state[context_state_idx_0].priority[priority_idx_0],
        "dnx_algo_field_action_sw[%d]->ipmf3.context_state[%d].priority[%d]",
        unit, context_state_idx_0, priority_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_IPMF3_CONTEXT_STATE_PRIORITY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_set(int unit, CONST dnx_algo_field_action_epmf_t *epmf)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf,
        epmf,
        dnx_algo_field_action_epmf_t,
        0,
        "dnx_algo_field_action_sw_epmf_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        epmf,
        "dnx_algo_field_action_sw[%d]->epmf",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_EPMF_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_get(int unit, dnx_algo_field_action_epmf_t *epmf)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        epmf);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *epmf = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf,
        "dnx_algo_field_action_sw[%d]->epmf",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_EPMF_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_fes_state_set(int unit, uint32 fes_state_idx_0, CONST dnx_algo_field_action_fes_state_t *fes_state)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[fes_state_idx_0],
        fes_state,
        dnx_algo_field_action_fes_state_t,
        0,
        "dnx_algo_field_action_sw_epmf_fes_state_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        fes_state,
        "dnx_algo_field_action_sw[%d]->epmf.fes_state[%d]",
        unit, fes_state_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_EPMF_FES_STATE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_fes_state_get(int unit, uint32 fes_state_idx_0, dnx_algo_field_action_fes_state_t *fes_state)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        fes_state);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *fes_state = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[fes_state_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[fes_state_idx_0],
        "dnx_algo_field_action_sw[%d]->epmf.fes_state[%d]",
        unit, fes_state_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_EPMF_FES_STATE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_fes_state_field_group_set(int unit, uint32 fes_state_idx_0, uint32 field_group_idx_0, dnx_field_group_t field_group)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        field_group_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[fes_state_idx_0].field_group[field_group_idx_0],
        field_group,
        dnx_field_group_t,
        0,
        "dnx_algo_field_action_sw_epmf_fes_state_field_group_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &field_group,
        "dnx_algo_field_action_sw[%d]->epmf.fes_state[%d].field_group[%d]",
        unit, fes_state_idx_0, field_group_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_EPMF_FES_STATE_FIELD_GROUP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_fes_state_field_group_get(int unit, uint32 fes_state_idx_0, uint32 field_group_idx_0, dnx_field_group_t *field_group)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        field_group);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        field_group_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *field_group = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[fes_state_idx_0].field_group[field_group_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[fes_state_idx_0].field_group[field_group_idx_0],
        "dnx_algo_field_action_sw[%d]->epmf.fes_state[%d].field_group[%d]",
        unit, fes_state_idx_0, field_group_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_EPMF_FES_STATE_FIELD_GROUP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_set(int unit, uint32 fes_state_idx_0, uint32 place_in_fg_idx_0, uint8 place_in_fg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        place_in_fg_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[fes_state_idx_0].place_in_fg[place_in_fg_idx_0],
        place_in_fg,
        uint8,
        0,
        "dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &place_in_fg,
        "dnx_algo_field_action_sw[%d]->epmf.fes_state[%d].place_in_fg[%d]",
        unit, fes_state_idx_0, place_in_fg_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_EPMF_FES_STATE_PLACE_IN_FG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_get(int unit, uint32 fes_state_idx_0, uint32 place_in_fg_idx_0, uint8 *place_in_fg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        place_in_fg);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        place_in_fg_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *place_in_fg = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[fes_state_idx_0].place_in_fg[place_in_fg_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[fes_state_idx_0].place_in_fg[place_in_fg_idx_0],
        "dnx_algo_field_action_sw[%d]->epmf.fes_state[%d].place_in_fg[%d]",
        unit, fes_state_idx_0, place_in_fg_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_EPMF_FES_STATE_PLACE_IN_FG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_set(int unit, uint32 fes_state_idx_0, uint32 mask_is_alloc_idx_0, uint8 mask_is_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        mask_is_alloc_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[fes_state_idx_0].mask_is_alloc[mask_is_alloc_idx_0],
        mask_is_alloc,
        uint8,
        0,
        "dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &mask_is_alloc,
        "dnx_algo_field_action_sw[%d]->epmf.fes_state[%d].mask_is_alloc[%d]",
        unit, fes_state_idx_0, mask_is_alloc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_EPMF_FES_STATE_MASK_IS_ALLOC_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_get(int unit, uint32 fes_state_idx_0, uint32 mask_is_alloc_idx_0, uint8 *mask_is_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fes_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        mask_is_alloc);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        mask_is_alloc_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *mask_is_alloc = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[fes_state_idx_0].mask_is_alloc[mask_is_alloc_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[fes_state_idx_0].mask_is_alloc[mask_is_alloc_idx_0],
        "dnx_algo_field_action_sw[%d]->epmf.fes_state[%d].mask_is_alloc[%d]",
        unit, fes_state_idx_0, mask_is_alloc_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_EPMF_FES_STATE_MASK_IS_ALLOC_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_context_state_priority_set(int unit, uint32 context_state_idx_0, uint32 priority_idx_0, dnx_field_action_priority_t priority)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_CONTEXTS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        priority_idx_0,
        DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.context_state[context_state_idx_0].priority[priority_idx_0],
        priority,
        dnx_field_action_priority_t,
        0,
        "dnx_algo_field_action_sw_epmf_context_state_priority_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &priority,
        "dnx_algo_field_action_sw[%d]->epmf.context_state[%d].priority[%d]",
        unit, context_state_idx_0, priority_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_EPMF_CONTEXT_STATE_PRIORITY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_context_state_priority_get(int unit, uint32 context_state_idx_0, uint32 priority_idx_0, dnx_field_action_priority_t *priority)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_state_idx_0,
        DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        priority);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        priority_idx_0,
        DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID);

    *priority = ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.context_state[context_state_idx_0].priority[priority_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.context_state[context_state_idx_0].priority[priority_idx_0],
        "dnx_algo_field_action_sw[%d]->epmf.context_state[%d].priority[%d]",
        unit, context_state_idx_0, priority_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_FIELD_ACTION_MODULE_ID,
        dnx_algo_field_action_sw_info,
        DNX_ALGO_FIELD_ACTION_SW_EPMF_CONTEXT_STATE_PRIORITY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_algo_field_action_sw_cbs dnx_algo_field_action_sw = 	{
	
	dnx_algo_field_action_sw_is_init,
	dnx_algo_field_action_sw_init,
		{
		
		dnx_algo_field_action_sw_ipmf2_set,
		dnx_algo_field_action_sw_ipmf2_get,
			{
			
			dnx_algo_field_action_sw_ipmf2_fes_state_set,
			dnx_algo_field_action_sw_ipmf2_fes_state_get,
				{
				
				dnx_algo_field_action_sw_ipmf2_fes_state_field_group_set,
				dnx_algo_field_action_sw_ipmf2_fes_state_field_group_get}
			,
				{
				
				dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_set,
				dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_get}
			,
				{
				
				dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_set,
				dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_get}
			}
		,
			{
			
				{
				
				dnx_algo_field_action_sw_ipmf2_context_state_priority_set,
				dnx_algo_field_action_sw_ipmf2_context_state_priority_get}
			}
		}
	,
		{
		
		dnx_algo_field_action_sw_ipmf3_set,
		dnx_algo_field_action_sw_ipmf3_get,
			{
			
			dnx_algo_field_action_sw_ipmf3_fes_state_set,
			dnx_algo_field_action_sw_ipmf3_fes_state_get,
				{
				
				dnx_algo_field_action_sw_ipmf3_fes_state_field_group_set,
				dnx_algo_field_action_sw_ipmf3_fes_state_field_group_get}
			,
				{
				
				dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_set,
				dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_get}
			,
				{
				
				dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_set,
				dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_get}
			}
		,
			{
			
				{
				
				dnx_algo_field_action_sw_ipmf3_context_state_priority_set,
				dnx_algo_field_action_sw_ipmf3_context_state_priority_get}
			}
		}
	,
		{
		
		dnx_algo_field_action_sw_epmf_set,
		dnx_algo_field_action_sw_epmf_get,
			{
			
			dnx_algo_field_action_sw_epmf_fes_state_set,
			dnx_algo_field_action_sw_epmf_fes_state_get,
				{
				
				dnx_algo_field_action_sw_epmf_fes_state_field_group_set,
				dnx_algo_field_action_sw_epmf_fes_state_field_group_get}
			,
				{
				
				dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_set,
				dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_get}
			,
				{
				
				dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_set,
				dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_get}
			}
		,
			{
			
				{
				
				dnx_algo_field_action_sw_epmf_context_state_priority_set,
				dnx_algo_field_action_sw_epmf_context_state_priority_get}
			}
		}
	}
;
#undef BSL_LOG_MODULE

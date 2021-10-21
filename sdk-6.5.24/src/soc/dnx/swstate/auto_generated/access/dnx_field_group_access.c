
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_group_diagnostic.h>





int
dnx_field_group_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]),
        "dnx_field_group_sw[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_group_sw_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_group_sw_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]),
        "dnx_field_group_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_field_group_sw_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_alloc(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        dnx_field_group_fg_sw_info_t,
        dnx_data_field.group.nof_fgs_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_group_sw_fg_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        "dnx_field_group_sw[%d]->fg_info",
        unit,
        dnx_data_field.group.nof_fgs_get(unit) * sizeof(dnx_field_group_fg_sw_info_t) / sizeof(*((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].field_stage,
        DNX_FIELD_STAGE_NOF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].fg_type,
        DNX_FIELD_GROUP_TYPE_NOF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].context_info[def_val_idx[1]].key_id.id[def_val_idx[2]],
        DNX_FIELD_KEY_ID_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].context_info[def_val_idx[1]].qual_attach_info[def_val_idx[2]].input_type,
        DNX_FIELD_INPUT_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].context_info[def_val_idx[1]].qual_attach_info[def_val_idx[2]].input_arg,
        0xFFFF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].context_info[def_val_idx[1]].qual_attach_info[def_val_idx[2]].offset,
        0xFFFF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].context_info[def_val_idx[1]].qual_attach_info[def_val_idx[2]].base_qual,
        DNX_FIELD_QUAL_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].context_info[def_val_idx[1]].flags,
        DNX_FIELD_GROUP_CONTEXT_ATTACH_FLAG_NOF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].key_template.key_qual_map[def_val_idx[1]].qual_type,
        DNX_FIELD_QUAL_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].key_length_type,
        DNX_FIELD_KEY_LENGTH_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].action_length_type,
        DNX_FIELD_ACTION_LENGTH_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].actions_payload_info.actions_on_payload_info[def_val_idx[1]].dnx_action,
        DNX_FIELD_ACTION_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].dbal_table_id,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].flags,
        DNX_FIELD_GROUP_ADD_FLAG_NOF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.group.nof_fgs_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].tcam_info.cache_mode,
        DNX_FIELD_GROUP_CACHE_MODE_CLEAR);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_field_stage_set(int unit, uint32 fg_info_idx_0, dnx_field_stage_e field_stage)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].field_stage,
        field_stage,
        dnx_field_stage_e,
        0,
        "dnx_field_group_sw_fg_info_field_stage_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &field_stage,
        "dnx_field_group_sw[%d]->fg_info[%d].field_stage",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_FIELD_STAGE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_field_stage_get(int unit, uint32 fg_info_idx_0, dnx_field_stage_e *field_stage)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        field_stage);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *field_stage = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].field_stage;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].field_stage,
        "dnx_field_group_sw[%d]->fg_info[%d].field_stage",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_FIELD_STAGE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_fg_type_set(int unit, uint32 fg_info_idx_0, dnx_field_group_type_e fg_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].fg_type,
        fg_type,
        dnx_field_group_type_e,
        0,
        "dnx_field_group_sw_fg_info_fg_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &fg_type,
        "dnx_field_group_sw[%d]->fg_info[%d].fg_type",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_FG_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_fg_type_get(int unit, uint32 fg_info_idx_0, dnx_field_group_type_e *fg_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        fg_type);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *fg_type = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].fg_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].fg_type,
        "dnx_field_group_sw[%d]->fg_info[%d].fg_type",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_FG_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_name_set(int unit, uint32 fg_info_idx_0, CONST field_group_name_t *name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].name,
        name,
        field_group_name_t,
        0,
        "dnx_field_group_sw_fg_info_name_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        name,
        "dnx_field_group_sw[%d]->fg_info[%d].name",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_NAME_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_name_get(int unit, uint32 fg_info_idx_0, field_group_name_t *name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        name);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *name = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].name;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].name,
        "dnx_field_group_sw[%d]->fg_info[%d].name",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_NAME_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_name_value_set(int unit, uint32 fg_info_idx_0, uint32 value_idx_0, char value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        value_idx_0,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].name.value[value_idx_0],
        value,
        char,
        0,
        "dnx_field_group_sw_fg_info_name_value_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &value,
        "dnx_field_group_sw[%d]->fg_info[%d].name.value[%d]",
        unit, fg_info_idx_0, value_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_name_value_get(int unit, uint32 fg_info_idx_0, uint32 value_idx_0, char *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        value);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        value_idx_0,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *value = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].name.value[value_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].name.value[value_idx_0],
        "dnx_field_group_sw[%d]->fg_info[%d].name.value[%d]",
        unit, fg_info_idx_0, value_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, CONST dnx_field_attach_context_info_t *context_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0],
        context_info,
        dnx_field_attach_context_info_t,
        0,
        "dnx_field_group_sw_fg_info_context_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        context_info,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d]",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, dnx_field_attach_context_info_t *context_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *context_info = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0],
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d]",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_context_info_valid_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 context_info_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].context_info_valid,
        context_info_valid,
        uint8,
        0,
        "dnx_field_group_sw_fg_info_context_info_context_info_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &context_info_valid,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].context_info_valid",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_CONTEXT_INFO_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_context_info_valid_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 *context_info_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        context_info_valid);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *context_info_valid = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].context_info_valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].context_info_valid,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].context_info_valid",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_CONTEXT_INFO_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, CONST dnx_field_key_id_t *key_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id,
        key_id,
        dnx_field_key_id_t,
        0,
        "dnx_field_group_sw_fg_info_context_info_key_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        key_id,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, dnx_field_key_id_t *key_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        key_id);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *key_id = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_id_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.id[id_idx_0],
        id,
        dbal_enum_value_field_field_key_e,
        0,
        "dnx_field_group_sw_fg_info_context_info_key_id_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &id,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.id[%d]",
        unit, fg_info_idx_0, context_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_id_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e *id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        id);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *id = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.id[id_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.id[id_idx_0],
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.id[%d]",
        unit, fg_info_idx_0, context_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_offset_on_first_key_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 offset_on_first_key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.offset_on_first_key,
        offset_on_first_key,
        uint8,
        0,
        "dnx_field_group_sw_fg_info_context_info_key_id_offset_on_first_key_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &offset_on_first_key,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.offset_on_first_key",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_OFFSET_ON_FIRST_KEY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_offset_on_first_key_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 *offset_on_first_key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        offset_on_first_key);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *offset_on_first_key = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.offset_on_first_key;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.offset_on_first_key,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.offset_on_first_key",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_OFFSET_ON_FIRST_KEY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_nof_bits_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 nof_bits_idx_0, uint8 nof_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        nof_bits_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.nof_bits[nof_bits_idx_0],
        nof_bits,
        uint8,
        0,
        "dnx_field_group_sw_fg_info_context_info_key_id_nof_bits_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &nof_bits,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.nof_bits[%d]",
        unit, fg_info_idx_0, context_info_idx_0, nof_bits_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_NOF_BITS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_nof_bits_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 nof_bits_idx_0, uint8 *nof_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        nof_bits);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        nof_bits_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *nof_bits = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.nof_bits[nof_bits_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.nof_bits[nof_bits_idx_0],
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.nof_bits[%d]",
        unit, fg_info_idx_0, context_info_idx_0, nof_bits_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_NOF_BITS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_qual_attach_info_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, CONST dnx_field_qual_attach_info_t *qual_attach_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0],
        qual_attach_info,
        dnx_field_qual_attach_info_t,
        0,
        "dnx_field_group_sw_fg_info_context_info_qual_attach_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        qual_attach_info,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].qual_attach_info[%d]",
        unit, fg_info_idx_0, context_info_idx_0, qual_attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_qual_attach_info_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, dnx_field_qual_attach_info_t *qual_attach_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        qual_attach_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *qual_attach_info = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0],
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].qual_attach_info[%d]",
        unit, fg_info_idx_0, context_info_idx_0, qual_attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_type_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, dnx_field_input_type_e input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0].input_type,
        input_type,
        dnx_field_input_type_e,
        0,
        "dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &input_type,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].qual_attach_info[%d].input_type",
        unit, fg_info_idx_0, context_info_idx_0, qual_attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_type_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, dnx_field_input_type_e *input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        input_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *input_type = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0].input_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0].input_type,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].qual_attach_info[%d].input_type",
        unit, fg_info_idx_0, context_info_idx_0, qual_attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_arg_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, int input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0].input_arg,
        input_arg,
        int,
        0,
        "dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_arg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &input_arg,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].qual_attach_info[%d].input_arg",
        unit, fg_info_idx_0, context_info_idx_0, qual_attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_arg_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, int *input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        input_arg);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *input_arg = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0].input_arg;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0].input_arg,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].qual_attach_info[%d].input_arg",
        unit, fg_info_idx_0, context_info_idx_0, qual_attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_qual_attach_info_offset_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, int offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0].offset,
        offset,
        int,
        0,
        "dnx_field_group_sw_fg_info_context_info_qual_attach_info_offset_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &offset,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].qual_attach_info[%d].offset",
        unit, fg_info_idx_0, context_info_idx_0, qual_attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_qual_attach_info_offset_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, int *offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        offset);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *offset = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0].offset;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0].offset,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].qual_attach_info[%d].offset",
        unit, fg_info_idx_0, context_info_idx_0, qual_attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_qual_attach_info_base_qual_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, dnx_field_qual_t base_qual)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0].base_qual,
        base_qual,
        dnx_field_qual_t,
        0,
        "dnx_field_group_sw_fg_info_context_info_qual_attach_info_base_qual_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &base_qual,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].qual_attach_info[%d].base_qual",
        unit, fg_info_idx_0, context_info_idx_0, qual_attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_BASE_QUAL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_qual_attach_info_base_qual_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, dnx_field_qual_t *base_qual)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        base_qual);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *base_qual = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0].base_qual;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].qual_attach_info[qual_attach_info_idx_0].base_qual,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].qual_attach_info[%d].base_qual",
        unit, fg_info_idx_0, context_info_idx_0, qual_attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_BASE_QUAL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_nof_cascading_refs_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint16 nof_cascading_refs)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].nof_cascading_refs,
        nof_cascading_refs,
        uint16,
        0,
        "dnx_field_group_sw_fg_info_context_info_nof_cascading_refs_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &nof_cascading_refs,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].nof_cascading_refs",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_NOF_CASCADING_REFS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_nof_cascading_refs_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint16 *nof_cascading_refs)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        nof_cascading_refs);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *nof_cascading_refs = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].nof_cascading_refs;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].nof_cascading_refs,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].nof_cascading_refs",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_NOF_CASCADING_REFS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_payload_offset_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 payload_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].payload_offset,
        payload_offset,
        uint32,
        0,
        "dnx_field_group_sw_fg_info_context_info_payload_offset_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &payload_offset,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].payload_offset",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_PAYLOAD_OFFSET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_payload_offset_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 *payload_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        payload_offset);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *payload_offset = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].payload_offset;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].payload_offset,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].payload_offset",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_PAYLOAD_OFFSET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_payload_id_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 payload_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].payload_id,
        payload_id,
        uint32,
        0,
        "dnx_field_group_sw_fg_info_context_info_payload_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &payload_id,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].payload_id",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_PAYLOAD_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_payload_id_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 *payload_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        payload_id);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *payload_id = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].payload_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].payload_id,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].payload_id",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_PAYLOAD_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_flags_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, dnx_field_group_context_attach_flags_e flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].flags,
        flags,
        dnx_field_group_context_attach_flags_e,
        0,
        "dnx_field_group_sw_fg_info_context_info_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &flags,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].flags",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_flags_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, dnx_field_group_context_attach_flags_e *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        flags);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        context_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *flags = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].flags,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].flags",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_key_template_set(int unit, uint32 fg_info_idx_0, CONST dnx_field_key_template_t *key_template)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_template,
        key_template,
        dnx_field_key_template_t,
        0,
        "dnx_field_group_sw_fg_info_key_template_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        key_template,
        "dnx_field_group_sw[%d]->fg_info[%d].key_template",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_key_template_get(int unit, uint32 fg_info_idx_0, dnx_field_key_template_t *key_template)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        key_template);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *key_template = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_template;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_template,
        "dnx_field_group_sw[%d]->fg_info[%d].key_template",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_key_template_key_qual_map_qual_type_set(int unit, uint32 fg_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        qual_type,
        dnx_field_qual_t,
        0,
        "dnx_field_group_sw_fg_info_key_template_key_qual_map_qual_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &qual_type,
        "dnx_field_group_sw[%d]->fg_info[%d].key_template.key_qual_map[%d].qual_type",
        unit, fg_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_key_template_key_qual_map_qual_type_get(int unit, uint32 fg_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t *qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        qual_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *qual_type = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        "dnx_field_group_sw[%d]->fg_info[%d].key_template.key_qual_map[%d].qual_type",
        unit, fg_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_key_template_key_qual_map_lsb_set(int unit, uint32 fg_info_idx_0, uint32 key_qual_map_idx_0, uint16 lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb,
        lsb,
        uint16,
        0,
        "dnx_field_group_sw_fg_info_key_template_key_qual_map_lsb_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &lsb,
        "dnx_field_group_sw[%d]->fg_info[%d].key_template.key_qual_map[%d].lsb",
        unit, fg_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_key_template_key_qual_map_lsb_get(int unit, uint32 fg_info_idx_0, uint32 key_qual_map_idx_0, uint16 *lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        lsb);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *lsb = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb,
        "dnx_field_group_sw[%d]->fg_info[%d].key_template.key_qual_map[%d].lsb",
        unit, fg_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_key_template_key_qual_map_size_set(int unit, uint32 fg_info_idx_0, uint32 key_qual_map_idx_0, uint8 size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size,
        size,
        uint8,
        0,
        "dnx_field_group_sw_fg_info_key_template_key_qual_map_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &size,
        "dnx_field_group_sw[%d]->fg_info[%d].key_template.key_qual_map[%d].size",
        unit, fg_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_key_template_key_qual_map_size_get(int unit, uint32 fg_info_idx_0, uint32 key_qual_map_idx_0, uint8 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *size = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size,
        "dnx_field_group_sw[%d]->fg_info[%d].key_template.key_qual_map[%d].size",
        unit, fg_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_key_length_type_set(int unit, uint32 fg_info_idx_0, dnx_field_key_length_type_e key_length_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_length_type,
        key_length_type,
        dnx_field_key_length_type_e,
        0,
        "dnx_field_group_sw_fg_info_key_length_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &key_length_type,
        "dnx_field_group_sw[%d]->fg_info[%d].key_length_type",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_KEY_LENGTH_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_key_length_type_get(int unit, uint32 fg_info_idx_0, dnx_field_key_length_type_e *key_length_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        key_length_type);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *key_length_type = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_length_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].key_length_type,
        "dnx_field_group_sw[%d]->fg_info[%d].key_length_type",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_KEY_LENGTH_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_action_length_type_set(int unit, uint32 fg_info_idx_0, dnx_field_action_length_type_e action_length_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].action_length_type,
        action_length_type,
        dnx_field_action_length_type_e,
        0,
        "dnx_field_group_sw_fg_info_action_length_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &action_length_type,
        "dnx_field_group_sw[%d]->fg_info[%d].action_length_type",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_ACTION_LENGTH_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_action_length_type_get(int unit, uint32 fg_info_idx_0, dnx_field_action_length_type_e *action_length_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        action_length_type);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *action_length_type = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].action_length_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].action_length_type,
        "dnx_field_group_sw[%d]->fg_info[%d].action_length_type",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_ACTION_LENGTH_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_actions_payload_info_set(int unit, uint32 fg_info_idx_0, CONST dnx_field_actions_fg_payload_sw_info_t *actions_payload_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info,
        actions_payload_info,
        dnx_field_actions_fg_payload_sw_info_t,
        0,
        "dnx_field_group_sw_fg_info_actions_payload_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        actions_payload_info,
        "dnx_field_group_sw[%d]->fg_info[%d].actions_payload_info",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_actions_payload_info_get(int unit, uint32 fg_info_idx_0, dnx_field_actions_fg_payload_sw_info_t *actions_payload_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        actions_payload_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *actions_payload_info = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info,
        "dnx_field_group_sw[%d]->fg_info[%d].actions_payload_info",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_set(int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, CONST dnx_field_action_in_group_info_t *actions_on_payload_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0],
        actions_on_payload_info,
        dnx_field_action_in_group_info_t,
        0,
        "dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        actions_on_payload_info,
        "dnx_field_group_sw[%d]->fg_info[%d].actions_payload_info.actions_on_payload_info[%d]",
        unit, fg_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_get(int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, dnx_field_action_in_group_info_t *actions_on_payload_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        actions_on_payload_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *actions_on_payload_info = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0],
        "dnx_field_group_sw[%d]->fg_info[%d].actions_payload_info.actions_on_payload_info[%d]",
        unit, fg_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dnx_action_set(int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, dnx_field_action_t dnx_action)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].dnx_action,
        dnx_action,
        dnx_field_action_t,
        0,
        "dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dnx_action_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &dnx_action,
        "dnx_field_group_sw[%d]->fg_info[%d].actions_payload_info.actions_on_payload_info[%d].dnx_action",
        unit, fg_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_DNX_ACTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dnx_action_get(int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, dnx_field_action_t *dnx_action)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_action);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *dnx_action = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].dnx_action;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].dnx_action,
        "dnx_field_group_sw[%d]->fg_info[%d].actions_payload_info.actions_on_payload_info[%d].dnx_action",
        unit, fg_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_DNX_ACTION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_set(int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 dont_use_valid_bit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].dont_use_valid_bit,
        dont_use_valid_bit,
        uint8,
        0,
        "dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &dont_use_valid_bit,
        "dnx_field_group_sw[%d]->fg_info[%d].actions_payload_info.actions_on_payload_info[%d].dont_use_valid_bit",
        unit, fg_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_DONT_USE_VALID_BIT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_get(int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 *dont_use_valid_bit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dont_use_valid_bit);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *dont_use_valid_bit = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].dont_use_valid_bit;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].dont_use_valid_bit,
        "dnx_field_group_sw[%d]->fg_info[%d].actions_payload_info.actions_on_payload_info[%d].dont_use_valid_bit",
        unit, fg_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_DONT_USE_VALID_BIT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_lsb_set(int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].lsb,
        lsb,
        uint8,
        0,
        "dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_lsb_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &lsb,
        "dnx_field_group_sw[%d]->fg_info[%d].actions_payload_info.actions_on_payload_info[%d].lsb",
        unit, fg_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_LSB_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_lsb_get(int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 *lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        lsb);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *lsb = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].lsb;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].lsb,
        "dnx_field_group_sw[%d]->fg_info[%d].actions_payload_info.actions_on_payload_info[%d].lsb",
        unit, fg_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_LSB_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_dbal_table_id_set(int unit, uint32 fg_info_idx_0, dbal_tables_e dbal_table_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].dbal_table_id,
        dbal_table_id,
        dbal_tables_e,
        0,
        "dnx_field_group_sw_fg_info_dbal_table_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &dbal_table_id,
        "dnx_field_group_sw[%d]->fg_info[%d].dbal_table_id",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_DBAL_TABLE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_dbal_table_id_get(int unit, uint32 fg_info_idx_0, dbal_tables_e *dbal_table_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dbal_table_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *dbal_table_id = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].dbal_table_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].dbal_table_id,
        "dnx_field_group_sw[%d]->fg_info[%d].dbal_table_id",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_DBAL_TABLE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_flags_set(int unit, uint32 fg_info_idx_0, dnx_field_group_add_flags_e flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].flags,
        flags,
        dnx_field_group_add_flags_e,
        0,
        "dnx_field_group_sw_fg_info_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &flags,
        "dnx_field_group_sw[%d]->fg_info[%d].flags",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_flags_get(int unit, uint32 fg_info_idx_0, dnx_field_group_add_flags_e *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        flags);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *flags = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].flags,
        "dnx_field_group_sw[%d]->fg_info[%d].flags",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_auto_tcam_bank_select_set(int unit, uint32 fg_info_idx_0, int auto_tcam_bank_select)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.auto_tcam_bank_select,
        auto_tcam_bank_select,
        int,
        0,
        "dnx_field_group_sw_fg_info_tcam_info_auto_tcam_bank_select_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &auto_tcam_bank_select,
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.auto_tcam_bank_select",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_AUTO_TCAM_BANK_SELECT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_auto_tcam_bank_select_get(int unit, uint32 fg_info_idx_0, int *auto_tcam_bank_select)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        auto_tcam_bank_select);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *auto_tcam_bank_select = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.auto_tcam_bank_select;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.auto_tcam_bank_select,
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.auto_tcam_bank_select",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_AUTO_TCAM_BANK_SELECT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_nof_tcam_banks_set(int unit, uint32 fg_info_idx_0, int nof_tcam_banks)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.nof_tcam_banks,
        nof_tcam_banks,
        int,
        0,
        "dnx_field_group_sw_fg_info_tcam_info_nof_tcam_banks_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &nof_tcam_banks,
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.nof_tcam_banks",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_NOF_TCAM_BANKS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_nof_tcam_banks_get(int unit, uint32 fg_info_idx_0, int *nof_tcam_banks)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        nof_tcam_banks);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *nof_tcam_banks = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.nof_tcam_banks;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.nof_tcam_banks,
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.nof_tcam_banks",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_NOF_TCAM_BANKS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_set(int unit, uint32 fg_info_idx_0, uint32 tcam_bank_ids_idx_0, int tcam_bank_ids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.tcam_bank_ids);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.tcam_bank_ids,
        tcam_bank_ids_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.tcam_bank_ids[tcam_bank_ids_idx_0],
        tcam_bank_ids,
        int,
        0,
        "dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &tcam_bank_ids,
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.tcam_bank_ids[%d]",
        unit, fg_info_idx_0, tcam_bank_ids_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_TCAM_BANK_IDS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_get(int unit, uint32 fg_info_idx_0, uint32 tcam_bank_ids_idx_0, int *tcam_bank_ids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        tcam_bank_ids);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.tcam_bank_ids,
        tcam_bank_ids_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.tcam_bank_ids);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *tcam_bank_ids = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.tcam_bank_ids[tcam_bank_ids_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.tcam_bank_ids[tcam_bank_ids_idx_0],
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.tcam_bank_ids[%d]",
        unit, fg_info_idx_0, tcam_bank_ids_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_TCAM_BANK_IDS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_alloc(int unit, uint32 fg_info_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.tcam_bank_ids,
        int,
        dnx_data_field.tcam.nof_banks_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.tcam_bank_ids,
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.tcam_bank_ids",
        unit, fg_info_idx_0,
        dnx_data_field.tcam.nof_banks_get(unit) * sizeof(int) / sizeof(*((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.tcam_bank_ids));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_TCAM_BANK_IDS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_is_allocated(int unit, uint32 fg_info_idx_0, uint8 *is_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_IS_ALLOCATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        is_allocated);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_IS_ALLOC(
        unit,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.tcam_bank_ids,
        is_allocated,
        DNXC_SW_STATE_NONE,
        "dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ISALLOC,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.tcam_bank_ids,
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.tcam_bank_ids",
        unit, fg_info_idx_0);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_cache_mode_set(int unit, uint32 fg_info_idx_0, dnx_field_group_cache_mode_e cache_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.cache_mode,
        cache_mode,
        dnx_field_group_cache_mode_e,
        0,
        "dnx_field_group_sw_fg_info_tcam_info_cache_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &cache_mode,
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.cache_mode",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_CACHE_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_cache_mode_get(int unit, uint32 fg_info_idx_0, dnx_field_group_cache_mode_e *cache_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        cache_mode);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *cache_mode = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.cache_mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.cache_mode,
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.cache_mode",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_CACHE_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_nof_valid_entries_set(int unit, uint32 fg_info_idx_0, int nof_valid_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.nof_valid_entries,
        nof_valid_entries,
        int,
        0,
        "dnx_field_group_sw_fg_info_tcam_info_nof_valid_entries_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &nof_valid_entries,
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.nof_valid_entries",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_NOF_VALID_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_nof_valid_entries_get(int unit, uint32 fg_info_idx_0, int *nof_valid_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        nof_valid_entries);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *nof_valid_entries = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.nof_valid_entries;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.nof_valid_entries,
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.nof_valid_entries",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_NOF_VALID_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_nof_valid_entries_inc(int unit, uint32 fg_info_idx_0, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.nof_valid_entries,
        inc_value,
        int,
        0,
        "dnx_field_group_sw_fg_info_tcam_info_nof_valid_entries_inc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_INC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_INC,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.nof_valid_entries,
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.nof_valid_entries",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_NOF_VALID_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_tcam_info_nof_valid_entries_dec(int unit, uint32 fg_info_idx_0, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info,
        fg_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.nof_valid_entries,
        dec_value,
        int,
        0,
        "dnx_field_group_sw_fg_info_tcam_info_nof_valid_entries_dec");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_DEC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_DEC,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].tcam_info.nof_valid_entries,
        "dnx_field_group_sw[%d]->fg_info[%d].tcam_info.nof_valid_entries",
        unit, fg_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_NOF_VALID_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_set(int unit, uint32 ace_format_info_idx_0, CONST dnx_field_ace_format_sw_info_t *ace_format_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0],
        ace_format_info,
        dnx_field_ace_format_sw_info_t,
        0,
        "dnx_field_group_sw_ace_format_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        ace_format_info,
        "dnx_field_group_sw[%d]->ace_format_info[%d]",
        unit, ace_format_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_get(int unit, uint32 ace_format_info_idx_0, dnx_field_ace_format_sw_info_t *ace_format_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ace_format_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *ace_format_info = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0],
        "dnx_field_group_sw[%d]->ace_format_info[%d]",
        unit, ace_format_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_alloc(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        dnx_field_ace_format_sw_info_t,
        dnx_data_field.ace.nof_ace_id_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_group_sw_ace_format_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        "dnx_field_group_sw[%d]->ace_format_info",
        unit,
        dnx_data_field.ace.nof_ace_id_get(unit) * sizeof(dnx_field_ace_format_sw_info_t) / sizeof(*((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.ace.nof_ace_id_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[def_val_idx[0]].actions_payload_info.actions_on_payload_info[def_val_idx[1]].dnx_action,
        DNX_FIELD_ACTION_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.ace.nof_ace_id_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[def_val_idx[0]].flags,
        DNX_FIELD_ACE_FORMAT_ADD_FLAG_NOF);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_actions_payload_info_set(int unit, uint32 ace_format_info_idx_0, CONST dnx_field_actions_ace_payload_sw_info_t *actions_payload_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].actions_payload_info,
        actions_payload_info,
        dnx_field_actions_ace_payload_sw_info_t,
        0,
        "dnx_field_group_sw_ace_format_info_actions_payload_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        actions_payload_info,
        "dnx_field_group_sw[%d]->ace_format_info[%d].actions_payload_info",
        unit, ace_format_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_ACTIONS_PAYLOAD_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_actions_payload_info_get(int unit, uint32 ace_format_info_idx_0, dnx_field_actions_ace_payload_sw_info_t *actions_payload_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        actions_payload_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *actions_payload_info = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].actions_payload_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].actions_payload_info,
        "dnx_field_group_sw[%d]->ace_format_info[%d].actions_payload_info",
        unit, ace_format_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_ACTIONS_PAYLOAD_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dnx_action_set(int unit, uint32 ace_format_info_idx_0, uint32 actions_on_payload_info_idx_0, dnx_field_action_t dnx_action)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].dnx_action,
        dnx_action,
        dnx_field_action_t,
        0,
        "dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dnx_action_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &dnx_action,
        "dnx_field_group_sw[%d]->ace_format_info[%d].actions_payload_info.actions_on_payload_info[%d].dnx_action",
        unit, ace_format_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_DNX_ACTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dnx_action_get(int unit, uint32 ace_format_info_idx_0, uint32 actions_on_payload_info_idx_0, dnx_field_action_t *dnx_action)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_action);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *dnx_action = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].dnx_action;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].dnx_action,
        "dnx_field_group_sw[%d]->ace_format_info[%d].actions_payload_info.actions_on_payload_info[%d].dnx_action",
        unit, ace_format_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_DNX_ACTION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_set(int unit, uint32 ace_format_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 dont_use_valid_bit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].dont_use_valid_bit,
        dont_use_valid_bit,
        uint8,
        0,
        "dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &dont_use_valid_bit,
        "dnx_field_group_sw[%d]->ace_format_info[%d].actions_payload_info.actions_on_payload_info[%d].dont_use_valid_bit",
        unit, ace_format_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_DONT_USE_VALID_BIT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_get(int unit, uint32 ace_format_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 *dont_use_valid_bit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dont_use_valid_bit);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *dont_use_valid_bit = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].dont_use_valid_bit;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].dont_use_valid_bit,
        "dnx_field_group_sw[%d]->ace_format_info[%d].actions_payload_info.actions_on_payload_info[%d].dont_use_valid_bit",
        unit, ace_format_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_DONT_USE_VALID_BIT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_lsb_set(int unit, uint32 ace_format_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].lsb,
        lsb,
        uint8,
        0,
        "dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_lsb_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &lsb,
        "dnx_field_group_sw[%d]->ace_format_info[%d].actions_payload_info.actions_on_payload_info[%d].lsb",
        unit, ace_format_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_LSB_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_lsb_get(int unit, uint32 ace_format_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 *lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        lsb);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        actions_on_payload_info_idx_0,
        DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *lsb = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].lsb;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].actions_payload_info.actions_on_payload_info[actions_on_payload_info_idx_0].lsb,
        "dnx_field_group_sw[%d]->ace_format_info[%d].actions_payload_info.actions_on_payload_info[%d].lsb",
        unit, ace_format_info_idx_0, actions_on_payload_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_LSB_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_flags_set(int unit, uint32 ace_format_info_idx_0, dnx_field_ace_format_add_flags_e flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].flags,
        flags,
        dnx_field_ace_format_add_flags_e,
        0,
        "dnx_field_group_sw_ace_format_info_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &flags,
        "dnx_field_group_sw[%d]->ace_format_info[%d].flags",
        unit, ace_format_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_flags_get(int unit, uint32 ace_format_info_idx_0, dnx_field_ace_format_add_flags_e *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        flags);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *flags = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].flags,
        "dnx_field_group_sw[%d]->ace_format_info[%d].flags",
        unit, ace_format_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_name_set(int unit, uint32 ace_format_info_idx_0, CONST field_ace_format_name_t *name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].name,
        name,
        field_ace_format_name_t,
        0,
        "dnx_field_group_sw_ace_format_info_name_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        name,
        "dnx_field_group_sw[%d]->ace_format_info[%d].name",
        unit, ace_format_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_NAME_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_name_get(int unit, uint32 ace_format_info_idx_0, field_ace_format_name_t *name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        name);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *name = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].name;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].name,
        "dnx_field_group_sw[%d]->ace_format_info[%d].name",
        unit, ace_format_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_NAME_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_name_value_set(int unit, uint32 ace_format_info_idx_0, uint32 value_idx_0, char value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        value_idx_0,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].name.value[value_idx_0],
        value,
        char,
        0,
        "dnx_field_group_sw_ace_format_info_name_value_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &value,
        "dnx_field_group_sw[%d]->ace_format_info[%d].name.value[%d]",
        unit, ace_format_info_idx_0, value_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_ace_format_info_name_value_get(int unit, uint32 ace_format_info_idx_0, uint32 value_idx_0, char *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info,
        ace_format_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        value);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        value_idx_0,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *value = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].name.value[value_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->ace_format_info[ace_format_info_idx_0].name.value[value_idx_0],
        "dnx_field_group_sw[%d]->ace_format_info[%d].name.value[%d]",
        unit, ace_format_info_idx_0, value_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




const char *
dnx_field_group_add_flags_e_get_name(dnx_field_group_add_flags_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_ADD_FLAG_WITH_ID", value, DNX_FIELD_GROUP_ADD_FLAG_WITH_ID);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_ADD_FLAG_EXTERNAL_COUNTERS", value, DNX_FIELD_GROUP_ADD_FLAG_EXTERNAL_COUNTERS);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_ADD_FLAG_PACK_QUALIFIERS", value, DNX_FIELD_GROUP_ADD_FLAG_PACK_QUALIFIERS);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_ADD_FLAG_NOF", value, DNX_FIELD_GROUP_ADD_FLAG_NOF);

    return NULL;
}




const char *
dnx_field_group_context_attach_flags_e_get_name(dnx_field_group_context_attach_flags_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_CONTEXT_ATTACH_FLAG_32_RESULT_MSB_ALIGN", value, DNX_FIELD_GROUP_CONTEXT_ATTACH_FLAG_32_RESULT_MSB_ALIGN);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_CONTEXT_ATTACH_FLAG_NOF", value, DNX_FIELD_GROUP_CONTEXT_ATTACH_FLAG_NOF);

    return NULL;
}




const char *
dnx_field_group_compare_id_e_get_name(dnx_field_group_compare_id_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_COMPARE_ID_FIRST", value, DNX_FIELD_GROUP_COMPARE_ID_FIRST);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_COMPARE_ID_NONE", value, DNX_FIELD_GROUP_COMPARE_ID_NONE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_COMPARE_ID_CMP_1", value, DNX_FIELD_GROUP_COMPARE_ID_CMP_1);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_COMPARE_ID_CMP_2", value, DNX_FIELD_GROUP_COMPARE_ID_CMP_2);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_COMPARE_ID_NOF", value, DNX_FIELD_GROUP_COMPARE_ID_NOF);

    return NULL;
}




const char *
dnx_field_ace_format_add_flags_e_get_name(dnx_field_ace_format_add_flags_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_ACE_FORMAT_ADD_FLAG_WITH_ID", value, DNX_FIELD_ACE_FORMAT_ADD_FLAG_WITH_ID);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_ACE_FORMAT_ADD_FLAG_NOF", value, DNX_FIELD_ACE_FORMAT_ADD_FLAG_NOF);

    return NULL;
}




const char *
dnx_field_group_cache_mode_e_get_name(dnx_field_group_cache_mode_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_CACHE_MODE_FIRST", value, DNX_FIELD_GROUP_CACHE_MODE_FIRST);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_CACHE_MODE_CLEAR", value, DNX_FIELD_GROUP_CACHE_MODE_CLEAR);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_CACHE_MODE_START", value, DNX_FIELD_GROUP_CACHE_MODE_START);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_CACHE_MODE_INSTALL", value, DNX_FIELD_GROUP_CACHE_MODE_INSTALL);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_GROUP_CACHE_MODE_NOF", value, DNX_FIELD_GROUP_CACHE_MODE_NOF);

    return NULL;
}





dnx_field_group_sw_cbs dnx_field_group_sw = 	{
	
	dnx_field_group_sw_is_init,
	dnx_field_group_sw_init,
		{
		
		dnx_field_group_sw_fg_info_alloc,
			{
			
			dnx_field_group_sw_fg_info_field_stage_set,
			dnx_field_group_sw_fg_info_field_stage_get}
		,
			{
			
			dnx_field_group_sw_fg_info_fg_type_set,
			dnx_field_group_sw_fg_info_fg_type_get}
		,
			{
			
			dnx_field_group_sw_fg_info_name_set,
			dnx_field_group_sw_fg_info_name_get,
				{
				
				dnx_field_group_sw_fg_info_name_value_set,
				dnx_field_group_sw_fg_info_name_value_get}
			}
		,
			{
			
			dnx_field_group_sw_fg_info_context_info_set,
			dnx_field_group_sw_fg_info_context_info_get,
				{
				
				dnx_field_group_sw_fg_info_context_info_context_info_valid_set,
				dnx_field_group_sw_fg_info_context_info_context_info_valid_get}
			,
				{
				
				dnx_field_group_sw_fg_info_context_info_key_id_set,
				dnx_field_group_sw_fg_info_context_info_key_id_get,
					{
					
					dnx_field_group_sw_fg_info_context_info_key_id_id_set,
					dnx_field_group_sw_fg_info_context_info_key_id_id_get}
				,
					{
					
					dnx_field_group_sw_fg_info_context_info_key_id_offset_on_first_key_set,
					dnx_field_group_sw_fg_info_context_info_key_id_offset_on_first_key_get}
				,
					{
					
					dnx_field_group_sw_fg_info_context_info_key_id_nof_bits_set,
					dnx_field_group_sw_fg_info_context_info_key_id_nof_bits_get}
				}
			,
				{
				
				dnx_field_group_sw_fg_info_context_info_qual_attach_info_set,
				dnx_field_group_sw_fg_info_context_info_qual_attach_info_get,
					{
					
					dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_type_set,
					dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_type_get}
				,
					{
					
					dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_arg_set,
					dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_arg_get}
				,
					{
					
					dnx_field_group_sw_fg_info_context_info_qual_attach_info_offset_set,
					dnx_field_group_sw_fg_info_context_info_qual_attach_info_offset_get}
				,
					{
					
					dnx_field_group_sw_fg_info_context_info_qual_attach_info_base_qual_set,
					dnx_field_group_sw_fg_info_context_info_qual_attach_info_base_qual_get}
				}
			,
				{
				
				dnx_field_group_sw_fg_info_context_info_nof_cascading_refs_set,
				dnx_field_group_sw_fg_info_context_info_nof_cascading_refs_get}
			,
				{
				
				dnx_field_group_sw_fg_info_context_info_payload_offset_set,
				dnx_field_group_sw_fg_info_context_info_payload_offset_get}
			,
				{
				
				dnx_field_group_sw_fg_info_context_info_payload_id_set,
				dnx_field_group_sw_fg_info_context_info_payload_id_get}
			,
				{
				
				dnx_field_group_sw_fg_info_context_info_flags_set,
				dnx_field_group_sw_fg_info_context_info_flags_get}
			}
		,
			{
			
			dnx_field_group_sw_fg_info_key_template_set,
			dnx_field_group_sw_fg_info_key_template_get}
		,
			{
			
			dnx_field_group_sw_fg_info_key_length_type_set,
			dnx_field_group_sw_fg_info_key_length_type_get}
		,
			{
			
			dnx_field_group_sw_fg_info_action_length_type_set,
			dnx_field_group_sw_fg_info_action_length_type_get}
		,
			{
			
			dnx_field_group_sw_fg_info_actions_payload_info_set,
			dnx_field_group_sw_fg_info_actions_payload_info_get,
				{
				
				dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_set,
				dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_get,
					{
					
					dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dnx_action_set,
					dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dnx_action_get}
				,
					{
					
					dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_set,
					dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_get}
				,
					{
					
					dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_lsb_set,
					dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_lsb_get}
				}
			}
		,
			{
			
			dnx_field_group_sw_fg_info_dbal_table_id_set,
			dnx_field_group_sw_fg_info_dbal_table_id_get}
		,
			{
			
			dnx_field_group_sw_fg_info_flags_set,
			dnx_field_group_sw_fg_info_flags_get}
		,
			{
			
				{
				
				dnx_field_group_sw_fg_info_tcam_info_auto_tcam_bank_select_set,
				dnx_field_group_sw_fg_info_tcam_info_auto_tcam_bank_select_get}
			,
				{
				
				dnx_field_group_sw_fg_info_tcam_info_nof_tcam_banks_set,
				dnx_field_group_sw_fg_info_tcam_info_nof_tcam_banks_get}
			,
				{
				
				dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_set,
				dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_get,
				dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_alloc,
				dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_is_allocated}
			,
				{
				
				dnx_field_group_sw_fg_info_tcam_info_cache_mode_set,
				dnx_field_group_sw_fg_info_tcam_info_cache_mode_get}
			,
				{
				
				dnx_field_group_sw_fg_info_tcam_info_nof_valid_entries_set,
				dnx_field_group_sw_fg_info_tcam_info_nof_valid_entries_get,
				dnx_field_group_sw_fg_info_tcam_info_nof_valid_entries_inc,
				dnx_field_group_sw_fg_info_tcam_info_nof_valid_entries_dec}
			}
		}
	,
		{
		
		dnx_field_group_sw_ace_format_info_set,
		dnx_field_group_sw_ace_format_info_get,
		dnx_field_group_sw_ace_format_info_alloc,
			{
			
			dnx_field_group_sw_ace_format_info_actions_payload_info_set,
			dnx_field_group_sw_ace_format_info_actions_payload_info_get,
				{
				
					{
					
					dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dnx_action_set,
					dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dnx_action_get}
				,
					{
					
					dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_set,
					dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_get}
				,
					{
					
					dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_lsb_set,
					dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_lsb_get}
				}
			}
		,
			{
			
			dnx_field_group_sw_ace_format_info_flags_set,
			dnx_field_group_sw_ace_format_info_flags_get}
		,
			{
			
			dnx_field_group_sw_ace_format_info_name_set,
			dnx_field_group_sw_ace_format_info_name_get,
				{
				
				dnx_field_group_sw_ace_format_info_name_value_set,
				dnx_field_group_sw_ace_format_info_name_value_get}
			}
		}
	}
;
#undef BSL_LOG_MODULE

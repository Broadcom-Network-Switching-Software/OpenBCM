
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
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_group_diagnostic.h>



dnx_field_group_sw_t* dnx_field_group_sw_dummy = NULL;



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
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
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

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_KBP_MAX_NOF_OPCODES)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[3], DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[def_val_idx[1]].master_key_info.segment_info[def_val_idx[2]].qual_idx[def_val_idx[3]],
        DNX_FIELD_GROUP_EXTERNAL_PACKED_QUAL_INDEX_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_KBP_MAX_NOF_OPCODES)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[def_val_idx[1]].master_key_info.segment_info[def_val_idx[2]].fg_id,
        DNX_FIELD_GROUP_INVALID);

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

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[def_val_idx[0]].context_info[def_val_idx[1]].key_id.full_key_allocated,
        (TRUE));

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
dnx_field_group_sw_fg_info_context_info_key_id_full_key_allocated_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 full_key_allocated)
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
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.full_key_allocated,
        full_key_allocated,
        uint8,
        0,
        "dnx_field_group_sw_fg_info_context_info_key_id_full_key_allocated_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &full_key_allocated,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.full_key_allocated",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_FULL_KEY_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_full_key_allocated_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 *full_key_allocated)
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
        full_key_allocated);

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

    *full_key_allocated = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.full_key_allocated;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.full_key_allocated,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.full_key_allocated",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_FULL_KEY_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_bit_range_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, CONST dnx_field_bit_range_t *bit_range)
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
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.bit_range,
        bit_range,
        dnx_field_bit_range_t,
        0,
        "dnx_field_group_sw_fg_info_context_info_key_id_bit_range_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        bit_range,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.bit_range",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_BIT_RANGE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_bit_range_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, dnx_field_bit_range_t *bit_range)
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
        bit_range);

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

    *bit_range = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.bit_range;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.bit_range,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.bit_range",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_BIT_RANGE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_valid_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 bit_range_valid)
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
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.bit_range.bit_range_valid,
        bit_range_valid,
        uint8,
        0,
        "dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &bit_range_valid,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.bit_range.bit_range_valid",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_valid_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 *bit_range_valid)
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
        bit_range_valid);

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

    *bit_range_valid = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.bit_range.bit_range_valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.bit_range.bit_range_valid,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.bit_range.bit_range_valid",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_offset_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 bit_range_offset)
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
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.bit_range.bit_range_offset,
        bit_range_offset,
        uint8,
        0,
        "dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_offset_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &bit_range_offset,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.bit_range.bit_range_offset",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_OFFSET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_offset_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 *bit_range_offset)
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
        bit_range_offset);

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

    *bit_range_offset = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.bit_range.bit_range_offset;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.bit_range.bit_range_offset,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.bit_range.bit_range_offset",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_OFFSET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_size_set(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 bit_range_size)
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
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.bit_range.bit_range_size,
        bit_range_size,
        uint8,
        0,
        "dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &bit_range_size,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.bit_range.bit_range_size",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_size_get(int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 *bit_range_size)
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
        bit_range_size);

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

    *bit_range_size = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.bit_range.bit_range_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->fg_info[fg_info_idx_0].context_info[context_info_idx_0].key_id.bit_range.bit_range_size,
        "dnx_field_group_sw[%d]->fg_info[%d].context_info[%d].key_id.bit_range.bit_range_size",
        unit, fg_info_idx_0, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_SIZE_INFO,
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




int
dnx_field_group_sw_kbp_info_set(int unit, uint32 kbp_info_idx_0, CONST dnx_field_group_kbp_info_t *kbp_info)
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

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0],
        kbp_info,
        dnx_field_group_kbp_info_t,
        0,
        "dnx_field_group_sw_kbp_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        kbp_info,
        "dnx_field_group_sw[%d]->kbp_info[%d]",
        unit, kbp_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_kbp_info_get(int unit, uint32 kbp_info_idx_0, dnx_field_group_kbp_info_t *kbp_info)
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

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kbp_info_idx_0,
        DNX_KBP_MAX_NOF_OPCODES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        kbp_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *kbp_info = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0],
        "dnx_field_group_sw[%d]->kbp_info[%d]",
        unit, kbp_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_kbp_info_master_key_info_set(int unit, uint32 kbp_info_idx_0, CONST dnx_field_group_kbp_master_key_info_t *master_key_info)
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

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info,
        master_key_info,
        dnx_field_group_kbp_master_key_info_t,
        0,
        "dnx_field_group_sw_kbp_info_master_key_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        master_key_info,
        "dnx_field_group_sw[%d]->kbp_info[%d].master_key_info",
        unit, kbp_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_kbp_info_master_key_info_get(int unit, uint32 kbp_info_idx_0, dnx_field_group_kbp_master_key_info_t *master_key_info)
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

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kbp_info_idx_0,
        DNX_KBP_MAX_NOF_OPCODES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        master_key_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *master_key_info = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info,
        "dnx_field_group_sw[%d]->kbp_info[%d].master_key_info",
        unit, kbp_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_kbp_info_master_key_info_segment_info_set(int unit, uint32 kbp_info_idx_0, uint32 segment_info_idx_0, CONST dnx_field_group_kbp_segment_info_t *segment_info)
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

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info.segment_info[segment_info_idx_0],
        segment_info,
        dnx_field_group_kbp_segment_info_t,
        0,
        "dnx_field_group_sw_kbp_info_master_key_info_segment_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        segment_info,
        "dnx_field_group_sw[%d]->kbp_info[%d].master_key_info.segment_info[%d]",
        unit, kbp_info_idx_0, segment_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_SEGMENT_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_kbp_info_master_key_info_segment_info_get(int unit, uint32 kbp_info_idx_0, uint32 segment_info_idx_0, dnx_field_group_kbp_segment_info_t *segment_info)
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

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kbp_info_idx_0,
        DNX_KBP_MAX_NOF_OPCODES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        segment_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        segment_info_idx_0,
        DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *segment_info = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info.segment_info[segment_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info.segment_info[segment_info_idx_0],
        "dnx_field_group_sw[%d]->kbp_info[%d].master_key_info.segment_info[%d]",
        unit, kbp_info_idx_0, segment_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_SEGMENT_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_kbp_info_master_key_info_segment_info_qual_idx_set(int unit, uint32 kbp_info_idx_0, uint32 segment_info_idx_0, uint32 qual_idx_idx_0, uint8 qual_idx)
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

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kbp_info_idx_0,
        DNX_KBP_MAX_NOF_OPCODES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        segment_info_idx_0,
        DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_idx_idx_0,
        DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info.segment_info[segment_info_idx_0].qual_idx[qual_idx_idx_0],
        qual_idx,
        uint8,
        0,
        "dnx_field_group_sw_kbp_info_master_key_info_segment_info_qual_idx_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &qual_idx,
        "dnx_field_group_sw[%d]->kbp_info[%d].master_key_info.segment_info[%d].qual_idx[%d]",
        unit, kbp_info_idx_0, segment_info_idx_0, qual_idx_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_SEGMENT_INFO_QUAL_IDX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_kbp_info_master_key_info_segment_info_qual_idx_get(int unit, uint32 kbp_info_idx_0, uint32 segment_info_idx_0, uint32 qual_idx_idx_0, uint8 *qual_idx)
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

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kbp_info_idx_0,
        DNX_KBP_MAX_NOF_OPCODES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        qual_idx);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        segment_info_idx_0,
        DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_idx_idx_0,
        DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *qual_idx = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info.segment_info[segment_info_idx_0].qual_idx[qual_idx_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info.segment_info[segment_info_idx_0].qual_idx[qual_idx_idx_0],
        "dnx_field_group_sw[%d]->kbp_info[%d].master_key_info.segment_info[%d].qual_idx[%d]",
        unit, kbp_info_idx_0, segment_info_idx_0, qual_idx_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_SEGMENT_INFO_QUAL_IDX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_kbp_info_master_key_info_segment_info_fg_id_set(int unit, uint32 kbp_info_idx_0, uint32 segment_info_idx_0, dnx_field_group_t fg_id)
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

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kbp_info_idx_0,
        DNX_KBP_MAX_NOF_OPCODES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        segment_info_idx_0,
        DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info.segment_info[segment_info_idx_0].fg_id,
        fg_id,
        dnx_field_group_t,
        0,
        "dnx_field_group_sw_kbp_info_master_key_info_segment_info_fg_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &fg_id,
        "dnx_field_group_sw[%d]->kbp_info[%d].master_key_info.segment_info[%d].fg_id",
        unit, kbp_info_idx_0, segment_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_SEGMENT_INFO_FG_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_kbp_info_master_key_info_segment_info_fg_id_get(int unit, uint32 kbp_info_idx_0, uint32 segment_info_idx_0, dnx_field_group_t *fg_id)
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

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kbp_info_idx_0,
        DNX_KBP_MAX_NOF_OPCODES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        fg_id);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        segment_info_idx_0,
        DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *fg_id = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info.segment_info[segment_info_idx_0].fg_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info.segment_info[segment_info_idx_0].fg_id,
        "dnx_field_group_sw[%d]->kbp_info[%d].master_key_info.segment_info[%d].fg_id",
        unit, kbp_info_idx_0, segment_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_SEGMENT_INFO_FG_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_kbp_info_master_key_info_nof_fwd_segments_set(int unit, uint32 kbp_info_idx_0, uint8 nof_fwd_segments)
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

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kbp_info_idx_0,
        DNX_KBP_MAX_NOF_OPCODES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info.nof_fwd_segments,
        nof_fwd_segments,
        uint8,
        0,
        "dnx_field_group_sw_kbp_info_master_key_info_nof_fwd_segments_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &nof_fwd_segments,
        "dnx_field_group_sw[%d]->kbp_info[%d].master_key_info.nof_fwd_segments",
        unit, kbp_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_NOF_FWD_SEGMENTS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_kbp_info_master_key_info_nof_fwd_segments_get(int unit, uint32 kbp_info_idx_0, uint8 *nof_fwd_segments)
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

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kbp_info_idx_0,
        DNX_KBP_MAX_NOF_OPCODES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        nof_fwd_segments);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *nof_fwd_segments = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info.nof_fwd_segments;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].master_key_info.nof_fwd_segments,
        "dnx_field_group_sw[%d]->kbp_info[%d].master_key_info.nof_fwd_segments",
        unit, kbp_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_NOF_FWD_SEGMENTS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_kbp_info_is_valid_set(int unit, uint32 kbp_info_idx_0, uint8 is_valid)
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

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kbp_info_idx_0,
        DNX_KBP_MAX_NOF_OPCODES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].is_valid,
        is_valid,
        uint8,
        0,
        "dnx_field_group_sw_kbp_info_is_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_GROUP_MODULE_ID,
        &is_valid,
        "dnx_field_group_sw[%d]->kbp_info[%d].is_valid",
        unit, kbp_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_IS_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_group_sw_kbp_info_is_valid_get(int unit, uint32 kbp_info_idx_0, uint8 *is_valid)
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

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kbp_info_idx_0,
        DNX_KBP_MAX_NOF_OPCODES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        is_valid);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_GROUP_MODULE_ID);

    *is_valid = ((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].is_valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_GROUP_MODULE_ID,
        &((dnx_field_group_sw_t*)sw_state_roots_array[unit][DNX_FIELD_GROUP_MODULE_ID])->kbp_info[kbp_info_idx_0].is_valid,
        "dnx_field_group_sw[%d]->kbp_info[%d].is_valid",
        unit, kbp_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_GROUP_MODULE_ID,
        dnx_field_group_sw_info,
        DNX_FIELD_GROUP_SW_KBP_INFO_IS_VALID_INFO,
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
					
					dnx_field_group_sw_fg_info_context_info_key_id_full_key_allocated_set,
					dnx_field_group_sw_fg_info_context_info_key_id_full_key_allocated_get}
				,
					{
					
					dnx_field_group_sw_fg_info_context_info_key_id_bit_range_set,
					dnx_field_group_sw_fg_info_context_info_key_id_bit_range_get,
						{
						
						dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_valid_set,
						dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_valid_get}
					,
						{
						
						dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_offset_set,
						dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_offset_get}
					,
						{
						
						dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_size_set,
						dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_size_get}
					}
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
	,
		{
		
		dnx_field_group_sw_kbp_info_set,
		dnx_field_group_sw_kbp_info_get,
			{
			
			dnx_field_group_sw_kbp_info_master_key_info_set,
			dnx_field_group_sw_kbp_info_master_key_info_get,
				{
				
				dnx_field_group_sw_kbp_info_master_key_info_segment_info_set,
				dnx_field_group_sw_kbp_info_master_key_info_segment_info_get,
					{
					
					dnx_field_group_sw_kbp_info_master_key_info_segment_info_qual_idx_set,
					dnx_field_group_sw_kbp_info_master_key_info_segment_info_qual_idx_get}
				,
					{
					
					dnx_field_group_sw_kbp_info_master_key_info_segment_info_fg_id_set,
					dnx_field_group_sw_kbp_info_master_key_info_segment_info_fg_id_get}
				}
			,
				{
				
				dnx_field_group_sw_kbp_info_master_key_info_nof_fwd_segments_set,
				dnx_field_group_sw_kbp_info_master_key_info_nof_fwd_segments_get}
			}
		,
			{
			
			dnx_field_group_sw_kbp_info_is_valid_set,
			dnx_field_group_sw_kbp_info_is_valid_get}
		}
	}
;
#undef BSL_LOG_MODULE

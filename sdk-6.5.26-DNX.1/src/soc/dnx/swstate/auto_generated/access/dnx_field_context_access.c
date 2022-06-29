
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_context_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_context_layout.h>





int
dnx_field_context_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]),
        "dnx_field_context_sw[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_init(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_context_sw_t,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_context_sw_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_field_context_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]),
        "dnx_field_context_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]),
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_alloc(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        dnx_field_context_sw_info_t,
        dnx_data_field.common_max_val.nof_contexts_get(unit),
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_context_sw_context_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        "dnx_field_context_sw[%d]->context_info",
        unit,
        dnx_data_field.common_max_val.nof_contexts_get(unit) * sizeof(dnx_field_context_sw_info_t) / sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.mode,
        DNX_FIELD_CONTEXT_COMPARE_MODE_NONE);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.key_info[def_val_idx[1]].key_template.key_qual_map[def_val_idx[2]].qual_type,
        DNX_FIELD_QUAL_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.key_info[def_val_idx[1]].key_id.id[def_val_idx[2]],
        DNX_FIELD_KEY_ID_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].input_type,
        DNX_FIELD_INPUT_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].input_arg,
        0xFFFF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].offset,
        0xFFFF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].base_qual,
        DNX_FIELD_QUAL_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.mode,
        DNX_FIELD_CONTEXT_COMPARE_MODE_NONE);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.key_info[def_val_idx[1]].key_template.key_qual_map[def_val_idx[2]].qual_type,
        DNX_FIELD_QUAL_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.key_info[def_val_idx[1]].key_id.id[def_val_idx[2]],
        DNX_FIELD_KEY_ID_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].input_type,
        DNX_FIELD_INPUT_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].input_arg,
        0xFFFF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].offset,
        0xFFFF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].base_qual,
        DNX_FIELD_QUAL_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.hashing_info.mode,
        DNX_FIELD_CONTEXT_HASH_MODE_DISABLED);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[def_val_idx[1]].qual_type,
        DNX_FIELD_QUAL_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.hashing_info.key_info.key_id.id[def_val_idx[1]],
        DNX_FIELD_KEY_ID_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.hashing_info.key_info.attach_info[def_val_idx[1]].input_type,
        DNX_FIELD_INPUT_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.hashing_info.key_info.attach_info[def_val_idx[1]].input_arg,
        0xFFFF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.hashing_info.key_info.attach_info[def_val_idx[1]].offset,
        0xFFFF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.hashing_info.key_info.attach_info[def_val_idx[1]].base_qual,
        DNX_FIELD_QUAL_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf2_info.cascaded_from,
        DNX_FIELD_CONTEXT_ID_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].state_table_info.mode,
        DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_set(int unit, uint32 context_info_idx_0, CONST dnx_field_context_ipmf1_sw_info_t *context_ipmf1_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info,
        context_ipmf1_info,
        dnx_field_context_ipmf1_sw_info_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        context_ipmf1_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_get(int unit, uint32 context_info_idx_0, dnx_field_context_ipmf1_sw_info_t *context_ipmf1_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        context_ipmf1_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        DNX_SW_STATE_DIAG_READ);

    *context_ipmf1_info = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_set(int unit, uint32 context_info_idx_0, CONST dnx_field_context_compare_info_t *compare_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info,
        compare_info,
        dnx_field_context_compare_info_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        compare_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_get(int unit, uint32 context_info_idx_0, dnx_field_context_compare_info_t *compare_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        compare_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        DNX_SW_STATE_DIAG_READ);

    *compare_info = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_set(int unit, uint32 context_info_idx_0, dnx_field_context_compare_mode_e mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.mode,
        mode,
        dnx_field_context_compare_mode_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        &mode,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_get(int unit, uint32 context_info_idx_0, dnx_field_context_compare_mode_e *mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        mode);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        DNX_SW_STATE_DIAG_READ);

    *mode = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.mode,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_set(int unit, uint32 context_info_idx_0, uint8 is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.is_set,
        is_set,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        &is_set,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_IS_SET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_get(int unit, uint32 context_info_idx_0, uint8 *is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        is_set);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        DNX_SW_STATE_DIAG_READ);

    *is_set = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.is_set;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.is_set,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_IS_SET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        qual_type,
        dnx_field_qual_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        &qual_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_qual_map[%d].qual_type",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t *qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        qual_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *qual_type = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_qual_map[%d].qual_type",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint16 lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb,
        lsb,
        uint16,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        &lsb,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_qual_map[%d].lsb",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint16 *lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        lsb);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        DNX_SW_STATE_DIAG_READ);

    *lsb = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_qual_map[%d].lsb",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint8 size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size,
        size,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        &size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_qual_map[%d].size",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint8 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        DNX_SW_STATE_DIAG_READ);

    *size = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_qual_map[%d].size",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint16 key_size_in_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_size_in_bits,
        key_size_in_bits,
        uint16,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        &key_size_in_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_size_in_bits",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_SIZE_IN_BITS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint16 *key_size_in_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        key_size_in_bits);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        DNX_SW_STATE_DIAG_READ);

    *key_size_in_bits = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_size_in_bits;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_size_in_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_size_in_bits",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_SIZE_IN_BITS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.id[id_idx_0],
        id,
        dbal_enum_value_field_field_key_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        &id,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.id[%d]",
        unit, context_info_idx_0, key_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e *id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        id);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        DNX_SW_STATE_DIAG_READ);

    *id = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.id[id_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.id[id_idx_0],
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.id[%d]",
        unit, context_info_idx_0, key_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_offset_on_first_key_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 offset_on_first_key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.offset_on_first_key,
        offset_on_first_key,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_offset_on_first_key_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        &offset_on_first_key,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.offset_on_first_key",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_OFFSET_ON_FIRST_KEY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_offset_on_first_key_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *offset_on_first_key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        offset_on_first_key);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        DNX_SW_STATE_DIAG_READ);

    *offset_on_first_key = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.offset_on_first_key;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.offset_on_first_key,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.offset_on_first_key",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_OFFSET_ON_FIRST_KEY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_nof_bits_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 nof_bits_idx_0, uint8 nof_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        nof_bits_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.nof_bits[nof_bits_idx_0],
        nof_bits,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_nof_bits_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        &nof_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.nof_bits[%d]",
        unit, context_info_idx_0, key_info_idx_0, nof_bits_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_NOF_BITS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_nof_bits_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 nof_bits_idx_0, uint8 *nof_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        nof_bits);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        nof_bits_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        DNX_SW_STATE_DIAG_READ);

    *nof_bits = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.nof_bits[nof_bits_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.nof_bits[nof_bits_idx_0],
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.nof_bits[%d]",
        unit, context_info_idx_0, key_info_idx_0, nof_bits_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_NOF_BITS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_type,
        input_type,
        dnx_field_input_type_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        &input_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].input_type",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e *input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        input_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *input_type = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].input_type",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_arg,
        input_arg,
        int,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        &input_arg,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].input_arg",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int *input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        input_arg);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        DNX_SW_STATE_DIAG_READ);

    *input_arg = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_arg;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_arg,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].input_arg",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].offset,
        offset,
        int,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        &offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].offset",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int *offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        offset);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        DNX_SW_STATE_DIAG_READ);

    *offset = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].offset;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].offset",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_base_qual_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_qual_t base_qual)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].base_qual,
        base_qual,
        dnx_field_qual_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_base_qual_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        &base_qual,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].base_qual",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_BASE_QUAL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_base_qual_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_qual_t *base_qual)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        base_qual);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        DNX_SW_STATE_DIAG_READ);

    *base_qual = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].base_qual;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].base_qual,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].base_qual",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_BASE_QUAL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_set(int unit, uint32 context_info_idx_0, dnx_field_context_compare_mode_e mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.mode,
        mode,
        dnx_field_context_compare_mode_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        &mode,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_get(int unit, uint32 context_info_idx_0, dnx_field_context_compare_mode_e *mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        mode);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        DNX_SW_STATE_DIAG_READ);

    *mode = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.mode,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_set(int unit, uint32 context_info_idx_0, uint8 is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.is_set,
        is_set,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        &is_set,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_IS_SET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_get(int unit, uint32 context_info_idx_0, uint8 *is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        is_set);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        DNX_SW_STATE_DIAG_READ);

    *is_set = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.is_set;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.is_set,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_IS_SET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        qual_type,
        dnx_field_qual_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        &qual_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_qual_map[%d].qual_type",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t *qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        qual_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *qual_type = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_qual_map[%d].qual_type",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint16 lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb,
        lsb,
        uint16,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        &lsb,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_qual_map[%d].lsb",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint16 *lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        lsb);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        DNX_SW_STATE_DIAG_READ);

    *lsb = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_qual_map[%d].lsb",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint8 size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size,
        size,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        &size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_qual_map[%d].size",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint8 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        DNX_SW_STATE_DIAG_READ);

    *size = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_qual_map[%d].size",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint16 key_size_in_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_size_in_bits,
        key_size_in_bits,
        uint16,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        &key_size_in_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_size_in_bits",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_SIZE_IN_BITS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint16 *key_size_in_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        key_size_in_bits);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        DNX_SW_STATE_DIAG_READ);

    *key_size_in_bits = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_size_in_bits;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_size_in_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_size_in_bits",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_SIZE_IN_BITS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.id[id_idx_0],
        id,
        dbal_enum_value_field_field_key_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        &id,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.id[%d]",
        unit, context_info_idx_0, key_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e *id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        id);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        DNX_SW_STATE_DIAG_READ);

    *id = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.id[id_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.id[id_idx_0],
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.id[%d]",
        unit, context_info_idx_0, key_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_offset_on_first_key_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 offset_on_first_key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.offset_on_first_key,
        offset_on_first_key,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_offset_on_first_key_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        &offset_on_first_key,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.offset_on_first_key",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_OFFSET_ON_FIRST_KEY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_offset_on_first_key_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *offset_on_first_key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        offset_on_first_key);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        DNX_SW_STATE_DIAG_READ);

    *offset_on_first_key = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.offset_on_first_key;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.offset_on_first_key,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.offset_on_first_key",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_OFFSET_ON_FIRST_KEY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_nof_bits_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 nof_bits_idx_0, uint8 nof_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        nof_bits_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.nof_bits[nof_bits_idx_0],
        nof_bits,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_nof_bits_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        &nof_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.nof_bits[%d]",
        unit, context_info_idx_0, key_info_idx_0, nof_bits_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_NOF_BITS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_nof_bits_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 nof_bits_idx_0, uint8 *nof_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        nof_bits);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        nof_bits_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        DNX_SW_STATE_DIAG_READ);

    *nof_bits = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.nof_bits[nof_bits_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.nof_bits[nof_bits_idx_0],
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.nof_bits[%d]",
        unit, context_info_idx_0, key_info_idx_0, nof_bits_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_NOF_BITS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_type,
        input_type,
        dnx_field_input_type_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        &input_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].input_type",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e *input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        input_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *input_type = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].input_type",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_arg,
        input_arg,
        int,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        &input_arg,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].input_arg",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int *input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        input_arg);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        DNX_SW_STATE_DIAG_READ);

    *input_arg = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_arg;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_arg,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].input_arg",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].offset,
        offset,
        int,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        &offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].offset",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int *offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        offset);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        DNX_SW_STATE_DIAG_READ);

    *offset = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].offset;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].offset",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_base_qual_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_qual_t base_qual)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].base_qual,
        base_qual,
        dnx_field_qual_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_base_qual_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        &base_qual,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].base_qual",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_BASE_QUAL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_base_qual_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_qual_t *base_qual)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        base_qual);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        DNX_SW_STATE_DIAG_READ);

    *base_qual = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].base_qual;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].base_qual,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].base_qual",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_BASE_QUAL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_set(int unit, uint32 context_info_idx_0, CONST dnx_field_context_hashing_info_t *hashing_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info,
        hashing_info,
        dnx_field_context_hashing_info_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        hashing_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_get(int unit, uint32 context_info_idx_0, dnx_field_context_hashing_info_t *hashing_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        hashing_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        DNX_SW_STATE_DIAG_READ);

    *hashing_info = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_set(int unit, uint32 context_info_idx_0, dnx_field_context_hash_mode_e mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.mode,
        mode,
        dnx_field_context_hash_mode_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        &mode,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_get(int unit, uint32 context_info_idx_0, dnx_field_context_hash_mode_e *mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        mode);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        DNX_SW_STATE_DIAG_READ);

    *mode = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.mode,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_set(int unit, uint32 context_info_idx_0, uint8 is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.is_set,
        is_set,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        &is_set,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_IS_SET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_get(int unit, uint32 context_info_idx_0, uint8 *is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        is_set);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        DNX_SW_STATE_DIAG_READ);

    *is_set = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.is_set;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.is_set,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_IS_SET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_set(int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        qual_type,
        dnx_field_qual_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        &qual_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%d].qual_type",
        unit, context_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_get(int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t *qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        qual_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *qual_type = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].qual_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%d].qual_type",
        unit, context_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_set(int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, uint16 lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].lsb,
        lsb,
        uint16,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        &lsb,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%d].lsb",
        unit, context_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_get(int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, uint16 *lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        lsb);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        DNX_SW_STATE_DIAG_READ);

    *lsb = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].lsb;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].lsb,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%d].lsb",
        unit, context_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_set(int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, uint8 size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].size,
        size,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        &size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%d].size",
        unit, context_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_get(int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, uint8 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        DNX_SW_STATE_DIAG_READ);

    *size = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%d].size",
        unit, context_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_set(int unit, uint32 context_info_idx_0, uint16 key_size_in_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits,
        key_size_in_bits,
        uint16,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        &key_size_in_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_SIZE_IN_BITS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_get(int unit, uint32 context_info_idx_0, uint16 *key_size_in_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        key_size_in_bits);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        DNX_SW_STATE_DIAG_READ);

    *key_size_in_bits = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_SIZE_IN_BITS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_set(int unit, uint32 context_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.id[id_idx_0],
        id,
        dbal_enum_value_field_field_key_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        &id,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.id[%d]",
        unit, context_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_get(int unit, uint32 context_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e *id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        id);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        DNX_SW_STATE_DIAG_READ);

    *id = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.id[id_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.id[id_idx_0],
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.id[%d]",
        unit, context_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_offset_on_first_key_set(int unit, uint32 context_info_idx_0, uint8 offset_on_first_key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.offset_on_first_key,
        offset_on_first_key,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_offset_on_first_key_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        &offset_on_first_key,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.offset_on_first_key",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_OFFSET_ON_FIRST_KEY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_offset_on_first_key_get(int unit, uint32 context_info_idx_0, uint8 *offset_on_first_key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        offset_on_first_key);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        DNX_SW_STATE_DIAG_READ);

    *offset_on_first_key = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.offset_on_first_key;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.offset_on_first_key,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.offset_on_first_key",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_OFFSET_ON_FIRST_KEY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_nof_bits_set(int unit, uint32 context_info_idx_0, uint32 nof_bits_idx_0, uint8 nof_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        nof_bits_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.nof_bits[nof_bits_idx_0],
        nof_bits,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_nof_bits_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        &nof_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.nof_bits[%d]",
        unit, context_info_idx_0, nof_bits_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_NOF_BITS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_nof_bits_get(int unit, uint32 context_info_idx_0, uint32 nof_bits_idx_0, uint8 *nof_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        nof_bits);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        nof_bits_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        DNX_SW_STATE_DIAG_READ);

    *nof_bits = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.nof_bits[nof_bits_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.nof_bits[nof_bits_idx_0],
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.nof_bits[%d]",
        unit, context_info_idx_0, nof_bits_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_NOF_BITS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_set(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].input_type,
        input_type,
        dnx_field_input_type_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        &input_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].input_type",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_get(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e *input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        input_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *input_type = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].input_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].input_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].input_type",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_set(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, int input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].input_arg,
        input_arg,
        int,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        &input_arg,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].input_arg",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_get(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, int *input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        input_arg);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        DNX_SW_STATE_DIAG_READ);

    *input_arg = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].input_arg;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].input_arg,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].input_arg",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_set(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, int offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].offset,
        offset,
        int,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        &offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].offset",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_get(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, int *offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        offset);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        DNX_SW_STATE_DIAG_READ);

    *offset = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].offset;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].offset",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_base_qual_set(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, dnx_field_qual_t base_qual)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].base_qual,
        base_qual,
        dnx_field_qual_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_base_qual_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        &base_qual,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].base_qual",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_BASE_QUAL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_base_qual_get(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, dnx_field_qual_t *base_qual)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        base_qual);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        DNX_SW_STATE_DIAG_READ);

    *base_qual = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].base_qual;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].base_qual,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].base_qual",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_BASE_QUAL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf2_info_set(int unit, uint32 context_info_idx_0, CONST dnx_field_context_ipmf2_sw_info_t *context_ipmf2_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf2_info,
        context_ipmf2_info,
        dnx_field_context_ipmf2_sw_info_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf2_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        context_ipmf2_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf2_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF2_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf2_info_get(int unit, uint32 context_info_idx_0, dnx_field_context_ipmf2_sw_info_t *context_ipmf2_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        context_ipmf2_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        DNX_SW_STATE_DIAG_READ);

    *context_ipmf2_info = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf2_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf2_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf2_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF2_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_set(int unit, uint32 context_info_idx_0, dnx_field_context_t cascaded_from)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf2_info.cascaded_from,
        cascaded_from,
        dnx_field_context_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        &cascaded_from,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf2_info.cascaded_from",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF2_INFO_CASCADED_FROM_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_get(int unit, uint32 context_info_idx_0, dnx_field_context_t *cascaded_from)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        cascaded_from);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        DNX_SW_STATE_DIAG_READ);

    *cascaded_from = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf2_info.cascaded_from;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf2_info.cascaded_from,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf2_info.cascaded_from",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF2_INFO_CASCADED_FROM_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_state_table_info_set(int unit, uint32 context_info_idx_0, CONST dnx_field_context_state_table_sw_info_t *state_table_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info,
        state_table_info,
        dnx_field_context_state_table_sw_info_t,
        0,
        "dnx_field_context_sw_context_info_state_table_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        state_table_info,
        "dnx_field_context_sw[%d]->context_info[%d].state_table_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_STATE_TABLE_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_state_table_info_get(int unit, uint32 context_info_idx_0, dnx_field_context_state_table_sw_info_t *state_table_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        state_table_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        DNX_SW_STATE_DIAG_READ);

    *state_table_info = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info,
        "dnx_field_context_sw[%d]->context_info[%d].state_table_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_STATE_TABLE_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_state_table_info_mode_set(int unit, uint32 context_info_idx_0, dnx_field_context_state_table_mode_e mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info.mode,
        mode,
        dnx_field_context_state_table_mode_e,
        0,
        "dnx_field_context_sw_context_info_state_table_info_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        &mode,
        "dnx_field_context_sw[%d]->context_info[%d].state_table_info.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_STATE_TABLE_INFO_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_state_table_info_mode_get(int unit, uint32 context_info_idx_0, dnx_field_context_state_table_mode_e *mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        mode);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        DNX_SW_STATE_DIAG_READ);

    *mode = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info.mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info.mode,
        "dnx_field_context_sw[%d]->context_info[%d].state_table_info.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_STATE_TABLE_INFO_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_state_table_info_is_set_set(int unit, uint32 context_info_idx_0, uint8 is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info.is_set,
        is_set,
        uint8,
        0,
        "dnx_field_context_sw_context_info_state_table_info_is_set_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        &is_set,
        "dnx_field_context_sw[%d]->context_info[%d].state_table_info.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_STATE_TABLE_INFO_IS_SET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_state_table_info_is_set_get(int unit, uint32 context_info_idx_0, uint8 *is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        is_set);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        DNX_SW_STATE_DIAG_READ);

    *is_set = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info.is_set;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info.is_set,
        "dnx_field_context_sw[%d]->context_info[%d].state_table_info.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_STATE_TABLE_INFO_IS_SET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_name_value_stringncat(int unit, uint32 context_info_idx_0, uint32 name_idx_0, char *src)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        SW_STATE_FUNC_STRINGNCAT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        name_idx_0,
        DNX_FIELD_STAGE_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGNCAT(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        src,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCAT_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCAT,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        "dnx_field_context_sw[%d]->context_info[%d].name[%d].value",
        unit, context_info_idx_0, name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_name_value_stringncmp(int unit, uint32 context_info_idx_0, uint32 name_idx_0, char *cmp_string, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        SW_STATE_FUNC_STRINGNCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        name_idx_0,
        DNX_FIELD_STAGE_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGNCMP(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        cmp_string,
        result,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCMP_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCMP,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        "dnx_field_context_sw[%d]->context_info[%d].name[%d].value",
        unit, context_info_idx_0, name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_name_value_stringncpy(int unit, uint32 context_info_idx_0, uint32 name_idx_0, char *src)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        SW_STATE_FUNC_STRINGNCPY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        name_idx_0,
        DNX_FIELD_STAGE_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGNCPY(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        src,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCPY_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCPY,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        "dnx_field_context_sw[%d]->context_info[%d].name[%d].value",
        unit, context_info_idx_0, name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_name_value_stringlen(int unit, uint32 context_info_idx_0, uint32 name_idx_0, uint32 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        SW_STATE_FUNC_STRINGLEN,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        name_idx_0,
        DNX_FIELD_STAGE_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGLEN(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        size);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGLEN_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGLEN,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        "dnx_field_context_sw[%d]->context_info[%d].name[%d].value",
        unit, context_info_idx_0, name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_context_sw_context_info_name_value_stringget(int unit, uint32 context_info_idx_0, uint32 name_idx_0, char *o_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        SW_STATE_FUNC_STRINGGET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        name_idx_0,
        DNX_FIELD_STAGE_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGGET(
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        o_string);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGGET_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGGET,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        "dnx_field_context_sw[%d]->context_info[%d].name[%d].value",
        unit, context_info_idx_0, name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



const char *
dnx_field_context_compare_mode_e_get_name(dnx_field_context_compare_mode_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_COMPARE_MODE_FIRST", value, DNX_FIELD_CONTEXT_COMPARE_MODE_FIRST);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_COMPARE_MODE_NONE", value, DNX_FIELD_CONTEXT_COMPARE_MODE_NONE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE", value, DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE", value, DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_COMPARE_MODE_NOF", value, DNX_FIELD_CONTEXT_COMPARE_MODE_NOF);

    return NULL;
}



const char *
dnx_field_context_hash_mode_e_get_name(dnx_field_context_hash_mode_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_HASH_MODE_FIRST", value, DNX_FIELD_CONTEXT_HASH_MODE_FIRST);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_HASH_MODE_DISABLED", value, DNX_FIELD_CONTEXT_HASH_MODE_DISABLED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_HASH_MODE_ENABLED", value, DNX_FIELD_CONTEXT_HASH_MODE_ENABLED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_HASH_MODE_NOF", value, DNX_FIELD_CONTEXT_HASH_MODE_NOF);

    return NULL;
}



const char *
dnx_field_context_state_table_mode_e_get_name(dnx_field_context_state_table_mode_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_STATE_TABLE_MODE_FIRST", value, DNX_FIELD_CONTEXT_STATE_TABLE_MODE_FIRST);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED", value, DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_STATE_TABLE_MODE_ENABLED", value, DNX_FIELD_CONTEXT_STATE_TABLE_MODE_ENABLED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_STATE_TABLE_MODE_NOF", value, DNX_FIELD_CONTEXT_STATE_TABLE_MODE_NOF);

    return NULL;
}



const char *
dnx_field_context_state_table_opcode_e_get_name(dnx_field_context_state_table_opcode_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_QUAL", value, DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_QUAL);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_INCREMENT", value, DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_INCREMENT);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_ADD", value, DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_ADD);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_WRITE", value, DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_WRITE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_AND", value, DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_AND);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_OR", value, DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_OR);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_XOR", value, DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_XOR);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_NOF", value, DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_NOF);

    return NULL;
}




dnx_field_context_sw_cbs dnx_field_context_sw = 	{
	
	dnx_field_context_sw_is_init,
	dnx_field_context_sw_init,
		{
		
		dnx_field_context_sw_context_info_alloc,
			{
			
			dnx_field_context_sw_context_info_context_ipmf1_info_set,
			dnx_field_context_sw_context_info_context_ipmf1_info_get,
				{
				
				dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_set,
				dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_get,
					{
					
						{
						
						dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_set,
						dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_get}
					,
						{
						
						dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_set,
						dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_get}
					,
						{
						
							{
							
								{
								
									{
									
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_set,
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_get}
								,
									{
									
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_set,
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_get}
								,
									{
									
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_set,
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_get}
								}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_get}
							}
						,
							{
							
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_offset_on_first_key_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_offset_on_first_key_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_nof_bits_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_nof_bits_get}
							}
						,
							{
							
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_base_qual_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_base_qual_get}
							}
						}
					}
				,
					{
					
						{
						
						dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_set,
						dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_get}
					,
						{
						
						dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_set,
						dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_get}
					,
						{
						
							{
							
								{
								
									{
									
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_set,
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_get}
								,
									{
									
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_set,
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_get}
								,
									{
									
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_set,
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_get}
								}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_get}
							}
						,
							{
							
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_offset_on_first_key_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_offset_on_first_key_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_nof_bits_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_nof_bits_get}
							}
						,
							{
							
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_base_qual_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_base_qual_get}
							}
						}
					}
				}
			,
				{
				
				dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_set,
				dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_get,
					{
					
					dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_set,
					dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_get}
				,
					{
					
					dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_set,
					dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_get}
				,
					{
					
						{
						
							{
							
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_get}
							}
						,
							{
							
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_set,
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_get}
						}
					,
						{
						
							{
							
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_set,
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_get}
						,
							{
							
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_offset_on_first_key_set,
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_offset_on_first_key_get}
						,
							{
							
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_nof_bits_set,
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_nof_bits_get}
						}
					,
						{
						
							{
							
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_set,
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_get}
						,
							{
							
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_set,
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_get}
						,
							{
							
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_set,
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_get}
						,
							{
							
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_base_qual_set,
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_base_qual_get}
						}
					}
				}
			}
		,
			{
			
			dnx_field_context_sw_context_info_context_ipmf2_info_set,
			dnx_field_context_sw_context_info_context_ipmf2_info_get,
				{
				
				dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_set,
				dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_get}
			}
		,
			{
			
			dnx_field_context_sw_context_info_state_table_info_set,
			dnx_field_context_sw_context_info_state_table_info_get,
				{
				
				dnx_field_context_sw_context_info_state_table_info_mode_set,
				dnx_field_context_sw_context_info_state_table_info_mode_get}
			,
				{
				
				dnx_field_context_sw_context_info_state_table_info_is_set_set,
				dnx_field_context_sw_context_info_state_table_info_is_set_get}
			}
		,
		
			{
			
				{
				
				dnx_field_context_sw_context_info_name_value_stringncat,
				dnx_field_context_sw_context_info_name_value_stringncmp,
				dnx_field_context_sw_context_info_name_value_stringncpy,
				dnx_field_context_sw_context_info_name_value_stringlen,
				dnx_field_context_sw_context_info_name_value_stringget}
			}
		}
	,
	}
;
#undef BSL_LOG_MODULE

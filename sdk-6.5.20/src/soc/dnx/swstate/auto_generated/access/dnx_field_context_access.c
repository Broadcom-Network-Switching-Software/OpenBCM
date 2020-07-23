
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
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_context_diagnostic.h>



dnx_field_context_sw_t* dnx_field_context_sw_dummy = NULL;



int
dnx_field_context_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]),
        "dnx_field_context_sw[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_context_sw_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_context_sw_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]),
        "dnx_field_context_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_field_context_sw_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_alloc(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        dnx_field_context_sw_info_t,
        dnx_data_field.common_max_val.nof_contexts_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_context_sw_context_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        "dnx_field_context_sw[%d]->context_info",
        unit,
        dnx_data_field.common_max_val.nof_contexts_get(unit) * sizeof(dnx_field_context_sw_info_t) / sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.mode,
        DNX_FIELD_CONTEXT_COMPARE_MODE_NONE);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.key_info[def_val_idx[1]].key_template.key_qual_map[def_val_idx[2]].qual_type,
        DNX_FIELD_QUAL_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.key_info[def_val_idx[1]].key_id.id[def_val_idx[2]],
        DNX_FIELD_KEY_ID_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.key_info[def_val_idx[1]].key_id.full_key_allocated,
        (TRUE));

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].input_type,
        DNX_FIELD_INPUT_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].input_arg,
        0xFFFF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_1.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].offset,
        0xFFFF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.mode,
        DNX_FIELD_CONTEXT_COMPARE_MODE_NONE);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.key_info[def_val_idx[1]].key_template.key_qual_map[def_val_idx[2]].qual_type,
        DNX_FIELD_QUAL_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.key_info[def_val_idx[1]].key_id.id[def_val_idx[2]],
        DNX_FIELD_KEY_ID_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.key_info[def_val_idx[1]].key_id.full_key_allocated,
        (TRUE));

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].input_type,
        DNX_FIELD_INPUT_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].input_arg,
        0xFFFF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.compare_info.pair_2.key_info[def_val_idx[1]].attach_info[def_val_idx[2]].offset,
        0xFFFF);

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

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf1_info.hashing_info.key_info.key_id.full_key_allocated,
        (TRUE));

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

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].context_ipmf2_info.cascaded_from,
        DNX_FIELD_CONTEXT_ID_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.common_max_val.nof_contexts_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[def_val_idx[0]].state_table_info.mode,
        DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_set(int unit, uint32 context_info_idx_0, CONST dnx_field_context_ipmf1_sw_info_t *context_ipmf1_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info,
        context_ipmf1_info,
        dnx_field_context_ipmf1_sw_info_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        context_ipmf1_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_get(int unit, uint32 context_info_idx_0, dnx_field_context_ipmf1_sw_info_t *context_ipmf1_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        context_ipmf1_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *context_ipmf1_info = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_set(int unit, uint32 context_info_idx_0, CONST dnx_field_context_compare_info_t *compare_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info,
        compare_info,
        dnx_field_context_compare_info_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        compare_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_get(int unit, uint32 context_info_idx_0, dnx_field_context_compare_info_t *compare_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        compare_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *compare_info = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_set(int unit, uint32 context_info_idx_0, dnx_field_context_compare_mode_e mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.mode,
        mode,
        dnx_field_context_compare_mode_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &mode,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_get(int unit, uint32 context_info_idx_0, dnx_field_context_compare_mode_e *mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        mode);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *mode = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.mode,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_set(int unit, uint32 context_info_idx_0, uint8 is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.is_set,
        is_set,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &is_set,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_IS_SET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_get(int unit, uint32 context_info_idx_0, uint8 *is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        is_set);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *is_set = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.is_set;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.is_set,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_IS_SET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        qual_type,
        dnx_field_qual_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &qual_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_qual_map[%d].qual_type",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t *qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        qual_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *qual_type = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_qual_map[%d].qual_type",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint16 lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb,
        lsb,
        uint16,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &lsb,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_qual_map[%d].lsb",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint16 *lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        lsb);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *lsb = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_qual_map[%d].lsb",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint8 size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size,
        size,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_qual_map[%d].size",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint8 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *size = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_qual_map[%d].size",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint16 key_size_in_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_size_in_bits,
        key_size_in_bits,
        uint16,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &key_size_in_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_size_in_bits",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_SIZE_IN_BITS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint16 *key_size_in_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        key_size_in_bits);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *key_size_in_bits = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_size_in_bits;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_template.key_size_in_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_template.key_size_in_bits",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_TEMPLATE_KEY_SIZE_IN_BITS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.id[id_idx_0],
        id,
        dbal_enum_value_field_field_key_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &id,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.id[%d]",
        unit, context_info_idx_0, key_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e *id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        id);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *id = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.id[id_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.id[id_idx_0],
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.id[%d]",
        unit, context_info_idx_0, key_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_full_key_allocated_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 full_key_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.full_key_allocated,
        full_key_allocated,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_full_key_allocated_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &full_key_allocated,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.full_key_allocated",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_FULL_KEY_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_full_key_allocated_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *full_key_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        full_key_allocated);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *full_key_allocated = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.full_key_allocated;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.full_key_allocated,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.full_key_allocated",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_FULL_KEY_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, CONST dnx_field_bit_range_t *bit_range)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.bit_range,
        bit_range,
        dnx_field_bit_range_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.bit_range",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_BIT_RANGE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, dnx_field_bit_range_t *bit_range)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *bit_range = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.bit_range;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.bit_range,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.bit_range",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_BIT_RANGE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_valid_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 bit_range_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.bit_range.bit_range_valid,
        bit_range_valid,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &bit_range_valid,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.bit_range.bit_range_valid",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_valid_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *bit_range_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range_valid);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *bit_range_valid = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.bit_range.bit_range_valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.bit_range.bit_range_valid,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.bit_range.bit_range_valid",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_offset_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 bit_range_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.bit_range.bit_range_offset,
        bit_range_offset,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_offset_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &bit_range_offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.bit_range.bit_range_offset",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_OFFSET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_offset_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *bit_range_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range_offset);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *bit_range_offset = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.bit_range.bit_range_offset;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.bit_range.bit_range_offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.bit_range.bit_range_offset",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_OFFSET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_size_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 bit_range_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.bit_range.bit_range_size,
        bit_range_size,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &bit_range_size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.bit_range.bit_range_size",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_size_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *bit_range_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range_size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *bit_range_size = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.bit_range.bit_range_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].key_id.bit_range.bit_range_size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].key_id.bit_range.bit_range_size",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_type,
        input_type,
        dnx_field_input_type_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &input_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].input_type",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e *input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        input_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *input_type = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].input_type",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_arg,
        input_arg,
        int,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &input_arg,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].input_arg",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int *input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        input_arg);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *input_arg = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_arg;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_arg,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].input_arg",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].offset,
        offset,
        int,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].offset",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int *offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        offset);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *offset = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].offset;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_1.key_info[key_info_idx_0].attach_info[attach_info_idx_0].offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_1.key_info[%d].attach_info[%d].offset",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_1_KEY_INFO_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_set(int unit, uint32 context_info_idx_0, dnx_field_context_compare_mode_e mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.mode,
        mode,
        dnx_field_context_compare_mode_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &mode,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_get(int unit, uint32 context_info_idx_0, dnx_field_context_compare_mode_e *mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        mode);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *mode = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.mode,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_set(int unit, uint32 context_info_idx_0, uint8 is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.is_set,
        is_set,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &is_set,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_IS_SET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_get(int unit, uint32 context_info_idx_0, uint8 *is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        is_set);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *is_set = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.is_set;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.is_set,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_IS_SET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        qual_type,
        dnx_field_qual_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &qual_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_qual_map[%d].qual_type",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t *qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        qual_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *qual_type = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_qual_map[%d].qual_type",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint16 lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb,
        lsb,
        uint16,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &lsb,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_qual_map[%d].lsb",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint16 *lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        lsb);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *lsb = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].lsb,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_qual_map[%d].lsb",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint8 size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size,
        size,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_qual_map[%d].size",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint8 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *size = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_qual_map[key_qual_map_idx_0].size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_qual_map[%d].size",
        unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint16 key_size_in_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_size_in_bits,
        key_size_in_bits,
        uint16,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &key_size_in_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_size_in_bits",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_SIZE_IN_BITS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint16 *key_size_in_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        key_size_in_bits);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *key_size_in_bits = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_size_in_bits;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_template.key_size_in_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_template.key_size_in_bits",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_TEMPLATE_KEY_SIZE_IN_BITS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.id[id_idx_0],
        id,
        dbal_enum_value_field_field_key_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &id,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.id[%d]",
        unit, context_info_idx_0, key_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e *id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        id);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *id = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.id[id_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.id[id_idx_0],
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.id[%d]",
        unit, context_info_idx_0, key_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_full_key_allocated_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 full_key_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.full_key_allocated,
        full_key_allocated,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_full_key_allocated_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &full_key_allocated,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.full_key_allocated",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_FULL_KEY_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_full_key_allocated_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *full_key_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        full_key_allocated);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *full_key_allocated = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.full_key_allocated;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.full_key_allocated,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.full_key_allocated",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_FULL_KEY_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, CONST dnx_field_bit_range_t *bit_range)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.bit_range,
        bit_range,
        dnx_field_bit_range_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.bit_range",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_BIT_RANGE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, dnx_field_bit_range_t *bit_range)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *bit_range = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.bit_range;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.bit_range,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.bit_range",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_BIT_RANGE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_valid_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 bit_range_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.bit_range.bit_range_valid,
        bit_range_valid,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &bit_range_valid,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.bit_range.bit_range_valid",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_valid_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *bit_range_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range_valid);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *bit_range_valid = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.bit_range.bit_range_valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.bit_range.bit_range_valid,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.bit_range.bit_range_valid",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_offset_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 bit_range_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.bit_range.bit_range_offset,
        bit_range_offset,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_offset_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &bit_range_offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.bit_range.bit_range_offset",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_OFFSET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_offset_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *bit_range_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range_offset);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *bit_range_offset = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.bit_range.bit_range_offset;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.bit_range.bit_range_offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.bit_range.bit_range_offset",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_OFFSET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_size_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 bit_range_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.bit_range.bit_range_size,
        bit_range_size,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &bit_range_size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.bit_range.bit_range_size",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_size_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *bit_range_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range_size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *bit_range_size = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.bit_range.bit_range_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].key_id.bit_range.bit_range_size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].key_id.bit_range.bit_range_size",
        unit, context_info_idx_0, key_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_type,
        input_type,
        dnx_field_input_type_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &input_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].input_type",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e *input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        input_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *input_type = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].input_type",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_arg,
        input_arg,
        int,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &input_arg,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].input_arg",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int *input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        input_arg);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *input_arg = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_arg;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].input_arg,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].input_arg",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_set(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].offset,
        offset,
        int,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].offset",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_get(int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int *offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        offset);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_info_idx_0,
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *offset = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].offset;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.compare_info.pair_2.key_info[key_info_idx_0].attach_info[attach_info_idx_0].offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.compare_info.pair_2.key_info[%d].attach_info[%d].offset",
        unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_COMPARE_INFO_PAIR_2_KEY_INFO_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_set(int unit, uint32 context_info_idx_0, CONST dnx_field_context_hashing_info_t *hashing_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info,
        hashing_info,
        dnx_field_context_hashing_info_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        hashing_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_get(int unit, uint32 context_info_idx_0, dnx_field_context_hashing_info_t *hashing_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        hashing_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *hashing_info = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_set(int unit, uint32 context_info_idx_0, dnx_field_context_hash_mode_e mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.mode,
        mode,
        dnx_field_context_hash_mode_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &mode,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_get(int unit, uint32 context_info_idx_0, dnx_field_context_hash_mode_e *mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        mode);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *mode = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.mode,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_set(int unit, uint32 context_info_idx_0, uint8 is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.is_set,
        is_set,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &is_set,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_IS_SET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_get(int unit, uint32 context_info_idx_0, uint8 *is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        is_set);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *is_set = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.is_set;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.is_set,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_IS_SET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_set(int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        qual_type,
        dnx_field_qual_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &qual_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%d].qual_type",
        unit, context_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_get(int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t *qual_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        qual_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *qual_type = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].qual_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].qual_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%d].qual_type",
        unit, context_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_set(int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, uint16 lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].lsb,
        lsb,
        uint16,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &lsb,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%d].lsb",
        unit, context_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_get(int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, uint16 *lsb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        lsb);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *lsb = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].lsb;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].lsb,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%d].lsb",
        unit, context_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_set(int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, uint8 size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].size,
        size,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%d].size",
        unit, context_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_get(int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, uint8 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_qual_map_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *size = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[key_qual_map_idx_0].size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%d].size",
        unit, context_info_idx_0, key_qual_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_set(int unit, uint32 context_info_idx_0, uint16 key_size_in_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits,
        key_size_in_bits,
        uint16,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &key_size_in_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_SIZE_IN_BITS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_get(int unit, uint32 context_info_idx_0, uint16 *key_size_in_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        key_size_in_bits);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *key_size_in_bits = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_TEMPLATE_KEY_SIZE_IN_BITS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_set(int unit, uint32 context_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.id[id_idx_0],
        id,
        dbal_enum_value_field_field_key_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &id,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.id[%d]",
        unit, context_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_get(int unit, uint32 context_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e *id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        id);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        id_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *id = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.id[id_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.id[id_idx_0],
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.id[%d]",
        unit, context_info_idx_0, id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_full_key_allocated_set(int unit, uint32 context_info_idx_0, uint8 full_key_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.full_key_allocated,
        full_key_allocated,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_full_key_allocated_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &full_key_allocated,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.full_key_allocated",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_FULL_KEY_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_full_key_allocated_get(int unit, uint32 context_info_idx_0, uint8 *full_key_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        full_key_allocated);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *full_key_allocated = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.full_key_allocated;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.full_key_allocated,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.full_key_allocated",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_FULL_KEY_ALLOCATED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_set(int unit, uint32 context_info_idx_0, CONST dnx_field_bit_range_t *bit_range)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range,
        bit_range,
        dnx_field_bit_range_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.bit_range",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_BIT_RANGE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_get(int unit, uint32 context_info_idx_0, dnx_field_bit_range_t *bit_range)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *bit_range = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.bit_range",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_BIT_RANGE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_valid_set(int unit, uint32 context_info_idx_0, uint8 bit_range_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_valid,
        bit_range_valid,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &bit_range_valid,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_valid",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_valid_get(int unit, uint32 context_info_idx_0, uint8 *bit_range_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range_valid);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *bit_range_valid = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_valid,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_valid",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_offset_set(int unit, uint32 context_info_idx_0, uint8 bit_range_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_offset,
        bit_range_offset,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_offset_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &bit_range_offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_offset",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_OFFSET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_offset_get(int unit, uint32 context_info_idx_0, uint8 *bit_range_offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range_offset);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *bit_range_offset = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_offset;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_offset",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_OFFSET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_size_set(int unit, uint32 context_info_idx_0, uint8 bit_range_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_size,
        bit_range_size,
        uint8,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &bit_range_size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_size",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_size_get(int unit, uint32 context_info_idx_0, uint8 *bit_range_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        bit_range_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *bit_range_size = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_size,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_size",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_set(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].input_type,
        input_type,
        dnx_field_input_type_e,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &input_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].input_type",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_get(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e *input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        input_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *input_type = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].input_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].input_type,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].input_type",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_set(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, int input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].input_arg,
        input_arg,
        int,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &input_arg,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].input_arg",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_get(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, int *input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        input_arg);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *input_arg = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].input_arg;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].input_arg,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].input_arg",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_set(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, int offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].offset,
        offset,
        int,
        0,
        "dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].offset",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_get(int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, int *offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        offset);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        attach_info_idx_0,
        DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *offset = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].offset;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf1_info.hashing_info.key_info.attach_info[attach_info_idx_0].offset,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf1_info.hashing_info.key_info.attach_info[%d].offset",
        unit, context_info_idx_0, attach_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF1_INFO_HASHING_INFO_KEY_INFO_ATTACH_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf2_info_set(int unit, uint32 context_info_idx_0, CONST dnx_field_context_ipmf2_sw_info_t *context_ipmf2_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf2_info,
        context_ipmf2_info,
        dnx_field_context_ipmf2_sw_info_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf2_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        context_ipmf2_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf2_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF2_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf2_info_get(int unit, uint32 context_info_idx_0, dnx_field_context_ipmf2_sw_info_t *context_ipmf2_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        context_ipmf2_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *context_ipmf2_info = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf2_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf2_info,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf2_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF2_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_set(int unit, uint32 context_info_idx_0, dnx_field_context_t cascaded_from)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf2_info.cascaded_from,
        cascaded_from,
        dnx_field_context_t,
        0,
        "dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &cascaded_from,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf2_info.cascaded_from",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF2_INFO_CASCADED_FROM_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_get(int unit, uint32 context_info_idx_0, dnx_field_context_t *cascaded_from)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        cascaded_from);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *cascaded_from = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf2_info.cascaded_from;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].context_ipmf2_info.cascaded_from,
        "dnx_field_context_sw[%d]->context_info[%d].context_ipmf2_info.cascaded_from",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_CONTEXT_IPMF2_INFO_CASCADED_FROM_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_state_table_info_set(int unit, uint32 context_info_idx_0, CONST dnx_field_context_state_table_info_t *state_table_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info,
        state_table_info,
        dnx_field_context_state_table_info_t,
        0,
        "dnx_field_context_sw_context_info_state_table_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        state_table_info,
        "dnx_field_context_sw[%d]->context_info[%d].state_table_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_STATE_TABLE_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_state_table_info_get(int unit, uint32 context_info_idx_0, dnx_field_context_state_table_info_t *state_table_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        state_table_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *state_table_info = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info,
        "dnx_field_context_sw[%d]->context_info[%d].state_table_info",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_STATE_TABLE_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_state_table_info_mode_set(int unit, uint32 context_info_idx_0, dnx_field_context_state_table_mode_e mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info.mode,
        mode,
        dnx_field_context_state_table_mode_e,
        0,
        "dnx_field_context_sw_context_info_state_table_info_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &mode,
        "dnx_field_context_sw[%d]->context_info[%d].state_table_info.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_STATE_TABLE_INFO_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_state_table_info_mode_get(int unit, uint32 context_info_idx_0, dnx_field_context_state_table_mode_e *mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        mode);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *mode = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info.mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info.mode,
        "dnx_field_context_sw[%d]->context_info[%d].state_table_info.mode",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_STATE_TABLE_INFO_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_state_table_info_is_set_set(int unit, uint32 context_info_idx_0, uint8 is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info.is_set,
        is_set,
        uint8,
        0,
        "dnx_field_context_sw_context_info_state_table_info_is_set_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &is_set,
        "dnx_field_context_sw[%d]->context_info[%d].state_table_info.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_STATE_TABLE_INFO_IS_SET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_state_table_info_is_set_get(int unit, uint32 context_info_idx_0, uint8 *is_set)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info,
        context_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        is_set);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    *is_set = ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info.is_set;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].state_table_info.is_set,
        "dnx_field_context_sw[%d]->context_info[%d].state_table_info.is_set",
        unit, context_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_STATE_TABLE_INFO_IS_SET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_name_value_stringncat(int unit, uint32 context_info_idx_0, uint32 name_idx_0, char *src)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_STRINGNCAT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    SW_STATE_STRING_STRINGNCAT(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        src,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCAT_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCAT,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        "dnx_field_context_sw[%d]->context_info[%d].name[%d].value",
        unit, context_info_idx_0, name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_name_value_stringncmp(int unit, uint32 context_info_idx_0, uint32 name_idx_0, char *cmp_string, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_STRINGNCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    SW_STATE_STRING_STRINGNCMP(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        cmp_string,
        result,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCMP_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCMP,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        "dnx_field_context_sw[%d]->context_info[%d].name[%d].value",
        unit, context_info_idx_0, name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_name_value_stringncpy(int unit, uint32 context_info_idx_0, uint32 name_idx_0, char *src)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_STRINGNCPY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    SW_STATE_STRING_STRINGNCPY(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        src,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCPY_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCPY,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        "dnx_field_context_sw[%d]->context_info[%d].name[%d].value",
        unit, context_info_idx_0, name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_name_value_stringlen(int unit, uint32 context_info_idx_0, uint32 name_idx_0, uint32 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_STRINGLEN,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    SW_STATE_STRING_STRINGLEN(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        size);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGLEN_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGLEN,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        "dnx_field_context_sw[%d]->context_info[%d].name[%d].value",
        unit, context_info_idx_0, name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_name_value_stringget(int unit, uint32 context_info_idx_0, uint32 name_idx_0, char *o_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        SW_STATE_FUNC_STRINGGET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID);

    SW_STATE_STRING_STRINGGET(
        DNX_FIELD_CONTEXT_MODULE_ID,
        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        o_string);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGGET_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGGET,
        DNX_FIELD_CONTEXT_MODULE_ID,
        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[context_info_idx_0].name[name_idx_0].value,
        "dnx_field_context_sw[%d]->context_info[%d].name[%d].value",
        unit, context_info_idx_0, name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_CONTEXT_MODULE_ID,
        dnx_field_context_sw_info,
        DNX_FIELD_CONTEXT_SW_CONTEXT_INFO_NAME_VALUE_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

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
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_full_key_allocated_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_full_key_allocated_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_get,
									{
									
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_valid_set,
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_valid_get}
								,
									{
									
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_offset_set,
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_offset_get}
								,
									{
									
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_size_set,
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_size_get}
								}
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
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_full_key_allocated_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_full_key_allocated_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_get,
									{
									
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_valid_set,
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_valid_get}
								,
									{
									
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_offset_set,
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_offset_get}
								,
									{
									
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_size_set,
									dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_size_get}
								}
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
							
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_full_key_allocated_set,
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_full_key_allocated_get}
						,
							{
							
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_set,
							dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_get,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_valid_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_valid_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_offset_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_offset_get}
							,
								{
								
								dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_size_set,
								dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_size_get}
							}
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
	}
;
#undef BSL_LOG_MODULE

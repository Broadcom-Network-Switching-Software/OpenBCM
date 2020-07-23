
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
#include <soc/dnx/swstate/auto_generated/access/dnx_field_range_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_range_diagnostic.h>



dnx_field_range_sw_db_t* dnx_field_range_sw_db_dummy = NULL;



int
dnx_field_range_sw_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_FIELD_RANGE_MODULE_ID,
        ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]),
        "dnx_field_range_sw_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_init(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_range_sw_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_range_sw_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_FIELD_RANGE_MODULE_ID,
        ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]),
        "dnx_field_range_sw_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_field_range_sw_db_info,
        DNX_FIELD_RANGE_SW_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_field_range_sw_db_t),
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_L4_OPS_NOF_EXT_TYPES)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_range_types[def_val_idx[1]],
        DNX_FIELD_RANGE_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[def_val_idx[1]].dnx_qual,
        DNX_FIELD_QUAL_ID_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[def_val_idx[1]].qual_info.input_type,
        DNX_FIELD_INPUT_TYPE_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[def_val_idx[1]].qual_info.input_arg,
        0xFFFF);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[def_val_idx[1]].qual_info.offset,
        0xFFFF);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_range_types_set(int unit, uint32 ext_l4_ops_range_types_idx_0, dnx_field_range_type_e ext_l4_ops_range_types)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ext_l4_ops_range_types_idx_0,
        DNX_DATA_MAX_FIELD_L4_OPS_NOF_EXT_TYPES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_RANGE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_range_types[ext_l4_ops_range_types_idx_0],
        ext_l4_ops_range_types,
        dnx_field_range_type_e,
        0,
        "dnx_field_range_sw_db_ext_l4_ops_range_types_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_RANGE_MODULE_ID,
        &ext_l4_ops_range_types,
        "dnx_field_range_sw_db[%d]->ext_l4_ops_range_types[%d]",
        unit, ext_l4_ops_range_types_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_field_range_sw_db_info,
        DNX_FIELD_RANGE_SW_DB_EXT_L4_OPS_RANGE_TYPES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_range_types_get(int unit, uint32 ext_l4_ops_range_types_idx_0, dnx_field_range_type_e *ext_l4_ops_range_types)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ext_l4_ops_range_types_idx_0,
        DNX_DATA_MAX_FIELD_L4_OPS_NOF_EXT_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        ext_l4_ops_range_types);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_RANGE_MODULE_ID);

    *ext_l4_ops_range_types = ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_range_types[ext_l4_ops_range_types_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_RANGE_MODULE_ID,
        &((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_range_types[ext_l4_ops_range_types_idx_0],
        "dnx_field_range_sw_db[%d]->ext_l4_ops_range_types[%d]",
        unit, ext_l4_ops_range_types_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_field_range_sw_db_info,
        DNX_FIELD_RANGE_SW_DB_EXT_L4_OPS_RANGE_TYPES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_set(int unit, uint32 ext_l4_ops_ffc_quals_idx_0, CONST dnx_field_range_type_qual_info_t *ext_l4_ops_ffc_quals)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_RANGE_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0],
        ext_l4_ops_ffc_quals,
        dnx_field_range_type_qual_info_t,
        0,
        "dnx_field_range_sw_db_ext_l4_ops_ffc_quals_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_RANGE_MODULE_ID,
        ext_l4_ops_ffc_quals,
        "dnx_field_range_sw_db[%d]->ext_l4_ops_ffc_quals[%d]",
        unit, ext_l4_ops_ffc_quals_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_field_range_sw_db_info,
        DNX_FIELD_RANGE_SW_DB_EXT_L4_OPS_FFC_QUALS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_get(int unit, uint32 ext_l4_ops_ffc_quals_idx_0, dnx_field_range_type_qual_info_t *ext_l4_ops_ffc_quals)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ext_l4_ops_ffc_quals_idx_0,
        DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        ext_l4_ops_ffc_quals);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_RANGE_MODULE_ID);

    *ext_l4_ops_ffc_quals = ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_RANGE_MODULE_ID,
        &((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0],
        "dnx_field_range_sw_db[%d]->ext_l4_ops_ffc_quals[%d]",
        unit, ext_l4_ops_ffc_quals_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_field_range_sw_db_info,
        DNX_FIELD_RANGE_SW_DB_EXT_L4_OPS_FFC_QUALS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dnx_qual_set(int unit, uint32 ext_l4_ops_ffc_quals_idx_0, dnx_field_qual_t dnx_qual)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ext_l4_ops_ffc_quals_idx_0,
        DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_RANGE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0].dnx_qual,
        dnx_qual,
        dnx_field_qual_t,
        0,
        "dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dnx_qual_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_RANGE_MODULE_ID,
        &dnx_qual,
        "dnx_field_range_sw_db[%d]->ext_l4_ops_ffc_quals[%d].dnx_qual",
        unit, ext_l4_ops_ffc_quals_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_field_range_sw_db_info,
        DNX_FIELD_RANGE_SW_DB_EXT_L4_OPS_FFC_QUALS_DNX_QUAL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dnx_qual_get(int unit, uint32 ext_l4_ops_ffc_quals_idx_0, dnx_field_qual_t *dnx_qual)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ext_l4_ops_ffc_quals_idx_0,
        DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_qual);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_RANGE_MODULE_ID);

    *dnx_qual = ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0].dnx_qual;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_RANGE_MODULE_ID,
        &((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0].dnx_qual,
        "dnx_field_range_sw_db[%d]->ext_l4_ops_ffc_quals[%d].dnx_qual",
        unit, ext_l4_ops_ffc_quals_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_field_range_sw_db_info,
        DNX_FIELD_RANGE_SW_DB_EXT_L4_OPS_FFC_QUALS_DNX_QUAL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_type_set(int unit, uint32 ext_l4_ops_ffc_quals_idx_0, dnx_field_input_type_e input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ext_l4_ops_ffc_quals_idx_0,
        DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_RANGE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0].qual_info.input_type,
        input_type,
        dnx_field_input_type_e,
        0,
        "dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_RANGE_MODULE_ID,
        &input_type,
        "dnx_field_range_sw_db[%d]->ext_l4_ops_ffc_quals[%d].qual_info.input_type",
        unit, ext_l4_ops_ffc_quals_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_field_range_sw_db_info,
        DNX_FIELD_RANGE_SW_DB_EXT_L4_OPS_FFC_QUALS_QUAL_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_type_get(int unit, uint32 ext_l4_ops_ffc_quals_idx_0, dnx_field_input_type_e *input_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ext_l4_ops_ffc_quals_idx_0,
        DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        input_type);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_RANGE_MODULE_ID);

    *input_type = ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0].qual_info.input_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_RANGE_MODULE_ID,
        &((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0].qual_info.input_type,
        "dnx_field_range_sw_db[%d]->ext_l4_ops_ffc_quals[%d].qual_info.input_type",
        unit, ext_l4_ops_ffc_quals_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_field_range_sw_db_info,
        DNX_FIELD_RANGE_SW_DB_EXT_L4_OPS_FFC_QUALS_QUAL_INFO_INPUT_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_arg_set(int unit, uint32 ext_l4_ops_ffc_quals_idx_0, int input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ext_l4_ops_ffc_quals_idx_0,
        DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_RANGE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0].qual_info.input_arg,
        input_arg,
        int,
        0,
        "dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_arg_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_RANGE_MODULE_ID,
        &input_arg,
        "dnx_field_range_sw_db[%d]->ext_l4_ops_ffc_quals[%d].qual_info.input_arg",
        unit, ext_l4_ops_ffc_quals_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_field_range_sw_db_info,
        DNX_FIELD_RANGE_SW_DB_EXT_L4_OPS_FFC_QUALS_QUAL_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_arg_get(int unit, uint32 ext_l4_ops_ffc_quals_idx_0, int *input_arg)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ext_l4_ops_ffc_quals_idx_0,
        DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        input_arg);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_RANGE_MODULE_ID);

    *input_arg = ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0].qual_info.input_arg;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_RANGE_MODULE_ID,
        &((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0].qual_info.input_arg,
        "dnx_field_range_sw_db[%d]->ext_l4_ops_ffc_quals[%d].qual_info.input_arg",
        unit, ext_l4_ops_ffc_quals_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_field_range_sw_db_info,
        DNX_FIELD_RANGE_SW_DB_EXT_L4_OPS_FFC_QUALS_QUAL_INFO_INPUT_ARG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_offset_set(int unit, uint32 ext_l4_ops_ffc_quals_idx_0, int offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ext_l4_ops_ffc_quals_idx_0,
        DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_RANGE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0].qual_info.offset,
        offset,
        int,
        0,
        "dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_offset_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_RANGE_MODULE_ID,
        &offset,
        "dnx_field_range_sw_db[%d]->ext_l4_ops_ffc_quals[%d].qual_info.offset",
        unit, ext_l4_ops_ffc_quals_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_field_range_sw_db_info,
        DNX_FIELD_RANGE_SW_DB_EXT_L4_OPS_FFC_QUALS_QUAL_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_offset_get(int unit, uint32 ext_l4_ops_ffc_quals_idx_0, int *offset)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ext_l4_ops_ffc_quals_idx_0,
        DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        offset);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_RANGE_MODULE_ID);

    *offset = ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0].qual_info.offset;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_RANGE_MODULE_ID,
        &((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[ext_l4_ops_ffc_quals_idx_0].qual_info.offset,
        "dnx_field_range_sw_db[%d]->ext_l4_ops_ffc_quals[%d].qual_info.offset",
        unit, ext_l4_ops_ffc_quals_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_RANGE_MODULE_ID,
        dnx_field_range_sw_db_info,
        DNX_FIELD_RANGE_SW_DB_EXT_L4_OPS_FFC_QUALS_QUAL_INFO_OFFSET_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




const char *
dnx_field_range_type_e_get_name(dnx_field_range_type_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_RANGE_TYPE_INVALID", value, DNX_FIELD_RANGE_TYPE_INVALID);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_RANGE_TYPE_FIRST", value, DNX_FIELD_RANGE_TYPE_FIRST);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_RANGE_TYPE_L4_SRC_PORT", value, DNX_FIELD_RANGE_TYPE_L4_SRC_PORT);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_RANGE_TYPE_L4_DST_PORT", value, DNX_FIELD_RANGE_TYPE_L4_DST_PORT);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_RANGE_TYPE_OUT_LIF", value, DNX_FIELD_RANGE_TYPE_OUT_LIF);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE", value, DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_RANGE_TYPE_L4OPS_PKT_HDR_SIZE", value, DNX_FIELD_RANGE_TYPE_L4OPS_PKT_HDR_SIZE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_RANGE_TYPE_IN_TTL", value, DNX_FIELD_RANGE_TYPE_IN_TTL);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_RANGE_TYPE_FFC1_LOW", value, DNX_FIELD_RANGE_TYPE_FFC1_LOW);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_RANGE_TYPE_FFC1_HIGH", value, DNX_FIELD_RANGE_TYPE_FFC1_HIGH);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_RANGE_TYPE_FFC2_LOW", value, DNX_FIELD_RANGE_TYPE_FFC2_LOW);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_RANGE_TYPE_FFC2_HIGH", value, DNX_FIELD_RANGE_TYPE_FFC2_HIGH);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_RANGE_TYPE_NOF", value, DNX_FIELD_RANGE_TYPE_NOF);

    return NULL;
}





dnx_field_range_sw_db_cbs dnx_field_range_sw_db = 	{
	
	dnx_field_range_sw_db_is_init,
	dnx_field_range_sw_db_init,
		{
		
		dnx_field_range_sw_db_ext_l4_ops_range_types_set,
		dnx_field_range_sw_db_ext_l4_ops_range_types_get}
	,
		{
		
		dnx_field_range_sw_db_ext_l4_ops_ffc_quals_set,
		dnx_field_range_sw_db_ext_l4_ops_ffc_quals_get,
			{
			
			dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dnx_qual_set,
			dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dnx_qual_get}
		,
			{
			
				{
				
				dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_type_set,
				dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_type_get}
			,
				{
				
				dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_arg_set,
				dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_arg_get}
			,
				{
				
				dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_offset_set,
				dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_offset_get}
			}
		}
	}
;
#undef BSL_LOG_MODULE

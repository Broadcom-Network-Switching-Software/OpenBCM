
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_field_apptype_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_apptype_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_apptype_layout.h>





int
dnx_field_apptype_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]),
        "dnx_field_apptype_sw[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_init(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_apptype_sw_t,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_apptype_sw_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_field_apptype_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]),
        "dnx_field_apptype_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]),
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[def_val_idx[1]].is_valid,
        FALSE);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[def_val_idx[1]].apptype,
        bcmFieldAppTypeCount);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[def_val_idx[1]].fwd2_context_ids[def_val_idx[2]],
        -1);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[def_val_idx[1]].valid_for_kbp,
        FALSE);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[def_val_idx[1]].apptype,
        (-1));

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[def_val_idx[1]].acl_context,
        DNX_FIELD_APPTYPE_SWSTATE_ACL_CONTEXT_INVALID);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_set(int unit, uint32 opcode_predef_info_idx_0, CONST dnx_field_predef_opcode_apptype_info_t *opcode_predef_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0],
        opcode_predef_info,
        dnx_field_predef_opcode_apptype_info_t,
        0,
        "dnx_field_apptype_sw_opcode_predef_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO,
        opcode_predef_info,
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d]",
        unit, opcode_predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_get(int unit, uint32 opcode_predef_info_idx_0, dnx_field_predef_opcode_apptype_info_t *opcode_predef_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO,
        opcode_predef_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO,
        DNX_SW_STATE_DIAG_READ);

    *opcode_predef_info = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0],
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d]",
        unit, opcode_predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_is_valid_set(int unit, uint32 opcode_predef_info_idx_0, uint8 is_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__IS_VALID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__IS_VALID,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__IS_VALID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__IS_VALID,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].is_valid,
        is_valid,
        uint8,
        0,
        "dnx_field_apptype_sw_opcode_predef_info_is_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__IS_VALID,
        &is_valid,
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d].is_valid",
        unit, opcode_predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__IS_VALID,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_IS_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_is_valid_get(int unit, uint32 opcode_predef_info_idx_0, uint8 *is_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__IS_VALID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__IS_VALID,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__IS_VALID,
        is_valid);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__IS_VALID,
        DNX_SW_STATE_DIAG_READ);

    *is_valid = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].is_valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__IS_VALID,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].is_valid,
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d].is_valid",
        unit, opcode_predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__IS_VALID,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_IS_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_apptype_set(int unit, uint32 opcode_predef_info_idx_0, uint8 apptype)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__APPTYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__APPTYPE,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__APPTYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__APPTYPE,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].apptype,
        apptype,
        uint8,
        0,
        "dnx_field_apptype_sw_opcode_predef_info_apptype_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__APPTYPE,
        &apptype,
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d].apptype",
        unit, opcode_predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__APPTYPE,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_APPTYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_apptype_get(int unit, uint32 opcode_predef_info_idx_0, uint8 *apptype)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__APPTYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__APPTYPE,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__APPTYPE,
        apptype);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__APPTYPE,
        DNX_SW_STATE_DIAG_READ);

    *apptype = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].apptype;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__APPTYPE,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].apptype,
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d].apptype",
        unit, opcode_predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__APPTYPE,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_APPTYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_cs_profile_id_set(int unit, uint32 opcode_predef_info_idx_0, uint8 cs_profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__CS_PROFILE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__CS_PROFILE_ID,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__CS_PROFILE_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__CS_PROFILE_ID,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].cs_profile_id,
        cs_profile_id,
        uint8,
        0,
        "dnx_field_apptype_sw_opcode_predef_info_cs_profile_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__CS_PROFILE_ID,
        &cs_profile_id,
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d].cs_profile_id",
        unit, opcode_predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__CS_PROFILE_ID,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_CS_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_cs_profile_id_get(int unit, uint32 opcode_predef_info_idx_0, uint8 *cs_profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__CS_PROFILE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__CS_PROFILE_ID,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__CS_PROFILE_ID,
        cs_profile_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__CS_PROFILE_ID,
        DNX_SW_STATE_DIAG_READ);

    *cs_profile_id = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].cs_profile_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__CS_PROFILE_ID,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].cs_profile_id,
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d].cs_profile_id",
        unit, opcode_predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__CS_PROFILE_ID,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_CS_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_nof_fwd2_context_ids_set(int unit, uint32 opcode_predef_info_idx_0, uint8 nof_fwd2_context_ids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].nof_fwd2_context_ids,
        nof_fwd2_context_ids,
        uint8,
        0,
        "dnx_field_apptype_sw_opcode_predef_info_nof_fwd2_context_ids_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        &nof_fwd2_context_ids,
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d].nof_fwd2_context_ids",
        unit, opcode_predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_NOF_FWD2_CONTEXT_IDS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_nof_fwd2_context_ids_get(int unit, uint32 opcode_predef_info_idx_0, uint8 *nof_fwd2_context_ids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        nof_fwd2_context_ids);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        DNX_SW_STATE_DIAG_READ);

    *nof_fwd2_context_ids = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].nof_fwd2_context_ids;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].nof_fwd2_context_ids,
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d].nof_fwd2_context_ids",
        unit, opcode_predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_NOF_FWD2_CONTEXT_IDS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_fwd2_context_ids_set(int unit, uint32 opcode_predef_info_idx_0, uint32 fwd2_context_ids_idx_0, uint8 fwd2_context_ids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__FWD2_CONTEXT_IDS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__FWD2_CONTEXT_IDS,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fwd2_context_ids_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__FWD2_CONTEXT_IDS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__FWD2_CONTEXT_IDS,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].fwd2_context_ids[fwd2_context_ids_idx_0],
        fwd2_context_ids,
        uint8,
        0,
        "dnx_field_apptype_sw_opcode_predef_info_fwd2_context_ids_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__FWD2_CONTEXT_IDS,
        &fwd2_context_ids,
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d].fwd2_context_ids[%d]",
        unit, opcode_predef_info_idx_0, fwd2_context_ids_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__FWD2_CONTEXT_IDS,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_FWD2_CONTEXT_IDS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_fwd2_context_ids_get(int unit, uint32 opcode_predef_info_idx_0, uint32 fwd2_context_ids_idx_0, uint8 *fwd2_context_ids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__FWD2_CONTEXT_IDS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__FWD2_CONTEXT_IDS,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__FWD2_CONTEXT_IDS,
        fwd2_context_ids);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fwd2_context_ids_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__FWD2_CONTEXT_IDS,
        DNX_SW_STATE_DIAG_READ);

    *fwd2_context_ids = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].fwd2_context_ids[fwd2_context_ids_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__FWD2_CONTEXT_IDS,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].fwd2_context_ids[fwd2_context_ids_idx_0],
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d].fwd2_context_ids[%d]",
        unit, opcode_predef_info_idx_0, fwd2_context_ids_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__FWD2_CONTEXT_IDS,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_FWD2_CONTEXT_IDS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_valid_for_kbp_set(int unit, uint32 opcode_predef_info_idx_0, uint8 valid_for_kbp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__VALID_FOR_KBP,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__VALID_FOR_KBP,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__VALID_FOR_KBP,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__VALID_FOR_KBP,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].valid_for_kbp,
        valid_for_kbp,
        uint8,
        0,
        "dnx_field_apptype_sw_opcode_predef_info_valid_for_kbp_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__VALID_FOR_KBP,
        &valid_for_kbp,
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d].valid_for_kbp",
        unit, opcode_predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__VALID_FOR_KBP,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_VALID_FOR_KBP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_opcode_predef_info_valid_for_kbp_get(int unit, uint32 opcode_predef_info_idx_0, uint8 *valid_for_kbp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__VALID_FOR_KBP,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__VALID_FOR_KBP,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        opcode_predef_info_idx_0,
        DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__VALID_FOR_KBP,
        valid_for_kbp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__VALID_FOR_KBP,
        DNX_SW_STATE_DIAG_READ);

    *valid_for_kbp = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].valid_for_kbp;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__VALID_FOR_KBP,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->opcode_predef_info[opcode_predef_info_idx_0].valid_for_kbp,
        "dnx_field_apptype_sw[%d]->opcode_predef_info[%d].valid_for_kbp",
        unit, opcode_predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__OPCODE_PREDEF_INFO__VALID_FOR_KBP,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_OPCODE_PREDEF_INFO_VALID_FOR_KBP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_set(int unit, uint32 user_def_info_idx_0, CONST dnx_field_user_opcode_info_t *user_def_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0],
        user_def_info,
        dnx_field_user_opcode_info_t,
        0,
        "dnx_field_apptype_sw_user_def_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO,
        user_def_info,
        "dnx_field_apptype_sw[%d]->user_def_info[%d]",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_get(int unit, uint32 user_def_info_idx_0, dnx_field_user_opcode_info_t *user_def_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO,
        user_def_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO,
        DNX_SW_STATE_DIAG_READ);

    *user_def_info = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0],
        "dnx_field_apptype_sw[%d]->user_def_info[%d]",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_apptype_set(int unit, uint32 user_def_info_idx_0, uint8 apptype)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_FAMILY_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        DNX2_DEVICE);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].apptype,
        apptype,
        uint8,
        0,
        "dnx_field_apptype_sw_user_def_info_apptype_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        &apptype,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].apptype",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_APPTYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_apptype_get(int unit, uint32 user_def_info_idx_0, uint8 *apptype)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_FAMILY_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        DNX2_DEVICE);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        apptype);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        DNX_SW_STATE_DIAG_READ);

    *apptype = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].apptype;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].apptype,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].apptype",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__APPTYPE,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_APPTYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_acl_context_set(int unit, uint32 user_def_info_idx_0, uint8 acl_context)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].acl_context,
        acl_context,
        uint8,
        0,
        "dnx_field_apptype_sw_user_def_info_acl_context_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT,
        &acl_context,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].acl_context",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_ACL_CONTEXT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_acl_context_get(int unit, uint32 user_def_info_idx_0, uint8 *acl_context)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT,
        acl_context);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT,
        DNX_SW_STATE_DIAG_READ);

    *acl_context = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].acl_context;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].acl_context,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].acl_context",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_ACL_CONTEXT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_base_opcode_set(int unit, uint32 user_def_info_idx_0, uint8 base_opcode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_OPCODE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_OPCODE,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_OPCODE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_OPCODE,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].base_opcode,
        base_opcode,
        uint8,
        0,
        "dnx_field_apptype_sw_user_def_info_base_opcode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_OPCODE,
        &base_opcode,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].base_opcode",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_OPCODE,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_BASE_OPCODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_base_opcode_get(int unit, uint32 user_def_info_idx_0, uint8 *base_opcode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_OPCODE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_OPCODE,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_OPCODE,
        base_opcode);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_OPCODE,
        DNX_SW_STATE_DIAG_READ);

    *base_opcode = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].base_opcode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_OPCODE,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].base_opcode,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].base_opcode",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_OPCODE,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_BASE_OPCODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_profile_id_set(int unit, uint32 user_def_info_idx_0, uint8 profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].profile_id,
        profile_id,
        uint8,
        0,
        "dnx_field_apptype_sw_user_def_info_profile_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID,
        &profile_id,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].profile_id",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_profile_id_get(int unit, uint32 user_def_info_idx_0, uint8 *profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID,
        profile_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID,
        DNX_SW_STATE_DIAG_READ);

    *profile_id = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].profile_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].profile_id,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].profile_id",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_name_stringncat(int unit, uint32 user_def_info_idx_0, char *src)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        SW_STATE_FUNC_STRINGNCAT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGNCAT(
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].name,
        src,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCAT_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCAT,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].name,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].name",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_NAME_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_name_stringncmp(int unit, uint32 user_def_info_idx_0, char *cmp_string, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        SW_STATE_FUNC_STRINGNCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGNCMP(
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].name,
        cmp_string,
        result,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCMP_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCMP,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].name,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].name",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_NAME_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_name_stringncpy(int unit, uint32 user_def_info_idx_0, char *src)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        SW_STATE_FUNC_STRINGNCPY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGNCPY(
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].name,
        src,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGNCPY_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGNCPY,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].name,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].name",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_NAME_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_name_stringlen(int unit, uint32 user_def_info_idx_0, uint32 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        SW_STATE_FUNC_STRINGLEN,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGLEN(
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].name,
        size);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGLEN_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGLEN,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].name,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].name",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_NAME_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_name_stringget(int unit, uint32 user_def_info_idx_0, char *o_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        SW_STATE_FUNC_STRINGGET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        DNX_SW_STATE_DIAG_STRING);

    SW_STATE_STRING_STRINGGET(
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].name,
        o_string);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_STRING_STRINGGET_LOGGING,
        BSL_LS_SWSTATEDNX_STRINGSTRINGGET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].name,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].name",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_NAME_INFO,
        DNX_SW_STATE_DIAG_STRING,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_field_apptype_sw_cbs dnx_field_apptype_sw = 	{
	
	dnx_field_apptype_sw_is_init,
	dnx_field_apptype_sw_init,
		{
		
		dnx_field_apptype_sw_opcode_predef_info_set,
		dnx_field_apptype_sw_opcode_predef_info_get,
			{
			
			dnx_field_apptype_sw_opcode_predef_info_is_valid_set,
			dnx_field_apptype_sw_opcode_predef_info_is_valid_get}
		,
			{
			
			dnx_field_apptype_sw_opcode_predef_info_apptype_set,
			dnx_field_apptype_sw_opcode_predef_info_apptype_get}
		,
			{
			
			dnx_field_apptype_sw_opcode_predef_info_cs_profile_id_set,
			dnx_field_apptype_sw_opcode_predef_info_cs_profile_id_get}
		,
		
		
			{
			
			dnx_field_apptype_sw_opcode_predef_info_nof_fwd2_context_ids_set,
			dnx_field_apptype_sw_opcode_predef_info_nof_fwd2_context_ids_get}
		,
			{
			
			dnx_field_apptype_sw_opcode_predef_info_fwd2_context_ids_set,
			dnx_field_apptype_sw_opcode_predef_info_fwd2_context_ids_get}
		,
			{
			
			dnx_field_apptype_sw_opcode_predef_info_valid_for_kbp_set,
			dnx_field_apptype_sw_opcode_predef_info_valid_for_kbp_get}
		}
	,
		{
		
		dnx_field_apptype_sw_user_def_info_set,
		dnx_field_apptype_sw_user_def_info_get,
			{
			
			dnx_field_apptype_sw_user_def_info_apptype_set,
			dnx_field_apptype_sw_user_def_info_apptype_get}
		
		,
		
			{
			
			dnx_field_apptype_sw_user_def_info_acl_context_set,
			dnx_field_apptype_sw_user_def_info_acl_context_get}
		,
			{
			
			dnx_field_apptype_sw_user_def_info_base_opcode_set,
			dnx_field_apptype_sw_user_def_info_base_opcode_get}
		,
			{
			
			dnx_field_apptype_sw_user_def_info_profile_id_set,
			dnx_field_apptype_sw_user_def_info_profile_id_get}
		,
			{
			
			dnx_field_apptype_sw_user_def_info_name_stringncat,
			dnx_field_apptype_sw_user_def_info_name_stringncmp,
			dnx_field_apptype_sw_user_def_info_name_stringncpy,
			dnx_field_apptype_sw_user_def_info_name_stringlen,
			dnx_field_apptype_sw_user_def_info_name_stringget}
		}
	}
;
#undef BSL_LOG_MODULE

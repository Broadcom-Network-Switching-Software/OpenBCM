
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

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], bcmFieldAppTypeCount)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], bcmFieldAppTypeCount)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[def_val_idx[1]].fwd2_context_ids[def_val_idx[2]],
        -1);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], bcmFieldAppTypeCount)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[def_val_idx[1]].valid_for_kbp,
        FALSE);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[def_val_idx[1]].flags,
        DNX_FIELD_APPTYPE_FLAG_NOF);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_predef_info_set(int unit, uint32 predef_info_idx_0, CONST dnx_field_predef_apptype_info_t *predef_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        predef_info_idx_0,
        bcmFieldAppTypeCount);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[predef_info_idx_0],
        predef_info,
        dnx_field_predef_apptype_info_t,
        0,
        "dnx_field_apptype_sw_predef_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO,
        predef_info,
        "dnx_field_apptype_sw[%d]->predef_info[%d]",
        unit, predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_PREDEF_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_predef_info_get(int unit, uint32 predef_info_idx_0, dnx_field_predef_apptype_info_t *predef_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        predef_info_idx_0,
        bcmFieldAppTypeCount);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO,
        predef_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO,
        DNX_SW_STATE_DIAG_READ);

    *predef_info = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[predef_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[predef_info_idx_0],
        "dnx_field_apptype_sw[%d]->predef_info[%d]",
        unit, predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_PREDEF_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_predef_info_nof_fwd2_context_ids_set(int unit, uint32 predef_info_idx_0, uint8 nof_fwd2_context_ids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        predef_info_idx_0,
        bcmFieldAppTypeCount);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[predef_info_idx_0].nof_fwd2_context_ids,
        nof_fwd2_context_ids,
        uint8,
        0,
        "dnx_field_apptype_sw_predef_info_nof_fwd2_context_ids_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        &nof_fwd2_context_ids,
        "dnx_field_apptype_sw[%d]->predef_info[%d].nof_fwd2_context_ids",
        unit, predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_PREDEF_INFO_NOF_FWD2_CONTEXT_IDS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_predef_info_nof_fwd2_context_ids_get(int unit, uint32 predef_info_idx_0, uint8 *nof_fwd2_context_ids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        predef_info_idx_0,
        bcmFieldAppTypeCount);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        nof_fwd2_context_ids);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        DNX_SW_STATE_DIAG_READ);

    *nof_fwd2_context_ids = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[predef_info_idx_0].nof_fwd2_context_ids;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[predef_info_idx_0].nof_fwd2_context_ids,
        "dnx_field_apptype_sw[%d]->predef_info[%d].nof_fwd2_context_ids",
        unit, predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_PREDEF_INFO_NOF_FWD2_CONTEXT_IDS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_predef_info_fwd2_context_ids_set(int unit, uint32 predef_info_idx_0, uint32 fwd2_context_ids_idx_0, uint8 fwd2_context_ids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        predef_info_idx_0,
        bcmFieldAppTypeCount);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fwd2_context_ids_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[predef_info_idx_0].fwd2_context_ids[fwd2_context_ids_idx_0],
        fwd2_context_ids,
        uint8,
        0,
        "dnx_field_apptype_sw_predef_info_fwd2_context_ids_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS,
        &fwd2_context_ids,
        "dnx_field_apptype_sw[%d]->predef_info[%d].fwd2_context_ids[%d]",
        unit, predef_info_idx_0, fwd2_context_ids_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_PREDEF_INFO_FWD2_CONTEXT_IDS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_predef_info_fwd2_context_ids_get(int unit, uint32 predef_info_idx_0, uint32 fwd2_context_ids_idx_0, uint8 *fwd2_context_ids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        predef_info_idx_0,
        bcmFieldAppTypeCount);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS,
        fwd2_context_ids);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        fwd2_context_ids_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS,
        DNX_SW_STATE_DIAG_READ);

    *fwd2_context_ids = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[predef_info_idx_0].fwd2_context_ids[fwd2_context_ids_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[predef_info_idx_0].fwd2_context_ids[fwd2_context_ids_idx_0],
        "dnx_field_apptype_sw[%d]->predef_info[%d].fwd2_context_ids[%d]",
        unit, predef_info_idx_0, fwd2_context_ids_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_PREDEF_INFO_FWD2_CONTEXT_IDS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_predef_info_valid_for_kbp_set(int unit, uint32 predef_info_idx_0, uint8 valid_for_kbp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        predef_info_idx_0,
        bcmFieldAppTypeCount);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[predef_info_idx_0].valid_for_kbp,
        valid_for_kbp,
        uint8,
        0,
        "dnx_field_apptype_sw_predef_info_valid_for_kbp_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP,
        &valid_for_kbp,
        "dnx_field_apptype_sw[%d]->predef_info[%d].valid_for_kbp",
        unit, predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_PREDEF_INFO_VALID_FOR_KBP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_predef_info_valid_for_kbp_get(int unit, uint32 predef_info_idx_0, uint8 *valid_for_kbp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        predef_info_idx_0,
        bcmFieldAppTypeCount);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP,
        valid_for_kbp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP,
        DNX_SW_STATE_DIAG_READ);

    *valid_for_kbp = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[predef_info_idx_0].valid_for_kbp;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[predef_info_idx_0].valid_for_kbp,
        "dnx_field_apptype_sw[%d]->predef_info[%d].valid_for_kbp",
        unit, predef_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_PREDEF_INFO_VALID_FOR_KBP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_set(int unit, uint32 user_def_info_idx_0, CONST dnx_field_user_apptype_info_t *user_def_info)
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
        dnx_field_user_apptype_info_t,
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
dnx_field_apptype_sw_user_def_info_get(int unit, uint32 user_def_info_idx_0, dnx_field_user_apptype_info_t *user_def_info)
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
dnx_field_apptype_sw_user_def_info_opcode_id_set(int unit, uint32 user_def_info_idx_0, uint8 opcode_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].opcode_id,
        opcode_id,
        uint8,
        0,
        "dnx_field_apptype_sw_user_def_info_opcode_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID,
        &opcode_id,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].opcode_id",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_OPCODE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_opcode_id_get(int unit, uint32 user_def_info_idx_0, uint8 *opcode_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID,
        opcode_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID,
        DNX_SW_STATE_DIAG_READ);

    *opcode_id = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].opcode_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].opcode_id,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].opcode_id",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_OPCODE_ID_INFO,
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
dnx_field_apptype_sw_user_def_info_base_apptype_set(int unit, uint32 user_def_info_idx_0, uint8 base_apptype)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].base_apptype,
        base_apptype,
        uint8,
        0,
        "dnx_field_apptype_sw_user_def_info_base_apptype_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE,
        &base_apptype,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].base_apptype",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_BASE_APPTYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_base_apptype_get(int unit, uint32 user_def_info_idx_0, uint8 *base_apptype)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE,
        base_apptype);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE,
        DNX_SW_STATE_DIAG_READ);

    *base_apptype = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].base_apptype;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].base_apptype,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].base_apptype",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_BASE_APPTYPE_INFO,
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
dnx_field_apptype_sw_user_def_info_flags_set(int unit, uint32 user_def_info_idx_0, dnx_field_apptype_flags_e flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS,
        ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].flags,
        flags,
        dnx_field_apptype_flags_e,
        0,
        "dnx_field_apptype_sw_user_def_info_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS,
        &flags,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].flags",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_apptype_sw_user_def_info_flags_get(int unit, uint32 user_def_info_idx_0, dnx_field_apptype_flags_e *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS,
        sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        user_def_info_idx_0,
        DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS,
        flags);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS,
        DNX_SW_STATE_DIAG_READ);

    *flags = ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS,
        &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[user_def_info_idx_0].flags,
        "dnx_field_apptype_sw[%d]->user_def_info[%d].flags",
        unit, user_def_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS,
        dnx_field_apptype_sw_info,
        DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_FLAGS_INFO,
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



const char *
dnx_field_apptype_flags_e_get_name(dnx_field_apptype_flags_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_APPTYPE_FLAG_WITH_ID", value, DNX_FIELD_APPTYPE_FLAG_WITH_ID);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_APPTYPE_FLAG_NOF", value, DNX_FIELD_APPTYPE_FLAG_NOF);

    return NULL;
}




dnx_field_apptype_sw_cbs dnx_field_apptype_sw = 	{
	
	dnx_field_apptype_sw_is_init,
	dnx_field_apptype_sw_init,
		{
		
		dnx_field_apptype_sw_predef_info_set,
		dnx_field_apptype_sw_predef_info_get,
		
		
			{
			
			dnx_field_apptype_sw_predef_info_nof_fwd2_context_ids_set,
			dnx_field_apptype_sw_predef_info_nof_fwd2_context_ids_get}
		,
			{
			
			dnx_field_apptype_sw_predef_info_fwd2_context_ids_set,
			dnx_field_apptype_sw_predef_info_fwd2_context_ids_get}
		,
			{
			
			dnx_field_apptype_sw_predef_info_valid_for_kbp_set,
			dnx_field_apptype_sw_predef_info_valid_for_kbp_get}
		}
	,
		{
		
		dnx_field_apptype_sw_user_def_info_set,
		dnx_field_apptype_sw_user_def_info_get,
			{
			
			dnx_field_apptype_sw_user_def_info_opcode_id_set,
			dnx_field_apptype_sw_user_def_info_opcode_id_get}
		,
			{
			
			dnx_field_apptype_sw_user_def_info_acl_context_set,
			dnx_field_apptype_sw_user_def_info_acl_context_get}
		,
			{
			
			dnx_field_apptype_sw_user_def_info_base_apptype_set,
			dnx_field_apptype_sw_user_def_info_base_apptype_get}
		,
			{
			
			dnx_field_apptype_sw_user_def_info_profile_id_set,
			dnx_field_apptype_sw_user_def_info_profile_id_get}
		,
			{
			
			dnx_field_apptype_sw_user_def_info_flags_set,
			dnx_field_apptype_sw_user_def_info_flags_get}
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

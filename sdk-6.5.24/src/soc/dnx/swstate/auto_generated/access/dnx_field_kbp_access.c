
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnx/swstate/auto_generated/access/dnx_field_kbp_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_kbp_diagnostic.h>





int
dnx_field_kbp_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]),
        "dnx_field_kbp_sw[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_kbp_sw_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_kbp_sw_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]),
        "dnx_field_kbp_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_field_kbp_sw_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_set(int unit, uint32 opcode_info_idx_0, CONST dnx_field_kbp_opcode_info_t *opcode_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0],
        opcode_info,
        dnx_field_kbp_opcode_info_t,
        0,
        "dnx_field_kbp_sw_opcode_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_KBP_MODULE_ID,
        opcode_info,
        "dnx_field_kbp_sw[%d]->opcode_info[%d]",
        unit, opcode_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_get(int unit, uint32 opcode_info_idx_0, dnx_field_kbp_opcode_info_t *opcode_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        opcode_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    *opcode_info = ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_KBP_MODULE_ID,
        &((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0],
        "dnx_field_kbp_sw[%d]->opcode_info[%d]",
        unit, opcode_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        dnx_field_kbp_opcode_info_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_kbp_sw_opcode_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        "dnx_field_kbp_sw[%d]->opcode_info",
        unit,
        nof_instances_to_alloc_0 * sizeof(dnx_field_kbp_opcode_info_t) / sizeof(*((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[def_val_idx[0]].master_key_info.segment_info[def_val_idx[1]].qual_idx[def_val_idx[2]],
        DNX_FIELD_KBP_PACKED_QUAL_INDEX_INVALID);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[def_val_idx[0]].master_key_info.segment_info[def_val_idx[1]].fg_id,
        DNX_FIELD_GROUP_INVALID);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_set(int unit, uint32 opcode_info_idx_0, CONST dnx_field_kbp_master_key_info_t *master_key_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info,
        master_key_info,
        dnx_field_kbp_master_key_info_t,
        0,
        "dnx_field_kbp_sw_opcode_info_master_key_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_KBP_MODULE_ID,
        master_key_info,
        "dnx_field_kbp_sw[%d]->opcode_info[%d].master_key_info",
        unit, opcode_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_get(int unit, uint32 opcode_info_idx_0, dnx_field_kbp_master_key_info_t *master_key_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        master_key_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    *master_key_info = ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_KBP_MODULE_ID,
        &((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info,
        "dnx_field_kbp_sw[%d]->opcode_info[%d].master_key_info",
        unit, opcode_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_set(int unit, uint32 opcode_info_idx_0, uint32 segment_info_idx_0, CONST dnx_field_kbp_segment_info_t *segment_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        segment_info_idx_0,
        DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info.segment_info[segment_info_idx_0],
        segment_info,
        dnx_field_kbp_segment_info_t,
        0,
        "dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_KBP_MODULE_ID,
        segment_info,
        "dnx_field_kbp_sw[%d]->opcode_info[%d].master_key_info.segment_info[%d]",
        unit, opcode_info_idx_0, segment_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_SEGMENT_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_get(int unit, uint32 opcode_info_idx_0, uint32 segment_info_idx_0, dnx_field_kbp_segment_info_t *segment_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        segment_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        segment_info_idx_0,
        DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    *segment_info = ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info.segment_info[segment_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_KBP_MODULE_ID,
        &((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info.segment_info[segment_info_idx_0],
        "dnx_field_kbp_sw[%d]->opcode_info[%d].master_key_info.segment_info[%d]",
        unit, opcode_info_idx_0, segment_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_SEGMENT_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_set(int unit, uint32 opcode_info_idx_0, uint32 segment_info_idx_0, uint32 qual_idx_idx_0, uint8 qual_idx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        segment_info_idx_0,
        DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_idx_idx_0,
        DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info.segment_info[segment_info_idx_0].qual_idx[qual_idx_idx_0],
        qual_idx,
        uint8,
        0,
        "dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_KBP_MODULE_ID,
        &qual_idx,
        "dnx_field_kbp_sw[%d]->opcode_info[%d].master_key_info.segment_info[%d].qual_idx[%d]",
        unit, opcode_info_idx_0, segment_info_idx_0, qual_idx_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_SEGMENT_INFO_QUAL_IDX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_get(int unit, uint32 opcode_info_idx_0, uint32 segment_info_idx_0, uint32 qual_idx_idx_0, uint8 *qual_idx)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        qual_idx);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        segment_info_idx_0,
        DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qual_idx_idx_0,
        DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    *qual_idx = ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info.segment_info[segment_info_idx_0].qual_idx[qual_idx_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_KBP_MODULE_ID,
        &((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info.segment_info[segment_info_idx_0].qual_idx[qual_idx_idx_0],
        "dnx_field_kbp_sw[%d]->opcode_info[%d].master_key_info.segment_info[%d].qual_idx[%d]",
        unit, opcode_info_idx_0, segment_info_idx_0, qual_idx_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_SEGMENT_INFO_QUAL_IDX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_set(int unit, uint32 opcode_info_idx_0, uint32 segment_info_idx_0, dnx_field_group_t fg_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        segment_info_idx_0,
        DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info.segment_info[segment_info_idx_0].fg_id,
        fg_id,
        dnx_field_group_t,
        0,
        "dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_KBP_MODULE_ID,
        &fg_id,
        "dnx_field_kbp_sw[%d]->opcode_info[%d].master_key_info.segment_info[%d].fg_id",
        unit, opcode_info_idx_0, segment_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_SEGMENT_INFO_FG_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_get(int unit, uint32 opcode_info_idx_0, uint32 segment_info_idx_0, dnx_field_group_t *fg_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        fg_id);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        segment_info_idx_0,
        DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    *fg_id = ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info.segment_info[segment_info_idx_0].fg_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_KBP_MODULE_ID,
        &((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info.segment_info[segment_info_idx_0].fg_id,
        "dnx_field_kbp_sw[%d]->opcode_info[%d].master_key_info.segment_info[%d].fg_id",
        unit, opcode_info_idx_0, segment_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_SEGMENT_INFO_FG_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_set(int unit, uint32 opcode_info_idx_0, uint8 nof_fwd_segments)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info.nof_fwd_segments,
        nof_fwd_segments,
        uint8,
        0,
        "dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_KBP_MODULE_ID,
        &nof_fwd_segments,
        "dnx_field_kbp_sw[%d]->opcode_info[%d].master_key_info.nof_fwd_segments",
        unit, opcode_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_NOF_FWD_SEGMENTS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_get(int unit, uint32 opcode_info_idx_0, uint8 *nof_fwd_segments)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        nof_fwd_segments);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    *nof_fwd_segments = ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info.nof_fwd_segments;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_KBP_MODULE_ID,
        &((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].master_key_info.nof_fwd_segments,
        "dnx_field_kbp_sw[%d]->opcode_info[%d].master_key_info.nof_fwd_segments",
        unit, opcode_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_MASTER_KEY_INFO_NOF_FWD_SEGMENTS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_is_valid_set(int unit, uint32 opcode_info_idx_0, uint8 is_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].is_valid,
        is_valid,
        uint8,
        0,
        "dnx_field_kbp_sw_opcode_info_is_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_KBP_MODULE_ID,
        &is_valid,
        "dnx_field_kbp_sw[%d]->opcode_info[%d].is_valid",
        unit, opcode_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_IS_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_is_valid_get(int unit, uint32 opcode_info_idx_0, uint8 *is_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info,
        opcode_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        is_valid);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_KBP_MODULE_ID);

    *is_valid = ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].is_valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_KBP_MODULE_ID,
        &((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[opcode_info_idx_0].is_valid,
        "dnx_field_kbp_sw[%d]->opcode_info[%d].is_valid",
        unit, opcode_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_KBP_MODULE_ID,
        dnx_field_kbp_sw_info,
        DNX_FIELD_KBP_SW_OPCODE_INFO_IS_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_field_kbp_sw_cbs dnx_field_kbp_sw = 	{
	
	dnx_field_kbp_sw_is_init,
	dnx_field_kbp_sw_init,
		{
		
		dnx_field_kbp_sw_opcode_info_set,
		dnx_field_kbp_sw_opcode_info_get,
		dnx_field_kbp_sw_opcode_info_alloc,
			{
			
			dnx_field_kbp_sw_opcode_info_master_key_info_set,
			dnx_field_kbp_sw_opcode_info_master_key_info_get,
				{
				
				dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_set,
				dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_get,
					{
					
					dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_set,
					dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_get}
				,
					{
					
					dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_set,
					dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_get}
				}
			,
				{
				
				dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_set,
				dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_get}
			}
		,
			{
			
			dnx_field_kbp_sw_opcode_info_is_valid_set,
			dnx_field_kbp_sw_opcode_info_is_valid_get}
		}
	}
;
#endif  
#endif  
#undef BSL_LOG_MODULE

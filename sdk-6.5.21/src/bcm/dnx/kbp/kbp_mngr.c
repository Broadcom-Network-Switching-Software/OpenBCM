/** \file kbp_mngr.c
 *
 *  kbp procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_KBP_MNGR
/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <shared/util.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/kbp/kbp_connectivity.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_field_types_defs.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

#include "kbp_mngr_internal.h"
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */
/*
 * }
 */

/*
 * GLOBALs
 * {
 */

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_key_segment_t_init(
    int unit,
    kbp_mngr_key_segment_t * segment_p)
{
    SHR_FUNC_INIT_VARS(unit);

    segment_p->is_overlay_field = FALSE;
    segment_p->overlay_offset_bytes = DNX_KBP_OVERLAY_FIELD_INVALID;
    segment_p->nof_bytes = DNX_KBP_INVALID_SEGMENT_SIZE;
    sal_memset(segment_p->name, 0, sizeof(segment_p->name));

    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_opcode_lookup_info_t_init(
    int unit,
    kbp_opcode_lookup_info_t * lookup_info_p)
{
    uint32 idx;

    SHR_FUNC_INIT_VARS(unit);

    lookup_info_p->result_index = DNX_KBP_INVALID_RESULT_ID;
    lookup_info_p->dbal_table_id = DNX_KBP_INVALID_DB_ID;
    lookup_info_p->nof_segments = 0;
    lookup_info_p->result_offset = 0;
    lookup_info_p->result_size = 0;

    for (idx = 0; idx < DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP; idx++)
    {
        lookup_info_p->key_segment_index[idx] = DNX_KBP_INVALID_SEGMENT_ID;
    }
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr_internal.h
 */
shr_error_e
kbp_mngr_lookups_disable(
    int unit,
    uint32 context_index)
{
    uint32 aligner_entry_handle_id;
    uint32 kbr_info_entry_handle_id;
    uint32 ffc_bitmap[2] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &aligner_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, &kbr_info_entry_handle_id));

    dbal_entry_key_field32_set(unit, aligner_entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_index);
    dbal_entry_key_field32_set(unit, aligner_entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_0);

    dbal_entry_value_field32_set(unit, aligner_entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, aligner_entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, aligner_entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, aligner_entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, kbr_info_entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, DBAL_DEFINE_FWD12_KBR_ID_KBP_0);
    dbal_entry_key_field32_set(unit, kbr_info_entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_index);
    dbal_entry_value_field_arr32_set(unit, kbr_info_entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, kbr_info_entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, kbr_info_entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, DBAL_DEFINE_FWD12_KBR_ID_KBP_1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, kbr_info_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (g_kbp_compiled)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_init_internal(unit));
    }

    if (!sw_state_is_warm_boot(unit))
    {
        /** update the aligner for the KBP in case that KBP lookup is not valid */
        if (!dnx_data_elk.general.device_type_get(unit))
        {
            uint32 context_index;

            for (context_index = 0; context_index < DNX_KBP_FWD_CONTEXT_LAST; context_index++)
            {
                SHR_IF_ERR_EXIT(kbp_mngr_lookups_disable(unit, context_index));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (g_kbp_compiled)
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)

/**
 * See kbp_mngr.h
 */
shr_error_e
kbp_mngr_opcode_printable_entry_get(
    int unit,
    uint32 flags,
    uint8 fwd_context,
    uint8 acl_context,
    uint32 *key_sig_value,
    int key_sig_size,
    uint32 *res_sig_value,
    int res_sig_size,
    uint8 *nof_print_info,
    kbp_printable_entry_t * entry_print_info)
{
    uint8 opcode_id;
    uint8 nof_key_print_info = 0;
    uint8 nof_res_print_info = 0;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    *nof_print_info = 0;

    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_get(unit, fwd_context, acl_context, &opcode_id, NULL));

    /** Value 2 is for FWD and RPF lookups */
    sal_memset(entry_print_info, 0x0, DNX_KBP_MAX_NOF_LOOKUPS * sizeof(kbp_printable_entry_t));

    /** Key parsing */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_printable_entry_key_parsing
                    (unit, opcode_id, key_sig_value, key_sig_size, &nof_key_print_info, entry_print_info));
    *nof_print_info = nof_key_print_info;

    /** Result parsing */
    rv = kbp_mngr_opcode_printable_entry_result_parsing
        (unit, flags, opcode_id, res_sig_value, res_sig_size, &nof_res_print_info, entry_print_info);

    /** we set the current err here as we don't want err message to print in case of _SHR_ERR_NOT_FOUND */
    SHR_SET_CURRENT_ERR(rv);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_to_contexts_get(
    int unit,
    uint8 opcode_id,
    uint8 *fwd_nof_contexts,
    kbp_mngr_fwd_acl_context_mapping_t fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE])
{
    uint32 entry_handle_id;
    int ctx_index;
    uint32 ctx_id;
    uint32 nof_ctxs;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(fwd_nof_contexts, _SHR_E_PARAM, "fwd_nof_contexts");
    SHR_NULL_CHECK(fwd_acl_ctx_mapping, _SHR_E_PARAM, "fwd_acl_ctx_mapping");

    sal_memset(fwd_acl_ctx_mapping, 0,
               (sizeof(fwd_acl_ctx_mapping[0]) * DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_OPCODE_FWD_CONTEXTS, INST_SINGLE, &nof_ctxs));

    if (nof_ctxs == 0)
    {
        (*fwd_nof_contexts) = nof_ctxs;
        SHR_EXIT();
    }
    if (nof_ctxs > DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "number of contexts (%d) for opcode %d is above maximum (%d).\n",
                     nof_ctxs, opcode_id, DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);
    }

    for (ctx_index = 0; ctx_index < nof_ctxs; ctx_index++)
    {
        /** FWD context is retrieved as enum; convert it to HW value. */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_OPCODE_FWD_CONTEXT, ctx_index, &ctx_id));
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_FWD2_CONTEXT_ID, ctx_id, &ctx_id));

        fwd_acl_ctx_mapping[ctx_index].fwd_context = ctx_id;

        
        if (opcode_id >= DNX_KBP_ACL_CONTEXT_BASE)
        {
            fwd_acl_ctx_mapping[ctx_index].acl_contexts[0] = opcode_id;
        }
        else
        {
            fwd_acl_ctx_mapping[ctx_index].acl_contexts[0] = ctx_id;
        }
        fwd_acl_ctx_mapping[ctx_index].nof_acl_contexts = 1;
    }

    (*fwd_nof_contexts) = nof_ctxs;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_context_to_opcode_get(
    int unit,
    uint8 fwd_context,
    uint8 acl_context,
    uint8 *opcode_id,
    char *opcode_name)
{
    uint32 entry_handle_id;
    uint8 is_valid = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_translate(unit, fwd_context, acl_context, opcode_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, (*opcode_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_valid));

    if (!is_valid)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode (%d) not valid\n", *opcode_id);
    }

    if (opcode_name)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_name));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_sync(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    if (dnx_kbp_device_enabled(unit))
    {
        SHR_IF_ERR_EXIT(kbp_mngr_sync_internal(unit));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device is not enabled\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_wb_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    if (dnx_kbp_device_enabled(unit))
    {
        SHR_IF_ERR_EXIT(kbp_mngr_wb_init_internal(unit));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device is not enabled\n");
    }

exit:
    SHR_FUNC_EXIT;
}

#endif /** defined (BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_status_update(
    int unit,
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    /** Updating KBP device status */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_STATUS, INST_SINGLE, kbp_mngr_status);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_status_get(
    int unit,
    dbal_enum_value_field_kbp_device_status_e * kbp_mngr_status)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * KBP compilation check is removed, because this API is used in BCM APIs
     * and will always produce an error when KBP is not compiled
     */

    /** Get the KBP device status */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_KBP_STATUS, INST_SINGLE, kbp_mngr_status);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_ipv4_public_enabled_get(
    int unit,
    uint8 *enabled)
{
    (*enabled) = 0;
    return _SHR_E_NONE;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_ipv6_public_enabled_get(
    int unit,
    uint8 *enabled)
{
    (*enabled) = 0;
    return _SHR_E_NONE;
}


shr_error_e
kbp_mngr_opcode_nof_acl_ffcs(
    int unit,
    uint8 opcode_id,
    uint32 *nof_ffcs)
{
    uint8 is_dynamic;
    uint8 is_standard_image;
    SHR_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_is_dynamic(unit, opcode_id, &is_dynamic));
    if (is_dynamic)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "function only supports predefined opcodes (given opcode %d). \n", opcode_id);
    }

    

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));
    if (is_standard_image)
    {
        uint32 opcode_id_enum;
        SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_KBP_FORWARD_APP_TYPES, opcode_id, &opcode_id_enum));
        if (opcode_id_enum == DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_NOP
            || opcode_id_enum == DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_MPLS)
        {
            (*nof_ffcs) = 16;
        }
        else if ((opcode_id_enum == DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_BRIDGE_SVL)
                 || (opcode_id_enum == DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_SRV6_ENDPOINT_CLASSIC)
                 || (opcode_id_enum == DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_SRV6_ENDPOINT_USID))
        {
            (*nof_ffcs) = 12;
        }
        else
        {
            (*nof_ffcs) = 8;
        }
    }
    else
    {
        (*nof_ffcs) = 16;
    }

exit:
    SHR_FUNC_EXIT;
}

#if defined (BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_db_create(
    int unit,
    uint8 counters_enable,
    dbal_tables_e dbal_table_id,
    uint32 initial_capacity,
    int algo_mode)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    DNX_KBP_BLOCK_API_IF_DEVICE_IS_LOCKED();

    SHR_IF_ERR_EXIT(kbp_mngr_db_create_internal
                    (unit, dbal_table_id, 0, counters_enable, initial_capacity, algo_mode, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_db_properties_get(
    int unit,
    dbal_tables_e dbal_table_id,
    uint8 *counters_enable,
    uint32 *initial_capacity,
    int *algo_mode)
{
    uint32 kbp_db_id;
    uint32 entry_handle_id;
    uint32 algo_mode_uint32;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, dbal_table_id, &kbp_db_id, NULL));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, kbp_db_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ALGO_MODE, INST_SINGLE, &algo_mode_uint32));
    if (algo_mode_uint32 == DBAL_ENUM_FVAL_ALGO_MODE_DEFAULT)
    {
        *algo_mode = DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION;
    }
    else
    {
        *algo_mode = algo_mode_uint32;
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, initial_capacity));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_ENTRY_COUNTERS_EN, INST_SINGLE, counters_enable));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_db_capacity_get(
    int unit,
    uint8 db_id,
    uint32 *nof_entries,
    uint32 *estimated_capacity)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_db_capacity_get_internal(unit, db_id, nof_entries, estimated_capacity));

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_total_result_size_get(
    int unit,
    uint8 opcode_id,
    uint32 *result_size)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_total_result_size_get_internal(unit, opcode_id, result_size));

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_master_key_set(
    int unit,
    uint8 opcode_id,
    uint32 nof_segments,
    kbp_mngr_key_segment_t * key_segments)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_set_internal(unit, opcode_id, nof_segments, key_segments));

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_master_key_get(
    int unit,
    uint32 opcode_id,
    uint32 *nof_key_segments,
    kbp_mngr_key_segment_t * key_segments)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get_internal(unit, opcode_id, nof_key_segments, key_segments));
exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_master_key_segments_add(
    int unit,
    uint32 opcode_id,
    uint32 new_segments_index,
    uint32 nof_new_segments,
    kbp_mngr_key_segment_t * key_segments)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add_internal
                    (unit, opcode_id, new_segments_index, nof_new_segments, key_segments));
exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_master_key_segment_remove(
    int unit,
    uint32 opcode_id,
    uint32 segment_index)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segment_remove_internal(unit, opcode_id, segment_index));
exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_lookup_add(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_add_internal(unit, opcode_id, lookup_info));
exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_lookup_get(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_get_internal(unit, opcode_id, lookup_info));
exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_result_clear(
    int unit,
    uint8 opcode_id,
    uint8 result_id)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_result_clear_internal(unit, opcode_id, result_id));
exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_clear(
    int unit,
    uint8 opcode_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    DNX_KBP_BLOCK_API_IF_DEVICE_IS_LOCKED();

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
kbp_mngr_opcode_create(
    int unit,
    uint8 source_opcode_id,
    char *opcode_name,
    uint8 *acl_ctx,
    uint8 *new_opcode_id)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    DNX_KBP_BLOCK_API_IF_DEVICE_IS_LOCKED();

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_create_internal(unit, source_opcode_id, opcode_name, acl_ctx, new_opcode_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
kbp_mngr_opcode_is_dynamic(
    int unit,
    uint8 opcode_id,
    uint8 *is_dynamic)
{
    SHR_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    if (opcode_id >= DNX_KBP_MAX_NOF_OPCODES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "opcode %d is out of range (%d-%d). \n", opcode_id, 0,
                     (DNX_KBP_MAX_NOF_OPCODES - 1));
    }

    if (opcode_id >= DNX_KBP_DYNAMIC_OPCODE_BASE)
    {
        (*is_dynamic) = TRUE;
    }
    else
    {
        (*is_dynamic) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
kbp_mngr_context_acl_is_dynamic(
    int unit,
    uint8 acl_ctx,
    uint8 *is_dynamic)
{
    SHR_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    if (acl_ctx >= DNX_KBP_NOF_FWD_CONTEXTS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "context %d is out of range (%d-%d). \n", acl_ctx, 0,
                     (DNX_KBP_NOF_FWD_CONTEXTS - 1));
    }

    if (acl_ctx >= DNX_KBP_ACL_CONTEXT_BASE)
    {
        (*is_dynamic) = TRUE;
    }
    else
    {
        (*is_dynamic) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_segment_used_result_indexes_get(
    int unit,
    uint8 opcode_id,
    char *acl_qual_name,
    uint32 result_ids[DNX_KBP_MAX_NOF_RESULTS])
{
    uint8 valid = 0;
    uint32 entry_handle_id;
    uint32 result_bmp = 0;
    uint8 result_id;
    kbp_opcode_lookup_info_t lookup_info;
    int is_segment_exist;
    uint32 nof_key_segments;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    int result_index_internal;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    for (result_id = 0; result_id < DNX_KBP_MAX_NOF_RESULTS; result_id++)
    {
        /** Initialize the Result IDs array to default values. */
        result_ids[result_id] = DNX_KBP_INVALID_RESULT_ID;
    }

    is_segment_exist = FALSE;
    result_index_internal = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &valid));

    if (!valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Opcode %d is not valid\n", opcode_id);
    }

    /** Get the result bitmap. */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get_internal(unit, opcode_id, &nof_key_segments, ms_key_segments));

    /** Iterate over all result IDs. */
    for (result_id = 0; result_id < DNX_KBP_MAX_NOF_RESULTS; result_id++)
    {
        /** Check if the current result ID is set in the result_bmp. */
        if (utilex_bitstream_test_bit(&result_bmp, result_id))
        {
            int seg_index;

            /** Get the lookup info for the current result ID. */
            SHR_IF_ERR_EXIT(kbp_opcode_lookup_info_t_init(unit, &lookup_info));
            lookup_info.result_index = result_id;
            SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_get_internal(unit, opcode_id, &lookup_info));

            /** Iterate over all segments related to the current result ID. */
            for (seg_index = 0; seg_index < lookup_info.nof_segments; seg_index++)
            {
                /** Compare the fwd segment name (taken from the ms_key) with the given qualifier name. */
                is_segment_exist =
                    sal_memcmp(ms_key_segments[lookup_info.key_segment_index[seg_index]].name, acl_qual_name,
                               DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
                if (is_segment_exist == 0)
                {
                    result_ids[result_index_internal] = result_id;
                    result_index_internal++;
                    break;
                }
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_context_to_opcode_translate(
    int unit,
    uint8 fwd_context,
    uint8 acl_context,
    uint8 *opcode_id)
{
    uint32 entry_handle_id;
    uint32 fwd_context_enum;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_MAPPING, &entry_handle_id));

    /** Update the enum value using the HW value */
    SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_context, &fwd_context_enum));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_context_enum);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT, acl_context);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_KBP_OPCODE_ID, INST_SINGLE, opcode_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** No matching opcode, the table was not updated */
    if ((*opcode_id) == 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        *opcode_id = -1;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * API which takes dbal_tables_e and and validates the relevant KBP application is enabled
 * Also performs validation on the input capacity (only for Multicast).
 */
static shr_error_e
kbp_mngr_table_capacity_validate(
    int unit,
    dbal_tables_e dbal_table_id,
    int capacity_value,
    uint8 validate_capacity)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dbal_table_id == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD ||
        dbal_table_id == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF ||
        dbal_table_id == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD)
    {
        /** IPv4 */
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4) == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "External IPv4 application is not enabled\n");
        }
        if (dbal_table_id == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF)
        {
            if (!kbp_mngr_split_rpf_in_use(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "KBP RPF tables are not available in non-split RPF mode");
            }
        }
        else if (dbal_table_id == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD)
        {
            if (validate_capacity && (capacity_value > dnx_data_elk.application.ipv4_mc_hash_table_size_get(unit)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "KBP IPv4 Multicast table capacity cannot exceed %d\n",
                             dnx_data_elk.application.ipv4_mc_hash_table_size_get(unit));
            }
        }
    }
    else if (dbal_table_id == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD ||
             dbal_table_id == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF ||
             dbal_table_id == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD)
    {
        /** IPv6 */
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6) == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "External IPv6 application is not enabled\n");
        }
        if (dbal_table_id == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF)
        {
            if (!kbp_mngr_split_rpf_in_use(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "KBP RPF tables are not available in non-split RPF mode");
            }
        }
        else if (dbal_table_id == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD)
        {
            if (validate_capacity && (capacity_value > dnx_data_elk.application.ipv6_mc_hash_table_size_get(unit)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "KBP IPv6 Multicast table capacity cannot exceed %d\n",
                             dnx_data_elk.application.ipv6_mc_hash_table_size_get(unit));
            }
        }
    }
    else if (dbal_table_id == DBAL_TABLE_KBP_NAT_EXTERNAL_TO_INTERNAL ||
             dbal_table_id == DBAL_TABLE_KBP_NAT_INTERNAL_TO_EXTERNAL)
    {
        /** NAT */
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_nat) == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "External NAT application is not enabled\n");
        }
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "External NAT tables capacity configuration is not supported\n");
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported DBAL table provided (%d) %s\n", dbal_table_id,
                     dbal_logical_table_to_string(unit, dbal_table_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_table_capacity_set(
    int unit,
    dbal_tables_e dbal_table_id,
    uint8 is_opt,
    int capacity_value)
{
    uint32 entry_handle_id;
    uint32 db_capacity = 0;
    uint32 ad_reg_capacity = 0;
    uint32 ad_opt_capacity = 0;
    uint32 db_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(kbp_mngr_table_capacity_validate(unit, dbal_table_id, capacity_value, TRUE));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));

    /** Get the DB ID related to the DBAL table */
    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, dbal_table_id, &db_id, NULL));

    /** Get the current capacities */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, db_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, &db_capacity);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RESULT_CAPACITY, INST_SINGLE, &ad_reg_capacity);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OPTIMIZED_CAPACITY, INST_SINGLE, &ad_opt_capacity);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Update the capacity value or do nothing if the value is the same as the existing one */
    if (is_opt)
    {
        if (ad_opt_capacity == capacity_value)
        {
            SHR_EXIT();
        }
        else
        {
            ad_opt_capacity = capacity_value;
        }
    }
    else
    {
        if (ad_reg_capacity == capacity_value)
        {
            SHR_EXIT();
        }
        else
        {
            ad_reg_capacity = capacity_value;
        }
    }

    /** Set DB capacity to the highest AD capacity */
    db_capacity = (ad_reg_capacity > ad_opt_capacity) ? ad_reg_capacity : ad_opt_capacity;

    /** Update the capacity values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, db_capacity);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_CAPACITY, INST_SINGLE, ad_reg_capacity);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPTIMIZED_CAPACITY, INST_SINGLE, ad_opt_capacity);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_table_capacity_get(
    int unit,
    dbal_tables_e dbal_table_id,
    uint8 is_opt,
    int *capacity_value)
{
    uint32 entry_handle_id;
    uint32 ad_capacity = 0;
    uint32 db_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(capacity_value, _SHR_E_PARAM, "capacity_value");

    SHR_IF_ERR_EXIT(kbp_mngr_table_capacity_validate(unit, dbal_table_id, *capacity_value, FALSE));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));

    /** Get the DB ID related to the DBAL table */
    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, dbal_table_id, &db_id, NULL));

    /** Get the current capacities */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, db_id);
    dbal_value_field32_request(unit, entry_handle_id,
                               (is_opt ? DBAL_FIELD_OPTIMIZED_CAPACITY : DBAL_FIELD_RESULT_CAPACITY), INST_SINGLE,
                               &ad_capacity);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *capacity_value = ad_capacity;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
uint8
kbp_mngr_split_rpf_in_use(
    int unit)
{
    return (dnx_data_elk.application.split_rpf_get(unit) ? TRUE : FALSE);
}

uint8
kbp_mngr_ipv4_in_use(
    int unit)
{
    return (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4) ? TRUE : FALSE);
}

uint8
kbp_mngr_ipv6_in_use(
    int unit)
{
    return (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6) ? TRUE : FALSE);
}

extern generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES];
extern kbp_warmboot_t Kbp_warmboot_data[SOC_MAX_NUM_DEVICES];

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_wb_sync(
    int unit)
{
    kbp_warmboot_t *warmboot_data;

    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    if (dnx_kbp_device_enabled(unit))
    {
        dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));

        /** If KBP sync was not performed, skip KBP warmboot sync */
        if (DNX_KBP_STATUS_IS_LOCKED(kbp_mngr_status) == FALSE)
        {
            SHR_EXIT();
        }

        warmboot_data = &Kbp_warmboot_data[unit];

        DNX_KBP_TRY(kbp_device_save_state_and_continue
                    (Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER], warmboot_data->kbp_file_read,
                     warmboot_data->kbp_file_write, warmboot_data->kbp_file_fp));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
uint8
kbp_mngr_smt_enabled(
    int unit)
{
    return (((Kbp_app_data[unit]->device_type == KBP_DEVICE_OP2)
             && (dnx_data_elk.connectivity.connect_mode_get(unit) !=
                 DNX_NIF_ELK_TCAM_CONNECT_MODE_SINGLE_PORT)) ? TRUE : FALSE);
}

/**
 * See kbp_mngr.h
 */
shr_error_e
dnx_kbp_device_init(
    int unit)
{
    int kbp_inst;
    uint32 flags = Kbp_app_data[unit]->flags;
    uint8 kbp_device_connect_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    flags |= KBP_DEVICE_ISSU /** this falg is used for WB support */ ;

    if (sw_state_is_warm_boot(unit))
    {
        flags |= KBP_DEVICE_SKIP_INIT;
    }

    switch (dnx_data_elk.connectivity.connect_mode_get(unit))
    {
        case DNX_NIF_ELK_TCAM_CONNECT_MODE_SINGLE:
        {
            kbp_device_connect_mode = DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE;
            break;
        }
        case DNX_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED:
        {
            kbp_device_connect_mode = DBAL_ENUM_FVAL_KBP_DEVICE_MODE_DUAL_SHARED;
            break;
        }
        case DNX_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT:
        {
            kbp_device_connect_mode = DBAL_ENUM_FVAL_KBP_DEVICE_MODE_DUAL_SMT;
            break;
        }
        case DNX_NIF_ELK_TCAM_CONNECT_MODE_SINGLE_PORT:
        {
            kbp_device_connect_mode = DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE_PORT;
            break;
        }
        case DNX_NIF_ELK_TCAM_CONNECT_MODE_SINGLE_PORT_SMT:
        {
            kbp_device_connect_mode = DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE_PORT_SMT;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported KBP device connect mode provided\n");
        }
    }

    SHR_IF_ERR_EXIT(kbp_mngr_wb_file_open(unit, "kbp", Kbp_app_data[unit]->device_type));

    DNX_KBP_INST_ITER(unit, kbp_inst)
    {
        if (!Kbp_app_data[unit]->dalloc_p[kbp_inst])
        {
            SHR_ERR_EXIT(_SHR_E_INIT, " default allocator was not performed.\n");
        }

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Initialize KBP device ID %d\n"), kbp_inst));
        DNX_KBP_TRY(kbp_device_init
                    (Kbp_app_data[unit]->dalloc_p[kbp_inst], Kbp_app_data[unit]->device_type, flags,
                     (struct kbp_xpt *) Kbp_app_data[unit]->xpt_p[kbp_inst], NULL,
                     &Kbp_app_data[unit]->device_p[kbp_inst]));
    }

    if (kbp_mngr_smt_enabled(unit))
    {
        DNX_KBP_TRY(kbp_device_thread_init(Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER], 0,
                                           &Kbp_app_data[unit]->smt_p[DNX_KBP_INST_MASTER][0]));
        DNX_KBP_TRY(kbp_device_thread_init(Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER], 1,
                                           &Kbp_app_data[unit]->smt_p[DNX_KBP_INST_MASTER][1]));
    }

    if (!sw_state_is_warm_boot(unit))
    {
        uint32 entry_handle_id;

        /** Update the device connect mode */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEVICE_CONNECT_MODE, INST_SINGLE,
                                     kbp_device_connect_mode);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_INIT_DONE));
    }
    else
    {
        SHR_IF_ERR_EXIT(kbp_mngr_wb_init(unit));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr.h
 */
shr_error_e
dnx_kbp_device_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (Kbp_app_data[unit] && (Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER] != NULL))
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Deinitialize KBP device ID %d\n"), DNX_KBP_INST_MASTER));
        DNX_KBP_TRY(kbp_device_destroy(Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER]));
    }

exit:
    SHR_FUNC_EXIT;
}

#else
/* *INDENT-OFF* */
/** dummy prototype to return error if KBP not compiled */

shr_error_e kbp_mngr_sync(int unit){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_sync")}
shr_error_e kbp_mngr_wb_sync(int unit){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_wb_sync")}

shr_error_e kbp_mngr_opcode_lookup_add(int unit, uint8 opcode_id, kbp_opcode_lookup_info_t * lookup_info){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_lookup_add_internal")}
shr_error_e kbp_mngr_opcode_lookup_get(int unit, uint8 opcode_id, kbp_opcode_lookup_info_t * lookup_info){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_lookup_get_internal")}

shr_error_e kbp_mngr_opcode_result_clear(int unit, uint8 opcode_id, uint8 lookup_idx){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_result_clear_internal")}

shr_error_e kbp_mngr_opcode_master_key_set(int unit, uint8 opcode_id, uint32 nof_segments, kbp_mngr_key_segment_t * key_segments){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_master_key_set_internal")}
shr_error_e kbp_mngr_opcode_master_key_get(int unit, uint32 opcode_id, uint32 *nof_key_segments, kbp_mngr_key_segment_t * key_segments){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_master_key_get_internal")}
shr_error_e kbp_mngr_opcode_master_key_segments_add(int unit, uint32 opcode_id, uint32 new_segments_index, uint32 nof_new_segments, kbp_mngr_key_segment_t * key_segments){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_master_key_segments_add_internal")}
shr_error_e kbp_mngr_opcode_master_key_segment_remove(int unit, uint32 opcode_id, uint32 segment_index){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_master_key_segment_remove_internal")}
shr_error_e kbp_mngr_opcode_create(int unit,uint8 source_opcode_id,char *opcode_name,uint8 *acl_ctx,uint8 *new_opcode_id){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_create_internal")}

shr_error_e kbp_mngr_opcode_is_dynamic(int unit, uint8 opcode_id, uint8 *is_dynamic){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_is_dynamic");}
shr_error_e kbp_mngr_context_acl_is_dynamic(int unit, uint8 opcode_id, uint8 *is_dynamic){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_context_acl_is_dynamic");}
shr_error_e kbp_mngr_segment_used_result_indexes_get(int unit,uint8 opcode_id,char *acl_qual_name,uint32 result_ids[DNX_KBP_MAX_NOF_RESULTS]){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_segment_used_result_indexes_get");}
shr_error_e kbp_mngr_context_to_opcode_translate(int unit,uint8 fwd_context,uint8 acl_context,uint8 *opcode_id){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_context_to_opcode_translate");}

shr_error_e kbp_mngr_opcode_total_result_size_get(int unit, uint8 opcode_id, uint32 *result_size){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_total_result_size_get_internal")}

shr_error_e kbp_mngr_db_create(int unit,uint8 counters_enable,dbal_tables_e dbal_table_id,uint32 initial_capacity,int algo_mode){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_db_create");}
shr_error_e kbp_mngr_db_properties_get(int unit,dbal_tables_e dbal_table_id,uint8 *counters_enable,uint32 *initial_capacity,int *algo_mode){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_db_properties_get");}
shr_error_e kbp_mngr_db_capacity_get(int unit, uint8 db_id, uint32 * nof_entries, uint32 * estimated_capacity){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_db_capacity_get_internal")}

shr_error_e kbp_mngr_table_capacity_set(int unit,dbal_tables_e dbal_table_id,uint8 is_opt,int capacity_value){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_table_capacity_set");}
shr_error_e kbp_mngr_table_capacity_get(int unit,dbal_tables_e dbal_table_id,uint8 is_opt,int *capacity_value){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_table_capacity_get");}

/** FWD related APIs used in L3 */
uint8 kbp_mngr_split_rpf_in_use(int unit){return 0;}
uint8 kbp_mngr_ipv4_in_use(int unit){return 0;}
uint8 kbp_mngr_ipv6_in_use(int unit){return 0;}

shr_error_e kbp_mngr_opcode_printable_entry_get(int unit,uint32 flags,uint8 fwd_context,uint8 acl_context,uint32 *key_sig_value,int key_sig_size,uint32 *res_sig_value,int res_sig_size,uint8 *nof_print_info,kbp_printable_entry_t * entry_print_info){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_printable_entry_get");}
shr_error_e kbp_mngr_opcode_to_contexts_get(int unit,uint8 opcode_id,uint8 *fwd_nof_contexts,kbp_mngr_fwd_acl_context_mapping_t fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE]){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_to_contexts_get");}
shr_error_e kbp_mngr_context_to_opcode_get(int unit,uint8 fwd_context,uint8 acl_context,uint8 *opcode_id,char *opcode_name){DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_context_to_opcode_get");}

/* *INDENT-ON* */
#endif /** defined (BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */

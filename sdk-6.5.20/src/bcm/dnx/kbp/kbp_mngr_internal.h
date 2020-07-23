/*! \file src/bcm/dnx/kbp/kbp_mngr_internal.h
 *
 * Internal DNX KBP APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _KBP_MNGR_INTERNAL_H_INCLUDED__
/*
 * {
 */
#define _KBP_MNGR_INTERNAL_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/dnx/dbal/dbal.h>
#include "src/soc/dnx/dbal/dbal_internal.h"
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define DNX_KBP_DUMMY_HOLE_DB_ID 0
#define DNX_KBP_KEY_SEGMENT_NOT_CONFIGURED "NOT_CONFIGURED"
#define DNX_KBP_OPCODES_XML_FILE_PATH_FROM_IMAGE_NAME(_opcodes_path_, _image_name_) \
do { \
    sal_snprintf(_opcodes_path_, DBAL_MAX_PRINTABLE_BUFFER_SIZE, "jericho2_a0/kbp/auto_generated/%s/%s_kbp_opcodes_definition.xml", _image_name_, _image_name_); \
} while(0)

/** Every master key must be a multiplier of 80 bits (10 bytes).*/
#define DNX_KBP_MASTER_KEY_BYTE_GRANULARITY   (10)
/** The minimum size result can have in bytes..*/
#define DNX_KBP_RESULT_SIZE_MIN_BYTES         (1)

#define DNX_KBP_ACL_CONTEXT_BASE              (DNX_KBP_FWD_CONTEXT_LAST)
#define DNX_KBP_NOF_FWD_CONTEXTS              (64)  /** NOF FWD2 contexts*/

#define DNX_KBP_DYNAMIC_OPCODE_BASE           (DNX_KBP_MAX_STATIC_OPCODES_NOF)

#define DNX_KBP_ENTRY_PARSE_FWD_ONLY          (1)

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
 * FUNCTIONs
 * {
 */

shr_error_e kbp_mngr_opcode_lookup_set(
    int unit,
    uint8 opcode_id,
    uint8 lookup_id,
    uint8 lookup_type,
    uint8 lookup_db,
    uint8 result_id,
    uint8 nof_key_segments,
    uint8 key_segment_index[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP]);

shr_error_e kbp_mngr_opcode_result_set(
    int unit,
    uint8 opcode_id,
    uint8 result_id,
    uint8 offset,
    uint8 size);

/**
 * See kbp_mngr_db_create() in kbp_mngr.h
 */
shr_error_e kbp_mngr_db_create_internal(
    int unit,
    dbal_tables_e table_id,
    uint8 clone_id,
    uint8 counters_enable,
    uint32 initial_capacity,
    int algo_mode,
    uint8 *db_id);

/**
 * See kbp_mngr_db_capacity_get() in kbp_mngr.h
 */
shr_error_e kbp_mngr_db_capacity_get_internal(
    int unit,
    uint8 db_id,
    uint32 *nof_entries,
    uint32 *estimated_capacity);

shr_error_e kbp_mngr_opcode_printable_entry_result_parsing(
    int unit,
    uint32 flags,
    uint8 opcode_id,
    uint32 *res_sig_value,
    int res_sig_size,
    uint8 *kbp_entry_print_num,
    kbp_printable_entry_t * kbp_entry_print_info);

shr_error_e kbp_mngr_opcode_printable_entry_key_parsing(
    int unit,
    uint8 opcode_id,
    uint32 *key_sig_value,
    int key_sig_size,
    uint8 *nof_print_info,
    kbp_printable_entry_t * kbp_entry_print_info);

/**
 * See kbp_mngr_opcode_total_result_size_get() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_total_result_size_get_internal(
    int unit,
    uint8 opcode_id,
    uint32 *result_size);

/**
 * See kbp_mngr_opcode_lookup_add() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_lookup_add_internal(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info);

/**
 * See kbp_mngr_opcode_lookup_get() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_lookup_get_internal(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info);

/**
 * See kbp_mngr_opcode_result_clear() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_result_clear_internal(
    int unit,
    uint8 opcode_id,
    uint8 result_id);

shr_error_e kbp_mngr_context_to_opcode_set(
    int unit,
    uint8 fwd_context,
    uint8 acl_context,
    uint8 opcode_id);

/**
 * See kbp_mngr_opcode_master_key_set() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_master_key_set_internal(
    int unit,
    uint8 opcode_id,
    uint32 nof_segments,
    kbp_mngr_key_segment_t * key_segments);

/**
 * See kbp_mngr_opcode_master_key_get() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_master_key_get_internal(
    int unit,
    uint32 opcode_id,
    uint32 *nof_key_segments,
    kbp_mngr_key_segment_t * key_segments);

/**
 * See kbp_mngr_opcode_master_key_segments_add() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_master_key_segments_add_internal(
    int unit,
    uint32 opcode_id,
    uint32 new_segments_index,
    uint32 nof_new_segments,
    kbp_mngr_key_segment_t * key_segments);

/**
 * See kbp_mngr_opcode_master_key_segments_add() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_master_key_segment_remove_internal(
    int unit,
    uint32 opcode_id,
    uint32 segment_index);

/**
 * See kbp_mngr_cache_commit() in kbp_mngr.h
 */
shr_error_e kbp_mngr_cache_commit_internal(
    int unit,
    int is_acl);

/**
 * See kbp_mngr_init() in kbp_mngr.h
 */
shr_error_e kbp_mngr_init_internal(
    int unit);

/**
 * See kbp_mngr_deinit() in kbp_mngr.h
 */
shr_error_e kbp_mngr_deinit_internal(
    int unit);

/**
 * See kbp_mngr_sync() in kbp_mngr.h
 */
shr_error_e kbp_mngr_sync_internal(
    int unit);

/**
 * See kbp_mngr_wb_init() in kbp_mngr.h
 */
shr_error_e kbp_mngr_wb_init_internal(
    int unit);

/**
* \brief
* This function disables the KBP lookups in case the soc properties of the KBP are
* disabled or kbp is not present.
* It is done be zeroing the aligner of the KBP key and the FFCs of the KBP KBRs.
* \param [in]  unit - The Unit number.
* \param [in]  context_index - The context to disable.
* \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
* \remark
*   None
* \see
*   shr_error_e
*/
shr_error_e kbp_mngr_lookups_disable(
    int unit,
    uint32 context_index);

/**
* \brief
* This function disables the pd order for FWD context in KBP in case of ACL only mode.
* when disabled it means that the FWD lookups will not process in the FLP
* \param [in]  unit - The Unit number.
* \param [in]  context_index - The context to disable.
* \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
* \remark
*   None
* \see
*   shr_error_e
*/
shr_error_e kbp_mngr_pd_order_disable(
    int unit,
    uint32 context_index);

/*
 *
 * }
 */
#endif /* _KBP_MNGR_INTERNAL_H_INCLUDED__ */

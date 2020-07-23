/*! \file spm_util.h
 *
 * Defines internal to SPM
 *
 * This file defines utility functions of the Strength Profile Manager CTH.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SPM_UTIL_H
#define SPM_UTIL_H

#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm_sbr_internal.h>
#include "spm_internal.h"

/*!
 * \brief Reset the SPM op info for the indicated unit.
 *
 * The SPM op info is the working space for assembling API field values
 * and PT entries with associated context.  For each op, this working
 * space should be set to a clean state.
 *
 * \param [in] unit Unit number.
 * \param [in] spm_lt_record Pointer to the SPM record structure.
 * \param [in] spm_op_info Working context for this op.
 *
 * \retval N/A
 */
extern void
bcmptm_spm_op_info_reset(int unit,
                         bcmptm_spm_lt_record_t *spm_lt_record,
                         bcmptm_spm_op_info_t *spm_op_info);

/*!
 * \brief Initialize a LTA field array in preparation for IMM read.
 *
 * This function prepares the space reserved to place a LTA output
 * list for IMM access via LTA.
 *
 * \param [in] unit Unit number.
 * \param [in] spm_lt_record Pointer to the SPM record structure.
 * \param [in] spm_op_info Working context for this op.
 *
 * \retval N/A
 */
extern void
bcmptm_spm_lta_output_init(int unit,
                           bcmptm_spm_lt_record_t *spm_lt_record,
                           bcmptm_spm_op_info_t *spm_op_info);

/*!
 * \brief Create cached LT key and value fields metadata from LRD data
 *
 * During LT ops, the key and value fields numbers and IDs must be
 * known.  This function initializes these records.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table identifier value.
 * \param [in] map LRD map for this LT.
 * \param [in] lt_str_ptr Logical table SBR mapping info pointer.
 * \param [out] lt_fields Pointer to return bcmptm_spm_lt_fields_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Data structure inconsistency.
 */
extern int
bcmptm_spm_lt_field_records_init(int unit,
                                 bcmltd_sid_t ltid,
                                 const bcmlrd_map_t *map,
                                 const bcmptm_sbr_lt_str_pt_info_t *lt_str_ptr,
                                 bcmptm_spm_lt_fields_t *lt_fields);

/*!
 * \brief Merge the API field lists into working list for this op.
 *
 * Combine the API input field list, current IMM record (if any), and
 * default values to create a single API value field list in
 * the SPM op info structure.
 *
 * NB:  All fields in this implementation are expected to be single index.
 *
 * \param [in] unit Unit number.
 * \param [in] spm_op_info Working context for this op.
 * \param [in] lt_fields Pointer to bcmptm_spm_lt_fields_t structure.
 * \param [in] api_fields Linked list of input API fields.
 * \param [in] lta_cur_fields LTA fields structure holding IMM record.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
extern int
bcmptm_spm_field_write_list_create(int unit,
                                   bcmptm_spm_op_info_t *spm_op_info,
                                   bcmptm_spm_lt_fields_t *lt_fields,
                                   const bcmltd_field_t *api_fields,
                                   const bcmltd_fields_t *lta_cur_fields);

/*!
 * \brief Analyze PT sizes for a specific LT.
 *
 * The device size of SBR tables must be determined by examining the
 * sizes of the individual SBR PTs and the offset for multiple
 * lookup entries within a single PT.
 *
 * \param [in] unit Unit number.
 * \param [in/out] lt_fields Pointer to return bcmptm_spm_lt_fields_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Data structure inconsistency.
 */
extern int
bcmptm_spm_pt_size_init(int unit,
                        bcmptm_spm_lt_fields_t *lt_fields);

/*!
 * \brief Update the SBR-RM assigned PT entries for dynamic SRB.
 *
 * For dynamic SBR LTs, the SBR-RM manages the PT index setting due to
 * possible index space fragmentation during tile mode change.  This
 * function retrieves the PT indexes for each (ltid, ptid) pair in this
 * LT.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] spm_op_info Working context for this op.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_fields Pointer to bcmptm_spm_lt_fields_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Data structure inconsistency.
 */
extern int
bcmptm_spm_pt_index_dynamic_update(int unit,
                                   bcmptm_spm_op_info_t *spm_op_info,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmptm_spm_lt_fields_t *lt_fields);

/*!
 * \brief Check a specific PT index for user controlled SBR addressing.
 *
 * For multi-entry SBR LTs, the index may be provided by application or
 * allocated within this CTH.  This function tests a specific index
 * to determine if it is available on all required device tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_fields Pointer to bcmptm_spm_lt_fields_t structure.
 * \param [in] test_lt_index PT index to test as valid.
 * \param [out] idx_free TRUE if test_lt_index may be used for all PT entries.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Did not find PT index valid for all maped PT entries.
 * \retval SHR_E_INTERNAL Data structure inconsistency.
 */
extern int
bcmptm_spm_user_lt_index_test(int unit,
                              bcmptm_spm_lt_fields_t *lt_fields,
                              uint16_t test_lt_index,
                              bool *idx_free);

/*!
 * \brief Retrieve the state of specific PT index for SP resource info LT.
 *
 * For multi-entry SBR LTs, a resource info LT is provided to permit
 * the application to examine the state of each entry. This function
 * searches the SBR records for a specific index to determine the
 * effective state of this LT index.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_record Pointer to the SPM record structure.
 * \param [in] lt_fields Pointer to bcmptm_spm_lt_fields_t structure.
 * \param [in] test_lt_index PT index to test as valid.
 * \param [out] idx_state The bcmptm_spm_entry_state_t status of this index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Did not find PT index valid for all maped PT entries.
 * \retval SHR_E_INTERNAL Data structure inconsistency.
 */
extern int
bcmptm_spm_user_lt_index_state(int unit,
                               bcmptm_spm_lt_record_t *lt_record,
                               bcmptm_spm_lt_fields_t *lt_fields,
                               uint16_t test_lt_index,
                               bcmptm_spm_entry_state_t *idx_state);

/*!
 * \brief Determine the entry limit of one SPM LT.
 *
 * The SBR-RM reserves some PT entries during initialization.  Any LT
 * index which maps to one of these entries will never be available
 * to the LT.  This function determines the number of available entries
 * and caches them for use by TABLE_INFO.ENTRY_LIMIT.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_fields Pointer to bcmptm_spm_lt_fields_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Did not find PT index valid for all maped PT entries.
 * \retval SHR_E_INTERNAL Data structure inconsistency.
 */
extern int
bcmptm_spm_user_lt_entry_count(int unit,
                               bcmptm_spm_lt_fields_t *lt_fields);

/*!
 * \brief Allocate an available LT index for user controlled SBR addressing.
 *
 * For user mode SBR LTs, the application may either provide a LT index,
 * or it may request a valid index be allocated.  This function searches
 * the SBR-RM state to find an availalbe LT index which is valid on all
 * mapped PTs, if such an index exists.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_fields Pointer to bcmptm_spm_lt_fields_t structure.
 * \param [out] lt_index Pointer to return a valid PT index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Did not find LT index valid for all maped PT entries.
 * \retval SHR_E_INTERNAL Data structure inconsistency.
 */
extern int
bcmptm_spm_user_lt_index_search(int unit,
                                bcmptm_spm_lt_fields_t *lt_fields,
                                uint16_t *lt_index);

/*!
 * \brief Search for the LT info structure index for the given LTID.
 *
 * Scan the device list of strenth profile LTs for the information
 * structure matching the provided LTID. Return a pointer to the
 * info structure index if found, -1 if not.
 *
 * \param [in] lt_record Pointer to the SPM record structure.
 * \param [in] ltid This is the logical table ID.
 *
 * \retval LTID infomation structure if LTID is matched.
 *         NULL if LTID not in records.
 */
extern bcmptm_spm_lt_fields_t *
bcmptm_spm_lt_fields_find(bcmptm_spm_lt_record_t *lt_record,
                          uint32_t ltid);

/*!
 * \brief Search for the LT resource info structure index for the given LTID.
 *
 * Scan the device list of strenth profile LTs for the information
 * structure matching the provided LTID. Return a pointer to the
 * info structure index if found, -1 if not.
 *
 * \param [in] resource_root Pointer to the SPM resource root structure.
 * \param [in] ltid This is the logical table ID.
 *
 * \retval LTID resource infomation structure if LTID is matched.
 *         NULL if LTID not in records.
 */
extern bcmptm_spm_lt_resource_info_t *
bcmptm_spm_lt_resource_find(bcmptm_spm_resource_root_t *resource_root,
                            uint32_t ltid);

/*!
 * \brief Update the SBR-RM assigned PT entries for user-mode SRB.
 *
 * For user assigned index SBR LTs, modify all the mapped PT entries at the
 * indicated index (plus any offset for each entry).
 *
 * This index should be either provided by application, allocated during
 * INSERT, or correspond to a previous INSERT key for UPDATE or DELETE.
 *
 * For each opcode INSERT, DELETE, or UPDATE, modify and/or verify the
 * current SBR-RM state for the entry for record-keeping and
 * consistency.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] spm_op_info SPM context for this unit.
 * \param [in] lt_fields Pointer to bcmptm_spm_lt_fields_t structure.
 * \param [in] lt_index The LT index for which to update PT entries.
 * \param [in] event This is the reason for the entry event.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Data structure inconsistency.
 */
extern int
bcmptm_spm_lt_index_user_update(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmptm_spm_op_info_t *spm_op_info,
                                bcmptm_spm_lt_fields_t *lt_fields,
                                uint16_t lt_index,
                                bcmimm_entry_event_t event);

#endif /* SPM_UTIL_H */

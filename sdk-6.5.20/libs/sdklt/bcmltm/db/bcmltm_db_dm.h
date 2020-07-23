/*! \file bcmltm_db_dm.h
 *
 * Logical Table Manager - Information for Direct Mapped Tables.
 *
 * This file contains routines to construct information for
 * direct map logical tables.
 *
 * This information is derived from the LRD Physical (direct map)
 * map groups.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_DM_H
#define BCMLTM_DB_DM_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>
#include "bcmltm_db_xfrm.h"
#include "bcmltm_db_dm_fs.h"
#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_dm_track.h"
#include "bcmltm_db_read_only_field.h"


/*!
 * Number of PT Keyed Op Information node cookies needed per Keyed LT.
 *
 * The PT Keyed Operations Information node cookie can be shared among
 * several PTM operations as follows:
 *     LOOKUP/TRAVERSE(GET_FIRST, GET_NEXT)
 *     INSERT/DELETE
 */
#define BCMLTM_DB_DM_PT_KEYED_NUM    2

/*! DB index for PT Keyed INSERT and DELETE ops information */
#define BCMLTM_DB_DM_PT_KEYED_INS_DEL_IDX     0

/*! DB index for PT Keyed LOOKUP and TRAVERSE ops information */
#define BCMLTM_DB_DM_PT_KEYED_LK_TRV_IDX      1


/*!
 * \brief Direct Mapped Tables Information.
 *
 * This structure contains information derived from the LRD
 * for direct map physical map groups.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 *
 * Node cookies that have the same information for several opcodes
 * can be shared among the corresponding operation node functions.
 */
typedef struct bcmltm_db_dm_info_s {
    /*! PTs information. */
    bcmltm_db_dm_pt_info_t *pt_info;

    /*!
     * Working buffer block ID for LT Private Internal Elements.
     */
    bcmltm_wb_block_id_t lt_pvt_wb_block_id;

    /*!
     * Indicates if PT suppress is available for LT.
     *
     * The PT suppress status is supplied by a key transform.
     */
    bool pt_suppress;

    /*!
     * Indicates if track index comes from the table map.
     *
     * The track index is supplied by either:
     * - A field transform map (track_index_map is TRUE)
     *   In this case, the transform provides the relationship
     *   between the source API keys and the track index.
     *   Note that the track index alone should be sufficient
     *   to obtain the source API keys.
     *   There is no track index field elements in this case.
     *
     * - LTM internal (track_index_map is FALSE)
     *   LTM composes the track index from a set of track index
     *   field elements.  PT key params, such as index, port, instance,
     *   are used for this case.
     */
    bool track_index_map;

    /*!
     * LT track index information.
     *
     * This is the FA node cookie for LTM index resource operation
     * on Index LTs.
     */
    bcmltm_track_index_t *track_index;

    /*!
     * LT index information.
     *
     * This is the EE node cookie for LTM index resource operation
     * on Index LTs.
     */
    bcmltm_ee_index_info_t *ee_lt_index;

    /*!
     * List of working buffer coordinates to copy,
     *   - from: Track index components
     *   - to  : PTM index and param sections.
     *
     * This is the FA node cookie used to propagate the tracking
     * index components into the PTM WB on Index LTs.
     * This is needed in operations where LTM determines the
     * tracking index, such TRAVERSE to get the next entry.
     */
    bcmltm_wb_copy_list_t *track_to_pt_copy;

    
    /*!
     * Track index information.
     *
     * This contains information used for managing the
     * track index on Index LTs.
     */
    bcmltm_db_dm_track_info_t *track_info;

    /*!
     * List of mapped physical tables that correspond to an Index LT.
     * This is the EE node cookie for PTM index operations.
     */
    bcmltm_pt_list_t *pt_list;

    /*!
     * List of mapped physical tables that correspond to a Keyed LT.
     * This is the EE node cookie for PTM keyed operations.
     */
    bcmltm_pt_keyed_op_info_t *pt_keyed_list[BCMLTM_DB_DM_PT_KEYED_NUM];

    /*!
     * List of direct physical mapped key fields on input direction.
     *
     * This is the FA node cookie used for mapping LT fields to
     * corresponding PT working buffer locations.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_field_select_mapping_t *in_key_fields;

    /*!
     * List of direct physical mapped key fields on output direction.
     *
     * This is the FA node cookie used for mapping PT working buffer
     * locations to corresponding LT fields.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     *
     * Note that for Index LTs, the field mapping input and output
     * information are the same; however, these are different on Keyed LTs.
     */
    bcmltm_field_select_mapping_t *out_key_fields;

    /*!
     * List of direct physical mapped key fields that are required
     * during an INSERT operation on Index with Allocation LTs.
     *
     * This is the FA node cookie used for mapping LT fields to
     * corresponding PT working buffer locations on an INSERT operation.
     *
     * This is applicable for Index with Allocation LTs on INSERT.
     */
    bcmltm_field_select_mapping_t *req_key_fields;

    /*!
     * List of direct physical mapped key fields that are allocatable
     * (optional) during an INSERT operation on Index with Allocation LTs.
     *
     * This is the FA node cookie used for mapping LT fields to
     * corresponding PT working buffer locations on an INSERT operation.
     * If optional keys are not present, they are allocated during
     * the operation.
     *
     * This is applicable for Index with Allocation LTs on INSERT.
     */
    bcmltm_field_select_mapping_t *alloc_key_fields;

    /*!
     * List of direct physical mapped value fields.
     *
     * This is the FA node cookie used for mapping LT fields to/from
     * corresponding PT working buffer locations.
     *
     * This is applicable for any directly mapped LT, Index or Keyed,
     * for any direction (input or output).
     */
    bcmltm_field_select_mapping_t *value_fields;

    /*!
     * List of direct fixed mapped key fields.
     *
     * This is the FA node cookie used for copying fixed key fields into
     * corresponding PT working buffer locations.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_fixed_field_list_t *fixed_key_fields;

    /*!
     * List of direct fixed mapped value fields.
     *
     * This is the FA node cookie used for copying fixed value fields into
     * corresponding PT working buffer locations.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_fixed_field_list_t *fixed_value_fields;

    /*!
     * List of direct-mapped read-only value fields.
     *
     * This is the FA node cookie used for rejecting read-only value fields
     * in the API input field list.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_ro_field_list_t *read_only_fields;

    /*!
     * List of direct-mapped wide value fields.
     *
     * This is the FA node cookie used for requiring wide value fields'
     * coherency (all elements present or none) in the API input field list.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_wide_field_list_t *wide_fields;

    /*!
     * Field Transforms Information.
     *
     * This contains all the information related to field transforms.
     */
    bcmltm_db_xfrm_info_t *xfrm;

    /*!
     * Field Selections Information.
     *
     * This contains all the information related to field selections.
     */
    bcmltm_db_dm_fs_info_t *fs;

    /*!
     * API Cache field mappings information for all value fields.
     *
     * This metadata is used by the field select drivers to
     * set the defaults for all value fields in the API Cache
     * during a DELETE or UPDATE operation.
     *
     * The list contains all value fields, regardless of whether
     * a field is selected or not.
     */
    bcmltm_field_select_mapping_t *apic_all_value_fields;

} bcmltm_db_dm_info_t;


/*!
 * \brief Create information for Direct Map Table.
 *
 * Create information for Direct Maps for the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] info_ptr Returning pointer to info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_info_create(int unit, bcmlrd_sid_t sid,
                         const bcmltm_table_attr_t *attr,
                         bcmltm_db_dm_info_t **info_ptr);

/*!
 * \brief Destroy information for Direct Map Table.
 *
 * Destroy given for Direct Map Table information.
 *
 * \param [in] info Pointer to info to destroy.
 */
extern void
bcmltm_db_dm_info_destroy(bcmltm_db_dm_info_t *info);


#endif /* BCMLTM_DB_DM_H */

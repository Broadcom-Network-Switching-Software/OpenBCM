/*! \file bcmltm_db_dm_xfrm.h
 *
 * Logical Table Manager - Information for Field Transforms.
 *
 * This file contains routines to construct information for
 * field transforms with selection in direct mapped logical tables.
 *
 * The metadata created is used for the following LTM driver flow:
 * (field select opcode driver version)
 *     API <-> API Cache <-> [VALIDATE, XFRM] <-> PTM
 *
 * This driver flow can be applied to tables with or without field selections.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_DM_XFRM_H
#define BCMLTM_DB_DM_XFRM_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>

#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_field_select.h"

/*!
 * \brief Field Transform Information.
 *
 * This structure contains information derived from the LRD
 * for a field transform.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_dm_xfrm_s {
    /*! Map entry identifier for this field transform. */
    bcmltm_db_map_entry_t map_entry;

    /*! LTA working buffer block ID for this field transform. */
    bcmltm_wb_block_id_t lta_wb_block_id;

    /*!
     * Indicates if transform contains allocatable keys.
     * Valid only for key transforms on Index with Allocation LTs.
     */
    bool is_alloc_keys;

    /*!
     * Field transform parameters used as the LTA FA node cookie.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_transform_params_t *params;

    /*!
     * Field transform input field list used as the LTA FA node cookie.
     * Forward Transform: API to LTA (src fields).
     * Reverse Transform: PTM to LTA (dst fields).
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_select_field_list_t *in_fields;

    /*!
     * Field transform key input field list used as the LTA FA node cookie.
     * This is used on forward and reverse value transforms.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_select_field_list_t *in_key_fields;

    /*!
     * Field transform output field list used as the LTA FA node cookie.
     * Forward Transform: LTA to PTM (dst fields).
     * Reverse Transform: LTA to API (src fields).
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_select_field_list_t *out_fields;

} bcmltm_db_dm_xfrm_t;

/*!
 * \brief Field Transform List.
 *
 * This structure contains information for a list of field transforms.
 */
typedef struct bcmltm_db_dm_xfrm_list_s {
    /*! Number of field transforms. */
    uint32_t num_xfrms;

    /*! Array of field transform information. */
    bcmltm_db_dm_xfrm_t **xfrms;

} bcmltm_db_dm_xfrm_list_t;

/*!
 * \brief Field Transforms Information.
 *
 * This structure contains information for the fields transforms
 * for a given field selection group on a logical table.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_dm_xfrm_info_s {
    /*!
     * List of Forward Key Field Transforms.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_db_dm_xfrm_list_t *fwd_keys;

    /*!
     * List of Forward Value Field Transforms.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_db_dm_xfrm_list_t *fwd_values;

    /*!
     * List of Reverse Key Field Transforms.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_db_dm_xfrm_list_t *rev_keys;

    /*!
     * List of Reverse Value Field Transforms.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_db_dm_xfrm_list_t *rev_values;

} bcmltm_db_dm_xfrm_info_t;

/*!
 * \brief Create field transform information for a selection group.
 *
 * This routine creates the field transforms information for the given
 * selection group in a logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] selection Field selection group.
 * \param [out] info_ptr Returning pointer to field transform information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_xfrm_info_create(int unit,
                              bcmlrd_sid_t sid,
                              const bcmltm_db_dm_arg_t *dm_arg,
                              const bcmltm_db_field_selection_t *selection,
                              bcmltm_db_dm_xfrm_info_t **info_ptr);

/*!
 * \brief Destroy field transform information.
 *
 * This routine destroys the given field transform information.
 *
 * \param [in] info Field transform information to destroy.
 */
extern void
bcmltm_db_dm_xfrm_info_destroy(bcmltm_db_dm_xfrm_info_t *info);

/*!
 * \brief Create transform reference list for given map entry list.
 *
 * This routine creates a list of transforms references for all
 * the map entries that match the specified transform type.
 * The input transform list is searched and a reference is made to it
 * rather than creating again the transform metadata.
 *
 * When destroying the create list, caller must call the corresponding
 * destroy routine to free the appropriate memory.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] map_list Map entry list to create transform list for.
 * \param [in] in_list Input transform list to search and reference.
 * \param [in] xfrm_type Specifies transform type to create metadata for.
 * \param [out] list_ptr Returning pointer to transform reference list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_xfrm_list_ref_create(int unit,
                                  bcmlrd_sid_t sid,
                                  const bcmltm_db_map_entry_list_t *map_list,
                                  const bcmltm_db_dm_xfrm_list_t *in_list,
                                  bcmltm_field_xfrm_type_t xfrm_type,
                                  bcmltm_db_dm_xfrm_list_t **list_ptr);

/*!
 * \brief Destroy transform reference list.
 *
 * This routine destroys the given field transform reference list.
 *
 * \param [in] list Transform reference list to destroy.
 */
extern void
bcmltm_db_dm_xfrm_list_ref_destroy(bcmltm_db_dm_xfrm_list_t *list);

/*!
 * \brief Find transform for given map entry identifier.
 *
 * This routine finds and returns the transform that matches
 * the map entry identifier in the given transform list.
 *
 * \param [in] list Transform list to search.
 * \param [in] map_entry Map entry identifier to find.
 *
 * \retval Pointer to matching field transform, if found.
 * \retval NULL, if not found.
 */
extern bcmltm_db_dm_xfrm_t *
bcmltm_db_dm_xfrm_find(const bcmltm_db_dm_xfrm_list_t *list,
                       const bcmltm_db_map_entry_t *map_entry);

#endif /* BCMLTM_DB_DM_XFRM_H */

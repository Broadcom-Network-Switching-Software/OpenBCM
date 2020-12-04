/*! \file bcmltm_db_xfrm.h
 *
 * Logical Table Manager - Information for Field Transforms.
 *
 * This file contains routines to construct information for
 * field transforms in logical tables.
 *
 * This information is derived from the LRD Field Transform map entries
 * in the Physical (direct map) map groups.
 *
 * The metadata created is used for the following LTM driver flow:
 * (non field select opcode driver version)
 *     API <-> [VALIDATE, XFRM] <-> PTM
 *
 * This driver flow can be applied only to tables without field selections.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_XFRM_H
#define BCMLTM_DB_XFRM_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>

#include "bcmltm_db_dm_util.h"

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
typedef struct bcmltm_db_xfrm_s {
    /*! Working buffer block ID for this LTA Field Transform. */
    bcmltm_wb_block_id_t wb_block_id;

    /*!
     * Field transform table parameters used as the LTA FA node cookie.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_transform_params_t *xfrm_params;

    /*!
     * Field transform input field list used as the LTA FA node cookie.
     * Forward Transform: API to LTA (src fields).
     * Reverse Transform: PTM to LTA (dst fields).
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_field_list_t *fin;

    /*!
     * Field transform key input field list used as the LTA FA node cookie.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_field_list_t *fin_key;

    /*!
     * Field transform output field list used as the LTA FA node cookie.
     * Forward Transform: LTA to PTM (dst fields).
     * Reverse Transform: LTA to API (src fields).
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_field_list_t *fout;

} bcmltm_db_xfrm_t;


/*!
 * \brief Field Transform List.
 *
 * This structure contains information for a list of field transforms.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_xfrm_list_s {
    /*! Number of field transforms. */
    uint32_t num_xfrms;

    /*! Array of field transform information. */
    bcmltm_db_xfrm_t *xfrms;

} bcmltm_db_xfrm_list_t;


/*!
 * \brief Field Transforms Information.
 *
 * This structure contains information for all fields transforms in
 * a logical table. The information is derived from the
 * LRD field transform maps.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_xfrm_info_s {
    /*!
     * List of Forward Key Field Transforms.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_db_xfrm_list_t *fwd_key_xfrm_list;

    /*!
     * List of Forward Value Field Transforms.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_db_xfrm_list_t *fwd_value_xfrm_list;

    /*!
     * List of Reverse Key Field Transforms.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_db_xfrm_list_t *rev_key_xfrm_list;

    /*!
     * List of Reverse Value Field Transforms.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_db_xfrm_list_t *rev_value_xfrm_list;

    /*!
     * List of LTA Transform to copy,
     * - from: Reverse key transform output
     * - to  : Forward key transform input
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_db_xfrm_list_t *rkey_to_fkey_xfrm_list;

    /*!
     * List of LTA Transform to copy,
     * - from: Reverse value transform output
     * - to  : Forward value transform input
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_db_xfrm_list_t *rvalue_to_fvalue_xfrm_list;

    /*!
     * List of LTA Transform to copy,
     * - from: Reverse Key transform output
     * - to  : Reverse Value transform input key
     *
     * This is applicable for extended transform with transform_key_src.
     * The transform_key_src value in value transform should be copied
     * from the output of key reverse transform in TRAVERSE operation.
 */
    bcmltm_db_xfrm_list_t *rkey_to_rvalue_xfrm_list;
} bcmltm_db_xfrm_info_t;


/*!
 * \brief Create information for Field Transforms.
 *
 * Create information for list of Field Transforms for the specified
 * logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] map_entries Map information.
 * \param [in] pt_info PT information.
 * \param [in] info_ptr Returning pointer to info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_xfrm_info_create(int unit, bcmlrd_sid_t sid,
                           const bcmltm_table_attr_t *attr,
                           bcmltm_db_dm_map_entries_t *map_entries,
                           const bcmltm_db_dm_pt_info_t *pt_info,
                           bcmltm_db_xfrm_info_t **info_ptr);

/*!
 * \brief Destroy information for Field Transforms.
 *
 * Destroy given for Field Transforms information.
 *
 * \param [in] info Pointer to info to destroy.
 */
extern void
bcmltm_db_xfrm_info_destroy(bcmltm_db_xfrm_info_t *info);

#endif /* BCMLTM_DB_XFRM_H */

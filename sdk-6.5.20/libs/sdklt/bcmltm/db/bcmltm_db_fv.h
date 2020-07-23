/*! \file bcmltm_db_fv.h
 *
 * Logical Table Manager - Information for Field Validations.
 *
 * This file contains routines to construct information for
 * field validations in logical tables.
 *
 * The metadata created is used for the following LTM driver flow:
 * (non field select opcode driver version)
 *     API <-> [VALIDATE, XFRM] <-> PTM
 *
 * This driver flow can be applied only to tables without field selections.
 *
 * This information is derived from the LRD Field Validation map groups.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_FV_H
#define BCMLTM_DB_FV_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>

#include "bcmltm_db_xfrm.h"

/*!
 * \brief Field Validation Information.
 *
 * This structure contains information derived from the LRD
 * for a field validation map entry.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_fv_s {
    /*! Working buffer block ID for this LTA Field Validation. */
    bcmltm_wb_block_id_t wb_block_id;

    /*!
     * LTA Field Validation parameters used as the LTA FA node cookie.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_validate_params_t *validate_params;

    /*!
     * LTA src field list used as the LTA FA node cookie.
     * (LTM to/from LTA).
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_field_list_t *fsrc;

} bcmltm_db_fv_t;

/*!
 * \brief Field Validation List.
 *
 * This structure contains information derived from the LRD
 * for field validations.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_fv_list_s {
    /*! Number of field validations. */
    uint32_t num_fvs;

    /*! Array of field validations information. */
    bcmltm_db_fv_t *fvs;

} bcmltm_db_fv_list_t;

/*!
 * \brief Field Validations Information.
 *
 * This structure contains information derived from the LRD
 * for Field Validation map groups.
 *
 * The information is referenced by the LTM metadata and
 * is used for the FA and EE node cookies.
 *
 * This information can be shared among several BCMLTM_MD operations
 * for a given symbol.
 */
typedef struct bcmltm_db_fv_info_s {
    /*!
     * List of key field validations.
     */
    bcmltm_db_fv_list_t *validate_keys;

    /*!
     * List of value field validations.
     */
    bcmltm_db_fv_list_t *validate_values;

    /*!
     * List of copy value field validations,
     * - from: Reverse value transform output
     * - to  : Value field validation input
     */
    bcmltm_db_fv_list_t *rvalue_to_value;

} bcmltm_db_fv_info_t;


/*!
 * \brief Create information for Field Validations.
 *
 * Create information for Field Validation interfaces for the specified
 * logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] info_ptr Returning pointer to info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_fv_info_create(int unit, bcmlrd_sid_t sid,
                         bcmltm_db_fv_info_t **info_ptr);

/*!
 * \brief Create LTA-LTA copy info for Field Validate calls for given table.
 *
 * Create the LTA-LTA copy information needed by the
 * LTM metadata for LTA Field Validate functions.
 * These are the reverse transform output to validate input
 * LTA field copy nodes used by the UPDATE opcode.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] from_list Node cookie list of reverse transforms from
 *                       which to copy field values in output list.
 * \param [in] to_list Node cookie list of validate functions to
 *                     which to copy field values into input list.
 * \param [out] list_ptr Returning pointer to LT info for validate copy.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_fv_info_copy_list_create(int unit, bcmlrd_sid_t sid,
                                   bcmltm_db_xfrm_list_t *from_list,
                                   bcmltm_db_fv_list_t *to_list,
                                   bcmltm_db_fv_list_t **list_ptr);

/*!
 * \brief Destroy information for Field Validations.
 *
 * Destroy given Field Validations information.
 *
 * \param [in] info Pointer to info to destroy.
 */
extern void
bcmltm_db_fv_info_destroy(bcmltm_db_fv_info_t *info);

#endif /* BCMLTM_DB_FV_H */

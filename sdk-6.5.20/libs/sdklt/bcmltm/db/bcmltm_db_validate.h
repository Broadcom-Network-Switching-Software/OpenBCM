/*! \file bcmltm_db_validate.h
 *
 * Logical Table Manager - Information for Field Validations.
 *
 * This file contains interfaces to construct information for
 * field validations in logical tables.
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

#ifndef BCMLTM_DB_VALIDATE_H
#define BCMLTM_DB_VALIDATE_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>

#include "bcmltm_db_field_select.h"

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
typedef struct bcmltm_db_validate_s {
    /*! LTA working buffer block ID for this field validation. */
    bcmltm_wb_block_id_t wb_block_id;

    /*!
     * Field validation parameters used as the LTA FA node cookie.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_validate_params_t *params;

    /*!
     * Field validation input field list used as the LTA FA node cookie:
     * API to LTA (src fields).
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_select_field_list_t *in_fields;

} bcmltm_db_validate_t;

/*!
 * \brief Field Validation List.
 *
 * This structure contains information for a list of field validations.
 */
typedef struct bcmltm_db_validate_list_s {
    /*! Number of field validations. */
    uint32_t num_validations;

    /*! Array of field validations information. */
    bcmltm_db_validate_t *validations;

} bcmltm_db_validate_list_t;

/*!
 * \brief Field Validations Information.
 *
 * This structure contains information for the field validations
 * for a given field selection group on a logical table.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_validate_info_s {
    /*! List of key field validations. */
    bcmltm_db_validate_list_t *keys;

    /*! List of value field validations. */
    bcmltm_db_validate_list_t *values;

} bcmltm_db_validate_info_t;


/*!
 * \brief Create field validation information for a selection group.
 *
 * This routine creates the field validations information for the given
 * selection group in a logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] selection Field selection group.
 * \param [out] info_ptr Returning pointer to field validation information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_validate_info_create(int unit,
                                bcmlrd_sid_t sid,
                                const bcmltm_db_field_selection_t *selection,
                                bcmltm_db_validate_info_t **info_ptr);

/*!
 * \brief Destroy field validation information.
 *
 * This routine destroys the given field validation information.
 *
 * \param [in] info Field validation information to destroy.
 */
extern void
bcmltm_db_validate_info_destroy(bcmltm_db_validate_info_t *info);

#endif /* BCMLTM_DB_VALIDATE_H */

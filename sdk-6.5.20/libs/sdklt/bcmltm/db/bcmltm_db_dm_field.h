/*! \file bcmltm_db_dm_field.h
 *
 * Logical Table Manager - Information for Direct Field Maps.
 *
 * This file contains interfaces to construct information for
 * logical fields with selections that are directly mapped to
 * physical fields in direct mapped logical tables.
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

#ifndef BCMLTM_DB_DM_FIELD_H
#define BCMLTM_DB_DM_FIELD_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_md_internal.h>

#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_field_select.h"


/*!
 * \brief Direct Field Map Specifiers.
 *
 * This structure contains information to indicate the desired
 * type of direct field maps to construct information for.
 * It is used as an input argument to routines to pass various
 * data and help reduce the number of input function arguments.
 */
typedef struct {
    /*! Indicates key or value field. */
    bcmltm_field_type_t field_type;

    /*!
     * Indicates input or output field direction.
     * Relevant only for Keyed LTs on Key fields.
     */
    bcmltm_field_dir_t field_dir;

    /*!
     * Indicates what keys to include in returning information.
     * Relevant only for key fields on IwA LTs.
     */
    bcmltm_field_key_type_t key_type;
} bcmltm_db_dm_field_map_spec_t;

/*!
 * \brief Direct Field Maps Information.
 *
 * This structure contains information for the direct map fields
 * for a given field selection group on a direct mapped logical table.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_dm_field_map_info_s {

    /*!
     * List of direct physical mapped key fields on input direction.
     *
     * This is the FA node cookie used for mapping LT fields to
     * corresponding PT working buffer locations.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_field_select_mapping_t *in_keys;

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
    bcmltm_field_select_mapping_t *out_keys;

    /*!
     * List of direct physical mapped key fields that are required
     * during an INSERT operation on Index with Allocation LTs.
     *
     * This is the FA node cookie used for mapping LT fields to
     * corresponding PT working buffer locations on an INSERT operation.
     *
     * This is applicable for Index with Allocation LTs on INSERT.
     */
    bcmltm_field_select_mapping_t *req_keys;

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
    bcmltm_field_select_mapping_t *alloc_keys;

    /*!
     * List of direct physical mapped value fields.
     *
     * This is the FA node cookie used for mapping LT fields to/from
     * corresponding PT working buffer locations.
     *
     * This is applicable for any directly mapped LT, Index or Keyed,
     * for any direction (input or output).
     */
    bcmltm_field_select_mapping_t *values;

} bcmltm_db_dm_field_map_info_t;


/*!
 * \brief Set field map specifiers struct.
 *
 * Utility routine to populate the field map specifiers struct.
 *
 * \param [in,out] fspec Field map specifier struct to set.
 * \param [in] field_type Indicates key or value field.
 * \param [in] field_dir Indicates input or output field direction,
 *                       (relevant only for key fields on Keyed LTs).
 * \param [in] key_type Desired key type (for IwA).
 */
static inline void
bcmltm_db_dm_field_map_spec_set(bcmltm_db_dm_field_map_spec_t *fspec,
                                bcmltm_field_type_t field_type,
                                bcmltm_field_dir_t field_dir,
                                bcmltm_field_key_type_t key_type)
{
    fspec->field_type = field_type;
    fspec->field_dir = field_dir;
    fspec->key_type = key_type;
}


/*!
 * \brief Create the field mapping metadata.
 *
 * Create the field mapping metadata for given field selection group.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] selection Field selection group.
 * \param [in] fspec Specifies type of fields to create metadata for.
 * \param [out] field_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_field_mapping_create(int unit,
                                  bcmlrd_sid_t sid,
                                  const bcmltm_db_dm_arg_t *dm_arg,
                                  const bcmltm_db_field_selection_t *selection,
                                  const bcmltm_db_dm_field_map_spec_t *fspec,
                            bcmltm_field_select_mapping_t **field_mapping_ptr);

/*!
 * \brief Create direct field map information for a selection group.
 *
 * This routine creates the direct field maps information for the given
 * selection group in logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] selection Field selection group.
 * \param [out] info_ptr Returning pointer to direct field map information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_field_map_info_create(int unit,
                                   bcmlrd_sid_t sid,
                                   const bcmltm_db_dm_arg_t *dm_arg,
                                   const bcmltm_db_field_selection_t *selection,
                                   bcmltm_db_dm_field_map_info_t **info_ptr);

/*!
 * \brief Destroy direct field map information.
 *
 * This routine destroys the given direct field map information.
 *
 * \param [in] info Direct field map information to destroy.
 */
extern void
bcmltm_db_dm_field_map_info_destroy(bcmltm_db_dm_field_map_info_t *info);

#endif /* BCMLTM_DB_DM_FIELD_H */

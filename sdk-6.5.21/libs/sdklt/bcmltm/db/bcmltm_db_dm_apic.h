/*! \file bcmltm_db_dm_apic.h
 *
 * Logical Table Manager - Information for API Cache Fields.
 *
 * This file contains interfaces to construct information for
 * copying logical fields between the API input/output field list
 * and the API Cache.
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

#ifndef BCMLTM_DB_DM_APIC_H
#define BCMLTM_DB_DM_APIC_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_md_internal.h>

#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_field_select.h"
#include "bcmltm_db_dm_field.h"
#include "bcmltm_db_dm_xfrm.h"

/*!
 * \brief API Cache Arguments.
 *
 * This structure contains context information needed to
 * construct the API cache field lists metadata for a
 * selection group.
 *
 * Information for the API cache field list is gathered
 * from the logical fields of:
 *   - Direct field maps
 *   - Field transforms (source fields)
 */
typedef struct bcmltm_db_dm_apic_arg_s {
    /*! DM arguments reference. */
    const bcmltm_db_dm_arg_t *dm_arg;

    /*! Direct field mappings information for a selection group. */
    const bcmltm_db_dm_field_map_info_t *direct_field_maps;

    /*! Field transforms information for a selection group. */
    const bcmltm_db_dm_xfrm_info_t *xfrms;
} bcmltm_db_dm_apic_arg_t;

/*!
 * \brief API Cache Fields Information.
 *
 * This structure contains information for the API Cache fields
 * for a given field selection group on a direct mapped logical table.
 *
 * Each of the field lists is a union of all the logical fields
 * that are part of:
 *   - Direct field maps
 *   - Field transforms (source fields)
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_dm_apic_field_info_s {
    /*!
     * List of logical key fields.
     *
     * This is the FA node cookie used for copying logical fields
     * between the API input/output field list and the API Cache.
     *
     * This is applicable for any directly mapped LT, Index or Keyed,
     * for any direction (input or output).
     */
    bcmltm_field_select_mapping_t *keys;

    /*!
     * List of logical key fields that are required
     * during an INSERT operation on Index with Allocation LTs.
     *
     * This is the FA node cookie used for copying logical fields
     * between the API input/output field list and the API Cache.
     *
     * This is applicable for Index with Allocation LTs on INSERT.
     */
    bcmltm_field_select_mapping_t *req_keys;

    /*!
     * List of logical key fields that are allocatable
     * (optional) during an INSERT operation on Index with Allocation LTs.
     *
     * This is the FA node cookie used for copying logical fields
     * between the API input/output field list and the API Cache.
     *
     * If optional keys are not present, they are allocated during
     * the operation.
     *
     * This is applicable for Index with Allocation LTs on INSERT.
     */
    bcmltm_field_select_mapping_t *alloc_keys;

    /*!
     * List of logical value fields.
     *
     * This is the FA node cookie used for copying logical fields
     * between the API input/output field list and the API Cache.
     *
     * NOTE:
     * Value fields can be read-only (key fields are always read-write).
     * As such the actual input field list for values may be different from
     * the output field list (input does not contain read-only fields).
     *
     * However, since the rule is that the upper layer component
     * should check that there are no read-only fields on input,
     * we can use the same value list (which contains all fields, including
     * read-only fields) on both input and output directions.
     *
     * On input, when processing the read-only fields (which should be
     * absent), the default will be applied to the API cache and will
     * later be overridden with the proper value on the output direction.
     */
    bcmltm_field_select_mapping_t *values;

} bcmltm_db_dm_apic_field_info_t;

/*!
 * \brief API Cache Fields Information List.
 *
 * This structure contains a list of API Cache fields information
 * for a given table.
 */
typedef struct bcmltm_db_dm_apic_field_info_list_s {
    /*! Number of API cache field information in array. */
    uint32_t num_apics;

    /*! Array of API cache field information. */
    bcmltm_db_dm_apic_field_info_t **apics;

} bcmltm_db_dm_apic_field_info_list_t;

/*!
 * \brief Create API cache field information for a selection group.
 *
 * This routine creates the API cache field information for the given
 * selection group in logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] apic_arg API cache arguments.
 * \param [in] selection Field selection group.
 * \param [out] info_ptr Returning pointer to API cache field information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_apic_field_info_create(int unit,
                              bcmlrd_sid_t sid,
                              const bcmltm_db_dm_apic_arg_t *apic_arg,
                              const bcmltm_db_field_selection_t *selection,
                              bcmltm_db_dm_apic_field_info_t **info_ptr);

/*!
 * \brief Destroy API Cache field information.
 *
 * This routine destroys the given API Cache field information.
 *
 * \param [in] info API Cache field information to destroy.
 */
extern void
bcmltm_db_dm_apic_field_info_destroy(bcmltm_db_dm_apic_field_info_t *info);

/*!
 * \brief Create the field mapping metadata for all value fields.
 *
 * This routine creates the API cache field mapping metadata for all
 * the value fields in a given table using the provided list of
 * API cache fields information (created per field selection group).
 *
 * The created list contains all value fields, regardless of whether
 * a field is selected or not, and is not subject to any field selection,
 * i.e. it is unconditional.
 *
 * The primary use of this metadata is to set the defaults for all
 * value fields in the API cache during a DELETE or UPDATE operation.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] apic_info_list List of API cache fields information.
 * \param [out] field_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmltm_db_dm_apic_all_value_field_mapping_create(int unit,
                                                 bcmlrd_sid_t sid,
                   const bcmltm_db_dm_apic_field_info_list_t *apic_info_list,
                   bcmltm_field_select_mapping_t **field_mapping_ptr);

#endif /* BCMLTM_DB_DM_APIC_H */

/*! \file bcmltm_db_dm_fs.h
 *
 * Logical Table Manager - Field Selection for Direct Mapped Tables.
 *
 * This file contains routines to construct information for
 * field selections in direct map logical tables.
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

#ifndef BCMLTM_DB_DM_FS_H
#define BCMLTM_DB_DM_FS_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_md_internal.h>

#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_dm_field.h"
#include "bcmltm_db_dm_fixed_field.h"
#include "bcmltm_db_dm_xfrm.h"
#include "bcmltm_db_validate.h"
#include "bcmltm_db_dm_apic.h"

/*!
 * \brief Field selection information for direct mapped tables.
 *
 * This structure contains information for a field selection
 * group.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_dm_fs_s {
    /*!
     * Field Selection ID.
     * This ID uniquely identifies a field selection group
     * within a logical table.
     */
    bcmltm_field_selection_id_t selection_id;

    /*!
     * Field selector type.
     *
     * Indicates if the field selector is a key or value
     * (or none for the unconditional selection group).
     */
    bcmltm_field_selector_type_t selector_type;

    /*!
     * Field selection level
     *   Level 0: no field selector (unconditional).
     *   Level 1: has 0 parents.
     *   ...
     *   Level n: has (n-1) parents.
     */
    uint32_t level;

    /*!
     * Field selection information.
     *
     * This is the FA node cookie used to determine the
     * corresponding selection map.
     *
     * This is not applicable for the special unconditional
     * field selection group (maps without selector).  In this case,
     * this is NULL.
     */
    bcmltm_field_select_maps_t *fsm;

    /*!
     * Direct field mappings information.
     *
     * This contains the FA node cookies used to process
     * the direct field maps and convert between the
     * API logical fields and the physical fields.
     */
    bcmltm_db_dm_field_map_info_t *field_maps;

    /*!
     * Fixed field mappings information.
     *
     * This contains the FA node cookie used for copying
     * fixed fields into corresponding PT working buffer locations.
     */
    bcmltm_db_dm_fixed_field_info_t *fixed_fields;

    /*!
     * Field transforms information.
     *
     * This contains the metadata used to process the field transforms.
     */
    bcmltm_db_dm_xfrm_info_t *xfrms;

    /*!
     * Field validations information.
     *
     * This contains the metadata used to process the field validations.
     */
    bcmltm_db_validate_info_t *field_validations;

    /*!
     * API Cache field mappings information.
     *
     * This contains the FA node cookies used to copy
     * logical fields between the API input/output field list
     * and the API Cache.
     */
    bcmltm_db_dm_apic_field_info_t *apic_fields;

} bcmltm_db_dm_fs_t;


/*!
 * \brief Field selections information for direct mapped tables.
 *
 * This structure contains information for all the field selection
 * groups for a direct mapped logical table.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 *
 * NOTE:
 * If the list contains at least one selection group with selector
 * (num_with_selectors > 0), LTM must to use the field select opcode
 * driver version to filter the fields accordingly.
 *
 * If the list contains only the unconditional selection group
 * (table has not selectors), LTM can choose to use the
 * field select opcode driver version which works for both,
 * LTs with field selectors and LTs without field selectors.
 */
typedef struct bcmltm_db_dm_fs_info_s {
    /*!
     * Number of field selection levels for the logical table.
     *
     * A level may have more than 1 selection group.
     */
    uint32_t num_levels;

    /*!
     * Number of selections in array that have field selectors.
     *
     * This number is to differentiate from 'num_selections',
     * which includes both, the unconditional selection
     * group (no field selector) and selection groups with field selectors.
     */
    uint32_t num_with_selectors;

    /*!
     * Number of field selections in array.
     *
     * This is the total number of selections in the array
     * which may include the unconditional group (no field selector).
     */
    uint32_t num_selections;

    /*! Array of field selections. */
    bcmltm_db_dm_fs_t *selections;

} bcmltm_db_dm_fs_info_t;


/*!
 * \brief Create information for Field Selections.
 *
 * This routine creates the Field Selections information for
 * the specified direct mapped logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [out] info_ptr Returning pointer to info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_fs_info_create(int unit, bcmlrd_sid_t sid,
                            const bcmltm_db_dm_arg_t *dm_arg,
                            bcmltm_db_dm_fs_info_t **info_ptr);

/*!
 * \brief Destroy information for Field Selections.
 *
 * This routine destroys the given Field Selections information.
 *
 * \param [in] info Pointer to info to destroy.
 */
extern void
bcmltm_db_dm_fs_info_destroy(bcmltm_db_dm_fs_info_t *info);

/*!
 * \brief Find field selection group for given selection ID.
 *
 * This routine finds and returns the field selection group
 * that matches the selection ID.
 *
 * \param [in] info Field selection information to search.
 * \param [in] selection_id Selection ID to find.
 *
 * \retval Pointer to matching field selection group, if found.
 * \retval NULL, if not found.
 */
extern const bcmltm_db_dm_fs_t *
bcmltm_db_dm_fs_find(const bcmltm_db_dm_fs_info_t *info,
                     bcmltm_field_selection_id_t selection_id);

#endif /* BCMLTM_DB_DM_FS_H */

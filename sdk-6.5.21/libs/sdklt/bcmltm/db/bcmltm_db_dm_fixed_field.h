/*! \file bcmltm_db_dm_fixed_field.h
 *
 * Logical Table Manager - Information for Fixed Field Maps.
 *
 * This file contains interfaces to construct information for
 * fixed field maps with selections in direct mapped logical tables.
 *
 * Fixed field maps are used to copy specific values into
 * specified destination locations (PTM working buffer).
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

#ifndef BCMLTM_DB_DM_FIXED_FIELD_H
#define BCMLTM_DB_DM_FIXED_FIELD_H

#include <shr/shr_types.h>
#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_md_internal.h>

#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_field_select.h"

/*!
 * \brief Fixed Fields Information.
 *
 * This structure contains information for the fixed field maps
 * for a given field selection group on a direct mapped logical table.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_dm_fixed_field_info_s {
    /*!
     * Fixed key field mappings information.
     *
     * This contains the FA node cookie used for copying
     * fixed keys into corresponding PT working buffer locations.
     */
    bcmltm_fixed_field_mapping_t *keys;

    /*!
     * Fixed value field mappings information.
     *
     * This contains the FA node cookie used for copying
     * fixed values into corresponding PT working buffer locations.
     */
    bcmltm_fixed_field_mapping_t *values;

} bcmltm_db_dm_fixed_field_info_t;

/*!
 * \brief Create fixed field map information for a selection group.
 *
 * This routine creates the fixed field maps information for the given
 * selection group in logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] selection Field selection group.
 * \param [out] info_ptr Returning pointer to fixed field map information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_fixed_field_info_create(int unit,
                              bcmlrd_sid_t sid,
                              const bcmltm_db_dm_arg_t *dm_arg,
                              const bcmltm_db_field_selection_t *selection,
                              bcmltm_db_dm_fixed_field_info_t **info_ptr);

/*!
 * \brief Destroy fixed field map information.
 *
 * This routine destroys the given fixed field map information.
 *
 * \param [in] info Fixed field map information to destroy.
 */
extern void
bcmltm_db_dm_fixed_field_info_destroy(bcmltm_db_dm_fixed_field_info_t *info);

#endif /* BCMLTM_DB_DM_FIXED_FIELD_H */

/*! \file bcmltm_db_dm_track.h
 *
 * Logical Table Manager - Information for Track Index.
 *
 * This file contains routines to construct information for
 * track index in direct map index logical tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_DM_TRACK_H
#define BCMLTM_DB_DM_TRACK_H

#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>

#include "bcmltm_db_map.h"
#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_dm_xfrm.h"

/*!
 * \brief Track Index Information.
 *
 * This structure contains information used for managing the
 * track index on Index LTs.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_dm_track_info_s {
    

    /*!
     * List of direct field maps to convert the chosen
     * track index elements back to API logical fields.
     *
     * This is needed in operations where LTM determines the
     * tracking index, such as the next entry during a TRAVERSE.
     */
    bcmltm_field_select_mapping_t *track_to_api;

    /*!
     * List of reverse field transforms to convert the chosen
     * track index elements in a transform back to API logical fields.
     *
     * This is needed in operations where LTM determines the
     * tracking index, such as the next entry during a TRAVERSE.
     */
    bcmltm_db_dm_xfrm_list_t *track_rev_xfrms;

} bcmltm_db_dm_track_info_t;

/*!
 * \brief Create the Track Index metadata.
 *
 * Create the Track Index metadata for the given table.
 *
 * This routine populates the required metadata information.
 *
 * LTs with no index or memory parameter will still reference
 * a valid WB offset location.  In this case, the content of the
 * location will always be zero (working buffer is always reset
 * to zero during its allocation for each operation).
 * In addition, the corresponding 'min' and 'max' metadata for
 * 'lt_index' and 'mem_param' are also set to zero.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] pt_list PT list.
 * \param [out] index_ptr Returning pointer FA Track Index metadata.
 * \param [out] map_list_ptr List of map entries chosen for track elements.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_track_index_create(int unit,
                                bcmlrd_sid_t sid,
                                const bcmltm_db_dm_arg_t *dm_arg,
                                const bcmltm_pt_list_t *pt_list,
                                bcmltm_track_index_t **index_ptr,
                                bcmltm_db_map_entry_list_t **map_list_ptr);

/*!
 * \brief Destroy the Track Index metadata.
 *
 * Destroy the given Track Index metadata.
 *
 * \param [in] index Track Index metadata.
 */
extern void
bcmltm_db_dm_track_index_destroy(bcmltm_track_index_t *index);

/*!
 * \brief Create track index information.
 *
 * This routine creates the track index information for the given
 * logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] map_list List of map entries chosen for track elements.
 * \param [in] rev_key_xfrms List of reverse key transforms.
 * \param [out] info_ptr Returning pointer to track index information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_track_info_create(int unit,
                               bcmlrd_sid_t sid,
                               const bcmltm_db_dm_arg_t *dm_arg,
                               const bcmltm_db_map_entry_list_t *map_list,
                               const bcmltm_db_dm_xfrm_list_t *rev_key_xfrms,
                               bcmltm_db_dm_track_info_t **info_ptr);

/*!
 * \brief Destroy track index information.
 *
 * This routine destroys the given track index information.
 *
 * \param [in] info Track index information to destroy.
 */
extern void
bcmltm_db_dm_track_info_destroy(bcmltm_db_dm_track_info_t *info);

#endif /* BCMLTM_DB_DM_TRACK_H */

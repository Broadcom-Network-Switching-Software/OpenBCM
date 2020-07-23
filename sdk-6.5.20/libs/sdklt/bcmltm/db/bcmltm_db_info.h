/*! \file bcmltm_db_info.h
 *
 * Logical Table Manager - Logical Table Database Information.
 *
 * This file contains routines to create the internal database for
 * logical tables and provide access to the information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_INFO_H
#define BCMLTM_DB_INFO_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>

#include "bcmltm_db_dm.h"
#include "bcmltm_db_xfrm.h"
#include "bcmltm_db_cth.h"
#include "bcmltm_db_ltm.h"
#include "bcmltm_db_fv.h"
#include "bcmltm_db_tc.h"

/*!
 * \brief Get pointer to table attribute information.
 *
 * \param [in] _info Logical table information pointer.
 */
#define DB_ATTR_INFO(_info)          (&(_info)->attr)

/*!
 * \brief Get pointer to Direct Mapped information.
 *
 * \param [in] _info Logical table information pointer.
 */
#define DB_DM_INFO(_info)           ((_info)->dm)

/*!
 * \brief Get pointer to Field Validation information.
 *
 * \param [in] _info Logical table information pointer.
 */
#define DB_FV_INFO(_info)           ((_info)->fv)

/*!
 * \brief Get pointer to Table Commit information.
 *
 * \param [in] _info Logical table information pointer.
 */
#define DB_TC_INFO(_info)           ((_info)->tc)

/*!
 * \brief Get pointer to Custom Table Handler information.
 *
 * \param [in] _info Logical table information pointer.
 */
#define DB_CTH_INFO(_info)          ((_info)->cth)

/*!
 * \brief Get pointer to LTM managed table information.
 *
 * \param [in] _info Logical table information pointer.
 */
#define DB_LTM_INFO(_info)          ((_info)->ltm)


/*!
 * \brief Information for a Logical Table.
 *
 * This structure contains information for a logical table
 * derived from LRD.
 *
 * It provides various data used during LTM metadata construction,
 * such as FA and EE node cookies referenced by the LTM opcode trees,
 * table and field attributes, etc.
 */
typedef struct bcmltm_db_lt_info_s {
    /*!
     * Table attributes.
     */
    bcmltm_table_attr_t attr;

    /*!
     * Map groups count.
     */
    bcmltm_db_map_group_count_t map_group_count;

    /*!
     * Information for logical tables with direct physical table mappings.
     */
    bcmltm_db_dm_info_t *dm;

    /*!
     * Information for Field Validations.
     */
    bcmltm_db_fv_info_t *fv;

    /*!
     * Information for Table Commit interfaces.
     */
    bcmltm_db_tc_info_t *tc;

    /*!
     * Information for Custom Table Handlers.
     */
    bcmltm_db_cth_info_t *cth;

    /*!
     * Information for logical tables with LTM group.
     */
    bcmltm_db_ltm_info_t *ltm;

} bcmltm_db_lt_info_t;


/*!
 * \brief Create logical table database.
 *
 * This routine creates the database for logical tables.  This database
 * contains information for logical tables used for metadata construction.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Maximum number of logical table IDs on unit.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_info_create(int unit, uint32_t sid_max_count);

/*!
 * \brief Destroy logical table database.
 *
 * This routine frees any memory allocated during the creation of
 * the internal database for logical tables.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmltm_db_info_destroy(int unit);

/*!
 * \brief Get the maximum number of logical table IDs.
 *
 * This routine gets the maximum number of logical table IDs
 * for the specified unit.
 *
 * \param [in] unit Unit number.
 * \param [out] sid_max_count Maximum number of logical table IDs on unit.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_info_sid_max_count_get(int unit, uint32_t *sid_max_count);

/*!
 * \brief Get the database information for the given logical table.
 *
 * This routine gets the database information for the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] info_ptr Returning pointer to logical table information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_lt_info_get(int unit, bcmlrd_sid_t sid,
                      const bcmltm_db_lt_info_t **info_ptr);

/*!
 * \brief Get direct map database information for the given logical table.
 *
 * This routine gets the direct map table database information
 * for the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] info_ptr Returning pointer to direct map table information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_info_get(int unit,
                      bcmlrd_sid_t sid,
                      const bcmltm_db_dm_info_t **info_ptr);

/*!
 * \brief Get track index information for the given logical table.
 *
 * This routine gets the track index information for the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] info_ptr Returning pointer to track index information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_track_info_get(int unit,
                            bcmlrd_sid_t sid,
                            const bcmltm_db_dm_track_info_t **info_ptr);

/*!
 * \brief Get field select information for the given logical table.
 *
 * This routine gets the field select information for the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] info_ptr Returning pointer to field select information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_fs_info_get(int unit,
                         bcmlrd_sid_t sid,
                         const bcmltm_db_dm_fs_info_t **info_ptr);

/*!
 * \brief Display database information for given logical table info struct.
 *
 * This routine displays the content for given logical table
 * database information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] info Information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
extern void
bcmltm_db_lt_info_dump(int unit, bcmlrd_sid_t sid,
                       const bcmltm_db_lt_info_t *info,
                       shr_pb_t *pb);

#endif /* BCMLTM_DB_INFO_H */

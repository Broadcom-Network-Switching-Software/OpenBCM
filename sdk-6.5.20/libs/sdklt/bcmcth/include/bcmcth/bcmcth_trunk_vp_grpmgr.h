/*! \file bcmcth_trunk_vp_grpmgr.h
 *
 * This file contains TRUNK_VP group management function declarations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TRUNK_VP_GRPMGR_H
#define BCMCTH_TRUNK_VP_GRPMGR_H
#include <shr/shr_types.h>
#include <bcmcth/bcmcth_trunk_vp_db_internal.h>

/*!
 * \brief Add an TRUNK_VP Group logical table entry to hardware tables.
 *
 * Reserves hardware table resources and installs TRUNK_VP logical table entry
 * fields to corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
extern int
bcmcth_trunk_vp_group_add(int unit,
                  bcmcth_trunk_vp_lt_entry_t *lt_entry);
/*!
 * \brief Update installed TRUNK_VP logical table entry field values.
 *
 * Update an existing TRUNK_VP logical table entry
 * field values. Multiple fields can
 * be updated in a single update call.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 * \return SHR_E_UNAVAIL Feature not supported yet.
 */
extern int
bcmcth_trunk_vp_group_update(int unit,
                     bcmcth_trunk_vp_lt_entry_t *lt_entry);

/*!
 * \brief Delete an TRUNK_VP Group logical table entry from hardware tables.
 *
 * Delete an TRUNK_VP Group logical table entry
 * from hardware tables. TRUNK_VP member
 * table resources consumed by the group are returned to the free pool as part
 * of this delete operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 */
extern int
bcmcth_trunk_vp_group_delete(int unit,
                     bcmcth_trunk_vp_lt_entry_t *lt_entry);

/*!
 * \brief Get an TRUNK_VP Group logical table entry data from hardware tables.
 *
 * Get an TRUNK_VP Group logical table entry data from hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 */
extern int
bcmcth_trunk_vp_group_get(int unit,
                  bcmcth_trunk_vp_lt_entry_t *lt_entry);

/*!
 * \brief Traverse specified TRUNK_VP logical table and find inserted entries.
 *
 * This function traverses specified TRUNK_VP
 * logical table, finds and returns the
 * first valid entry when Get-First operation is performed. For Get-Next
 * operation, it starts searching for next valid entry
 * after the given TRUNK_VP_ID.
 * When an entry is found, it returns entry's data field values.
 *
 * When no valid entries are found for Get-First API call this function returns
 * SHR_E_EMPTY error code. For Get-Next API call when no entries are found
 * after the specified entry, it returns SHR_E_NOT_FOUND error code.
 *
 * Get-First vs Get-Next API call is indicated via the 'first' boolean input
 * parameter variable of this function.
 *
 * \param [in] unit Unit number.
 * \param [in] first Indicates get first logical table operation.
 * \param [out] lt_entry Pointer to TRUNK_VP logical table entry.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_EMPTY No entries in the logical table.
 * \return SHR_E_NOT_FOUND No entries found in the logical table.
 */
extern int
bcmcth_trunk_vp_group_find(int unit,
                   bool first,
                   bcmcth_trunk_vp_lt_entry_t *lt_entry);

/*!
 * \brief Free memory allocated to store group next-hop members.
 *
 * Free memory allocated to store group next-hop members.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmcth_trunk_vp_groups_cleanup(int unit);

/*!
 * \brief Update installed TRUNK_VP logical table entry field values.
 *
 * Update an existing TRUNK_VP logical table
 * entry field values. Multiple fields can
 * be updated in a single update call.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 * \return SHR_E_UNAVAIL Feature not supported yet.
 */
extern int
bcmcth_trunk_vp_group_update_entry_set(int unit,
                                       bcmcth_trunk_vp_lt_entry_t *lt_entry);

#endif /* BCMCTH_TRUNK_VP_GRPMGR_H */

/*! \file bcmecmp_group.h
 *
 * This file contains ECMP group management function declarations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_GROUP_H
#define BCMECMP_GROUP_H
#include <shr/shr_types.h>
#include <bcmecmp/bcmecmp_db_internal.h>
#include <bcmecmp/bcmecmp_flex_db_internal.h>

/*!
 * \brief Initalize bcmecmp_tbl_op_t structure.
 *
 * \param [in] op_info Pointer to bcmecmp_tbl_op_t_init structure.
 *
 * \return Nothing.
 */
extern void
bcmecmp_tbl_op_t_init(bcmecmp_tbl_op_t *op_info);

/*!
 * \brief Add an ECMP Group logical table entry to hardware tables.
 *
 * Reserves hardware table resources and installs ECMP logical table entry
 * fields to corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
extern int
bcmecmp_group_add(int unit,
                  bcmecmp_lt_entry_t *lt_entry);
/*!
 * \brief Update installed ECMP logical table entry field values.
 *
 * Update an existing ECMP logical table entry field values. Multiple fields can
 * be updated in a single update call.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 * \return SHR_E_UNAVAIL Feature not supported yet.
 */
extern int
bcmecmp_group_update(int unit,
                     bcmecmp_lt_entry_t *lt_entry);

/*!
 * \brief Delete an ECMP Group logical table entry from hardware tables.
 *
 * Delete an ECMP Group logical table entry from hardware tables. ECMP member
 * table resources consumed by the group are returned to the free pool as part
 * of this delete operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 */
extern int
bcmecmp_group_delete(int unit,
                     bcmecmp_lt_entry_t *lt_entry);

/*!
 * \brief Get an ECMP Group logical table entry data from hardware tables.
 *
 * Get an ECMP Group logical table entry data from hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 */
extern int
bcmecmp_group_get(int unit,
                  bcmecmp_lt_entry_t *lt_entry);

/*!
 * \brief Traverse specified ECMP logical table and find inserted entries.
 *
 * This function traverses specified ECMP logical table, finds and returns the
 * first valid entry when Get-First operation is performed. For Get-Next
 * operation, it starts searching for next valid entry after the given ECMP_ID.
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
 * \param [out] lt_entry Pointer to ECMP logical table entry.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_EMPTY No entries in the logical table.
 * \return SHR_E_NOT_FOUND No entries found in the logical table.
 */
extern int
bcmecmp_group_find(int unit,
                   bool first,
                   bcmecmp_lt_entry_t *lt_entry);

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
bcmecmp_groups_cleanup(int unit);

/*!
 * \brief RH-ECMP groups pre-config function for warm start sequence.
 *
 * For all RH-ECMP groups that have non-zero NUM_PATHS value set during insert
 * operation in cold boot mode, their load balanced members state has to be
 * reconstructed for use during update operation post warm boot. This function
 * identifies such RH-ECMP groups during warm start pre_config sequence.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmecmp_rh_groups_preconfig(int unit);

/*!
 * \brief RH-ECMP groups pre-config function for warm start sequence.
 *
 * For all RH-ECMP groups that have non-zero NUM_PATHS value set during insert
 * operation in cold boot mode, their load balanced members state has to be
 * reconstructed for use during update operation post warm boot. This function
 * identifies such RH-ECMP groups during warm start pre_config sequence.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmecmp_flex_rh_groups_preconfig(int unit);

/*!
 * \brief Sorts group next-hop array elements.
 *
 * Sorts list of next-hop members belonging to the group.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
extern int
bcmecmp_group_nhops_sort(int unit, bcmecmp_lt_entry_t *lt_entry);

/*!
 * \brief Update installed ECMP logical table entry field values.
 *
 * Update an existing ECMP logical table entry field values. Multiple fields can
 * be updated in a single update call.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 * \return SHR_E_UNAVAIL Feature not supported yet.
 */
extern int
bcmecmp_group_update_entry_set(int unit,
                                       bcmecmp_lt_entry_t *lt_entry);

/*!
 * \brief Group list pre-config function for warm start sequence.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmecmp_grp_list_preconfig(int unit);

/*!
 * \brief reallocates itbm blocks during warm boot for ECMP groups.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmecmp_wb_itbm_group_preconfig(int unit);

/*!
 * \brief Set the limits for ECMP groups.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Oper arguments.
 * \param [in] lt_id LT sid.
 * \param [in] ecmp_limit Ptr of ECMP limit.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_BUSY Groups are busy error.
 */
extern int
bcmecmp_limit_set(int unit,
                  const bcmltd_op_arg_t *op_arg,
                  bcmltd_sid_t lt_id,
                  bcmecmp_limit_t *ecmp_limit);

/*!
 * \brief Get the limits  for ECMP groups.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Oper arguments.
 * \param [in] lt_id LT sid.
 * \param [in/out] ecmp_limit Ptr of ECMP limit.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmecmp_limit_get(int unit,
                  const bcmltd_op_arg_t *op_arg,
                  bcmltd_sid_t lt_id,
                  bcmecmp_limit_t *ecmp_limit);

/*!
 * \brief Add an ECMP flex Group logical table entry to hardware tables.
 *
 * Reserves hardware table resources and installs ECMP logical table entry
 * fields to corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
extern int
bcmecmp_flex_group_add(int unit,
                  bcmecmp_flex_lt_entry_t *lt_entry);
/*!
 * \brief Update installed ECMP flex logical table entry field values.
 *
 * Update an existing ECMP logical table entry field values. Multiple fields can
 * be updated in a single update call.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 * \return SHR_E_UNAVAIL Feature not supported yet.
 */
extern int
bcmecmp_flex_group_update(int unit,
                     bcmecmp_flex_lt_entry_t *lt_entry);

/*!
 * \brief Delete an ECMP flex Group logical table entry from hardware tables.
 *
 * Delete an ECMP Group logical table entry from hardware tables. ECMP member
 * table resources consumed by the group are returned to the free pool as part
 * of this delete operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 */
extern int
bcmecmp_flex_group_delete(int unit,
                     bcmecmp_flex_lt_entry_t *lt_entry);

#endif /* BCMECMP_GROUP_H */

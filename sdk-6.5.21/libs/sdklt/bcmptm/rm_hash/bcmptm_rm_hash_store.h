/*! \file bcmptm_rm_hash_store.h
 *
 * RM hash store header file
 *
 * This file defines data structures and function for hash store.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_HASH_STORE_H
#define BCMPTM_RM_HASH_STORE_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>



/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Initialize the HA block for hash store.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot flag.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_store_init(int unit, bool warm);

/*!
 * \brief Clean up for hash store.
 *
 * \param [in] unit Unit number.
 *
 * \retval None.
 */
extern void
bcmptm_rm_hash_store_cleanup(int unit);

/*!
 * \brief Check the new entry is in collision with current entry.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID of the new entry.
 * \param [in] hash_store Hash store value of the new entry.
 * \param [in] full_key Full key of the new entry.
 * \param [out] collision TRUE indicats collision happens.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_collision_check(int unit,
                               bcmltd_sid_t ltid,
                               uint32_t hash_store,
                               uint32_t *full_key,
                               bool *collision);

/*!
 * \brief Check if the searched full_key is the same with hash store entry's
 * full key.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID of the new entry.
 * \param [in] hash_store Hash store value of the entry.
 * \param [in] full_key Full key for the searched operation.
 * \param [out] same TRUE indicates the entry has the same key with
 *              the searched full_key.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_store_full_key_same(int unit,
                                   bcmltd_sid_t ltid,
                                   uint32_t hash_store,
                                   uint32_t *full_key,
                                   bool *same);

/*!
 * \brief Get the full key related to the hash store value.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID of the new entry.
 * \param [in] hash_store Hash store value of the new entry.
 * \param [out] full_key Returned full key pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_store_full_key_get(int unit,
                                  bcmltd_sid_t ltid,
                                  uint32_t hash_store,
                                  uint32_t **full_key);

/*!
 * \brief Search a hash store value in sorted array.
 * If returned found is TRUE, found_idx indicates the found entry.
 * If returned found is FALSE, ins_idx indicates the position to be inserted.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] search_val Value to be searched.
 * \param [out] found TRUE if it is found, otherwise FALSE.
 * \param [out] found_idx If found is TRUE, found_idx indicates the found entry.
 * \param [out] ins_idx If found is FALSE, ins_idx indicates the position
 *              to be inserted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_store_bin_search(int unit,
                                bcmltd_sid_t ltid,
                                uint32_t search_val,
                                bool *found,
                                uint32_t *found_idx,
                                uint32_t *ins_idx);

/*!
 * \brief Insert a TABLE_HASH_STORE_INFO entry.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID of the entry.
 * \param [in] hash_store Hash store value of the entry.
 * \param [in] full_key Full key of the new entry.
 * \param [in] ins_idx The index to insert.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_store_entry_insert(int unit,
                                  bcmltd_sid_t ltid,
                                  uint32_t hash_store,
                                  uint32_t *full_key,
                                  uint32_t ins_idx);

/*!
 * \brief Delete a TABLE_HASH_STORE_INFO entry.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID of the entry.
 * \param [in] del_idx The index of entry to delete.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_store_entry_delete(int unit,
                                  bcmltd_sid_t ltid,
                                  uint32_t del_idx);

#endif /* BCMPTM_RM_HASH_STORE_H */

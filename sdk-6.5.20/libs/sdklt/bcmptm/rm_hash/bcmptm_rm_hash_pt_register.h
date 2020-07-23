/*! \file bcmptm_rm_hash_pt_register.h
 *
 * Utils, defines internal to RM Hash state
 *
 * This file defines data structures of hash resource manager, and declares
 * functions that operate on these data structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_HASH_PT_REGISTER_H
#define BCMPTM_RM_HASH_PT_REGISTER_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>
#include "bcmptm_rm_hash_robust_hash.h"

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Allocate a resource node for a ltid on per SID basis.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_pt_reg_info_add(int unit, bcmltd_sid_t ltid);

/*!
 * \brief Initialize hash vector information.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Flag indicating if it is warmboot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_pt_info_vec_init(int unit, bool warm);

/*!
 * \brief Update robust hash information in pt info.
 *
 * \param [in] unit Unit number.
 * \param [in] rhash_ctrl Pointer to rm_hash_rhash_ctrl_t structure.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank_list Bank list.
 * \param [in] num_rbanks Bank number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_robust_hash_info_update(int unit,
                                       rm_hash_rhash_ctrl rhash_ctrl,
                                       rm_hash_pt_info_t *pt_info,
                                       uint8_t *rbank_list,
                                       uint8_t num_rbanks);

/*!
 * \brief Get the list of all the banks registered via a specific SID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table SID.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [out] rbank_list Pointer to the buffer to hold the bank list.
 * \param [out] rbank_cnt Pointer to hold the number of banks.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
void
bcmptm_rm_hash_sid_rbank_list_get(int unit,
                                  bcmdrd_sid_t sid,
                                  rm_hash_pt_info_t *pt_info,
                                  uint8_t *rbank_list,
                                  uint8_t *rbank_cnt);

/*!
 * \brief Get the resource node for the given SID.
 *
 * \param [in] unit Unit number.
 * \param [out] pt_info Pointer to rm_hash_pt_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_pt_info_get(int unit,
                           rm_hash_pt_info_t **pt_info);

/*!
 * \brief Allocate global HA block control structure for a device.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Flag indicating if it is warmboot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_blk_ctrl_alloc(int unit, bool warm);

/*!
 * \brief Cleanup the rm hash pt info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_pt_reg_info_cleanup(int unit);

/*!
 * \brief Initialize bank working mode.
 *
 * \param [in] unit Unit number.
 * \param [in] num_rbanks Number of banks.
 * \param [in] rbank_list Bank list.
 * \param [in] pipe_unique If bank is per pipe.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_pt_info_bank_mode_init(int unit,
                                      uint8_t num_rbanks,
                                      uint8_t *rbank_list,
                                      bool pipe_unique);

/*!
 * \brief Dump resource node information for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_pt_reg_info_dump(int unit);

/*!
 * \brief Update key attribute information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table SID.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] key_format Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [out] e_nm Buffer to store the entry narrow mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_pt_info_key_attrib_update(int unit,
                                         bcmdrd_sid_t sid,
                                         rm_hash_pt_info_t *pt_info,
                                         const bcmptm_rm_hash_key_format_t *key_format,
                                         rm_hash_entry_narrow_mode_t *e_nm);

/*!
 * \brief Configure pt information
 *
 * \param [in] unit Unit number.

 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_pt_info_slb_info_update(int unit, rm_hash_pt_info_t *pt_info);

/*!
 * \brief Initialize event handler for rm hash.
 *
 * \param [in] unit Unit number.

 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_event_handle_init(int unit);

/*!
 * \brief SID based event handler for rm hash.
 *
 * \param [in] unit Unit number.

 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_sid_based_event_handler(int unit);
/*!
 * \brief Get if hash table bank has separated SID.
 *
 * \param [in] unit Unit number.
 * \param [out] separate Buffer to hold the result.

 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_bank_has_separated_sid(int unit, bool *separate);

/*!
 * \brief Configure the per unit hash control object.
 *
 * \param [in] unit Unit number.
 *
 * \retval NONE.
 */
extern int
bcmptm_rm_hash_pt_info_pre_config(int unit);

/*!
 * \brief Update bank narrow mode enabling state in pt info.
 *
 * \param [in] unit Unit number.
 * \param [out] pt_info Pointer to rm_hash_pt_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_narrow_mode_state_update(int unit, rm_hash_pt_info_t *pt_info);

#endif /* BCMPTM_RM_HASH_PT_REGISTER_H */

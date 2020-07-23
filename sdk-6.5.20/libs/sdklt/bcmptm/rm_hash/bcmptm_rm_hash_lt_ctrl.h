/*! \file bcmptm_rm_hash_lt_ctrl.h
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

#ifndef BCMPTM_RM_HASH_LT_CTRL_H
#define BCMPTM_RM_HASH_LT_CTRL_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>


/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Get lt_ctrl for a specific hash table ltid.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table id.
 * \param [out] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_get(int unit,
                           bcmltd_sid_t ltid,
                           rm_hash_lt_ctrl_t **lt_ctrl);

/*!
 * \brief Instantiate an object of rm_hash_lt_ctrl_t for a ltid.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_add(int unit, bcmltd_sid_t ltid);

/*!
 * \brief Initialize robust hash information for the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_rbust_info_init(int unit);


/*!
 * \brief Configure robust hash information for the device.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Flag indicating if it is warmboot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_rbust_hash_config(int unit, bool warm);

/*!
 * \brief Initialize move depth information for the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_move_depth_init(int unit);

/*!
 * \brief Initialize LT working mode pipe unique or not.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_pipe_mode_init(int unit);

/*!
 * \brief Init resource manager lt control for hash tables in a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_init(int unit);

/*!
 * \brief Clean up resource manager lt control for hash tables in a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_cleanup(int unit);

/*!
 * \brief Initialize the bucket mode.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_bkt_mode_init(int unit);

/*!
 * \brief Dump the detailed information for all the physical hash tables.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid logical table id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_dump(int unit, bcmltd_sid_t ltid);

/*!
 * \brief Dump the detailed information for all the physical hash tables.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid logical table id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_dev_lt_ctrl_dump(int unit);

/*!
 * \brief LTID based event handler for rm hash.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ltid_based_event_handler(int unit);

/*!
 * \brief Event handler for move depth change.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_move_depth_change(int unit);

/*!
 * \brief Bank enable status init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_bank_enable_init(int unit);

/*!
 * \brief Bank attribute update for a hash LT.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_hash_vec_attr_update(int unit);

/*!
 * \brief Get hit context information for a given LT and SID.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] sid Physical table sid.
 * \param [in] hit_context Hit context for the LT and SID.
 * \param [in] context_exist Indicating if context exists for this LT and SID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_hit_context_get(int unit,
                                       rm_hash_lt_ctrl_t *lt_ctrl,
                                       bcmdrd_sid_t sid,
                                       bcmptm_rm_hash_hit_context_t *hit_context,
                                       bool *context_exist);

/*!
 * \brief Configure hit context for the hash LT.
 *
 * \param [in] unit Unit number.
 * \param [in] report_enable Whether flexctr has interest on this LT.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_hit_context_configure(int unit,
                                     bool report_enable,
                                     rm_hash_lt_ctrl_t *lt_ctrl);
#endif /* BCMPTM_RM_HASH_LT_CTRL_H */

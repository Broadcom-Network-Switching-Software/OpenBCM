/*! \file bcmltm_table_op_pt_info.h
 *
 * Interface to update in-memory table with PT status.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_TABLE_OP_PT_INFO_H
#define BCMLTM_TABLE_OP_PT_INFO_H

#include <bcmptm/bcmptm.h>
#include <bcmltm/bcmltm_types.h>
#include <bcmbd/bcmbd.h>

/*!
 * \brief TABLE_OP_PT_INFO logical table with PT status for keyed LT.
 *
 * This function updates the TABLE_OP_PT_INFO table with
 * PT status of keyed LT operation.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logical Table enum for keyed operation.
 * \param [in] lt_trans_id Logical Table transaction id.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t.
 * \param [in] mrsp_info Response information of last operation.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_pt_status_mreq_keyed_lt(int unit, bcmltd_sid_t ltid,
                               uint32_t lt_trans_id,
                               bcmbd_pt_dyn_info_t *pt_dyn_info,
                               bcmptm_keyed_lt_mrsp_info_t *mrsp_info);

/*!
 * \brief TABLE_OP_PT_INFO logical table with PT status for indexed LT.
 *
 * This function updates the TABLE_OP_PT_INFO table with
 * PT status of indexed LT operation.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logical Table enum for indexed operation.
 * \param [in] lt_trans_id Logical Table transaction id.
 * \param [in] ptid Physical table ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_pt_status_mreq_indexed_lt(int unit, bcmltd_sid_t ltid,
                                 uint32_t lt_trans_id,
                                 uint32_t ptid,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info);

/*!
 * \brief TABLE_OP_PT_INFO logical table with PT status for interactive hash path.
 *
 * This function updates the TABLE_OP_PT_INFO table with
 * PT status of HASH LT operation in interactive path.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logical Table enum for current operation.
 * \param [in] lt_trans_id Logical Table transaction id.
 * \param [in] ptid Physical table ID.
 * \param [in] entry_index Index in the physical table.
 * \param [in] pt_dyn_hash_info Pointer to bcmbd_pt_dyn_hash_info_t.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_pt_status_ireq_hash_lt(int unit, bcmltd_sid_t ltid, uint32_t lt_trans_id,
                              uint32_t ptid, uint32_t entry_index,
                              bcmbd_pt_dyn_hash_info_t *pt_dyn_hash_info);

/*!
 * \brief TABLE_OP_PT_INFO logical table with PT status for interactive path.
 *
 * This function updates the TABLE_OP_PT_INFO table with
 * PT status of indexed LT operation in interactive path.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logical Table enum for current operation.
 * \param [in] lt_trans_id Logical Table transaction id.
 * \param [in] ptid Physical table ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_pt_status_ireq_op(int unit, bcmltd_sid_t ltid, uint32_t lt_trans_id,
                         uint32_t ptid,
                         bcmbd_pt_dyn_info_t *pt_dyn_info);

#endif /* BCMLTM_TABLE_OP_PT_INFO_H */

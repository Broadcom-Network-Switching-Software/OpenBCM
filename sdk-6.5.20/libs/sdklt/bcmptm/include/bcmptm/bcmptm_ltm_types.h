/*! \file bcmptm_ltm_types.h
 *
 * APIs, defines for LTM callbacks within PTM interfaces
 *
 * This file contains APIs, defines for LTM callbacks within PTM interfaces
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_LTM_TYPES_H
#define BCMPTM_LTM_TYPES_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_types.h>


/*******************************************************************************
 * Defines
 */

/* LTM callbacks for LT statistics and PT status tracking */

/*!
 * \brief Callback for LTM function to increment LT_STATS field by LTID
 *
.* \param [in] unit Unit number.
 * \param [in] ltid Logical Table ID.
 * \param [in] stat_field FID indicating which stat is to increment.
 */
typedef void
(*bcmptm_lt_stats_increment_cb)(int unit,
                                uint32_t ltid,
                                uint32_t stat_field);

/* LTM callbacks for PT status recording. */

/*!
 * \brief Callback for LTM function to record PT status for modeled
 *        keyed operations.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logical Table enum for current operation.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t.
 * \param [in] mrsp_info Response information of last operation.
 */
typedef int
(*bcmptm_pt_status_mreq_keyed_lt_cb)(int unit,
                                     bcmltd_sid_t ltid,
                                     uint32_t lt_trans_id,
                                     bcmbd_pt_dyn_info_t *pt_dyn_info,
                                     bcmptm_keyed_lt_mrsp_info_t *mrsp_info);

/*!
 * \brief Callback for LTM function to record PT status for modeled
 *        index operations.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logical Table enum for current operation.
 * \param [in] ptid Physical table ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t.
 */
typedef int
(*bcmptm_pt_status_mreq_indexed_lt_cb)(int unit,
                                       bcmltd_sid_t ltid,
                                       uint32_t lt_trans_id,
                                       uint32_t ptid,
                                       bcmbd_pt_dyn_info_t *pt_dyn_info);

/*!
 * \brief Callback for LTM function to record PT status for interactive
 *        hash operations.
 *
 * Note that only hash LOOKUP ops are currently supported by the LTM.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logical Table enum for current operation.
 * \param [in] ptid Physical table ID.
 * \param [in] entry_index Index in the physical table.
 * \param [in] pt_dyn_hash_info Pointer to bcmbd_pt_dyn_hash_info_t.
 */
typedef int
(*bcmptm_pt_status_ireq_hash_lt_cb)(int unit,
                                    bcmltd_sid_t ltid,
                                    uint32_t lt_trans_id,
                                    uint32_t ptid,
                                    uint32_t entry_index,
                             bcmbd_pt_dyn_hash_info_t *pt_dyn_hash_info);

/*!
 * \brief Callback for LTM function to record PT status for interactive
 *        index read/write operations.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logical Table enum for current operation.
 * \param [in] ptid Physical table ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t.
 */
typedef int
(*bcmptm_pt_status_ireq_op_cb)(int unit,
                               bcmltd_sid_t ltid,
                               uint32_t lt_trans_id,
                               uint32_t ptid,
                               bcmbd_pt_dyn_info_t *pt_dyn_info);

/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Invoke LTM statistics callback.
 *
 * CTH implementations of Logical Tables require a mechanism to update
 * the PT ops related statistic values.  This callback permits the PTM
 * implementation to update these statistics.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] stat_field LTM statistics counter to update.
 */
extern void
bcmptm_lt_stat_increment(int unit, uint32_t ltid, uint32_t stat_field);

#endif /* BCMPTM_LTM_TYPES_H */

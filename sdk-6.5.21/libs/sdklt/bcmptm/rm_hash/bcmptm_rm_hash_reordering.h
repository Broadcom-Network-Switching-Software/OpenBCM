/*! \file bcmptm_rm_hash_reordering.h
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

#ifndef BCMPTM_RM_HASH_REORDERING_H
#define BCMPTM_RM_HASH_REORDERING_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Perform entry move.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt requested operation context information.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] depth Reordering depth.
 * \param [in] num_slb_info Number of SLB info.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] ent_size Entry size.
 * \param [in] e_bm Entry bucket mode.
 * \param [out] e_loc Pointer to rm_hash_ent_loc_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_move(int unit,
                        rm_hash_req_context_t *req_ctxt,
                        rm_hash_pt_info_t *pt_info,
                        int depth,
                        uint8_t num_slb_info,
                        rm_hash_ent_slb_info_t *slb_info,
                        uint8_t ent_size,
                        rm_hash_bm_t e_bm,
                        rm_hash_ent_loc_t *e_loc,
                        uint8_t *rbank);

/*!
 * \brief Initialize entry move related resource.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_move_info_init(int unit);

/*!
 * \brief Cleanup entry move related resource.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_move_info_cleanup(int unit);

#endif /* BCMPTM_RM_HASH_REORDERING_H */

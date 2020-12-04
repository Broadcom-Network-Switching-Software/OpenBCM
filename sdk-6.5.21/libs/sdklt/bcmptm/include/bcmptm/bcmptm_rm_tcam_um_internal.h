/*! \file bcmptm_rm_tcam_um_internal.h
 *
 * APIs, defines for LTM to PTM interfacing for PTRM-Tcam-Unmapped LTs.
 * Example for such LTs are FP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_TCAM_UM_INTERNAL_H
#define BCMPTM_RM_TCAM_UM_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_internal.h>

/*******************************************************************************
 * Defines
 */

/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Global variables
 */

/*******************************************************************************
 * Function declarations (prototypes)
 */

/*!
 * \brief Component config phase of init sequence
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 * \param [in] phase 1,2,3
 * \n 1 => 1st scan of all LTIDs. Should see calls to bcmptm_rm_tcam_um_lt_reg
 * \n 2 => 2nd scan of all LTIDs. Should see calls to bcmptm_rm_tcam_um_lt_reg
 * \n 3 => No scan of LTIDs. No calls to bcmptm_rm_tcam_um_lt_reg
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_um_comp_config(int unit, bool warm, bcmptm_sub_phase_t phase);

/*!
 * \brief Registration of LTID with rm_tcam_um
 *
 * \param [in] unit Logical device id
 * \param [in] ltid LTID that needs to be managed by rsrc_mgr
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 * \param [in] phase 1,2,3
 * \param [in] lt_state_offset Offset into LT state mem (in rsrc_mgr)
 * \n Note: lt_state_offset will be stored by caller only in phase 2
 * (comp_config)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_um_lt_reg(int unit, bcmltd_sid_t ltid, bool warm,
                         bcmptm_sub_phase_t phase,
                         uint32_t *lt_state_offset);

/*!
 * \brief Commit, Abort, Idle_Check
 *
 * \param [in] unit Logical device id
 * \param [in] flags usage TBD
 * \param [in] cseq_id Transaction id
 * \param [in] trans_cmd Commit, Abort, Idle_Check (See bcmptm_trans_cmd_t)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_um_trans_cmd(int unit, uint64_t flags, uint32_t cseq_id,
                            bcmptm_trans_cmd_t trans_cmd);

#endif /* BCMPTM_RM_TCAM_UM_INTERNAL_H */

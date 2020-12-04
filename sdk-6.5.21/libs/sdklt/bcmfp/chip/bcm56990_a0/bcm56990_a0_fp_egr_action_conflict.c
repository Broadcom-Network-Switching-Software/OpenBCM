/*! \file bcm56990_a0_fp_egr_action_conflict.c
 *
 * EFP action conflict check function for Tomahawk-4(56990_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56990_a0_fp.h>
#include <bcmltd/chip/bcmltd_id.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56990_a0_egress_action_cfg_conflict_check(int unit,
                                                   bcmfp_action_t action1,
                                                   bcmfp_action_t action)
{
    uint32_t fid = 0;

    switch (action1) {
        case FP_EGR_POLICY_TEMPLATEt_ACTION_DROPf:
        case FP_EGR_POLICY_TEMPLATEt_ACTION_DROP_CANCELf:
            fid = FP_EGR_POLICY_TEMPLATEt_ACTION_DROPf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_EGR_POLICY_TEMPLATEt_ACTION_DROP_CANCELf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        default:
            break;
    }
    return SHR_E_NONE;
}
